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
check_libcxx_in_use(MP_UNITS_LIBCXX)

# project build options
option(MP_UNITS_BUILD_CXX_MODULES "Add C++ modules to the list of default targets" OFF)
option(MP_UNITS_BUILD_INSTALL "Install the library" ON)

message(STATUS "MP_UNITS_BUILD_CXX_MODULES: ${MP_UNITS_BUILD_CXX_MODULES}")
message(STATUS "MP_UNITS_BUILD_INSTALL: ${MP_UNITS_BUILD_INSTALL}")

# check for C++ features
check_cxx_feature_supported(__cpp_lib_format MP_UNITS_LIB_FORMAT_SUPPORTED)
check_cxx_feature_supported(__cpp_explicit_this_parameter MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED)
check_cxx_feature_supported("__cpp_constexpr_exceptions >= 202411L" MP_UNITS_CONSTEXPR_EXCEPTIONS_SUPPORTED)

# libc++ has a basic supports for std::format but does not set __cpp_lib_format
# https://github.com/llvm/llvm-project/issues/77773
if(NOT MP_UNITS_LIB_FORMAT_SUPPORTED AND MP_UNITS_LIBCXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17")
        message(STATUS "Clang 17+ with libc++ detected, overriding `std::format` support")
        set(MP_UNITS_LIB_FORMAT_SUPPORTED ON)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "16")
        message(STATUS "AppleClang 16+ with libc++ detected, overriding `std::format` support")
        set(MP_UNITS_LIB_FORMAT_SUPPORTED ON)
    endif()
endif()

# clang++-18 supports explicit `this` parameter
# https://github.com/llvm/llvm-project/issues/82780
# AppleClang 17 also supports explicit `this` parameter (feature check macro unimplemented)
# https://developer.apple.com/documentation/xcode-release-notes/xcode-16_3-release-notes
# MSVC 19.32 also supports explicit `this` parameter (feature check macro unimplemented in this version)
# https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-notes-v17.2
if(NOT MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED AND CMAKE_CXX_STANDARD GREATER_EQUAL 23)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "18")
        message(STATUS "Clang 18+ detected, overriding `no CRTP` support")
        set(MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17")
        message(STATUS "AppleClang 17+ detected, overriding `no CRTP` support")
        set(MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED ON)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.32")
        message(STATUS "MSVC 19.32+ detected, overriding `no CRTP` support")
        set(MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED ON)
    endif()
endif()

# project API settings
option(MP_UNITS_API_STD_FORMAT "Enable `std::format` support" ${MP_UNITS_LIB_FORMAT_SUPPORTED})
option(MP_UNITS_API_NO_CRTP "Enable class definitions without CRTP idiom" ${MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED})
option(MP_UNITS_API_THROWING_CONSTRAINTS "Enable throwing constraints" MP_UNITS_CONSTEXPR_EXCEPTIONS_SUPPORTED)
option(MP_UNITS_API_FREESTANDING "Builds only freestanding part of the library" OFF)
set(MP_UNITS_API_CONTRACTS GSL-LITE CACHE STRING "Enable contract checking")
check_cache_var_values(MP_UNITS_API_CONTRACTS NONE GSL-LITE MS-GSL)
option(MP_UNITS_API_NATURAL_UNITS "Enables natural units support" ON)

message(STATUS "MP_UNITS_API_STD_FORMAT: ${MP_UNITS_API_STD_FORMAT}")
message(STATUS "MP_UNITS_API_NO_CRTP: ${MP_UNITS_API_NO_CRTP}")
message(STATUS "MP_UNITS_API_THROWING_CONSTRAINTS: ${MP_UNITS_API_THROWING_CONSTRAINTS}")
message(STATUS "MP_UNITS_API_FREESTANDING: ${MP_UNITS_API_FREESTANDING}")
message(STATUS "MP_UNITS_API_CONTRACTS: ${MP_UNITS_API_CONTRACTS}")
message(STATUS "MP_UNITS_API_NATURAL_UNITS: ${MP_UNITS_API_NATURAL_UNITS}")

# validate options
if(MP_UNITS_API_FREESTANDING AND NOT MP_UNITS_API_CONTRACTS STREQUAL "NONE")
    message(FATAL_ERROR "'MP_UNITS_API_CONTRACTS' should be set to 'NONE' for a freestanding build")
endif()

if(NOT MP_UNITS_API_FREESTANDING AND MP_UNITS_API_STD_FORMAT AND NOT MP_UNITS_LIB_FORMAT_SUPPORTED)
    message(FATAL_ERROR "`std::format` enabled but not supported")
endif()

if(MP_UNITS_API_NO_CRTP AND NOT MP_UNITS_EXPLICIT_THIS_PARAMETER_SUPPORTED)
    message(FATAL_ERROR "`NO_CRTP` mode enabled but explicit `this` parameter is not supported")
endif()

if(MP_UNITS_BUILD_CXX_MODULES)
    if(CMAKE_VERSION VERSION_LESS "3.29")
        message(FATAL_ERROR "CMake versions before 3.29 do not support C++ modules properly")
    endif()
    set(MP_UNITS_TARGET_SCOPE "PUBLIC")
else()
    set(MP_UNITS_TARGET_SCOPE "INTERFACE")
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

if(MP_UNITS_BUILD_INSTALL)
    # local build
    export(EXPORT mp-unitsTargets NAMESPACE mp-units::)
    configure_file(mp-unitsConfig.cmake.in mp-unitsConfig.cmake @ONLY)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(mp-unitsConfigVersion.cmake COMPATIBILITY SameMajorVersion)

    # installation
    install(EXPORT mp-unitsTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-units NAMESPACE mp-units::)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mp-unitsConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/mp-unitsConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mp-units
    )
endif()
