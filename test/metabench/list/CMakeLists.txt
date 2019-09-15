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


add_metabench_test(metabench.data.list.type_list.concepts_all "all concepts" type_list_concepts_all.cpp.erb "[3, 6, 9, 12, 15]")
add_metabench_test(metabench.data.list.type_list.concepts_iface "concepts in interface" type_list_concepts_iface.cpp.erb "[3, 6, 9, 12, 15]")
add_metabench_test(metabench.data.list.type_list.concepts_no "no concepts" type_list_concepts_no.cpp.erb "[3, 6, 9, 12, 15]")
metabench_add_chart(metabench.chart.list.concepts
    TITLE "Sorting a list of size N"
    SUBTITLE "(lower is better)"
    DATASETS
        metabench.data.list.type_list.concepts_all
        metabench.data.list.type_list.concepts_iface
        metabench.data.list.type_list.concepts_no
)

add_metabench_test(metabench.data.list.type_list.conditional_std "std::conditional" type_list_conditional_std.cpp.erb "[3, 6, 9, 12, 15]")
add_metabench_test(metabench.data.list.type_list.conditional_alias "alias conditional" type_list_conditional_alias.cpp.erb "[3, 6, 9, 12, 15]")
add_metabench_test(metabench.data.list.type_list.conditional_alias_hard "alias conditional hard" type_list_conditional_alias_hard.cpp.erb "[3, 6, 9, 12, 15]")
metabench_add_chart(metabench.chart.list.conditional
    TITLE "Sorting a list of size N"
    SUBTITLE "(lower is better)"
    DATASETS
        metabench.data.list.type_list.conditional_std
        metabench.data.list.type_list.conditional_alias
        metabench.data.list.type_list.conditional_alias_hard
)

add_custom_target(metabench.chart.list
    DEPENDS
        metabench.chart.list.concepts
        metabench.chart.list.conditional
)

add_dependencies(metabench metabench.chart.list)
