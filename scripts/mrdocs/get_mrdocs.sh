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
# Linux, macOS, and Windows under a POSIX shell (Git Bash, MSYS2, Cygwin).
# The cache is shared across worktrees; $MRDOCS_CACHE_DIR overrides its
# location.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=mrdocs_version.sh disable=SC1091
source "$HERE/mrdocs_version.sh"

if [ -n "${MRDOCS:-}" ]; then
    echo "$MRDOCS"
    exit 0
fi

case "$(uname -s)" in
    Linux)                    platform="Linux";  archive_ext="tar.xz"; exe="" ;;
    Darwin)                   platform="Darwin"; archive_ext="tar.xz"; exe="" ;;
    MINGW* | MSYS* | CYGWIN*) platform="win64";  archive_ext="zip";    exe=".exe" ;;
    *)
        echo "error: no pinned MrDocs build for $(uname -s)." >&2
        echo "       Install MrDocs $MRDOCS_VERSION and set MRDOCS to its path." >&2
        exit 1
        ;;
esac

# Windows has no XDG convention; per-user caches belong under LOCALAPPDATA,
# which arrives as a native path and has to be converted for a POSIX shell.
default_cache="${XDG_CACHE_HOME:-$HOME/.cache}"
if [ "$platform" = "win64" ] && [ -n "${LOCALAPPDATA:-}" ]; then
    if command -v cygpath >/dev/null 2>&1; then
        default_cache="$(cygpath -u "$LOCALAPPDATA")"
    else
        default_cache="${LOCALAPPDATA//\\//}"
    fi
fi

CACHE_ROOT="${MRDOCS_CACHE_DIR:-$default_cache/mp-units/mrdocs}"
PREFIX="$CACHE_ROOT/$MRDOCS_VERSION"
BINARY="$PREFIX/bin/mrdocs$exe"

if [ -x "$BINARY" ]; then
    echo "$BINARY"
    exit 0
fi

# An already-installed mrdocs is fine, but only at the pinned version: silently
# generating with whatever is installed is how a published site stops being
# reproducible.
if command -v "mrdocs$exe" >/dev/null 2>&1; then
    installed="$("mrdocs$exe" --version 2>/dev/null | sed -n 's/^Release: *//p' | head -1)"
    if [ "$installed" = "$MRDOCS_VERSION" ]; then
        command -v "mrdocs$exe"
        exit 0
    fi
fi

expected="MRDOCS_SHA256_$platform"
expected="${!expected}"
archive="MrDocs-$MRDOCS_VERSION-$platform.$archive_ext"
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

if command -v sha256sum >/dev/null 2>&1; then
    actual="$(sha256sum "$tmp/$archive" | cut -d' ' -f1)"
elif command -v shasum >/dev/null 2>&1; then
    actual="$(shasum -a 256 "$tmp/$archive" | cut -d' ' -f1)"
else
    actual="$(powershell.exe -NoProfile -Command \
        "(Get-FileHash -Algorithm SHA256 -LiteralPath '$(cygpath -w "$tmp/$archive")').Hash.ToLower()" \
        | tr -d '\r')"
fi

if [ "$actual" != "$expected" ]; then
    echo "error: checksum mismatch for $archive" >&2
    echo "       expected $expected" >&2
    echo "       actual   $actual" >&2
    exit 1
fi

# Both archives hold a single top-level directory, named after MrDocs' internal
# version rather than the release tag, so strip it rather than assuming a name.
mkdir -p "$tmp/unpacked"
if [ "$archive_ext" = "zip" ]; then
    if command -v unzip >/dev/null 2>&1; then
        unzip -qq "$tmp/$archive" -d "$tmp/zip"
    else
        powershell.exe -NoProfile -Command \
            "Expand-Archive -LiteralPath '$(cygpath -w "$tmp/$archive")' \
             -DestinationPath '$(cygpath -w "$tmp/zip")' -Force" >/dev/null
    fi
    mv "$tmp"/zip/*/* "$tmp/unpacked/"
else
    tar -xJf "$tmp/$archive" -C "$tmp/unpacked" --strip-components=1
fi

mkdir -p "$(dirname "$PREFIX")"
# Move into place as one step, so a cancelled download leaves no half-cache.
rm -rf "$PREFIX"
mv "$tmp/unpacked" "$PREFIX"

echo "Installed to $PREFIX" >&2
echo "$BINARY"
