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

??? info "Supported compilers"

    This library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses C++20 features and the
    [explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
    from C++23.

    Even though the library benefits from C++23 (if available), C++20 is enough to compile and
    use all of the library's functionality. C++23 features are hidden behind
    a [preprocessor macro](users_guide/framework_basics/systems_of_quantities.md#defining-quantities)
    providing a backward-compatible way to use it.

    The below table provides the minimum compiler version required to compile the code using the
    specific feature:

    | Feature              | gcc  | clang | apple-clang | MSVC |
    |----------------------|:----:|:-----:|:-----------:|:----:|
    | **Minimum support**  |  12  |  16   |     15      | None |
    | **`std::format`**    | None | None  |    None     | None |
    | **C++ modules**      | None |  17   |    None     | None |
    | **C++23 extensions** |  14  |  18   |    None     | None |

    More requirements for C++ modules support can be found in the
    [CMake's documentation](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html).

=== "C++ modules"

    ```cpp
    #include <iostream>
    import mp_units;

    using namespace mp_units;

    inline constexpr struct smoot : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

    int main()
    {
      constexpr quantity dist = 364.4 * smoot;
      std::cout << "Harvard Bridge length = " << dist << "(" << dist.in(usc::foot) << ", " << dist.in(si::metre) << ") ± 1 εar\n";
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/ostream.h>
    #include <mp-units/systems/si/si.h>
    #include <mp-units/systems/usc/usc.h>
    #include <iostream>

    using namespace mp_units;

    inline constexpr struct smoot : named_unit<"smoot", mag<67> * usc::inch> {} smoot;

    int main()
    {
      constexpr quantity dist = 364.4 * smoot;
      std::cout << "Harvard Bridge length = " << dist << "(" << dist.in(usc::foot) << ", " << dist.in(si::metre) << ") ± 1 εar\n";
    }
    ```

Output:

```txt
Harvard Bridge length = 364.4 smoot(2034.57 ft, 620.136 m) ± 1 εar
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/K66zKsT89)"

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
    - [P2981: Improving our safety with a physical quantities and units library](https://wg21.link/p2981),
    - [P2982: `std::quantity` as a numeric type](https://wg21.link/p2982).

    We are actively looking for parties interested in field-trialing the library.
