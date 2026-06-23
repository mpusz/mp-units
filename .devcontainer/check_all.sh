#!/bin/bash

# Codespace-specific multi-compiler testing script for mp-units
# This script mirrors the functionality of the local check_all.sh but is designed
# specifically for the standardized Codespace environment.

set -e

build_policy="missing"
while getopts :hdn opt
do
  case $opt in
    h)
      echo "Usage: $0 [-d] [-n] [install|build|create]"
      echo "  -d    Run debug builds in addition to release builds"
      echo "  -n    Never build: require all dependencies to be pre-built (fail if build from source is needed)"
      echo "Commands:"
      echo "  install  Install dependencies only"
      echo "  build    Build the project with all compiler configurations"
      echo "  create   Create Conan packages with all configurations"
      exit 0
      ;;
    d)
      run_debug=true
      ;;
    n)
      build_policy="never"
      ;;
    *)
      echo "Invalid option: -$OPTARG" >&2
      echo "Use -h for help." >&2
      exit 1
      ;;
  esac
done

# get the command to run
shift $((OPTIND - 1))

if [[ $1 != "install" && $1 != "build" && $1 != "create" ]]; then
  echo 'Invalid operation! "install", "build", or "create" expected.'
  echo 'Use -h for help.'
  exit 1
fi

echo "🚀 Starting mp-units multi-compiler testing in Codespace environment..."
echo "📋 Available compilers:"
echo "  GCC: $(for c in /usr/bin/gcc-[0-9]*; do [ -e "$c" ] && echo "$c"; done | sort -V | tr '\n' ' ')"
echo "  Clang: $(for c in /usr/bin/clang++-[0-9]* /usr/local/bin/clang++-[0-9]*; do [ -e "$c" ] && echo "$c"; done | sort -V | tr '\n' ' ')"
echo "🔧 Operation: $1"
if [[ $run_debug ]]; then
  echo "🐛 Debug builds: enabled"
fi
if [[ $build_policy == "never" ]]; then
  echo "⚙️  Never build mode: requiring pre-built dependencies only"
fi
echo ""

if [[ $1 != "install" ]]; then
  echo "🧹 Running pre-commit checks..."
  pre-commit run --all-files
  echo ""
fi

echo "🏗️  Starting compiler matrix testing..."

# GCC configurations
echo "🚀 Starting release build configurations..."
echo "⚙️  Testing GCC configurations..."
set -x
conan "$1" . -pr gcc12   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b "$build_policy"
conan "$1" . -pr gcc13   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=23 -b "$build_policy"
conan "$1" . -pr gcc14   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=ms-gsl'   -s compiler.cppstd=23 -b "$build_policy"
conan "$1" . -pr gcc15   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy"
set +x

echo "⚙️  Testing Clang configurations..."
set -x
conan "$1" . -pr clang16 -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b "$build_policy"
conan "$1" . -pr clang17 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=ms-gsl'   -s compiler.cppstd=26 -b "$build_policy"
conan "$1" . -pr clang18 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy"
# clang-19 will never compile mp-units due to https://github.com/llvm/llvm-project/pull/118288
conan "$1" . -pr clang20 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy"
conan "$1" . -pr clang21 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy"
set +x

if [[ $run_debug ]]; then
  echo "🐛 Starting debug build configurations..."
  echo "⚙️  Testing GCC debug configurations..."
  set -x
  conan "$1" . -pr gcc12   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr gcc13   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=23 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr gcc14   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=ms-gsl'   -s compiler.cppstd=23 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr gcc15   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy" -s build_type=Debug
  set +x

  echo "⚙️  Testing Clang debug configurations..."
  set -x
  conan "$1" . -pr clang16 -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr clang17 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=ms-gsl'   -s compiler.cppstd=26 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr clang18 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy" -s build_type=Debug
  # clang-19 will never compile mp-units due to https://github.com/llvm/llvm-project/pull/118288
  conan "$1" . -pr clang20 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy" -s build_type=Debug
  conan "$1" . -pr clang21 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b "$build_policy" -s build_type=Debug
  set +x
  echo "✅ Debug builds completed!"
fi

echo ""
echo "🎉 Multi-compiler testing completed successfully!"
