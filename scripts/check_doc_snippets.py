#!/usr/bin/env python3
"""Compile the C++ code samples embedded in the documentation.

A sample is wrapped in a named template (see `scripts/snippet_templates/`) and
compiled. An unmarked sample tries `si`, then `isq`. Marker lines steer it, and the
site build strips every one of them from the rendered page:

- `// snippet-template: <name>` selects a template. `none` opts the sample out,
  and has to say why on the same line, starting with a category:
  `// snippet-template: none (category: why)`.
- `// snippet-continues` compiles the sample after the previous one on the same
  page.
- `// snippet-id: <id>` names a sample, and `// snippet-after: <id>, ...` compiles
  a sample after the named ones, above or below it on the same page. `path#id`
  names one on another page or in a header, and `example/<file>.cpp` a whole
  example source, whose `main` the sample's statements go into. A `:decls` suffix
  takes only that sample's declarations, without its statements in `main`. A
  sample compiled after others inherits their template and standard.
- `// snippet-setup: <declaration>` is compiled as part of the sample and never shown,
  for what the page leaves to the reader (a function it only calls, a value it only
  reads). It may only declare, and anything else is reported.
- `// snippet-std: 20` compiles the sample as C++20 (or 26) rather than the
  default, for a sample written for that standard.

A line whose trailing comment starts with `❌` is checked to fail, and so is the
statement under a comment line that holds nothing but a `// ❌` comment. A
commented-out line is uncommented for its own check. The sample without those lines
must compile. Each of them,
added back on its own, must make the compiler report an error on that line, or a
`required from here` pointing at it. The first such error must not be about an
undeclared name, because a misspelled name would make any line fail. A compile
that reports a deprecation counts as a failure, because a sample that teaches a
deprecated spelling is out of date. A sample from a doc comment also gets the
header it documents.

Where the body belongs is decided by trying each placement and keeping whichever
the compiler accepts. Given files, only the samples in those files are checked
(with the samples they are compiled after), unless one of them is this script or a
template, which re-checks everything. The exit status is non-zero while any
checked sample fails.

The outcome of every compile is kept in ~/.cache with the files it read, and reused
while none of them changed. A second run over unchanged sources compiles again only a
sample whose `#include` isn't found.
"""

import argparse
import concurrent.futures
import hashlib
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import textwrap
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TEMPLATES = Path(__file__).resolve().parent / "snippet_templates"
# generated trees: their samples come from sources that are compiled already
SKIP_DIRS = ("api_reference/mrdocs", "systems_reference")

# A fence may carry attributes after the language, as in ```cpp title="x.cpp"
FENCE = re.compile(r"^([ \t]*)```+[ \t]*cpp\b[^\n]*\n(.*?)^\1```+", re.S | re.M)
MRDOCS_CODE = re.compile(r"@code\{?\.?c?p?p?\}?\n(.*?)@endcode", re.S)
# Each pattern matches one whole line, so blanking it keeps the line numbers.
MARKER = re.compile(
    r"^[ \t]*//[ \t]*snippet-template:[ \t]*([\w-]+)(?:[ \t]+(\S.*?))?[ \t]*$", re.M
)
# The reasons a sample may give for not compiling at all (CONTRIBUTING.md lists them).
OPT_OUT_REASON = re.compile(
    r"\(?(?:historical|not this library|third-party|pseudo-code|exercise skeleton):"
)
STD = re.compile(r"^[ \t]*//[ \t]*snippet-std:[ \t]*(\d+)[ \t]*$", re.M)
# A declaration the check needs and the page leaves to the reader: compiled, never shown
SETUP = re.compile(r"^([ \t]*)//[ \t]*snippet-setup:[ \t]*(\S.*?)[ \t]*$", re.M)
CONTINUES = re.compile(r"^[ \t]*//[ \t]*snippet-continues[ \t]*$", re.M)
ID = re.compile(r"^[ \t]*//[ \t]*snippet-id:[ \t]*([\w-]+)[ \t]*$", re.M)
AFTER = re.compile(r"^[ \t]*//[ \t]*snippet-after:[ \t]*(\S.*?)[ \t]*$", re.M)
CE_EMBED = re.compile(r"^[ \t]*//[ \t]*ce-embed\b.*$", re.M)
# Every template already provides the library headers these import. Compiling a
# sample through modules would need a module build of the library for each run.
IMPORT = re.compile(
    r"^[ \t]*(?:export[ \t]+)?import\s+(?:std|mp_units(?:\.\w+)*)\s*;[ \t]*$", re.M
)
# A `❌` at the start of a comment says the code it is about doesn't compile.
# a line of code whose trailing comment is marked, as in `auto q = a + b;  // ❌ different kinds`
EXPECT_ERROR = re.compile(r"^\s*[^/\s].*?//\s*❌")
# a commented-out line of code whose trailing comment is marked, as in
# `// auto q = a + b;  // ❌ different kinds`: the indentation, then the code
COMMENTED_OUT_ERROR = re.compile(r"^(\s*)//\s?((?!\s*//)[^\n]*?[;)}])\s*//\s*❌")
# a commented-out line of code, which a marked comment line above it can mark as rejected
COMMENTED_OUT_CODE = re.compile(r"^(\s*)//\s?((?!\s*//)\S.*)$")
# a comment line with no code that marks the statement under it, as in `// ❌ different kinds`
EXPECT_ERROR_BELOW = re.compile(r"^\s*//\s*❌")
# what GCC says about a deprecated entity, and what `MP_UNITS_DEPRECATED_HEADER` says
DEPRECATED = re.compile(r"warning: .*\bdeprecated\b", re.I)
# what GCC and Clang say when a name isn't declared at all, which is no reason for a line to be rejected
UNDECLARED = re.compile(
    r"was not declared|has not been declared|does not name a type|is not a member of"  # GCC
    r"|use of undeclared identifier|unknown type name|no member named .* in namespace"  # Clang
)
INCLUDE_DIRS = [
    ROOT / f"src/{layer}/include" for layer in ("core", "systems", "utility")
]
# the integrations, and the headers the examples include, which an embedded example needs
EXTRA_INCLUDE_DIRS = [
    ROOT / d
    for d in ("src/integrations/include", "example/include", "example/kalman_filter")
]
# `--8<-- "path"` or `--8<-- "path:first:last"`, the pymdownx.snippets embed of a file or lines of it
EMBED = re.compile(r'^\s*--8<--\s+"([^":]+)(?::(\d+)(?::(\d+))?)?"\s*$', re.M)
INCLUDE_LINE = re.compile(r"^\s*#\s*include\b")
USING_DIRECTIVE = re.compile(r"^\s*using\s+namespace\s+[\w:]+\s*;\s*(?://.*)?$")


