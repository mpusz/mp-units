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

# project options
option(${projectPrefix}BUILD_CXX_MODULES "Add C++ modules to the list of default targets" OFF)
message(STATUS "${projectPrefix}BUILD_CXX_MODULES: ${${projectPrefix}BUILD_CXX_MODULES}")

option(${projectPrefix}AS_SYSTEM_HEADERS "Exports library as system headers" OFF)
message(STATUS "${projectPrefix}AS_SYSTEM_HEADERS: ${${projectPrefix}AS_SYSTEM_HEADERS}")

option(${projectPrefix}USE_FMTLIB "Enables usage of fmtlib instead of the 'std::format' facilities" OFF)
message(STATUS "${projectPrefix}USE_FMTLIB: ${${projectPrefix}USE_FMTLIB}")

# C++ features
check_cxx_feature_supported(__cpp_explicit_this_parameter ${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)
check_cxx_feature_supported("__cpp_constexpr >= 202211L" ${projectPrefix}STATIC_CONSTEXPR_VARS_IN_CONSTEXPR_FUNCTIONS)

if(${projectPrefix}EXPLICIT_THIS_PARAMETER_SUPPORTED)
    message(STATUS "API/ABI: `quantity_spec` uses explicit `this` parameter in its definition")
else()
    message(STATUS "API/ABI: `quantity_spec` uses CRTP in its definition")
endif()

if(${projectPrefix}STATIC_CONSTEXPR_VARS_IN_CONSTEXPR_FUNCTIONS)
    message(STATUS "API/ABI: `unit_symbol()` returns `std::string_view`")
else()
    message(STATUS "API/ABI: `unit_symbol()` returns `fixed_string`")
endif()

# C++ modules
if(${projectPrefix}BUILD_CXX_MODULES)
    if(CMAKE_VERSION VERSION_LESS "3.28.1")
        message(FATAL_ERROR, "CMake versions before 3.28.1 do not support C++ modules properly")
    endif()
    cmake_minimum_required(VERSION 3.28.1)

    # # none of the below seems to work
    # cmake_policy(VERSION 3.28.1)
    # cmake_policy(SET CMP0155 NEW)

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
