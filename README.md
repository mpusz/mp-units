[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)
[![Travis CI](https://img.shields.io/travis/com/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.com/mpusz/units)
[![AppVeyor](https://img.shields.io/appveyor/ci/mpusz/units/master.svg?label=AppVeyor)](https://ci.appveyor.com/project/mpusz/units)
[![Conan stable](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg?version=0.3.1%3Astable) ](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/0.3.1%3Astable/link)
[![Conan testing](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg) ](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/_latestVersion)

# `mp-units` - A Units Library for C++

## Summary

`Units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions. 

Here is a small example of possible operations:

```cpp
// simple numeric operations
static_assert(10_km / 2 == 5_km);

// unit conversions
static_assert(1_h == 3600_s);
static_assert(1_km + 1_m == 1001_m);

// dimension conversions
static_assert(1_km / 1_s == 1000_mps);
static_assert(2_kmph * 2_h == 4_km);
static_assert(2_km / 2_kmph == 1_h);

static_assert(1000 / 1_s == 1_kHz);

static_assert(10_km / 5_km == 2);
```

## Repository structure

That repository contains the following independent `cmake`-based projects:
 - `./src` - header-only project for `units`
 - `.` - project used for development needs that wraps `./src` project together with
   usage examples and unit tests
 - `./test_package` - library installation and Conan package verification
 
Please note that the projects depend on `cmake` git submodule in the `./cmake/common`
subdirectory.

## Building, testing, and installation

For a detailed information on project compilation, testing and reuse please refer to
[doc/INSTALL.md](doc/INSTALL.md).

NOTE: This library as of now compiles correctly only with gcc-9.1 and newer.  

## Library design

`units` library design rationale and documentation can be found in
[doc/DESIGN.md](doc/DESIGN.md) 

## Release notes

- 0.4.0 ???
  - Support for derived dimensions in `exp` added
  - Added `pow()` and `sqrt()` operations on quantities
  - `units` removed from a `std::experimental` namespace
  - Downcasting facility refactored so the user does not have to write the boilerplate code anymore

- 0.3.1 Sep 18, 2019
  - cmcstl2 dependency changed to range-v3 0.9.1

- 0.3.0 Sep 16, 2019 (CppCon 2019 design)
  - The design as described on CppCon 2019 talk
  - Applied the feedback from the Cologne evening session
    - `upcasting_traits` renamed to `downcasting_traits`
    - `Dimension` template parameter removed from quantity
  - `units` moved to a `std::experimental` namespace
  - Leading underscore prefix removed from UDLs
  - Added a few more derived dimensions
  - `meter` renamed to `metre`
  - Missing `operator*` added 
  - Predefined dimensions moved to a dedicated directory
  - `dimension_` prefix removed from names of derived dimensions
  - cmcstl2 library updated to 2019.09.19
  - `base_dimension` is a value provided as `const&` to the `exp` type
  - integrated with Compiler Explorer
  - gsl-lite deppendency removed
  - Fractional dimension exponents support added
  - `QuantityOf` concept introduced
  - `quantity_cast<U, Rep>()` support added 

- 0.2.0 July 18, 2019 (C++Now 2019 design)
  - The design as described on C++Now 2019 talk (https://youtu.be/wKchCktZPHU)
  - Added C++20 features supported by gcc-9.1 (`std::remove_cvref_t`, down with typename, `std::type_identity`)
  - Compile-time performance optimisations (`type_list`, `common_ratio`, `ratio`, `conditional_t`)

- 0.1.0 May 18, 2019
  - Initial library release
  - Begin semantic versioning
  - The last version to work with gcc-8
