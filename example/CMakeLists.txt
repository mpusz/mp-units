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
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} PRIVATE ${ARGN})
endfunction()

add_example(avg_speed mp-units::si mp-units::si-cgs mp-units::si-international)
add_example(box_example mp-units::si)
add_example(capacitor_time_curve mp-units::si)
add_example(clcpp_response mp-units::si mp-units::si-iau mp-units::si-imperial mp-units::si-international mp-units::si-typographic mp-units::si-us)
add_example(conversion_factor mp-units::si)
add_example(custom_systems mp-units::si)
add_example(experimental_angle mp-units::si)
add_example(foot_pound_second mp-units::si-fps)
add_example(hello_units mp-units::si mp-units::si-international)
add_example(kalman_filter-alpha_beta_filter_example2 mp-units::si)
add_example(measurement mp-units::si)
add_example(total_energy mp-units::si mp-units::isq-natural)
add_example(unknown_dimension mp-units::si)

if(NOT UNITS_LIBCXX)
    add_executable(glide_computer
        geographic.cpp geographic.h
        glide_computer.cpp glide_computer.h
        glide_computer_example.cpp
    )
    target_link_libraries(glide_computer PRIVATE mp-units::si mp-units::si-international)

    find_package(linear_algebra CONFIG REQUIRED)
    add_example(linear_algebra mp-units::si)
    target_link_libraries(linear_algebra PRIVATE linear_algebra::linear_algebra)
endif()

add_subdirectory(alternative_namespaces)
