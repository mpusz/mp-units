#!/bin/bash

# Validates that the mp-units Docker image has all required prerequisites
# This runs BEFORE devcontainer lifecycle commands to ensure the base image is correct

set -e

echo "🧪 Validating mp-units Docker image prerequisites..."
echo ""

# Verify essential tools are available in the base image
echo "📋 Verifying essential tools:"
tools=(gcc g++ clang clang++ cmake ninja conan pre-commit properdocs)
all_found=true
for tool in "${tools[@]}"; do
  if command -v "$tool" >/dev/null 2>&1; then
    echo "  ✅ $tool"
  else
    echo "  ❌ $tool: NOT FOUND"
    all_found=false
  fi
done

if [ "$all_found" = false ]; then
  echo ""
  echo "❌ Docker image is missing required tools"
  exit 1
fi

echo ""
echo "🔧 Default tool versions:"
gcc --version | head -1
g++ --version | head -1
clang --version | head -1
clang++ --version | head -1
cmake --version | head -1
ninja --version
conan --version
pre-commit --version
properdocs --version

echo ""
echo "📋 Verifying compiler matrix:"

# Expected GCC versions (12, 13, 14, 15)
expected_gcc=(12 13 14 15)
for version in "${expected_gcc[@]}"; do
  if command -v "gcc-$version" >/dev/null 2>&1 && command -v "g++-$version" >/dev/null 2>&1; then
    echo "  ✅ GCC-$version"
  else
    echo "  ❌ GCC-$version: NOT FOUND"
    exit 1
  fi
done

# Expected Clang versions (16, 17, 18, 19, 20, 21)
expected_clang=(16 17 18 19 20 21)
for version in "${expected_clang[@]}"; do
  if command -v "clang-$version" >/dev/null 2>&1 && command -v "clang++-$version" >/dev/null 2>&1; then
    echo "  ✅ Clang-$version"
  else
    echo "  ❌ Clang-$version: NOT FOUND"
    exit 1
  fi
done

echo ""
echo "📦 Verifying Conan profiles exist in image:"

# Verify expected Conan profiles exist in the Docker image
expected_profiles=(gcc12 gcc13 gcc14 gcc15 clang16 clang17 clang18 clang19 clang20 clang21)
for profile in "${expected_profiles[@]}"; do
  if conan profile show -pr "$profile" >/dev/null 2>&1; then
    echo "  ✅ Profile $profile"
  else
    echo "  ❌ Profile $profile: MISSING"
    exit 1
  fi
done

echo ""
echo "✅ Docker image validation passed! Ready for devcontainer lifecycle commands."
