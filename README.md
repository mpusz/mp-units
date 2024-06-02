<img align="right" height=135px src="docs/assets/images/mp-units-color.svg">

[![GitHub license](https://img.shields.io/github/license/mpusz/mp-units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![GitHub license](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-conan.yml)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-test-package-cmake.yml)
[![clang-tidy CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-clang-tidy.yml?branch=master&label=clang-tidy%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-clang-tidy.yml)
[![Freestanding CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-freestanding.yml?branch=master&label=Freestanding%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-freestanding.yml)
[![Formatting CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-formatting.yml?branch=master&label=Formatting%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-formatting.yml)
[![GitHub Workflow Documentation](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/documentation.yml?branch=master&label=Documentation)](https://github.com/mpusz/mp-units/actions?query=workflow%3ADocumentation+branch%3Amaster)

[![Conan stable](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-2.2.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/2.2.0)


# `mp-units` - The quantities and units library for C++

**The mp-units library might be the subject of ISO standardization for C++29. More on this can
be found in the following ISO C++ proposals:**

- [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935),
- [P2980: A motivation, scope, and plan for a quantities and units library](https://wg21.link/p2980),
- [P3045: Quantities and units library](https://wg21.link/p3045).

**We are actively looking for parties interested in field-trialing the library.**

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/mpusz/mp-units)


## Video Introduction

A brief introduction to the library's interfaces and the rationale for changes in the version 2.0
of **mp-units** were provided in detail by [Mateusz Pusz](https://github.com/mpusz) in the
["The Power of C++ Templates With mp-units: Lessons Learned & a New Library Design" talk at the C++ on Sea 2023 conference](https://www.youtube.com/watch?v=eUdz0WvOMm0).


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
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// simple numeric operations
static_assert(10 * km / 2 == 5 * km);

// conversions to common units
static_assert(1 * h == 3600 * s);
static_assert(1 * km + 1 * m == 1001 * m);

// derived quantities
static_assert(1 * km / (1 * s) == 1000 * m / s);
static_assert(2 * km / h * (2 * h) == 4 * km);
static_assert(2 * km / (2 * km / h) == 1 * h);

static_assert(2 * m * (3 * m) == 6 * m2);

static_assert(10 * km / (5 * km) == 2 * one);

static_assert(1000 / (1 * s) == 1 * kHz);
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/8acPeq743)._

This library heavily uses C++20 features (concepts, classes as NTTPs, ...). Thanks to
them the user gets a powerful but still easy to use interfaces and all unit conversions
and dimensional analysis can be performed without sacrificing on runtime performance or
accuracy. Please see the below example for a quick preview of basic library features:

```cpp
#include <mp-units/format.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/international.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <format>
#include <iomanip>
#include <iostream>
#include <print>

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

  constexpr quantity v1 = 110 * km / h;
  constexpr quantity v2 = 70 * mph;
  constexpr quantity v3 = avg_speed(220. * isq::distance[km], 2 * h);
  constexpr quantity v4 = avg_speed(isq::distance(140. * mi), 2 * h);
  constexpr quantity v5 = v3.in(m / s);
  constexpr quantity v6 = value_cast<m / s>(v4);
  constexpr quantity v7 = value_cast<int>(v6);

  std::cout << v1 << '\n';                                        // 110 km/h
  std::cout << std::setw(10) << std::setfill('*') << v2 << '\n';  // ***70 mi/h
  std::cout << std::format("{:*^10}\n", v3);                      // *110 km/h*
  std::println("{:%N in %U of %D}", v4);                          // 70 in mi/h of LT⁻¹
  std::println("{::N[.2f]}", v5);                                 // 30.56 m/s
  std::println("{::N[.2f]U[dn]}", v6);                            // 31.29 m⋅s⁻¹
  std::println("{:%N}", v7);                                      // 31
}
```

_Try it on the [Compiler Explorer](https://godbolt.org/z/nhqhT8Mzb)._
