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

cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(mp-units REQUIRED)

add_executable(test_package test_package.cpp)
target_link_libraries(test_package PRIVATE mp-units::mp-units)
target_compile_definitions(test_package PRIVATE MP_UNITS_API_STD_FORMAT=$<BOOL:${MP_UNITS_API_STD_FORMAT}>)

if(MP_UNITS_API_CONTRACTS STREQUAL "NONE")
    target_compile_definitions(test_package PRIVATE MP_UNITS_API_CONTRACTS=0)
elseif(MP_UNITS_API_CONTRACTS STREQUAL "GSL-LITE")
    target_compile_definitions(test_package PRIVATE MP_UNITS_API_CONTRACTS=2)
elseif(MP_UNITS_API_CONTRACTS STREQUAL "MS-GSL")
    target_compile_definitions(test_package PRIVATE MP_UNITS_API_CONTRACTS=3)
endif()