def hoisted(lines):
    """The indices of the lines put at namespace scope ahead of the rest of the sample.

    These are the `#include` lines, and the using-directives the sample opens with,
    wherever the rest of it goes. They belong at the top of a file, where a later
    sample's declarations see them too.
    """
    out, leading = [], True
    for i, line in enumerate(lines):
        if INCLUDE_LINE.match(line):
            out.append(i)
        elif leading and USING_DIRECTIVE.match(line):
            out.append(i)
        elif is_code(line):
            leading = False
    return out


MAIN = re.compile(r"\bint\s+main\s*\(")
# A line that begins a declaration. Some of these are valid only at namespace scope, and the
# rest are more common there than in `main`.
DECLARATION = re.compile(
    r"^\s*(?:template\s*<|struct\s|class\s|union\s|namespace\s|enum\s|concept\s|inline\s|export\s"
    r"|static_assert\b|using\s+\w+\s*=|QUANTITY_SPEC\b|MP_UNITS_EXPORT|#\s*define"
    r"|(?:void|bool|int|double|float|auto)\s+\w+\s*\([^;]*\)\s*(?:->[^;{]*)?;)"
)


def is_code(line):
    return bool(line.strip()) and not line.lstrip().startswith("//")


def code_of(line):
    """The code of a line without its trailing comment (rough: a `//` in a string ends it too)."""
    return line.split("//")[0].rstrip()


def ends_statement(line):
    code = code_of(line)
    return not code or code.endswith((";", "{", "}")) or code.lstrip().startswith("#")


def rejected_groups(lines):
    """The lines that must not compile, each group checked on its own.

    A marked line that ends a statement spanning several lines takes the lines before it
    that the statement started on. A commented-out line (`// auto q = a + b;  // ❌`) is
    checked like a live one: it is uncommented for that check only.
    """

    def statement_start(i):
        """The first line of the statement that line `i` ends."""
        start = i
        while (
            start > 0
            and is_code(lines[start - 1])
            and not ends_statement(lines[start - 1])
        ):
            start -= 1
        return start

    def through_block(i):
        """The line, and when it opens a block, every line up to the one closing it."""
        group, depth = [], 0
        for j in range(i, len(lines)):
            group.append(j)
            code = lines[j].split("//")[0]
            depth += code.count("{") - code.count("}")
            if depth <= 0:
                break
        return group

    def commented_code(j):
        """Whether line `j` is a commented-out line of code."""
        m = COMMENTED_OUT_CODE.match(lines[j])
        # prose ends with a word or a full stop, code with punctuation or an operator
        return (
            bool(m)
            and code_of(m.group(2)).endswith(tuple(";{}()[],+-*/=<>&|"))
            and not EXPECT_ERROR_BELOW.match(lines[j])
        )

    groups = []
    for i, line in enumerate(lines):
        if COMMENTED_OUT_ERROR.match(line):
            # a commented-out statement may start on the commented-out lines above it
            start = i
            while (
                start > 0
                and commented_code(start - 1)
                and not ends_statement(
                    COMMENTED_OUT_CODE.match(lines[start - 1]).group(2)
                )
            ):
                start -= 1
            groups.append(list(range(start, i + 1)))
        elif EXPECT_ERROR.match(line):
            groups.append(list(range(statement_start(i), i)) + through_block(i))
        elif EXPECT_ERROR_BELOW.match(line):
            # the statement under the comment, live or commented out, up to its `;`
            group = []
            for j in range(i + 1, len(lines)):
                if not (is_code(lines[j]) or commented_code(j)):
                    break
                group.append(j)
                text = (
                    COMMENTED_OUT_CODE.match(lines[j]).group(2)
                    if commented_code(j)
                    else lines[j]
                )
                if code_of(text).endswith(";"):
                    break
            if group:
                groups.append(group)
    return groups


class Sample:
    def __init__(self, origin, line, body, template, header=None, std=None):
        self.origin, self.line, self.template, self.std = origin, line, template, std
        self.header = header  # the header a doc comment documents, if any
        self.id = None
        self.refs = []  # how the sample names the ones it is compiled after
        self.after = []  # the samples it is compiled after
        self.lines = body.splitlines()
        self.rejected = rejected_groups(self.lines)
        self.placement = None
        self.context = None  # what a sample compiled after this one gets
        self.error = ""
        self.error_line = line
        self.embed = (
            False  # an embedded file, compiled only as another sample's context
        )
        self.decls_only = set()  # the samples it takes only the declarations of
        self.setup = set()  # the lines that are `snippet-setup` declarations, by index

    @property
    def name(self):
        return f"{self.origin}:{self.line}"

    def body(self, keep=()):
        """The body without the rejected lines, except those in `keep` (blanked, so the line count holds).

        A kept line that is commented out is uncommented. A dropped live `return` statement
        becomes `__builtin_unreachable();`, so that a function whose only `return` is
        rejected still has a body that compiles.
        """
        drop = {i for group in self.rejected for i in group} - set(keep)

        def line(i, text):
            if i in drop:
                if m := re.match(r"(\s*)return\b.*;", text):
                    return m.group(1) + "__builtin_unreachable();"
                return ""
            if i in keep and (m := COMMENTED_OUT_ERROR.match(text)):
                return m.group(1) + m.group(2)
            if i in keep and (m := COMMENTED_OUT_CODE.match(text)):
                return m.group(1) + m.group(2)
            return text

        return "\n".join(line(i, ln) for i, ln in enumerate(self.lines))


def not_declarations(lines, setup):
    """The first `snippet-setup` line that is not part of a declaration, or None.

    Hidden code may declare what the page leaves to the reader (a function it only calls,
    a value it only reads), never do something. A line continuing the declaration on the
    setup line above it counts as part of it.
    """
    for i in setup:
        line = lines[i]
        continues = i - 1 in setup and not ends_statement(lines[i - 1])
        if not (
            continues or DECLARATION.match(line) or DECLARED_NAME.match(code_of(line))
        ):
            return line.strip()
    return None


def documented_header(path):
    """The `#include` spelling of a header under one of the include directories."""
    for base in INCLUDE_DIRS:
        if path.is_relative_to(base):
            return path.relative_to(base).as_posix()
    return None


