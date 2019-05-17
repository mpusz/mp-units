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


function(add_metabench_test target name erb_path range)
    metabench_add_dataset(${target} "${erb_path}" "${range}" NAME "${name}")
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_link_libraries(${target}
        PUBLIC
            CONAN_PKG::cmcstl2
            CONAN_PKG::gsl-lite
    )
endfunction()


include(metabench)
if(NOT METABENCH_DIR)
    return()
endif()

enable_testing()

add_metabench_test(metabench.std_ratio "std::ratio" ratio/std_ratio.cpp.erb "[10, 50, 100, 500, 1000]")
add_metabench_test(metabench.ratio_type_constexpr "ratio with constexpr" ratio/ratio_type_constexpr.cpp.erb "[10, 50, 100, 500, 1000]")
metabench_add_chart(metabench.ratio
    TITLE "Ratio calculations on n types"
    SUBTITLE "(smaller is better)"
    DATASETS
        metabench.std_ratio
        metabench.ratio_type_constexpr
)

add_custom_target(metabench DEPENDS metabench.ratio)
