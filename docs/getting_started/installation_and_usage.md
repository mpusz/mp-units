# Installation and Usage

This chapter explains how to obtain **mp-units** and build your code with it.


## Obtaining dependencies

Most dependencies are provided via the [Conan Package Manager](https://conan.io/).
If you obtain them differently you may need to adjust some CMake files.


??? info "Conan quick intro"

    Install or upgrade Conan with:

    ```shell
    pip install -U conan
    ```

    Then create a profile for your environment in _~/.conan2/profiles_. Example:

    ```ini hl_lines="5" title="~/.conan2/profiles/gcc12"
    [settings]
    arch=x86_64
    build_type=Release
    compiler=gcc
    compiler.cppstd=20
    compiler.libcxx=libstdc++11
    compiler.version=15
    os=Linux

    [conf]
    tools.build:compiler_executables={"c": "gcc-15", "cpp": "g++-15"}
    ```

    !!! tip "Setting the language version"

        **mp-units** requires at least C++20. Setting it in the profile avoids repeating
        `-s compiler.cppstd=20` in the Conan command line command each time.

    !!! tip "Using Ninja as a CMake generator for Conan"

        Prefer Ninja. You can set the `tools.cmake.cmaketoolchain:generator`
        in _~/.conan2/global.conf_:

        ```text title="~/.conan2/global.conf"
        tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
        ```

    !!! tip "Separate build folders for different configurations"

        Optionally set `tools.cmake.cmake_layout:build_folder_vars` to get separate build
        folders per compiler/std version:

        ```text title="~/.conan2/global.conf"
        tools.cmake.cmake_layout:build_folder_vars=["settings.compiler", "settings.compiler.version", "settings.compiler.cppstd"]
        ```

        Then use a configuration-specific preset name in the Conan instructions
        provided below rather than the default `conan-default` and `conan-release`
        (e.g., `conan-gcc-13-23` and `conan-gcc-13-23-release`)

??? info "CMake with Presets support"

    Use CMake 3.23+ to benefit from Conan-generated presets. Otherwise, you need
    to do something like:

    ```shell
    mkdir build && cd build
    cmake .. -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=<path_to_generators_dir>/conan_toolchain.cmake
    cmake --build . --config Release
    ```


## Build options

!!! note

    Most of the below options are related to the C++ language features available in the compilers.
    Please refer to the [C++ compiler support](cpp_compiler_support.md) chapter to learn more
    about which C++ features are required for each option and which compilers support them.

### Conan options

#### `cxx_modules`

:   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24: `True`/`False`
    (Default: auto from settings)

    Adds C++ modules to the default targets.

#### `import_std` :test_tube:{ title="Experimental" }

:   [:octicons-tag-24: 2.3.0][release-2-3-0] · :octicons-milestone-24: `True`/`False`
    (Default: `False`)

    Enables `import std;` usage.

#### `std_format`

:   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24: `True`/`False`
    (Default: auto from settings)

    Enables [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format)
    and associated facilities for text formatting. If not supported, falls back to
    [{fmt}](https://github.com/fmtlib/fmt).

#### `no_crtp`

:   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24: `True`/`False`
    (Default: auto from settings)

    Avoids CRTP in [`quantity_spec` definitions](../users_guide/framework_basics/systems_of_quantities.md#defining-quantities).

#### `contracts`

:   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24: `none`/`gsl-lite`/`ms-gsl`
    (Default: see below)

    Enables precondition checks and additional assertions.

    If `import_std` defaults to `True`, `contracts` defaults to `none`; otherwise `gsl-lite`.

#### `freestanding`

:   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24: `True`/`False`
    (Default: `False`)

    Configures the library in the [freestanding](https://en.cppreference.com/w/cpp/freestanding)
    mode. When enabled, the library's source code will build with the compiler's
    [`-ffreestanding`](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html) compilation option
    without any issues.

#### `natural_units`

:   [:octicons-tag-24: 2.5.0][release-2-5-0] · :octicons-milestone-24: `ON`/`OFF`
    (Default: `ON`)

    Enables experimental natural units systems.

??? info "CMake options to set when Conan is not being used"

    ### CMake options

    Conan sets all the below CMake options automatically based on its configuration
    (described above).
    Manual setting of the below CMake options is only needed when Conan is not being used.

    [`MP_UNITS_BUILD_CXX_MODULES`](#MP_UNITS_BUILD_CXX_MODULES){ #MP_UNITS_BUILD_CXX_MODULES }

    :   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: `OFF`)

        Adds C++ modules to the list of default targets.

    [`MP_UNITS_BUILD_INSTALL`](#MP_UNITS_BUILD_INSTALL){ #MP_UNITS_BUILD_INSTALL }

    :   [:octicons-tag-24: 2.5.0][release-2-5-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: `ON`)

        Creates an installable target. Users may want to turn this off for example when
        consuming the library via CMake's `add_subdirectory` or similar mechanisms.

    [`MP_UNITS_API_STD_FORMAT`](#MP_UNITS_API_STD_FORMAT){ #MP_UNITS_API_STD_FORMAT }

    :   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: auto)

        Enables the usage of [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format)
        and associated facilities for text formatting. If it is not supported, then
        the [{fmt}](https://github.com/fmtlib/fmt) library is used instead.

    [`MP_UNITS_API_NO_CRTP`](#MP_UNITS_API_NO_CRTP){ #MP_UNITS_API_NO_CRTP }

    :   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: auto)

        Removes the need for the usage of the CRTP idiom in the
        [`quantity_spec` definitions](../users_guide/framework_basics/systems_of_quantities.md#defining-quantities).

    [`MP_UNITS_API_CONTRACTS`](#MP_UNITS_API_CONTRACTS){ #MP_UNITS_API_CONTRACTS }

    :   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24:
        `NONE`/`GSL-LITE`/`MS-GSL` (Default: `GSL-LITE`)

        Enables checking of preconditions and additional asserts in the code.

    [`MP_UNITS_API_FREESTANDING`](#MP_UNITS_API_FREESTANDING){ #MP_UNITS_API_FREESTANDING }

    :   [:octicons-tag-24: 2.2.0][release-2-2-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: `OFF`)

        Configures the library in the [freestanding](https://en.cppreference.com/w/cpp/freestanding)
        mode. When enabled, the library's source code should build with the compiler's
        [`-ffreestanding`](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)
        compilation option without any issues.

    [`MP_UNITS_API_NATURAL_UNITS`](#MP_UNITS_API_NATURAL_UNITS){ #MP_UNITS_API_NATURAL_UNITS }

    :   [:octicons-tag-24: 2.5.0][release-2-5-0] · :octicons-milestone-24:
        `ON`/`OFF` (Default: `ON`)

        Enables experimental natural units systems support.

[release-2-2-0]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0
[release-2-3-0]: https://github.com/mpusz/mp-units/releases/tag/v2.3.0
[release-2-5-0]: https://github.com/mpusz/mp-units/releases/tag/v2.5.0

## Installation and reuse

There are many ways to install or reuse **mp-units**. Below we list a few common approaches.

!!! important "Important: Prefer using Conan if possible"

    The easiest and most recommended way to obtain **mp-units** is with the Conan package manager.
    See [Conan + CMake (release)](#conan-cmake-release) for a detailed instruction.

### Conan + CMake (release) { #conan-cmake-release }

!!! tip

    If you are new to the Conan package manager you may want to read
    [Obtaining Dependencies](#obtaining-dependencies) and refer to the
    [Consuming packages](https://docs.conan.io/2/tutorial/consuming_packages.html)
    chapter of the official Conan documentation for more information.

**mp-units** releases are hosted on [Conan-Center](https://conan.io/center/recipes/mp-units).
The following steps may be performed to obtain an official library release:

1. Create Conan configuration file (either _conanfile.txt_ or _conanfile.py_) in your
   project's top-level directory and add **mp-units** as a dependency of your project.
   For example, the simplest file may look as follows:

    === "_conanfile.txt_"

        ```ini
        [requires]
        mp-units/2.4.0

        [options]
        # The below mp-units options are set to defaults by Conan.
        # Uncomment and set to an explicit value to override the defaults.
        #
        # mp-units*:cxx_modules=True
        # mp-units*:import_std=False
        # mp-units*:std_format=True
        # mp-units*:no_crtp=True
        # mp-units*:contracts=gsl-lite
        # mp-units*:freestanding=False

        [layout]
        cmake_layout

        [generators]
        CMakeToolchain
        CMakeDeps
        ```

    === "_conanfile.py_"

        ```python
        from conan import ConanFile
        from conan.tools.build import can_run
        from conan.tools.cmake import CMake, cmake_layout

        class MPUnitsTestConan(ConanFile):
            settings = "os", "arch", "compiler", "build_type"
            generators = "CMakeDeps", "CMakeToolchain"

            def requirements(self):
                self.requires(
                    "mp-units/2.4.0",
                    options={
                        # The below mp-units options are set to defaults by Conan.
                        # Uncomment and set to an explicit value to override the defaults.
                        #
                        # "cxx_modules": False,
                        # "import_std": False,
                        # "std_format": True,
                        # "no_crtp": True,
                        # "contracts": "gsl-lite",
                        # "freestanding": False,
                    },
                )

            def layout(self):
                cmake_layout(self)

            def build(self):
                cmake = CMake(self)
                cmake.configure()
                cmake.build()
                if can_run(self):
                    cmake.ctest(cli_args=["--output-on-failure"])
        ```

2. Import **mp-units** and its dependencies definitions with `find_package`:

    ```cmake
    find_package(mp-units REQUIRED)
    ```

3. Link your CMake targets with **mp-units**:

    ```cmake
    target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> mp-units::mp-units)
    ```

4. Download, build, and install Conan dependencies before running the CMake configuration step:

    === "_conanfile.txt_ or _conanfile.py_"

        ```shell
        conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
        cmake --preset conan-default
        cmake --build --preset conan-release
        cmake --build --preset conan-release --target test
        ```

    === "_conanfile.py_ only"

        ```shell
        conan build . -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
        ```

### Conan + CMake (Live At Head)

This chapter describes the procedure to Live At Head, which means using the latest stable version
of **mp-units** all the time.

!!! note

    Please note that even though the Conan packages that you will be using are generated **ONLY**
    for builds that are considered stable (passed our CI tests), some minor regressions may happen
    (CI and C++ build environments are not perfect yet). Also, please expect that the library
    interface might, and probably will, change occasionally. Even though we do our best, such
    changes might not be reflected in the project's documentation right away.

The procedure is similar to the one described in [Conan + CMake (release)](#conan-cmake-release)
with the following differences:

1. Before starting the previous procedure, add **mp-units** remote to your Conan configuration:

    ```shell
    conan remote add conan-mpusz https://mpusz.jfrog.io/artifactory/api/conan/conan-oss
    ```

2. In your Conan configuration file, provide the package identifier of the `mpusz/testing`
  stream:

    === "_conanfile.txt_"

        ```ini hl_lines="2"
        [requires]
        mp-units/2.5.0@mpusz/testing

        [options]
        # The below mp-units options are set to defaults by Conan.
        # Uncomment and set to an explicit value to override the defaults.
        #
        # mp-units*:cxx_modules=True
        # mp-units*:import_std=False
        # mp-units*:std_format=True
        # mp-units*:no_crtp=True
        # mp-units*:contracts=gsl-lite
        # mp-units*:freestanding=False

        [layout]
        cmake_layout

        [generators]
        CMakeToolchain
        CMakeDeps
        ```

    === "_conanfile.py_"

        ```python hl_lines="11"
        from conan import ConanFile
        from conan.tools.build import can_run
        from conan.tools.cmake import CMake, cmake_layout

        class MPUnitsTestConan(ConanFile):
            settings = "os", "arch", "compiler", "build_type"
            generators = "CMakeDeps", "CMakeToolchain"

            def requirements(self):
                self.requires(
                    "mp-units/2.5.0@mpusz/testing",
                    options={
                        # The below mp-units options are set to defaults by Conan.
                        # Uncomment and set to an explicit value to override the defaults.
                        #
                        # "cxx_modules": False,
                        # "import_std": False,
                        # "std_format": True,
                        # "no_crtp": True,
                        # "contracts": "gsl-lite",
                        # "freestanding": False,
                    },
                )

            def layout(self):
                cmake_layout(self)

            def build(self):
                cmake = CMake(self)
                cmake.configure()
                cmake.build()
                if can_run(self):
                    cmake.ctest(cli_args=["--output-on-failure"])
        ```

    !!! tip

        The identifiers of the latest packages can always be found in
        [the project's README file](https://github.com/mpusz/mp-units/blob/master/README.md) or on
        [the project's Artifactory](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units).

3. Force Conan to check for updated recipes with `-u`:

    ```shell
    conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing -u
    ```

??? info "Alternative installation scenarios"

    ### Copy

    As **mp-units** is a C++ header-only library you can simply copy all needed _src/*/include_ subdirectories
    to your source tree.

    !!! note

        In such a case, you are on your own to ensure all the dependencies are installed and their header
        files can be located during the build. Please also note that some compiler-specific flags are needed
        to make the code compile without issues.


    ### Copy + CMake

    If you copy the **mp-units** library source code from **the project's _./src_ directory**
    (not the entire repo from its root), you can reuse CMake targets defined by the library.
    To do so, **you should use _CMakeLists.txt_ file from the _./src_ directory**:

    ```cmake
    add_subdirectory(<path_to_mp_units_lib_folder>)
    # ...
    target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> mp-units::mp-units)
    ```

    !!! note

        You are still on your own to make sure all the dependencies are installed and their header and CMake
        configuration files can be located during the build.

    !!! important "Important: Library users should not use the top-level CMake file"

        Top level _CMakeLists.txt_ file should only be used by **mp-units developers and contributors**
        as an entry point for the project's development.
        _./src/CMakeLists.txt_ contains only a pure library definition and **should be used by the
        customers** that prefer to use CMake's
        [`add_subdirectory()`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html) to
        handle the dependencies.

        To learn more about the rationale, please check our
        [FAQ](faq.md#why-dont-we-have-cmake-options-to-disable-the-building-of-tests-and-examples).


    ### CPM or FetchContent (CMake)

    **mp-units** can be consumed via [CPM](https://github.com/cpm-cmake/CPM.cmake) or CMake's built-in
    [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) mechanism. An example
    CPM configuration is shown below.

    ```cmake
    CPMFindPackage(
        NAME mp-units
        VERSION 2.4.0
        GIT_TAG 6e06eddf205deaf6c2f2f63500c8c70ec220a99f
        GITHUB_REPOSITORY mpusz/mp-units
        SOURCE_SUBDIR src
        SYSTEM YES
        EXCLUDE_FROM_ALL YES
        OPTIONS
            "MP_UNITS_BUILD_INSTALL OFF"
    )
    # ...
    target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> mp-units::mp-units)
    ```

    !!! note

        This is essentially a shortcut way to do what [Copy + CMake](#copy-cmake) mentions above, so the same
        prerequisites apply.

    ### Install

    If you don't want to use Conan in your project and just want to install the **mp-units**
    library on your file system, and use `find_package(mp-units)` from another repository to find it;
    it is enough to perform the following steps:

    ```shell
    conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
    mv CMakeUserPresets.json src
    cd src
    cmake --preset conan-default -DCMAKE_INSTALL_PREFIX=<your_installation_path>
    cmake --build --preset conan-release --target install
    ```
