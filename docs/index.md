---
hide:
  - navigation
  - toc
---

# Welcome to **mp-units**!

**mp-units** is a compile-time enabled feature-rich Modern C++ modular/header-only library that
provides compile-time dimensional analysis and unit/quantity manipulation. Its key strengths
include safety, performance, and developer experience.

The library source code is hosted on [GitHub](https://github.com/mpusz/mp-units) with a permissive
[MIT license](https://github.com/mpusz/units/blob/master/LICENSE.md).

!!! important "Important: Help needed!"

    The **mp-units** library might be the subject of ISO standardization for C++29. More on this can
    be found in the following ISO C++ proposals:

    - [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935),
    - [P2980: A motivation, scope, and plan for a physical quantities and units library](https://wg21.link/p2980),
    - [P2981: Improving our safety with a physical quantities and units library](https://wg21.link/p2981),
    - [P2982: `std::quantity` as a numeric type](https://wg21.link/p2982).

    We are actively looking for parties interested in field-trialing the library.

??? info "Supported compilers"

    This library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses C++20 features and the
    [explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
    from C++23.

    Even though the library benefits from C++23 (if available), C++20 is enough to compile and
    use all of the library's functionality. C++23 features are hidden behind
    a [preprocessor macro](users_guide/framework_basics/systems_of_quantities.md#defining-quantities)
    providing a backward-compatible way to use it.

    As of today, the library compiles fine on the following compilers (or newer):

    - gcc-11
        - please note that we observed some ICEs on gcc-11
        - no problems with gcc-12.2+
    - clang-16
    - apple-clang-15
