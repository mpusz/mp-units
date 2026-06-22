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

add_library(example_utils-headers INTERFACE)
target_sources(
    example_utils-headers
    INTERFACE
        FILE_SET
        HEADERS
        BASE_DIRS
        include
        FILES
        include/geographic.h
        include/measurement.h
)
target_link_libraries(example_utils-headers INTERFACE mp-units::mp-units)

if(MP_UNITS_BUILD_CXX_MODULES)
    add_library(example_utils INTERFACE)
    target_compile_features(example_utils INTERFACE cxx_std_20)
    target_compile_definitions(example_utils INTERFACE MP_UNITS_MODULES)
    target_link_libraries(example_utils INTERFACE example_utils-headers)
endif()

#
# add_example(target <depependencies>...)
#
function(add_example target)
    if(MP_UNITS_BUILD_CXX_MODULES)
        add_executable(${target} ${target}.cpp)
        target_compile_features(${target} PRIVATE cxx_std_20)
        target_compile_definitions(${target} PRIVATE MP_UNITS_MODULES)
        target_link_libraries(${target} PRIVATE mp-units::mp-units ${ARGN})
        if(MP_UNITS_DEV_TIME_TRACE STREQUAL "MODULES")
            target_compile_options(${target} PRIVATE "-ftime-trace")
        endif()
    endif()

    add_executable(${target}-headers ${target}.cpp)
    list(TRANSFORM ARGN APPEND "-headers")
    target_link_libraries(${target}-headers PRIVATE mp-units::mp-units ${ARGN})
    if(MP_UNITS_DEV_TIME_TRACE STREQUAL "HEADERS")
        target_compile_options(${target}-headers PRIVATE "-ftime-trace")
    endif()
endfunction()

add_example(avg_speed)
add_example(capacitor_time_curve)
add_example(currency)
add_example(foot_pound_second)
add_example(glide_computer glide_computer_lib)
add_example(hello_units)
add_example(hw_voltage)
add_example(measurement example_utils)
add_example(si_constants)
add_example(spectroscopy_units)
add_example(storage_tank)
add_example(strong_angular_quantities)
add_example(total_energy)
add_example(unmanned_aerial_vehicle example_utils)

#
# Linear algebra integration examples
#
# The same `linear_algebra.cpp` source is compiled once per available third-party linear algebra
# library (the source self-selects the backend via `__has_include`, and each target links exactly
# one library). The `mp-units::integrations-<backend>` target carries both the adapter header/module
# and the third-party library, so we link it and gate on it. It is defined by `src/integrations`
# only when the corresponding library is found.
function(add_linear_algebra_example backend)
    if(NOT TARGET mp-units::integrations-${backend})
        message(STATUS "Skipping the '${backend}' linear algebra example (integration not available)")
        return()
    endif()
    # module-mode variant: `import mp_units.integrations.<backend>`
    if(MP_UNITS_BUILD_CXX_MODULES)
        add_executable(linear_algebra-${backend} linear_algebra.cpp)
        target_compile_definitions(linear_algebra-${backend} PRIVATE MP_UNITS_MODULES)
        target_link_libraries(linear_algebra-${backend} PRIVATE mp-units::mp-units mp-units::integrations-${backend})
    endif()
    # header-mode variant: `#include <mp-units/integrations/<backend>.h>`
    add_executable(linear_algebra-${backend}-headers linear_algebra.cpp)
    target_link_libraries(
        linear_algebra-${backend}-headers PRIVATE mp-units::mp-units mp-units::integrations-${backend}
    )
endfunction()

add_linear_algebra_example(eigen)
add_linear_algebra_example(glm)
add_linear_algebra_example(blaze)

# The built-in `cartesian_vector` backend ships with `mp-units::mp-units` (no third-party dependency
# and no integration plugin), so it is always built and forced via `MP_UNITS_LA_USE_CARTESIAN`.
if(MP_UNITS_BUILD_CXX_MODULES)
    add_executable(linear_algebra-cartesian linear_algebra.cpp)
    target_compile_definitions(linear_algebra-cartesian PRIVATE MP_UNITS_MODULES MP_UNITS_LA_USE_CARTESIAN)
    target_link_libraries(linear_algebra-cartesian PRIVATE mp-units::mp-units)
endif()
add_executable(linear_algebra-cartesian-headers linear_algebra.cpp)
target_compile_definitions(linear_algebra-cartesian-headers PRIVATE MP_UNITS_LA_USE_CARTESIAN)
target_link_libraries(linear_algebra-cartesian-headers PRIVATE mp-units::mp-units)

add_subdirectory(glide_computer_lib)
add_subdirectory(kalman_filter)
