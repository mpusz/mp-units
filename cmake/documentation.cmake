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

cmake_minimum_required(VERSION 3.5)

# TODO Switch to CONFIG when Conan will start supporting imported executables in CMakeDeps
find_package(Doxygen MODULE REQUIRED)
find_package(Sphinx REQUIRED)

#
# add_documentation(TargetName [ALL]
#                   BREATHE_PROJECT projectName
#                   [DOXYFILE_TEMPLATE Doxyfile.in]
#                   [CODE_SOURCE_DIR dir]
#                   [DOCS_SOURCE_DIR dir]
#                   [INSTALL_DIR dir]
#                   CODE_DEPENDS codeDeps...
#                   DOC_DEPENDS docDeps...)
#
function(add_documentation targetName)
    set(_options ALL)
    set(_one_value_args BREATHE_PROJECT DOXYFILE_TEMPLATE CODE_SOURCE_DIR DOCS_SOURCE_DIR INSTALL_DIR)
    set(_multi_value_args CODE_DEPENDS DOCS_DEPENDS)
    cmake_parse_arguments(_args "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})

    if(NOT _args_BREATHE_PROJECT)
        message(FATAL_ERROR "BREATHE_PROJECT not provided")
    endif()

    # Validate arguments
    if(NOT _args_DOXYFILE_TEMPLATE)
        set(_args_DOXYFILE_TEMPLATE Doxyfile.in)
    endif()
    get_filename_component(_doxyfileIn ${_args_DOXYFILE_TEMPLATE} REALPATH)
    if(NOT EXISTS ${_doxyfileIn})
        message(FATAL_ERROR "'${_args_DOXYFILE_TEMPLATE}' does not exist")
    endif()

    if(_args_DOCS_SOURCE_DIR)
        if(NOT EXISTS ${_args_DOCS_SOURCE_DIR})
            message(FATAL_ERROR "'${_args_DOCS_SOURCE_DIR}' does not exist")
        endif()
    else()
        set(_args_DOCS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    # Validate CODE_SOURCE_DIR and set input for the configuration file
    if(_args_CODE_SOURCE_DIR)
        if(NOT EXISTS ${_args_CODE_SOURCE_DIR})
            message(FATAL_ERROR "'${_args_CODE_SOURCE_DIR}' does not exist")
        endif()
        set(DOXYGEN_INPUT_DIR "${_args_CODE_SOURCE_DIR}")
    else()
        set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}")
    endif()
    set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

    # Replace variables inside @@ with the current values
    set(_doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    configure_file("${_doxyfileIn}" "${_doxyfile}" @ONLY)

    set(_doxygenIndexFile "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")

    # Only regenerate Doxygen when the Doxyfile or given dependencies change
    add_custom_command(
        OUTPUT "${_doxygenIndexFile}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
        COMMAND Doxygen::doxygen ARGS "${_doxyfile}"
        MAIN_DEPENDENCY "${_doxyfileIn}"
        DEPENDS "${_doxyfile}" "${_args_CODE_DEPENDS}"
        COMMENT "Generating doxygen XML metadata"
        USES_TERMINAL VERBATIM
    )

    set(_sphinx_docs_dir "${CMAKE_CURRENT_BINARY_DIR}/sphinx")
    set(_sphinx_index_file "${_sphinx_docs_dir}/index.html")

    # Only regenerate Sphinx when:
    # - Doxygen has rerun
    # - Our doc files have been updated
    # - The Sphinx config has been updated
    add_custom_command(
        OUTPUT "${_sphinx_index_file}"
        COMMAND "${SPHINX_EXECUTABLE}" ARGS -b html -j auto
                "-Dbreathe_projects.${_args_BREATHE_PROJECT}=${DOXYGEN_OUTPUT_DIR}/xml" "${_args_DOCS_SOURCE_DIR}"
                "${_sphinx_docs_dir}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        MAIN_DEPENDENCY "${_args_DOCS_SOURCE_DIR}/conf.py"
        DEPENDS "${_doxygenIndexFile}" "${_args_DOCS_DEPENDS}"
        COMMENT "Generating documentation with Sphinx"
        USES_TERMINAL VERBATIM
    )

    # Custom target
    if(_args_ALL)
        set(_all ALL)
    endif()
    add_custom_target(${targetName} ${_all} DEPENDS "${_sphinx_index_file}")

    if(_args_INSTALL_DIR)
        # Add an install step to install the docs
        install(DIRECTORY ${_sphinx_docs_dir} TYPE DOC)
    endif()
endfunction()
