[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](./LICENSE.md)
[![Travis CI](https://img.shields.io/travis/com/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.com/mpusz/units)
[![Conan stable](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg?version=0.4.0%3Astable)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/0.4.0%3Astable/link)
[![Conan testing](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/_latestVersion)


# `mp-units` - A Units Library for C++

## TL;DR

This library is the subject of this ISO C++ proposal: [P1935](https://wg21.link/p1935). It is
explained in this [CppCon 2019 talk](https://youtu.be/0YW6yxkdhlU) (slightly dated see below).
We are working towards potentially having it standardized for C++23 and are actively looking
for parties interested in field trialing the library.

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
  Velocity auto v1 = avg_speed(220q_km, 2q_h);
  Velocity auto v2 = avg_speed(si::length<international::mile>(140), si::time<si::hour>(2));
  Velocity auto v3 = quantity_cast<si::metre_per_second>(v2);
  Velocity auto v4 = quantity_cast<int>(v3);

  std::cout << v1 << '\n';    // 110 km/h
  std::cout << v2 << '\n';    // 70 mi/h
  std::cout << v3 << '\n';    // 31.2928 m/s
  std::cout << v4 << '\n';    // 31 m/s
}
```

Try it on [Compiler Explorer](https://godbolt.org/z/_Yx6D7).


## Summary

`mp-units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions.

Here is a small example of possible operations:

```cpp
// simple numeric operations
static_assert(10q_km / 2 == 5q_km);

// unit conversions
static_assert(1q_h == 3600q_s);
static_assert(1q_km + 1q_m == 1001q_m);

// dimension conversions
static_assert(1q_km / 1q_s == 1000q_mps);
static_assert(2q_kmph * 2q_h == 4q_km);
static_assert(2q_km / 2q_kmph == 1q_h);

static_assert(1000 / 1q_s == 1q_kHz);

static_assert(10q_km / 5q_km == 2);
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
auto d = 123q_km;    // units::length<units::si::kilometre, std::int64_t>
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
  auto v1 = avg_speed(220q_km, 2q_h);
  auto v2 = avg_speed(140q_mi, 2q_h);
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
  Velocity auto v1 = avg_speed(220q_km, 2q_h);
  Velocity auto v2 = avg_speed(140q_mi, 2q_h);

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
