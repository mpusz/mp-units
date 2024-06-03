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
    inline constexpr struct dim_currency : base_dimension<"$"> {} dim_currency;
    inline constexpr struct currency : quantity_spec<dim_currency> {} currency;

    inline constexpr struct us_dollar : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

    namespace unit_symbols {

    inline constexpr auto USD = us_dollar;
    inline constexpr auto USD_s = scaled_us_dollar;

    }  // namespace unit_symbols

    using Price = quantity_point<currency[us_dollar]>;
    using Scaled = quantity_point<currency[scaled_us_dollar], zeroth_point_origin<currency>, std::int64_t>;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct dim_currency : base_dimension<"$"> {} dim_currency;
    inline constexpr struct currency : quantity_spec<currency, dim_currency> {} currency;

    inline constexpr struct us_dollar : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

    namespace unit_symbols {

    inline constexpr auto USD = us_dollar;
    inline constexpr auto USD_s = scaled_us_dollar;

    }  // namespace unit_symbols

    using Price = quantity_point<currency[us_dollar]>;
    using Scaled = quantity_point<currency[scaled_us_dollar], zeroth_point_origin<currency>, std::int64_t>;
    ```

=== "Portable"

    ```cpp
    inline constexpr struct dim_currency : base_dimension<"$"> {} dim_currency;
    QUANTITY_SPEC(currency, dim_currency);

    inline constexpr struct us_dollar : named_unit<"USD", kind_of<currency>> {} us_dollar;
    inline constexpr struct scaled_us_dollar : named_unit<"USD_s", mag_power<10, -8> * us_dollar> {} scaled_us_dollar;

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

As a shortcut, instead of providing a unit and a representation type to `value_cast`, you may also provide a
`Quantity` type directly, from which unit and representation type are taken. However, `value_cast<Quantity>`,
still only allows for changes in unit and representation type, but not changing the type of the quantity.
For that, you will have to use a `quantity_cast` instead.

Overloads are also provided for instances of `quantity_point`.
All variants of `value_cast<...>(q)` that apply to instances of `quantity`
have a corresponding version applicable to `quantity_point`, where the `point_origin` remains untouched,
and the cast changes how the "offset" from the origin is represented.
Specifically, for any `quantity_point` instance `qp`, all of the following equivalences hold:
```cpp
static_assert( value_cast<Rep>(qp) == quantity_point{value_cast<Rep>(qp.quantity_from(qp.point_origin)), qp.point_origin} );
static_assert( value_cast<U>(qp) == quantity_point{value_cast<U>(qp.quantity_from(qp.point_origin)), qp.point_origin} );
static_assert( value_cast<U, Rep>(qp) == quantity_point{value_cast<U, Rep>(qp.quantity_from(qp.point_origin)), qp.point_origin} );
static_assert( value_cast<Q>(qp) == quantity_point{value_cast<Q>(qp.quantity_from(qp.point_origin)), qp.point_origin} );
```

Furthermore, there is one additional overload `value_cast<ToQP>(qp)`.
This overload permits to additionally replace the `point_origin` with another compatible one,
while still representing the same point in the affine space.
Thus, it is roughly equivalent to
`value_cast<ToQP::unit, ToQP::rep>(qp).point_for(ToQP::point_origin)`.
In contrast to a separate `value_cast` followed by `point_for` (or vice-versa), the combined
`value_cast` tries to choose the order of the individual conversion steps in a way
to avoid both overflow and unnecessary loss of precision. Overflow is a risk because the change of origin point
may require an addition of a potentially large offset (the difference between the origin points),
which may well be outside the range of one or both quantity types.
