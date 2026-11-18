#!/usr/bin/env python3
"""Compile the C++ code samples embedded in the documentation.

A sample is wrapped in a named template (see `scripts/snippet_templates/`) and
compiled. An unmarked sample tries `si`, then `isq`. A sample selects a template
with a `// snippet-template: <name>` line, and opts out entirely with `none`.
A sample from a doc comment also gets the header it documents.

Little is inferred about a sample: where the body belongs is decided by trying
each placement and keeping whichever the compiler accepts. The report lists the
samples that no placement accepts, which is the worklist for annotating them.
The exit status is non-zero while that list is not empty.
"""

import argparse
import concurrent.futures
import os
import re
import subprocess
import sys
import tempfile
import textwrap
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TEMPLATES = Path(__file__).resolve().parent / "snippet_templates"
# generated trees: their samples come from sources that are compiled already
SKIP_DIRS = ("api_reference/mrdocs", "systems_reference")

# A fence may carry attributes after the language, as in ```cpp title="x.cpp"
FENCE = re.compile(r"^([ \t]*)```+[ \t]*cpp\b[^\n]*\n(.*?)^\1```+", re.S | re.M)
MRDOCS_CODE = re.compile(r"@code\{?\.?c?p?p?\}?\n(.*?)@endcode", re.S)
MARKER = re.compile(r"^\s*//\s*snippet-template:\s*([\w-]+)\s*$", re.M)
CE_EMBED = re.compile(r"^\s*//\s*ce-embed\b.*\n", re.M)
# Every template already provides the library headers these import. Compiling a
# sample through modules would need a module build of the library for each run.
IMPORT = re.compile(r"^\s*import\s+(?:std|mp_units(?:\.\w+)*)\s*;[ \t]*\n?", re.M)
INCLUDE_DIRS = [
    ROOT / f"src/{layer}/include" for layer in ("core", "systems", "utility")
]


class Sample:
    def __init__(self, origin, line, body, template, header=None):
        self.origin, self.line, self.body, self.template = origin, line, body, template
        self.header = header  # the header a doc comment documents, if any
        self.placement = None
        self.error = ""

    @property
    def name(self):
        return f"{self.origin}:{self.line}"


def documented_header(path):
    """The `#include` spelling of a header under one of the include directories."""
    for base in INCLUDE_DIRS:
        if path.is_relative_to(base):
            return path.relative_to(base).as_posix()
    return None


def collect():
    out = []
    for base, pat, strip_stars in (
        (ROOT / "docs", FENCE, False),
        (ROOT / "src", MRDOCS_CODE, True),
    ):
        for path in sorted(base.rglob("*.md" if not strip_stars else "*.h")):
            rel = str(path.relative_to(ROOT))
            if any(d in rel for d in SKIP_DIRS):
                continue
            header = documented_header(path) if strip_stars else None
            text = path.read_text(encoding="utf-8")
            for m in pat.finditer(text):
                body = m.group(2) if not strip_stars else m.group(1)
                if strip_stars:  # drop the leading " * " of a doc comment
                    body = "\n".join(
                        re.sub(r"^\s*\*\s?", "", ln) for ln in body.splitlines()
                    )
                body = textwrap.dedent(body)
                line = text[: m.start()].count("\n") + 1
                if "--8<--" in body:  # pulled from a real file, compiled there
                    continue
                tpl = None
                mk = MARKER.search(body)
                if mk:
                    tpl = mk.group(1)
                    body = MARKER.sub("", body)
                body = CE_EMBED.sub("", body)
                if tpl == "none" or not body.strip():
                    continue
                out.append(Sample(rel, line, body, tpl, header))
    return out


def placements_for(body):
    """Where the body may go, most specific first.

    `raw` compiles the sample as it stands, because it brings its own includes.
    `whole` puts it after the template preamble, because it brings its own `main`.
    Otherwise it is tried as the body of `main`, then at namespace scope.
    """
    if re.search(r"^\s*#\s*include", body, re.M):
        return ["raw"]
    if re.search(r"\bint\s+main\s*\(", body):
        return ["whole"]
    return ["main", "decl"]


def render(sample, placement, template_text):
    body = IMPORT.sub("", sample.body)
    if placement == "main":
        inner = "\n".join("  " + ln for ln in body.splitlines())
        body = "int main()\n{\n" + inner + "\n}\n"
    elif placement == "decl":
        body = body + "\nint main() {}\n"
    if sample.header:
        body = f"#include <{sample.header}>\n{body}"
    return template_text.replace("{{BODY}}", body)


# What the compiler says when a placement is wrong for the sample, rather than
# the sample being wrong. GCC stops at the first of these, so such an attempt
# looks like the best one by error count alone.
WRONG_PLACEMENT = re.compile(r"block scope|is not allowed here|cannot appear at")


