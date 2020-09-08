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

cmake_minimum_required(VERSION 3.12)

option(GENERATE_DOCS "Generate project documenation" ON)
if(NOT GENERATE_DOCS)
    return()
endif()

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/src")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")
set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
set(DOXYFILE_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

# Find all the public headers
file(GLOB_RECURSE UNITS_PUBLIC_HEADERS ${DOXYGEN_INPUT_DIR}/*.h)

# Replace variables inside @@ with the current values
configure_file("${DOXYFILE_IN}" "${DOXYFILE_OUT}" @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIR}")

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT "${DOXYGEN_INDEX_FILE}"
                   DEPENDS ${UNITS_PUBLIC_HEADERS}
                   COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE_OUT}"
                   MAIN_DEPENDENCY "${DOXYFILE_OUT}" "${DOXYFILE_IN}"
                   COMMENT "Generating doxygen XML metadata"
                   VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(doxygen ALL DEPENDS "${DOXYGEN_INDEX_FILE}")

set(SPHINX_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")
set(SPHINX_BUILD "${CMAKE_CURRENT_BINARY_DIR}/sphinx")
set(SPHINX_INDEX_FILE "${SPHINX_BUILD}/index.html")

# Only regenerate Sphinx when:
# - Doxygen has rerun
# - Our doc files have been updated
# - The Sphinx config has been updated
add_custom_command(OUTPUT "${SPHINX_INDEX_FILE}"
                   COMMAND
                       "${SPHINX_EXECUTABLE}" -b html -j auto "-Dbreathe_projects.mp-units=${DOXYGEN_OUTPUT_DIR}/xml" "${SPHINX_SOURCE}" "${SPHINX_BUILD}"
                   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                   DEPENDS
                       "${CMAKE_CURRENT_SOURCE_DIR}/_static/css/custom.css"
                       "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/concepts.png"
                       "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/downcast_1.png"
                       "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/downcast_2.png"
                       "${CMAKE_CURRENT_SOURCE_DIR}/_static/img/units.png"

                       "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md"

                       "${CMAKE_CURRENT_SOURCE_DIR}/design.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/design/directories.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/design/downcasting.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/design/quantity.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/examples.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/avg_speed.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/box_example.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/capacitor_time_curve.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/clcpp_response.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/conversion_factor.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/foot_pound_second.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/glide_computer.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/hello_units.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter-alpha_beta_filter_example2.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/linear_algebra.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/measurement.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/total_energy.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/unknown_dimension.rst"
                       
                       "${CMAKE_CURRENT_SOURCE_DIR}/faq.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/framework.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/basic_concepts.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/constants.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/conversions_and_casting.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/dimensions.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantities.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantity_points.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/text_output.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/units.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/genindex.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/glossary.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/index.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/introduction.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/quick_start.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/concepts.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/customization_points.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/functions.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/metafunctions.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/dimensions.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/prefixes.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/quantity_point.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/units.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/basic_fixed_string.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/basic_symbol_text.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/core/types/utilities/ratio.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/math.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/random.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/data.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/data/bitrate.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/data/information.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/data/prefixes.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/acceleration.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/area.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/energy.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/force.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/mass.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/power.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/pressure.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/speed.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/cgs/time.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/dimensions_and_concepts.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/acceleration.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/area.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/density.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/energy.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/force.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/mass.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/power.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/pressure.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/speed.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/time.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/fps/volume.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/iau.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/iau/length.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/imperial.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/imperial/length.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/international.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/international/area.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/international/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/international/speed.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/international/volume.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/natural.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/natural/constants.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/natural/dimensions.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/natural/units.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/absorbed_dose.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/acceleration.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/area.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/capacitance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/catalytic_activity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/charge_density.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/concentration.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/conductance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/constants.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/current_density.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/current.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/density.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/dynamic_viscosity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/electric_charge.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/electric_field_strength.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/energy.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/force.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/frequency.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/heat_capacity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/inductance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/length.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/luminance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/luminous_intensity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/magnetic_flux.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/magnetic_induction.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/mass.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/molar_energy.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/momentum.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/permeability.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/permittivity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/power.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/prefixes.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/pressure.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/resistance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/speed.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/substance.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/surface_tension.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/temperature.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/thermal_conductivity.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/time.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/voltage.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/si/volume.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/typographic.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/typographic/length.rst"

                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/us.rst"
                    #    "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems/physical/us/length.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/references.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/custom_representation_types.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/extensions.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/legacy_interfaces.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/linear_algebra.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/natural_units.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/use_cases/unknown_dimensions.rst"

                       "${CMAKE_CURRENT_SOURCE_DIR}/usage.rst"
                       "${DOXYGEN_INDEX_FILE}"
                   MAIN_DEPENDENCY "${SPHINX_SOURCE}/conf.py"
                   COMMENT "Generating documentation with Sphinx")

# Nice named target so we can run the job easily
add_custom_target(sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})

# Add an install target to install the docs
include(GNUInstallDirs)
install(DIRECTORY ${SPHINX_BUILD}
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)
