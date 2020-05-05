[![GitHub license](https://img.shields.io/github/license/mpusz/units?color=blue?maxAge=3600)](./LICENSE.md)
[![Travis CI](https://img.shields.io/travis/com/mpusz/units/master.svg?label=Travis%20CI)](https://travis-ci.com/mpusz/units)
[![Conan stable](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg?version=0.4.0%3Astable)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/0.4.0%3Astable/link)
[![Conan testing](https://api.bintray.com/packages/mpusz/conan-mpusz/mp-units%3Ampusz/images/download.svg)](https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz/_latestVersion)
[](https://mp-units.readthedocs.io/en/latest/?badge=latest)

# `mp-units` - A Units Library for C++

**The mp-units library is the subject of this ISO C++ paper: [P1935](https://wg21.link/p1935).
It is explained in this [CppCon 2019 talk](https://youtu.be/0YW6yxkdhlU) (slightly dated now).
We are working towards potentially having it standardized for C++23 and are actively looking
for parties interested in field trialing the library.**


## Documentation

An extensive project documentation including installation instructions and user's
guide can be found on [mp-units GitHub Pages](https://mpusz.github.io/units).


## TL;DR

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
static_assert(2q_m * 3q_m == 6q_m2);
static_assert(10q_km / 5q_km == 2);
static_assert(1000 / 1q_s == 1q_kHz);
static_assert(1q_km / 1q_s == 1000q_m_per_s);
static_assert(2q_km_per_h * 2q_h == 4q_km);
static_assert(2q_km / 2q_km_per_h == 1q_h);
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/_Yx6D7)._

This library requires some C++20 features (concepts, classes as NTTPs, ...). Thanks to
them the user gets a powerful but still easy to use interface and all unit conversions
and dimensional analysis can be performed without sacrificing on accuracy. Please see
the below example for a quick preview of basic library features:

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

_Try it on the [Compiler Explorer](https://godbolt.org/z/_Yx6D7)._
