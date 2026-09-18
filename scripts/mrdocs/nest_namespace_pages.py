"""Move each namespace's page inside its own directory.

MrDocs writes a namespace's page *beside* its directory (`mp_units.md` next to
`mp_units/`). Material's `navigation.indexes` only folds a page into its section
when the page is `index.md`, so left as-is every namespace appears twice in the
sidebar: a section `mp_units` containing a page `mp_units`.

Moving the page inside its directory fixes that and changes no URL, because
MkDocs serves `a/b.md` and `a/b/index.md` from the same `a/b/`.

Namespaces only, which the tagfile identifies. Doing the same to a class page
would make Material absorb it into whichever section the MkDocs hook nests it
under, hiding the class's own members.
"""

from __future__ import annotations

import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def main(root: Path) -> int:
    tagfile = root / "reference.tag.xml"
    if not tagfile.is_file():
        print(f"no tagfile at {tagfile}", file=sys.stderr)
        return 1

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

    print(f"nested {moved} namespace pages")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(Path(sys.argv[1])))
