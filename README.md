<!-- markdownlint-disable MD041 -->
<!-- markdownlint-disable-next-line MD033 -->
<img align="right" height=135px src="docs/assets/images/mp-units-color.svg" alt="logo">

[![License](https://img.shields.io/github/license/mpusz/mp-units?cacheSeconds=3600&color=informational&label=License)](./LICENSE.md)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-conan.yml)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-test-package-cmake.yml)
[![clang-tidy CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-clang-tidy.yml?branch=master&label=clang-tidy%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-clang-tidy.yml)
[![Freestanding CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-freestanding.yml?branch=master&label=Freestanding%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-freestanding.yml)
[![Formatting CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-formatting.yml?branch=master&label=Formatting%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-formatting.yml)
[![Documentation](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/documentation.yml?branch=master&label=Documentation)](https://github.com/mpusz/mp-units/actions?query=workflow%3ADocumentation+branch%3Amaster)

[![Conan Center](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-2.5.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/2.5.0)


# `mp-units` – The Quantities and Units Library for C++

> 🚀 **ISO C++29 Standardization Candidate** – The future of dimensional analysis in C++!

**The `mp-units` library is a candidate for ISO standardization for C++29.**
More context can be found in the following ISO C++ proposals:

- 📄 [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935)
- 🎯 [P2980: A motivation, scope, and plan for a quantities and units library](https://wg21.link/p2980)
- 📐 [P3045: Quantities and units library](https://wg21.link/p3045)

> 🤝 **We are actively seeking organizations and individuals interested in**
> **field‑trialing the library!**

## 📚 Documentation

Extensive project documentation is available on the **[project site](https://mpusz.github.io/mp-units)**.
It includes:

- 🚀 **Installation instructions** – Get up and running quickly
- 📋 **Detailed user's guide** – Comprehensive usage documentation  
- 🧠 **Design rationale** – Understanding the architectural decisions
- 🔍 **API reference** – Complete technical documentation
- 🎓 **Tutorials** – Step-by-step learning resources
- 💡 **Examples** – Real-world usage demonstrations


## ⚡ TL;DR

**`mp-units`** is a Modern C++ (C++20 and later) library providing compile‑time
dimensional analysis and safe manipulation of units, quantities, and quantity points.

### Key Features

- 🛡️ **Type Safety** – Strongly typed quantities, units, dimensions, and quantity points
- ⚡ **Zero Runtime Cost** – Compile‑time dimensional analysis with no runtime overhead
- 🎨 **Unified Design** – Comprehensive model for units, dimensions, quantities, and point origins
- ✏️ **Rich Text Formatting** – Text formatting support with extensive options &
  character sets
- 📦 **Flexible Usage** – C++ modules support (when available) and header‑only usage
- ⚙️ **Configurable** – Contracts, freestanding mode, and natural units (experimental)
- 🔗 **Interoperable** – Seamless pathways for legacy and external libraries

### Quick Example

Here's a taste of what **mp-units** can do:

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

> 🚀 [**Try it live on Compiler Explorer!**](https://godbolt.org/z/fT1r4sohs)

### Modern C++ Design

The library makes extensive use of **C++20 features** (concepts, class types as NTTPs, etc.).
This enables powerful yet easy‑to‑use interfaces while performing all conversions and dimensional
analysis at compile time—without sacrificing runtime performance or accuracy.

### More Advanced Example

Here's a broader preview showcasing **mp-units** capabilities:

```cpp
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

> 🚀 [**Try it live on Compiler Explorer!**](https://godbolt.org/z/fxcjs19ah)

## 🔍 Try It Out

### Compiler Explorer

**mp-units** is available on Compiler Explorer for instant experimentation!
Perfect for fast, zero‑setup trials and prototyping your ideas.


### GitHub Codespaces

For **advanced development** or **contributions**, we provide a fully configured cloud
development environment with [GitHub Codespaces](https://docs.github.com/en/codespaces):

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/mpusz/mp-units)

**Alternatives:**
1. Navigate to the repository → **"Code"** → **"Codespaces"** → **"Create codespace on master"**
2. Use the pre‑configured devcontainer and Docker image manually in your IDE

For detailed environment documentation, see [`.devcontainer/README.md`](.devcontainer/README.md).

### Install as a Dependency

🥇 **Recommended:** We recommend using **Conan** to integrate **mp-units** with your project.

📋 **Multiple options available!** Please refer to our comprehensive
[**Installation and Usage Guide**](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage)
for all supported integration methods.