def collect():
    """Every sample, in page order, the ones whose markers are wrong, and the opted-out ones."""
    out, problems, opted_out, embeds = [], [], [], []
    for base, pat, strip_stars in (
        (ROOT / "docs", FENCE, False),
        (ROOT / "src", MRDOCS_CODE, True),
    ):
        for path in sorted(base.rglob("*.md" if not strip_stars else "*.h")):
            rel = path.relative_to(ROOT).as_posix()
            if any(d in rel for d in SKIP_DIRS):
                continue
            header = documented_header(path) if strip_stars else None
            text = path.read_text(encoding="utf-8")
            prev = None  # the previous sample on the page, or why there is none
            for m in pat.finditer(text):
                body = m.group(2) if not strip_stars else m.group(1)
                if strip_stars:  # drop the leading " * " of a doc comment
                    body = "\n".join(
                        re.sub(r"^\s*\*\s?", "", ln) for ln in body.splitlines()
                    )
                body = textwrap.dedent(body)
                # the body starts on the line after the one holding the fence
                line = text[: m.start()].count("\n") + 1
                if "--8<--" in body:
                    # Pulled from a real file, which is compiled there. It is compiled here
                    # only as the context of a sample continuing it.
                    prev = embedded(rel, line, body, header)
                    if isinstance(prev, Sample):
                        embeds.append(prev)
                    continue
                tpl = reason = std = None
                if mk := MARKER.search(body):
                    tpl, reason = mk.groups()
                if st := STD.search(body):
                    std = st.group(1)
                ident = ID.search(body)
                refs = [
                    r.strip()
                    for a in AFTER.findall(body)
                    for r in a.split(",")
                    if r.strip()
                ]
                if CONTINUES.search(body):
                    refs.insert(
                        0,
                        (
                            prev
                            if isinstance(prev, Sample)
                            else f"<previous, which {prev or 'is missing'}>"
                        ),
                    )
                setup_lines = [
                    i for i, ln in enumerate(body.split("\n")) if SETUP.match(ln)
                ]
                body = SETUP.sub(lambda m: m.group(1) + m.group(2), body)
                for pattern in (MARKER, STD, CONTINUES, ID, AFTER, CE_EMBED):
                    body = pattern.sub("", body)
                if tpl == "none" and reason and not OPT_OUT_REASON.match(reason):
                    reason = None  # reported below, like a missing reason
                if tpl == "none" and reason:
                    opted_out.append((rel, line, reason))
                if (tpl == "none" and reason) or not body.strip():
                    prev = "is opted out" if tpl == "none" else "is empty"
                    continue
                s = Sample(rel, line, body, tpl, header, std)
                s.id, s.refs, s.setup = ident and ident.group(1), refs, set(setup_lines)
                if bad := not_declarations(s.lines, setup_lines):
                    problems.append(
                        (
                            s,
                            f"a `snippet-setup` line must be a declaration, not `{bad}`",
                        )
                    )
                    prev = s
                    continue
                if tpl == "none":
                    problems.append(
                        (
                            s,
                            "opts out without a reason in one of the categories: "
                            "`// snippet-template: none (category: why)`",
                        )
                    )
                    prev = "is opted out"
                    continue
                if reason:
                    problems.append((s, f"only `none` takes a reason, not `{tpl}`"))
                elif refs and (tpl or std):
                    problems.append(
                        (
                            s,
                            "a sample compiled after others inherits their template and standard",
                        )
                    )
                else:
                    out.append(s)
                prev = s
    # An embedded file stays only while a sample is compiled after it. One with an id may
    # be named from anywhere, so it is kept, and dropped below if nothing does.
    used = {id(p) for s in out for p in s.refs if isinstance(p, Sample)}
    out += [e for e in embeds if id(e) in used or e.id]
    # resolve `snippet-after`, on the same page or, with a path, on another
    by_id = {(s.origin, s.id): s for s in out if s.id}
    sources = {}
    for s in list(out):
        for ref in s.refs:
            if isinstance(ref, Sample):
                s.after.append(ref)
                continue
            only_decls = ref.endswith(":decls")
            name = ref.removesuffix(":decls")
            if "#" in name:
                origin, _, ident = name.rpartition("#")
            elif "/" in name:  # a source file, compiled whole
                origin, ident = name, ""
            else:
                origin, ident = s.origin, name
            target = by_id.get((origin, ident))
            if (
                target is None
                and not ident
                and (ROOT / origin).is_file()
                and origin.startswith("example/")
            ):
                # a whole example source the page is about, as `snippet-after: example/x.cpp#`
                target = sources.setdefault(origin, source_sample(origin))
                if target not in out:
                    out.append(target)
            if target is not None and only_decls:
                s.decls_only.add(id(target))
            if target is None:
                problems.append(
                    (s, f"is compiled after `{ref}`, which names no compiled sample")
                )
                out.remove(s)
                break
            s.after.append(target)
    for s, why in problems:
        s.error = why
    return out, [s for s, _ in problems], opted_out


def embedded(origin, line, body, header):
    """The sample an `--8<--` fence embeds, or why there is none."""
    found = EMBED.findall(body)
    if not found:
        return "embeds a file in a form this check does not read"
    lines = []
    for path, first, last in found:
        source = ROOT / path
        if not source.is_file():
            return f"embeds {path}, which does not exist"
        text = source.read_text(encoding="utf-8").splitlines()
        first = int(first) if first else 1
        lines += text[first - 1 : int(last) if last else len(text)]
    s = Sample(origin, line, "\n".join(lines), None, header)
    s.embed = True
    if ident := ID.search(body):
        s.id = ident.group(1)
    return s


def source_sample(path):
    """An example source compiled whole, as the context of a sample naming it."""
    s = Sample(path, 1, (ROOT / path).read_text(encoding="utf-8"), None)
    s.embed = True
    return s


def in_order(samples):
    """The samples with every one after those it is compiled after; a cycle is an error."""
    done, order, broken = set(), [], []

    def visit(s, path):
        if id(s) in done:
            return True
        if id(s) in path:
            s.error = "is compiled after itself, through `snippet-after`"
            return False
        path.add(id(s))
        ok = all(visit(p, path) for p in s.after)
        path.discard(id(s))
        if ok:
            done.add(id(s))
            order.append(s)
        else:
            broken.append(s)
        return ok

    for s in samples:
        visit(s, set())
    return order, broken


EMPTY = {
    "includes": [],
    "head": [],
    "decls": [],
    "stmts": [],
    "tail": [],
    "main": None,
    "closed": False,
}


