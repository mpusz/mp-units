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

add_library(glide_computer_lib-headers STATIC glide_computer_lib.cpp include/glide_computer_lib.h)
target_compile_features(glide_computer_lib-headers PUBLIC cxx_std_20)
target_link_libraries(glide_computer_lib-headers PUBLIC mp-units::mp-units example_utils-headers)
target_include_directories(glide_computer_lib-headers PUBLIC include)
if(${projectPrefix}DEV_TIME_TRACE STREQUAL "HEADERS")
    target_compile_options(glide_computer_lib-headers PRIVATE "-ftime-trace")
endif()

if(${projectPrefix}BUILD_CXX_MODULES)
    add_library(glide_computer_lib STATIC glide_computer_lib.cpp include/glide_computer_lib.h)
    target_compile_features(glide_computer_lib PUBLIC cxx_std_20)
    target_compile_definitions(glide_computer_lib PUBLIC ${projectPrefix}MODULES)
    target_link_libraries(glide_computer_lib PUBLIC mp-units::mp-units example_utils)
    target_include_directories(glide_computer_lib PUBLIC include)

    if(${projectPrefix}DEV_TIME_TRACE STREQUAL "MODULES")
        target_compile_options(glide_computer_lib PRIVATE "-ftime-trace")
    endif()
endif()
