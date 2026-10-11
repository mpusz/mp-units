#!/usr/bin/env python3
# The MIT License (MIT)
#
# Copyright (c) 2018 Mateusz Pusz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Generate an index of the hand-written documentation, in two files.

It answers the question a doc comment's `docs:` link has to answer: which page
actually covers this entity? A filename is no guide: `pure_dimensional_analysis.md`
sounds like the home of `DimensionOf` and never mentions it.

- `docs/.index.md` is the lookup: one line per entity, naming the few pages whose
  code uses it most, so `grep '^- `DimensionOf`' docs/.index.md` answers the
  question. It is kept small because an agent reads it whole.
- `docs/.index-pages.md` is for browsing: each page's title, headings, first
  paragraph where that is real prose, and every entity its code uses, counted.

Blog posts are left out: an entity is never linked to a release announcement, and
they would crowd the reference pages out of the lookup.

An entity is a name the public headers declare at namespace scope outside
`detail`: concepts, class templates, functions, variables and quantity specs. The
vocabulary is read from the headers rather than written down, so a renamed entity
cannot linger here. Only code counts as a mention (fenced blocks, inline code, and
the `--8<--` snippets a page includes), because prose says "dimension" without
meaning `Dimension`. Member functions are not indexed.

The output is committed, and a pre-commit hook (which CI runs too) fails when it
is stale:

    python3 scripts/docs_index.py            # regenerate both files
    python3 scripts/docs_index.py --check    # verify; fail if it would change

