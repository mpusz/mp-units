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


# Helper to use conan generated configuration if provided
macro(conan_init generator)
    if(${generator} STREQUAL "cmake_paths")
        include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)
    elseif(${generator} STREQUAL "cmake")
        if(NOT DEFINED CONAN_PACKAGE_NAME)
            if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
                include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
                conan_basic_setup(TARGETS)
            endif()
        endif()
    else()
        message(FATAL_ERROR "Unknown Conan generator: ${generator}")
    endif()
endmacro()


# Checks if conan installed testing dependencies
macro(conan_check_testing test_framework)
    if(NOT TARGET CONAN_PKG::${test_framework})
        message(FATAL_ERROR "CONAN_PKG::${test_framework} not found!\nPlease run `conan install` with `-e CONAN_RUN_TESTS=True`.")
    endif()
endmacro()
