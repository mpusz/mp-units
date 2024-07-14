# The MIT License (MIT)
#
# Copyright (c) 2017 Mateusz Pusz
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

#
# enable_clang_tidy([PROGRAM]                     # clang-tidy by default
#                   [QUIET] [REQUIRED])
#
function(enable_clang_tidy)
    set(_options QUIET REQUIRED)
    set(_one_value_args PROGRAM)
    set(_multi_value_args)
    cmake_parse_arguments(PARSE_ARGV 0 _enable_clang_tidy "${_options}" "${_one_value_args}" "${_multi_value_args}")

    # validate and process arguments
    if(_enable_clang_tidy_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments '${_enable_clang_tidy_UNPARSED_ARGUMENTS}'")
    endif()

    if(_enable_clang_tidy_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "No value provided for '${_enable_clang_tidy_KEYWORDS_MISSING_VALUES}'")
    endif()

    if(NOT _enable_iwyu_QUIET)
        message(STATUS "Enabling clang-tidy${log_postfix}")
    endif()

    if(${_enable_clang_tidy_REQUIRED})
        set(_error_log_level FATAL_ERROR)
    elseif(NOT _enable_clang_tidy_QUIET)
        set(_error_log_level STATUS)
    endif()

    if(NOT DEFINED _enable_clang_tidy_PROGRAM)
        set(_enable_clang_tidy_PROGRAM clang-tidy)
    endif()

    find_program(_clang_tidy_path ${_enable_clang_tidy_PROGRAM})
    if(_clang_tidy_path)
        if(NOT _enable_clang_tidy_QUIET)
            message(STATUS "  Executable: ${_clang_tidy_path}")
        endif()
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/.clang-tidy")
            message(FATAL_ERROR "'${CMAKE_SOURCE_DIR}/.clang-tidy' configuration file not found!")
        endif()
        set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_cmd}" PARENT_SCOPE)
    else()
        if(DEFINED _error_log_level)
            message(${_error_log_level} "  '${_enable_clang_tidy_PROGRAM}' executable was not found")
        endif()
        if(NOT _enable_iwyu_QUIET)
            message(STATUS "Enabling clang-tidy${log_postfix} - failed")
        endif()
        return()
    endif()

    if(NOT _enable_iwyu_QUIET)
        message(STATUS "Enabling clang-tidy${log_postfix} - done")
    endif()
endfunction()
