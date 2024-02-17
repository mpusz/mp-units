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

#[=======================================================================[.rst:
CheckCXXFeatureSupported
------------------------

Check if a C++ feature is supported.

.. command:: check_cxx_feature_supported

  .. code-block:: cmake

    check_cxx_feature_supported(<condition> <variable>)

  Check that the ``<condition>`` is true and store the result in a ``<variable>``.

The following variables may be set before calling this macro to modify
the way the check is run:

.. include:: /module/CMAKE_REQUIRED_FLAGS.txt

.. include:: /module/CMAKE_REQUIRED_DEFINITIONS.txt

.. include:: /module/CMAKE_REQUIRED_INCLUDES.txt

.. include:: /module/CMAKE_REQUIRED_LINK_OPTIONS.txt

.. include:: /module/CMAKE_REQUIRED_LIBRARIES.txt

.. include:: /module/CMAKE_REQUIRED_QUIET.txt

For example:

.. code-block:: cmake

  include(CheckCXXFeatureSupported)

  check_cxx_feature_supported(__cpp_explicit_this_parameter EXPLICIT_THIS_PARAMETER_SUPPORTED)
  check_cxx_feature_supported("__cpp_constexpr >= 202211L" STATIC_CONSTEXPR_VARS_IN_CONSTEXPR_FUNCTIONS)
#]=======================================================================]

include_guard(GLOBAL)

macro(CHECK_CXX_FEATURE_SUPPORTED CONDITION VARIABLE)
    if(NOT DEFINED "${VARIABLE}" OR "x${${VARIABLE}}" STREQUAL "x${VARIABLE}")
        set(_CFS_SOURCE "/* */\n")
        set(MACRO_CHECK_FEATURE_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
        if(CMAKE_REQUIRED_LINK_OPTIONS)
            set(CHECK_FEATURE_EXISTS_LINK_OPTIONS LINK_OPTIONS ${CMAKE_REQUIRED_LINK_OPTIONS})
        else()
            set(CHECK_FEATURE_EXISTS_LINK_OPTIONS)
        endif()
        if(CMAKE_REQUIRED_LIBRARIES)
            set(CHECK_FEATURE_EXISTS_LIBS LINK_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
        else()
            set(CHECK_FEATURE_EXISTS_LIBS)
        endif()
        if(CMAKE_REQUIRED_INCLUDES)
            set(CMAKE_FEATURE_EXISTS_INCLUDES "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
        else()
            set(CMAKE_FEATURE_EXISTS_INCLUDES)
        endif()
        string(APPEND _CFS_SOURCE "#include <version>\n")
        string(APPEND
               _CFS_SOURCE
               "
int main()
{
  static_assert(${CONDITION});
}\n"
        )

        if(NOT CMAKE_REQUIRED_QUIET)
            message(CHECK_START "Checking C++ feature test macro '${CONDITION}'")
        endif()
        try_compile(
            ${VARIABLE} SOURCE_FROM_VAR
            "CheckFeatureExists.cxx" _CFS_SOURCE
            COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} ${CHECK_FEATURE_EXISTS_LINK_OPTIONS}
                                ${CHECK_FEATURE_EXISTS_LIBS}
            CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_FEATURE_EXISTS_FLAGS}
                        "${CMAKE_FEATURE_EXISTS_INCLUDES}"
        )
        if(${VARIABLE})
            if(NOT CMAKE_REQUIRED_QUIET)
                message(CHECK_PASS "SUCCESS")
            endif()
            set(${VARIABLE} 1 CACHE INTERNAL "C++ feature test macro '${CONDITION}' supported")
        else()
            if(NOT CMAKE_REQUIRED_QUIET)
                message(CHECK_FAIL "FAIL")
            endif()
            set(${VARIABLE} "" CACHE INTERNAL "C++ feature test macro '${CONDITION}' supported")
        endif()
        unset(_CFS_SOURCE)
    endif()
endmacro()
