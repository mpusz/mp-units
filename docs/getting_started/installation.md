# Installation

## Clone Repository

To clone the repository you can clone it directly or fork the repo and clone it from your fork.

```shell
git clone https://github.com/mpusz/mp-units.git
```

## Install Dependencies
### Conan

This library assumes that most of the dependencies will be provided by the
[Conan Package Manager](https://conan.io/). If you want to obtain required
dependencies by other means, some modifications to the library's CMake files might be needed.
The rest of the dependencies responsible for documentation generation are provided by
`python3-pip`.


#### Conan quick intro

In case you are not familiar with Conan, to install it (or upgrade) just do:

```shell
pip install -U conan
```

After that, you might need to add a custom profile file for your development environment
in _~/.conan2/profiles_ directory. An example profile can look as follows:

```ini hl_lines="5" title="~/.conan2/profiles/gcc12"
[settings]
arch=x86_64
build_type=Release
compiler=gcc
compiler.cppstd=20
compiler.libcxx=libstdc++11
compiler.version=12
os=Linux

[conf]
tools.build:compiler_executables={"c": "gcc-12", "cpp": "g++-12"}
```

!!! tip "Setting the language version"

    Please note that the **mp-units** library requires at least C++20 to be set in a Conan profile
    or forced via the Conan command line. If we do the former, we will not need to provide
    `-s compiler.cppstd=20` every time we run a Conan command line (as provided in the command
    line instructions below).

!!! tip "Using Ninja as a CMake generator for Conan"

    It is highly recommended to set Ninja as a CMake generator for Conan. To do so, we should
    create a _~/.conan2/global.conf_ file that will set `tools.cmake.cmaketoolchain:generator`
    to one of the Ninja generators. For example:

    ```text title="~/.conan2/global.conf"
    tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
    ```

!!! tip "Separate build folders for different configurations"

    _~/.conan2/global.conf_ file may also set `tools.cmake.cmake_layout:build_folder_vars` which
    [makes working with several compilers or build configurations easier](https://docs.conan.io/2/reference/tools/cmake/cmake_layout.html#multi-setting-option-cmake-layout).
    For example, the below line will force Conan to generate separate CMake presets and folders for
    each compiler and C++ standard version:

    ```text title="~/.conan2/global.conf"
    tools.cmake.cmake_layout:build_folder_vars=["settings.compiler", "settings.compiler.version", "settings.compiler.cppstd"]
    ```

    In such a case, we will need to use a configuration-specific preset name in the Conan instructions
    provided below rather than just `conan-default` and `conan-release`
    (e.g. `conan-gcc-13-23` and `conan-gcc-13-23-release`)

