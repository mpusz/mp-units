cmake_minimum_required(VERSION 3.25.0)
project(mp_units_reference_documentations LANGUAGES NONE)

include(JEGPAddStandardeseSources)

set(pdf_title "Mp-units Library")
set(page_license "MIT License")
set(first_library_chapter "nums")
set(last_library_chapter "qties")
# set(pdf_creator "Johel Ernesto Guerrero Pe\\~{n}a")
# set(document_number [[\unspec]])
# set(previous_document_number [[\unspec]])
set(cover_title "Mp-units Library Reference Documentations")
set(reply_to "\\href{${PROJECT_HOMEPAGE_URL}/discussions}{Discussions}, \\href{${PROJECT_HOMEPAGE_URL}/issues}{issues}")
jegp_add_standardese_sources(
    mp_units_reference_documentations
    LIBRARIES intro numbers;quantities
    EXTENSIONS macros_extensions
    # CHECKED TRUE
    PDF_PATH "mp_units.pdf"
)
