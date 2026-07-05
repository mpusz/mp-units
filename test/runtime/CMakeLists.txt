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

find_package(Catch2 3 REQUIRED)

add_executable(
    unit_tests_runtime
    atomic_test.cpp
    bounded_quantity_point_test.cpp
    cartesian_tensor_test.cpp
    cartesian_vector_test.cpp
    constrained_test.cpp
    safe_int_test.cpp
    distribution_test.cpp
    fixed_point_test.cpp
    fixed_string_test.cpp
    fmt_test.cpp
    math_test.cpp
    polar_spherical_test.cpp
    quantity_test.cpp
    truncation_test.cpp
)
if(MP_UNITS_BUILD_CXX_MODULES)
    target_compile_definitions(unit_tests_runtime PUBLIC MP_UNITS_MODULES)
endif()
target_link_libraries(unit_tests_runtime PRIVATE mp-units::mp-units Catch2::Catch2WithMain)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        unit_tests_runtime PRIVATE /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    )
endif()

include(Catch)
catch_discover_tests(unit_tests_runtime)

#
# Linear algebra integration tests
#
# The same `linear_algebra_test.cpp` source is compiled once per available third-party linear
# algebra library (the source self-selects the backend via `__has_include`), as a separate
# executable each. The `mp-units::integrations-<backend>` target carries the adapter header and the
# third-party library, and is defined by `src/integrations` only when the library is found.
#
# Tests are header-mode only (consistent with the rest of the suite, which favors textual includes
# for access to implementation details). Module-mode consumption is demonstrated by the example.
function(add_linear_algebra_test backend)
    if(NOT TARGET mp-units::integrations-${backend})
        message(STATUS "Skipping the '${backend}' linear algebra test (integration not available)")
        return()
    endif()
    add_executable(linear_algebra_test-${backend} linear_algebra_test.cpp)
    target_link_libraries(
        linear_algebra_test-${backend} PRIVATE mp-units::mp-units mp-units::integrations-${backend}
                                               Catch2::Catch2WithMain
    )
    catch_discover_tests(linear_algebra_test-${backend})
endfunction()

add_linear_algebra_test(eigen)
add_linear_algebra_test(glm)
add_linear_algebra_test(blaze)

# The built-in `cartesian_vector` backend ships with `mp-units::mp-units` (no third-party dependency
# and no integration plugin), so it is always built and forced via `MP_UNITS_LA_USE_CARTESIAN`. It
# additionally covers integral representations and `constexpr` evaluation.
add_executable(linear_algebra_test-cartesian linear_algebra_test.cpp)
target_compile_definitions(linear_algebra_test-cartesian PRIVATE MP_UNITS_LA_USE_CARTESIAN)
target_link_libraries(linear_algebra_test-cartesian PRIVATE mp-units::mp-units Catch2::Catch2WithMain)
catch_discover_tests(linear_algebra_test-cartesian)
