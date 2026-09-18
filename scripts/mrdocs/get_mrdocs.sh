#!/bin/bash
# Print the path to the pinned MrDocs binary, downloading it if necessary.
#
# CI and contributors get the same build from the same place, so the generated
# pages do not depend on whatever happens to be installed locally.
#
# Resolution order:
#   1. $MRDOCS, if set (an explicit override always wins)
#   2. a previously cached download of the pinned version
#   3. `mrdocs` on PATH, if it is exactly the pinned version
#   4. download the pinned release into the cache
#
# The cache lives under $XDG_CACHE_HOME (or ~/.cache), shared across worktrees,
# and $MRDOCS_CACHE_DIR overrides it.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=mrdocs_version.sh disable=SC1091
source "$HERE/mrdocs_version.sh"

if [ -n "${MRDOCS:-}" ]; then
    echo "$MRDOCS"
    exit 0
fi

CACHE_ROOT="${MRDOCS_CACHE_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/mp-units/mrdocs}"
PREFIX="$CACHE_ROOT/$MRDOCS_VERSION"
BINARY="$PREFIX/bin/mrdocs"

if [ -x "$BINARY" ]; then
    echo "$BINARY"
    exit 0
fi

# An already-installed mrdocs is fine, but only at the pinned version.
if command -v mrdocs >/dev/null 2>&1; then
    installed="$(mrdocs --version 2>/dev/null | sed -n 's/^Release: *//p' | head -1)"
    if [ "$installed" = "$MRDOCS_VERSION" ]; then
        command -v mrdocs
        exit 0
    fi
fi

case "$(uname -s)" in
    Linux)  platform="Linux" ;;
    Darwin) platform="Darwin" ;;
    *)
        echo "error: no pinned MrDocs build for $(uname -s)." >&2
        echo "       Install MrDocs $MRDOCS_VERSION and set MRDOCS to its path." >&2
        exit 1
        ;;
esac

expected="MRDOCS_SHA256_$platform"
expected="${!expected}"
archive="MrDocs-$MRDOCS_VERSION-$platform.tar.xz"
url="https://github.com/cppalliance/mrdocs/releases/download/$MRDOCS_VERSION/$archive"

# Everything below is progress reporting, so it must not pollute stdout: the
# caller reads that as the binary path.
{
    echo "Downloading MrDocs $MRDOCS_VERSION for $platform..."
    echo "  $url"
} >&2

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

curl -fsSL -o "$tmp/$archive" "$url"

actual="$(sha256sum "$tmp/$archive" 2>/dev/null | cut -d' ' -f1 \
    || shasum -a 256 "$tmp/$archive" | cut -d' ' -f1)"
if [ "$actual" != "$expected" ]; then
    echo "error: checksum mismatch for $archive" >&2
    echo "       expected $expected" >&2
    echo "       actual   $actual" >&2
    exit 1
fi

mkdir -p "$tmp/unpacked"
tar -xJf "$tmp/$archive" -C "$tmp/unpacked" --strip-components=1
mkdir -p "$(dirname "$PREFIX")"
# Move into place as one step, so a cancelled download leaves no half-cache.
rm -rf "$PREFIX"
mv "$tmp/unpacked" "$PREFIX"

echo "Installed to $PREFIX" >&2
echo "$BINARY"
