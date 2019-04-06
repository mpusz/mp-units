[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](https://raw.githubusercontent.com/mpusz/units/master/LICENSE)
[![Travis CI](https://img.shields.io/travis/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.org/mpusz/units)
[![AppVeyor](https://img.shields.io/appveyor/ci/mpusz/units/master.svg?label=AppVeyor)](https://ci.appveyor.com/project/mpusz/units)
[![Download](https://api.bintray.com/packages/mpusz/conan-mpusz/units%3Ampusz/images/download.svg)](https://bintray.com/mpusz/conan-mpusz/units%3Ampusz/_latestVersion)

# `units` - Physical Units Library for C++

## Summary

`Units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions. 

Here is a small example of possible operations:

```cpp
static_assert(1000 / 1_s == 1_kHz);
static_assert(1_h == 3600_s);
static_assert(1_km + 1_m == 1001_m);
static_assert(10_km / 5_km == 2);
static_assert(10_km / 2 == 5_km);
static_assert(1_km / 1_s == 1000_mps);
static_assert(2_kmph * 2_h == 4_km);
static_assert(2_km / 2_kmph == 1_h);
```


## Repository structure

That repository contains the following `cmake`-based projects:
 - `./src` - header-only project for `units`
 - `.` - project used for development needs that wraps `./src` project together with
   usage examples and unit tests
 - `./test_package` - library installation and conan package verification
 
Please note that all projects depend on some `cmake` modules in `./cmake` directory.


## Building, testing and installation

For detailed information on project compilation, testing and reuse please refer to
[doc/INSTALL.md](doc/INSTALL.md).


## Library design

`units` library design rationale and documentation can be found in
[doc/DESIGN.md](doc/DESIGN.md) 
