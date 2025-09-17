#!/bin/bash

# Codespace-specific multi-compiler testing script for mp-units
# This script mirrors the functionality of the local check_all.sh but is designed
# specifically for the standardized Codespace environment.

set -e

while getopts :hd opt
do
  case $opt in
    h)
      echo "Usage: $0 [-d] [install|build|create]"
      echo "  -d    Run debug builds in addition to release builds"
      echo "Commands:"
      echo "  install  Install dependencies only"
      echo "  build    Build the project with all compiler configurations"
      echo "  create   Create Conan packages with all configurations"
      exit 0
      ;;
    d)
      run_debug=true
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
echo "  GCC: $(ls /usr/bin/gcc-* | grep -E "gcc-[0-9]+" | sort -V | tr '\n' ' ')"
echo "  Clang: $(ls /usr/bin/clang++-* /usr/local/bin/clang++-* 2>/dev/null | grep -E "clang\+\+-[0-9]+" | sort -V | tr '\n' ' ')"
echo "🔧 Operation: $1"
if [[ $run_debug ]]; then
  echo "🐛 Debug builds: enabled"
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
conan $1 . -pr gcc12   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b missing
conan $1 . -pr gcc13   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=23 -b missing
conan $1 . -pr gcc14   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=ms-gsl'   -s compiler.cppstd=23 -b missing
# gcc-15.0.1 has bugs preventing mp-units from compiling
# conan $1 . -pr gcc15   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=gsl-lite' -s compiler.cppstd=26 -b missing

echo "⚙️  Testing Clang configurations..."
conan $1 . -pr clang16 -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b missing
conan $1 . -pr clang17 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=ms-gsl'   -s compiler.cppstd=26 -b missing
conan $1 . -pr clang18 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing
# clang-19 will never compile mp-units due to https://github.com/llvm/llvm-project/pull/118288
conan $1 . -pr clang20 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing
conan $1 . -pr clang21 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing

if [[ $run_debug ]]; then
  echo "🐛 Starting debug build configurations..."
  echo "⚙️  Testing GCC debug configurations..."
  conan $1 . -pr gcc12   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b missing -s build_type=Debug
  conan $1 . -pr gcc13   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=23 -b missing -s build_type=Debug
  conan $1 . -pr gcc14   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=ms-gsl'   -s compiler.cppstd=23 -b missing -s build_type=Debug
  # gcc-15.0.1 has bugs preventing mp-units from compiling
  # conan $1 . -pr gcc15   -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=gsl-lite' -s compiler.cppstd=26 -b missing -s build_type=Debug

  echo "⚙️  Testing Clang debug configurations..."
  conan $1 . -pr clang16 -c user.mp-units.build:all=True -o '&:cxx_modules=False' -o '&:import_std=False' -o '&:std_format=False' -o '&:contracts=gsl-lite' -s compiler.cppstd=20 -b missing -s build_type=Debug
  conan $1 . -pr clang17 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=False' -o '&:std_format=True'  -o '&:contracts=ms-gsl'   -s compiler.cppstd=26 -b missing -s build_type=Debug
  conan $1 . -pr clang18 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing -s build_type=Debug
  # clang-19 will never compile mp-units due to https://github.com/llvm/llvm-project/pull/118288
  conan $1 . -pr clang20 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing -s build_type=Debug
  conan $1 . -pr clang21 -c user.mp-units.build:all=True -o '&:cxx_modules=True'  -o '&:import_std=True'  -o '&:std_format=True'  -o '&:contracts=none'     -s compiler.cppstd=26 -b missing -s build_type=Debug
  echo "✅ Debug builds completed!"
fi

echo ""
echo "🎉 Multi-compiler testing completed successfully!"
