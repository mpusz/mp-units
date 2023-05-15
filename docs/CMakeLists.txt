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

option(${projectPrefix}BUILD_DOCS "Generate project documentation" ON)
message(STATUS "${projectPrefix}BUILD_DOCS: ${${projectPrefix}BUILD_DOCS}")

if(NOT ${projectPrefix}BUILD_DOCS)
    return()
endif()

# set paths to Conan packages (needed for doxygen)
# TODO: remove when Conan will learn how to properly create imported build tools targets
include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

# Find all the public headers
file(GLOB_RECURSE unitsPublicHeaders "${PROJECT_SOURCE_DIR}/src/*.h")

# Sphinx documentation dependencies
set(unitsSphinxDocs
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/css/custom.css"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/concepts.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/dimensions.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/downcast_1.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/downcast_2.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/units.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/quantity_like.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/defining_systems.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/defining_systems/angular_units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/defining_systems/isq.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/defining_systems/si.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/design.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/design/directories.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/design/downcasting.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/design/quantity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/design/quantity_kind.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/avg_speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/box_example.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/capacitor_time_curve.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/clcpp_response.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/experimental_angle.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/foot_pound_second.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/glide_computer.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/hello_units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/total_energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/basics/unknown_dimension.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_representation.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_representation/linear_algebra.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_representation/measurement.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_systems.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_systems/custom_systems.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_utilities.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/custom_utilities/conversion_factor.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_1.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_2.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_3.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_4.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_5.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_6.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_7.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/example_8.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter/kalman.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/faq.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/arithmetics.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/basic_concepts.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/constants.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/conversions_and_casting.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/dimensions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/magnitudes.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantities.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantity_kinds.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantity_like.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantity_points.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/text_output.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/framework/units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/genindex.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/glossary.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/index.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/introduction.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/quick_start.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/concepts.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/customization_points.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/downcasting.rst"
    # "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/functions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/metafunctions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/dimensions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/kinds.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/prefixes.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/reference.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity_kind.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity_point_kind.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity_point.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/basic_fixed_string.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/basic_symbol_text.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/ratio.rst"
    # "${CMAKE_CURRENT_SOURCE_DIR}/reference/math.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/random.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/generic.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/generic/angle.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/generic/dimensionless.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/generic/unknown.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000/binary_prefixes.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000/modulation_rate.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000/storage_capacity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000/traffic_intensity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/iec80000/transfer_rate.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/acceleration.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/constants.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/force.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/mass.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/momentum.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/time.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/natural/units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_cgs.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/acceleration.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/area.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/force.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/mass.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/power.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/pressure.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/cgs/time.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/dimensions_and_concepts.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_fps.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/acceleration.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/area.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/density.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/force.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/mass.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/power.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/pressure.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/time.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/fps/volume.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_iau.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/iau/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_imperial.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/imperial/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_international.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/international/area.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/international/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/international/speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/international/volume.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/absorbed_dose.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/acceleration.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/amount_of_substance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/area.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/capacitance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/catalytic_activity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/charge_density.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/concentration.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/conductance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/constants.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/current_density.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/density.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/dynamic_viscosity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/electric_charge.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/electric_current.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/electric_field_strength.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/force.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/frequency.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/heat_capacity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/inductance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/luminance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/luminous_intensity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/magnetic_flux.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/magnetic_induction.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/mass.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/molar_energy.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/momentum.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/permeability.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/permittivity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/power.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/prefixes.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/pressure.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/resistance.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/speed.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/surface_tension.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/thermal_conductivity.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/thermodynamic_temperature.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/time.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/voltage.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/volume.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_typographic.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/typographic/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si_uscs.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/isq/si/uscs/length.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/references.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/custom_representation_types.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/extensions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/interoperability.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/legacy_interfaces.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/linear_algebra.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/natural_units.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/unknown_dimensions.rst"
    "${CMAKE_CURRENT_SOURCE_DIR}/usage.rst"
)

include(documentation)
include(GNUInstallDirs)

add_documentation(
    documentation ALL
    BREATHE_PROJECT mp-units
    CODE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src"
    INSTALL_DIR ${CMAKE_INSTALL_DOCDIR}
    CODE_DEPENDS ${unitsPublicHeaders}
    DOCS_DEPENDS ${unitsSphinxDocs}
)

add_custom_command(
    TARGET documentation POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/_static/img/*.svg
                                            ${CMAKE_CURRENT_BINARY_DIR}/sphinx/_images
)
