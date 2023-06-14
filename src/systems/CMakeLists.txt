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

cmake_minimum_required(VERSION 3.19)

# systems
add_subdirectory(angular)
add_subdirectory(iec80000)
add_subdirectory(isq)
add_subdirectory(isq_angle)
add_subdirectory(natural)
add_subdirectory(si)
add_subdirectory(cgs)
add_subdirectory(hep)
add_subdirectory(iau)
add_subdirectory(imperial)
add_subdirectory(international)
add_subdirectory(typographic)
add_subdirectory(usc)

# wrapper for all the systems
add_library(mp-units-systems INTERFACE)
target_link_libraries(
    mp-units-systems
    INTERFACE
        mp-units::angular
        mp-units::iec80000
        mp-units::isq
        mp-units::isq_angle
        mp-units::natural
        mp-units::si
        mp-units::cgs
        mp-units::hep
        mp-units::iau
        mp-units::imperial
        mp-units::international
        mp-units::typographic
        mp-units::usc
)
add_library(mp-units::systems ALIAS mp-units-systems)
set_target_properties(mp-units-systems PROPERTIES EXPORT_NAME systems)
install(TARGETS mp-units-systems EXPORT mp-unitsTargets)
