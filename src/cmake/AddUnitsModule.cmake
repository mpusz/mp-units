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
# add_units_module(ModuleName
#                  DEPENDENCIES <depependency>...
#                  HEADERS <header_file>...)
#
function(add_units_module name)
    # parse arguments
    set(multiValues DEPENDENCIES HEADERS)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "${multiValues}")

    # validate and process arguments
    validate_unparsed(${name} ARG)
    validate_arguments_exists(${name} ARG DEPENDENCIES HEADERS)

    # define the target for a module
    add_library(mp-units-${name} INTERFACE ${ARG_HEADERS})
    target_link_libraries(mp-units-${name} INTERFACE ${ARG_DEPENDENCIES})
    target_include_directories(
        mp-units-${name} ${unitsAsSystem} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                    $<INSTALL_INTERFACE:include>
    )
    set_target_properties(mp-units-${name} PROPERTIES EXPORT_NAME ${name})
    add_library(mp-units::${name} ALIAS mp-units-${name})

    install(TARGETS mp-units-${name} EXPORT mp-unitsTargets)
    install(DIRECTORY include/units TYPE INCLUDE)
endfunction()
