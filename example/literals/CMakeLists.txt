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

cmake_minimum_required(VERSION 3.2)

#
# add_example(target <depependencies>...)
#
function(add_example target)
    add_executable(${target}-literals ${target}.cpp)
    target_link_libraries(${target}-literals PRIVATE ${ARGN})
    target_compile_definitions(${target}-literals PRIVATE UNITS_LITERALS)
endfunction()

add_example(avg_speed mp-units::core-io mp-units::si mp-units::si-cgs mp-units::si-international)
add_example(box_example mp-units::core-fmt mp-units::si)
add_example(capacitor_time_curve mp-units::core-io mp-units::si)
add_example(clcpp_response mp-units::core-fmt mp-units::core-io mp-units::si mp-units::si-iau mp-units::si-imperial mp-units::si-international mp-units::si-typographic mp-units::si-us)
add_example(conversion_factor mp-units::core-fmt mp-units::core-io mp-units::si)
add_example(custom_systems mp-units::core-io mp-units::si)
add_example(experimental_angle mp-units::core-fmt mp-units::core-io mp-units::si)
add_example(foot_pound_second mp-units::core-fmt mp-units::si-fps)
add_example(kalman_filter-alpha_beta_filter_example2 mp-units::core-fmt mp-units::si)
add_example(measurement mp-units::core-io mp-units::si)
add_example(total_energy mp-units::core-io mp-units::si mp-units::isq-natural)
add_example(unknown_dimension mp-units::core-io mp-units::si)

if(NOT UNITS_LIBCXX)
    add_executable(glide_computer-literals
        geographic.cpp geographic.h
        glide_computer.cpp glide_computer.h
        glide_computer_example.cpp
    )
    target_link_libraries(glide_computer-literals PRIVATE mp-units::core-fmt mp-units::si mp-units::si-international)
    target_include_directories(glide_computer-literals PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_compile_definitions(glide_computer-literals PRIVATE UNITS_LITERALS)

    find_package(linear_algebra CONFIG REQUIRED)
    add_example(linear_algebra mp-units::core-fmt mp-units::core-io mp-units::si)
    target_link_libraries(linear_algebra-literals PRIVATE linear_algebra::linear_algebra)
endif()
