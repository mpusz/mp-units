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
project(mp-units VERSION 0.8.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Only change the folder behavior if cpr is not a subproject
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

    # use ccache if available
    include(ccache)
    enable_ccache(BASE_DIR ${PROJECT_SOURCE_DIR})
endif()

# Avoid the dll boilerplate code for windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# set restrictive compilation warnings
include(warnings)
set_warnings()

# make sure that the file is being used as an entry point
include(modern_project_structure)
ensure_entry_point()

# set all contracts checking in a Debug build
add_compile_definitions($<$<CONFIG:Debug>:gsl_CONFIG_CONTRACT_CHECKING_AUDIT>)

macro(units_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow overriding the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

macro(units_option_downcast OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    set(${OPTION_NAME} ${OPTION_DEFAULT} CACHE STRING ${OPTION_TEXT})
    set_property(CACHE ${OPTION_NAME} PROPERTY STRINGS AUTO ON OFF)

    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()

    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()

    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "mp-units CMake Options")
message(STATUS "=======================================================")
units_option(UNITS_BUILD_TESTS "Set to ON to build mp-units tests." OFF)
units_option(UNITS_BUILD_DOCS "Set to ON to build mp-units docs." OFF)
units_option(UNITS_BUILD_EXAMPLES "Set to ON to build mp-units examples." OFF)
units_option(UNITS_IWYU "Set to ON to enable libfmt instead of the one from 'std'." OFF)
units_option(UNITS_USE_LIB_FMT "Set to ON to enable 'include-what-you-use'." OFF)
units_option(UNITS_AS_SYSTEM_HEADERS "Set to ON to export library as system header." OFF)
units_option(UNITS_WARNINGS_AS_ERRORS "Set to ON to treat all compile warnings as errors." OFF)
units_option_downcast(UNITS_DOWNCAST_MODE "Downcasting mode" ON)
message(STATUS "=======================================================")

add_subdirectory(src)

if(UNITS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)

    # tests for standalone headers
    include(TestPublicHeaders)
    add_public_header_test(test_headers mp-units::mp-units)
endif()

if(UNITS_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if(UNITS_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(UNITS_IWYU)
    include(include-what-you-use)
    enable_iwyu(
        MAPPING_FILE "${PROJECT_SOURCE_DIR}/.mp-units.imp"
        NO_FORWARD_DECLARATIONS QUOTED_INCLUDES_FIRST
        MAX_LINE_LENGTH 120
        NO_COMMENTS
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(UNITS_AS_SYSTEM_HEADERS ON)
    endif()
endif()
