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

include(GNUInstallDirs)

#
# Optional integrations with third-party libraries.
#
# An integration adapts a third-party library to mp-units.
#
# The integration HEADERS are dependency-free (guarded by `__has_include`) and are therefore ALWAYS
# shipped - mp-units never takes a runtime dependency on a third-party library. They live in a header
# `FILE_SET` on the always-present `mp-units-integrations` target (installed via `mp-unitsTargets`);
# `mp-units::core` already exposes the install include directory, so no extra include path is needed.
#
# Only the optional C++ module (`mp_units.integrations.<lib>`) is compiled against a library, and
# only when that library happens to be available at configure time (e.g. in the `build_all` developer
# build or any modules build). Those module targets are exported separately (NOT through
# `mp-unitsTargets`), so `find_package(mp-units)` never gains a dependency on a third-party library.
#

# Always-present, dependency-free integration headers (shipped via a header FILE_SET).
add_library(mp-units-integrations INTERFACE)
target_link_libraries(mp-units-integrations INTERFACE mp-units::core)
target_sources(
    mp-units-integrations
    INTERFACE
        FILE_SET
        HEADERS
        BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
        include/mp-units/integrations/blaze.h
        include/mp-units/integrations/eigen.h
        include/mp-units/integrations/glm.h
)
set_target_properties(mp-units-integrations PROPERTIES EXPORT_NAME integrations)
add_library(mp-units::integrations ALIAS mp-units-integrations)

if(MP_UNITS_BUILD_INSTALL)
    install(TARGETS mp-units-integrations EXPORT mp-unitsTargets FILE_SET HEADERS)
endif()

find_package(Eigen3 QUIET)
find_package(glm QUIET)
find_package(blaze QUIET)

#
# add_mp_units_integration(name DEPENDENCY <find_package_name> <imported_target>)
#
function(add_mp_units_integration name)
    set(multiValues DEPENDENCY)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "${multiValues}")
    list(GET ARG_DEPENDENCY 0 dep_package)
    list(GET ARG_DEPENDENCY 1 dep_target)

    if(NOT TARGET ${dep_target})
        message(STATUS "Skipping the 'mp_units.integrations.${name}' integration (${dep_package} not found)")
        return()
    endif()

    set(target mp-units-integrations-${name})
    if(MP_UNITS_TARGET_SCOPE STREQUAL INTERFACE)
        add_library(${target} INTERFACE)
    else()
        add_library(${target})
    endif()

    if(DEFINED CMAKE_CXX_STANDARD AND CMAKE_CXX_STANDARD GREATER 20)
        target_compile_features(${target} ${MP_UNITS_TARGET_SCOPE} cxx_std_${CMAKE_CXX_STANDARD})
    else()
        target_compile_features(${target} ${MP_UNITS_TARGET_SCOPE} cxx_std_20)
    endif()

    # The headers (and their include path) come from `mp-units::integrations`; this target only adds
    # the third-party dependency and, in a modules build, the compiled module.
    target_link_libraries(${target} ${MP_UNITS_TARGET_SCOPE} mp-units::integrations ${dep_target})

    if(MP_UNITS_BUILD_CXX_MODULES)
        target_sources(${target} PUBLIC FILE_SET CXX_MODULES FILES mp-units-integrations-${name}.cpp)
    endif()

    set_target_properties(${target} PROPERTIES EXPORT_NAME integrations-${name})
    add_library(mp-units::integrations-${name} ALIAS ${target})

    if(MP_UNITS_BUILD_INSTALL)
        # The component is a separate CMake package `mp-units-integrations-<name>` so it lives in its
        # own `cmake/` subdirectory where `find_package(mp-units-integrations-<name>)` looks.
        set(export_name mp-units-integrations-${name}-targets)
        set(config_dir ${CMAKE_INSTALL_LIBDIR}/cmake/mp-units-integrations-${name})
        if(MP_UNITS_BUILD_CXX_MODULES)
            install(TARGETS ${target}
                    EXPORT ${export_name}
                    FILE_SET CXX_MODULES
                    DESTINATION ${CMAKE_INSTALL_LIBDIR}/miu
            )
        else()
            install(TARGETS ${target} EXPORT ${export_name})
        endif()
        install(EXPORT ${export_name} DESTINATION ${config_dir} NAMESPACE mp-units::)

        set(MP_UNITS_INTEGRATION_NAME ${name})
        set(MP_UNITS_INTEGRATION_DEP_PACKAGE ${dep_package})
        set(MP_UNITS_INTEGRATION_TARGETS_FILE ${export_name})
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/mp-units-integration-config.cmake.in mp-units-integrations-${name}Config.cmake
            @ONLY
        )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mp-units-integrations-${name}Config.cmake DESTINATION ${config_dir})
    endif()
endfunction()

add_mp_units_integration(eigen DEPENDENCY Eigen3 Eigen3::Eigen)
add_mp_units_integration(glm DEPENDENCY glm glm::glm)
add_mp_units_integration(blaze DEPENDENCY blaze blaze::blaze)
