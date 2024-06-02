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

cmake_minimum_required(VERSION 3.23)
project(mp-units-dev LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(projectPrefix MP_UNITS_)

option(${projectPrefix}DEV_BUILD_LA "Build code depending on the linear algebra library" ON)
option(${projectPrefix}DEV_IWYU "Enables include-what-you-use" OFF)
option(${projectPrefix}DEV_CLANG_TIDY "Enables clang-tidy" OFF)

message(STATUS "${projectPrefix}DEV_BUILD_LA: ${${projectPrefix}DEV_BUILD_LA}")
message(STATUS "${projectPrefix}DEV_IWYU: ${${projectPrefix}DEV_IWYU}")
message(STATUS "${projectPrefix}DEV_CLANG_TIDY: ${${projectPrefix}DEV_CLANG_TIDY}")

# make sure that the file is being used as an entry point
include(modern_project_structure)
ensure_entry_point()

# use ccache if available
include(ccache)
enable_ccache(BASE_DIR ${PROJECT_SOURCE_DIR})

if(${projectPrefix}DEV_IWYU)
    set(${projectPrefix}BUILD_AS_SYSTEM_HEADERS ON)

    include(include-what-you-use)
    enable_iwyu(
        REQUIRED
        MAPPING_FILE "${PROJECT_SOURCE_DIR}/.mp-units.imp"
        NO_FORWARD_DECLARATIONS QUOTED_INCLUDES_FIRST
        MAX_LINE_LENGTH 120
        NO_COMMENTS
    )
elseif(${projectPrefix}DEV_CLANG_TIDY)
    include(static_analysis)
    enable_clang_tidy(PROGRAM clang-tidy-18 REQUIRED)
else()
    # set restrictive compilation warnings
    # (some gcc warnings are not recognized by IWYU which is based on clang)
    include(warnings)
    set_warnings()
endif()

# add project code
add_subdirectory(src)

if(NOT ${projectPrefix}API_FREESTANDING)
    # add usage example
    add_subdirectory(example)
endif()

# add unit tests
enable_testing()

add_subdirectory(test)
