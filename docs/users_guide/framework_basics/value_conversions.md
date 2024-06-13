# Value Conversions

## Value-preserving conversions

```cpp
auto q1 = 5 * km;
std::cout << q1.in(m) << '\n';
quantity<si::metre, int> q2 = q1;
```

The second line above converts the current quantity to the one expressed in meters and prints its
contents. The third line converts the quantity expressed in kilometers into the one measured
in meters.

In case a user would like to perform an opposite transformation:

```cpp
auto q1 = 5 * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1;
```

Both conversions will fail to compile.

There are two ways to make the above work. The first solution is to use a floating-point
representation type:

```cpp
auto q1 = 5. * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;
```

or

```cpp
auto q1 = 5 * m;
std::cout << value_cast<double>(q1).in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;  // double by default
```

!!! important

    The **mp-units** library follows [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration)
    logic and treats floating-point types as value-preserving.


## Value-truncating conversions

The second solution is to force a truncating conversion:

```cpp
auto q1 = 5 * m;
std::cout << value_cast<km>(q1) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1.force_in(km);
```

This explicit cast makes it clear that something unsafe is going on. It is easy to spot in code
reviews or while chasing a bug in the source code.

!!! note

    `q.force_in(U)` is just a shortcut to run `value_cast<U>(q)`. There is no difference in behavior
    between those two interfaces. `q.force_in(U)` was added for consistency with `q.in(U)` and
    `q.force_numerical_value_in(U)`.

Another place where this cast is useful is when a user wants to convert a quantity with
a floating-point representation to the one using an integral one. Again, this is a truncating
conversion, so an explicit cast is needed:

```cpp
quantity<si::metre, int> q3 = value_cast<int>(3.14 * m);
```

!!! info

    It is often OK to use an integral as a representation type, but in general, floating-point
    types provide better precision and are privileged in the library as they are considered
    to be value-preserving.

In some cases, a unit and a representation type should be changed simultaneously. Moreover,
sometimes, the order of doing those operations matters. In such cases, the library provides
the `value_cast<U, Rep>(q)` which always returns the most precise result:

=== "C++23"

    ```cpp
    inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
    inline constexpr struct currency final : quantity_spec<dim_currency> {} currency;

    inline constexpr struct us_dollar final : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar final : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

    namespace unit_symbols {

    inline constexpr auto USD = us_dollar;
    inline constexpr auto USD_s = scaled_us_dollar;

    }  // namespace unit_symbols

    using Price = quantity_point<currency[us_dollar]>;
    using Scaled = quantity_point<currency[scaled_us_dollar], zeroth_point_origin<currency>, std::int64_t>;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
    inline constexpr struct currency final : quantity_spec<currency, dim_currency> {} currency;

    inline constexpr struct us_dollar final : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar final : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

    namespace unit_symbols {

    inline constexpr auto USD = us_dollar;
    inline constexpr auto USD_s = scaled_us_dollar;

    }  // namespace unit_symbols

    using Price = quantity_point<currency[us_dollar]>;
    using Scaled = quantity_point<currency[scaled_us_dollar], zeroth_point_origin<currency>, std::int64_t>;
    ```

=== "Portable"

    ```cpp
    inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
    QUANTITY_SPEC(currency, dim_currency);

    inline constexpr struct us_dollar final : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar final : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

    namespace unit_symbols {

    inline constexpr auto USD = us_dollar;
    inline constexpr auto USD_s = scaled_us_dollar;

    }  // namespace unit_symbols

    using Price = quantity_point<currency[us_dollar]>;
    using Scaled = quantity_point<currency[scaled_us_dollar], zeroth_point_origin<currency>, std::int64_t>;
    ```

```cpp
using namespace unit_symbols;
Price price{12.95 * USD};
Scaled spx = value_cast<USD_s, std::int64_t>(price);
```
