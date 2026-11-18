#!/usr/bin/env python3
"""Regenerate the API reference under docs/reference/api_reference/mrdocs/.

    python scripts/mrdocs/run.py

Runs the same way from cmd.exe, PowerShell, Git Bash and a Unix shell, needing
nothing beyond the Python that ProperDocs already requires. Python rather than a
shell script because Windows would otherwise need a POSIX shell, `unzip` (which
Git for Windows does not ship), `cygpath`, and a PowerShell fallback for
checksums - where `zipfile`, `tarfile`, `hashlib` and `urllib` are all standard
library.

The pinned MrDocs build is downloaded and cached on first use. Set MRDOCS to
use a different binary, or MRDOCS_CACHE_DIR to move the cache.
"""

from __future__ import annotations

import argparse
import contextlib
import hashlib
import html
import json
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time
import urllib.request
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path

# --------------------------------------------------------------------------
# The pinned release
#
# The output is version-sensitive - page names, section headings and the
# tagfile all move between releases - so an unpinned `latest` would change the
# published site without anything in this repo changing.
#
# To upgrade: bump VERSION, replace the checksums from
# https://github.com/cppalliance/mrdocs/releases, regenerate, and review the
# diff before committing.
# --------------------------------------------------------------------------

VERSION = "2026.9.29"

# platform -> (archive suffix, sha256)
RELEASES = {
    "Linux": (
        "tar.xz",
        "9d96e42f303046fb0b221aa9071eabccc40b9683e0b7af2b02a929d15476ca6a",
    ),
    "Darwin": (
        "tar.xz",
        "b393adf3df2180fc00edd50d6a976a79de3a1b9fbab0e93844495e4faa1c8435",
    ),
    "win64": (
        "zip",
        "c6ef68278816f3793152ff956d1318c74e80d5f98ae1d926c4e48835097ca4c4",
    ),
}

BASE_URL = "https://github.com/cppalliance/mrdocs/releases/download"
SOURCE_URL = "https://github.com/mpusz/mp-units/blob"

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
OUTPUT = ROOT / "docs" / "reference" / "api_reference" / "mrdocs"
CONFIG = HERE / "mrdocs.yml"
# Inside the output tree on purpose: deleting the output must invalidate
# the cache, and `main()` wipes the tree before regenerating.
CACHE = OUTPUT / ".cache.json"
STYLE_FILE = ROOT / ".clang-format"


# --------------------------------------------------------------------------
# Getting MrDocs
# --------------------------------------------------------------------------


def _platform() -> str:
    system = platform.system()
    if system == "Windows":
        return "win64"
    if system in RELEASES:
        return system
    raise SystemExit(
        f"error: no pinned MrDocs build for {system}.\n"
        f"       Install MrDocs {VERSION} and set MRDOCS to its path."
    )


def _cache_root() -> Path:
    override = os.environ.get("MRDOCS_CACHE_DIR")
    if override:
        return Path(override)
    if platform.system() == "Windows":
        base = os.environ.get("LOCALAPPDATA") or Path.home() / "AppData" / "Local"
    elif platform.system() == "Darwin":
        base = Path.home() / "Library" / "Caches"
    else:
        base = os.environ.get("XDG_CACHE_HOME") or Path.home() / ".cache"
    return Path(base) / "mp-units" / "mrdocs"


def _installed_version(binary: str) -> str:
    try:
        out = subprocess.run(
            [binary, "--version"], capture_output=True, text=True, timeout=30
        ).stdout
    except (OSError, subprocess.SubprocessError):
        return ""
    for line in out.splitlines():
        if line.startswith("Release:"):
            return line.split(":", 1)[1].strip()
    return ""


def _extract(archive: Path, into: Path) -> None:
    """Unpack, dropping the single top-level directory.

    It is named after MrDocs' internal version (`MrDocs-0.8.0-win64`) rather
    than the release tag, so it cannot be predicted from VERSION.
    """
    staging = into.parent / "staging"
    staging.mkdir(parents=True)
    if archive.suffix == ".zip":
        with zipfile.ZipFile(archive) as zf:
            zf.extractall(staging)
    else:
        with tarfile.open(archive) as tf:
            tf.extractall(staging, filter="data")

    roots = [entry for entry in staging.iterdir()]
    top = roots[0] if len(roots) == 1 and roots[0].is_dir() else staging
    shutil.move(str(top), str(into))


