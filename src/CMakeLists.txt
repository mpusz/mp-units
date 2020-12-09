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

project(mp-units
    VERSION 0.7.0
    LANGUAGES CXX
)

set(UNITS_DOWNCAST_MODE ON CACHE STRING "Select downcasting mode")
set_property(CACHE UNITS_DOWNCAST_MODE PROPERTY STRINGS AUTO ON OFF)

# set path to custom cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# include common tools and workarounds
include(common/scripts)

find_package(fmt CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

# library definition
add_library(mp-units INTERFACE)
#target_sources(mp-units INTERFACE
#    include/units/dimension.h
#    include/units/quantity.h
#    include/units/unit.h
#
#    include/units/bits/tools.h
#    include/units/bits/type_list.h
#
#    include/units/si/base_dimensions.h
#    include/units/si/frequency.h
#    include/units/si/length.h
#    include/units/si/time.h
#    include/units/si/speed.h
#)
target_compile_features(mp-units INTERFACE cxx_std_20)
target_link_libraries(mp-units
    INTERFACE
        fmt::fmt
        Microsoft.GSL::GSL
)
target_include_directories(mp-units
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(mp-units
        INTERFACE
            range-v3::range-v3
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mp-units
        INTERFACE
            -Wno-non-template-friend
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mp-units
        INTERFACE
            /utf-8  # Specifies both the source character set and the execution character set as UTF-8
    )
endif()

if(UNITS_DOWNCAST_MODE STREQUAL "AUTO")
    message(STATUS "Configuring UNITS_DOWNCAST_MODE=AUTOMATIC")
    target_compile_definitions(mp-units INTERFACE UNITS_DOWNCAST_MODE=2)
elseif(UNITS_DOWNCAST_MODE)
    message(STATUS "Configuring UNITS_DOWNCAST_MODE=ON")
    target_compile_definitions(mp-units INTERFACE UNITS_DOWNCAST_MODE=1)
else()
    message(STATUS "Configuring UNITS_DOWNCAST_MODE=OFF")
    target_compile_definitions(mp-units INTERFACE UNITS_DOWNCAST_MODE=0)
endif()

add_library(mp-units::mp-units ALIAS mp-units)

# installation info
install_targets(mp-units)
install(DIRECTORY include/units
    DESTINATION include
    COMPONENT Devel
)

# generate configuration files and install the package
configure_and_install(../cmake/mp-units-config.cmake.in mp-units SameMajorVersion)
