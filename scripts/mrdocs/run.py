#!/usr/bin/env python3
"""Regenerate the API reference under docs/reference/api_reference/mrdocs/.

    python scripts/mrdocs/run.py

Runs the same way from cmd.exe, PowerShell, Git Bash and a Unix shell, needing
nothing beyond the Python that MkDocs already requires. Doing this in a shell
script meant a POSIX shell on Windows, plus `unzip` (which Git for Windows does
not ship), `cygpath` and a PowerShell fallback for checksums; `zipfile`,
`tarfile`, `hashlib` and `urllib` are all standard library.

The pinned MrDocs build is downloaded and cached on first use. Set MRDOCS to
use a different binary, or MRDOCS_CACHE_DIR to move the cache.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import platform
import shutil
import subprocess
import sys
import tarfile
import tempfile
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

VERSION = "2026.9.4"

# platform -> (archive suffix, sha256)
RELEASES = {
    "Linux": (
        "tar.xz",
        "1e0a455d3e68cb0bacc0884b10fbdf9865b260ae76ce46003c5542e2489af769",
    ),
    "Darwin": (
        "tar.xz",
        "a1cfa12f2a980772f91610091978b2bb40302cbbc0740500026ea870c6893e2e",
    ),
    "win64": (
        "zip",
        "4a570d433d449b230d9672eab7bb59f062bb04cbb71a2a3bf1d81ab6a7aeb688",
    ),
}

BASE_URL = "https://github.com/cppalliance/mrdocs/releases/download"
SOURCE_URL = "https://github.com/mpusz/mp-units/blob"

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
OUTPUT = ROOT / "docs" / "reference" / "api_reference" / "mrdocs"


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
    twice in the sidebar. Moving it changes no URL, because MkDocs serves
    `a/b.md` and `a/b/index.md` from the same `a/b/`.

    Namespaces only, which the tagfile identifies. Doing the same to a class
    page would make Material absorb it into whichever section the MkDocs hook
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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("args", nargs="*", help="extra arguments forwarded to mrdocs")
    options = parser.parse_args()

    if _git("status", "--porcelain", "--", "src"):
        print(
            "warning: src/ has uncommitted changes; source links will point at\n"
            f"         {source_ref()} and their line numbers will not match.",
            file=sys.stderr,
        )

    binary = mrdocs_binary()

    if OUTPUT.exists():
        shutil.rmtree(OUTPUT)

    result = subprocess.run(
        [
            str(binary),
            "--config=./mrdocs.yml",
            f"--base-url={SOURCE_URL}/{source_ref()}/src/",
            *options.args,
        ],
        cwd=HERE,
    )
    if result.returncode:
        return result.returncode

    print(f"nested {nest_namespace_pages(OUTPUT)} namespace pages")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
