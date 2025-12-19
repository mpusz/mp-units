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

cmake_minimum_required(VERSION 3.30.5)
project(test_package LANGUAGES CXX)

message(STATUS "MP_UNITS_BUILD_CXX_MODULES: ${MP_UNITS_BUILD_CXX_MODULES}")

find_package(mp-units REQUIRED)

add_executable(test_package-headers test_package.cpp)
target_compile_features(test_package-headers PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>)
target_link_libraries(test_package-headers PRIVATE mp-units::mp-units)

if(MP_UNITS_BUILD_CXX_MODULES)
    add_executable(test_package test_package.cpp)
    target_compile_features(test_package PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>)
    # TODO clang complains about missing module
    # target_compile_definitions(test_package PRIVATE MP_UNITS_MODULES)
    target_link_libraries(test_package PRIVATE mp-units::mp-units)
endif()
