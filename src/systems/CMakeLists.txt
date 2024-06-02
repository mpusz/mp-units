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

cmake_minimum_required(VERSION 3.23)

add_mp_units_module(
    systems mp-units-systems
    DEPENDENCIES mp-units::core
    HEADERS include/mp-units/systems/angular/units.h
            include/mp-units/systems/iec80000/binary_prefixes.h
            include/mp-units/systems/iec80000/quantities.h
            include/mp-units/systems/iec80000/unit_symbols.h
            include/mp-units/systems/iec80000/units.h
            include/mp-units/systems/isq/atomic_and_nuclear_physics.h
            include/mp-units/systems/isq/base_quantities.h
            include/mp-units/systems/isq/electromagnetism.h
            include/mp-units/systems/isq/light_and_radiation.h
            include/mp-units/systems/isq/mechanics.h
            include/mp-units/systems/isq/si_quantities.h
            include/mp-units/systems/isq/space_and_time.h
            include/mp-units/systems/isq/thermodynamics.h
            include/mp-units/systems/si/constants.h
            include/mp-units/systems/si/prefixes.h
            include/mp-units/systems/si/unit_symbols.h
            include/mp-units/systems/si/units.h
            include/mp-units/systems/angular.h
            include/mp-units/systems/cgs.h
            include/mp-units/systems/hep.h
            include/mp-units/systems/iau.h
            include/mp-units/systems/iec80000.h
            include/mp-units/systems/imperial.h
            include/mp-units/systems/international.h
            include/mp-units/systems/isq.h
            include/mp-units/systems/isq_angle.h
            include/mp-units/systems/natural.h
            include/mp-units/systems/si.h
            include/mp-units/systems/typographic.h
            include/mp-units/systems/usc.h
    MODULE_INTERFACE_UNIT mp-units-systems.cpp
)

if(NOT ${projectPrefix}API_FREESTANDING)
    target_sources(
        mp-units-systems
        PUBLIC FILE_SET
               HEADERS
               BASE_DIRS
               ${CMAKE_CURRENT_SOURCE_DIR}/include
               FILES
               include/mp-units/systems/angular/math.h
               include/mp-units/systems/si/math.h
               include/mp-units/systems/si/chrono.h
    )
endif()
