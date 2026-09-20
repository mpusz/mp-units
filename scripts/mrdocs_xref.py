"""MkDocs hook for the MrDocs-generated API reference.

MrDocs writes ~5300 Markdown pages under `API_DIR`. Listing them all in `nav`
makes the sidebar unusable and Material renders the whole tree into every page,
so only the namespaces go into `nav` and each symbol page is grafted onto its
namespace for its own render. This hook does three things:

1. **Navigation.** Builds a `nav` subtree of namespaces and splices it in place
   of the committed `API Reference` entry, then, for each symbol page, grafts
   that page onto its namespace under the category heading it is listed beneath
   (Types, Functions, Concepts, ...).
2. **Cross-references.** Resolves `[text][mp_units::symbol]` against the tagfile
   MrDocs writes, so prose links into the reference by name:

       The [`quantity`][mp_units::quantity] class template ...

   Only symbols a page actually references get a definition, so an unresolved
   name stays visible as literal text instead of silently vanishing.
3. **Synopsis highlighting.** Colours keywords and comments in a synopsis. The
   block cannot go through Pygments because MrDocs has already linked the types
   in it, but the keywords are plain text between those links.

Link URLs are not handled here: the `mkdocs_url` Handlebars helper in the
generator emits MkDocs-style directory URLs directly, where the page paths are
known.
"""

from __future__ import annotations

import posixpath
import re
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from mkdocs.structure.nav import Link, Section

# Where `mrdocs` writes its output, relative to `docs_dir`.
API_DIR = "reference/api_reference/mrdocs"
TAGFILE = "reference.tag.xml"

# The hand-written page the `API Reference` entry points at, in preference to
# the generated one (the whole corpus flattened onto a single page).
LANDING = "reference/api_reference/overview.md"

# The committed `nav` entry the generated subtree replaces. It points at the
# landing page, which becomes the subtree's first child, so the committed nav
# still names a real file and `--strict` stays happy without this hook.
REPLACES = LANDING

# A reference-style link whose label looks like a qualified C++ name.
_REF = re.compile(r"\]\[([A-Za-z_][\w:]*(?:::[\w:~+\-*/%^&|!=<>\[\]()]+)+)\]")

# A category heading on a namespace page, and the member links beneath it. The
# generator emits MkDocs directory URLs, relative to where the page is *served*.
_HEADING = re.compile(r"^## (.+)$", re.MULTILINE)
_MEMBER_LINK = re.compile(r'href="([^":#?]+/)"')

# A synopsis block, and what is worth colouring inside one.
_SYNOPSIS = re.compile(
    r'(?s)(<pre class="mrdocs-synopsis"><code[^>]*>)(.*?)(</code></pre>)'
)
_ANCHOR = re.compile(r"(?s)<a\b.*?</a>")
_COMMENT = re.compile(r"/\*.*?\*/")
_KEYWORD = re.compile(
    r"\b(?:template|class|struct|union|enum|namespace|typename|concept|requires|"
    r"public|protected|private|friend|virtual|override|final|using|typedef|"
    r"auto|const|constexpr|consteval|constinit|static|inline|explicit|mutable|"
    r"volatile|noexcept|operator|return|decltype|sizeof|alignof|this|nullptr|"
    r"true|false|void|bool|char|char8_t|char16_t|char32_t|wchar_t|int|long|"
    r"short|signed|unsigned|float|double)\b"
)


@dataclass
class _Corpus:
    """What the hook learns once per build, from the generated output."""

    docs_dir: Path = Path()
    files: Any = None

    #: fully qualified symbol name -> page path, for prose cross-references
    xrefs: dict[str, str] = field(default_factory=dict)
    #: namespace page uri -> its fully qualified name
    namespaces: dict[str, str] = field(default_factory=dict)
    #: namespace page uri -> its category headings, in page order, with anchors
    categories: dict[str, list[tuple[str, str]]] = field(default_factory=dict)
    #: symbol page uri -> the category heading it is listed under
    category_of: dict[str, str] = field(default_factory=dict)
    #: namespace page uri -> the nav section that holds it
    sections: dict[str, Any] = field(default_factory=dict)

    def reset(self, docs_dir: str) -> None:
        self.docs_dir = Path(docs_dir)
        self.xrefs.clear()
        self.namespaces.clear()
        self.categories.clear()
        self.category_of.clear()
        self.sections.clear()

    def page_uri(self, base: str) -> str:
        """Where a scope's page lives.

        `run.sh` moves a namespace page inside its own directory, so that
        `navigation.indexes` folds it into its section instead of rendering the
        namespace twice. Everything else stays beside its directory.
        """
        nested = f"{base}/index.md"
        return nested if (self.docs_dir / nested).is_file() else f"{base}.md"

    def section_for(self, src_uri: str):
        """The nav section of the nearest enclosing namespace."""
        directory = posixpath.dirname(src_uri)
        while directory.startswith(API_DIR):
            section = self.sections.get(f"{directory}/index.md")
            if section is not None:
                return section
            directory = posixpath.dirname(directory)
        return None

    def namespace_uri_of(self, section) -> str:
        """The namespace page uri a nav section was indexed under."""
        for uri, candidate in self.sections.items():
            if candidate is section and uri in self.namespaces:
                return uri
        return ""


