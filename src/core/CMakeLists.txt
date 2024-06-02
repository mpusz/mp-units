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

cmake_minimum_required(VERSION 3.23)

function(set_feature_flag name)
    set(val_list "TRUE" "FALSE")
    if(${projectPrefix}${name} IN_LIST val_list)
        target_compile_definitions(
            mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}${name}=$<BOOL:${${projectPrefix}${name}}>
        )
    endif()
endfunction()

# core library definition
add_mp_units_module(
    core mp-units-core
    HEADERS include/mp-units/bits/core_gmf.h
            include/mp-units/bits/get_associated_quantity.h
            include/mp-units/bits/get_common_base.h
            include/mp-units/bits/hacks.h
            include/mp-units/bits/math_concepts.h
            include/mp-units/bits/module_macros.h
            include/mp-units/bits/ratio.h
            include/mp-units/bits/sudo_cast.h
            include/mp-units/bits/text_tools.h
            include/mp-units/bits/type_list.h
            include/mp-units/ext/algorithm.h
            include/mp-units/ext/fixed_string.h
            include/mp-units/ext/prime.h
            include/mp-units/ext/type_name.h
            include/mp-units/ext/type_traits.h
            include/mp-units/framework/compare.h
            include/mp-units/framework/customization_points.h
            include/mp-units/framework/dimension.h
            include/mp-units/framework/dimension_concepts.h
            include/mp-units/framework/expression_template.h
            include/mp-units/framework/magnitude.h
            include/mp-units/framework/quantity.h
            include/mp-units/framework/quantity_cast.h
            include/mp-units/framework/quantity_concepts.h
            include/mp-units/framework/quantity_point.h
            include/mp-units/framework/quantity_point_concepts.h
            include/mp-units/framework/quantity_spec.h
            include/mp-units/framework/quantity_spec_concepts.h
            include/mp-units/framework/reference.h
            include/mp-units/framework/reference_concepts.h
            include/mp-units/framework/representation_concepts.h
            include/mp-units/framework/symbol_text.h
            include/mp-units/framework/system_reference.h
            include/mp-units/framework/unit.h
            include/mp-units/framework/unit_concepts.h
            include/mp-units/framework/value_cast.h
            include/mp-units/compat_macros.h
            include/mp-units/concepts.h
            include/mp-units/framework.h
    MODULE_INTERFACE_UNIT mp-units-core.cpp
)

if(NOT ${projectPrefix}API_FREESTANDING)
    target_sources(
        mp-units-core
        PUBLIC FILE_SET
               HEADERS
               BASE_DIRS
               ${CMAKE_CURRENT_SOURCE_DIR}/include
               FILES
               include/mp-units/bits/fmt.h
               include/mp-units/bits/requires_hosted.h
               include/mp-units/math.h
               include/mp-units/ostream.h
               include/mp-units/format.h
               include/mp-units/random.h
    )
endif()

set_feature_flag(API_STD_FORMAT)
set_feature_flag(API_STRING_VIEW_RET)
set_feature_flag(API_NO_CRTP)

# Text formatting
if(NOT ${projectPrefix}API_FREESTANDING
   AND (${projectPrefix}API_STD_FORMAT STREQUAL "FALSE" OR (${projectPrefix}API_STD_FORMAT STREQUAL "AUTO"
                                                            AND NOT ${projectPrefix}LIB_FORMAT_SUPPORTED))
)
    if(NOT TARGET fmt::fmt)
        find_package(fmt REQUIRED)
    endif()
    target_link_libraries(mp-units-core ${${projectPrefix}TARGET_SCOPE} fmt::fmt)
endif()

# Contracts checking
if(${projectPrefix}API_CONTRACTS STREQUAL "NONE")
    target_compile_definitions(mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}API_CONTRACTS=0)
elseif(${projectPrefix}API_CONTRACTS STREQUAL "GSL-LITE")
    if(NOT TARGET gsl::gsl-lite)
        find_package(gsl-lite REQUIRED)
    endif()
    target_link_libraries(mp-units-core ${${projectPrefix}TARGET_SCOPE} gsl::gsl-lite)
    target_compile_definitions(mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}API_CONTRACTS=2)
elseif(${projectPrefix}API_CONTRACTS STREQUAL "MS-GSL")
    if(NOT TARGET Microsoft.GSL::GSL)
        find_package(Microsoft.GSL REQUIRED)
    endif()
    target_link_libraries(mp-units-core ${${projectPrefix}TARGET_SCOPE} Microsoft.GSL::GSL)
    target_compile_definitions(mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}API_CONTRACTS=3)
endif()

# C++20 modules
if(${projectPrefix}BUILD_CXX_MODULES)
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18)
            target_compile_definitions(mp-units-core PUBLIC _LIBCPP_NO_ABI_TAG)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
            target_compile_options(mp-units-core PUBLIC "-Wno-include-angled-in-module-purview")
        endif()
    endif()
endif()

# UTF-8 source and execution character set
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        mp-units-core ${${projectPrefix}TARGET_SCOPE}
        /utf-8 # Specifies both the source character set and the execution character set as UTF-8
    )
endif()

# Freestanding
target_compile_definitions(
    mp-units-core ${${projectPrefix}TARGET_SCOPE}
                  ${projectPrefix}HOSTED=$<NOT:$<BOOL:${${projectPrefix}API_FREESTANDING}>>
)
