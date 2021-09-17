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

add_library(unit_tests_static_truncating
    quantity_test.cpp
)
if(NOT UNITS_LIBCXX)
    target_sources(unit_tests_static_truncating PRIVATE
        quantity_kind_test.cpp
        quantity_point_kind_test.cpp
    )
endif()
target_link_libraries(unit_tests_static_truncating PRIVATE
    mp-units::mp-units
)
target_compile_options(unit_tests_static_truncating PRIVATE
    $<IF:$<CXX_COMPILER_ID:MSVC>,/wd4242 /wd4244,-Wno-conversion>
)

add_library(unit_tests_static
    cgs_test.cpp
    chrono_test.cpp
    concepts_test.cpp
    custom_rep_test_min_expl.cpp
    custom_unit_test.cpp
    dimension_op_test.cpp
    dimensions_concepts_test.cpp
    fixed_string_test.cpp
    fps_test.cpp
    iec80000_test.cpp
    kind_test.cpp
    math_test.cpp
    point_origin_test.cpp
    ratio_test.cpp
    references_test.cpp
    si_test.cpp
    si_cgs_test.cpp
    si_fps_test.cpp
    si_hep_test.cpp
    symbol_text_test.cpp
    type_list_test.cpp
    unit_test.cpp
    us_test.cpp
)

if(NOT UNITS_LIBCXX)
    target_sources(unit_tests_static PRIVATE
        custom_rep_test_min_impl.cpp
        quantity_point_test.cpp
    )
endif()

target_link_libraries(unit_tests_static PRIVATE
    unit_tests_static_truncating
    mp-units::mp-units
)