_corpus = _Corpus()


@dataclass
class _Graft:
    """Nav nodes added for the page currently rendering, undone on the next."""

    section: Any
    nodes: list[Any]
    pages: list[Any] = field(default_factory=list)

    def undo(self) -> None:
        for node in self.nodes:
            if node in self.section.children:
                self.section.children.remove(node)
        for page in self.pages:
            page.parent = None


_graft: _Graft | None = None


# --------------------------------------------------------------------------
# Reading the generated output
# --------------------------------------------------------------------------


def _load_tagfile(corpus: _Corpus) -> None:
    """Symbol names and namespaces, from the tagfile MrDocs writes."""
    path = corpus.docs_dir / API_DIR / TAGFILE
    if not path.is_file():
        return
    for compound in ET.parse(path).getroot().iter("compound"):
        name = compound.findtext("name")
        filename = compound.findtext("filename")
        if not name or not filename:
            continue
        # First definition wins: MrDocs emits the primary page first.
        corpus.xrefs.setdefault(name, filename)
        if compound.get("kind") == "namespace":
            base = f"{API_DIR}/{filename}"[: -len(".md")]
            corpus.namespaces[corpus.page_uri(base)] = name


def _slug(text: str) -> str:
    """The anchor Python-Markdown's `toc` gives a heading."""
    return re.sub(r"[^\w]+", "-", text.strip().lower()).strip("-")


def _load_categories(corpus: _Corpus) -> None:
    """The category each symbol is listed under, from its namespace page.

    MrDocs groups a namespace's members under `## Types`, `## Functions` and so
    on. That grouping is the one the sidebar mirrors, and it is already in the
    generated Markdown, so there is nothing to recompute.
    """
    for uri in corpus.namespaces:
        path = corpus.docs_dir / uri
        if not path.is_file():
            continue
        text = path.read_text(encoding="utf-8")
        # A page at `a/b/index.md` is served from `a/b/`, which is what its
        # relative links resolve against.
        here = posixpath.dirname(uri)
        headings = list(_HEADING.finditer(text))
        corpus.categories[uri] = [
            (h.group(1).strip(), _slug(h.group(1).strip())) for h in headings
        ]
        for index, heading in enumerate(headings):
            end = (
                headings[index + 1].start() if index + 1 < len(headings) else len(text)
            )
            for link in _MEMBER_LINK.finditer(text[heading.start() : end]):
                served = posixpath.normpath(posixpath.join(here, link.group(1)))
                corpus.category_of.setdefault(
                    corpus.page_uri(served), heading.group(1).strip()
                )


# --------------------------------------------------------------------------
# Building the navigation
# --------------------------------------------------------------------------


def _namespace_tree(corpus: _Corpus) -> list:
    """A `nav` subtree of namespaces, nested by qualified name.

    Two levels per namespace on purpose: an outer group, and inside it a section
    holding just the namespace page. `navigation.indexes` folds the page into
    that inner section, so the inner row renders as the page's own link and the
    page's categories - and the symbol branch grafted on per render - nest
    underneath it, while the child namespaces stay at the outer level.
    """
    tree: dict = {}
    for uri, name in sorted(corpus.namespaces.items(), key=lambda kv: kv[1]):
        node = tree
        entry: dict = {}
        for part in name.split("::"):
            entry = node.setdefault(part, {"page": None, "children": {}})
            node = entry["children"]
        entry["page"] = uri

    def render(node: dict) -> list:
        out = []
        for key in sorted(node, key=str.lower):
            entry = node[key]
            if not entry["page"]:
                continue
            inner = {key: [{key: entry["page"]}]}
            out.append({key: [inner] + render(entry["children"])})
        return out

    subtree = render(tree)
    landing = LANDING if (corpus.docs_dir / LANDING).is_file() else ""
    if not landing and (corpus.docs_dir / API_DIR / "index.md").is_file():
        landing = f"{API_DIR}/index.md"
    if landing:
        subtree.insert(0, {"Overview": landing})
    return subtree


