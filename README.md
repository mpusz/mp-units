[![GitHub license](https://img.shields.io/github/license/mpusz/units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![GitHub license](https://img.shields.io/badge/C%2B%2B-20-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/units/actions?query=workflow%3A%22Conan%20CI%22+branch%3Amaster)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/units/actions?query=workflow%3A%22CMake+Test+Package+CI%22+branch%3Amaster)
[![Check CI](https://img.shields.io/github/actions/workflow/status/mpusz/units/ci-check.yml?branch=master&label=Check%20CI)](https://github.com/mpusz/units/actions?query=workflow%3A%22Check%20CI%22+branch%3Amaster)
[![GitHub Workflow Documentation](https://img.shields.io/github/actions/workflow/status/mpusz/units/documentation.yml?branch=master&label=Documentation)](https://github.com/mpusz/units/actions?query=workflow%3ADocumentation+branch%3Amaster)

[![Conan stable](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-0.8.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/0.8.0)


# `mp-units` - A Units Library for C++

**The mp-units library is the subject of ISO standardization for C++23/26. More on this can
be found in ISO C++ paper [P1935](https://wg21.link/p1935) and
[NDC TechTown 2021 talk](https://www.youtube.com/watch?v=nudq58d0TFc). We are actively looking for
parties interested in field trialing the library.**

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/mpusz/units)


## Documentation

An extensive project documentation including installation instructions and user's
guide can be found on [mp-units GitHub Pages](https://mpusz.github.io/units).


## Terms and Definitions

This project uses the official metrology vocabulary defined by the ISO and BIPM.
Please familiarize yourself with those terms to better understand the documentation
and improve domain-related communication and discussions. You can find essential
project-related definitions in [our documentation's "Glossary" chapter](https://mpusz.github.io/units/glossary.html).
Even more terms are provided in the official vocabulary of the [ISO](https://www.iso.org/obp/ui#iso:std:iso-iec:guide:99:ed-1:v2:en)
and [BIPM](https://jcgm.bipm.org/vim/en).

## TL;DR

`mp-units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions.

Here is a small example of possible operations:

```cpp
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

_Try it on the [Compiler Explorer](https://godbolt.org/z/qbbbnfK3s)._

This library requires some C++20 features (concepts, classes as NTTPs, ...). Thanks to
them the user gets a powerful but still easy to use interface and all unit conversions
and dimensional analysis can be performed without sacrificing on accuracy. Please see
the below example for a quick preview of basic library features:

```cpp
#include <units/format.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/international/speed.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>
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
  constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
  constexpr Speed auto v6 = quantity_cast<si::metre_per_second>(v4);
  constexpr Speed auto v7 = quantity_cast<int>(v6);

  std::cout << v1 << '\n';                                  // 110 km/h
  std::cout << v2 << '\n';                                  // 70 mi/h
  std::cout << std::format("{}", v3) << '\n';               // 110 km/h
  std::cout << std::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
  std::cout << std::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
  std::cout << std::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
  std::cout << std::format("{:%Q}", v7) << '\n';            // 31
}
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/b4a3Ya6dY)._
