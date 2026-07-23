<!-- markdownlint-disable MD041 -->
<!-- markdownlint-disable-next-line MD033 -->
<img align="right" height=135px src="docs/assets/images/mp-units-color.svg" alt="logo">

[![License](https://img.shields.io/github/license/mpusz/mp-units)](https://github.com/mpusz/mp-units/blob/master/LICENSE.md)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20%2F23-blue)](https://en.cppreference.com/w/cpp/compiler_support#cpp20)

[![Conan CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-conan.yml?branch=master&label=Conan%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-conan.yml)
[![CMake CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-test-package-cmake.yml?branch=master&label=CMake%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-test-package-cmake.yml)
[![clang-tidy CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-clang-tidy.yml?branch=master&label=clang-tidy%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-clang-tidy.yml)
[![Freestanding CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-freestanding.yml?branch=master&label=Freestanding%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-freestanding.yml)
[![Formatting CI](https://img.shields.io/github/actions/workflow/status/mpusz/mp-units/ci-formatting.yml?branch=master&label=Formatting%20CI)](https://github.com/mpusz/mp-units/actions/workflows/ci-formatting.yml)

[![Conan Center](https://img.shields.io/conan/v/mp-units?label=ConanCenter&color=blue)](https://conan.io/center/mp-units)
[![Conan testing](https://img.shields.io/badge/mpusz.jfrog.io-2.5.0%3Atesting-blue)](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units/2.5.0)


# `mp-units` – The Domain-Correct Quantities and Units Library for C++


## 🎯 Overview

**`mp-units`** is a Modern C++ (C++20 and later) library providing the full spectrum
of compile-time safety for domain-specific quantities and units, from dimensional
analysis to quantity kind safety, built on the ISO 80000 International System of
Quantities (ISQ). It is a candidate for C++29 standardization
([P3045](https://wg21.link/p3045)), your chance to shape the future of C++.

```cpp
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Compile-time dimensional analysis — zero runtime overhead
static_assert(1 * km / (1 * s) == 1000 * m / s);

// Function signatures encode domain/physics, not just dimensions
void calculate_trajectory(quantity<isq::kinetic_energy[J]> e);

int main()
{
  quantity<isq::potential_energy[J]> Ep = 42 * J;
  quantity<isq::kinetic_energy[J]>   Ek = 123 * J;
  calculate_trajectory(Ek);         // ✅ correct
  // calculate_trajectory(Ep);      // ❌ potential energy ≠ kinetic energy (both in J)

  // quantity<Gy> q = 42 * Sv;      // ❌ absorbed dose ≠ dose equivalent (both J/kg)
}
```

[![Try it live on Compiler Explorer](https://img.shields.io/badge/Try_live_on-Compiler_Explorer-black?style=for-the-badge&logo=compilerexplorer&labelColor=black&color=67C52A)](https://godbolt.org/z/YWErT7ofs)

### What Sets mp-units Apart?

Beyond standard dimensional analysis and automatic unit conversions, **mp-units** provides
additional safety levels:

- **Quantity Kind Safety** — **mp-units** pioneered this level: it distinguishes
  quantities that share the same dimension but represent fundamentally different physical
  concepts: _frequency_ (Hz) ≠ _radioactive activity_ (Bq), _absorbed dose_ (Gy) ≠
  _dose equivalent_ (Sv), _plane angle_ (rad) ≠ _solid angle_ (sr). Dimensional analysis
  alone cannot catch these errors, **mp-units** prevents them at compile time.

- **ISO 80000 (ISQ) Support** — Built on the International System of Quantities, functions
  can require _specific_ quantities: `isq::height` (not just any `isq::length`),
  `isq::kinetic_energy` (not just any `isq::energy`). The physics of your domain becomes
  part of the type system.

- **Strongly-Typed Numerics for Any Domain** — The quantity framework extends beyond
  physics: define semantically distinct types for item counts, financial values, identifiers,
  or any numeric abstraction that should never be silently mixed at compile time.


## ✅ Key Features

**Safety**
- Quantity kind safety: same dimension, different meaning → compile-time error
- Affine space strong types (`quantity` and `quantity_point`)
- Value-preserving conversions

**Performance**
- All dimensional analysis at compile time, zero runtime overhead
- Performance on par with (sometimes even better than) fundamental types

**User Experience**
- Optimized for readable, actionable compilation errors
- Expressive, composable unit expressions

**Feature Rich**
- Systems of Quantities *and* Units; scalar, vector, and tensor quantities
- Affine space, natural units, strong angular system
- Highly adjustable text output formatting

**Easy to Extend**
- Custom dimensions, quantities, and units in a single line of code

**Low Adoption Cost**
- No external dependencies · macro-free API · C++20 modules-ready · freestanding-capable

→ [Full feature overview](https://mpusz.github.io/mp-units/latest/getting_started/about/)


## 📚 Documentation

Extensive project documentation covers everything from getting started to advanced usage:

<!-- markdownlint-disable MD013 -->
- [**Getting Started**](https://mpusz.github.io/mp-units/latest/getting_started/) – Get up and running quickly
- [**Tutorials**](https://mpusz.github.io/mp-units/latest/tutorials/) – Step-by-step learning resources
- [**User's Guide**](https://mpusz.github.io/mp-units/latest/users_guide/) – Comprehensive usage documentation
- [**Workshops**](https://mpusz.github.io/mp-units/latest/workshops/) – Hands-on practice exercises
- [**How-to Guides**](https://mpusz.github.io/mp-units/latest/how_to_guides/) – Task-oriented recipes
- [**Examples**](https://mpusz.github.io/mp-units/latest/examples/) – Real-world usage demonstrations
- [**Reference**](https://mpusz.github.io/mp-units/latest/reference/) – Complete technical documentation
- [**Blog**](https://mpusz.github.io/mp-units/latest/blog/)
    - [Metrology](https://mpusz.github.io/mp-units/latest/blog/category/metrology/) – Design rationale and domain theory
    - [Releases](https://mpusz.github.io/mp-units/latest/blog/category/releases/) – Narrative release articles with examples and migration guidance
    - [WG21 Updates](https://mpusz.github.io/mp-units/latest/blog/category/wg21-updates/) – Standardization progress
<!-- markdownlint-enable MD013 -->

→ [Explore the full documentation](https://mpusz.github.io/mp-units)


## 🔍 Try It Out

For **advanced development** or **contributions**, we provide a preconfigured cloud
development environment with [GitHub Codespaces](https://docs.github.com/en/codespaces):

[![Open in GitHub Codespaces](https://img.shields.io/badge/Open_in-GitHub_Codespaces-blue?style=for-the-badge&logo=github&labelColor=black&color=2088FF)](https://codespaces.new/mpusz/mp-units)

**Alternatives:**
1. Navigate to the repository → **"Code"** → **"Codespaces"** → **"Create codespace on master"**
2. Use the preconfigured devcontainer and Docker image manually in your IDE

For detailed environment documentation, see [`.devcontainer/README.md`](.devcontainer/README.md).


## 🚀 Help Shape the Future of C++

**`mp-units` is a candidate for ISO standardization for C++29.** The technical case is
documented in:

- [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935)
- [P2980: A motivation, scope, and plan for a quantities and units library](https://wg21.link/p2980)
- [P3045: Quantities and units library](https://wg21.link/p3045)
- [P4185: Completing the Mathematical Model for C++ Quantities and Units](https://wg21.link/p4185)

> **We are actively seeking organizations and individuals interested in**
> **field‑trialing the library!**

**Your experience matters.** Real-world testimonials demonstrate value to the ISO C++ Committee
and help potential adopters decide. Whether you are using **mp-units** in **production**,
**research**, or **education**:

- **Organizations**: Share your production deployments and success stories
- **Academics**: Report research applications and teaching experiences
- **Developers**: Tell us about your innovative use cases and benefits

[![Share Experience](https://img.shields.io/badge/Share_Your-Usage_Experience-blue?style=for-the-badge&labelColor=black&label=🌟%20Share%20Your)](https://github.com/mpusz/mp-units/issues/new?template=usage_experience.yml)


## 🤝 Contributors

**mp-units** is made possible by our community of contributors!

[![Contributors](https://img.shields.io/github/contributors/mpusz/mp-units?style=for-the-badge&logo=github&labelColor=black&color=blue)](https://github.com/mpusz/mp-units/graphs/contributors)
[![Commits](https://img.shields.io/github/commit-activity/m/mpusz/mp-units?style=for-the-badge&logo=git&labelColor=black&color=green)](https://github.com/mpusz/mp-units/pulse)
[![Stars](https://img.shields.io/github/stars/mpusz/mp-units?style=for-the-badge&label=⭐%20Stars&labelColor=black&color=gold)](https://github.com/mpusz/mp-units/stargazers)

### Core Team

- **[Mateusz Pusz](https://github.com/mpusz)** – Project founder and lead; C++ trainer
  at [Train IT](https://train-it.eu)
- **[Johel Ernesto Guerrero Peña](https://github.com/JohelEGP)** – Core maintainer
- **[Chip Hogg](https://github.com/chiphogg)** – Core maintainer

### All Contributors

We appreciate **every contribution**, from code to documentation to community support.

See the [**Contributors Page**](CONTRIBUTORS.md) for the complete list and recognition
details.

> **Ready to contribute?** Check out our
> [**Contributing Guide**](https://mpusz.github.io/mp-units/latest/getting_started/contributing/)
> to get started! 🚀


## 💝 Support the Project

**mp-units** is developed as open source, with the goal of C++29 standardization. Your
support helps sustain development and the standardization work.

**Ways to support:**
- ⭐ **Star the repository** – Show your appreciation and help others discover **mp-units**
- 💰 **Become a sponsor** – Financial support enables continued development

  [![Sponsor](https://img.shields.io/badge/Sponsor-GitHub_Sponsors-pink?style=for-the-badge&logo=githubsponsors&labelColor=black&color=EA4AAA)](https://github.com/sponsors/mpusz)

- 📢 **Share your success story** – Help demonstrate real-world value for standardization
  and other potential users
- 🤝 **Contribute** – Code, documentation, feedback, and community support
