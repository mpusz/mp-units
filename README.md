[![GitHub license](https://img.shields.io/github/license/mpusz/mp-units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![GitHub license](https://img.shields.io/badge/C%2B%2B-20-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/mp-units/actions?query=workflow%3A%22Conan%20CI%22+branch%3Amaster)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/mp-units/actions?query=workflow%3A%22CMake+Test+Package+CI%22+branch%3Amaster)
[![Check CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-check.yml?branch=master&label=Check%20CI)](https://github.com/mpusz/mp-units/actions?query=workflow%3A%22Check%20CI%22+branch%3Amaster)
[![GitHub Workflow Documentation](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/documentation.yml?branch=master&label=Documentation)](https://github.com/mpusz/mp-units/actions?query=workflow%3ADocumentation+branch%3Amaster)

[![Conan stable](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-2.0.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/2.0.0)


# `mp-units` - A Physical Quantities and Units library for C++

**The mp-units library might be the subject of ISO standardization for C++29.
More on this can be found in ISO C++ paper [P1935](https://wg21.link/p1935).
We are actively looking for parties interested in field trialing the library.**

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/mpusz/mp-units)


## Video Introduction

A brief introduction to the library's interfaces and the rationale for changes in the version 2.0
of **mp-units** were provided in detail by [Mateusz Pusz](https://github.com/mpusz) in the
["mp-units: Lessons Learned and a New C++ Library Design" talk at the ACCU 2023 conference](https://www.youtube.com/watch?v=l0rXdJfXLZc).


## Documentation

An extensive project documentation can be found on [mp-units GitHub Pages](https://mpusz.github.io/mp-units).
It includes installation instructions and a detailed user's guide.


## Terms and Definitions

This project uses the official metrology vocabulary defined by the ISO and BIPM.
Please familiarize yourself with those terms to better understand the documentation
and improve domain-related communication and discussions. You can find essential
project-related definitions in [our documentation's "Glossary" chapter](https://mpusz.github.io/mp-units/latest/appendix/glossary).
Even more terms are provided in the official vocabulary of the [ISO](https://www.iso.org/obp/ui#iso:std:iso-iec:guide:99:ed-1:v2:en)
and [BIPM](https://jcgm.bipm.org/vim/en).


## TL;DR

`mp-units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation.

Here is a small example of possible operations:

```cpp
#include <mp-units/systems/si/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// simple numeric operations
static_assert(10 * km / 2 == 5 * km);

// unit conversions
static_assert(1 * h == 3600 * s);
static_assert(1 * km + 1 * m == 1001 * m);

// derived quantities
inline constexpr auto kmph = km / h;
static_assert(1 * km / (1 * s) == 1000 * (m / s));
static_assert(2 * kmph * (2 * h) == 4 * km);
static_assert(2 * km / (2 * kmph) == 1 * h);

static_assert(2 * m * (3 * m) == 6 * m2);

static_assert(10 * km / (5 * km) == 2 * one);

static_assert(1000 / (1 * s) == 1 * kHz);
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/81Ev7qhTd)._

This library heavily uses C++20 features (concepts, classes as NTTPs, ...). Thanks to
them the user gets a powerful but still easy to use interfaces and all unit conversions
and dimensional analysis can be performed without sacrificing on runtime performance or
accuracy. Please see the below example for a quick preview of basic library features:

```cpp
#include <mp-units/format.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/isq.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

using namespace mp_units;

constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto d,
                                                QuantityOf<isq::time> auto t)
{
  return d / t;
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::international::unit_symbols;

  constexpr quantity v1 = 110 * (km / h);
  constexpr quantity v2 = 70 * mph;
  constexpr quantity v3 = avg_speed(220. * isq::distance[km], 2 * h);
  constexpr quantity v4 = avg_speed(isq::distance(140. * mi), 2 * h);
  constexpr quantity v5 = v3.in(m / s);
  constexpr quantity v6 = value_cast<m / s>(v4);
  constexpr quantity v7 = value_cast<int>(v6);

  std::cout << v1 << '\n';                                  // 110 km/h
  std::cout << v2 << '\n';                                  // 70 mi/h
  std::cout << std::format("{}", v3) << '\n';               // 110 km/h
  std::cout << std::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
  std::cout << std::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
  std::cout << std::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
  std::cout << std::format("{:%Q}", v7) << '\n';            // 31
}
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/Tsesa1Pvq)._