def merged(samples, decls_only=()):
    """The contributions of the samples a sample is compiled after, in order and without duplicates.

    From a sample named with `:decls`, and whatever it is compiled after, only the
    declarations are taken: its statements, and their locals, are left out.
    """
    seen, parts = set(), []
    for s in samples:
        for key, part in s.context:
            if id(s) in decls_only:
                part = {**part, "stmts": [], "main": None}
            if key not in seen:
                seen.add(key)
                parts.append((key, part))
    return parts


def view(parts):
    """The contributions joined into one context, with the same keys as `EMPTY`."""
    ctx = {
        "includes": [],
        "head": [],
        "decls": [],
        "stmts": [],
        "tail": [],
        "main": None,
        "closed": False,
    }
    for _, part in parts:
        for key in ("includes", "head", "decls", "stmts", "tail"):
            ctx[key] += part[key]
        ctx["closed"] |= part["closed"]
        ctx["main"] = part["main"] or ctx["main"]
    return ctx


# `Type name = ...`, `Type name{...}` or `using name = ...`: what a line declares, roughly
DECLARED_NAME = re.compile(
    r"^\s*(?!(?:return|throw|co_return|co_yield|else|case|goto|delete)\b)"  # not a statement
    r"(?:(?:const|constexpr|static|inline)\s+)*"
    r"(?:using\s+(\w+)\s*=|[\w:<>,\[\]\s*&]+?\s(\w+)\s*(?:=|\{))"
)


def declared_names(lines):
    """The names some of these lines declare at their own level, to spot a name declared twice."""
    names = set()
    for line in lines:
        if is_code(line) and (m := DECLARED_NAME.match(code_of(line))):
            names.add(m.group(1) or m.group(2))
    return names


def split_points(lines):
    """Where a sample that declares things and then uses them may be cut in two.

    A cut goes after a blank line outside any braces, or before the first line that
    can only be a statement, with code on both sides. The part above goes to
    namespace scope and the part below into `main`.
    """
    cuts, depth, statement_seen = [], 0, False
    for k, line in enumerate(lines):
        if depth == 0 and any(map(is_code, lines[:k])) and any(map(is_code, lines[k:])):
            below = next((ln for ln in lines[k + 1 :] if is_code(ln)), "")
            # Only a cut before statements: below a cut that starts with a declaration, a
            # second declaration of a name above it would hide in `main`'s own scope.
            redeclared = declared_names(lines[:k]) & declared_names(lines[k + 1 :])
            if not line.strip() and not DECLARATION.match(below) and not redeclared:
                cuts.append(("split", k + 1))
            elif not statement_seen and STATEMENT.match(line):
                # declarations followed by statements with no blank line between them
                statement_seen = True
                cuts.append(("split", k))
        code = line.split("//")[0]
        depth += code.count("{") - code.count("}")
    return list(dict.fromkeys(cuts))


# a line that can only be a statement, so a cut before it may separate declarations from use
STATEMENT = re.compile(
    r"^\s*(?:std::(?:cout|cerr|print|println)\b|assert\s*\(|if\s*\(|for\s*\(|while\s*\()"
)


def placements_for(lines, ctx):
    """Where the body may go, most likely first.

    `raw` compiles the sample as it stands, for one that brings its own includes.
    `whole` puts it after the template preamble, because it brings its own `main`.
    Otherwise it is tried as the body of `main`, then at namespace scope, then cut
    in two (see `split_points`) into declarations and the statements that use them. A sample's
    own `#include` lines go ahead of the template's using-directives. A sample
    compiled after one that has its own `main` can only add declarations.
    """
    has_main = any(MAIN.search(ln) for ln in lines)
    # This only sets the order, so a wrong guess costs extra compiles. Where a sample compiles
    # in more than one placement, the order decides which one is kept.
    code = [ln for ln in lines if is_code(ln) and not INCLUDE_LINE.match(ln)]
    # A sample opening with a declaration tries namespace scope, then the cuts, and `main`
    # last. A function declaration compiles in `main` too, but there a later sample's
    # declarations don't see it.
    if code and DECLARATION.match(code[0]):
        either = ["decl", *split_points(lines), "main"]
    else:
        either = ["main", "decl", *split_points(lines)]
    if ctx is None:
        raw = ["raw"] if any(INCLUDE_LINE.match(ln) for ln in lines) else []
        return raw + (["whole"] if has_main else either)
    if ctx["closed"]:
        return ["decl"]
    if has_main:
        return ["whole"] if not ctx["stmts"] else []
    return either


def open_main(lines):
    """A program cut around its `main`: what precedes it, its signature, its body, and what follows.

    A sample continuing a whole program, as a workshop's Solution, often adds
    statements that belong in that program's `main`, using its locals.
    """
    text = "\n".join(lines)
    m = re.search(r"\bint\s+main\s*\([^)]*\)\s*\{", text)
    if not m:
        return None
    depth = 0
    for pos in range(m.end() - 1, len(text)):
        depth += {"{": 1, "}": -1}.get(text[pos], 0)
        if depth == 0:
            signature = text[m.start() : m.end() - 1].strip()
            return text[: m.start()], signature, text[m.end() : pos], text[pos + 1 :]
    return None


def contribution(lines, placement):
    """What a sample adds to the context of a sample compiled after it."""
    part = {
        "includes": [],
        "head": [],
        "decls": [],
        "stmts": [],
        "tail": [],
        "main": None,
        "closed": False,
    }
    lines = [IMPORT.sub("", ln) for ln in lines]
    if placement in ("raw", "whole"):
        if placement == "whole":
            part["includes"] = [ln for ln in lines if INCLUDE_LINE.match(ln)]
            lines = ["" if INCLUDE_LINE.match(ln) else ln for ln in lines]
        if opened := open_main(lines):
            before, part["main"], body, after = opened
            part["head" if placement == "raw" else "decls"] = [before]
            part["stmts"], part["tail"] = [body], [after]
        else:
            part["head" if placement == "raw" else "decls"] = ["\n".join(lines)]
            part["closed"] = any(MAIN.search(ln) for ln in lines)
        return part
    top = set(hoisted(lines))
    part["includes"] = [
        ln for i, ln in enumerate(lines) if i in top and INCLUDE_LINE.match(ln)
    ]
    usings = [
        ln for i, ln in enumerate(lines) if i in top and not INCLUDE_LINE.match(ln)
    ]
    rest = ["" if i in top else ln for i, ln in enumerate(lines)]
    if usings:  # at namespace scope, after the context they may name a namespace of
        part["decls"] = ["\n".join(usings)]
    if placement == "decl":
        part["decls"] += ["\n".join(rest)]
    elif placement == "main":
        part["stmts"] = ["\n".join(rest)]
    else:
        cut = placement[1]
        part["decls"] += ["\n".join(rest[:cut])]
        part["stmts"] = ["\n".join(rest[cut:])]
    return part


