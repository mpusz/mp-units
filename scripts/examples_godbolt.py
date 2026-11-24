"""ProperDocs hook that builds the "Try it live on Compiler Explorer" button of each example page.

`docs/examples/<name>.md` shows code from `example/<name>.cpp` (the file CMake builds and CI
compiles), but its button used to be a hand-made `godbolt.org/z/...` short link. A short link
freezes the source at the moment someone clicked "Share", so every API change left the button
running old code. This hook builds the link from the file at documentation build time instead,
as a Compiler Explorer URL, so the button always runs what the page shows.

The button stays one line in the page, and the hook only rewrites that line:

    [Try it live on Compiler Explorer](godbolt){ .md-button }

The target `godbolt` is a placeholder. A page that has no such line is left alone.
"""

from __future__ import annotations

import re
from pathlib import Path
from urllib.parse import quote_plus

ROOT = Path(__file__).resolve().parent.parent
BUTTON = re.compile(
    r"^\[Try it live on Compiler Explorer\]\(godbolt\)(\{ \.md-button \})$", re.M
)
LEADING_COMMENT = re.compile(r"\A\s*(?://[^\n]*\n\s*|/\*.*?\*/\s*)+", re.S)

# Toolchain of the button. Rules (the maintainer's):
#  - the compiler is always a concrete release, the newest one, never `trunk`: a trunk compiler is
#    slower on Compiler Explorer and the examples already hit its timeouts (GCC 16.2 times out on
#    storage_tank and foot_pound_second, Clang 23.1.0 builds all of them in time);
#  - no optimization (`-O0`): the output of an example does not depend on it, and `-O3` is what pushes the
#    slowest ones (foot_pound_second under GCC 16.2) over the timeout. Only a page that shows generated
#    assembly, like the zero-overhead demo, needs `-O3`;
#  - the warnings stay on (Clang 23.1.0 gives none for the examples), so an example that starts to warn shows it;
#  - the library is `trunk` on purpose: the examples are built against the current branch, not a release.
# Bump COMPILER and COMPILER_NAME together when Compiler Explorer gets a newer Clang release.
COMPILER = "clang2310"
COMPILER_NAME = "x86-64 clang 23.1.0"
OPTIONS = "-std=c++23 -stdlib=libc++ -O0 -Wall -Wextra -Wpedantic"

# What survives of a Rison string in the URL: `!` and `'` are Rison's own escapes and are kept literal.
_SAFE = "-_.~/:,()!'"


def _rison(value: str) -> str:
    return value.replace("!", "!!").replace("'", "!'")


def godbolt_url(source: str) -> str:
    """Compiler Explorer link with the layout the maintainers use: editor on the left, compiler and
    the Output pane (the program's output, wrapped) stacked on the right.

    A `clientstate` link would be shorter but carries no layout, so the Output pane would be missing.
    """
    code = quote_plus(_rison(source), safe=_SAFE)
    flags = quote_plus(_rison(OPTIONS), safe=_SAFE)
    # Booleans are Rison `!t` / `!f`. The `'0'` / `'1'` strings of older share links read as truthy here,
    # which switched the assembly to binary mode and left the program unexecuted.
    filters = (
        "binary:!f,binaryObject:!f,commentOnly:!t,debugCalls:!f,demangle:!t,directives:!t,"
        "execute:!t,intel:!t,labels:!t,libraryCode:!t,trim:!f,verboseDemangling:!t"
    )
    return (
        "https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:15,fontUsePx:!t,j:1,"
        f"lang:c%2B%2B,source:'{code}'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:50,l:'4',n:'0',o:'',"
        "s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:"
        + COMPILER
        + ",filters:("
        + filters
        + "),"
        "flagsViewOpen:!f,fontScale:14,fontUsePx:!t,j:1,lang:c%2B%2B,libs:!((name:mp-units,ver:trunk)),"
        f"options:'{flags}',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,"
        "positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,"
        "startLineNumber:1),source:1),l:'5',n:'0',o:'"
        + quote_plus(COMPILER_NAME, safe=_SAFE)
        + "+(Editor+%231)',"
        "t:'0')),header:(),k:50,l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'"
        + quote_plus(COMPILER_NAME, safe=_SAFE)
        + "',editorid:1,fontScale:14,fontUsePx:!t,j:1,wrap:!t),"
        "l:'5',n:'0',o:'Output+of+"
        + quote_plus(COMPILER_NAME, safe=_SAFE)
        + "+(Compiler+%231)',t:'0')),"
        "header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:50,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),"
        "version:4"
    )


def on_page_markdown(markdown, page, config, files):
    if not page.file.src_uri.startswith("examples/"):
        return markdown
    if not BUTTON.search(markdown):
        return markdown
    example = ROOT / "example" / f"{Path(page.file.src_uri).stem}.cpp"
    if not example.is_file():
        raise FileNotFoundError(
            f"{page.file.src_uri} has a Compiler Explorer button but {example} does not exist"
        )
    source = LEADING_COMMENT.sub(
        "", example.read_text(encoding="utf-8").replace("\r\n", "\n"), count=1
    )
    return BUTTON.sub(
        lambda m: f"[Try it live on Compiler Explorer]({godbolt_url(source)}){m.group(1)}",
        markdown,
    )
