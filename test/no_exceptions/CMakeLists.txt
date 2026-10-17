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

# A hosted toolchain with exceptions disabled is a common embedded setup, and a different configuration
# from freestanding. Every public header has to compile in it, so each one gets its own translation unit
# built with `-fno-exceptions`.

set(header_targets mp-units-core mp-units-systems mp-units-utility)

set(sources)
foreach(target IN LISTS header_targets)
    get_target_property(headers ${target} HEADER_SET)
    get_target_property(target_dir ${target} SOURCE_DIR)
    foreach(header IN LISTS headers)
        # deprecated wrappers warn on inclusion and are excluded from header-set verification as well
        get_source_file_property(skip "${header}" DIRECTORY "${target_dir}" SKIP_LINTING)
        if(skip)
            continue()
        endif()
        string(REGEX REPLACE "^.*/include/" "" include_path "${header}")
        string(MAKE_C_IDENTIFIER "${include_path}" name)
        set(source "${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp")
        file(CONFIGURE OUTPUT "${source}" CONTENT "#include <${include_path}>\n")
        list(APPEND sources "${source}")
    endforeach()
endforeach()

add_library(unit_tests_no_exceptions OBJECT ${sources})
target_link_libraries(unit_tests_no_exceptions PRIVATE mp-units::mp-units)
target_compile_options(unit_tests_no_exceptions PRIVATE -fno-exceptions)
