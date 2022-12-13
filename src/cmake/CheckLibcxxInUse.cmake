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

function(check_libcxx_in_use variable)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(CHECK_START "Checking if libc++ is being used")
        list(APPEND CMAKE_MESSAGE_INDENT "  ")

        include(CheckCXXSymbolExists)
        check_cxx_symbol_exists(_LIBCPP_VERSION "ciso646" ${variable})
        set(${variable} ${${variable}} PARENT_SCOPE)

        list(POP_BACK CMAKE_MESSAGE_INDENT)
        if(${variable})
            message(CHECK_PASS "found")
        else()
            message(CHECK_FAIL "not found")
        endif()
    endif()
endfunction()
