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

add_example(kalman_filter-example_1 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_2 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_3 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_4 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_5 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_6 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_7 mp-units::core-fmt mp-units::si mp-units::utility)
add_example(kalman_filter-example_8 mp-units::core-fmt mp-units::si mp-units::utility)
