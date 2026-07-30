# Value Conversions

One of the most important features of every unit library is to provide support for
compile-time-enabled conversions of a [numerical value of a quantity](../../reference/glossary.md#numerical-value).

A [numerical value of a quantity](../../reference/glossary.md#numerical-value) depends on
two elements:

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

The second solution is to explicitly accept the value loss. Every conversion that the
framework considers truncating requires the user to state what should happen to the
information that does not fit in the result. This is done by passing a **rounding policy**
as the last function argument:

```cpp
quantity q1 = 1234 * m;
std::cout << q1.in(km, truncated) << '\n';     // 1 km
std::cout << q1.in(km, rounded) << '\n';       // 1 km
std::cout << q1.in(km, rounded_up) << '\n';    // 2 km
std::cout << q1.in(km, rounded_down) << '\n';  // 1 km
quantity<si::kilo<si::metre>, int> q2 = q1.in(km, rounded);
```

The policy makes it clear at the call site that the conversion loses information and, at
the same time, states precisely which representable value gets selected. It is easy to spot
in code reviews or while chasing a bug in the source code.

The library provides four rounding policies:

| Policy         | Semantics                                                           | Consistent with                             |
|----------------|---------------------------------------------------------------------|---------------------------------------------|
| `truncated`    | rounds towards zero                                                 | `static_cast`, `std::chrono::duration_cast` |
| `rounded`      | rounds to the nearest representable value, to even in halfway cases | `std::chrono::round`                        |
| `rounded_down` | rounds towards negative infinity                                    | `std::chrono::floor`                        |
| `rounded_up`   | rounds towards positive infinity                                    | `std::chrono::ceil`                         |

!!! note

    `mp_units::floor/ceil/round<U>(q)` perform a different operation. They round to an
    integral multiple of the provided unit even when the representation could store the
    exact result (e.g. for a floating-point representation). The rounding policies never
    adjust a value that the destination type represents exactly. Both coincide for
    integral representations.

!!! important

    `truncated` and `rounded_down` differ for negative values. `(-1234 * m).in(km, truncated)`
    yields `-1 km` while `(-1234 * m).in(km, rounded_down)` yields `-2 km`. This is exactly
    the trap that made `std::chrono` retrofit `floor()`, `ceil()`, and `round()` after
    shipping the truncating `duration_cast`. In **mp-units**, the conversion states its
    rounding direction explicitly, so there is no default to be surprised by.

Another place where such a conversion is useful is when a user wants to convert a quantity
with a floating-point representation to the one using an integral one. Again, this is
a truncating conversion, so a rounding policy is needed:

```cpp
quantity<si::metre, int> q3 = (3.14 * m).in<int>(truncated);
quantity<si::metre, int> q4 = (3.14 * m).in<int>(rounded);
```

!!! info

    It is often OK to use an integral as a representation type, but in general, floating-point
    types provide better precision and are privileged in the library as they are considered
    to be value-preserving.

The same policies may be passed as the last argument to `value_cast`, which remains the
escape hatch for generic contexts where the representation type is a dependent name and
the member function would require the `template` disambiguator:

```cpp
quantity q5 = value_cast<km>(1567 * m, rounded);  // 2 km
quantity q6 = value_cast<int>(3.14 * m);          // truncated (static_cast semantics)
```

When no policy is provided, `value_cast` keeps its `static_cast`-like (`truncated`) semantics
for backward compatibility.

!!! note

    A floating-point destination represents every conversion result with a rounding error
    of at most half ULP, which is the semantics of `rounded`, and the directed modes cannot
    be delivered for it. This is why a floating-point destination accepts only the
    `truncated` (understood as the `static_cast` semantics) and `rounded` policies.

    Rounding happens only on the way to an integral representation, so that is where the policies
    ask something of the representation type:

    - An adjusting policy has to decide which side of a rounding boundary the exact result falls
      on, which needs a _real scalar_. A real scalar wrapper such as [`safe_int`](safe_int.md)
      works with every policy, while a vector, tensor, or complex representation offers no single
      value to compare against a boundary and takes only `truncated` there.
    - Rounding a floating-point value to an integral one works on the value's integral part, which
      the library performs for standard floating-point types only.

In some cases, a unit and a representation type should be changed simultaneously. Moreover,
sometimes, the order of doing those operations matters. In such cases, the library provides
the `value_cast<U, Rep>(q)` and `q.in<Rep>(U, policy)` which always return the most precise
result:

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
    using Scaled = quantity_point<currency[scaled_us_dollar], natural_point_origin<currency>, std::int64_t>;
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
    using Scaled = quantity_point<currency[scaled_us_dollar], natural_point_origin<currency>, std::int64_t>;
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
    using Scaled = quantity_point<currency[scaled_us_dollar], natural_point_origin<currency>, std::int64_t>;
    ```

```cpp
using namespace unit_symbols;
Price price{12.95 * USD};
Scaled spx1 = value_cast<USD_s, std::int64_t>(price);
Scaled spx2 = price.in<std::int64_t>(USD_s, truncated);
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
    quantity q = (std::int64_t{1'000'000'000'000'000'000} * deg).in(grad, truncated);
    // Expected:          1'111'111'111'111'111'111ᵍ
    // Naive result:       -938'527'119'301'061'290ᵍ (silent undefined behaviour)
    // mp-units result:   1'111'111'111'111'111'111ᵍ (correct)
    ```

- **Floating-point dependency** — conversions involving irrational factors (e.g. `deg → rad`,
  factor `π/180`) require a `double` intermediate in a naive implementation.  This fails
  silently on FPU-less embedded targets and loses precision for 64-bit integer values
  (a `double` has only 53 bits of mantissa).

Both challenges are addressed by using **fixed-point arithmetic**: the conversion factor is
represented at compile time as a widened integer constant (64-bit for types up to 32 bits,
128-bit for `int64_t`), so the runtime computation is a pure integer multiply followed by
a right-shift with no risk of intermediate overflow and no floating-point operations.

??? info "Implementation details"

    The library distinguishes three sub-cases based on the magnitude $M$ that relates the
    two units:

    | Case              | Condition                 | Example                    | Operation            | Conversion |
    |-------------------|---------------------------|----------------------------|----------------------|:----------:|
    | Integral factor   | $M \in \mathbb{Z}^+$      | `m → mm` ($\times 1000$)   | `value * M`          |  implicit  |
    | Integral divisor  | $M^{-1} \in \mathbb{Z}^+$ | `mm → m` ($\div 1000$)     | `value / M`          |  explicit  |
    | Non-integer ratio | otherwise                 | `ft → m` ($\times 0.3048$) | fixed-point multiply |  explicit  |

    For the non-integer case the magnitude is converted **at compile time** to a
    fixed-point constant with widened bit-width. The library uses **`int64_t` for
    all types up to 32 bits** (`int8_t`, `int16_t`, `int32_t`) and **128-bit arithmetic
    for `int64_t`**.  The actual runtime computation is then a pure integer multiply
    followed by a right-shift:

    $$
    \text{result} = \left\lfloor \text{value} \times \lfloor M \cdot 2^N \rfloor \right\rfloor \gg N
    $$

    where $N$ equals 64 for types up to 32 bits, and 128 for `int64_t`.  On platforms
    where `__int128` is available (most 64-bit targets), the 128-bit arithmetic is
    implemented natively; on others, a portable `double_width_int` emulation is used in
    `constexpr` context.

    Because the intermediate is significantly widened (e.g., `int64_t` for types up to
    32 bits, providing much more than double-width headroom for smaller types), it cannot
    overflow as long as the input value fits in the representation type — for example,
    a value of `std::int32_t` computed in `int64_t` has 32 extra bits of safety margin.

    For the non-integer ratio path, the result honors the requested rounding policy
    (`truncated` rounds towards zero, and the remaining modes adjust the floor quotient
    computed by the shift accordingly). The fixed-point constant is rounded *away* from
    zero at compile time to compensate for one level of double-rounding, keeping the
    maximum error within 1 ULP of the true result (i.e. at most ±1 relative to the last
    bit of the output). Because the factor itself is an approximation, a rounding policy
    may select the neighboring representable value when the exact result lies within that
    error of a rounding boundary.

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
quantity q2 = q1.in(m, truncated);  // Compile-time error (1)
if(q1 != 1 * m) { /* ... */ }       // Compile-time error (2)
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
conversion machinery — including how to implement the correct `operator*` and
`implicitly_scalable<FromUnit, FromRep, ToUnit, ToRep>` — see
[Using Custom Representation Types](../../how_to_guides/integration/using_custom_representation_types.md#creating-your-own-representation-type).


## Value conversions summary

The table below provides all the value conversion functions that may be run on `x` being the
instance of either `quantity` or `quantity_point`:

In the table below, `p` is one of the rounding policies (`truncated`, `rounded`,
`rounded_down`, `rounded_up`), and providing it is what enables a truncating conversion:

| Truncating | Representation | Unit | Member function | Non-member function                                  |
|:----------:|:--------------:|:----:|-----------------|------------------------------------------------------|
|     No     |      Same      | `u`  | `x.in(u)`       |                                                      |
|     No     |      `T`       | Same | `x.in<T>()`     |                                                      |
|     No     |      `T`       | `u`  | `x.in<T>(u)`    |                                                      |
|    Yes     |      Same      | `u`  | `x.in(u, p)`    | `value_cast<u>(x, p)`                                |
|    Yes     |      `T`       | Same | `x.in<T>(p)`    | `value_cast<T>(x, p)`                                |
|    Yes     |      `T`       | `u`  | `x.in<T>(u, p)` | `value_cast<u, T>(x, p)` or `value_cast<T, u>(x, p)` |

The non-member functions may also be called without a policy, in which case they keep the
`static_cast`-like (`truncated`) truncating semantics.
