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

cmake_minimum_required(VERSION 3.19)

# core-macros library definition
add_library(
    mp-units-core-macros INTERFACE include/mp-units/bits/external/hacks.h include/mp-units/quantity_spec_macro.h
)
target_compile_features(mp-units-core-macros INTERFACE cxx_std_20)
target_include_directories(
    mp-units-core-macros ${unitsAsSystem} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                    $<INSTALL_INTERFACE:include>
)

set_target_properties(mp-units-core-macros PROPERTIES EXPORT_NAME core-macros)
add_library(mp-units::core-macros ALIAS mp-units-core-macros)

# installation
install(TARGETS mp-units-core-macros EXPORT mp-unitsTargets)
install(DIRECTORY include/mp-units TYPE INCLUDE)