def relevance(stderr):
    """Sort key over rejected attempts: the most informative diagnostic first."""
    return bool(WRONG_PLACEMENT.search(stderr)), stderr.count(": error:")


def build(args, work):
    inc = [f"-I{d}" for d in INCLUDE_DIRS]
    flags = [args.cxx, f"-std=c++{args.std}", *inc]
    templates = {
        p.stem: p.read_text(encoding="utf-8") for p in TEMPLATES.glob("*.template")
    }

    # one precompiled header per template, so the library is parsed once per
    # template rather than once per sample
    pch = {}
    for name, text in templates.items():
        head = text.split("{{BODY}}")[0]
        h = work / f"pre_{name}.h"
        h.write_text(head, encoding="utf-8")
        r = subprocess.run(
            [*flags, "-x", "c++-header", str(h), "-o", str(h) + ".gch"],
            capture_output=True,
            text=True,
        )
        if r.returncode != 0:
            sys.exit(f"template '{name}' does not compile:\n{r.stderr}")
        pch[name] = head

    samples = collect()
    if args.only:
        samples = [s for s in samples if s.origin.startswith(args.only)]
    if args.limit:
        samples = samples[: args.limit]
    print(f"collected {len(samples)} samples", flush=True)

    def compile_one(src):
        return subprocess.run(
            [*flags, f"-I{work}", "-fsyntax-only", str(src)],
            capture_output=True,
            text=True,
        )

    def run(idx_sample):
        idx, s = idx_sample
        attempts = []  # the diagnostic of every rejected attempt
        placements = placements_for(s.body)
        if placements == ["raw"]:  # no template involved, so one compile decides
            src = work / f"s{idx}_raw.cpp"
            src.write_text(s.body, encoding="utf-8")
            r = compile_one(src)
            if r.returncode == 0:
                s.placement = "raw"
            else:
                s.error = r.stderr
            return s
        # an unmarked sample tries the candidates in order, and the compiler picks
        candidates = [s.template] if s.template else ["si", "isq"]
        for tpl in candidates:
            text = templates.get(tpl)
            if text is None:
                s.error = f"unknown template '{tpl}'"
                return s
            for placement in placements:
                src = work / f"s{idx}_{tpl}_{placement}.cpp"
                body = render(s, placement, text)
                # swap the preamble for the PCH include so the header is reused
                body = body.replace(pch[tpl], f'#include "pre_{tpl}.h"\n')
                src.write_text(body, encoding="utf-8")
                r = compile_one(src)
                if r.returncode == 0:
                    s.placement, s.template = placement, tpl
                    return s
                attempts.append(r.stderr)
        # The first attempt is rarely the informative one: a declaration tried
        # as the body of `main` fails for being at block scope, which says
        # nothing about the sample. Of those that do not fail on placement, the
        # attempt with the fewest errors got the furthest, so its diagnostic is
        # the one worth reading.
        s.error = min(attempts, key=relevance)
        return s

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        results = list(ex.map(run, enumerate(samples)))
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cxx", default="g++")
    ap.add_argument("--std", default="23")
    ap.add_argument("--jobs", type=int, default=os.cpu_count())
    ap.add_argument(
        "--workdir", help="keep the generated sources here (default: a temporary one)"
    )
    ap.add_argument("--only", help="only samples whose path starts with this prefix")
    ap.add_argument("--limit", type=int, help="stop after N samples (smoke test)")
    ap.add_argument("--list-failures", action="store_true")
    ap.add_argument(
        "--report", help="write every failure, with its diagnostic, to this file"
    )
    args = ap.parse_args()

    if args.workdir:
        work = Path(args.workdir)
        work.mkdir(parents=True, exist_ok=True)
        results = build(args, work)
    else:
        with tempfile.TemporaryDirectory(prefix="mp-units-snippets-") as tmp:
            results = build(args, Path(tmp))
    ok = [r for r in results if r.placement]
    bad = [r for r in results if not r.placement]
    by_placement = {}
    for r in ok:
        key = f"{r.template}/{r.placement}" if r.placement != "raw" else "raw"
        by_placement[key] = by_placement.get(key, 0) + 1
    print(f"\ncompiled {len(ok)}/{len(results)}")
    for k, v in sorted(by_placement.items()):
        print(f"  {v:5d}  placement={k}")
    print(f"  {len(bad):5d}  FAILED")
    if args.report:
        with open(args.report, "w", encoding="utf-8") as fh:
            for r in bad:
                fh.write(
                    f"### {r.name}  [template={r.template}]\n{r.body}\n--- diagnostic ---\n{r.error}\n\n"
                )
        print(f"  failures written to {args.report}")
    if args.list_failures:
        for r in bad:
            first = next(
                (ln for ln in r.error.splitlines() if ": error:" in ln),
                r.error.splitlines()[0] if r.error else "?",
            )
            print(f"    {r.name}  [{r.template}]  {first[:150]}")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
