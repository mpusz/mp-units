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
project(mp-units VERSION 2.2.0 LANGUAGES CXX)

set(projectPrefix MP_UNITS_)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CheckCXXFeatureSupported)
include(AddMPUnitsModule)
include(GNUInstallDirs)

# check if libc++ is being used
include(CheckLibcxxInUse)
check_libcxx_in_use(${projectPrefix}LIBCXX)

# project build options
option(${projectPrefix}BUILD_CXX_MODULES "Add C++ modules to the list of default targets" OFF)
message(STATUS "${projectPrefix}BUILD_CXX_MODULES: ${${projectPrefix}BUILD_CXX_MODULES}")

# project API settings
function(cache_var_values name)
    set_property(CACHE ${projectPrefix}${name} PROPERTY STRINGS ${ARGN})
    if(NOT ${projectPrefix}${name} IN_LIST ARGN)
        message(FATAL_ERROR
                    "Invalid value '${${projectPrefix}${name}}' provided for a cache variable ${projectPrefix}${name} (${ARGN} allowed)"
        )
    endif()
    message(STATUS "${projectPrefix}${name}: ${${projectPrefix}${name}}")
endfunction()

set(${projectPrefix}API_STD_FORMAT AUTO CACHE STRING "Enable `std::format` support")
cache_var_values(API_STD_FORMAT AUTO TRUE FALSE)

set(${projectPrefix}API_STRING_VIEW_RET AUTO CACHE STRING
                                                   "Enable returning `std::string_view` from `constexpr` functions"
)
cache_var_values(API_STRING_VIEW_RET AUTO TRUE FALSE)

set(${projectPrefix}API_NO_CRTP AUTO CACHE STRING "Enable class definitions without CRTP idiom")
cache_var_values(API_NO_CRTP AUTO TRUE FALSE)

# C++ features
check_cxx_feature_supported(__cpp_lib_format ${projectPrefix}LIB_FORMAT_SUPPORTED)
check_cxx_feature_supported("__cpp_constexpr >= 202211L" ${projectPrefix}STATIC_CONSTEXPR_VARS_IN_CONSTEXPR_FUNCTIONS)
check_cxx_feature_supported(__cpp_explicit_this_parameter ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)

# validate settings
if(${projectPrefix}API_STD_FORMAT STREQUAL "TRUE"
   AND NOT
       (${projectPrefix}LIB_FORMAT_SUPPORTED
        # libc++ has a basic supports for std::format but does not set __cpp_lib_format
        # https://github.com/llvm/llvm-project/issues/77773
        OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17"
            AND ${projectPrefix}LIBCXX))
)
    message(FATAL_ERROR "`std::format` enabled but not supported")
endif()

if(${projectPrefix}API_STRING_VIEW_RET STREQUAL "TRUE" AND NOT
                                                           ${projectPrefix}STATIC_CONSTEXPR_VARS_IN_CONSTEXPR_FUNCTIONS
)
    message(FATAL_ERROR "`std::string_view` returns enabled but not supported")
endif()

if(${projectPrefix}API_NO_CRTP STREQUAL "TRUE" AND NOT ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)
    message(FATAL_ERROR "`NO_CRTP` mode enabled but explicit `this` parameter is not supported")
endif()

if(${projectPrefix}BUILD_CXX_MODULES STREQUAL "TRUE")
    if(CMAKE_VERSION VERSION_LESS "3.29")
        message(FATAL_ERROR "CMake versions before 3.29 do not support C++ modules properly")
    endif()
    set(${projectPrefix}TARGET_SCOPE "PUBLIC")
else()
    set(${projectPrefix}TARGET_SCOPE "INTERFACE")
endif()

# components/modules
add_subdirectory(core)
add_subdirectory(systems)

# project-wide wrapper
add_mp_units_module(mp-units mp-units DEPENDENCIES mp-units::core mp-units::systems MODULE_INTERFACE_UNIT mp-units.cpp)

# local build
export(EXPORT mp-unitsTargets NAMESPACE mp-units::)
configure_file("mp-unitsConfig.cmake" "." COPYONLY)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(mp-unitsConfigVersion.cmake COMPATIBILITY SameMajorVersion)

# installation
install(EXPORT mp-unitsTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-units NAMESPACE mp-units::)

install(FILES mp-unitsConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/mp-unitsConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-units
)
