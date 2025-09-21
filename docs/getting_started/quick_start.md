# Quick Start

This chapter gives a concise tour so you can start using **mp-units** quickly.
More detail lives in the
[User's Guide](../users_guide/terms_and_definitions.md).

## Quantities

A **quantity** is a measurable property of something—like length, mass, or temperature—
that can be described by a number and a reference. Here, a "reference" means not just a unit,
but also all other compile-time known information about the quantity—such as its dimension,
quantity type, character, and other properties. In the library this is modeled by the
`quantity` class template.

_**Note:** In this context, "reference" is an official metrology term and is unrelated to
C++ reference types._

The SI Brochure says:

!!! quote "SI Brochure"

    The value of the quantity is the product of the number and the unit. The space between
    the number and the unit is regarded as a multiplication sign (just as a space between
    units implies multiplication).


Following the above, a quantity value in **mp-units** is created by multiplying a number with
a predefined unit:

=== "C++ modules"

    ```cpp
    import mp_units;

    using namespace mp_units;

    quantity q = 42 * si::metre / si::second;
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>

    using namespace mp_units;

    quantity q = 42 * si::metre / si::second;
    ```

!!! info

    If you dislike the multiply syntax or there is an `operator*` ambiguity with another
    library, two alternative construction styles are available:

    1. `delta` construction helper:

        === "C++ modules"

            ```cpp
            import mp_units;

            using namespace mp_units;

            quantity q = delta<si::metre / si::second>(42);
            ```

        === "Header files"

            ```cpp
            #include <mp-units/systems/si.h>

            using namespace mp_units;

            quantity q = delta<si::metre / si::second>(42);
            ```

    2. A two-parameter constructor:

        === "C++ modules"

            ```cpp
            import mp_units;

            using namespace mp_units;

            quantity q{42, si::metre / si::second};
            ```

        === "Header files"

            ```cpp
            #include <mp-units/systems/si.h>

            using namespace mp_units;

            quantity q{42, si::metre / si::second};
            ```

The above creates an instance of `quantity<derived_unit<si::metre, per<si::second>>{}, int>`.
The same can be expressed with (opt‑in) unit symbols:

=== "C++ modules"

    ```cpp
    import mp_units;

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    quantity q = 42 * m / s;
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    quantity q = 42 * m / s;
    ```

!!! important

    Unit symbols introduce many short identifiers into the current scope, which can collide
    with unrelated existing names. They are therefore opt‑in and are best imported only in
    narrow scope (e.g., a function body) where needed.

    A user has several options depending on the scenario and possible naming conflicts:

    === "using-directive"

        Explicitly bring all symbols of a specific system of units from its `unit_symbols`
        namespace with a
        [using-directive](https://en.cppreference.com/w/cpp/language/namespace#Using-directives):

        ```cpp
        using namespace mp_units;

        void foo(double speed_m_s)
        {
          // imports all the SI symbols at once
          using namespace si::unit_symbols;
          quantity speed = speed_m_s * m / s;
          // ...
        }
        ```

        !!! note

            This works well for small, isolated scopes but may cause surprising issues when
            used at namespace or project scope.

            There are 29 named SI units, each with many prefixed variations (e.g., `ng`,
            `kcd`, ...). It is pretty easy to introduce a name collision with those.

    === "using-declaration"

        Selectively bring only the required, non‑conflicting symbols with
        using-declarations](https://en.cppreference.com/w/cpp/language/using_declaration):

        ```cpp
        using namespace mp_units;

        void foo(double N)
        {
          // 'N' would collide with the SI symbol for Newton; only bring what we need
          using si::unit_symbols::m;
          using si::unit_symbols::s;
          quantity speed = N * m / s;
          // ...
        }
        ```

    === "custom short identifier"

        Specify a custom, non‑conflicting identifier for a unit:

        ```cpp
        using namespace mp_units;

        void foo(double speed_m_s)
        {
          // local variable names conflict with the symbols we want to use
          auto m = ...;
          auto s = ...;

          constexpr Unit auto mps = si::metre / si::second;
          quantity speed = speed_m_s * mps;
        }
        ```

    === "unit names"

        Full unit names are straightforward and often the most readable:

        ```cpp
        using namespace mp_units;

        void foo(double m, double s)
        {
          quantity speed = m * si::metre / (s * si::second);
          // ...
        }
        ```

Quantities of the same kind can be added, subtracted, and compared:

=== "C++ modules"

    ```cpp
    import mp_units;

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    static_assert(1 * km + 50 * m == 1050 * m);
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    static_assert(1 * km + 50 * m == 1050 * m);
    ```

Different quantities can be multiplied or divided to produce derived quantities:

```cpp
static_assert(140 * km / (2 * h) == 70 * km / h);
```

!!! note

    Wondering why user‑defined literals (UDLs) are not used?
    See the [FAQ](faq.md#why-dont-we-use-udls-to-create-quantities).


## Quantity points

The quantity point specifies an absolute quantity relative to an origin. If no origin is
provided explicitly, an implicit one is supplied by the library.

Together with quantities, they model [The Affine Space](../users_guide/framework_basics/the_affine_space.md).

Use quantity points where adding two absolute values is meaningless (temperature points,
timestamps, altitudes, odometer readings, etc.).

Fewer operations are available on quantity points than on quantities—by design for
additional type safety.

=== "C++ modules"

    ```cpp
    #include <print>
    import mp_units;

    int main()
    {
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;
      using namespace mp_units::usc::unit_symbols;

      quantity_point temp = point<deg_C>(20.);
      std::println("Temperature: {} ({})",
                   temp.quantity_from_zero(),
                   temp.in(deg_F).quantity_from_zero());
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/usc.h>
    #include <print>

    int main()
    {
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;
      using namespace mp_units::usc::unit_symbols;

      quantity_point temp = point<deg_C>(20.);
      std::println("Temperature: {} ({})",
                   temp.quantity_from_zero(),
                   temp.in(deg_F).quantity_from_zero());
    }
    ```

The above outputs:

```text
Temperature: 20 ℃ (68 ℉)
```

!!! info

    See [The Affine Space](../users_guide/framework_basics/the_affine_space.md) for details
    on quantity points.
