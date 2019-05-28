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


add_metabench_test(metabench.data.ratio.create.std_ratio "std::ratio" create_std_ratio.cpp.erb "[1000, 2500, 5000, 7500, 10000]")
add_metabench_test(metabench.data.ratio.create.ratio_type_constexpr "ratio with constexpr" create_ratio_type_constexpr.cpp.erb "[1000, 2500, 5000, 7500, 10000]")
metabench_add_chart(metabench.chart.ratio.create
    TITLE "Creation on 2*N ratios"
    SUBTITLE "(smaller is better)"
    DATASETS
        metabench.data.ratio.create.std_ratio
        metabench.data.ratio.create.ratio_type_constexpr
)

add_metabench_test(metabench.data.ratio.multiply_divide.std_ratio "std::ratio" all_std_ratio.cpp.erb "[10, 50, 100, 500, 1000]")
add_metabench_test(metabench.data.ratio.multiply_divide.ratio_type_constexpr "ratio with constexpr" all_ratio_type_constexpr.cpp.erb "[10, 50, 100, 500, 1000]")
metabench_add_chart(metabench.chart.ratio.multiply_divide
    TITLE "N ratio multiply + divide operations"
    SUBTITLE "(smaller is better)"
    DATASETS
        metabench.data.ratio.multiply_divide.std_ratio
        metabench.data.ratio.multiply_divide.ratio_type_constexpr
)

add_metabench_test(metabench.data.ratio.common_ratio.std_ratio "std::ratio" all_std_ratio.cpp.erb "[10, 50, 100, 500, 1000]")
add_metabench_test(metabench.data.ratio.common_ratio.ratio_type_constexpr "ratio with constexpr" all_ratio_type_constexpr.cpp.erb "[10, 50, 100, 500, 1000]")
metabench_add_chart(metabench.chart.ratio.common_ratio
    TITLE "N common_ratio operations"
    SUBTITLE "(smaller is better)"
    DATASETS
        metabench.data.ratio.common_ratio.std_ratio
        metabench.data.ratio.common_ratio.ratio_type_constexpr
)

add_metabench_test(metabench.data.ratio.all.std_ratio "std::ratio" all_std_ratio.cpp.erb "[10, 50, 100, 500, 1000]")
add_metabench_test(metabench.data.ratio.all.ratio_type_constexpr "ratio with constexpr" all_ratio_type_constexpr.cpp.erb "[10, 50, 100, 500, 1000]")
metabench_add_chart(metabench.chart.ratio.all
    TITLE "N x all ratio operations"
    SUBTITLE "(smaller is better)"
    DATASETS
        metabench.data.ratio.all.std_ratio
        metabench.data.ratio.all.ratio_type_constexpr
)

add_custom_target(metabench.chart.ratio
    DEPENDS
        metabench.chart.ratio.create
        metabench.chart.ratio.multiply_divide
        metabench.chart.ratio.common_ratio
        metabench.chart.ratio.all
)

add_dependencies(metabench metabench.chart.ratio)
