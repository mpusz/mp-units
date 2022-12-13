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
    isq
    DEPENDENCIES mp-units::core
    HEADERS include/units/isq/dimensions/absorbed_dose.h
            include/units/isq/dimensions/acceleration.h
            include/units/isq/dimensions/amount_of_substance.h
            include/units/isq/dimensions/angular_acceleration.h
            include/units/isq/dimensions/angular_velocity.h
            include/units/isq/dimensions/area.h
            include/units/isq/dimensions/capacitance.h
            include/units/isq/dimensions/catalytic_activity.h
            include/units/isq/dimensions/charge_density.h
            include/units/isq/dimensions/concentration.h
            include/units/isq/dimensions/conductance.h
            include/units/isq/dimensions/current_density.h
            include/units/isq/dimensions/density.h
            include/units/isq/dimensions/dynamic_viscosity.h
            include/units/isq/dimensions/electric_charge.h
            include/units/isq/dimensions/electric_current.h
            include/units/isq/dimensions/electric_field_strength.h
            include/units/isq/dimensions/energy.h
            include/units/isq/dimensions/energy_density.h
            include/units/isq/dimensions/force.h
            include/units/isq/dimensions/frequency.h
            include/units/isq/dimensions/heat_capacity.h
            include/units/isq/dimensions/inductance.h
            include/units/isq/dimensions/length.h
            include/units/isq/dimensions/luminance.h
            include/units/isq/dimensions/luminous_flux.h
            include/units/isq/dimensions/luminous_intensity.h
            include/units/isq/dimensions/magnetic_flux.h
            include/units/isq/dimensions/magnetic_induction.h
            include/units/isq/dimensions/mass.h
            include/units/isq/dimensions/molar_energy.h
            include/units/isq/dimensions/momentum.h
            include/units/isq/dimensions/permeability.h
            include/units/isq/dimensions/permittivity.h
            include/units/isq/dimensions/power.h
            include/units/isq/dimensions/pressure.h
            include/units/isq/dimensions/radioactivity.h
            include/units/isq/dimensions/resistance.h
            include/units/isq/dimensions/speed.h
            include/units/isq/dimensions/surface_tension.h
            include/units/isq/dimensions/thermal_conductivity.h
            include/units/isq/dimensions/thermodynamic_temperature.h
            include/units/isq/dimensions/time.h
            include/units/isq/dimensions/torque.h
            include/units/isq/dimensions/voltage.h
            include/units/isq/dimensions/volume.h
            include/units/isq/dimensions.h
)