def mrdocs_binary() -> Path:
    """The pinned MrDocs binary, downloading it on first use."""
    override = os.environ.get("MRDOCS")
    if override:
        return Path(override)

    target = _platform()
    suffix, expected = RELEASES[target]
    exe = ".exe" if target == "win64" else ""

    prefix = _cache_root() / VERSION
    binary = prefix / "bin" / f"mrdocs{exe}"
    if binary.is_file():
        return binary

    # An already-installed mrdocs is fine, but only at the pinned version:
    # silently generating with whatever is installed is how a published site
    # stops being reproducible.
    found = shutil.which(f"mrdocs{exe}")
    if found and _installed_version(found) == VERSION:
        return Path(found)

    archive_name = f"MrDocs-{VERSION}-{target}.{suffix}"
    url = f"{BASE_URL}/{VERSION}/{archive_name}"
    print(f"Downloading MrDocs {VERSION} for {target}...", file=sys.stderr)
    print(f"  {url}", file=sys.stderr)

    with tempfile.TemporaryDirectory() as tmp:
        archive = Path(tmp) / archive_name
        with urllib.request.urlopen(url) as response, archive.open("wb") as out:
            shutil.copyfileobj(response, out)

        actual = hashlib.sha256(archive.read_bytes()).hexdigest()
        if actual != expected:
            raise SystemExit(
                f"error: checksum mismatch for {archive_name}\n"
                f"       expected {expected}\n"
                f"       actual   {actual}"
            )

        unpacked = Path(tmp) / "mrdocs"
        _extract(archive, unpacked)
        prefix.parent.mkdir(parents=True, exist_ok=True)
        # Move into place as one step, so a cancelled download leaves no
        # half-populated cache behind.
        if prefix.exists():
            shutil.rmtree(prefix)
        shutil.move(str(unpacked), str(prefix))

    print(f"Installed to {prefix}", file=sys.stderr)
    return binary


# --------------------------------------------------------------------------
# Generating
# --------------------------------------------------------------------------


def _git(*args: str) -> str:
    try:
        return subprocess.run(
            ["git", "-C", str(ROOT), *args], capture_output=True, text=True
        ).stdout.strip()
    except OSError:
        return ""


def source_ref() -> str:
    """The ref the source links should point at.

    The line numbers come from the working tree, so pinning to a moving ref
    like `master` makes every link drift as soon as a file changes. Prefer an
    exact tag, so released docs link to released code.
    """
    override = os.environ.get("MRDOCS_SOURCE_REF")
    if override:
        return override
    return _git("describe", "--tags", "--exact-match") or _git("rev-parse", "HEAD")


def nest_namespace_pages(root: Path) -> int:
    """Move each namespace's page inside its own directory.

    MrDocs writes a namespace's page *beside* its directory (`mp_units.md` next
    to `mp_units/`). Material's `navigation.indexes` only folds a page into its
    section when the page is `index.md`, so left as-is every namespace appears
    twice in the sidebar. Moving it changes no URL, because ProperDocs serves
    `a/b.md` and `a/b/index.md` from the same `a/b/`.

    Namespaces only, which the tagfile identifies. Doing the same to a class
    page would make Material absorb it into whichever section the ProperDocs hook
    nests it under, hiding the class's own members.
    """
    tagfile = root / "reference.tag.xml"
    if not tagfile.is_file():
        raise SystemExit(f"error: no tagfile at {tagfile}")

    moved = 0
    for compound in ET.parse(tagfile).getroot().iter("compound"):
        if compound.get("kind") != "namespace":
            continue
        filename = compound.findtext("filename")
        if not filename:
            continue
        page = root / filename
        directory = page.with_suffix("")
        if page.is_file() and directory.is_dir():
            page.rename(directory / "index.md")
            moved += 1
    return moved


# --------------------------------------------------------------------------
# Deciding whether to regenerate
#
# ProperDocs calls this on every build, so the common case has to be free. The
# hash covers everything that can change a rendered page: the headers, the
# MrDocs configuration, the Handlebars templates, and the pinned release.
# --------------------------------------------------------------------------


