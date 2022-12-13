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
add_subdirectory(isq)
add_subdirectory(isq-iec80000)
add_subdirectory(isq-natural)
add_subdirectory(si)
add_subdirectory(si-cgs)
add_subdirectory(si-fps)
add_subdirectory(si-hep)
add_subdirectory(si-iau)
add_subdirectory(si-imperial)
add_subdirectory(si-international)
add_subdirectory(si-typographic)
add_subdirectory(si-uscs)

# wrapper for all the systems
add_library(mp-units-systems INTERFACE)
target_link_libraries(
    mp-units-systems
    INTERFACE
        mp-units::isq
        mp-units::isq-iec80000
        mp-units::isq-natural
        mp-units::si
        mp-units::si-cgs
        mp-units::si-fps
        mp-units::si-hep
        mp-units::si-iau
        mp-units::si-imperial
        mp-units::si-international
        mp-units::si-typographic
        mp-units::si-uscs
)
add_library(mp-units::systems ALIAS mp-units-systems)
set_target_properties(mp-units-systems PROPERTIES EXPORT_NAME systems)
install(TARGETS mp-units-systems EXPORT mp-unitsTargets)
