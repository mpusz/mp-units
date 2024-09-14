cmake_minimum_required(VERSION 3.24.0)
project(mp-units_reference_documentations LANGUAGES NONE)

include(JEGPAddStandardeseSources)

set(pdf_title "mp-units Library")
set(page_license "MIT License")
set(first_library_chapter "qties")
set(last_library_chapter "qties")
set(cover_title "mp-units Library Reference Documentations")
set(reply_to "\\href{${PROJECT_HOMEPAGE_URL}/discussions}{Discussions}, \\href{${PROJECT_HOMEPAGE_URL}/issues}{issues}")
jegp_add_standardese_sources(
    mp-units_reference_documentations
    LIBRARIES intro quantities
    EXTENSIONS macros_extensions
    CHECKED TRUE
    # PDF PATH "mp-units.pdf" #[[EXCLUDE_FROM_MAIN]]
    HTML PATH "mp-units.html" #[[EXCLUDE_FROM_MAIN]]
         SECTION_FILE_STYLE "WithExtension"
         LATEX_REGEX_REPLACE
             # Latex commands.
             [[\\href{([^}]+)}{([^}]+)};HREF(\1)(\2)]]
             # Macros extensions.
             [[\\refcpp{([^}]+)};REFCPP(\1)]]
             [[\\irefcpp{([^}]+)};~(REFCPP(\1))]]
             [[\\refcppx{([^}]+)}{([^}]+)};REFCPPX(\1)(\2)]]
             [[\\irefcppx{([^}]+)}{([^}]+)};~(REFCPPX(\1)(\2))]]
             [[\\refiev{([^}]+)};REFIEV(\1)]]
             [[\\irefiev{([^}]+)};~(REFIEV(\1))]]
             # Main matter and annexes.
             [[\\\"{o};ö]]
         HTML_REGEX_REPLACE
             # Latex commands.
             [[HREF\(([^)]+)\)\(([^)]+)\);<a href="\1">\2</a>]]
             # Macros extensions.
             [[REFCPP\(([^)]+)\);<a href="https://wg21.link/\1">ISOCPP, [\1]</a>]]
             [[REFCPPX\(([^)]+)\)\(([^)]+)\);<a href="https://wg21.link/\1\2">ISOCPP, [\1]</a>]] #
             [[ISOCPP;N4971]]
             [[REFIEV\(([^)]+)\);<a href="https://www.electropedia.org/iev/iev.nsf/display?openform&ievref=\1">IEC 60050, \1</a>]]
             # Main matter and annexes.
)