def _input_hash() -> str:
    import yaml  # a ProperDocs dependency, so always available where this matters

    hasher = hashlib.sha256()
    hasher.update(VERSION.encode())
    hasher.update(CONFIG.read_bytes())

    # The synopses are laid out by clang-format, so the style file and the
    # formatter's version change the output just as a header does.
    if STYLE_FILE.is_file():
        hasher.update(STYLE_FILE.read_bytes())
    binary = clang_format_binary()
    if binary:
        hasher.update(_clang_format_version(binary).encode())

    config = yaml.safe_load(CONFIG.read_text())
    patterns = config.get("file-patterns") or ["*.h"]
    scanned: set[Path] = set()
    for entry in config.get("input") or []:
        root = (HERE / entry).resolve()
        for pattern in patterns:
            scanned.update(path for path in root.rglob(pattern) if path.is_file())

    # `exclude` is deliberately not applied. A header under bits/ is not
    # documented, but it still spells the constraints that appear in a
    # documented signature, so editing one has to invalidate the cache.
    addons = (path for path in (HERE / "addons").rglob("*") if path.is_file())

    for path in sorted(scanned) + sorted(addons):
        hasher.update(str(path.relative_to(ROOT)).encode())
        hasher.update(path.read_bytes())

    return hasher.hexdigest()


def _cached() -> dict:
    try:
        return json.loads(CACHE.read_text())
    except (json.JSONDecodeError, OSError, ValueError):
        return {}


NO_CLANG_FORMAT = (
    "WARNING -  mrdocs: clang-format was not found, so the synopses keep the\n"
    "           layout MrDocs produced rather than this project's. Install it\n"
    "           with `pip install -r requirements.txt`."
)


# How long a lock may go without naming its owner before it is treated as
# abandoned. The holder writes the pid immediately after `mkdir`, so a gap this
# long is a process that died in between rather than one still starting up.
_UNCLAIMED_LOCK = 10.0


def _lock_owner(owner: Path) -> int | None:
    """The pid holding the lock, or `None` if the holder is gone."""
    try:
        pid = int(owner.read_text().strip())
    except (OSError, ValueError):
        # Nothing has claimed it. For a moment that is the window between `mkdir` and
        # the write, and mistaking it for a corpse would break the exclusion.
        try:
            age = time.time() - owner.parent.stat().st_mtime
        except OSError:
            return None
        return None if age > _UNCLAIMED_LOCK else 0
    if os.name == "nt":
        # There `os.kill(pid, 0)` sends a Ctrl-C (`CTRL_C_EVENT` is 0) rather than
        # probing, so a holder is taken to be alive and the timeout applies.
        return pid
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return None
    except PermissionError:
        return pid  # someone else's process, so it is alive
    return pid


@contextlib.contextmanager
def _exclusive(timeout: float = 900.0):
    """Serialise generation across processes.

    ProperDocs regenerates from its own `on_config`, so a `properdocs serve` rebuild
    and a hand-run of this script can otherwise wipe and rewrite the same tree
    at the same time, which fails with a bare `Directory not empty`.

    `mkdir` rather than a lock file, because it is atomic on Windows too. The
    lock lives beside the download cache, outside the docs tree, so ProperDocs
    neither serves it nor treats creating it as a change to rebuild for.
    """
    lock = _cache_root() / "generate.lock"
    owner = lock / "pid"
    lock.parent.mkdir(parents=True, exist_ok=True)
    deadline = time.monotonic() + timeout
    announced = False
    while True:
        try:
            lock.mkdir()
            break
        except FileExistsError:
            holder = _lock_owner(owner)
            if holder is None:
                # The owner died without unwinding: it was killed, or its terminal
                # closed. Waiting out the full timeout for a lock nobody holds
                # looks like a hang.
                print(f"warning: clearing a stale lock at {lock}", file=sys.stderr)
                # Separately, because an owner that died before writing its pid
                # left no file to unlink, and the directory still has to go.
                with contextlib.suppress(OSError):
                    owner.unlink()
                with contextlib.suppress(OSError):
                    lock.rmdir()
                continue
            if not announced:
                # Without this the script prints nothing at all while it waits,
                # which is indistinguishable from having hung.
                print(
                    "waiting for another generation to finish"
                    + (f" (pid {holder})" if holder else ""),
                    file=sys.stderr,
                )
                announced = True
            if time.monotonic() >= deadline:
                # Better to generate and risk the clash than to wait forever.
                print(f"warning: ignoring the lock at {lock}", file=sys.stderr)
                break
            time.sleep(0.5)
    with contextlib.suppress(OSError):
        owner.write_text(str(os.getpid()))
    try:
        yield
    finally:
        with contextlib.suppress(OSError):
            owner.unlink()
        with contextlib.suppress(OSError):
            lock.rmdir()


