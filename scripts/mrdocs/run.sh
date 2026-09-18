#!/bin/bash
# Regenerate the API reference under docs/reference/api_reference/mrdocs/.
#
# The pinned MrDocs build is downloaded and cached on first use; set MRDOCS to
# use a different binary. See get_mrdocs.sh and mrdocs_version.sh.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Source links must point at the exact commit the pages were generated from.
# The line numbers come from the working tree, so pinning the URL to a moving
# ref like `master` makes every link drift as soon as the file changes. Prefer
# an exact tag (so released docs link to released code), else the commit SHA.
SOURCE_REF="${MRDOCS_SOURCE_REF:-$(git -C "$HERE" describe --tags --exact-match 2>/dev/null \
    || git -C "$HERE" rev-parse HEAD)}"

if [ -n "$(git -C "$HERE" status --porcelain -- "$HERE/../../src")" ]; then
    echo "warning: src/ has uncommitted changes; source links will point at" >&2
    echo "         $SOURCE_REF and their line numbers will not match." >&2
fi

rm -rf "$HERE/../../docs/reference/api_reference/mrdocs"
cd "$HERE"
MRDOCS_BIN="$("$HERE/get_mrdocs.sh")"

"$MRDOCS_BIN" --config=./mrdocs.yml \
    --base-url="https://github.com/mpusz/mp-units/blob/$SOURCE_REF/src/" "$@"

# Namespace pages become `index.md`; see the script for why, and why only
# namespaces.
python3 "$HERE/nest_namespace_pages.py" "$HERE/../../docs/reference/api_reference/mrdocs"
