# Installation and Reuse

There are a few different ways of installing/reusing `units` in your project 

## Copy

As `units` is a header-only library you can simply copy `src/include` directory to
your source tree and use it as regular header files.

NOTE: Until C++20 arrives the library has some 3rd party dependencies that provide
experimental C++20 features. They can be easily obtained with conan

```python
requires = (
    "cmcstl2/2019.03.18@mpusz/stable",
    "gsl-lite/0.33.0@nonstd-lite/stable"
)
```

## cmake

To use `units` as a `cmake` imported library via `cmake` configuration files the following
steps may be done.

### cmake install

```bash
$ mkdir build && cd build
$ cmake ../src -DCMAKE_INSTALL_PREFIX=<your_cmake_installation_dir> <your_cmake_configuration>
$ cmake --build . --target install <your_cmake_configuration>
```

To use such `cmake` target in your project it is enough to add following line to your
`CMakeList.txt` file

```cmake
find_package(units CONFIG REQUIRED)
```

and configure it with

```bash
$ cmake .. -DCMAKE_INSTALL_PREFIX=<your_cmake_installation_dir> <your_cmake_configuration>
```

### cmake + conan

To use `units` with `cmake` via `conan` it is enough to:
- add the following remotes to your local `conan` instance

```bash
$ conan remote add conan-mpusz https://bintray.com/mpusz/conan-mpusz
$ conan remote add conan-nonstd https://api.bintray.com/conan/martinmoene/nonstd-lite
```

- add the following dependency to your `conanfile.txt` or `conanfile.py` files

```python
requires = "mp-units/0.0.1@mpusz/testing"
```

- install conan dependencies before configuring cmake

```bash
$ cd build
$ conan install .. -pr <your_conan_profile> -b=outdated -u
```


# Full build and unit testing

In case you would like to build all the code in that repository (with unit tests and examples)
you should use `CMakeLists.txt` from the parent directory. 

```bash
mkdir build && cd build
conan install .. <your_profile_and_settings>
cmake .. <your_cmake_configuration>
cmake --build .
```


# Packaging

To create a `conan` package and test `cmake` installation and `conan` packaging run:  

```bash
$ conan create . <username>/<channel> --build=outdated <your_profile_and_settings>
```


# Upload package to conan server

```bash
$ conan upload -r <remote-name> --all mp-units/0.0.1@<user>/<channel>
```