def render(sample, lines, placement, ctx, pch="", prologue="", epilogue=""):
    """The translation unit, and a map from its line numbers to the sample's own lines."""
    out = []

    def add(text):
        out.extend((ln, None) for ln in text.split("\n"))

    def add_body(indices):
        out.extend((IMPORT.sub("", lines[i]), i) for i in indices)

    everything = range(len(lines))
    if placement == "raw":
        add_body(everything)
    else:
        top = hoisted(lines)
        includes = [i for i in top if INCLUDE_LINE.match(lines[i])]
        usings = [i for i in top if i not in includes]
        rest = [i for i in everything if i not in top]
        if pch:
            add(pch)
        add_body(includes)
        if sample.header:
            add(f"#include <{sample.header}>")
        for text in [*ctx["includes"], prologue, *ctx["head"], *ctx["decls"]]:
            if text:
                add(text)
        add_body(usings)  # after the context, which may declare the namespace they name
        cut = placement[1] if isinstance(placement, tuple) else None
        if placement in ("decl", "whole"):
            add_body(rest)
        elif cut is not None:
            add_body([i for i in rest if i < cut])
        if placement != "whole" and not ctx["closed"]:
            add((ctx["main"] or "int main()") + "\n{")
            for text in ctx["stmts"]:
                add(text)
            if placement == "main":
                add_body(rest)
            elif cut is not None:
                add_body([i for i in rest if i >= cut])
            add("}")
            for text in ctx["tail"]:
                if text.strip():
                    add(text)
        if epilogue.strip():
            add(epilogue)
    text = "\n".join(ln for ln, _ in out) + "\n"
    return text, {n: i for n, (_, i) in enumerate(out, 1) if i is not None}


def first_error(stderr, src, linemap, sample):
    """The first diagnostic, and the documentation line it points at."""
    lines = stderr.splitlines()
    first = next((ln for ln in lines if ": error:" in ln), None)
    first = first or next(
        (ln for ln in lines if DEPRECATED.search(ln)), lines[0] if lines else ""
    )
    m = re.match(rf"{re.escape(str(src))}:(\d+):", first)
    if m and int(m.group(1)) in linemap:
        return first, sample.line + 1 + linemap[int(m.group(1))]
    return first, sample.line


def split_preamble(template):
    """The template's text ahead of the body: what can be precompiled, and what follows it.

    The precompiled part is compiled on its own, so it can't leave a namespace open. A
    template that opens a namespace around the body keeps that opening out of it.
    """
    lines = template.split("{{BODY}}")[0].splitlines(keepends=True)
    depth, cut = 0, 0
    for i, ln in enumerate(lines, 1):
        depth += ln.count("{") - ln.count("}")
        if depth == 0:
            cut = i
    return "".join(lines[:cut]), "".join(lines[cut:])


def fingerprint(files):
    """The contents of the files a precompiled header was built from.

    GCC doesn't reject a precompiled header when one of them changes. It uses the
    stale one. Wherever the translation unit includes the changed file again, GCC
    reads it a second time, because a `#pragma once` file with different contents is
    a different file to it. So every byte counts, comments included.
    """
    digest = hashlib.sha256()
    for name in sorted(files):
        path = Path(name)
        digest.update(
            name.encode()
            + b"\0"
            + (path.read_bytes() if path.exists() else b"<missing>")
        )
    return digest.hexdigest()


def pch_cache():
    """Where the precompiled headers are kept between runs."""
    root = (
        Path(os.environ.get("XDG_CACHE_HOME", Path.home() / ".cache"))
        / "mp-units"
        / "doc-snippets"
    )
    root.mkdir(parents=True, exist_ok=True)
    return root


class ResultCache:
    """The outcome of every compile, kept between runs and reused while nothing it read changed.

    An entry is found by the compiler, the command line and the source text. An edited
    sample, or one whose context changed, gets a new entry. It records the files the
    compile read and their contents. It is reused only while every one of them is byte
    for byte the same, so a changed header recompiles exactly the samples that include it, wherever
    they live. GCC's list of those files leaves out a precompiled header and everything
    it was built from, so the caller names the header and its `.deps` record adds them.
    """

    SRC = "\0SRC\0"  # stands in for the generated source's path, which differs between runs
    MAX_AGE_DAYS = 30

    def __init__(self, root, cxx):
        self.root = root / "results"
        self.root.mkdir(exist_ok=True)
        stale = time.time() - self.MAX_AGE_DAYS * 86400
        for p in self.root.glob("*.json"):  # entries no run has used for a month
            if p.stat().st_mtime < stale:
                p.unlink(missing_ok=True)
        version = subprocess.run(
            [cxx, "--version"], capture_output=True, text=True
        ).stdout
        self.compiler = f"{shutil.which(cxx)}\0{version}"
        self.digests = {}
        self.hits = self.misses = 0
        self.lock = threading.Lock()  # for the counts, updated from every worker

    def digest(self, name):
        """One file's contents, read once per run."""
        if name not in self.digests:
            path = Path(name)
            self.digests[name] = hashlib.sha256(
                path.read_bytes() if path.exists() else b"<missing>"
            ).hexdigest()
        return self.digests[name]

    def fingerprint(self, files):
        return hashlib.sha256(
            "\0".join(f"{f}\0{self.digest(f)}" for f in files).encode()
        ).hexdigest()

    def compile(self, cmd, src, text, pch=None):
        """`cmd` run on `src` holding `text`, which includes the precompiled header `pch`.

        The result comes from the cache when nothing the compile read has changed.
        """
        src.write_text(text, encoding="utf-8")
        key = hashlib.sha256(
            "\0".join([self.compiler, *cmd, text]).encode()
        ).hexdigest()
        entry = self.root / f"{key}.json"
        try:
            known = json.loads(entry.read_text(encoding="utf-8"))
            if self.fingerprint(known["deps"]) == known["fingerprint"]:
                os.utime(entry)
                with self.lock:
                    self.hits += 1
                return subprocess.CompletedProcess(
                    cmd,
                    known["returncode"],
                    "",
                    known["stderr"].replace(self.SRC, str(src)),
                )
        except (OSError, ValueError, KeyError):
            pass
        with self.lock:
            self.misses += 1
        depfile = src.with_suffix(".d")
        r = subprocess.run(
            [*cmd, "-MD", "-MF", str(depfile), str(src)],
            capture_output=True,
            text=True,
        )
        # No depfile when an `#include` isn't found. That outcome isn't kept, so the
        # file appearing later is noticed.
        if depfile.exists():
            deps = set(
                depfile.read_text(encoding="utf-8")
                .replace("\\\n", " ")
                .split(":", 1)[1]
                .split()
            ) - {str(src)}
            depfile.unlink()
            if pch:
                deps.add(str(pch))
                deps.update(
                    Path(f"{pch}.deps").read_text(encoding="utf-8").splitlines()[1:]
                )
            deps = sorted(deps)
            partial = entry.with_name(f"{entry.name}.{os.getpid()}.{id(src)}.tmp")
            partial.write_text(
                json.dumps(
                    {
                        "returncode": r.returncode,
                        "stderr": r.stderr.replace(str(src), self.SRC),
                        "deps": deps,
                        "fingerprint": self.fingerprint(deps),
                    }
                ),
                encoding="utf-8",
            )
            os.replace(partial, entry)
        return r


