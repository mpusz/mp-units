# Interface Introduction

## New style of definitions

The library uses an unusual but purposeful pattern to define entities. Here is how `metre`
and `second` [SI](../../appendix/glossary.md#si) base units are defined:

```cpp
inline constexpr struct metre final : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct second final : named_unit<"s", kind_of<isq::time>> {} second;
```

The above reuses the same identifier for a type and its value. Rationale:

- Users write values and rarely need the verbose type name.
- Types still appear in compilation errors and debuggers.

!!! important

    To improve error readability and make correlation with user code easier, the library
    adopts the idiom of reusing the same identifier for a type and its instance.

    To prevent issues in compile-time logic all entities are `final`. This avoids users
    deriving from them and preserves simplification of equivalent symbolic expressions.

## Strong types instead of aliases

Looking again at those unit definitions: each entity has a short, readable identifier that
derives from a verbose class template instantiation. This greatly improves debugging and
error analysis.

!!! note

    Such brevity is rare. Other units libraries often generate enormous error messages
    where even the first line does not fit on a presentation slide.


## Entities composability

Many libraries assign strong types to every entity (e.g., each derived unit).
`metre_per_second` may not look alarming, but units of angular momentum would yield a
`kilogram_metre_sq_per_second` style type. How many scaled versions would you predefine?
What is the maintenance and standardization cost?

Therefore **mp-units** emphasizes composability. To create a speed quantity you can write:

```cpp
quantity<si::metre / si::second> q;
```

If that unit recurs often you can introduce a helper:

```cpp
constexpr auto metre_per_second = si::metre / si::second;
quantity<metre_per_second> q;
```

or choose any shorter identifier of our choice.

For angular momentum, composability lets a user write:

```cpp
using namespace mp_units::si::unit_symbols;
auto q = la_vector{1, 2, 3} * isq::angular_momentum[kg * m2 / s];
```

This is terse, clear, and scales: `mg * square(mm) / min` needs no extra predefined types.


## Value-based equations

The library relies on C++20 features that improve user experience. One such improvement is
value-based equations.

Entities act as values and compose. Derived entities are defined using these value-based
equations. This is a huge improvement compared to what we can find in other physical units
libraries or what we have to deal with when we want to write some equations based on
`std::ratio`.

For example, below are a few definitions of the SI derived units showing the power of C++20
extensions to Non-Type Template Parameters, which allow us to directly pass a result of
the value-based [unit equation](../../appendix/glossary.md#unit-equation) to a class template
definition:

```cpp
inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct pascal final : named_unit<"Pa", newton / square(metre)> {} pascal;
inline constexpr struct joule  final : named_unit<"J", newton * metre> {} joule;
```


## Symbolic expressions

Not predefining every derived type can harm error readability in other libraries because
framework-generated types become unwieldy.

**mp-units** improves this by using symbolic expressions consistently to describe results of:

- [dimension equation](../../appendix/glossary.md#dimension-equation) - the result is put into
  the `derived_dimension<>` class template
- [quantity equation](../../appendix/glossary.md#quantity-equation) - the result is put into
  the `derived_quantity_spec<>` class template
- [unit equation](../../appendix/glossary.md#unit-equation) - the result is put into the
  `derived_unit<>` class template

For example, dividing base units inside a quantity definition:

```cpp
quantity<metre / second> q;
```

produces the following type in the debugger:

```text
(gdb) ptype q
type = class mp_units::quantity<mp_units::derived_unit<metre, mp_units::per<second>>(), double> [with Rep = double] {
```

The same identifier appears in compilation errors.

!!! important

    Expression templates are extensively used to improve readability of resulting types.


### Identities

Equations on dimensions, quantities, and units require an identity object for each domain:


| Domain Concept |    Identity     |
|----------------|:---------------:|
| `Dimension`    | `dimension_one` |
| `QuantitySpec` | `dimensionless` |
| `Unit`         |      `one`      |

You can explicitly refer to an identity object:

```cpp
constexpr auto my_unit = one / second;
```

!!! note

    Another way to achieve the same result is to call an `inverse()` function:

    ```cpp
    constexpr auto my_unit = inverse(second);
    ```

    Both cases will result in the same symbolic expression being generated and put into the wrapper
    class template.


### Supported operations and their results

Only a few operations exist; each maps to a unique representation:

|                   Operation                   | Resulting template expression arguments |
|:---------------------------------------------:|:---------------------------------------:|
|                    `A * B`                    |                 `A, B`                  |
|                    `B * A`                    |                 `A, B`                  |
|                    `A * A`                    |              `power<A, 2>`              |
|               `{identity} * A`                |                   `A`                   |
|               `A * {identity}`                |                   `A`                   |
|                    `A / B`                    |               `A, per<B>`               |
|                    `A / A`                    |              `{identity}`               |
|               `A / {identity}`                |                   `A`                   |
|               `{identity} / A`                |          `{identity}, per<A>`           |
|                  `pow<2>(A)`                  |              `power<A, 2>`              |
|             `pow<2>({identity})`              |              `{identity}`               |
|          `sqrt(A)` or `pow<1, 2>(A)`          |            `power<A, 1, 2>`             |
| `sqrt({identity})` or `pow<1, 2>({identity})` |              `{identity}`               |


### Simplifying the resulting symbolic expressions

To keep generated types short and readable, the library applies several simplification rules.

1. **Ordering**

    The resulting comma-separated arguments of multiplication are always sorted according to
    a specific predicate. This is why:

    ```cpp
    static_assert(A * B == B * A);
    static_assert(std::is_same_v<decltype(A * B), decltype(B * A)>);
    ```

    This is probably the most important step: it enables comparing types and the rest of
    the simplification rules.

2. **Aggregation**

    Two identical adjacent identifiers aggregate into one entry:

    |              Before              |      After       |
    |:--------------------------------:|:----------------:|
    |              `A, A`              |  `power<A, 2>`   |
    |         `A, power<A, 2>`         |  `power<A, 3>`   |
    |  `power<A, 1, 2>, power<A, 2>`   | `power<A, 5, 2>` |
    | `power<A, 1, 2>, power<A, 1, 2>` |       `A`        |

3. **Simplification**

    In case two of the same identifiers are found in the numerator and denominator
    argument lists; they are being simplified into one entry:

    |        Before         |        After         |
    |:---------------------:|:--------------------:|
    |      `A, per<A>`      |     `{identity}`     |
    | `power<A, 2>, per<A>` |         `A`          |
    | `power<A, 3>, per<A>` |    `power<A, 2>`     |
    | `A, per<power<A, 2>>` | `{identity}, per<A>` |

    It is important to notice here that only the elements with exactly the same type are being
    simplified. This means that, for example, `m/m` results in `one`, but `km/m` will not be
    simplified. The resulting derived unit will preserve both symbols and their relative
    magnitude. This allows us to properly print symbols of some units or constants that require
    such behavior. For example, the Hubble constant is expressed in `km⋅s⁻¹⋅Mpc⁻¹`, where both
    `km` and `Mpc` are units of _length_.

    Also, to prevent possible issues in compile-time logic, all of the library's entities
    must be marked `final`. This prevents the users to derive own strong types from them,
    which would prevent symbolic expression simplification of equivalent entities.

4. **Repacking**

    In case an expression uses two results of other operations, the components of its
    arguments are repacked into one resulting type and simplified there.

    For example, assuming:

    ```cpp
    constexpr auto X = A / B;
    ```

    then:

    | Operation | Resulting template expression arguments |
    |:---------:|:---------------------------------------:|
    |  `X * B`  |                   `A`                   |
    |  `X * A`  |          `power<A, 2>, per<B>`          |
    |  `X * X`  |     `power<A, 2>, per<power<B, 2>>`     |
    |  `X / X`  |              `{identity}`               |
    |  `X / A`  |          `{identity}, per<B>`           |
    |  `X / B`  |          `A, per<power<B, 2>>`          |


## Example

Thanks to all of the features described above, a user may write the code like this one:

```cpp
using namespace mp_units::si::unit_symbols;
quantity speed = 60. * isq::speed[km / h];
quantity duration = 8 * s;
quantity acceleration = speed / duration;
std::cout << "acceleration: " << acceleration << " (" << acceleration.in(m / s2) << ")\n";
```

The `acceleration` quantity, being the result of the above code, has the following type
(after stripping the `mp_units` namespace for brevity):

```text
quantity<reference<derived_quantity_spec<isq::speed, per<isq::time>>{}, derived_unit<si::kilo_<si::metre{}>, per<non_si::hour, si::second>>{}>{}, int>
```

and the text output presents:

```text
acceleration: 7.5 km h⁻¹ s⁻¹ (2.08333 m/s²)
```
