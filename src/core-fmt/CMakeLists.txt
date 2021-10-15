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

option(UNITS_USE_LIBFMT "Enables usage of libfmt instead of the one from 'std'" ON)
message(STATUS "UNITS_USE_LIBFMT: ${UNITS_USE_LIBFMT}")

add_units_module(core-fmt mp-units::core)
target_compile_definitions(mp-units-core-fmt INTERFACE UNITS_USE_LIBFMT=$<BOOL:${UNITS_USE_LIBFMT}>)

if(UNITS_USE_LIBFMT)
  find_package(fmt CONFIG REQUIRED)
  target_link_libraries(mp-units-core-fmt INTERFACE fmt::fmt)
endif()