def build(args, work, selected):
    inc = [f"-I{d}" for d in INCLUDE_DIRS + EXTRA_INCLUDE_DIRS] + [f"-I{TEMPLATES}"]

    def flags(std):
        return [args.cxx, f"-std=c++{std}", *inc]

    everything, broken, _ = collect()
    if selected is not None:
        broken = [s for s in broken if s.origin in selected]
        # the selected samples, and those they are compiled after
        wanted_ids, stack = set(), [s for s in everything if s.origin in selected]
        while stack:
            s = stack.pop()
            if id(s) not in wanted_ids:
                wanted_ids.add(id(s))
                stack += s.after
        everything = [s for s in everything if id(s) in wanted_ids]
    samples, cyclic = in_order(everything)
    broken += cyclic
    if args.limit:
        samples = samples[: args.limit]
    print(f"collected {len(samples) + len(broken)} samples", flush=True)
    if not samples:
        return broken

    for s in samples:  # one compiled after others takes theirs once they are resolved
        if not s.after:
            s.std = s.std or args.std

    # One precompiled header per template and standard, so the library is parsed
    # once for each rather than once per sample. Only the ones in use are built.
    templates = {
        p.stem: p.read_text(encoding="utf-8") for p in TEMPLATES.glob("*.template")
    }
    roots = [s for s in samples if not s.after]
    wanted = {(s.template, s.std) for s in roots if s.template} | {
        (tpl, s.std) for s in roots if not s.template for tpl in ("si", "isq")
    }
    wanted = {(tpl, std) for tpl, std in wanted if tpl in templates}
    pch_dir = pch_cache() if not args.no_cache else work
    preamble = {}

    def precompile(wanted_one):
        name, std = wanted_one
        precompiled, rest = split_preamble(templates[name])
        key = hashlib.sha256(
            "\0".join([*flags(std), precompiled]).encode()
        ).hexdigest()[:16]
        h = pch_dir / f"pre_{name}{std}_{key}.h"
        gch, deps = Path(f"{h}.gch"), Path(f"{h}.deps")
        preamble[name, std] = (f'#include "{h.name}"', rest, h)
        if not args.no_cache and gch.exists() and deps.exists():
            recorded, *files = deps.read_text(encoding="utf-8").splitlines()
            if fingerprint(files) == recorded:
                return
        h.write_text(precompiled, encoding="utf-8")
        # moved in place only once complete, so a concurrent run never reads half of one
        partial, depfile = (
            pch_dir / f"{h.name}.{os.getpid()}.tmp",
            pch_dir / f"{h.name}.{os.getpid()}.d",
        )
        r = subprocess.run(
            [
                *flags(std),
                "-x",
                "c++-header",
                str(h),
                "-o",
                str(partial),
                "-MD",
                "-MF",
                str(depfile),
            ],
            capture_output=True,
            text=True,
        )
        if r.returncode != 0 or DEPRECATED.search(r.stderr):
            partial.unlink(missing_ok=True)
            depfile.unlink(missing_ok=True)
            sys.exit(f"template '{name}' does not compile as C++{std}:\n{r.stderr}")
        files = sorted(
            set(
                depfile.read_text(encoding="utf-8")
                .replace("\\\n", " ")
                .split(":", 1)[1]
                .split()
            )
        )
        depfile.unlink()
        os.replace(partial, gch)
        deps.write_text(fingerprint(files) + "\n" + "\n".join(files), encoding="utf-8")

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        list(ex.map(precompile, sorted(wanted)))

    cache = None if args.no_cache else ResultCache(pch_dir, args.cxx)

    def compile_one(src, text, std, pch):
        cmd = [*flags(std), f"-I{pch_dir}", "-fsyntax-only"]
        if cache:
            return cache.compile(cmd, src, text, pch)
        src.write_text(text, encoding="utf-8")
        return subprocess.run([*cmd, str(src)], capture_output=True, text=True)

    def attempt(idx, s, tpl, placement, ctx, keep=()):
        where = (
            "split" + str(placement[1]) if isinstance(placement, tuple) else placement
        )
        src = work / f"s{idx}_{tpl}_{where}{'_l' + str(keep[0]) if keep else ''}.cpp"
        pch, prologue, pch_header = preamble.get((tpl, s.std), ("", "", None))
        epilogue = templates[tpl].split("{{BODY}}", 1)[1] if tpl else ""
        text, linemap = render(
            s, s.body(keep).split("\n"), placement, ctx, pch, prologue, epilogue
        )
        return src, linemap, compile_one(src, text, s.std, pch_header)

    def check_rejected(idx, s, ctx):
        """Each group of lines that must not compile, added back alone, has to be what fails."""
        for group in s.rejected:
            src, linemap, r = attempt(idx, s, s.template, s.placement, ctx, keep=group)
            doc_line = s.line + 1 + group[0]
            generated = {n for n, line in linemap.items() if line in group}
            # what the compiler blames on the group: an error there, or an instantiation
            # it reports as "required from here" there (a `note:` naming it doesn't count)
            blamed = [
                ln
                for ln in r.stderr.splitlines()
                if any(ln.startswith(f"{src}:{n}:") for n in generated)
                and (": error:" in ln or "required from here" in ln)
            ]
            if r.returncode == 0:
                s.error, s.error_line = (
                    "this line is marked as not compiling, but it compiles",
                    doc_line,
                )
                if DEPRECATED.search(r.stderr):
                    s.error += f" (with a deprecation warning only):\n{r.stderr}"
            elif not blamed:
                s.error = f"the line marked as not compiling is not the one that fails:\n{r.stderr}"
                s.error_line = doc_line
            elif UNDECLARED.search(blamed[0]):
                # Only the first error counts. A misspelled name brings more errors on the same
                # line, and none of them is a reason the line was meant to be rejected for.
                s.error = (
                    "the line marked as not compiling fails first because it names something the "
                    f"sample never declares, so it proves nothing:\n{r.stderr}"
                )
                s.error_line = doc_line
            else:
                continue
            s.placement = None
            return

    def run(idx, s, waits):
        for w in waits:  # a sample compiled after others needs them resolved
            w.result()
        if failed := [p for p in s.after if not p.placement]:
            s.error = f"is compiled after {failed[0].name}, which does not compile"
            if failed[0].embed:  # not reported on its own, so its diagnostic goes here
                s.error += f" (an embedded file):\n{failed[0].error}"
            return s
        if s.after:  # a sample compiled after others takes their template and standard
            s.template, s.std = s.after[0].template, s.after[0].std
            # after a sample that brings its own includes, the default template's
            # usual context still applies (the headers it includes are each read only once)
            s.template = s.template or "si"
        parts = merged(s.after, s.decls_only) if s.after else []
        ctx = view(parts) if s.after else None
        lines = s.body().split("\n")
        placements = placements_for(lines, ctx)
        if not placements:
            s.error = "follows samples whose statements already fill `main`, so it cannot bring its own"
            return s
        candidates = [s.template] if s.after or s.template else ["si", "isq"]
        tries = [(None, "raw")] if "raw" in placements else []
        tries += [(tpl, p) for tpl in candidates for p in placements if p != "raw"]
        attempts = []  # the diagnostic of every rejected attempt
        for tpl, placement in tries:
            if tpl is not None and tpl not in templates:
                s.error = f"unknown template '{tpl}'"
                return s
            src, linemap, r = attempt(idx, s, tpl, placement, ctx or EMPTY)
            if r.returncode == 0 and not DEPRECATED.search(r.stderr):
                s.placement, s.template = placement, tpl
                s.context = parts + [(id(s), contribution(lines, placement))]
                check_rejected(idx, s, ctx or EMPTY)
                if not s.placement and placement == "decl" and s.rejected:
                    # The rest is declarations, but a rejected line may be a statement. At
                    # namespace scope a statement fails only because it is there, so put
                    # everything from the first rejected line on into `main`.
                    cut = ("split", s.rejected[0][0])
                    src, linemap, r = attempt(idx, s, tpl, cut, ctx or EMPTY)
                    if r.returncode == 0 and not DEPRECATED.search(r.stderr):
                        failed_as_decl = s.error
                        s.placement, s.error = cut, ""
                        s.context = parts + [(id(s), contribution(lines, cut))]
                        check_rejected(idx, s, ctx or EMPTY)
                        if not s.placement:
                            s.error = s.error or failed_as_decl
                return s
            attempts.append((r.stderr, src, linemap))
        # The first attempt is rarely the informative one: a declaration tried
        # as the body of `main` fails for being at block scope, which says
        # nothing about the sample. Of those that do not fail on placement, the
        # attempt with the fewest errors got the furthest, so its diagnostic is
        # the one worth reading.
        stderr, src, linemap = min(attempts, key=lambda a: relevance(a[0]))
        s.error = stderr
        s.error_line = first_error(stderr, src, linemap, s)[1]
        return s

    # The pool starts tasks in submission order. A sample is always submitted after the
    # ones it is compiled after, so those have started before it waits for them, and it
    # can't block them.
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        futures = {}
        for idx, s in enumerate(samples):
            futures[id(s)] = ex.submit(run, idx, s, [futures[id(p)] for p in s.after])
        results = [f.result() for f in futures.values()]
    if cache:
        print(
            f"{cache.hits} of {cache.hits + cache.misses} compiles reused from the cache",
            flush=True,
        )
    # embedded files, and the samples compiled only as another one's context, are not reported
    results = [
        r for r in results if not r.embed and (selected is None or r.origin in selected)
    ]
    return results + broken


