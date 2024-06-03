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

cmake_minimum_required(VERSION 3.5)

find_package(Catch2 3 REQUIRED)

add_executable(
    unit_tests_runtime distribution_test.cpp fixed_string_test.cpp fmt_test.cpp math_test.cpp atomic_test.cpp
                       truncation_test.cpp
)
if(${projectPrefix}BUILD_CXX_MODULES)
    target_compile_definitions(unit_tests_runtime PUBLIC ${projectPrefix}MODULES)
endif()
target_link_libraries(unit_tests_runtime PRIVATE mp-units::mp-units Catch2::Catch2WithMain)

if(${projectPrefix}DEV_BUILD_LA)
    find_package(wg21_linear_algebra REQUIRED)
    target_sources(unit_tests_runtime PRIVATE linear_algebra_test.cpp)
    target_link_libraries(unit_tests_runtime PRIVATE wg21_linear_algebra::wg21_linear_algebra)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        unit_tests_runtime PRIVATE /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    )
endif()

include(Catch)
catch_discover_tests(unit_tests_runtime)
