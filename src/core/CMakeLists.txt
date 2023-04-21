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

cmake_minimum_required(VERSION 3.19)

# find dependencies
find_package(gsl-lite CONFIG REQUIRED)

# check if libc++ is being used
include(CheckLibcxxInUse)
check_libcxx_in_use(${projectPrefix}LIBCXX)

# core library definition
add_library(
    mp-units-core
    INTERFACE

    include/mp_units/bits/external/fixed_string.h
    include/mp_units/bits/external/hacks.h
    include/mp_units/bits/external/type_list.h
    include/mp_units/bits/external/type_name.h
    include/mp_units/bits/external/type_traits.h

    include/mp_units/bits/algorithm.h
    include/mp_units/bits/dimension_concepts.h
    include/mp_units/bits/expression_template.h
    include/mp_units/bits/get_associated_quantity.h
    include/mp_units/bits/get_common_base.h
    include/mp_units/bits/magnitude.h
    include/mp_units/bits/math_concepts.h
    include/mp_units/bits/prime.h
    include/mp_units/bits/quantity_cast.h
    include/mp_units/bits/quantity_concepts.h
    include/mp_units/bits/quantity_point_concepts.h
    include/mp_units/bits/quantity_spec_concepts.h
    include/mp_units/bits/ratio.h
    include/mp_units/bits/reference_concepts.h
    include/mp_units/bits/representation_concepts.h
    include/mp_units/bits/sudo_cast.h
    include/mp_units/bits/symbol_text.h
    include/mp_units/bits/text_tools.h
    include/mp_units/bits/unit_concepts.h
    include/mp_units/bits/value_cast.h

    include/mp_units/concepts.h
    include/mp_units/core.h
    include/mp_units/customization_points.h
    include/mp_units/dimension.h
    include/mp_units/quantity.h
    include/mp_units/quantity_point.h
    include/mp_units/quantity_spec.h
    include/mp_units/reference.h
    include/mp_units/system_reference.h
    include/mp_units/unit.h
)
target_compile_features(mp-units-core INTERFACE cxx_std_20)
target_link_libraries(mp-units-core INTERFACE gsl::gsl-lite)
target_include_directories(
    mp-units-core ${unitsAsSystem} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                             $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(${projectPrefix}LIBCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
        find_package(range-v3 CONFIG REQUIRED)
        target_link_libraries(mp-units-core INTERFACE range-v3::range-v3)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        mp-units-core
        INTERFACE /utf-8 # Specifies both the source character set and the execution character set as UTF-8
    )
endif()

set_target_properties(mp-units-core PROPERTIES EXPORT_NAME core)
add_library(mp-units::core ALIAS mp-units-core)

# installation
install(TARGETS mp-units-core EXPORT mp-unitsTargets)
install(DIRECTORY include/units TYPE INCLUDE)
