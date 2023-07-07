# Interface Introduction

## New style of definitions

The **mp-units** library decided to use a rather unusual pattern to define entities.
Here is how we define `metre` and `second` [SI](../../../appendix/glossary/#si) base units:

```cpp
inline constexpr struct metre : named_unit<"m", kind_of<isq::length>> {} metre;
inline constexpr struct second : named_unit<"s", kind_of<isq::time>> {} second;
```

Please note that the above reuses the same identifier for a type and its object. The rationale
behind this is that:

- Users always work with objects and never have to spell such a type name.
- The types appear in the compilation errors and during debugging.

!!! info

    To improve compiler errors' readability and make it easier to correlate them with
    a user's written code, a new idiom in the library is to use the same identifier for
    a type and its instance.


## Strong types instead of aliases

Let's look again at the above units definitions. Another important point to notice is that
all the types describing entities in the library are short, nicely named identifiers
that derive from longer, more verbose class template instantiations. This is really important
to improve the user experience while debugging the program or analyzing the compilation error.

!!! note

    Such a practice is rare in the industry. Some popular C++ physical units libraries
    generate enormously long error messages where even only the first line failed o fit
    on a slide with a tiny font.


## Entities composability

Many physical units libraries (in C++ or any other programming language) assign strong types
to library entities (i.e. derived units). While `metre_per_second` as a type may not look too
scary, consider, for example, units of angular momentum. If we followed this path, its
[coherent unit](../../../appendix/glossary/#coherent-derived-unit) would look like
`kilogram_metre_sq_per_second`. Now, consider how many scaled versions of this unit would you
predefine in the library to ensure that all users are happy with your choice?
How expensive would it be from the implementation point of view?
What about potential future standardization efforts?

This is why in **mp-units**, we put a strong requirement to make everything as composable as
possible. For example, to create a quantity with a unit of speed, one may write:

```cpp
quantity<si::metre / si::second> q;
```

In case you use such an unit often and would prefer to have a handy helper for it, you can
always do something like this:

```cpp
constexpr auto metre_per_second = si::metre / si::second;
quantity<metre_per_second> q;
```

or choose any shorter identifier of your choice.

Coming back to the angular momentum case, thanks to the composability of units, a user can
create such a quantity in the following way:

```cpp
using namespace mp_units::si::unit_symbols;
auto q = la_vector{1, 2, 3} * isq::angular_momentum[kg * m2 / s];
```

It is a much better solution. It is terse and easy to understand. Please also notice how
easy it is to obtain any scaled version of such a unit (i.e. `mg * square(mm) / min`)
without having to introduce hundreds of types to predefine them.


## Value-based equations

The **mp-units** library is based on C++20, which greatly improves a user's experience. One of
such improvements are value-based equations.

As we have learned above, the entities are being used as values in the code, and they compose.
Moreover, derived entities can be defined in the library using such value-based equations.
This is a huge improvement compared to what we can find in other physical units libraries or
what we have to deal with when we want to write some equations for `std::ratio`.

For example, below are a few definitions of the SI derived units showing the power of C++20
extensions to Non-Type Template Parameters, which allows us to directly pass a result of
the value-based [unit equation](../../../appendix/glossary/#unit-equation) to a class template
definition:

```cpp
inline constexpr struct newton : named_unit<"N", kilogram * metre / square(second)> {} newton;
inline constexpr struct pascal : named_unit<"Pa", newton / square(metre)> {} pascal;
inline constexpr struct joule : named_unit<"J", newton * metre> {} joule;
```


## Expression templates

The previous chapter provided a rationale for not having predefined types for derived entities.
In many libraries, such an approach results in long and unreadable compilation errors, as
framework-generated types are typically far from being easy to read and understand.

The **mp-units** library greatly improves the user experience by extensively using
expression templates. Such expressions are used consistently throughout the entire library
to describe the results of:

- [dimension equation](../../../appendix/glossary/#dimension-equation) - the result is put into
  the `derived_dimension<>` class template
- [quantity equation](../../../appendix/glossary/#quantity-equation) - the result is put into
  the `derived_quantity_spec<>` class template
- [unit equation](../../../appendix/glossary/#unit-equation) - the result is put into
  the `derived_unit<>` class template

For example, if we take the above-defined base units and put the results of their division into
the quantity class template like this:

```cpp
quantity<metre / second> q;
```

we will observe the following type in the debugger

```
(gdb) ptype q
type = class mp_units::quantity<mp_units::derived_unit<metre, mp_units::per<second>>(), double> [with Rep = double] {
```

The same type identifier will be visible in the compilation error (in case it happens).

!!! info

    Expressions templates are extensively used throughout the library to improve the readability
    of the resulting types.


### Identities

As mentioned above, equations can be done on dimensions, quantities, and units. Each such domain must
introduce an identity object that can be used in the resulting expressions. Here is the list of
identities used in the library:


| Domain Concept |    Identity     |
|----------------|:---------------:|
| `Dimension`    | `dimension_one` |
| `QuantitySpec` | `dimensionless` |
| `Unit`         |      `one`      |

In the equations, a user can refer to an identity object either explicitly:

```cpp
constexpr auto my_unit = one / second;
```

or implicitly:

```cpp
constexpr auto my_unit = 1 / second;
```

Both cases with result in the same expression template being generated and put into the wrapper
class template.


### Supported operations and their results

There are only a few operations that one can do on such entities and the result of each of them has
its unique representation in the library:

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


### Simplifying the resulting expression templates

To limit the length and improve the readability of generated types, there are many rules to simplify
the resulting expression template.

1. **Ordering**

    The resulting comma-separated arguments of multiplication are always sorted according to
    a specific predicate. This is why:

    ```cpp
    static_assert(A * B == B * A);
    static_assert(std::is_same_v<decltype(A * B), decltype(B * A)>);
    ```

    This is probably the most important of all steps, as it allows comparing types and enables the rest of
    simplification rules.

2. **Aggregation**

    In case two of the same identifiers are found next to each other on the argument list they
    will be aggregated in one entry:

    |              Before              |      After       |
    |:--------------------------------:|:----------------:|
    |              `A, A`              |  `power<A, 2>`   |
    |         `A, power<A, 2>`         |  `power<A, 3>`   |
    |  `power<A, 1, 2>, power<A, 2>`   | `power<A, 5, 2>` |
    | `power<A, 1, 2>, power<A, 1, 2>` |       `A`        |

3. **Simplification**

    In case two of the same identifiers are found in the numerator and denominator argument lists;
    they are being simplified into one entry:

    |        Before         |        After         |
    |:---------------------:|:--------------------:|
    |      `A, per<A>`      |     `{identity}`     |
    | `power<A, 2>, per<A>` |         `A`          |
    | `power<A, 3>, per<A>` |    `power<A, 2>`     |
    | `A, per<power<A, 2>>` | `{identity}, per<A>` |

4. **Repacking**

    In case an expression uses two results of other operations, the components of its arguments are repacked
    into one resulting type and simplified there.

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
auto speed = 60. * isq::speed[km / h];
auto duration = 8 * s;
auto acceleration = speed / duration;
std::cout << "acceleration: " << acceleration << " (" << acceleration[m / s2] << ")\n";
```

The `acceleration`, being the result of the above code, has the following type
(after stripping the `mp_units` namespace for brevity):

```text
quantity<reference<derived_quantity_spec<isq::speed, per<isq::time>>{}, derived_unit<si::kilo_<si::metre{}>, per<non_si::hour, si::second>>{}>{}, int>
```

and the text output presents:

```text
acceleration: 7.5 km h⁻¹ s⁻¹ (2.08333 m/s²)
```
