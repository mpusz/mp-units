# The MIT License (MIT)
#
# Copyright (c) 2017 Mateusz Pusz
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


# A path to scripts directory
set(CMAKE_SCRIPTS_ROOT ${CMAKE_CURRENT_LIST_DIR})

# Install provided targets
function(install_targets)
    if(NOT CMAKE_INSTALL_BINDIR)
        set(CMAKE_INSTALL_BINDIR "bin")
    endif()
    if(NOT CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif()
    if(NOT CMAKE_INSTALL_INCLUDEDIR)
        set(CMAKE_INSTALL_INCLUDEDIR "include")
    endif()
    install(TARGETS ${ARGN}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # TODO Remove when CMAKE 3.14
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # TODO Remove when CMAKE 3.14
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # TODO Remove when CMAKE 3.14
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endfunction()

# Generate configuration files and install the package
function(configure_and_install configure_in_file_path namespace version_compare_rules)
    if(NOT APPLE)
        set(CMAKE_INSTALL_RPATH ${ORIGIN})
    endif()

    # prepare installation files
    include(CMakePackageConfigHelpers)
    set(ConfigPackageSource ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
    set(ConfigPackageDestination lib/cmake/${PROJECT_NAME})
    write_basic_package_version_file(
            ${ConfigPackageSource}/${PROJECT_NAME}-config-version.cmake
            COMPATIBILITY ${version_compare_rules})
    configure_package_config_file(${configure_in_file_path}
            ${ConfigPackageSource}/${PROJECT_NAME}-config.cmake
            INSTALL_DESTINATION ${ConfigPackageDestination})

    # install library
    install(EXPORT ${PROJECT_NAME}Targets
            DESTINATION ${ConfigPackageDestination}
            FILE ${PROJECT_NAME}-targets.cmake
            NAMESPACE ${namespace}::
            COMPONENT Devel)
    install(FILES
            "${ConfigPackageSource}/${PROJECT_NAME}-config.cmake"
            "${ConfigPackageSource}/${PROJECT_NAME}-config-version.cmake"
            DESTINATION ${ConfigPackageDestination}
            COMPONENT Devel)

    # local package
    export(EXPORT ${PROJECT_NAME}Targets
           NAMESPACE ${namespace}::
           FILE ${ConfigPackageSource}/${PROJECT_NAME}-targets.cmake)
endfunction()
