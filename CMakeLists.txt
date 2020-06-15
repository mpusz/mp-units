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

cmake_minimum_required(VERSION 3.12)
project(mp-units)

set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT ON)
endif()

option(MP_UNITS_DOCS "Enable documentation" ${MASTER_PROJECT})
option(MP_UNITS_EXAMPLES "Enable examples" ${MASTER_PROJECT})
option(MP_UNITS_TESTS "Enable tests" ${MASTER_PROJECT})

# set path to custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# include common tools and workarounds
include(common/cmake/scripts)

# use Conan configuration if available
conan_init(cmake)

# enable static analysis
#enable_clang_tidy()
#enable_iwyu()

# set restrictive compilation warnings
set_warnings(TREAT_AS_ERRORS)

# add project code
add_subdirectory(src)

# add unit tests
if(MP_UNITS_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# add usage example
if(MP_UNITS_EXAMPLES)
    add_subdirectory(example)
endif()

# generate project documentation
if(MP_UNITS_DOCS)
    add_subdirectory(docs)
endif()
