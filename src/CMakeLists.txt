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
project(mp-units VERSION 0.8.0 LANGUAGES CXX)

set(projectPrefix UNITS_)

option(${projectPrefix}AS_SYSTEM_HEADERS "Exports library as system headers" OFF)
message(STATUS "${projectPrefix}AS_SYSTEM_HEADERS: ${${projectPrefix}AS_SYSTEM_HEADERS}")

option(${projectPrefix}BUILD_LA "Build code depending on the linear algebra library" ON)
message(STATUS "${projectPrefix}BUILD_LA: ${${projectPrefix}BUILD_LA}")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(AddUnitsModule)
include(GNUInstallDirs)

if(${projectPrefix}AS_SYSTEM_HEADERS)
    set(unitsAsSystem SYSTEM)
endif()

add_subdirectory(core)
add_subdirectory(core-fmt)
add_subdirectory(core-io)
add_subdirectory(systems)

# project-wide wrapper
add_library(mp-units INTERFACE)
target_link_libraries(mp-units INTERFACE mp-units::core mp-units::core-io mp-units::core-fmt mp-units::systems)
add_library(mp-units::mp-units ALIAS mp-units)
install(TARGETS mp-units EXPORT mp-unitsTargets)

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
