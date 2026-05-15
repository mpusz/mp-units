---
hide:
  - navigation
  - toc
---

<!-- markdownlint-disable-next-line MD026 -->
# Welcome to **mp-units**!

**mp-units** is the only Modern C++ (C++20) library providing the full spectrum of
compile‑time safety for physical quantities and units — from dimensional analysis and
unit conversions to **quantity kind safety** — built on the ISO 80000 International
System of Quantities (ISQ).

The library source code is hosted on [GitHub](https://github.com/mpusz/mp-units) with a
permissive [MIT license](https://github.com/mpusz/mp-units/blob/master/LICENSE.md).

=== "C++ modules"

    ```cpp
    #include <iostream>
    #include <print>
    import mp_units;

    using namespace mp_units;

    inline constexpr struct smoot : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

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

    inline constexpr struct smoot : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

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

[Try it live on Compiler Explorer](https://godbolt.org/z/xexMjMsxn){ .md-button }

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

??? info "Supported compilers"

    This library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses the latest C++ language features.

    Even though the library benefits from the latest C++ versions (if available), C++20 is enough
    to compile and use all of the library's functionality.

    Please refer to [C++ compiler support chapter](getting_started/cpp_compiler_support.md)
    for more details.

!!! important "Help shape the future of C++!"

    **mp-units** is a candidate for ISO standardization in C++29. To support this effort,
    we need your help in demonstrating its real-world value to the ISO C++ Committee.

    **We are seeking:**

    - **Field trial partners** – Organizations interested in evaluating the library in production environments
    - **Success stories** – Real-world testimonials from production deployments, research projects, or educational use
    - **Feedback** – Practical insights from developers, academics, and organizations using the library

    Your experience—whether from production systems, research applications, or teaching—provides
    invaluable evidence for standardization and helps other potential users evaluate the library.

    [![Share Your Experience](https://img.shields.io/badge/Share_Your-Usage_Experience-blue?style=for-the-badge&labelColor=black&label=🌟%20Share%20Your)](https://github.com/mpusz/mp-units/issues/new?template=usage_experience.yml)

    For more details, see the ISO C++ proposals:
    [P1935](https://wg21.link/p1935) ·
    [P2980](https://wg21.link/p2980) ·
    [P3045](https://wg21.link/p3045) ·
    [P4185](https://wg21.link/p4185)
