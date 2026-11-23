"""ProperDocs hook that builds the "Try it live on Compiler Explorer" button of each example page.

`docs/examples/<name>.md` shows code from `example/<name>.cpp` (the file CMake builds and CI
compiles), but its button used to be a hand-made `godbolt.org/z/...` short link. A short link
freezes the source at the moment someone clicked "Share", so every API change left the button
running old code. This hook builds the link from the file at documentation build time instead,
as a Compiler Explorer "clientstate" URL, so the button always runs what the page shows.

The button stays one line in the page, and the hook only rewrites that line:

    [Try it live on Compiler Explorer](godbolt){ .md-button }

The target `godbolt` is a placeholder. A page that has no such line is left alone.
"""

from __future__ import annotations

import base64
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUTTON = re.compile(r"^\[Try it live on Compiler Explorer\]\(godbolt\)(\{ \.md-button \})$", re.M)
LEADING_COMMENT = re.compile(r"\A\s*(?://[^\n]*\n\s*|/\*.*?\*/\s*)+", re.S)

# Same toolchain the examples were verified with: the library from its trunk, run on the server.
COMPILER = "clang2110"
OPTIONS = "-std=c++23 -stdlib=libc++ -O3"
LIBRARY = {"name": "mp-units", "ver": "trunk"}


def clientstate_url(source: str) -> str:
    state = {
        "sessions": [
            {
                "id": 1,
                "language": "c++",
                "source": source,
                "compilers": [
                    {
                        "id": COMPILER,
                        "options": OPTIONS,
                        "filters": {"execute": True, "commentOnly": True, "directives": True, "labels": True},
                        "libs": [LIBRARY],
                    }
                ],
                "executors": [],
            }
        ]
    }
    payload = base64.urlsafe_b64encode(json.dumps(state).encode()).decode()
    return f"https://godbolt.org/clientstate/{payload}"


def on_page_markdown(markdown, page, config, files):
    if not page.file.src_uri.startswith("examples/"):
        return markdown
    if not BUTTON.search(markdown):
        return markdown
    example = ROOT / "example" / f"{Path(page.file.src_uri).stem}.cpp"
    if not example.is_file():
        raise FileNotFoundError(f"{page.file.src_uri} has a Compiler Explorer button but {example} does not exist")
    source = LEADING_COMMENT.sub("", example.read_text(encoding="utf-8").replace("\r\n", "\n"), count=1)
    return BUTTON.sub(lambda m: f"[Try it live on Compiler Explorer]({clientstate_url(source)}){m.group(1)}", markdown)
