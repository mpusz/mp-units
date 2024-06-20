# Quick Start

This chapter provides a quick introduction to get you started with **mp-units**.
Much more details can be found in our [User's Guide](../users_guide/terms_and_definitions.md).

## Quantities

A **quantity** is a concrete amount of a unit representing a quantity type of a specified dimension with a
specific representation. It is represented in the library with a `quantity` class template.

The [SI Brochure](../appendix/references.md#SIBrochure) says:

!!! quote "SI Brochure"

    The value of the quantity is the product of the number and the unit. The space between the number
    and the unit is regarded as a multiplication sign (just as a space between units implies
    multiplication).


Following the above, the value of a quantity in the **mp-units** library is created by multiplying
a number with a predefined unit:

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

    In case someone doesn't like the multiply syntax or there is an ambiguity between `operator*`
    provided by this and other libraries, a quantity can also be created with a two-parameter
    constructor:

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
The same can be obtained using optional unit symbols:

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

    Unit symbols introduce a lot of short identifiers into the current scope, which may cause
    naming collisions with unrelated but already existing identifiers in the code base.
    This is why unit symbols are opt-in and typically should be imported only in the context
    where they are being used (e.g., function scope).

    A user has several options here to choose from depending on the required scenario and possible
    naming conflicts:

    === "using-directive"

        Explicitly "import" all of the symbols of a specific system of units from a dedicated
        `unit_symbols` namespace with a
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

            This solution is perfect for small and isolated scopes but can cause surprising issues
            when used in larger scopes or when used for the entire program namespace.

            There are 29 named units in SI, and each of them has many prefixed variations (e.g.,
            `ng`, `kcd`, ...). It is pretty easy to introduce a name collision with those.

    === "using-declaration"

        Selectively bring only the required and not-conflicting symbols with
        [using-declarations](https://en.cppreference.com/w/cpp/language/using_declaration):

        ```cpp
        using namespace mp_units;

        void foo(double N)
        {
          // 'N' function parameter would collide with the SI symbol for Newton, so we only bring what we need
          using si::unit_symbols::m;
          using si::unit_symbols::s;
          quantity speed = N * m / s;
          // ...
        }
        ```

    === "custom short identifier"

        Specify a custom not conflicting unit identifier for a unit:

        ```cpp
        using namespace mp_units;

        void foo(double speed_m_s)
        {
          // names of some local variables are conflicting with the symbols we want to use
          auto m = ...;
          auto s = ...;

          constexpr Unit auto mps = si::metre / si::second;
          quantity speed = speed_m_s * mps;
        }
        ```

    === "unit names"

        Full unit names are straightforward to use and often provide the most readable code:

        ```cpp
        using namespace mp_units;

        void foo(double m, double s)
        {
          quantity speed = m * si::metre / (s * si::second);
          // ...
        }
        ```

Quantities of the same kind can be added, subtracted, and compared to each other:

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

Various quantities can be multiplied or divided by each other:

```cpp
static_assert(140 * km / (2 * h) == 70 * km / h);
```

!!! note

    In case you wonder why this library does not use UDLs to create quantities, please check
    our [FAQ](faq.md#why-dont-we-use-udls-to-create-quantities).


## Quantity points

The quantity point specifies an absolute quantity with respect to an origin.
If no origin is provided explicitly, an implicit one will be provided by the library.

Together with quantities, they model [The Affine Space](../users_guide/framework_basics/the_affine_space.md).

Quantity points should be used in all places where adding two values is meaningless
(e.g., temperature points, timestamps, altitudes, readouts from the car's odometer, etc.).

The set of operations that can be done on quantity points is limited compared to quantities.
This introduces an additional type-safety.

=== "C++ modules"

    ```cpp
    #include <print>
    import mp_units;

    int main()
    {
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;
      using namespace mp_units::usc::unit_symbols;

      quantity_point temp = 20. * absolute(deg_C);
      std::println("Temperature: {} ({})",
                   temp.quantity_from_zero(),
                   temp.in(deg_F).quantity_from_zero());
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/format.h>
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/usc.h>
    #include <print>

    int main()
    {
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;
      using namespace mp_units::usc::unit_symbols;

      quantity_point temp = 20. * absolute(deg_C);
      std::println("Temperature: {} ({})",
                   temp.quantity_from_zero(),
                   temp.in(deg_F).quantity_from_zero());
    }
    ```

The above outputs:

```text
Temperature: 20 °C (68 °F)
```

!!! info

    Check [The Affine Space](../users_guide/framework_basics/the_affine_space.md) chapter to learn
    more about quantity points.
