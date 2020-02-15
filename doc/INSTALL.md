# Installation Guide

## Installation and Reuse

There are a few different ways of installing/reusing `units` in your project.

### Conan quick intro

In case you are not familiar with `conan`, to install it just do:

```shell
pip3 install -U conan
```

After that you might need to add a custom profile in `~/.conan/profile` for your
developnment environment. An example profile can look as follows:

```text
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=9
compiler.cppstd=20
compiler.libcxx=libstdc++11
build_type=Release

[options]
[build_requires]

[env]
CC=/usr/bin/gcc-9
CXX=/usr/bin/g++-9
```

### Copy

As `units` is a header-only library you can simply copy `src/include` directory to
your source tree and use it as regular header files.

NOTE: Until C++20 arrives the library has some 3rd party dependencies that provide
experimental C++20 features. The list of dependencies include:
- `range-v3@ericniebler` (only for gcc-9, gcc-10 uses gcc's concepts implementation)
- `fmt@_`

All of them are easily to obtain with `conan`.

NOTE: In case a full library's repository is to be compiled (instead of just copying
`src/include` headers), additionally, the library's unit tests depend on
`Catch2@catchorg` and `linear_algebra@public-conan` conan packages.

### cmake + conan

To use `units` as a `cmake` imported library via `cmake` configuration files the following
steps may be done:
- clone the repository with its submodules:

  ```shell
  git clone --recurse-submodules https://github.com/mpusz/units.git
  ```
  
  or in case it is already cloned without submodules initialize, fetch, and checkout them with:
  
  ```shell
  git submodule update --init
  ```

- add the following remotes to your local `conan` instance

  ```shell
  conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz
  ```

- add `units` as a dependency to your `conan` file. For example to use testing version of
  `0.5.0` of `mp-units` add:
  - `conanfile.txt`
  
    ```ini
    [requires]
    mp-units/0.5.0@mpusz/testing
    ```

  - `conanfile.py`

    ```python
    requires = "mp-units/0.5.0@mpusz/testing"
    ```

- import `conan` dependencies to top level `CMakeLists.txt` file

  ```cmake
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  ```

- link your `cmake` target with units

  ```cmake
  target_link_libraries(<your_target> PUBLIC|PRIVATE|INTERFACE CONAN_PKG::mp-units)
  ```

- install `conan` dependencies before configuring cmake

  ```shell
  cd build
  conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=outdated -u
  ```


## Full build and unit testing

In case you would like to build all the code in this repository (with unit tests and examples)
you should use the `CMakeLists.txt` from the parent directory and run Conan with
`CONAN_RUN_TESTS=True`.

```shell
git clone --recurse-submodules https://github.com/mpusz/units.git
mkdir units/build && cd units/build
conan remote add linear_algebra https://api.bintray.com/conan/twonington/public-conan
conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -e CONAN_RUN_TESTS=True -b outdated
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest -VV
```


## Packaging

To create a `conan` package and test `cmake` installation and `conan` packaging run:  

```shell
conan create . <username>/<channel> -pr <your_conan_profile> -s compiler.cppstd=20 -e CONAN_RUN_TESTS=True -b outdated
```


## Upload package to conan server

```shell
conan upload -r <remote-name> --all mp-units/0.5.0@<user>/<channel>
```