def _splice(nav, subtree) -> bool:
    """Replace the committed API Reference entry in place. True if found."""
    if isinstance(nav, list):
        for index, item in enumerate(nav):
            if isinstance(item, dict) and len(item) == 1:
                ((key, value),) = item.items()
                if value == REPLACES:
                    nav[index] = {key: subtree}
                    return True
            if _splice(item, subtree):
                return True
    elif isinstance(nav, dict):
        return any(_splice(value, subtree) for value in nav.values())
    return False


# --------------------------------------------------------------------------
# Grafting a symbol page onto its namespace
# --------------------------------------------------------------------------


def _page_url(uri: str) -> str:
    """A page's URL relative to the site root, in MkDocs directory form.

    mkdocs resolves a nav `Link` URL against the site root and makes it
    page-relative itself, so handing it an already-relative path resolves
    against the wrong base and lands on the site root.
    """
    if posixpath.basename(uri) == "index.md":
        served = posixpath.dirname(uri)
    else:
        served = uri[: -len(".md")] if uri.endswith(".md") else uri
    return f"{served}/" if served else ""


def _relative_to(title: str, scope: str) -> str:
    """A name with its enclosing scope stripped, for a nested sidebar entry."""
    return (
        title[len(scope) + 2 :] if scope and title.startswith(f"{scope}::") else title
    )


def _ancestor_chain(corpus: _Corpus, src_uri: str, ns_uri: str) -> list[str]:
    """Pages from just below the namespace down to `src_uri`."""
    ns_dir = posixpath.dirname(ns_uri)
    if not src_uri.startswith(f"{ns_dir}/"):
        return []
    parts = [part for part in src_uri[len(ns_dir) + 1 :].split("/") if part]
    if parts and parts[-1].endswith(".md"):
        parts[-1] = parts[-1][: -len(".md")]
    if parts and parts[-1] == "index":
        parts.pop()
    return [
        corpus.page_uri(f"{ns_dir}/{'/'.join(parts[:depth])}")
        for depth in range(1, len(parts) + 1)
    ]


def _wrap_in_ancestors(corpus: _Corpus, page, chain: list[str], scope: str):
    """Nest the page inside a section per enclosing symbol.

    `mp_units::quantity::in` then reads as `quantity` -> `in` rather than as a
    single entry beside the namespace's own categories.
    """
    node: Any = page
    touched = [page]
    for ancestor_uri in reversed(chain[:-1]):
        found = corpus.files.get_file_from_path(ancestor_uri) if corpus.files else None
        ancestor = getattr(found, "page", None)
        if ancestor is None:
            continue
        ancestor.title = _relative_to(ancestor.title or "", scope) or ancestor.title
        node.title = _relative_to(getattr(node, "title", "") or "", ancestor.title)
        wrapper = Section(title=ancestor.title, children=[ancestor, node])
        ancestor.parent = wrapper
        node.parent = wrapper
        touched.append(ancestor)
        node = wrapper
    return node, touched


def _category_nodes(corpus: _Corpus, ns_uri: str, active: str, node) -> list:
    """The namespace's whole category list, with `active` holding the page.

    The other categories come from the namespace page's own table of contents,
    which Material renders only while that page is active, so on a symbol page
    they would otherwise disappear.
    """
    headings = corpus.categories.get(ns_uri, [])
    if not headings or not active:
        return [node]
    base = _page_url(ns_uri)
    return [
        node if heading == active else Link(title=heading, url=f"{base}#{anchor}")
        for heading, anchor in headings
    ]


def _insert(section, nodes: list, pages: list) -> _Graft:
    """Add nodes after the section's own page, above the child namespaces."""
    position = (
        1 if section.children and getattr(section.children[0], "is_page", False) else 0
    )
    for offset, node in enumerate(nodes):
        node.parent = section
        section.children.insert(position + offset, node)
    return _Graft(section=section, nodes=nodes, pages=pages)


# --------------------------------------------------------------------------
# Rendering
# --------------------------------------------------------------------------


