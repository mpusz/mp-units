[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)
[![Travis CI](https://img.shields.io/travis/com/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.com/mpusz/units)
[![Conan stable](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg?version=0.3.1%3Astable)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/0.3.1%3Astable/link)
[![Conan testing](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/_latestVersion)

# `mp-units` - A Units Library for C++

## Summary

`Units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions.

Here is a small example of possible operations:

```cpp
// simple numeric operations
static_assert(10km / 2 == 5km);

// unit conversions
static_assert(1h == 3600s);
static_assert(1km + 1m == 1001m);

// dimension conversions
static_assert(1km / 1s == 1000mps);
static_assert(2kmph * 2h == 4km);
static_assert(2km / 2kmph == 1h);

static_assert(1000 / 1s == 1kHz);

static_assert(10km / 5km == 2);
```

## Usage Overview

The library framework consists of a few concepts: quantities, units, dimensions and their exponents. From the user's
point of view the most important is a `quantity`.

Quantity is a concrete amount of a unit for a specified dimension with a specific representation:

```cpp
units::quantity<units::kilometre, double> d1(123);
auto d2 = 123km;    // units::quantity<units::kilometre, std::int64_t>
```

There are C++ concepts provided for each such quantity type:

```cpp
template<typename T>
concept Length = QuantityOf<T, length>;
```

With that we can easily write a function template like this:

```cpp
constexpr units::Velocity auto avg_speed(units::Length auto d, units::Time auto t)
{
  return d / t;
}
```

Concepts usage in the function above guarantee correctness enforced in compile-time and the
guarantee that no unneeded intermediate conversions will ever be applied no matter which
units the user will decide to pass to such function template:

```cpp
using namespace units;

constexpr quantity<kilometre> distance(220);
constexpr quantity<hour> time(2);
constexpr Velocity speed = avg_speed(distance, time);

static_assert(std::same_as<std::remove_cvref_t<decltype(speed)>, quantity<kilometre_per_hour>>);
static_assert(speed.count() == 110);
```

The units library also tries really hard to printing any quantity in the most user friendly fashion:

```cpp
std::cout << speed << '\n';
std::cout << quantity_cast<metre_per_second>(speed) << '\n';
std::cout << avg_speed(140.mi, 2h) << '\n';
```

Try it on [Compiler Explorer](https://godbolt.org/z/OVpkT1).


## Library design

`mp-units` library design rationale and documentation can be found in [doc/DESIGN.md](doc/DESIGN.md)


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


## Release notes

- 0.4.0 ???
  - Support for derived dimensions in `exp` added
  - Added `pow()` and `sqrt()` operations on quantities
  - `units` removed from a `std::experimental` namespace
  - Downcasting facility refactored so the user does not have to write the boilerplate code anymore
  - From now on base dimensions should inherit from `base_dimension` class template
  - Added unit symbols definitions to `base_dimension` and `derived_unit`
  - Added `coherent_derived_unit` helper
  - Added support for `operator<<` on `quantity`
  - Refactored the way prefixed units are defined

- 0.3.1 Sep 18, 2019
  - cmcstl2 dependency changed to range-v3 0.9.1

- 0.3.0 Sep 16, 2019 (CppCon 2019 design)
  - The design as described on CppCon 2019 talk (<https://youtu.be/0YW6yxkdhlU>)
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
  - gsl-lite dependency removed
  - Fractional dimension exponents support added
  - `QuantityOf` concept introduced
  - `quantity_cast<U, Rep>()` support added

- 0.2.0 July 18, 2019 (C++Now 2019 design)
  - The design as described on C++Now 2019 talk (<https://youtu.be/wKchCktZPHU>)
  - Added C++20 features supported by gcc-9.1 (`std::remove_cvref_t`, down with typename, `std::type_identity`)
  - Compile-time performance optimizations (`type_list`, `common_ratio`, `ratio`, `conditional_t`)

- 0.1.0 May 18, 2019
  - Initial library release
  - Begin semantic versioning
  - The last version to work with gcc-8
