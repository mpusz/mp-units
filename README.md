[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)
[![Travis CI](https://img.shields.io/travis/com/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.com/mpusz/units)
[![Conan stable](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg?version=0.3.1%3Astable)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/0.3.1%3Astable/link)
[![Conan testing](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/_latestVersion)


# `mp-units` - A Units Library for C++

## TL;DR

This library is the subject of this ISO C++ proposal: [P1935R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1935r2.html). It is explained in this [CppCon 2019 talk](https://youtu.be/0YW6yxkdhlU) (slightly dated see below). We are working towards potentially having it standardised for C++23 and are actively looking for parties interested in field trialling the library. 

```cpp
#include <units/physical/si/velocity.h>
#include <units/physical/international/velocity.h>
#include <iostream>

using namespace units;

constexpr Velocity auto avg_speed(Length auto d, Time auto t)
{
  return d / t;
}

int main()
{
  using namespace si::literals;
  Velocity auto v1 = avg_speed(220km, 2h);
  Velocity auto v2 = avg_speed(si::length<international::mile>(140), si::time<si::hour>(2));
  Velocity auto v3 = quantity_cast<si::metre_per_second>(v2);
  Velocity auto v4 = quantity_cast<int>(v3);

  std::cout << v1 << '\n';    // 110 km/h
  std::cout << v2 << '\n';    // 70 mi/h
  std::cout << v3 << '\n';    // 31.2928 m/s
  std::cout << v4 << '\n';    // 31 m/s
}
```

Try it on [Compiler Explorer](https://godbolt.org/z/eMhDQ9).


## Summary

`mp-units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
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


## Getting Started

The library framework consists of a few concepts: quantities, units, dimensions and their
exponents. From the user's point of view the most important is a `quantity`.

A quantity is a concrete amount of a unit for a specified dimension with a specific representation:

```cpp
units::quantity<units::si::dim_length, units::si::kilometre, double> d(123);
```

To simplify quantity creation the library provides helper aliases for quantities of different
dimensions. Thanks to then the above example can be rewritten as follows:

```cpp
units::si::length<units::si::kilometre, double> d(123);
```

To simplify creations of compile-time known constants the library provides UDLs for each unit.
Thanks to them the same code can be as simple as:

```cpp
using namespace units::si::literals;
auto d = 123km;    // units::length<units::si::kilometre, std::int64_t>
```

For brevity, the next examples will assume:

```cpp
using namespace units;
```

Let's assume that the user wants to write the following code:

```cpp
int main()
{
  using namespace si::literals;
  auto v1 = avg_speed(220km, 2h);
  auto v2 = avg_speed(140mi, 2h);
  // ...
}
```

`avg_speed` is a simple function calculating an average speed from a distance and duration. It can
be implemented as:

```cpp
constexpr si::velocity<si::metre_per_second, int> avg_speed(si::length<si::metre> d, si::time<si::second> t)
{
  return d / t;
}
```

However, this function will perform unnecessary intermediate conversions (from kilometers to meters,
from hours to seconds, and from meters per second to kilometers per hour). To eliminate all that
overhead we have to write a template function:

```cpp
template<typename U1, typename R1, typename U2, typename R2>
constexpr auto avg_speed(si::length<U1, R1> d, si::time<U2, R2> t)
{
  return d / t;
}
```

This function will work for every SI unit and representation without any unnecessary overhead.
It is also simple enough to ensure that the returned type is actually a velocity. However,
it might not always be the case. For more complicated calculations we would like to ensure
that we are returning a correct type and also inform the user of that fact in the function
template interface. Also we might want to implement a truly generic function that will work
efficiently not only with SI units but also with other systems of units like CGS. The solution
to this are C++20 concepts and generic functions.

```cpp
constexpr Velocity auto avg_speed(Length auto d, Time auto t)
{
  return d / t;
}
```

The units library also tries really hard to print any quantity in the most user friendly
fashion:

```cpp
int main()
{
  using namespace si::literals;
  using namespace international::literals;
  Velocity auto v1 = avg_speed(220km, 2h);
  Velocity auto v2 = avg_speed(140mi, 2h);

  std::cout << v1 << '\n';                                          // 110 km/h
  std::cout << quantity_cast<si::metre_per_second>(speed) << '\n';  // 30.5556 m/s
  std::cout << v2 << '\n';                                          // 70 mi/h
}
```


## Library design

A detailed `mp-units` library design rationale and documentation can be found in
[doc/DESIGN.md](doc/DESIGN.md)


## Repository structure

This repository contains three independent `cmake`-based projects:
1. `./src` - header-only project containing whole `mp-units` library
2. `.` - project used as an entry point for library development (it wraps `./src` project
  together with usage examples and tests)
3. `./test_package` - library installation and Conan package verification

NOTE: Please note that this repository depends on a git submodule in the `./cmake/common`
subdirectory.


## Building, testing, and installation

For a detailed information on project compilation, testing and reuse please refer to
[doc/INSTALL.md](doc/INSTALL.md).

NOTE: This library as of now compiles correctly only with gcc-9.1 and newer.  


## Release notes

- 0.5.0 ???
  - Major refactoring and rewrite of the library
  - Units are now independent from dimensions
  - Dimensions are now depended on units (base or coherent units are provided in a class template)
  - Quantity gets a Dimension template parameter again (as unit does not provide information about
    its dimension anymore)
  - Added official CGS system support
  - Added official data information system support
  - Repository file tree cleanup
  - `ratio` refactored to contain `Exp` template parameter

- 0.4.0 Nov 17, 2019
  - Support for derived dimensions in `exp` added
  - Added `pow()` and `sqrt()` operations on quantities
  - `units` removed from a `std::experimental` namespace
  - Downcasting facility refactored so the user does not have to write the boilerplate code anymore
  - From now on base dimensions should inherit from `base_dimension` class template
  - Added unit symbols definitions to `base_dimension` and derived units
  - Added support for `operator<<` on `quantity`
  - `fmt` support added
  - Derived unit factory helpers refactored
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
