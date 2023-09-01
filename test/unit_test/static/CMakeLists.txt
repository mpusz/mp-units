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

cmake_minimum_required(VERSION 3.2)

add_library(unit_tests_static_truncating quantity_test.cpp)
target_link_libraries(unit_tests_static_truncating PRIVATE mp-units::mp-units)
target_compile_options(
    unit_tests_static_truncating PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/wd4242 /wd4244,-Wno-conversion>
)

add_library(
    unit_tests_static
    angular_test.cpp
    cgs_test.cpp
    chrono_test.cpp
    concepts_test.cpp
    # custom_rep_test_min_expl.cpp
    custom_rep_test_min_impl.cpp
    dimension_test.cpp
    fixed_string_test.cpp
    fractional_exponent_quantity.cpp
    hep_test.cpp
    iau_test.cpp
    iec80000_test.cpp
    imperial_test.cpp
    international_test.cpp
    isq_test.cpp
    isq_angle_test.cpp
    # magnitude_test.cpp
    math_test.cpp
    natural_test.cpp
    prime_test.cpp
    quantity_point_test.cpp
    quantity_spec_test.cpp
    ratio_test.cpp
    reference_test.cpp
    si_test.cpp
    symbol_text_test.cpp
    type_list_test.cpp
    typographic_test.cpp
    unit_test.cpp
    unit_symbol_test.cpp
    usc_test.cpp
    zero_test.cpp
)

target_link_libraries(unit_tests_static PRIVATE mp-units::mp-units)
target_link_libraries(unit_tests_static PRIVATE unit_tests_static_truncating mp-units::mp-units)
