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
                       "${SPHINX_EXECUTABLE}" -b html -j auto -Dbreathe_projects.mp-units="${DOXYGEN_OUTPUT_DIR}/xml" "${SPHINX_SOURCE}" "${SPHINX_BUILD}"
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
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/hello_units.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/kalman_filter-alpha_beta_filter_example2.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/linear_algebra.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/measurement.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/examples/total_energy.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/faq.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/basic_concepts.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/constants.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/conversions_and_casting.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/dimensions.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/quantities.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/text_output.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/framework/units.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/genindex.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/glossary.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/index.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/introduction.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/quick_start.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/reference/concepts.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/reference/functions.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/reference/systems.rst"
                       "${CMAKE_CURRENT_SOURCE_DIR}/reference/types.rst"
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
