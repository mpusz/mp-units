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

cmake_minimum_required(VERSION 3.25)
project(mp-units VERSION 2.5.0 LANGUAGES CXX)

set(projectPrefix MP_UNITS_)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(AddMPUnitsModule)
include(CheckCacheVarValues)
include(CheckCXXFeatureSupported)
include(GNUInstallDirs)

# check if libc++ is being used
include(CheckLibcxxInUse)
check_libcxx_in_use(${projectPrefix}LIBCXX)

# project build options
option(${projectPrefix}BUILD_AS_SYSTEM_HEADERS "Export library as system headers" OFF)
option(${projectPrefix}BUILD_CXX_MODULES "Add C++ modules to the list of default targets" OFF)

message(STATUS "${projectPrefix}BUILD_AS_SYSTEM_HEADERS: ${${projectPrefix}BUILD_AS_SYSTEM_HEADERS}")
message(STATUS "${projectPrefix}BUILD_CXX_MODULES: ${${projectPrefix}BUILD_CXX_MODULES}")

if(${projectPrefix}BUILD_AS_SYSTEM_HEADERS)
    set(${projectPrefix}_AS_SYSTEM SYSTEM)
endif()

# check for C++ features
check_cxx_feature_supported(__cpp_lib_format ${projectPrefix}LIB_FORMAT_SUPPORTED)
check_cxx_feature_supported(__cpp_explicit_this_parameter ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)
check_cxx_feature_supported("__cpp_constexpr_exceptions >= 202411L" ${projectPrefix}CONSTEXPR_EXCEPTIONS_SUPPORTED)

# libc++ has a basic supports for std::format but does not set __cpp_lib_format
# https://github.com/llvm/llvm-project/issues/77773
if(NOT ${projectPrefix}LIB_FORMAT_SUPPORTED AND ${projectPrefix}LIBCXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17")
        message(STATUS "Clang 17+ with libc++ detected, overriding `std::format` support")
        set(${projectPrefix}LIB_FORMAT_SUPPORTED ON)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16")
        message(STATUS "AppleClang 16+ with libc++ detected, overriding `std::format` support")
        set(${projectPrefix}LIB_FORMAT_SUPPORTED ON)
    endif()
endif()

# clang++-18 supports explicit `this` parameter
# https://github.com/llvm/llvm-project/issues/82780
if(NOT ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED
   AND CMAKE_CXX_STANDARD GREATER_EQUAL 23
   AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "18"
)
    message(STATUS "Clang 18+ detected, overriding `no CRTP` support")
    set(${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED ON)
endif()

# project API settings
option(${projectPrefix}API_STD_FORMAT "Enable `std::format` support" ${${projectPrefix}LIB_FORMAT_SUPPORTED})
option(${projectPrefix}API_NO_CRTP "Enable class definitions without CRTP idiom"
       ${${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED}
)
option(${projectPrefix}API_THROWING_CONSTRAINTS "Enable throwing constraints"
       ${projectPrefix}CONSTEXPR_EXCEPTIONS_SUPPORTED
)
option(${projectPrefix}API_FREESTANDING "Builds only freestanding part of the library" OFF)
set(${projectPrefix}API_CONTRACTS GSL-LITE CACHE STRING "Enable contract checking")
check_cache_var_values(API_CONTRACTS NONE GSL-LITE MS-GSL)
option(${projectPrefix}API_NATURAL_UNITS "Enables natural units support" ON)

message(STATUS "${projectPrefix}API_STD_FORMAT: ${${projectPrefix}API_STD_FORMAT}")
message(STATUS "${projectPrefix}API_NO_CRTP: ${${projectPrefix}API_NO_CRTP}")
message(STATUS "${projectPrefix}API_THROWING_CONSTRAINTS: ${${projectPrefix}API_THROWING_CONSTRAINTS}")
message(STATUS "${projectPrefix}API_FREESTANDING: ${${projectPrefix}API_FREESTANDING}")
message(STATUS "${projectPrefix}API_CONTRACTS: ${${projectPrefix}API_CONTRACTS}")
message(STATUS "${projectPrefix}API_NATURAL_UNITS: ${${projectPrefix}API_NATURAL_UNITS}")

# validate options
if(${projectPrefix}API_FREESTANDING AND NOT ${projectPrefix}API_CONTRACTS STREQUAL "NONE")
    message(FATAL_ERROR "'${projectPrefix}API_CONTRACTS' should be set to 'NONE' for a freestanding build")
endif()

if(NOT ${projectPrefix}API_FREESTANDING AND ${projectPrefix}API_STD_FORMAT AND NOT ${projectPrefix}LIB_FORMAT_SUPPORTED)
    message(FATAL_ERROR "`std::format` enabled but not supported")
endif()

if(${projectPrefix}API_NO_CRTP AND NOT ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)
    message(FATAL_ERROR "`NO_CRTP` mode enabled but explicit `this` parameter is not supported")
endif()

if(${projectPrefix}BUILD_CXX_MODULES)
    if(CMAKE_VERSION VERSION_LESS "3.29")
        message(FATAL_ERROR "CMake versions before 3.29 do not support C++ modules properly")
    endif()
    set(${projectPrefix}TARGET_SCOPE "PUBLIC")
else()
    set(${projectPrefix}TARGET_SCOPE "INTERFACE")
endif()

if(CMAKE_CXX_MODULE_STD)
    if(CMAKE_VERSION VERSION_LESS "3.30")
        message(FATAL_ERROR "CMake versions before 3.30 do not support `import std;` properly")
    endif()
    if(CMAKE_CXX_STANDARD LESS 23)
        message(FATAL_ERROR "`import std;` requires at least C++23")
    endif()
endif()

# components/modules
include(MPUnitsContracts)
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
