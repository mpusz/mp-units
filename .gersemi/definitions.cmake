# Signatures gersemi cannot read from the real definitions, because those parse their
# arguments in a helper macro. CMake never includes this file.

function(enable_iwyu)
    cmake_parse_arguments(
        PARSE_ARGV 0
        _enable_iwyu
        "QUIET;REQUIRED;ERROR;NO_DEFAULT_MAPPINGS;PCH_IN_CODE;TRANSITIVE_INCLUDES_ONLY;NO_COMMENTS;NO_FORWARD_DECLARATIONS;CXX17_NAMESPACES;QUOTED_INCLUDES_FIRST"
        "PROGRAM;MAPPING_FILE;MAX_LINE_LENGTH"
        "KEEP"
    )
endfunction()
