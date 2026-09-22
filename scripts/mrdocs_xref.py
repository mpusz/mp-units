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

import importlib.util
import posixpath
import re
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from mkdocs.structure.nav import Link, Section
from mkdocs.utils import get_relative_url

# Where `mrdocs` writes its output, relative to `docs_dir`.
API_DIR = "reference/api_reference/mrdocs"
TAGFILE = "reference.tag.xml"
# The heading MrDocs gives a namespace page's list of child namespaces. The
# nav nests the child namespaces under it, so that category is a section
# already and must not also be added as a plain link.
NAMESPACES = "Namespaces"

# The hand-written page the `API Reference` entry points at, in preference to
# the generated one (the whole corpus flattened onto a single page).
LANDING = "reference/api_reference/overview.md"

# The committed `nav` entry the generated subtree replaces. It points at the
# landing page, which becomes the subtree's first child, so the committed nav
# still names a real file and `--strict` stays happy without this hook.
REPLACES = LANDING

# A reference-style link whose label looks like a qualified C++ name.
#
# No `:` in either character class, so the only way across a `::` is the group
# and any label has one parse. Letting both match it made the two ways of
# splitting `::` ambiguous, and an unmatched label cost twice as much per
# segment while the engine tried them all.
_REF = re.compile(r"\]\[([A-Za-z_]\w*(?:::[\w~+\-*/%^&|!=<>\[\]()]+)+)\]")

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

        run.py moves a namespace page inside its own directory, so that
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
    opened: list[Any] = field(default_factory=list)

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

    One level per namespace, its page leading the section. `navigation.indexes`
    folds that page in, so the row is a link to the namespace with a chevron
    beside it, and the page's categories - and the symbol branch grafted on per
    render - nest underneath.

    The child namespaces go under a `Namespaces` section rather than beside the
    categories, because otherwise a namespace is in the sidebar twice: once in
    the category listing it belongs to and once as a sibling of that listing.
    It also makes every category behave alike, each being an anchor into the
    page and the container of its own members.
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
            children = render(entry["children"])
            items = [entry["page"]]
            if children:
                items.append({NAMESPACES: children})
            out.append({key: items})
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
    nodes = []
    for heading, anchor in headings:
        if heading == NAMESPACES:
            continue
        link = Link(title=heading, url=f"{base}#{anchor}")
        if heading != active:
            nodes.append(link)
            continue
        # Without this the category holding the open page is the one row in
        # the sidebar that cannot be clicked: it is a section, and Material
        # renders a section as a single toggle, so clicking the name collapses
        # the branch being read rather than returning to the listing.
        #
        # `navigation.indexes` splits that row in two, a link to the section's
        # index page and a separate chevron that folds it, which is the
        # behaviour wanted here. Material picks the index by looking for a
        # child with a true `is_index`, takes only its `url`, titles the row
        # from the section itself, and drops it from the child list. A link to
        # the category's heading answers all of that, so the category gets an
        # index page's behaviour without a page existing to be its index.
        link.is_index = True
        link.parent = node
        node.children.insert(0, link)
        nodes.append(node)
    return nodes


def _set_active(section, value: bool) -> None:
    """Set a section's active flag without touching its ancestors.

    `Section.active` is a property that assigns to the parent as well, so
    clearing it again would switch off the whole chain above it.
    """
    section._Section__active = value


def _open_namespaces(section) -> list:
    """Let the namespaces list open on a page that is not inside it.

    `navigation.prune` renders an inactive branch as a plain link, and the
    chevron Material draws beside it is decorative, so on a namespace's own
    page the `Namespaces` row could only be followed and not opened. From a
    page inside a child namespace it behaves, because then it is on the
    active path, which is why it starts refusing and later works.
    """
    opened = []
    for child in section.children:
        if getattr(child, "holds_namespaces", False) and not child.active:
            _set_active(child, True)
            opened.append(child)
    return opened


def _insert(section, nodes: list, pages: list) -> _Graft:
    """Add nodes after the section's own page and its child namespaces."""
    position = 0
    for child in section.children:
        if getattr(child, "is_page", False) or getattr(
            child, "holds_namespaces", False
        ):
            position += 1
        else:
            break
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


