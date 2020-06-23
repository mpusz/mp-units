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

function(add_example target)
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} PRIVATE mp::units)
endfunction()

add_example(avg_speed)
add_example(hello_units)
add_example(measurement)
add_example(total_energy)
add_example(unknown_dimension)
add_example(box_example)
add_example(capacitor_time_curve)
add_example(clcpp_response)
add_example(conversion_factor)
add_example(kalman_filter-alpha_beta_filter_example2)
add_example(experimental_angle)
add_example(chrono_duration_compat)

conan_check_testing(linear_algebra)
add_example(linear_algebra)
target_link_libraries(linear_algebra
    PRIVATE
        $<IF:$<TARGET_EXISTS:CONAN_PKG::linear_algebra>,CONAN_PKG::linear_algebra,linear_algebra::linear_algebra>
)

add_subdirectory(alternative_namespaces)
