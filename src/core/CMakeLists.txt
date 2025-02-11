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

function(set_feature_flag name)
    if(NOT ${projectPrefix}${name} STREQUAL "AUTO")
        target_compile_definitions(
            mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}${name}=$<BOOL:${${projectPrefix}${name}}>
        )
    endif()
endfunction()

# core library definition
add_mp_units_module(
    core mp-units-core
    HEADERS include/mp-units/bits/constexpr_math.h
            include/mp-units/bits/core_gmf.h
            include/mp-units/bits/get_associated_quantity.h
            include/mp-units/bits/hacks.h
            include/mp-units/bits/math_concepts.h
            include/mp-units/bits/module_macros.h
            include/mp-units/bits/quantity_spec_hierarchy.h
            include/mp-units/bits/ratio.h
            include/mp-units/bits/sudo_cast.h
            include/mp-units/bits/text_tools.h
            include/mp-units/bits/type_list.h
            include/mp-units/bits/unit_magnitude.h
            include/mp-units/ext/algorithm.h
            include/mp-units/ext/contracts.h
            include/mp-units/ext/fixed_string.h
            include/mp-units/ext/inplace_vector.h
            include/mp-units/ext/prime.h
            include/mp-units/ext/type_name.h
            include/mp-units/ext/type_traits.h
            include/mp-units/framework/compare.h
            include/mp-units/framework/construction_helpers.h
            include/mp-units/framework/customization_points.h
            include/mp-units/framework/dimension.h
            include/mp-units/framework/dimension_concepts.h
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
            include/mp-units/framework/symbolic_expression.h
            include/mp-units/framework/system_reference.h
            include/mp-units/framework/unit.h
            include/mp-units/framework/unit_concepts.h
            include/mp-units/framework/unit_magnitude.h
            include/mp-units/framework/unit_magnitude_concepts.h
            include/mp-units/framework/unit_symbol_formatting.h
            include/mp-units/framework/value_cast.h
            include/mp-units/compat_macros.h
            include/mp-units/concepts.h
            include/mp-units/core.h
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
               include/mp-units/ext/format.h
               include/mp-units/cartesian_vector.h
               include/mp-units/format.h
               include/mp-units/math.h
               include/mp-units/ostream.h
               include/mp-units/random.h
    )
endif()

set_feature_flag(API_STD_FORMAT)
set_feature_flag(API_NO_CRTP)

# Text formatting
if(NOT ${projectPrefix}API_FREESTANDING
   AND (NOT ${projectPrefix}API_STD_FORMAT OR (${projectPrefix}API_STD_FORMAT STREQUAL "AUTO"
                                               AND NOT ${projectPrefix}LIB_FORMAT_SUPPORTED))
)
    if(NOT TARGET fmt::fmt)
        find_package(fmt REQUIRED)
    endif()
    target_link_libraries(mp-units-core ${${projectPrefix}TARGET_SCOPE} fmt::fmt)
endif()

# Contracts checking
target_link_libraries(mp-units-core ${${projectPrefix}TARGET_SCOPE} mp-units-contracts)

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

if(CMAKE_CXX_MODULE_STD)
    target_compile_definitions(mp-units-core ${${projectPrefix}TARGET_SCOPE} ${projectPrefix}IMPORT_STD)
    # https://github.com/llvm/llvm-project/issues/75057
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
        target_compile_options(mp-units-core ${${projectPrefix}TARGET_SCOPE} "-Wno-deprecated-declarations")
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

if(${projectPrefix}DEV_TIME_TRACE STREQUAL "ALL")
    target_compile_options(mp-units-core ${${projectPrefix}TARGET_SCOPE} "-ftime-trace")
elseif(${projectPrefix}DEV_TIME_TRACE STREQUAL "MODULES")
    target_compile_options(mp-units-core PRIVATE "-ftime-trace")
endif()
