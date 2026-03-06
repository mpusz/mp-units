# Value Conversions

One of the most important features of every unit library is to provide support for
compile-time-enabled conversions of a [numerical value of a quantity](../../reference/glossary.md#numerical-value).

A [numerical value of a quantity](../../reference/glossary.md#numerical-value) depends on two
elements:

- quantity representation type (e.g., `int`, `double`) that stores the number expressing
  the amount of quantity,
- [unit](../../reference/glossary.md#unit) in which the quantity is being measured.

Changing any of the above may require changing the value stored in a quantity.


## Value-preserving conversions

```cpp
quantity q1 = 5 * km;
std::cout << q1.in(m) << '\n';
quantity<si::metre, int> q2 = q1;
```

The second line above converts the current quantity to the one expressed in meters and
prints its contents. The third line converts the quantity expressed in kilometers into
the one measured in meters.

In case a user would like to perform an opposite transformation:

```cpp
quantity q1 = 5 * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1;
```

Both conversions will fail to compile because they try to truncate the quantity value.

There are two ways to make the above work. The first solution is to use a floating-point
representation type:

```cpp
quantity q1 = 5. * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;
```

or

```cpp
quantity q1 = 5 * m;
std::cout << q1.in<double>(km) << '\n';
std::cout << value_cast<double>(q1).in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;  // double by default
```

!!! important

    The **mp-units** library follows [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration)
    logic and treats floating-point types as implicitly convertible to any unit —
    see [`implicitly_scalable`](../../how_to_guides/integration/using_custom_representation_types.md#implicitly_scalable)
    for details.


## Value-truncating conversions

The second solution is to force a truncating conversion:

```cpp
quantity q1 = 5 * m;
std::cout << value_cast<km>(q1) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1.force_in(km);
```

This explicit cast makes it clear that something unsafe is going on. It is easy to spot
in code reviews or while chasing a bug in the source code.

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
the `value_cast<U, Rep>(q)` and `q.force_in<Rep>(U)` which always return the most precise
result:

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
Scaled spx1 = value_cast<USD_s, std::int64_t>(price);
Scaled spx2 = price.force_in<std::int64_t>(USD_s);
```

As a shortcut, instead of providing a unit and a representation type to `value_cast`, you
may also provide a `Quantity` type directly, from which unit and representation type are
taken. However, `value_cast<Quantity>`, still only allows for changes in unit and
representation type, but not changing the type of the quantity. For that, you will have
to use a [`quantity_cast`](simple_and_typed_quantities.md#quantity_cast-to-force-unsafe-conversions)
instead.

Overloads are also provided for instances of [`quantity_point`](the_affine_space.md#quantity_point).
All variants of `value_cast<...>(q)` that apply to instances of `quantity` have a corresponding
version applicable to `quantity_point`, where the `point_origin` remains untouched, and
the cast changes how the "offset" from the origin is represented. Specifically, for any
`quantity_point` instance `qp`, all of the following equivalences hold:

```cpp
static_assert(value_cast<Rep>(qp) == quantity_point{value_cast<Rep>(qp.quantity_from(qp.point_origin)), qp.point_origin});
static_assert(value_cast<U>(qp) == quantity_point{value_cast<U>(qp.quantity_from(qp.point_origin)), qp.point_origin});
static_assert(value_cast<U, Rep>(qp) == quantity_point{value_cast<U, Rep>(qp.quantity_from(qp.point_origin)), qp.point_origin});
static_assert(value_cast<Q>(qp) == quantity_point{value_cast<Q>(qp.quantity_from(qp.point_origin)), qp.point_origin});
```

Furthermore, there is one additional overload `value_cast<ToQP>(qp)`. This overload permits
to additionally replace the `point_origin` with another compatible one, while still representing
the same point in the affine space. Thus, it is roughly equivalent to
`value_cast<ToQP::unit, ToQP::rep>(qp).point_for(ToQP::point_origin)`.
In contrast to a separate `value_cast` followed by `point_for` (or vice-versa), the combined
`value_cast` tries to choose the order of the individual conversion steps in a way to avoid
both overflow and unnecessary loss of precision. Overflow is a risk because the change of
origin point may require an addition of a potentially large offset (the difference between
the origin points), which may well be outside the range of one or both quantity types.


## Integer scaling: fixed-point arithmetic

When both the source and target representation are integral types, unit conversions with
a non-integer conversion factor (e.g. `deg → grad`, factor 10/9) raise two challenges
that a naive implementation cannot handle correctly:

- **Intermediate overflow** — computing `value × num / den` in `intmax_t` overflows for
  large values even when the final result fits in the representation type, producing
  silently wrong results:

    ```cpp
    // deg -> grad: factor 10/9
    // A naive implementation multiplies first: 1e18 * 10 overflows int64_t (max ≈ 9.22e18):
    quantity q = (std::int64_t{1'000'000'000'000'000'000} * deg).force_in(grad);
    // Expected:          1'111'111'111'111'111'111ᵍ
    // Naive result:       -938'527'119'301'061'290ᵍ (silent undefined behaviour)
    // mp-units result:   1'111'111'111'111'111'111ᵍ (correct)
    ```

- **Floating-point dependency** — conversions involving irrational factors (e.g. `deg → rad`,
  factor `π/180`) require a `double` intermediate in a naive implementation.  This fails
  silently on FPU-less embedded targets and loses precision for 64-bit integer values
  (a `double` has only 53 bits of mantissa).

Both challenges are addressed by using **fixed-point arithmetic**: the conversion factor is
represented at compile time as a double-width integer constant, so the runtime computation
is a pure integer multiply followed by a right-shift with no risk of intermediate overflow
and no floating-point operations.

??? info "Implementation details"

    The library distinguishes three sub-cases based on the magnitude $M$ that relates the
    two units:

    | Case              | Condition                 | Example                    | Operation            | Conversion |
    |-------------------|---------------------------|----------------------------|----------------------|:----------:|
    | Integral factor   | $M \in \mathbb{Z}^+$      | `m → mm` ($\times 1000$)   | `value * M`          |  implicit  |
    | Integral divisor  | $M^{-1} \in \mathbb{Z}^+$ | `mm → m` ($\div 1000$)     | `value / M`          |  explicit  |
    | Non-integer ratio | otherwise                 | `ft → m` ($\times 0.3048$) | fixed-point multiply |  explicit  |

    For the non-integer case the magnitude is converted **at compile time** to a
    fixed-point constant with double the bit-width of the representation type.  For
    example, when scaling a 32-bit integer value, a 64-bit fixed-point intermediate is
    used.  The actual runtime computation is then a pure integer multiply followed by a
    right-shift:

    $$
    \text{result} = \left\lfloor \text{value} \times \lfloor M \cdot 2^N \rfloor \right\rfloor \gg N
    $$

    where $N$ equals the bit-width of the source representation type.  On platforms where
    `__int128` is available (most 64-bit targets), the double-width arithmetic is
    implemented natively; on others, a portable `double_width_int` emulation is used in
    `constexpr` context.

    Because the intermediate is double-width, it cannot overflow as long as the input
    value fits in the representation type — a value of `std::int64_t` will never silently
    overflow during the multiplication step.

    For the non-integer ratio path, the result is **truncated toward zero**.  The
    fixed-point constant is rounded *away* from zero at compile time to compensate for
    one level of double-rounding, keeping the maximum error within 1 ULP of the true
    result (i.e. at most ±1 relative to the last bit of the output).

    !!! hint

        Chained conversions can accumulate this truncation error additively.  Where exact
        round-trip behavior is required, prefer floating-point representations or perform
        conversions in a single step rather than via an intermediate unit.


## Scaling overflow prevention

In the case of small integral types, it is easy to overflow the representation type for
every value besides `0` while performing simple and popular unit conversions. This is why
the library prevents such invalid conversions at compile-time both for explicit and implicit
conversions:

```cpp
quantity q1 = std::int8_t(1) * km;
quantity q2 = q1.force_in(m);   // Compile-time error (1)
if(q1 != 1 * m) { /* ... */ }   // Compile-time error (2)
```

1. Forced conversion would overflow on scaling.
2. Implicit conversion that brings arguments to a common unit before comparison would
   overflow on scaling.

In the above example, the conversion factor between `km` and `m` is `1'000`, which is
larger than the maximum value that can be stored in `std::int8_t`. Even if we want to
convert the smallest possible integral amount (e.g., `1 km`), we will overflow the quantity
representation type. We decided not to allow such conversions for safety reasons despite
the value of `0 km` would work.


## Custom representation types

For information on how to integrate a custom representation type with the quantity
conversion machinery — including how to provide a `scaling_traits<From, To>` specialization
and `implicitly_scalable<FromUnit, FromRep, ToUnit, ToRep>` — see
[Using Custom Representation Types](../../how_to_guides/integration/using_custom_representation_types.md#scaling_traits).


## Value conversions summary

The table below provides all the value conversion functions that may be run on `x` being the
instance of either `quantity` or `quantity_point`:

| Forcing | Representation | Unit | Member function    | Non-member function                            |
|:-------:|:--------------:|:----:|--------------------|------------------------------------------------|
|   No    |      Same      | `u`  | `x.in(u)`          |                                                |
|   No    |      `T`       | Same | `x.in<T>()`        |                                                |
|   No    |      `T`       | `u`  | `x.in<T>(u)`       |                                                |
|   Yes   |      Same      | `u`  | `x.force_in(u)`    | `value_cast<u>(x)`                             |
|   Yes   |      `T`       | Same | `x.force_in<T>()`  | `value_cast<T>(x)`                             |
|   Yes   |      `T`       | `u`  | `x.force_in<T>(u)` | `value_cast<u, T>(x)` or `value_cast<T, u>(x)` |