def generate_if_needed(force: bool = False, extra: list[str] | None = None) -> int:
    """Regenerate only when an input changed. Returns a process exit code."""
    with _exclusive():
        return _generate_if_needed(force, extra)


def _generate_if_needed(force: bool, extra: list[str] | None) -> int:
    # Re-read the cache inside the lock: while this process waited, the one
    # holding it may have generated exactly what this one was going to.
    #
    # Hash before generating, so that an edit made while MrDocs is running is
    # not silently recorded as already processed.
    current = _input_hash()
    cached = _cached()
    if not force and current == cached.get("input_hash"):
        print("API reference is up to date (sources unchanged)")
        # Said again on every build, not only the one that generated. A
        # warning printed once, among everything else a docs build prints,
        # is a warning nobody sees, and the pages stay unformatted until
        # something unrelated happens to invalidate the cache.
        if not cached.get("formatted", True):
            print(NO_CLANG_FORMAT, file=sys.stderr)
        return 0

    formatted = clang_format_binary() is not None
    result = _generate(extra or [])
    if result == 0:
        CACHE.write_text(
            json.dumps({"input_hash": current, "formatted": formatted}, indent=2) + "\n"
        )
    return result


def _sync(staging: Path, destination: Path) -> tuple[int, int, int]:
    """Copy across only what changed, leaving unchanged pages untouched.

    ProperDocs rebuilds a page when its file changes, and MrDocs has no incremental
    mode, so regenerating in place made every one of the thousands of pages look
    new on every run and the whole site was rebuilt for a one-header edit. The
    generated output is mostly identical between runs, so comparing content and
    writing only the difference keeps the rest of the pipeline cheap.
    """
    written = removed = kept = 0
    destination.mkdir(parents=True, exist_ok=True)
    fresh = {path.relative_to(staging) for path in staging.rglob("*") if path.is_file()}
    stale = {
        path.relative_to(destination)
        for path in destination.rglob("*")
        if path.is_file()
    }

    for relative in sorted(stale - fresh):
        # written by the caller once generation has succeeded, so it is never in staging
        if destination / relative == CACHE:
            continue
        (destination / relative).unlink()
        removed += 1

    for relative in sorted(fresh):
        source, target = staging / relative, destination / relative
        if target.is_file() and target.read_bytes() == source.read_bytes():
            kept += 1
            continue
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
        written += 1

    # deepest first, so a directory emptied by its children's removal also goes
    for directory in sorted(
        (path for path in destination.rglob("*") if path.is_dir()),
        key=lambda path: len(path.parts),
        reverse=True,
    ):
        if not any(directory.iterdir()):
            directory.rmdir()

    return written, removed, kept


def _generate(extra: list[str]) -> int:
    if _git("status", "--porcelain", "--", "src"):
        print(
            "warning: src/ has uncommitted changes; source links will point at\n"
            f"         {source_ref()} and their line numbers will not match.",
            file=sys.stderr,
        )

    binary = mrdocs_binary()

    # An explicit --output means the caller wants the pages somewhere of their
    # own; generate straight there and leave the reference alone.
    redirected = any(argument.startswith("--output") for argument in extra)

    with tempfile.TemporaryDirectory(prefix="mrdocs-") as staging_dir:
        staging = OUTPUT if redirected else Path(staging_dir) / "out"
        result = subprocess.run(
            [
                str(binary),
                "--config=./mrdocs.yml",
                f"--base-url={SOURCE_URL}/{source_ref()}/src/",
                *([] if redirected else [f"--output={staging}"]),
                *extra,
            ],
            cwd=HERE,
        )
        if result.returncode:
            return result.returncode

        print(f"nested {nest_namespace_pages(staging)} namespace pages")

        formatter = clang_format_binary()
        if formatter is None:
            print(NO_CLANG_FORMAT, file=sys.stderr)
        else:
            done, kept = format_synopses(staging, formatter)
            print(
                f"formatted {done} synopses"
                + (f", kept {kept} as generated" if kept else "")
            )

        if not redirected:
            written, removed, unchanged = _sync(staging, OUTPUT)
            print(
                f"synced {written} new or changed pages, removed {removed}, "
                f"left {unchanged} untouched"
            )
    return 0


