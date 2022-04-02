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

cmake_minimum_required(VERSION 3.7)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # run IWYU only for clang compiler
    # other compilers will generate i.e. unknown compilation flags warnings
    return()
endif()

set(IWYU_VERBOSITY_LEVEL 3 CACHE STRING "IWYU verbosity level (the higher the level, the more output)")

macro(_iwyu_args_append arg)
    list(APPEND _iwyu_args "-Xiwyu" "${arg}")
endmacro()

macro(_iwyu_args_append_if_present option arg)
    if(_enable_iwyu_${option})
        _iwyu_args_append("${arg}")
    endif()
endmacro()

macro(_enable_iwyu_failed log_postfix)
    if(NOT _enable_iwyu_QUIET)
        message(STATUS "Enabling include-what-you-use${log_postfix} - failed")
    endif()
    return()
endmacro()

macro(_process_iwyu_arguments offset log_postfix)
    set(_options
        QUIET
        REQUIRED
        NO_DEFAULT_MAPPINGS
        PCH_IN_CODE
        TRANSITIVE_INCLUDES_ONLY
        NO_COMMENTS
        NO_FORWARD_DECLARATIONS
        CXX17_NAMESPACES
        QUOTED_INCLUDES_FIRST
    )
    set(_one_value_args PROGRAM MAPPING_FILE MAX_LINE_LENGTH)
    set(_multi_value_args KEEP)
    cmake_parse_arguments(PARSE_ARGV ${offset} _enable_iwyu "${_options}" "${_one_value_args}" "${_multi_value_args}")

    # validate and process arguments
    if(_enable_iwyu_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments '${_enable_iwyu_UNPARSED_ARGUMENTS}'")
    endif()

    if(_enable_iwyu_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "No value provided for '${_enable_iwyu_KEYWORDS_MISSING_VALUES}'")
    endif()

    if(NOT _enable_iwyu_QUIET)
        message(STATUS "Enabling include-what-you-use${log_postfix}")
    endif()

    if(${_enable_iwyu_REQUIRED})
        set(_error_log_level FATAL_ERROR)
    elseif(NOT _enable_iwyu_QUIET)
        set(_error_log_level STATUS)
    endif()

    if(NOT DEFINED _enable_iwyu_PROGRAM)
        set(_enable_iwyu_PROGRAM include-what-you-use)
    endif()

    find_program(IWYU_PATH ${_enable_iwyu_PROGRAM})
    if(IWYU_PATH)
        if(NOT _enable_iwyu_QUIET)
            message(STATUS "  Executable: ${IWYU_PATH}")
        endif()
    else()
        if(DEFINED _error_log_level)
            message(${_error_log_level} "  '${_enable_iwyu_PROGRAM}' executable was not found")
        endif()
        _enable_iwyu_failed("${log_postfix}")
    endif()

    if(_enable_iwyu_KEEP)
        foreach(_pattern ${_enable_iwyu_KEEP})
            _iwyu_args_append("--keep=${_pattern}")
        endforeach()
    endif()

    if(_enable_iwyu_MAPPING_FILE)
        if(NOT EXISTS ${_enable_iwyu_MAPPING_FILE})
            message(FATAL_ERROR "IWYU: Mapping file '${_enable_iwyu_MAPPING_FILE}' does not exist")
        endif()
        _iwyu_args_append("--mapping_file=${_enable_iwyu_MAPPING_FILE}")
    endif()

    if(_enable_iwyu_MAX_LINE_LENGTH)
        if(NOT _enable_iwyu_MAX_LINE_LENGTH GREATER 0)
            message(FATAL_ERROR "IWYU: Invalid MAX_LINE_LENGTH value = '${_enable_iwyu_MAX_LINE_LENGTH}")
        endif()
        _iwyu_args_append("--max_line_length=${_enable_iwyu_MAX_LINE_LENGTH}")
    endif()

    _iwyu_args_append_if_present(NO_DEFAULT_MAPPINGS "--no_default_mappings")
    _iwyu_args_append_if_present(PCH_IN_CODE "--pch_in_code")
    _iwyu_args_append_if_present(TRANSITIVE_INCLUDES_ONLY "--transitive_includes_only")
    _iwyu_args_append_if_present(NO_COMMENTS "--no_comments")
    _iwyu_args_append_if_present(NO_FORWARD_DECLARATIONS "--no_fwd_decls")
    _iwyu_args_append_if_present(CXX17_NAMESPACES "--cxx17ns")
    _iwyu_args_append_if_present(QUOTED_INCLUDES_FIRST "--quoted_includes_first")

    _iwyu_args_append("--verbose=${IWYU_VERBOSITY_LEVEL}")

    if(NOT _enable_iwyu_QUIET)
        message(STATUS "  Arguments: ${_iwyu_args}")
        message(STATUS "Enabling include-what-you-use${log_postfix} - done")
    endif()
endmacro()

#
# enable_iwyu([PROGRAM]                     # include-what-you-use by default
#             [QUIET] [REQUIRED]
#             [KEEP patterns...]
#             [MAPPING_FILE file]
#             [NO_DEFAULT_MAPPINGS]
#             [PCH_IN_CODE]
#             [TRANSITIVE_INCLUDES_ONLY]
#             [MAX_LINE_LENGTH length]
#             [NO_COMMENTS]
#             [NO_FORWARD_DECLARATIONS]
#             [CXX17_NAMESPACES]
#             [QUOTED_INCLUDES_FIRST])
#
function(enable_iwyu)
    _process_iwyu_arguments(0 "")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${_iwyu_path};${_iwyu_args}" PARENT_SCOPE)
endfunction()

#
# enable_target_iwyu(TargetName
#                    [PROGRAM]                     # include-what-you-use by default
#                    [QUIET] [REQUIRED]
#                    [KEEP pattern...]
#                    [MAPPING_FILE file]
#                    [NO_DEFAULT_MAPPINGS]
#                    [PCH_IN_CODE]
#                    [TRANSITIVE_INCLUDES_ONLY]
#                    [MAX_LINE_LENGTH length]
#                    [NO_COMMENTS]
#                    [NO_FORWARD_DECLARATIONS]
#                    [CXX17_NAMESPACES]
#                    [QUOTED_INCLUDES_FIRST])
#
function(enable_target_iwyu target)
    _process_iwyu_arguments(1 " for '${target}'")
    set_target_properties(${target} PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "${_iwyu_path};${_iwyu_args}")
endfunction()
