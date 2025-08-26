#!/bin/bash

set -euo pipefail

# Script to build API reference documentation
# Usage: ./api_reference.sh [-s] [-d DIR] [-h]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DEFAULT_DEPS_DIR="$(dirname "$PROJECT_ROOT")/api_reference_deps"

# Default values
BUILD_DOCS=true
DEPS_DIR="$DEFAULT_DEPS_DIR"

show_usage() {
    echo "Usage: $0 [-s] [-d DIR] [-h]"
    echo ""
    echo "Generate API reference documentation from C++ source code."
    echo "By default, the script performs both setup (CMake configuration) and build."
    echo ""
    echo "Options:"
    echo "  -s            Setup only (skip building the documentation)"
    echo "  -d DIR        Directory for dependencies (default: $DEFAULT_DEPS_DIR)"
    echo "                This directory will contain cloned repositories:"
    echo "                - jegp.cmake_modules: CMake modules for documentation"
    echo "                - draft: C++ standard draft sources"
    echo "                - cxxdraft-htmlgen: HTML generation tools"
    echo "  -h            Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                     # Setup and build documentation (default)"
    echo "  $0 -s                  # Setup only, skip build"
    echo "  $0 -d /tmp/api_deps    # Use custom dependency directory"
    echo "  $0 -s -d ./deps        # Setup only with custom directory"
}

# Parse command line arguments using getopts
while getopts :hsd: opt; do
    case $opt in
        h)
            show_usage
            exit 0
            ;;
        s)
            BUILD_DOCS=false
            ;;
        d)
            DEPS_DIR="$OPTARG"
            ;;
        \?)
            echo "Error: Invalid option -$OPTARG" >&2
            show_usage
            exit 1
            ;;
        :)
            echo "Error: Option -$OPTARG requires an argument" >&2
            show_usage
            exit 1
            ;;
    esac
done

shift $((OPTIND - 1))

echo "Setting up API reference documentation..."
echo "Dependencies directory: $DEPS_DIR"
echo "Project root: $PROJECT_ROOT"

# Create dependencies directory if it doesn't exist
if [[ ! -d "$DEPS_DIR" ]]; then
    echo "Creating dependencies directory: $DEPS_DIR"
    mkdir -p "$DEPS_DIR"
else
    echo "Dependencies directory already exists: $DEPS_DIR"
fi

# Clone repositories if they don't already exist
clone_if_missing() {
    local repo_url="$1"
    local branch="$2"
    local target_dir="$3"
    local repo_name=$(basename "$repo_url" .git)
    local full_path="$target_dir/$repo_name"

    if [[ ! -d "$full_path" ]]; then
        echo "Cloning $repo_name..."
        if [[ "$branch" == "default" ]]; then
            git clone "$repo_url" --depth=1 "$full_path"
        else
            git clone "$repo_url" --branch "$branch" --depth=1 "$full_path"
        fi
    else
        echo "Repository $repo_name already exists at $full_path"
    fi
}

# Clone required repositories
clone_if_missing "https://github.com/JohelEGP/jegp.cmake_modules.git" "default" "$DEPS_DIR"
clone_if_missing "https://github.com/JohelEGP/draft.git" "standardese_sources_base" "$DEPS_DIR"
clone_if_missing "https://github.com/JohelEGP/cxxdraft-htmlgen.git" "standardese_sources_base" "$DEPS_DIR"

# Configure CMake
echo "Configuring CMake for API reference documentation..."
BUILD_DIR="$PROJECT_ROOT/build/docs/api_reference"
SOURCE_DIR="$PROJECT_ROOT/docs/api_reference/src"

if [[ ! -d "$SOURCE_DIR" ]]; then
    echo "Error: Source directory not found: $SOURCE_DIR" >&2
    exit 1
fi

cmake -S "$SOURCE_DIR" \
      -B "$BUILD_DIR" \
      -DCMAKE_MODULE_PATH="$DEPS_DIR/jegp.cmake_modules/modules" \
      -DJEGP_STANDARDESE_SOURCES_GIT_REPOSITORY="$DEPS_DIR/draft" \
      -DJEGP_CXXDRAFT_HTMLGEN_GIT_REPOSITORY="$DEPS_DIR/cxxdraft-htmlgen"

echo "CMake configuration completed successfully."

# Create symlink for generated HTML documentation
# This allows accessing the generated docs via docs/api_reference/gen
echo "Creating symlink for generated documentation..."
SYMLINK_TARGET="$PROJECT_ROOT/docs/api_reference/gen"
SYMLINK_SOURCE="${BUILD_DIR}/mp-units.html"

# Ensure the parent directory exists
mkdir -p "$(dirname "$SYMLINK_TARGET")"

# Create the symlink (remove existing one if present)
if ln -sf "$SYMLINK_SOURCE" "$SYMLINK_TARGET"; then
    echo "Documentation symlink created: $SYMLINK_TARGET -> $SYMLINK_SOURCE"
else
    echo "Warning: Failed to create documentation symlink" >&2
fi

# Build documentation if requested
if [[ "$BUILD_DOCS" == true ]]; then
    echo "Building API reference documentation..."

    # Set proper locale environment for the build
    export LC_ALL=C.UTF-8
    export LANG=C.UTF-8

    # Create log file with timestamp
    LOG_FILE="$BUILD_DIR/api_reference_build_$(date +%Y%m%d_%H%M%S).log"
    echo "Build output will be logged to: $LOG_FILE"

    # Use tee to show output on console and save to log file
    cmake --build "$BUILD_DIR" 2>&1 | tee "$LOG_FILE"

    # Check if build was successful
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
        echo "API reference documentation build completed successfully."
    else
        echo "API reference documentation build failed. Check the log file: $LOG_FILE"
        exit 1
    fi
else
    echo "To build the documentation only (without setup), run:"
    echo "  cmake --build $BUILD_DIR"
    echo "Or use: $0 (without -s option)"
fi

echo "API reference setup completed successfully."