def _generate_api_reference(docs_dir: Path) -> None:
    """Regenerate the MrDocs output, unless nothing it depends on changed.

    Loaded by path rather than imported, because scripts/ is not a package and
    MkDocs runs a hook from wherever it was launched.

    This has to happen in `on_config` rather than the more natural
    `on_pre_build`: MkDocs fires `on_config` first, and the tagfile is read
    below.
    """
    run_py = docs_dir.parent / "scripts" / "mrdocs" / "run.py"
    if not run_py.is_file():
        return

    spec = importlib.util.spec_from_file_location("mrdocs_run", run_py)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)

    if module.generate_if_needed() != 0:
        raise SystemExit("mrdocs: generating the API reference failed")


def on_config(config, **kwargs):
    _corpus.reset(config["docs_dir"])
    _generate_api_reference(_corpus.docs_dir)
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

    # The child namespaces are nested under a `Namespaces` section, so that
    # section stands in for the category of the same name. Point it at the
    # heading it replaces: Material takes a section's index from a child with
    # a true `is_index` and leaves that child out of the list, so the row
    # becomes a link to the listing with a chevron that opens the namespaces.
    for uri, section in _corpus.sections.items():
        if uri not in _corpus.namespaces:
            continue
        anchor = next(
            (
                a
                for heading, a in _corpus.categories.get(uri, [])
                if heading == NAMESPACES
            ),
            "",
        )
        for child in section.children:
            if getattr(child, "is_section", False) and child.title == NAMESPACES:
                child.holds_namespaces = True
                if anchor and not any(
                    getattr(item, "is_index", False) for item in child.children
                ):
                    link = Link(title=NAMESPACES, url=f"{_page_url(uri)}#{anchor}")
                    link.is_index = True
                    link.parent = child
                    child.children.insert(0, link)
                break

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
                if heading != NAMESPACES
            ],
            [],
        )
        _graft.opened = _open_namespaces(section)
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
    _graft.opened = _open_namespaces(section)
    # Re-assert: mkdocs set this before the page had a parent, so the activation
    # never propagated up the tree.
    page.active = True
    return context


# Material decides "is this branch pruned" and "is it open" from the one
# `active` flag, so a section cannot be both kept and closed. `_open_namespaces`
# sets the flag to keep the row expandable; this takes the open state back off
# the rendered page, leaving the toggle itself untouched so it still opens.


def _collapse(output: str, href: str) -> str:
    """Uncheck the toggle belonging to the row that links to `href`.

    Keyed on the row's own link rather than on its position, because a page
    inside a namespace renders its parent's namespaces row too, and collapsing
    that one would hide the branch the reader is in.

    Matches nothing if Material ever renders this differently, in which case
    the row stays open as it did before.
    """
    pattern = re.compile(
        r"<input(?P<head>[^>]*md-nav__toggle[^>]*?)\s+checked(?P<tail>[^>]*)>"
        r"(?P<gap>(?:(?!<input\b)[\s\S]){0,800}?"
        r'<a href="' + re.escape(href) + r'" class="md-nav__link)'
    )
    return pattern.sub(r"<input\g<head>\g<tail>>\g<gap>", output, count=1)


def on_post_page(output: str, page, config, **kwargs):
    if not (_graft and _graft.opened):
        return output
    for section in _graft.opened:
        index = next(
            (child for child in section.children if getattr(child, "is_index", False)),
            None,
        )
        if index is not None:
            output = _collapse(output, get_relative_url(index.url, page.url))
        # Cleared here rather than when the graft is undone. mkdocs marks the
        # next page active before calling `on_page_context`, so clearing it
        # there would undo an activation it had just propagated, and a section
        # the reader is actually inside would come out pruned.
        _set_active(section, False)
    _graft.opened = []
    return output


def on_page_markdown(markdown: str, page, config, files, **kwargs):
    src_uri = page.file.src_uri
    if src_uri.startswith(f"{API_DIR}/"):
        markdown = _highlight(markdown)

    definitions = _definitions(markdown, src_uri)
    return f"{markdown}\n\n{definitions}\n" if definitions else markdown
