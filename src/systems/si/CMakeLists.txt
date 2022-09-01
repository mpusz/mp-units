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

add_units_module(
    si
    DEPENDENCIES mp-units::isq
    HEADERS include/units/isq/si/absorbed_dose.h
            include/units/isq/si/acceleration.h
            include/units/isq/si/amount_of_substance.h
            include/units/isq/si/angular_acceleration.h
            include/units/isq/si/angular_velocity.h
            include/units/isq/si/area.h
            include/units/isq/si/capacitance.h
            include/units/isq/si/catalytic_activity.h
            include/units/isq/si/charge_density.h
            include/units/isq/si/concentration.h
            include/units/isq/si/conductance.h
            include/units/isq/si/constants.h
            include/units/isq/si/current_density.h
            include/units/isq/si/density.h
            include/units/isq/si/dynamic_viscosity.h
            include/units/isq/si/electric_charge.h
            include/units/isq/si/electric_current.h
            include/units/isq/si/electric_field_strength.h
            include/units/isq/si/energy.h
            include/units/isq/si/energy_density.h
            include/units/isq/si/force.h
            include/units/isq/si/frequency.h
            include/units/isq/si/heat_capacity.h
            include/units/isq/si/inductance.h
            include/units/isq/si/length.h
            include/units/isq/si/luminance.h
            include/units/isq/si/luminous_flux.h
            include/units/isq/si/luminous_intensity.h
            include/units/isq/si/magnetic_flux.h
            include/units/isq/si/magnetic_induction.h
            include/units/isq/si/mass.h
            include/units/isq/si/molar_energy.h
            include/units/isq/si/momentum.h
            include/units/isq/si/permeability.h
            include/units/isq/si/permittivity.h
            include/units/isq/si/power.h
            include/units/isq/si/prefixes.h
            include/units/isq/si/pressure.h
            include/units/isq/si/radioactivity.h
            include/units/isq/si/resistance.h
            include/units/isq/si/si.h
            include/units/isq/si/speed.h
            include/units/isq/si/surface_tension.h
            include/units/isq/si/thermal_conductivity.h
            include/units/isq/si/thermodynamic_temperature.h
            include/units/isq/si/time.h
            include/units/isq/si/torque.h
            include/units/isq/si/voltage.h
            include/units/isq/si/volume.h
)
