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
project(mp-units VERSION 2.2.0 LANGUAGES CXX)

set(projectPrefix MP_UNITS_)

option(${projectPrefix}AS_SYSTEM_HEADERS "Exports library as system headers" OFF)
message(STATUS "${projectPrefix}AS_SYSTEM_HEADERS: ${${projectPrefix}AS_SYSTEM_HEADERS}")

option(${projectPrefix}BUILD_CXX_MODULES "Add C++ modules to the list of default targets" OFF)
message(STATUS "${projectPrefix}BUILD_CXX_MODULES: ${${projectPrefix}BUILD_CXX_MODULES}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(AddUnitsModule)
include(GNUInstallDirs)

if(${projectPrefix}AS_SYSTEM_HEADERS)
    set(unitsAsSystem SYSTEM)
endif()

add_subdirectory(core-macros)
add_subdirectory(core)
add_subdirectory(core-fmt-macros)
add_subdirectory(core-fmt)
add_subdirectory(core-io)
add_subdirectory(systems)
add_subdirectory(utility)

# project-wide wrapper
add_library(mp-units INTERFACE)
target_link_libraries(
    mp-units INTERFACE mp-units::core mp-units::core-io mp-units::core-fmt mp-units::systems mp-units::utility
)
add_library(mp-units::mp-units ALIAS mp-units)
install(TARGETS mp-units EXPORT mp-unitsTargets)

# C++ modules
if(${projectPrefix}BUILD_CXX_MODULES)
    add_library(mp-units-core-module)
    add_library(mp-units::core-module ALIAS mp-units-core-module)
    target_sources(
        mp-units-core-module PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
                                    "mp-units-core.cpp"
    )
    target_compile_definitions(mp-units-core-module PUBLIC ${projectPrefix}MODULES)
    target_link_libraries(mp-units-core-module PRIVATE mp-units::core mp-units::utility PUBLIC mp-units::core-macros)
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18)
            target_compile_definitions(mp-units-core-module PUBLIC _LIBCPP_NO_ABI_TAG)
        endif()
    
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
            target_compile_options(mp-units-core-module PUBLIC "-Wno-include-angled-in-module-purview")
        endif()
    endif()

    add_library(mp-units-core-fmt-module)
    add_library(mp-units::core-fmt-module ALIAS mp-units-core-fmt-module)
    target_sources(
        mp-units-core-fmt-module PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
                                        "mp-units-core-fmt.cpp"
    )
    target_link_libraries(
        mp-units-core-fmt-module PRIVATE mp-units::core-fmt PUBLIC mp-units::core-fmt-macros mp-units::core-module
    )

    add_library(mp-units-core-io-module)
    add_library(mp-units::core-io-module ALIAS mp-units-core-io-module)
    target_sources(
        mp-units-core-io-module PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
                                       "mp-units-core-io.cpp"
    )
    target_link_libraries(mp-units-core-io-module PRIVATE mp-units::core-io PUBLIC mp-units::core-module)

    add_library(mp-units-systems-module)
    add_library(mp-units::systems-module ALIAS mp-units-systems-module)
    target_sources(
        mp-units-systems-module PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES
                                       "mp-units-systems.cpp"
    )
    target_link_libraries(
        mp-units-systems-module PRIVATE mp-units::systems mp-units::utility PUBLIC mp-units::core-module
    )

    add_library(mp-units-modules)
    add_library(mp-units::modules ALIAS mp-units-modules)
    target_sources(
        mp-units-modules PUBLIC FILE_SET CXX_MODULES BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" FILES "mp-units.cpp"
    )
    target_link_libraries(
        mp-units-modules PUBLIC mp-units::core-module mp-units::core-fmt-module mp-units::core-io-module
                                mp-units::systems-module
    )
endif()

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