# What the compiler says when a placement is wrong for the sample, rather than
# the sample being wrong. GCC skips the construct it rejects there, with every error
# inside it, so such an attempt can look like the best one by error count alone.
WRONG_PLACEMENT = re.compile(r"block scope|is not allowed here|cannot appear at")


def relevance(stderr):
    """Sort key over rejected attempts: the most informative diagnostic first."""
    return bool(WRONG_PLACEMENT.search(stderr)), stderr.count(": error:")


def readable(error, limit=40):
    """The diagnostic without the generated file names, cut to a readable length."""
    text = re.sub(r"\S*/s\d+_\w+?\.cpp", "<sample>", error)
    text = re.sub(re.escape(str(ROOT)) + "/", "", text)
    lines = text.splitlines()
    return "\n".join(
        lines[:limit]
        + ([f"... ({len(lines) - limit} more lines)"] if len(lines) > limit else [])
    )


def repro(r, args):
    return f"python3 scripts/check_doc_snippets.py --cxx {args.cxx} --std {args.std} {shlex.quote(r.origin)}"


# CI pins the compiler (.github/workflows/documentation.yml), because whether a line marked
# `❌` is rejected, and on which line, depends on it. The precompiled headers are GCC's too.
REQUIRED_GCC = 15


def resolve_compiler(cxx, hook=False):
    """The compiler to check with: `cxx`, or the first GCC 15 found. Exits if there is none.

    From the hook, a missing compiler only warns, so a machine without GCC 15 can still
    commit, and CI checks the samples before the documentation is published.
    """
    found = []
    for name in [cxx] if cxx else [f"g++-{REQUIRED_GCC}", "g++"]:
        if not shutil.which(name):
            continue
        try:
            macros = subprocess.run(
                [name, "-dM", "-E", "-x", "c++", os.devnull],
                capture_output=True,
                text=True,
                timeout=60,
            ).stdout
        except (OSError, subprocess.TimeoutExpired):
            continue
        major = re.search(r"^#define __GNUC__ (\d+)$", macros, re.M)
        if "__clang__" in macros or not major:
            found.append(f"{name} (not GCC)")
        elif int(major.group(1)) != REQUIRED_GCC:
            found.append(f"{name} (GCC {major.group(1)})")
        else:
            return name
    found = ", ".join(found) or (
        f"no {cxx}" if cxx else f"neither g++-{REQUIRED_GCC} nor g++"
    )
    if hook:
        print(
            f"warning: the documentation samples were not checked, because GCC {REQUIRED_GCC} isn't here.\n"
            f"Found: {found}. CI checks them before the documentation is published.\n"
            f"To check them here, install g++-{REQUIRED_GCC} or use the dev container."
        )
        sys.exit(0)
    sys.exit(
        f"error: the documentation samples are checked with GCC {REQUIRED_GCC}, which CI pins,\n"
        f"because whether a line marked `❌` fails, and where, depends on the compiler.\n"
        f"Found: {found}.\n"
        f"Install g++-{REQUIRED_GCC} and pass it with --cxx if it is not on PATH as g++-{REQUIRED_GCC},\n"
        f"or use the dev container. To commit without the check: SKIP=doc-snippets git commit ..."
    )


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "files", nargs="*", help="check only the samples in these files (default: all)"
    )
    ap.add_argument(
        "--cxx",
        help=f"GCC {REQUIRED_GCC} (default: g++-{REQUIRED_GCC}, or g++ if that is GCC {REQUIRED_GCC})",
    )
    ap.add_argument("--std", default="23")
    ap.add_argument("--jobs", type=int, default=os.cpu_count())
    ap.add_argument(
        "--workdir", help="keep the generated sources here (default: a temporary one)"
    )
    ap.add_argument(
        "--no-cache",
        action="store_true",
        help="build the precompiled headers and compile every sample afresh, rather than reuse ~/.cache",
    )
    ap.add_argument("--only", help="only samples whose path starts with this prefix")
    ap.add_argument("--limit", type=int, help="stop after N samples (smoke test)")
    ap.add_argument("--list-failures", action="store_true")
    ap.add_argument(
        "--hook",
        action="store_true",
        help="as the pre-commit hook: warn rather than fail without GCC 15, "
        "list the failures, and print one line when nothing fails",
    )
    ap.add_argument(
        "--list-opt-outs",
        action="store_true",
        help="list the opted-out samples and why",
    )
    ap.add_argument(
        "--list-setup",
        action="store_true",
        help="list the snippet-setup declarations, which the site does not show",
    )
    ap.add_argument(
        "--report", help="write every failure, with its diagnostic, to this file"
    )
    ap.add_argument(
        "--summary", help="append the failures, as GitHub Markdown, to this file"
    )
    args = ap.parse_args()
    args.cxx = resolve_compiler(args.cxx, args.hook)

    selected = None
    if args.files:
        files = {Path(f).resolve() for f in args.files}
        here = Path(__file__).resolve()
        # the checker or a template changed, so any sample may have changed with it
        if not any(f == here or f.is_relative_to(TEMPLATES) for f in files):
            selected = {
                f.relative_to(ROOT).as_posix() for f in files if f.is_relative_to(ROOT)
            }
    if args.only:
        samples, broken, opted = collect()
        everything = {s.origin for s in samples + broken} | {o[0] for o in opted}
        selected = {o for o in (selected or everything) if o.startswith(args.only)}

    began = time.monotonic()
    if args.workdir:
        work = Path(args.workdir)
        work.mkdir(parents=True, exist_ok=True)
        results = build(args, work, selected)
    else:
        with tempfile.TemporaryDirectory(prefix="mp-units-snippets-") as tmp:
            results = build(args, Path(tmp), selected)
    elapsed = time.monotonic() - began
    collected, _, all_opted_out = collect()
    opted_out = [o for o in all_opted_out if selected is None or o[0] in selected]
    setup = [
        (s.origin, s.line + 1 + i, s.lines[i].strip())
        for s in collected
        if selected is None or s.origin in selected
        for i in sorted(s.setup)
    ]

    ok = [r for r in results if r.placement]
    bad = sorted(
        (r for r in results if not r.placement), key=lambda r: (r.origin, r.line)
    )
    by_placement = {}
    for r in ok:
        where = "split" if isinstance(r.placement, tuple) else r.placement
        key = f"{r.template}/{where}" if where != "raw" else "raw"
        by_placement[key] = by_placement.get(key, 0) + 1
    rejected = sum(len(r.rejected) for r in ok)
    print(f"\ncompiled {len(ok)}/{len(results)} in {elapsed:.0f} s")
    if args.hook and not bad:
        return 0
    for k, v in sorted(by_placement.items()):
        print(f"  {v:5d}  placement={k}")
    print(f"  {sum(1 for r in ok if r.after):5d}  of them compiled after other samples")
    print(f"  {rejected:5d}  lines (or statements) checked not to compile")
    print(f"  {len(bad):5d}  FAILED")
    print(f"  {len(opted_out):5d}  opted out (not compiled; --list-opt-outs says why)")
    if args.list_opt_outs:
        for origin, line, reason in opted_out:
            print(f"    {origin}:{line}  {reason}")
    print(f"  {len(setup):5d}  hidden setup declarations (--list-setup shows them)")
    if args.list_setup:
        for origin, line, code in setup:
            print(f"    {origin}:{line}  {code}")
    if args.report:
        with open(args.report, "w", encoding="utf-8") as fh:
            for r in bad:
                fh.write(
                    f"### {r.origin}:{r.error_line}  [sample {r.name}, template={r.template}]\n"
                    f"{r.body()}\n--- diagnostic ---\n{r.error}\n\n"
                )
        print(f"  failures written to {args.report}")
    if args.list_failures or bad and (args.files or args.hook):
        for r in bad:
            lines = r.error.splitlines() or ["?"]
            first = next(
                (ln for ln in lines if ": error:" in ln or DEPRECATED.search(ln)),
                lines[0],
            )
            print(
                f"    {r.origin}:{r.error_line}  [{r.template}]  {readable(first)[:150]}"
            )
        if bad and (args.files or args.hook):
            print("\nreproduce one with:", repro(bad[0], args), "--report /dev/stdout")
    if args.summary and bad:
        with open(args.summary, "a", encoding="utf-8") as fh:
            fh.write(f"## ❌ {len(bad)} documentation sample(s) do not compile\n\n")
            for r in bad:
                fh.write(
                    f"### `{r.origin}:{r.error_line}`\n\n```text\n{readable(r.error)}\n```\n\n"
                )
                fh.write(
                    "**Local repro** (in the provided '.devcontainer' or GitHub CodeSpaces environment):\n\n"
                )
                fh.write(f"```sh\n{repro(r, args)} --report /dev/stdout\n```\n\n")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
