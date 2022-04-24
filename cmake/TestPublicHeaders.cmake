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

function(get_target_linked_targets target targets_out)
    get_target_property(linked_libs ${target} INTERFACE_LINK_LIBRARIES)
    foreach(linked_lib ${linked_libs})
        if(NOT linked_lib IN_LIST targets)
            if(TARGET ${linked_lib})
                get_target_linked_targets(${linked_lib} child_targets)
                list(APPEND targets ${linked_lib} ${child_targets})
            endif()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES targets)
    list(REMOVE_DUPLICATES link_libs)
    set(${targets_out} ${targets} PARENT_SCOPE)
endfunction()

function(get_target_sources target paths_out)
    get_target_linked_targets(${target} targets)
    list(APPEND targets ${target})

    foreach(t ${targets})
        get_target_property(sources ${t} SOURCES)
        if(sources)
            get_target_property(source_dir ${t} SOURCE_DIR)
            foreach(f ${sources})
                file(REAL_PATH "${f}" path BASE_DIRECTORY "${source_dir}")
                file(RELATIVE_PATH path ${CMAKE_CURRENT_LIST_DIR} "${path}")
                list(APPEND paths "${path}")
            endforeach()
        endif()
    endforeach()

    set(${paths_out} ${paths} PARENT_SCOPE)
endfunction()

include(TestHeaders)

function(add_public_header_test target test_target)
    get_target_sources(${test_target} sources)
    add_header_test(${target} HEADERS ${sources})
    target_link_libraries(${target} PRIVATE ${test_target})
    target_include_directories(${target} PRIVATE .)
endfunction()
