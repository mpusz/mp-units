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

cmake_minimum_required(VERSION 3.25)

add_library(mp-units-contracts INTERFACE)

if(MP_UNITS_API_CONTRACTS STREQUAL "NONE")
    target_compile_definitions(mp-units-contracts INTERFACE MP_UNITS_API_CONTRACTS=0)
elseif(MP_UNITS_API_CONTRACTS STREQUAL "STD")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16")
        target_compile_options(mp-units-contracts INTERFACE -fcontracts)
        if(NOT MP_UNITS_API_FREESTANDING)
            # the default contract-violation handler lives in the experimental library;
            # a freestanding application has to provide its own handler (or compile with
            # `-fcontract-evaluation-semantic=quick_enforce`)
            target_link_libraries(mp-units-contracts INTERFACE stdc++exp)
        endif()
    else()
        message(FATAL_ERROR "C++ contracts are not supported by ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}")
    endif()
    target_compile_definitions(mp-units-contracts INTERFACE MP_UNITS_API_CONTRACTS=1)
elseif(MP_UNITS_API_CONTRACTS STREQUAL "GSL-LITE")
    if(NOT TARGET gsl-lite::gsl-lite)
        find_package(gsl-lite REQUIRED)
    endif()
    target_link_libraries(mp-units-contracts INTERFACE gsl-lite::gsl-lite)
    target_compile_definitions(mp-units-contracts INTERFACE MP_UNITS_API_CONTRACTS=2)
elseif(MP_UNITS_API_CONTRACTS STREQUAL "MS-GSL")
    if(NOT TARGET Microsoft.GSL::GSL)
        find_package(Microsoft.GSL REQUIRED)
    endif()
    target_link_libraries(mp-units-contracts INTERFACE Microsoft.GSL::GSL)
    target_compile_definitions(mp-units-contracts INTERFACE MP_UNITS_API_CONTRACTS=3)
endif()

install(TARGETS mp-units-contracts EXPORT mp-unitsTargets)
