# The MIT License (MIT)
#
# Copyright (c) 2016 Mateusz Pusz
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

# Based on https://github.com/lefticus/cpp_starter_project/blob/master/cmake/CompilerWarnings.cmake

cmake_minimum_required(VERSION 3.15)

option(${projectPrefix}WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

macro(_set_flags)
    set(MSVC_WARNINGS
        /W4 # Baseline reasonable warnings
        /w14062 # enumerator 'identifier' in a switch of enum 'enumeration' is not handled
        /w14242 # 'identifier': conversion from 'type1' to 'type1', possible loss of data
        /w14254 # 'operator': conversion from 'type1:field_bits' to 'type2:field_bits', possible loss of data
        /w14263 # 'function': member function does not override any base class virtual member function
        /w14265 # 'classname': class has virtual functions, but destructor is not virtual instances of this class may not be destructed correctly
        /w14266 # 'function': no override available for virtual member function from base 'type'; function is hidden
        /w14287 # 'operator': unsigned/negative constant mismatch
        /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
        /w14296 # 'operator': expression is always 'boolean_value'
        /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545 # expression before comma evaluates to a function which is missing an argument list
        /w14546 # function call before comma missing argument list
        /w14547 # 'operator': operator before comma has no effect; expected operator with side-effect
        /w14549 # 'operator': operator before comma has no effect; did you intend 'operator'?
        /w14555 # expression has no effect; expected expression with side- effect
        /w14619 # pragma warning: there is no warning number 'number'
        /w14640 # Enable warning on thread un-safe static member initialization
        /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may cause unexpected runtime behavior.
        /w14905 # wide string literal cast to 'LPSTR'
        /w14906 # string literal cast to 'LPWSTR'
        /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
        /permissive- # standards conformance mode for MSVC compiler.
    )

    set(GCC_COMMON_WARNINGS
        -Wall
        -Wextra # reasonable and standard
        -Wpedantic # warn if non-standard C++ is used
        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
        -Wold-style-cast # warn for c-style casts
        -Wcast-align # warn for potential performance problem casts
        -Wunused # warn on anything being unused
        -Woverloaded-virtual # warn if you overload (not override) a virtual function
        -Wcast-qual # warn on dropping const or volatile qualifiers
        -Wconversion # warn on type conversions that may lose data
        -Wsign-conversion # warn on sign conversions
        -Wnull-dereference # warn if a null dereference is detected
        -Wformat=2 # warn on security issues around functions that format output (ie printf)
        -Wdangling-else # warn about constructions where there may be confusion to which if statement an else branch belongs
    )

    set(CLANG_WARNINGS ${GCC_COMMON_WARNINGS})

    set(GCC_WARNINGS
        ${GCC_COMMON_WARNINGS}
        -Wdouble-promotion # warn if float is implicit promoted to double
        -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
        -Wduplicated-cond # warn if if / else chain has duplicated conditions
        -Wduplicated-branches # warn if if / else branches have duplicated code
        -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
        -Wduplicated-branches # warn when an if-else has identical branches
        -Wduplicated-cond # warn about duplicated conditions in an if-else-if chain
    )

    if(${projectPrefix}WARNINGS_AS_ERRORS)
        set(GCC_WARNINGS ${GCC_WARNINGS} -Werror)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
        set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
    endif()

    if(MSVC)
        set(flags ${MSVC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(flags ${CLANG_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(flags ${GCC_WARNINGS})
    else()
        message(WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
        return()
    endif()
endmacro()

# Set global compiler warning level
function(set_warnings)
    _set_flags()

    message(STATUS "Setting restrictive compilation warnings")
    message(STATUS "  Treat warnings as errors: ${${projectPrefix}WARNINGS_AS_ERRORS}")
    message(STATUS "  Flags: ${flags}")
    message(STATUS "Setting restrictive compilation warnings - done")

    add_compile_options(${flags})
endfunction()

# Set compiler warning level for a provided CMake target
function(set_target_warnings target scope)
    set(scopes PUBLIC INTERFACE PRIVATE)

    if(NOT scope IN_LIST scopes)
        message(FATAL_ERROR "'scope' argument should be one of ${scopes} ('${scope}' received)")
    endif()

    _set_flags()

    message(STATUS "Setting ${scope} restrictive compilation warnings for '${target}'")
    message(STATUS "  Treat warnings as errors: ${${projectPrefix}WARNINGS_AS_ERRORS}")
    message(STATUS "  Flags: ${flags}")
    message(STATUS "Setting ${scope} restrictive compilation warnings for '${target}' - done")

    target_compile_options(${target} ${scope} ${flags})
endfunction()
