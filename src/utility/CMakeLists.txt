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

# Add-on representation types and helpers layered on top of `core` and `systems`. Everything here is
# beyond the C++29 standardization target (which is `core` = framework + math). Hosted-only for now;
# `constrained`/`safe_int` (the freestanding numerics) stay in `core` pending a separate decision.
add_mp_units_module(
    utility mp-units-utility DEPENDENCIES mp-units::core mp-units::systems MODULE_INTERFACE_UNIT mp-units-utility.cpp
)

if(NOT MP_UNITS_API_FREESTANDING)
    target_sources(
        mp-units-utility
        PUBLIC FILE_SET
               HEADERS
               BASE_DIRS
               ${CMAKE_CURRENT_SOURCE_DIR}/include
               FILES
               include/mp-units/cartesian_vector.h
               include/mp-units/random.h
               include/mp-units/utility/cartesian_tensor.h
               include/mp-units/utility/cartesian_vector.h
               include/mp-units/utility/random.h
    )
endif()

if(MP_UNITS_DEV_TIME_TRACE STREQUAL "MODULES")
    target_compile_options(mp-units-utility PRIVATE "-ftime-trace")
endif()
