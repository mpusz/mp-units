---
hide:
  - navigation
  - toc
---

# Welcome to **mp-units**!

**mp-units** is a compile-time enabled feature-rich Modern C++ modular/header-only library that
provides compile-time dimensional analysis and unit/quantity manipulation. Its key strengths
include safety, performance, and developer experience.

It is the first library on the market that, besides being **unit-safe** and **dimension-safe**,
is also **quantity-safe**.

The library source code is hosted on [GitHub](https://github.com/mpusz/mp-units) with a permissive
[MIT license](https://github.com/mpusz/units/blob/master/LICENSE.md).

??? info "Supported compilers"

    This library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses the latest C++ language features.

    Even though the library benefits from the latest C++ versions (if available), C++20 is enough
    to compile and use all of the library's functionality.

    Please refer to [C++ compiler support chapter](getting_started/cpp_compiler_support.md)
    for more details.

=== "C++ modules"

    ```cpp
    #include <iostream>
    #include <print>
    import mp_units;

    using namespace mp_units;

    inline constexpr struct smoot final : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

    int main()
    {
      constexpr quantity dist = 364.4 * smoot;
      std::println("Harvard Bridge length = {::N[.1f]} ({::N[.1f]}, {::N[.2f]}) ± 1 εar",
                   dist, dist.in(usc::foot), dist.in(si::metre));
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/usc.h>
    #include <print>

    using namespace mp_units;

    inline constexpr struct smoot final : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

    int main()
    {
      constexpr quantity dist = 364.4 * smoot;
      std::println("Harvard Bridge length = {::N[.1f]} ({::N[.1f]}, {::N[.2f]}) ± 1 εar",
                   dist, dist.in(usc::foot), dist.in(si::metre));
    }
    ```

Output:

```txt
Harvard Bridge length = 364.4 smoot (2034.6 ft, 620.14 m) ± 1 εar
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/xexMjMsxn)"

??? question "What is `smoot`?"

    The smoot (/ˈsmuːt/) is a nonstandard unit of length created as part of an MIT fraternity prank.
    It is named after Oliver R. Smoot, a fraternity pledge to Lambda Chi Alpha, who, in October 1958,
    lay on the Harvard Bridge (between Boston and Cambridge, Massachusetts) and was used by his
    fraternity brothers to measure the length of the bridge.

    One smoot equals Oliver Smoot's height at the time of the prank (five feet and seven inches).
    The bridge's length was measured to be 364.4 smoots plus or minus one ear, with the "plus or minus"
    intended to express the measurement uncertainty.

    Oliver Smoot graduated from MIT with the class of 1962, became a lawyer, and later became
    chairman of the American National Standards Institute (ANSI) and president of the International
    Organization for Standardization (ISO).

    More on the smoot unit of length can be found at <https://en.wikipedia.org/wiki/Smoot>.


!!! important "Important: Help needed!"

    The **mp-units** library might be the subject of ISO standardization for C++29. More on this can
    be found in the following ISO C++ proposals:

    - [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935),
    - [P2980: A motivation, scope, and plan for a physical quantities and units library](https://wg21.link/p2980),
    - [P3045: Quantities and units library](https://wg21.link/p3045).

    We are actively looking for parties interested in field-trialing the library.
