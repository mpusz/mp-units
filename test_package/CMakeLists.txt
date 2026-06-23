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

cmake_minimum_required(VERSION 3.30.5)
project(test_package LANGUAGES CXX)

message(STATUS "MP_UNITS_BUILD_CXX_MODULES: ${MP_UNITS_BUILD_CXX_MODULES}")

find_package(mp-units REQUIRED)

# Consuming a C++ module from an INSTALLED package only works when a single configuration is active.
# Under Ninja Multi-Config with several configurations it hits CMake bug #26312 (transitive synthetic
# targets for imported modules are only hooked up for the first configuration in
# CMAKE_CONFIGURATION_TYPES), so `import mp_units;` fails with "module 'mp_units.core' not found"
# unless the built config happens to be first. The Conan `test_package` therefore pins
# CMAKE_CONFIGURATION_TYPES to the one config it builds (see conanfile.py); other consumers (e.g. the
# unpinned Multi-Config CMake preset) stay in header mode.
# https://gitlab.kitware.com/cmake/cmake/-/issues/26312
get_property(mp_units_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
list(LENGTH CMAKE_CONFIGURATION_TYPES mp_units_num_configs)
if(NOT mp_units_multi_config OR mp_units_num_configs EQUAL 1)
    set(mp_units_single_config TRUE)
else()
    set(mp_units_single_config FALSE)
endif()

add_executable(test_package-headers test_package.cpp)
target_compile_features(test_package-headers PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>)
target_link_libraries(test_package-headers PRIVATE mp-units::mp-units)

# Module-mode consumer (`import mp_units;`). Only built with a single active configuration: under an
# unpinned Multi-Config generator it hits #26312, and it would otherwise just be a header-mode
# duplicate of `test_package-headers`.
if(MP_UNITS_BUILD_CXX_MODULES AND mp_units_single_config)
    add_executable(test_package test_package.cpp)
    target_compile_features(test_package PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>)
    target_compile_definitions(test_package PRIVATE MP_UNITS_MODULES)
    target_link_libraries(test_package PRIVATE mp-units::mp-units)
endif()

#
# Linear algebra integration consumers
#
# For each backing library found, verify the integration can be consumed from the installed
# package: the header always ships (header-mode consumer), and in a modules build the
# `mp_units.integrations.<lib>` module is consumed too (module-mode consumer).
find_package(Eigen3 QUIET)
find_package(glm QUIET)
find_package(blaze QUIET)

# Each integration is verified in header mode (the header always ships and is consumable from any
# package) and, in a single-config modules build, in module mode (`import mp_units.integrations.<lib>`).
# The compiled module ships as a separate `mp-units-integrations-<lib>` package; module-mode consumers
# are built single-config only, for the same #26312 reason as the main `test_package` above.
function(add_integration_test_package backend dependency)
    if(NOT TARGET ${dependency})
        return()
    endif()
    add_executable(test_package_integration-${backend}-headers test_package_integration.cpp)
    target_compile_features(
        test_package_integration-${backend}-headers PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>
    )
    target_link_libraries(
        test_package_integration-${backend}-headers PRIVATE mp-units::integrations mp-units::systems ${dependency}
    )

    if(MP_UNITS_BUILD_CXX_MODULES AND mp_units_single_config)
        find_package(mp-units-integrations-${backend} QUIET)
        if(TARGET mp-units::integrations-${backend})
            add_executable(test_package_integration-${backend} test_package_integration.cpp)
            target_compile_features(
                test_package_integration-${backend} PRIVATE $<IF:$<BOOL:${CMAKE_CXX_MODULE_STD}>,cxx_std_23,cxx_std_20>
            )
            target_compile_definitions(test_package_integration-${backend} PRIVATE MP_UNITS_MODULES)
            target_link_libraries(
                test_package_integration-${backend} PRIVATE mp-units::mp-units mp-units::integrations-${backend}
            )
        endif()
    endif()
endfunction()

add_integration_test_package(eigen Eigen3::Eigen)
add_integration_test_package(glm glm::glm)
add_integration_test_package(blaze blaze::blaze)
