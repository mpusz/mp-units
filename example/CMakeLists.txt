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

add_library(example_utils INTERFACE)
target_include_directories(example_utils INTERFACE include)

#
# add_example(target <depependencies>...)
#
function(add_example target)
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} PRIVATE ${ARGN})
endfunction()

add_example(avg_speed mp-units::core-io mp-units::si mp-units::cgs mp-units::usc)
add_example(capacitor_time_curve mp-units::core-io mp-units::si mp-units::utility)
add_example(
    clcpp_response
    mp-units::core-fmt
    mp-units::core-io
    mp-units::si
    mp-units::iau
    mp-units::imperial
    mp-units::international
    mp-units::typographic
    mp-units::usc
)
add_example(conversion_factor mp-units::core-fmt mp-units::core-io mp-units::si)
add_example(currency mp-units::core-io)
add_example(foot_pound_second mp-units::core-fmt mp-units::international mp-units::imperial)
add_example(glide_computer mp-units::core-fmt mp-units::international mp-units::utility glide_computer_lib)
add_example(hello_units mp-units::core-fmt mp-units::core-io mp-units::si mp-units::usc)
add_example(measurement mp-units::core-io mp-units::si)
add_example(si_constants mp-units::core-fmt mp-units::si)
add_example(spectroscopy_units mp-units::core-fmt mp-units::si)
add_example(storage_tank mp-units::core-fmt mp-units::si mp-units::utility)
add_example(
    strong_angular_quantities mp-units::core-fmt mp-units::core-io mp-units::si mp-units::isq_angle mp-units::utility
)
add_example(total_energy mp-units::core-io mp-units::si mp-units::natural mp-units::utility)
add_example(
    unmanned_aerial_vehicle
    mp-units::core-fmt
    mp-units::core-io
    mp-units::si
    mp-units::international
    mp-units::utility
    example_utils
)

add_subdirectory(glide_computer_lib)
add_subdirectory(kalman_filter)
