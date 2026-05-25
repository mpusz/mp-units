# The MIT License (MIT)
#
# Copyright (c) 2018 Mateusz Pusz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Compiler-specific setup needed when consuming `import std;`.
#
# CMake builds the std module BMI as a separate internal target
# (`__cmake_cxx_std_NN`) that doesn't link mp-units and therefore doesn't
# inherit mp-units' target-level compile options. So whatever flags the BMI
# needs to be compatible with consumer translation units must be applied
# project-wide (via `add_compile_options`) instead of target-locally. This
# script is included from every entry point — the dev top-level CMakeLists,
# the installed mp-unitsConfig.cmake, and (via Conan's `cmake_build_modules`)
# Conan-generated configs — so all consumption paths converge here.

if(NOT CMAKE_CXX_MODULE_STD)
    return()
endif()

if(MSVC)
    # C5050: the BMI must be built with the same `_UTF8` preprocessor state as
    # consumer TUs. `/utf-8` defines `_UTF8`; without this consumers compiled
    # with `/utf-8` would mismatch a BMI built without it.
    add_compile_options(/utf-8)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Workaround for CMake 4.3 + Clang 21 incompatibility:
    # Clang 21 reports `export module std;` in libc++'s std.cppm as
    # `-Wreserved-module-identifier` (now default-on), because CMake compiles
    # the stdlib module with the project's compiler flags.
    add_compile_options(-Wno-error=reserved-module-identifier)
endif()
