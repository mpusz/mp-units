# Installation Guide

## Installation and Reuse

There are a few different ways of installing/reusing `units` in your project.

### Copy

As `units` is a header-only library you can simply copy `src/include` directory to
your source tree and use it as regular header files.

NOTE: Until C++20 arrives the library has some 3rd party dependencies that provide
experimental C++20 features. The list of dependncies include:
- `range-v3@ericniebler`

All of them are easily obtained with `conan`.

### cmake + conan

To use `units` as a `cmake` imported library via `cmake` configuration files the following
steps may be done:
- add the following remotes to your local `conan` instance

  ```shell
  conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz
  ```

- add `units` as a dependency to your `conan` file. For example to use testing version of
  `0.4.0` of `mp-units` add:
  - `conanfile.txt`
  
    ```text
    [requires]
    mp-units/0.4.0@mpusz/testing
    ```

  - `conanfile.py`

    ```python
    requires = "mp-units/0.4.0@mpusz/testing"
    ```

- link your `cmake` target with units

  ```text
  target_link_libraries(<your_target> PUBLIC|PRIVATE|INTERFACE CONAN_PKG::mp-units)
  ```

- install `conan` dependencies before configuring cmake

  ```shell
  cd build
  conan install .. -pr <your_conan_profile> -s cppstd=20 -b=outdated -u
  ```


## Full build and unit testing

In case you would like to build all the code in that repository (with unit tests and examples)
you should use the `CMakeLists.txt` from the parent directory.

```shell
mkdir build && cd build
conan install .. <your_profile_and_settings> -s cppstd=20
cmake .. <your_cmake_configuration>
cmake --build .
```


## Packaging

To create a `conan` package and test `cmake` installation and `conan` packaging run:  

```shell
conan create . <username>/<channel> -s cppstd=20 -b=outdated <your_profile_and_settings>
```


## Upload package to conan server

```shell
conan upload -r <remote-name> --all mp-units/0.4.0@<user>/<channel>
```
