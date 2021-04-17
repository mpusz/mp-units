[![GitHub license](https://img.shields.io/github/license/mpusz/units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![Conan CI](https://img.shields.io/github/workflow/status/mpusz/units/Conan%20CI/master?label=Conan%20CI)](https://github.com/mpusz/units/actions?query=workflow%3A%22Conan%20CI%22+branch%3Amaster)
[![CMake CI](https://img.shields.io/github/workflow/status/mpusz/units/CMake%20Test%20Package%20CI/master?label=CMake%20CI)](https://github.com/mpusz/units/actions?query=workflow%3A%22CMake+Test+Package+CI%22+branch%3Amaster)
[![GitHub Workflow Documentation](https://img.shields.io/github/workflow/status/mpusz/units/Documentation/master?label=Documentation%20CI)](https://github.com/mpusz/units/actions?query=workflow%3ADocumentation+branch%3Amaster)
[![Conan stable](https://img.shields.io/badge/ConanCenter-0.6.0%3Astable-blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-0.7.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/0.7.0)

# `mp-units` - A Units Library for C++

**The mp-units library is the subject of ISO standardization for C++23/26. More on this can
be found in ISO C++ paper [P1935](https://wg21.link/p1935) and
[CppCon 2020 talk](https://www.youtube.com/watch?v=7dExYGSOJzo). We are actively looking for
parties interested in field trialing the library.**


## Documentation

An extensive project documentation including installation instructions and user's
guide can be found on [mp-units GitHub Pages](https://mpusz.github.io/units).


## TL;DR

`mp-units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions.

Here is a small example of possible operations:

```cpp
#define UNITS_REFERENCES

#include <units/isq/si/area.h>
#include <units/isq/si/frequency.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
#include <units/isq/si/time.h>

using namespace units::isq::si::references;

// simple numeric operations
static_assert(10 * km / 2 == 5 * km);

// unit conversions
static_assert(1 * h == 3600 * s);
static_assert(1 * km + 1 * m == 1001 * m);

// dimension conversions
inline constexpr auto kmph = km / h;
static_assert(1 * km / (1 * s) == 1000 * (m / s));
static_assert(2 * kmph * (2 * h) == 4 * km);
static_assert(2 * km / (2 * kmph) == 1 * h);

static_assert(2 * m * (3 * m) == 6 * m2);

static_assert(10 * km / (5 * km) == 2);

static_assert(1000 / (1 * s) == 1 * kHz);
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/53bTahKd8)._

This library requires some C++20 features (concepts, classes as NTTPs, ...). Thanks to
them the user gets a powerful but still easy to use interface and all unit conversions
and dimensional analysis can be performed without sacrificing on accuracy. Please see
the below example for a quick preview of basic library features:

```cpp
#define UNITS_ALIASES
#define UNITS_LITERALS
#define UNITS_REFERENCES

#include <units/format.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/international/speed.h> // IWYU pragma: keep
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h> // IWYU pragma: keep
#include <units/isq/si/time.h>
#include <units/quantity_io.h>
#include <iostream>

using namespace units::isq;

constexpr Speed auto avg_speed(Length auto d, Time auto t)
{
  return d / t;
}

int main()
{
  using namespace units::isq::si::literals;
  using namespace units::isq::si::references;
  using namespace units::aliases::isq::si::international;

  constexpr Speed auto v1 = 110 * (km / h);
  constexpr Speed auto v2 = mi_per_h(70.);
  constexpr Speed auto v3 = avg_speed(220_q_km, 2_q_h);
  constexpr Speed auto v4 = avg_speed(si::length<si::international::mile>(140), si::time<si::hour>(2));
#if UNITS_DOWNCAST_MODE == 0
  constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
  constexpr Speed auto v6 = quantity_cast<si::dim_speed, si::metre_per_second>(v4);
#else
  constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
  constexpr Speed auto v6 = quantity_cast<si::metre_per_second>(v4);
#endif
  constexpr Speed auto v7 = quantity_cast<int>(v6);

  std::cout << v1 << '\n';                                  // 110 km/h
  std::cout << v2 << '\n';                                  // 70 mi/h
  std::cout << fmt::format("{}", v3) << '\n';               // 110 km/h
  std::cout << fmt::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
  std::cout << fmt::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
  std::cout << fmt::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
  std::cout << fmt::format("{:%Q}", v7) << '\n';            // 31
}
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/WP39WvE6M)._