def _highlight(markup: str) -> str:
    """Colour keywords and comments in a synopsis, leaving links untouched."""

    def wrap(text: str) -> str:
        text = _KEYWORD.sub(r'<span class="mrdocs-k">\g<0></span>', text)
        return _COMMENT.sub(r'<span class="mrdocs-c">\g<0></span>', text)

    def one(match: re.Match) -> str:
        opening, body, closing = match.groups()
        parts = []
        last = 0
        for anchor in _ANCHOR.finditer(body):
            parts.append(wrap(body[last : anchor.start()]))
            parts.append(anchor.group(0))
            last = anchor.end()
        parts.append(wrap(body[last:]))
        return opening + "".join(parts) + closing

    return _SYNOPSIS.sub(one, markup)


def _definitions(markdown: str, src_uri: str) -> str:
    """Markdown link definitions for the symbols this page references."""
    wanted = {m.group(1) for m in _REF.finditer(markdown)}
    resolved = {name: _corpus.xrefs[name] for name in wanted if name in _corpus.xrefs}
    if not resolved:
        return ""
    here = posixpath.dirname(src_uri)
    return "\n".join(
        f"[{name}]: {posixpath.relpath(posixpath.join(API_DIR, target), here or '.')}"
        for name, target in sorted(resolved.items())
    )


# --------------------------------------------------------------------------
# MkDocs events
# --------------------------------------------------------------------------


def on_config(config, **kwargs):
    _corpus.reset(config["docs_dir"])
    if not (_corpus.docs_dir / API_DIR).is_dir():
        return config

    _load_tagfile(_corpus)
    _load_categories(_corpus)
    print(
        f"INFO    -  mrdocs: {len(_corpus.xrefs)} symbols, "
        f"{len(_corpus.namespaces)} namespaces"
    )

    if config.get("nav") and not _splice(config["nav"], _namespace_tree(_corpus)):
        print(f"WARNING -  mrdocs: no '{REPLACES}' entry found in nav")
    return config


def on_files(files, config, **kwargs):
    _corpus.files = files
    return files


def on_nav(nav, config, files, **kwargs):
    """Index the namespace sections so symbol pages can be attached to them."""
    _corpus.sections.clear()

    def walk(items):
        for item in items:
            if not getattr(item, "is_section", False):
                continue
            for child in item.children:
                # `navigation.indexes`: the section's own page leads it.
                is_page = getattr(child, "is_page", False)
                if is_page and child.file.src_uri.startswith(API_DIR):
                    _corpus.sections[child.file.src_uri] = item
                    break
            walk(item.children)

    walk(nav.items)
    return nav


def on_page_context(context, page, config, nav, **kwargs):
    """Give the page its namespace's sidebar, for this render only.

    A page outside `nav` has no parent, so mkdocs never marks anything active
    and Material renders a bare sidebar. Grafting it on just before its template
    renders fixes that without putting 5300 entries in `nav`; mkdocs resets
    `active` in a `finally` after each render, and the graft is undone when the
    next page starts.
    """
    global _graft

    if _graft is not None:
        _graft.undo()
        _graft = None

    src_uri = page.file.src_uri
    if not src_uri.startswith(f"{API_DIR}/"):
        return context

    section = _corpus.section_for(src_uri)
    if section is None:
        return context

    # A namespace page: list its own categories. `navigation.indexes` folds this
    # page into its section, and Material renders the integrated table of
    # contents only for a standalone page item, so they would otherwise vanish.
    if src_uri in _corpus.categories:
        base = _page_url(src_uri)
        _graft = _insert(
            section,
            [
                Link(title=heading, url=f"{base}#{anchor}")
                for heading, anchor in _corpus.categories[src_uri]
            ],
            [],
        )
        return context

    if page.parent is not None:
        return context

    ns_uri = _corpus.namespace_uri_of(section)
    scope = _corpus.namespaces.get(ns_uri, "")
    page.title = _relative_to(page.title or "", scope) or page.title

    chain = _ancestor_chain(_corpus, src_uri, ns_uri) if ns_uri else []
    node, touched = _wrap_in_ancestors(_corpus, page, chain, scope)

    category = _corpus.category_of.get(chain[0] if chain else src_uri)
    if category:
        node = Section(title=category, children=[node])
        node.children[0].parent = node

    _graft = _insert(section, _category_nodes(_corpus, ns_uri, category, node), touched)
    # Re-assert: mkdocs set this before the page had a parent, so the activation
    # never propagated up the tree.
    page.active = True
    return context


def on_page_markdown(markdown: str, page, config, files, **kwargs):
    src_uri = page.file.src_uri
    if src_uri.startswith(f"{API_DIR}/"):
        markdown = _highlight(markdown)

    definitions = _definitions(markdown, src_uri)
    return f"{markdown}\n\n{definitions}\n" if definitions else markdown