# --------------------------------------------------------------------------
# Laying out the synopses
#
# MrDocs' templates decide where a declaration breaks, and no set of rules
# expressed in Handlebars will agree with the one the library is written to.
# clang-format already encodes that, so the synopses are reformatted with the
# project's own .clang-format, pinned to the version pre-commit uses.
# --------------------------------------------------------------------------

SYNOPSIS = re.compile(
    r'(<pre class="mrdocs-synopsis"><code[^>]*>)(.*?)(</code></pre>)', re.DOTALL
)
# Separates declarations within one clang-format invocation. Formatting them
# one at a time costs about 100s over this corpus, against under a second
# batched, and the results are identical.
MARKER = "//@@MRDOCS@@"


def clang_format_binary() -> str | None:
    override = os.environ.get("CLANG_FORMAT")
    if override:
        return override
    return shutil.which("clang-format")


def _clang_format_version(binary: str) -> str:
    try:
        return subprocess.run(
            [binary, "--version"], capture_output=True, text=True, timeout=30
        ).stdout.strip()
    except (OSError, subprocess.SubprocessError):
        return ""


def _split_markup(fragment: str) -> tuple[list[tuple[str, str]], str]:
    """Separate a synopsis into a tag/character stream and its plain text."""
    stream: list[tuple[str, str]] = []
    for part in re.split(r"(<[^>]+>)", fragment):
        if part.startswith("<"):
            stream.append(("tag", part))
        else:
            stream.extend(("char", char) for char in html.unescape(part))
    return stream, "".join(char for kind, char in stream if kind == "char")


def _reapply_markup(stream: list[tuple[str, str]], formatted: str) -> str:
    """Put the links back, following clang-format's whitespace.

    Safe only because clang-format moves whitespace and never rewrites a
    token, so the two character streams agree once whitespace is removed.
    The caller checks that before calling this.
    """
    out: list[str] = []
    index = 0
    for char in formatted:
        if char.isspace():
            out.append(char)
            continue
        while index < len(stream):
            kind, value = stream[index]
            index += 1
            if kind == "tag":
                out.append(value)
            elif not value.isspace():
                out.append(html.escape(value, quote=False))
                break
    out.extend(value for kind, value in stream[index:] if kind == "tag")
    return "".join(out)


def _derived_style(binary: str, directory: Path) -> Path | None:
    """The project's style with the token-rewriting options turned off.

    Built from `--dump-config` rather than by appending to .clang-format,
    because the options to override are already set there and clang-format
    rejects a duplicated key.

    Those options are right for a source file and wrong here. MrDocs has
    already settled what a declaration says; this pass only decides where it
    breaks. `QualifierAlignment: Left` would rewrite `Q const&` as `const Q&`,
    and `BreakStringLiterals` would show two literals where the header has
    one. Both also move the type names carrying the cross-reference links,
    which are reattached by position afterwards.
    """
    import yaml  # a ProperDocs dependency, so always available where this matters

    dumped = subprocess.run(
        [
            binary,
            f"--style=file:{STYLE_FILE}",
            "--assume-filename=synopsis.cpp",
            "--dump-config",
        ],
        capture_output=True,
        text=True,
    )
    if dumped.returncode:
        return None

    config = yaml.safe_load(dumped.stdout)
    config["BreakStringLiterals"] = False
    config["QualifierAlignment"] = "Leave"

    style = directory / ".clang-format"
    style.write_text(yaml.safe_dump(config, sort_keys=False), encoding="utf-8")
    return style


