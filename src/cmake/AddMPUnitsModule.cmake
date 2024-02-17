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

include_guard(GLOBAL)

function(validate_unparsed module prefix)
    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid arguments '${${prefix}_UNPARSED_ARGUMENTS}' " "for module '${module}'")
    endif()
endfunction()

function(validate_argument_exists module prefix arg)
    if(NOT ${prefix}_${arg})
        message(FATAL_ERROR "'${arg}' not provided for module '${module}'")
    endif()
endfunction()

function(validate_arguments_exists module prefix)
    foreach(arg ${ARGN})
        validate_argument_exists(${module} ${prefix} ${arg})
    endforeach()
endfunction()

#
# add_mp_units_module(Name TargetName
#                     DEPENDENCIES <depependency>...
#                     [HEADERS <header_file>...]
#                     MODULE_INTERFACE_UNIT <miu_file>)
#
function(add_mp_units_module name target_name)
    # parse arguments
    set(oneValue MODULE_INTERFACE_UNIT)
    set(multiValues DEPENDENCIES HEADERS)
    cmake_parse_arguments(PARSE_ARGV 2 ARG "" "${oneValue}" "${multiValues}")

    # validate and process arguments
    validate_unparsed(${name} ARG)
    validate_arguments_exists(${name} ARG DEPENDENCIES MODULE_INTERFACE_UNIT)

    if(${projectPrefix}TARGET_SCOPE STREQUAL INTERFACE)
        set(SCOPE "INTERFACE")
    endif()

    # define the target for a module
    add_library(${target_name} ${SCOPE})
    target_compile_features(${target_name} ${${projectPrefix}TARGET_SCOPE} cxx_std_20)
    target_link_libraries(${target_name} ${${projectPrefix}TARGET_SCOPE} ${ARG_DEPENDENCIES})
    set_target_properties(${target_name} PROPERTIES EXPORT_NAME ${name})

    if(ARG_HEADERS)
        target_sources(
            ${target_name} PUBLIC FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include FILES ${ARG_HEADERS}
        )
    endif()

    if(${projectPrefix}BUILD_CXX_MODULES)
        target_sources(${target_name} PUBLIC FILE_SET CXX_MODULES FILES ${ARG_MODULE_INTERFACE_UNIT})
        install(TARGETS ${target_name}
                EXPORT mp-unitsTargets
                FILE_SET HEADERS FILE_SET CXX_MODULES
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/miu
        )
    else()
        install(TARGETS ${target_name} EXPORT mp-unitsTargets FILE_SET HEADERS)
    endif()

    add_library(mp-units::${name} ALIAS ${target_name})
endfunction()
