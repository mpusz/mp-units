# Type Introspection

The **mp-units** library provides several class templates that expose public static members
to query their properties at compile-time. These members allow users to inspect the metadata
of quantities, units, and related types.

!!! note

    Public members with the `_XXX_` naming convention (where `XXX` is surrounded by underscores)
    are implementation details and should not be used in user code.

!!! info

    For obtaining textual representations of units and dimensions, see the
    [Text Output](text_output.md) chapter.


## `dimension`

Base dimensions created with `base_dimension` do not expose public static members.


## `quantity_spec`

The `quantity_spec` class template and its specializations provide the following public
static members:

| Member      | Type                 | Description                                            |
|-------------|----------------------|--------------------------------------------------------|
| `dimension` | `Dimension`          | The dimension of the quantity specification            |
| `character` | `quantity_character` | The character of the quantity (scalar, vector, tensor) |

!!! example

    ```cpp
    static_assert(isq::length.dimension == isq::dim_length);
    static_assert(isq::length.character == quantity_character::real_scalar);

    static_assert(isq::velocity.dimension == isq::dim_length / isq::dim_time);
    static_assert(isq::velocity.character == quantity_character::vector);
    ```


## `unit` and `Reference`

Units, including specializations of `named_unit`, `prefixed_unit`, and `derived_unit`,
are fundamental building blocks in **mp-units**. They satisfy the `Reference` concept,
which is a key abstraction in the library.

### The `Reference` Concept

The `Reference` concept is satisfied by two different types:

1. **Units** - such as `si::metre`, `si::second`, or derived units like `si::metre / si::second`
2. **`reference` class template** - explicitly combining a quantity specification with a unit,
   such as `isq::length[si::metre]`

Because the `Reference` concept is satisfied by different types with different internal
structures, neither units nor explicit references expose public static data members for
metadata access. Instead, the library provides non-member functions to extract metadata
from any `Reference`:

- `get_quantity_spec(R)` - extracts the quantity specification
- `get_unit(R)` - extracts the unit

!!! info "Where `Reference` is Used?"

    The `Reference` concept appears as a template parameter in:

    - `quantity<R, Rep>` - where `R` is a `Reference` (either a unit or explicit reference)
    - `quantity_point<R, PO, Rep>` - where `R` is a `Reference`

!!! example

    Extracting metadata from different `Reference` types:

    ```cpp
    // From a unit (satisfies Reference concept)
    static_assert(get_quantity_spec(si::metre) == kind_of<isq::length>);
    static_assert(get_unit(si::metre) == si::metre);

    // From an explicit reference
    constexpr auto ref = isq::length[si::metre];
    static_assert(get_quantity_spec(ref) == isq::length);
    static_assert(get_unit(ref) == si::metre);

    // From a derived unit
    constexpr auto speed_unit = si::metre / si::second;
    static_assert(get_quantity_spec(speed_unit) == kind_of<isq::length / isq::time>);
    static_assert(get_unit(speed_unit) == si::metre / si::second);
    ```


## `quantity`

The `quantity<R, Rep>` class template takes a `Reference` as its first template parameter.
It provides the following public static members:

| Member          | Type           | Description                                                                           |
|-----------------|----------------|---------------------------------------------------------------------------------------|
| `reference`     | `Reference`    | The reference used to instantiate this quantity (can be a unit or explicit reference) |
| `quantity_spec` | `QuantitySpec` | The quantity specification (extracted via `get_quantity_spec(reference)`)             |
| `dimension`     | `Dimension`    | The dimension of the quantity                                                         |
| `unit`          | `Unit`         | The measurement unit (extracted via `get_unit(reference)`)                            |

Additionally, `quantity` provides a public type alias:

- `rep` - the representation type used to store the numerical value

!!! example

    ```cpp
    using distance = quantity<isq::length[si::metre], int>;

    static_assert(distance::reference == isq::length[si::metre]);
    static_assert(distance::quantity_spec == isq::length);
    static_assert(distance::dimension == isq::dim_length);
    static_assert(distance::unit == si::metre);
    static_assert(std::is_same_v<distance::rep, int>);
    ```

    For dimensionless quantities:

    ```cpp
    using ratio = quantity<one>;

    static_assert(ratio::reference == one);
    static_assert(ratio::quantity_spec == kind_of<dimensionless>);
    static_assert(ratio::dimension == dimension_one);
    static_assert(ratio::unit == one);
    static_assert(std::is_same_v<ratio::rep, double>);
    ```


## `quantity_point`

The `quantity_point<R, PO, Rep>` class template takes a `Reference` as its first template
parameter. It provides the following public static members:

| Member                  | Type           | Description                                                                                 |
|-------------------------|----------------|---------------------------------------------------------------------------------------------|
| `reference`             | `Reference`    | The reference used to instantiate this quantity point (can be a unit or explicit reference) |
| `quantity_spec`         | `QuantitySpec` | The quantity specification (extracted via `get_quantity_spec(reference)`)                   |
| `dimension`             | `Dimension`    | The dimension of the quantity point                                                         |
| `unit`                  | `Unit`         | The measurement unit (extracted via `get_unit(reference)`)                                  |
| `point_origin`          | `PointOrigin`  | The origin of this quantity point                                                           |
| `absolute_point_origin` | `PointOrigin`  | The absolute origin in the origin hierarchy                                                 |

Additionally, `quantity_point` provides public type aliases:

- `rep` - the representation type used to store the numerical value
- `quantity_type` - the corresponding quantity type (`quantity<reference, rep>`)

!!! example

    With default (zeroth) origin:

    ```cpp
    using length_point = quantity_point<si::metre>;

    static_assert(length_point::reference == si::metre);
    static_assert(length_point::quantity_spec == kind_of<isq::length>);
    static_assert(length_point::dimension == isq::dim_length);
    static_assert(length_point::unit == si::metre);
    static_assert(std::is_same_v<length_point::rep, double>);
    static_assert(std::is_same_v<length_point::quantity_type, quantity<si::metre, double>>);
    ```

    With explicit origin:

    ```cpp
    inline constexpr struct mean_sea_level final : absolute_point_origin<isq::height> {} mean_sea_level;
    inline constexpr struct ground_level final : relative_point_origin<mean_sea_level + 42 * si::metre> {} ground_level;

    using altitude = quantity_point<isq::height[si::metre], mean_sea_level>;
    using altitude_agl = quantity_point<isq::height[si::metre], ground_level>;

    static_assert(altitude::reference == isq::height[si::metre]);
    static_assert(altitude::quantity_spec == isq::height);
    static_assert(altitude::dimension == isq::dim_length);
    static_assert(altitude::unit == si::metre);
    static_assert(std::is_same_v<altitude::rep, double>);
    static_assert(std::is_same_v<altitude::quantity_type, quantity<isq::height[si::metre]>>);

    static_assert(altitude::point_origin == mean_sea_level);
    static_assert(altitude::absolute_point_origin == mean_sea_level);

    static_assert(altitude_agl::point_origin == ground_level);
    static_assert(altitude_agl::absolute_point_origin == mean_sea_level);
    ```

## Compile-Time Introspection

All these static members can be used in constant expressions, enabling powerful
compile-time introspection and type computations:

```cpp
template<Quantity Q>
constexpr auto get_dimension() { return Q::dimension; }

template<Quantity Q>
constexpr bool is_length_quantity() { return Q::dimension == isq::dim_length; }

static_assert(is_length_quantity<quantity<isq::length[si::metre]>>());
static_assert(!is_length_quantity<quantity<isq::time[si::second]>>());
```

### Advanced Introspection Examples

Checking dimensional relationships between quantities:

```cpp
template<mp_units::Dimension auto... Ds>
constexpr bool are_time_derivatives = false;

template<mp_units::Dimension auto D>
constexpr bool are_time_derivatives<D> = true;

template<mp_units::Dimension auto D1, mp_units::Dimension auto D2, mp_units::Dimension auto... Ds>
constexpr bool are_time_derivatives<D1, D2, Ds...> =
  (D1 / D2 == mp_units::isq::dim_time) && are_time_derivatives<D2, Ds...>;

// Position, velocity, acceleration are time derivatives
static_assert(are_time_derivatives<isq::dim_length, isq::dim_length / isq::dim_time,
                                   isq::dim_length / pow<2>(isq::dim_time)>);
```

!!! example "Real-World Application"

    This pattern of checking time derivatives is particularly useful in Kalman filters,
    where system states often represent quantities and their derivatives (e.g., _position_,
    _velocity_, and _acceleration_). See the [Kalman Filter Examples](../../examples/kalman_filter.md)
    for a complete implementation.

### Accessing Numerical Values Safely

While metadata queries are always safe, accessing the numerical value requires care.
The safest approach is to use `.numerical_value_in(unit)` with an explicit unit:

```cpp
quantity<si::metre> distance = 100 * si::metre;
auto value = distance.numerical_value_in(si::metre);  // Safe: explicit unit
```

In generic code, you can use the quantity's own unit, though this bypasses unit safety
during refactoring:

```cpp
template<Quantity Q>
void process(Q q)
{
  // This will always compile but won't catch unit changes during refactoring
  auto value = q.numerical_value_in(q.unit);
  // ...
}
```

!!! warning

    Using `q.numerical_value_in(q.unit)` is type-safe but bypasses the unit safety
    that would catch accidental unit changes during refactoring. Prefer explicit
    units when possible.