ProperDocs skips dot files, so neither file becomes a page of the site.
"""

import argparse
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs"
LOOKUP = DOCS / ".index.md"
PAGES = DOCS / ".index-pages.md"

# pages per entity in the lookup; the tail of a long list is passing mentions
LOOKUP_PAGES = 5

# generated trees, indexed by their own tooling, and the blog
EXCLUDED_DOCS = (
    "reference/api_reference/mrdocs/",
    "reference/systems_reference/",
    "blog/",
)

HEADER_ROOTS = ("src/core/include", "src/systems/include", "src/utility/include")


def _tracked(*paths: str) -> list[Path]:
    """Files git tracks (staged included) under `paths`; untracked drafts must not
    leak into a committed index that CI regenerates without them."""
    out = subprocess.run(
        ["git", "ls-files", "-z", "--", *paths],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    return sorted(ROOT / p for p in out.split("\0") if p and (ROOT / p).is_file())


# ---------------------------------------------------------------------------
# Vocabulary: public names declared by the headers
# ---------------------------------------------------------------------------

_CPP_NOISE = re.compile(
    r"""
      //[^\n]*
    | /\*.*?\*/
    | R"(?P<delim>[^(\s]*)\(.*?\)(?P=delim)"
    | (?:u8|u|U|L)?"(?:\\.|[^"\\\n])*"
    | (?<![\w'])'(?:\\.|[^'\\\n])+'
    """,
    re.S | re.X,
)

_KEYWORDS = frozenset(
    "alignas alignof auto bool char class concept const consteval constexpr "
    "constinit decltype default delete double explicit export extern false final "
    "float friend inline int long namespace noexcept operator requires return "
    "short signed sizeof static static_assert struct template this true typename "
    "union unsigned using void volatile".split()
)


def _strip_preprocessor(text: str) -> str:
    """Drop directives, and every `#else`/`#elif` branch, so that braces balance
    whichever configuration a header was written for."""
    out: list[str] = []
    skipping: list[bool] = []  # per open #if: inside one of its later branches
    for line in text.replace("\\\n", " ").splitlines():
        directive = re.match(r"\s*#\s*(\w+)", line)
        if not directive:
            if not any(skipping):
                out.append(line)
            continue
        word = directive.group(1)
        if word.startswith("if"):
            skipping.append(False)
        elif word in ("else", "elif", "elifdef", "elifndef") and skipping:
            skipping[-1] = True
        elif word == "endif" and skipping:
            skipping.pop()
    return "\n".join(out)


def _skip_balanced(text: str, pos: int, open_: str, close: str) -> int:
    """Index just past the bracket closing the one at `pos`."""
    depth = 0
    for i in range(pos, len(text)):
        if text[i] == open_:
            depth += 1
        elif text[i] == close and not (close == ">" and text[i - 1] == "-"):
            depth -= 1
            if depth == 0:
                return i + 1
    return len(text)


def _strip_template_args(text: str) -> str:
    """`a<b<c>> d(e<f>)` -> `a d(e)`, so a `(` inside template arguments is not
    mistaken for a parameter list."""
    out, i = [], 0
    while i < len(text):
        if text[i] == "<" and i > 0 and re.match(r"[\w>]", text[i - 1]):
            i = _skip_balanced(text, i, "<", ">")
            continue
        out.append(text[i])
        i += 1
    return "".join(out)


def _declared_name(head: str) -> str | None:
    """The name a namespace-scope declaration introduces, or None."""
    head = re.sub(r"\[\[.*?\]\]", " ", head, flags=re.S)
    # leading macros (export, diagnostics), but not the ones that declare
    head = re.sub(
        r"^\s*(?:(?!QUANTITY_SPEC\b)[A-Z][A-Z0-9_]+\b(?:\s*\([^()]*\))?\s*)+", "", head
    )
    head = head.strip()
    # an operator first is the rest of a constraint cut short by a `requires { }`
    if not re.match(r"[\w\[]", head):
        return None
    while head.startswith("template"):
        start = head.find("<")
        if start < 0:
            return None
        head = head[_skip_balanced(head, start, "<", ">") :].strip()
    if head.startswith("requires"):
        # an unparsed constraint; the declaration starts at its first specifier
        found = re.search(
            r"\b(?:inline|constexpr|consteval|constinit|struct|class)\b", head
        )
        if not found:
            return None
        head = head[found.start() :]
    if re.match(
        r"(?:using\s+namespace|using\s+[\w:]+\s*$|friend|static_assert|extern|namespace)\b",
        head,
    ):
        return None
    if m := re.match(r"QUANTITY_SPEC\s*\(\s*(\w+)", head):
        return m.group(1)
    if m := re.match(r"concept\s+(\w+)", head):
        return m.group(1)
    if m := re.match(r"using\s+(\w+)\s*=", head):
        return m.group(1)
    if m := re.match(
        r"(?:(?:inline|constexpr|static)\s+)*(?:struct|class|union|enum(?:\s+(?:class|struct))?)\s+"
        r"(?:[A-Z][A-Z0-9_]+\s+)?(\w+)\b(?!\s*::)",  # an attribute macro may precede the name
        head,
    ):
        return m.group(1)
    flat = _strip_template_args(head)
    if "operator" in flat:
        return None
    eq, paren = flat.find("="), flat.find("(")
    if paren >= 0 and (eq < 0 or paren < eq):
        names = re.findall(r"\w+", flat[:paren])  # function
    else:
        names = re.findall(r"\w+", flat[:eq] if eq >= 0 else flat)  # variable
    if len(names) < 2:  # a name with no type in front of it is not a declaration
        return None
    return names[-1] if names[-1] not in _KEYWORDS else None


def _header_names(path: Path) -> set[str]:
    """Exported names declared at namespace scope, qualified below `mp_units`
    (`isq::speed`, `utility::cartesian_vector`, `QuantitySpec`)."""
    text = _strip_preprocessor(_CPP_NOISE.sub(" ", path.read_text(encoding="utf-8")))
    names: set[str] = set()
    # a namespace frame is (path, exported); a block frame is "body", or "expr"
    # for braces inside an unfinished declaration (`template<T t = T{}>`)
    stack: list = []
    stmt_start = 0
    export_block = False  # inside MP_UNITS_EXPORT_BEGIN ... MP_UNITS_EXPORT_END

    def at_namespace_scope() -> bool:
        return all(isinstance(frame, tuple) for frame in stack)

    def current() -> tuple[tuple[str, ...], bool]:
        return stack[-1] if stack else ((), False)

    def unfinished(head: str) -> bool:
        flat = head.replace("->", "")
        return flat.count("(") > flat.count(")") or flat.count("<") > flat.count(">")

    def record(head: str) -> None:
        ns, exported = current()
        if not ns or ns[0] != "mp_units" or "detail" in ns or "std" in ns:
            return
        if not (exported or export_block or re.search(r"\bMP_UNITS_EXPORT\b", head)):
            return
        if (name := _declared_name(head)) and not name.startswith("_"):
            names.add("::".join((*ns[1:], name)))

    token_re = r"(MP_UNITS_EXPORT\s+)?(inline\s+)?namespace\s+([\w:]+)?\s*\{|MP_UNITS_EXPORT_(?:BEGIN|END)|[{};]"
    for m in re.finditer(token_re, text):
        token = m.group(0)
        if token.startswith("MP_UNITS_EXPORT_"):
            export_block = token.endswith("BEGIN")
            if at_namespace_scope():
                stmt_start = m.end()
        elif token.endswith("{") and "namespace" in token:
            if at_namespace_scope():
                ns, exported = current()
                exported = exported or bool(m.group(1))
                # an inline namespace is transparent: its names are spelled without it
                parts = (
                    ()
                    if m.group(2)
                    else tuple(filter(None, (m.group(3) or "").split("::")))
                )
                stack.append(((*ns, *parts), exported))
            else:
                stack.append("body")
            stmt_start = m.end()
        elif token == "{":
            if not at_namespace_scope():
                stack.append("body")
            elif unfinished(head := text[stmt_start : m.start()]):
                stack.append("expr")
            else:
                record(head)
                stack.append("body")
        elif token == "}":
            if not stack:
                raise SystemExit(f"{path}: unbalanced braces")
            frame = stack.pop()
            if frame != "expr" and at_namespace_scope():
                stmt_start = m.end()
        elif at_namespace_scope():  # ';'
            record(text[stmt_start : m.start()])
            stmt_start = m.end()
    if stack:
        raise SystemExit(f"{path}: unbalanced braces")
    return names


def vocabulary() -> set[str]:
    names: set[str] = set()
    for header in _tracked(*HEADER_ROOTS):
        if header.suffix == ".h" and "/bits/" not in header.as_posix():
            names |= _header_names(header)
    return names


# ---------------------------------------------------------------------------
# Pages
# ---------------------------------------------------------------------------

_FENCE = re.compile(r"^(\s*)(`{3,}|~{3,})")
_SNIPPET = re.compile(r'^\s*-{2}8<-{2}\s+"([^"]+)"')
_HEADING = re.compile(r"^(#{1,6})\s+(.*?)\s*#*\s*$")
_INLINE_CODE = re.compile(r"(`+)(.+?)\1")


def _snippet(spec: str) -> str:
    """The text a `--8<-- "path:start:end"` line includes."""
    path, _, lines = spec.partition(":")
    file = ROOT / path
    if not file.is_file():
        return ""
    text = file.read_text(encoding="utf-8").splitlines()
    if m := re.fullmatch(r"(\d*):(\d*)", lines):
        start = int(m.group(1) or 1)
        end = int(m.group(2) or len(text))
        text = text[start - 1 : end]
    return "\n".join(text)


def _clean_inline(text: str) -> str:
    """Heading or paragraph markup down to plain words."""
    text = re.sub(r"\s*\{[#.][^}]*\}\s*$", "", text)  # attr_list
    text = re.sub(r"!?\[([^\]]*)\]\([^)]*\)", r"\1", text)  # links, images
    text = re.sub(r"\[([^\]]*)\]\[[^\]]*\]", r"\1", text)
    text = re.sub(r"<[^>]+>", "", text)
    text = re.sub(r"(\*\*|__|(?<!\w)[*_](?=\S))(.+?)\1", r"\2", text)
    return re.sub(r"\s+", " ", text).strip()


class Page:
    def __init__(self, path: Path):
        self.path = path.relative_to(DOCS).as_posix()
        self.title = ""
        self.headings: list[tuple[int, str]] = []
        self.summary = ""
        self.code: list[str] = []  # fenced blocks, included snippets, inline spans
        self._parse(path.read_text(encoding="utf-8"))

    def _parse(self, text: str) -> None:
        lines = text.splitlines()
        if lines and lines[0].strip() == "---":  # front matter
            end = next(
                (i for i, line in enumerate(lines[1:], 1) if line.strip() == "---"), 0
            )
            if m := re.search(
                r"(?m)^title:\s*['\"]?(.*?)['\"]?\s*$", "\n".join(lines[1:end])
            ):
                self.title = m.group(1)
            lines = lines[end + 1 :]

        fence: str | None = None
        block: list[str] = []
        paragraph: list[str] | None = None  # collected until the first heading after H1
        for line in lines:
            if fence is not None:
                if line.strip().startswith(fence):
                    self.code.append("\n".join(block))
                    fence, block = None, []
                elif snippet := _SNIPPET.match(line):
                    block.append(_snippet(snippet.group(1)))
                else:
                    block.append(line)
                continue
            if m := _FENCE.match(line):
                fence = m.group(2)
                continue
            if snippet := _SNIPPET.match(line):
                self.code.append(_snippet(snippet.group(1)))
                continue
            self.code += [m.group(2) for m in _INLINE_CODE.finditer(line)]
            if m := _HEADING.match(line):
                level, heading = len(m.group(1)), _clean_inline(m.group(2))
                if level == 1 and not self.title:
                    self.title = heading
                    paragraph = []
                elif level > 1:
                    self.headings.append((level, heading))
                    self._take_summary(paragraph)
                    paragraph = None
                continue
            if paragraph is not None:
                if line.strip():
                    paragraph.append(line)
                elif paragraph:
                    self._take_summary(paragraph)
                    paragraph = None
        self._take_summary(paragraph)

    def _take_summary(self, paragraph: list[str] | None) -> None:
        """Keep the paragraph opening the page only when it is prose: not an
        admonition, a list, a table, a figure or a markup directive."""
        if self.summary or not paragraph:
            return
        first = paragraph[0].lstrip()
        if re.match(r"(?:!!!|\?\?\?|[-*+] |\d+\. |\||<|!\[|\{|:)", first):
            return
        text = _clean_inline(" ".join(line.strip() for line in paragraph))
        sentences = re.split(r"(?<=[.!?])\s+(?=[A-Z`*])", text)
        while sentences and re.match(r"(?:Welcome|Thank you)\b[^.]*!", sentences[0]):
            sentences.pop(0)  # "Welcome to mp-units!" says nothing about the page
        text = " ".join(sentences[:2])
        if len(text.split()) >= 6 and not text.endswith(":"):
            self.summary = text

    def entities(self, vocab: set[str], unqualified: dict[str, str]) -> Counter[str]:
        found: Counter[str] = Counter()
        for code in self.code:
            for m in re.finditer(r"(?<![\w.>])((?:\w+::)*)(\w+)\b(?!\s*::)", code):
                qualifier, name = m.group(1), m.group(2)
                path = qualifier.removeprefix("::").removeprefix("mp_units::")
                if path.startswith("std::"):
                    continue
                if path:
                    if (entity := path + name) in vocab:
                        found[entity] += 1
                elif name in unqualified and not code[: m.start()].endswith(
                    ("->", ".")
                ):
                    found[unqualified[name]] += 1
        return found


# ---------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------


_PREAMBLE = [
    "<!-- Generated by scripts/docs_index.py; do not edit. -->",
    # one line per entity is what grep needs, and a heading such as
    # "1. Quick Start" is quoted verbatim, not numbered as a list
    "<!-- markdownlint-disable MD013 MD029 -->",
    "",
]


def generate() -> dict[Path, str]:
    vocab = vocabulary()
    # A bare name means a framework entity: the core, or utility after its
    # `using namespace`. Systems are spelled qualified in the docs, and their bare
    # unit symbols (`m`, `s`) would match every one-letter variable.
    unqualified = {name: name for name in vocab if "::" not in name}
    for name in vocab:
        if name.startswith("utility::") and name.count("::") == 1:
            unqualified.setdefault(name.removeprefix("utility::"), name)

    pages = [
        Page(path)
        for path in _tracked("docs")
        if path.suffix == ".md"
        and not path.name.startswith(".")
        and not path.relative_to(DOCS).as_posix().startswith(EXCLUDED_DOCS)
    ]
    users: defaultdict[str, list[tuple[int, str]]] = defaultdict(list)

    browse = [
        *_PREAMBLE,
        "# Documentation pages",
        "",
        "Each page under `docs/` (blog posts excepted) with its headings and the library",
        "entities its code uses, counted: uses in fenced blocks, inline code and included",
        "snippets, not in prose. To find the page for an entity, use `.index.md` instead.",
    ]
    for page in pages:
        found = page.entities(vocab, unqualified)
        ranked = sorted(found.items(), key=lambda item: (-item[1], item[0]))
        for entity, count in ranked:
            users[entity].append((count, page.path))
        browse += ["", f"## {page.path}", "", f"title: {page.title}"]
        if page.summary:
            browse.append(f"summary: {page.summary}")
        if ranked:
            browse.append("entities: " + ", ".join(f"`{e}` {c}" for e, c in ranked))
        if page.headings:
            top = min(level for level, _ in page.headings)
            browse += [
                "headings:",
                *(f"{'    ' * (level - top)}- {text}" for level, text in page.headings),
            ]

    lookup = [
        *_PREAMBLE,
        "# Documentation index",
        "",
        "For each library entity, the pages under `docs/` whose code uses it most, by count",
        "of uses in fenced blocks, inline code and included snippets (not prose). A page",
        "that uses an entity is not yet a page about it: read it before linking. Headings",
        "and summaries per page are in `.index-pages.md`.",
        "",
    ]
    for entity in sorted(users, key=str.casefold):
        ranked = sorted(users[entity], key=lambda item: (-item[0], item[1]))
        line = f"- `{entity}`: " + ", ".join(
            f"{path} {count}" for count, path in ranked[:LOOKUP_PAGES]
        )
        if len(ranked) > LOOKUP_PAGES:
            line += f" (+{len(ranked) - LOOKUP_PAGES} more)"
        lookup.append(line)

    return {
        LOOKUP: "\n".join(lookup) + "\n",
        PAGES: "\n".join(browse) + "\n",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument(
        "--check", action="store_true", help="verify only; fail if the index is stale"
    )
    args = parser.parse_args()

    stale = False
    for path, content in generate().items():
        if path.is_file() and path.read_text(encoding="utf-8") == content:
            continue
        stale = True
        if args.check:
            print(f"{path.relative_to(ROOT)} is stale", file=sys.stderr)
        else:
            path.write_text(content, encoding="utf-8")
            print(f"wrote {path.relative_to(ROOT)}")
    if args.check and stale:
        print("run: python3 scripts/docs_index.py", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
