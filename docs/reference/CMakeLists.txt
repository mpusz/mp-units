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

cmake_minimum_required(VERSION 3.19.0)
project(mp-units-reference-documentations LANGUAGES NONE)

include(JEGPAddStandardeseSources)

set(pdf_title "mp-units - A Physical Quantities and Units library for C++")
set(page_license "MIT License")
set(first_library_chapter "nums")
set(last_library_chapter "qties")
# set(pdf_creator "") # Defaults to "the `user.name` of Git's configuration".
# set(document_number [[\unspec]])
# set(previous_document_number [[\unspec]])
set(cover_title "mp-units Library Reference Documentations")
set(reply_to "\\href{${PROJECT_HOMEPAGE_URL}/discussions}{Discussions}, \\href{${PROJECT_HOMEPAGE_URL}/issues}{issues}")
jegp_add_standardese_sources(
    mp-units-reference-documentations
    LIBRARIES intro numbers;quantities
    EXTENSIONS macros_extensions
    # CHECKED TRUE
    PDF_PATH "mp_units.pdf"
)