def _run_clang_format(binary: str, style: Path, source: str) -> str | None:
    """Format `source`. None if clang-format refused.

    `--assume-filename` names a `.cpp`, not a `.h`: for a header clang-format
    guesses between C, C++ and Objective-C, and on an input the size of this
    corpus it guesses Objective-C, at which point the style's `Language: Cpp`
    section stops applying and every declaration is laid out to the built-in
    default instead.
    """
    result = subprocess.run(
        [binary, f"--style=file:{style}", "--assume-filename=synopsis.cpp"],
        input=source,
        capture_output=True,
        text=True,
    )
    if result.returncode or "Error" in result.stderr:
        return None
    return result.stdout


def format_synopses(root: Path, binary: str) -> tuple[int, int]:
    """Reformat every synopsis in the generated pages. Returns (done, kept)."""
    pages = sorted(root.rglob("*.md"))
    jobs: list[tuple[Path, int, list[tuple[str, str]], str, bool]] = []
    bodies: dict[Path, str] = {}

    for page in pages:
        text = page.read_text(encoding="utf-8")
        matches = list(SYNOPSIS.finditer(text))
        if not matches:
            continue
        bodies[page] = text
        for number, match in enumerate(matches):
            stream, plain = _split_markup(match.group(2))
            plain = plain.strip()
            if not plain:
                continue
            # A class synopsis is a fragment with no terminator. Left as-is it
            # is formatted as a labelled statement, giving `struct x final:`
            # rather than `struct x final :`.
            terminate = not plain.endswith((";", "}"))
            jobs.append((page, number, stream, plain, terminate))

    if not jobs:
        return 0, 0

    blob = f"\n{MARKER}\n".join(
        plain + (";" if terminate else "") for _, _, _, plain, terminate in jobs
    )
    with tempfile.TemporaryDirectory() as stash:
        style = _derived_style(binary, Path(stash))
        if style is None:
            print(
                "warning: could not read the clang-format style, leaving synopses as generated",
                file=sys.stderr,
            )
            return 0, len(jobs)

        result = _run_clang_format(binary, style, blob)
        # A style clang-format cannot parse is not a reason to fall back to its
        # own default and format the whole reference to the wrong rules.
        if result is None:
            print(
                "warning: clang-format failed, leaving synopses as generated",
                file=sys.stderr,
            )
            return 0, len(jobs)

        chunks = result.split(MARKER)
        if len(chunks) != len(jobs):
            print(
                "warning: clang-format output did not line up, leaving synopses as generated",
                file=sys.stderr,
            )
            return 0, len(jobs)

        done = kept = 0
        replacements: dict[Path, dict[int, str]] = {}
        for (page, number, stream, plain, terminate), chunk in zip(jobs, chunks):
            formatted = chunk.strip()
            if terminate and formatted.endswith(";"):
                formatted = formatted[:-1].rstrip()
            # If anything but whitespace moved, the rebuild would misplace a link.
            if "".join(formatted.split()) != "".join(plain.split()):
                # Usually the batch is to blame: a declaration inherits the
                # indentation left by the one before it, and a line that no longer
                # fits gets its string literal split in two. Alone it is fine.
                retry = _run_clang_format(
                    binary, style, plain + (";" if terminate else "")
                )
                formatted = (retry or "").strip()
                if terminate and formatted.endswith(";"):
                    formatted = formatted[:-1].rstrip()
                if not formatted or "".join(formatted.split()) != "".join(
                    plain.split()
                ):
                    kept += 1
                    continue
            replacements.setdefault(page, {})[number] = _reapply_markup(
                stream, formatted
            )
            done += 1

    for page, byindex in replacements.items():
        counter = iter(range(10**9))

        def substitute(match, byindex=byindex, counter=counter):
            number = next(counter)
            body = byindex.get(number)
            return match.group(1) + body + match.group(3) if body else match.group(0)

        page.write_text(SYNOPSIS.sub(substitute, bodies[page]), encoding="utf-8")

    return done, kept


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--force",
        action="store_true",
        help="regenerate even when no input has changed",
    )
    parser.add_argument("args", nargs="*", help="extra arguments forwarded to mrdocs")
    options = parser.parse_args()
    return generate_if_needed(force=options.force, extra=options.args)


if __name__ == "__main__":
    raise SystemExit(main())
