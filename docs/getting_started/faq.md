# Frequently Asked Questions

## Why do we spell `metre` instead of `meter`?

This is how ISO 80000 defines it (British English spelling by default).


## Why don't we use UDLs to create quantities?

Many reasons make UDLs a poor choice for a physical units library:

1. UDLs work only with literals (compile-time known values). Our observation is that besides
   the unit tests, there are few compile-time known constants used in the production code.
2. Typical implementations of UDLs tend to always use the widest representation type available.
   In the case of `std::chrono::duration`, the following is true:

    ```cpp
    using namespace std::chrono_literals;
    auto d1 = 42s;
    auto d2 = 42.s;
    static_assert(std::is_same_v<decltype(d1)::rep, std::int64_t>);
    static_assert(std::is_same_v<decltype(d2)::rep, long double>);
    ```

3. While increasing the coverage for the library, we learned that many unit symbols conflict  with
   built-in types or numeric extensions. A few of those are: `F` (farad), `J` (joule), `W` (watt),
   `K` (kelvin), `d` (day), `l` or `L` (litre), `erg`, `ergps`. For a while for those we used `_` prefix
   to make the library work at all, but at some point, we had to unify the naming, and we came up with `_q_`
   prefix, which resulted in creating a quantity of a provided unit. So in case the library is
   standardized, all quantities would be created with UDLs having `q_` prefix (i.e. `42q_s`)
   which is not that nice anymore.

4. UDLs with the same identifiers defined in different namespace can't be disambiguated in the C++
   language. If both SI and CGS systems define `_q_s` UDL for a second unit, then it would not be possible
   to specify which one to use in case both namespaces are "imported".

5. Another bad property of UDLs is that they do not compose. A coherent unit of angular momentum would
   have a UDL specified as `_q_kg_m2_per_s`. Now imagine that you want to make every possible user happy.
   How many variations of that unit would you predefine for differently scaled versions of unit ingredients?

6. UDLs are also really expensive to define and specify. For each unit, we need two definitions. One for
   integral and another one for floating-point representation. Before the V2 framework, the coherent unit of
   angular momentum was defined as:

    ```cpp
    constexpr auto operator"" _q_kg_m2_per_s(unsigned long long l)
    {
      gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
      return angular_momentum<kilogram_metre_sq_per_second, std::int64_t>(static_cast<std::int64_t>(l));
    }
    constexpr auto operator"" _q_kg_m2_per_s(long double l)
    {
      return angular_momentum<kilogram_metre_sq_per_second, long double>(l);
    }
    ```


## Why `60 * km / h` does not compile?

The library design does not allow multiplying or dividing a quantity (the result of `60 * km`)
by another unit. This significantly limits the number of possible errors and surprises in the
quantity equations.

Consider the following expression:

```cpp
auto q = 60 * km / 2 * h;
```

Looks like `30 km/h`, right? But it is not. If the above code was allowed, it would result
in `30 km⋅h`. In case you want to divide `60 * km` by `2 * h` a parenthesis is needed
`60 * km / (2 * h)`.

Another surprising issue could result from the following code:

```cpp
template<typename T>
auto make_length(T v) { return v * si::metre; }

auto v = 42;
auto q = make_length(v);
```

The above might look like a good idea, but consider what would happen in the user provided
an already existing quantity:

```cpp
auto v = 42 * m;
auto q = make_length(v);
```

Fortunately, with the current design, such issues are detected at compile-time.


## Why a dimensionless quantity is not just a fundamental arithmetic type?

In the initial design of this library, the resulting type of division of two quantities was their
common representation type:

```cpp
static_assert(std::is_same_v<decltype(10 * km / (5 * km)), std::int64_t>);
```

First of all, this was consistent with
[`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration/operator_arith4) behavior.
Additional reasoning behind it was not providing a false impression of a strong `quantity` type for
something that looks and feels like a regular number. Also, all of the mathematic and trigonometric functions
were working fine out of the box with such representation types, so we did not have to rewrite
`sin()`, `cos()`, `exp()`, and others.

However, the feedback we got from the production usage was that such an approach is really bad for generic
programming. It is hard to handle the result of the two quantities' division (or multiplication) as
it might be either a quantity or a fundamental type. If we want to raise such a result to some power, we
must use `units::pow` or `std::pow` depending on the resulting type. Those are only a few issues related
to such an approach.

Moreover, suppose you divide quantities of the same dimension but with units of significantly different
magnitudes. In that case, you may end up with a really small or a huge floating-point value, which may result
in losing lots of precision. Returning a dimensionless quantity from such cases allows us to benefit from
all the properties of scaled units and is consistent with the rest of the library.

!!! info

    More information on the current design can be found in the Dimensionless Quantities chapter.


## Why Unicode quantity symbols are used by default instead of ASCII-only characters?

Both C++ and ISO 80000 are standardized by the ISO. ISO 80000 and the SI standards specify Unicode symbols
as the official unit names for some quantities (i.e. `Ω` symbol for the resistance quantity).
As **mp-units** library will be proposed for standardization as a part of the C++ Standard Library
we have to obey the rules and be consistent with ISO specifications.

!!! info

    We do understand engineering reality and the constraints of some environments. This is why the library
    has the option of ASCII-only Quantity Symbols.


## Why don't you have CMake options to disable the building of tests and examples?

Over time many people provided PRs proposing adding options to build tests and examples conditionally.
Here are a few examples:

- [Add CMake options for disabling docs, examples and tests](https://github.com/mpusz/mp-units/pull/124)
- [build: add options to disable part of the build](https://github.com/mpusz/mp-units/pull/402)
- [CMake Refactoring and Option Cleanup](https://github.com/mpusz/mp-units/pull/456)

We admit this is a common practice in the industry, but we also believe this is a bad pattern.

First, the only need for such options comes when a user wants to use `add_subdirectory()` in CMake
to handle dependencies. Such an approach does not scale and should be discouraged. There is little
use for such a practice in times when we have dedicated package managers like Conan.

The second thing is that our observation is that many people are fixed on disabling "unneeded" subdirectories
from compilation, but they do not see or address the biggest issue, which is polluting user's build
environment with our development-specific settings. Propagating our restrictive compilation flags to user's
project is not the best idea as it might cause a lot of harm if this project stops to compile
because of that.

Last but not least, not having those options is on purpose. Top level _CMakeLists.txt_ file should only
be used by **mp-units** developers and contributors as an entry point for project's development.
We want to ensure that everyone will build **ALL** the code correctly before pushing a commit. Having
such options would allow unintended issues to leak to PRs and CI.

This is why our projects have two entry points:

- _./CMakeLists.txt_ is to be used by projects developers to build **ALL** the project code with really
  restrictive compilation flags,
- _./src/CMakeLists.txt_ contains only a pure library definition and should be used by the customers
  that prefer to use CMake's `add_subdirectory()` to handle the dependencies.

!!! info

    For more details on this please refer to the [CMake + Conan: 3 Years Later - Mateusz Pusz](https://youtu.be/mrSwJBJ-0z8?t=1931)
    lecture that Mateusz Pusz provided at the C++Now 2021 conference.
