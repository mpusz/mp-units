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

cmake_minimum_required(VERSION 3.5)

# find dependencies
if(NOT TARGET gsl::gsl-lite)
    find_package(gsl-lite CONFIG REQUIRED)
endif()

if(${projectPrefix}BUILD_CXX_MODULES)
    add_library(example_utils INTERFACE)
    target_compile_definitions(example_utils INTERFACE ${projectPrefix}MODULES)
    target_include_directories(example_utils INTERFACE include)
    target_link_libraries(example_utils INTERFACE gsl::gsl-lite)
endif()

add_library(example_utils-headers INTERFACE)
target_include_directories(example_utils-headers INTERFACE include)
target_link_libraries(example_utils-headers INTERFACE gsl::gsl-lite)

#
# add_example(target <depependencies>...)
#
function(add_example target)
    if(${projectPrefix}BUILD_CXX_MODULES)
        add_executable(${target} ${target}.cpp)
        target_compile_definitions(${target} PUBLIC ${projectPrefix}MODULES)
        target_link_libraries(${target} PRIVATE mp-units::mp-units ${ARGN})
    endif()

    add_executable(${target}-headers ${target}.cpp)
    list(TRANSFORM ARGN APPEND "-headers")
    target_link_libraries(${target}-headers PRIVATE mp-units::mp-units ${ARGN})
endfunction()

add_example(avg_speed)
add_example(capacitor_time_curve)
add_example(clcpp_response)
add_example(conversion_factor)
add_example(currency)
add_example(foot_pound_second)
add_example(glide_computer glide_computer_lib)
add_example(hello_units)
add_example(measurement)
add_example(si_constants)
add_example(spectroscopy_units)
add_example(storage_tank)
add_example(strong_angular_quantities)
add_example(total_energy)
add_example(unmanned_aerial_vehicle example_utils)

add_subdirectory(glide_computer_lib)
add_subdirectory(kalman_filter)
