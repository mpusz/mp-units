# Using Custom Representation Types

## Overview

The **mp-units** library allows you to use any conforming custom type as the representation
type for a quantity. While the library works seamlessly with fundamental arithmetic types
(except `bool`) and `std::complex`, you can provide your own custom representation types
to model domain-specific requirements—such as range-validated values, vectors, or
specialized numeric types.

Creating a quantity with a custom representation type is straightforward: simply multiply
your custom value with a unit or reference:

```cpp
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

my_custom_type value{42};
auto distance = value * m;  // quantity<si::metre, my_custom_type>
```

However, your custom type must satisfy certain constraints defined by the library's
representation concepts. To understand these constraints, we first need to understand
**quantity characters**, which determine what operations are allowed on different types
of quantities.


## Quantity Character

Quantities in physics have different mathematical natures that determine which operations
make sense for them. For detailed information, see
[Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md).

The character is defined at the **quantity specification** level:

- Explicitly in the quantity spec definition (e.g., `displacement` is defined as a vector
  quantity)
- Derived from quantity equations (e.g., `vector_product(v1, v2)` yields a vector,
  `scalar_product(v1, v2)` yields a scalar)
- Defaults to `real_scalar` if not specified

Common character requirements:

- **Real scalar types** (like `int`, `double`) satisfy `real_scalar` character requirements
- **Complex types** (like `std::complex<double>`) satisfy `complex_scalar` character requirements
- **Vector types** (like `cartesian_vector<double>`) satisfy `vector` character requirements

A single representation type may satisfy multiple character requirements. For example, `int`
and `double` satisfy both `real_scalar` and `vector` requirements (as 1-dimensional vectors),
so they work with both scalar and vector quantity specifications.

When you use a **typed quantity** (with a specific quantity spec like `isq::height`),
the library checks whether your representation type satisfies the character requirements:

```cpp
using vec = cartesian_vector<double>;

quantity q1 = 42 * m;                                  // ✅ OK: quantity kind, no character checking
quantity q2 = vec{1., 2., 3.} * m;                     // ✅ OK: quantity kind, no character checking

quantity q3 = 42 * isq::height[m];                     // ✅ OK: height is scalar, int satisfies scalar requirements
quantity q4 = isq::height(42 * m);                     // ✅ OK: same as above
quantity q5 = 42 * isq::displacement[m];               // ✅ OK: displacement is vector, int can represent 1-D vectors
quantity q6 = isq::displacement(42 * m);               // ✅ OK: same as above
quantity q7 = vec{1., 2., 3.} * isq::displacement[m];  // ✅ OK: displacement is vector, vec satisfies vector requirements
quantity q8 = isq::displacement(vec{1., 2., 3.} * m);  // ✅ OK: same as above

// quantity q9 = vec{1., 2., 3.} * isq::height[m];     // ❌ Error: height is scalar,
                                                       //           vec doesn't satisfy scalar requirements
```


## Representation Requirements

To be used as a representation type in **mp-units**, a type must satisfy the
[`RepresentationOf`](../../users_guide/framework_basics/concepts.md#RepresentationOf) concept.
The library supports different types of representations corresponding to different
quantity characters.

The following table summarizes the requirements for different representation characters:

| Requirement                                |          Real Scalar           |                        Complex Scalar                         |                   Vector                    |
|--------------------------------------------|:------------------------------:|:-------------------------------------------------------------:|:-------------------------------------------:|
| Copyable                                   |               ✅                |                               ✅                               |                      ✅                      |
| Addition/subtraction (`+`, `-`, unary `-`) |               ✅                |                               ✅                               |                      ✅                      |
| Scalable (multiply/divide by scalar)       |               ✅                |                               ✅                               |                      ✅                      |
| Self-scalable (`T * T`, `T / T`)           |               ✅                |                               ✅                               |                      -                      |
| Equality comparable (`==`)                 |               ✅                |                               ✅                               |                      ✅                      |
| Totally ordered (`<`, `>`, `<=`, `>=`)     |               ✅                |                               -                               |                      -                      |
| Not a quantity type itself                 |               ✅                |                               ✅                               |                      ✅                      |
| **Construction**                           |               -                |                        `T{real, imag}`                        |                      -                      |
| **Required CPOs**                          |               -                | `mp_units::real()`, `mp_units::imag()`, `mp_units::modulus()` |           `mp_units::magnitude()`           |
| **Opt-out mechanism**                      |       `disable_real<T>`        |                               -                               |                      -                      |
| **Examples**                               | `int`, `double`, `long double` |                    `std::complex<double>`                     | `cartesian_vector<double>`, `int`, `double` |

!!! note "Scaling Factor Type"

    All types must be scalable with a scaling factor type, typically their underlying value type or
    a standard numeric type (like `long double` or `std::intmax_t`).

!!! note "Weakly Regular Types"

    All representation types must be **weakly regular**, which means they satisfy the
    `std::regular` concept except for the default-constructibility requirement. Specifically,
    they must be:

    - **Copyable** (`std::copyable`)
    - **Equality comparable** (`std::equality_comparable`)

    This ensures that representation types have value semantics suitable for use in quantities.
    Default construction is not required, allowing types like range-validated representations
    that may not have a meaningful default value.

!!! note "Complex Types Construction and Total Ordering"

    **Construction**

    Complex scalars **must** be constructible from real and imaginary parts: `T{real_value, imag_value}`.
    This requirement is essential for operations that combine real-valued quantities into complex results.
    For example, combining _active power_ and _reactive power_ into _complex power_:

    ```cpp
    quantity active = isq::active_power(100.0 * W);
    quantity reactive = isq::isq::reactive_power(50.0 * W);
    // Library needs to construct: std::complex<double>{active.numerical_value(), reactive.numerical_value()}
    ```

    **Total Ordering**

    The library assumes that well-designed complex-like types do not provide total ordering
    (`operator<`, etc.) since there is no natural ordering for complex numbers. If you have
    a complex-like type that does provide ordering operators (e.g., lexicographical comparison
    for use in containers), use the `disable_real` opt-out mechanism:

    ```cpp
    template<>
    constexpr bool mp_units::disable_real<my_complex_type> = true;
    ```

    Alternatively, the library could explicitly check for the absence of `mp_units::real()` and
    `mp_units::imag()` operations to distinguish real from complex scalars. This is a design
    choice that may be refined based on standardization discussions.

### Tensor Representations

Tensor representations are planned for future support but are not yet fully implemented.


## Customization Points

The library provides several customization mechanisms for representation types. These are
the tools you can use to adapt your custom types to work with **mp-units**.

The customization points fall into two categories: **Character determination** (CPOs and opt-out
mechanisms that determine what kind of representation type you have) and **Behavior and values**
(customization points that control library behavior and provide special values).

### Character Determination

#### Customization Point Objects (CPOs)

The library uses several CPOs to support different representation types. Providing these CPOs
determines the **character** of your representation type. Each CPO checks for implementations
in the following priority order:

**`mp_units::real(c)`** - Returns the real part of a complex number:

1. `c.real()` member function
2. `real(c)` free function found via ADL

**`mp_units::imag(c)`** - Returns the imaginary part of a complex number:

1. `c.imag()` member function
2. `imag(c)` free function found via ADL

**`mp_units::modulus(c)`** - Returns the magnitude of a complex number:

1. `c.modulus()` member function
2. `modulus(c)` free function found via ADL
3. `c.abs()` member function
4. `abs(c)` free function found via ADL

**`mp_units::magnitude(v)`** - Returns the magnitude of a vector as a scalar:

1. `v.magnitude()` member function
2. `magnitude(v)` free function found via ADL
3. For arithmetic types: `std::abs(v)`
4. For real scalar types: `v.abs()` member function
5. For real scalar types: `abs(v)` free function found via ADL

!!! note "Why `abs()` is also checked?"

    **Complex Scalars (`modulus()` CPO)**

    Provides compatibility with `std::complex` and similar types that use `abs()` as the
    function name for returning the modulus value. This is checked as a fallback if
    `modulus()` is not provided.

    **Vectors (`magnitude()` CPO)**

    Allows real scalar types (like `int`, `double`) to represent 1-dimensional vectors,
    which is very common in engineering. This enables seamless use of arithmetic types
    for both scalar and vector quantities.

---

#### `disable_real<T>`

A specializable variable template to opt out a type from being treated as a real scalar:

```cpp
template<typename T>
constexpr bool mp_units::disable_real = false;
```

**Purpose:** Controls the **character** of your representation type by preventing it from
being classified as a real scalar, even if it satisfies the real scalar requirements
(copyable, totally ordered, supports arithmetic operations).

**When to specialize:** If your type accidentally satisfies the real scalar requirements
but shouldn't be used as one:

```cpp
template<>
constexpr bool mp_units::disable_real<my_type> = true;
```

**Example use case:** The library uses this internally to prevent `bool` from being used
as a scalar representation, even though `bool` is technically totally ordered and supports
arithmetic operations.

---

### Behavior and Values

#### `value_type` or `element_type`

The library uses `value_type_t<T>` to determine the underlying arithmetic type of your
representation, which is used for:

- Determining the scaling factor type (what type to multiply/divide your type by)
- Checking if the type should be treated as floating-point

How it works:

1. If your type has a `value_type` or `element_type` member type (checked via `std::indirectly_readable_traits`),
   the library recursively unwraps it until it finds the underlying type
2. Otherwise, your type itself is used as the value type

**Recommendation:** Provide a `value_type` member type for wrapper types:

```cpp
template<typename T>
class my_wrapper {
public:
  using value_type = T;  // Exposes the underlying type
  // ...
};
```

!!! warning "Don't provide both `value_type` and `element_type`"

    If your type provides both `value_type` and `element_type` that refer to **different types**,
    `std::indirectly_readable_traits<T>::value_type` will be ill-formed (undefined), and your type
    won't work with the library. If both exist and refer to the **same type**, that type will be used.

    **Recommendation:** Provide only `value_type` unless you have a specific reason to provide both
    (e.g., satisfying iterator concepts), in which case ensure they refer to the same type.

---

#### `treat_as_floating_point<Rep>`

A specializable variable template that tells the library whether a type should be treated as
floating-point for the purpose of allowing implicit conversions:

```cpp
template<typename Rep>
constexpr bool mp_units::treat_as_floating_point = /* implementation-defined */;
```

**Default behavior:**

- In hosted environments: uses `std::chrono::treat_as_floating_point_v<value_type_t<Rep>>`
- In freestanding: uses `std::is_floating_point_v<value_type_t<Rep>>`

**When to specialize:** If you have a custom type that wraps a floating-point value but the
automatic detection doesn't work correctly:

```cpp
template<>
constexpr bool mp_units::treat_as_floating_point<my_fixed_point_type> = true;
```

**Impact:** When `treat_as_floating_point<Rep>` is `true`, the type is treated as floating-point
for conversion purposes. See [Value Conversions](../../users_guide/framework_basics/value_conversions.md#value-conversions)
for details on how this affects implicit conversions between quantities

---

#### `is_value_preserving<From, To>`

A specializable variable template that determines whether a conversion from one representation
type to another preserves values:

```cpp
template<typename From, typename To>
constexpr bool mp_units::is_value_preserving =
  treat_as_floating_point<To> || !treat_as_floating_point<From>;
```

**Default behavior:** A conversion is value-preserving if:

- The destination type is floating-point (can represent any source value), OR
- The source type is not floating-point (integer → integer or integer → float is safe)

This follows the same practice as `std::chrono::duration` conversions.

**When to specialize:** If you have custom types with specific value preservation semantics:

```cpp
// Example: my_decimal has more precision than double
template<>
constexpr bool mp_units::is_value_preserving<my_decimal, double> = false;

template<>
constexpr bool mp_units::is_value_preserving<double, my_decimal> = true;
```

**Impact:** Controls whether conversions are implicit or require explicit casts.

---

#### `representation_values<Rep>`

A specializable class template that provides special values for a representation type:

```cpp
template<typename Rep>
struct mp_units::representation_values {
  static constexpr Rep zero() noexcept;  // Required for quantity::zero()
  static constexpr Rep one() noexcept;   // Required for quantity operations
  static constexpr Rep min() noexcept;   // Required for quantity::min()
  static constexpr Rep max() noexcept;   // Required for quantity::max()
};
```

**Default behavior:**

- In hosted environments: inherits from `std::chrono::duration_values<Rep>`
- `zero()`: returns `Rep(0)` if Rep is constructible from `int`
- `one()`: returns `Rep(1)` if Rep is constructible from `int`
- `min()`: returns `std::numeric_limits<Rep>::lowest()` if available
- `max()`: returns `std::numeric_limits<Rep>::max()` if available

**When to specialize:** If your type needs custom special values:

```cpp
template<typename T>
struct mp_units::representation_values<my_custom_type<T>> {
  static constexpr my_custom_type<T> zero() noexcept {
    return my_custom_type<T>{T{0}};
  }

  static constexpr my_custom_type<T> one() noexcept {
    return my_custom_type<T>{T{1}};
  }

  static constexpr my_custom_type<T> min() noexcept {
    return my_custom_type<T>{std::numeric_limits<T>::lowest()};
  }

  static constexpr my_custom_type<T> max() noexcept {
    return my_custom_type<T>{std::numeric_limits<T>::max()};
  }
};
```

**Usage:** These values are used by:

- `quantity::zero()`, `quantity::min()`, `quantity::max()` static member functions
- Mathematical operations like `floor()`, `ceil()`, `round()`
- Division by zero checks


## Built-in Support

All fundamental arithmetic types except `bool` satisfy the real scalar and 1-D vector
representation requirements:

```cpp
quantity<si::metre, int> length1 = 42 * m;
quantity<si::metre, double> length2 = 3.14 * m;
quantity<si::metre, long double> length3 = 2.718L * m;
```

!!! note "Why is `bool` excluded?"

    Although `bool` technically satisfies the syntactic requirements (it's copyable, totally
    ordered, and supports arithmetic operations), it's excluded via `disable_real<bool> = true`
    because using boolean values as physical quantities rarely makes sense and can lead to
    confusing code.

The library also supports `std::complex` for complex-valued quantities:

```cpp
#include <complex>

std::complex<double> impedance{50.0, 30.0};
quantity z = impedance * si::ohm;  // Complex impedance
```

## Examples

??? example "Range-Validated Representation"

    The library examples include a `ranged_representation` type that ensures values stay within
    specified bounds. This is useful for quantities with physical or logical constraints:

    ```cpp
    template<std::movable T, auto Min, auto Max>
    class ranged_representation {
      T value_;
    public:
      constexpr ranged_representation(T v) : value_(std::clamp(v, T{Min}, T{Max})) {}

      constexpr T value() const { return value_; }
      constexpr operator T() const { return value_; }  // Conversion operator to underlying type
      constexpr ranged_representation operator-() const { return ranged_representation(-value_); }
      // ... other required operations
    };
    ```

    This is used in the [glide computer example](../../examples/glide_computer.md#geographic-integration)
    for _latitude_ and _longitude_:

    ```cpp
    #include <mp-units/systems/si.h>

    using namespace mp_units;

    template<typename T = double>
    using latitude = quantity_point<si::degree, equator, ranged_representation<T, -90, 90>>;

    template<typename T = double>
    using longitude = quantity_point<si::degree, prime_meridian, ranged_representation<T, -180, 180>>;

    void example()
    {
      latitude lat{45.0 * deg};    // Valid: within [-90, 90]
      longitude lon{120.0 * deg};  // Valid: within [-180, 180]
    }
    ```

    The range validation happens at construction time, ensuring coordinates are always valid.

??? example "Vector Representation"

    The library provides [`cartesian_vector`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h)
    as a vector representation type with full support for vector operations:

    ```cpp
    #include <mp-units/cartesian_vector.h>
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/isq.h>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    void example()
    {
      // Create 3D vector quantities
      quantity_point pos1{cartesian_vector{1., 2., 3.} * m};
      quantity_point pos2{cartesian_vector{4., 5., 6.} * m};

      // Vector subtraction
      quantity delta = pos2 - pos1;  // {3, 3, 3} m

      // Scalar multiplication
      quantity delta_scaled = 2 * delta;  // {6, 6, 6} m

      // Magnitude (returns scalar quantity)
      quantity distance = pos1.quantity_from_zero();
      quantity mag = magnitude(distance);  // sqrt(1² + 2² + 3²) m

      // Scalar product (dot product)
      auto dot = scalar_product(pos1, pos2);  // Returns quantity with dimension m²

      // Vector product (cross product)
      auto cross = vector_product(pos1, pos2);  // Returns vector quantity with dimension m²
    }
    ```

    The `cartesian_vector` implementation demonstrates how to create a full-featured vector type:

    - Implements arithmetic operations (`+`, `-`, `*`, `/`)
    - Provides `magnitude()` member function
    - Supports scalar and vector products
    - Works with quantity characters to distinguish between scalars and vectors

??? example "Creating Your Own Representation Type"

    To create a custom representation type, follow these steps:

    ### 1. Define Your Type

    Create a class with the value semantics and operations you need:

    ```cpp
    template<typename T>
    class my_custom_type {
      T value_;
    public:
      using value_type = T;

      constexpr explicit my_custom_type(T v) : value_(v) {}
      constexpr T value() const { return value_; }

      // Required: Arithmetic operations
      constexpr my_custom_type operator-() const { return my_custom_type{-value_}; }

      friend constexpr my_custom_type operator+(const my_custom_type& lhs, const my_custom_type& rhs)
      {
        return my_custom_type{lhs.value_ + rhs.value_};
      }

      friend constexpr my_custom_type operator-(const my_custom_type& lhs, const my_custom_type& rhs)
      {
        return my_custom_type{lhs.value_ - rhs.value_};
      }

      // Required: Scaling operations
      friend constexpr my_custom_type operator*(const my_custom_type& v, T factor)
      {
        return my_custom_type{v.value_ * factor};
      }

      friend constexpr my_custom_type operator*(T factor, const my_custom_type& v)
      {
        return my_custom_type{factor * v.value_};
      }

      friend constexpr my_custom_type operator/(const my_custom_type& v, T factor)
      {
        return my_custom_type{v.value_ / factor};
      }

      // Required for scalar types: Self-scaling operations
      friend constexpr my_custom_type operator*(const my_custom_type& lhs, const my_custom_type& rhs)
      {
        return my_custom_type{lhs.value_ * rhs.value_};
      }

      friend constexpr my_custom_type operator/(const my_custom_type& lhs, const my_custom_type& rhs)
      {
        return my_custom_type{lhs.value_ / rhs.value_};
      }

      // Required: Equality comparison
      constexpr bool operator==(const my_custom_type&) const = default;

      // Required for real scalar types: Comparison
      constexpr auto operator<=>(const my_custom_type&) const = default;
    };
    ```

    ### 2. Provide Required Customization Points (if needed)

    For complex scalars, provide the required customization points. You can do this via member functions:

    ```cpp
    template<typename T>
    class my_complex_type {
      T real_, imag_;
    public:
      constexpr T real() const { return real_; }
      constexpr T imag() const { return imag_; }
      constexpr T modulus() const { return std::hypot(real_, imag_); }
      // ... other required operations
    };
    ```

    Or via free functions found through ADL (Argument-Dependent Lookup):

    ```cpp
    template<typename T>
    constexpr T real(const my_complex_type<T>& c) { return c.get_real(); }

    template<typename T>
    constexpr T imag(const my_complex_type<T>& c) { return c.get_imag(); }

    template<typename T>
    constexpr T modulus(const my_complex_type<T>& c) { return c.get_magnitude(); }
    // Note: You can also provide abs() instead of modulus()
    ```

    For vectors, provide the `magnitude()` customization point:

    ```cpp
    template<typename T>
    class my_vector_type {
      // ... implementation
    public:
      constexpr T magnitude() const { /* compute magnitude */ }
      // ... other required operations
    };
    ```

    Or via a free function:

    ```cpp
    template<typename T>
    constexpr T magnitude(const my_vector_type<T>& v) { return v.compute_magnitude(); }
    // Note: You can also provide abs() instead of magnitude().
    //       abs() is also checked to allow real scalar types to represent 1-dimensional vectors.
    ```

    ### 3. Optional: Provide Formatting Support (if needed)

    To enable formatting with `std::format`, specialize the formatter:

    ```cpp
    template<typename T, typename Char>
    struct std::formatter<my_custom_type<T>, Char> : std::formatter<T, Char> {
      template<typename FormatContext>
      auto format(const my_custom_type<T>& v, FormatContext& ctx) const { return std::formatter<T, Char>::format(v.value(), ctx); }
    };
    ```

    ### 4. Optional: Specialize `representation_values` (if needed)

    If your type needs custom implementations for zero, one, minimum, or maximum values,
    specialize the `representation_values` trait. See the
    [`representation_values<Rep>` customization point](#representation_valuesrep) for complete details.

    Example:

    ```cpp
    template<typename T>
    struct mp_units::representation_values<my_custom_type<T>> {
      static constexpr my_custom_type<T> zero() noexcept { return my_custom_type<T>{T{0}}; }
      static constexpr my_custom_type<T> one() noexcept { return my_custom_type<T>{T{1}}; }
      static constexpr my_custom_type<T> min() noexcept { return my_custom_type<T>{std::numeric_limits<T>::lowest()}; }
      static constexpr my_custom_type<T> max() noexcept { return my_custom_type<T>{std::numeric_limits<T>::max()}; }
    };
    ```

    ### 5. Use It with Quantities

    Once your type satisfies the required concepts, use it naturally with units:

    ```cpp
    my_custom_type value{42.0};
    auto length = value * m;  // quantity<si::metre, my_custom_type<double>>

    auto area = length * length;  // Quantities compose naturally
    ```

    !!! tip "Validation with `static_assert`"

        Use `static_assert` to verify your type satisfies the expected concepts:

        ```cpp
        static_assert(RepresentationOf<my_custom_type<double>, quantity_character::real_scalar>);
        static_assert(treat_as_floating_point<my_custom_type<double>>);
        static_assert(!treat_as_floating_point<my_custom_type<int>>);
        ```

        This provides clear compile-time feedback if something is missing.


## Common Pitfalls

### Scaling Factor Type

The library needs to scale your type by a numeric factor during unit conversions. Make
sure your type provides a `value_type` member to help the library determine the correct
scaling factor type. See [`value_type` or `element_type`](#value_type-or-element_type)
for complete details.

```cpp
template<typename T>
class my_type {
public:
  using value_type = T;  // Helps the library determine the scaling factor type
  // ...
};
```

### ADL and Namespace Placement

When providing free functions for customization points (like `real()`, `imag()`,
`magnitude()`), place them in the same namespace as your type to ensure Argument-Dependent
Lookup (ADL) finds them:

```cpp
namespace my_namespace {

  template<typename T>
  class my_complex {
    // ...
  };

  // ✅ Good: in the same namespace, found by ADL
  template<typename T>
  constexpr T real(const my_complex<T>& c) { return c.get_real(); }

}  // namespace my_namespace
```


## Summary

**mp-units** provides a flexible framework for custom representation types:

- **Use any type** that satisfies the representation concepts
- **Built-in support** for fundamental types and `std::complex`
- **Examples provided** for range-validated types, vectors, and more
- **Character system** ensures type-safe operations based on physical meaning
- **Simple integration**: just multiply your value with a unit

By providing custom representation types, you can encode domain-specific constraints,
use specialized numeric types, or work with mathematical objects like vectors and
complex numbers—all while maintaining the strong type safety and dimensional analysis
that **mp-units** provides.


## See Also

<!-- markdownlint-disable MD013 -->
**Related Documentation:**

- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) - Understanding quantity characters
- [Value Conversions](../../users_guide/framework_basics/value_conversions.md) - How `treat_as_floating_point` affects conversions
- [Concepts](../../users_guide/framework_basics/concepts.md#RepresentationOf) - The `RepresentationOf` concept definition

**Implementation References:**

- [`representation_concepts.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/representation_concepts.h) - Concept definitions
- [`customization_points.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/customization_points.h) - CPO implementations
- [`cartesian_vector.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h) - Vector implementation example
- [`ranged_representation.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/example/include/ranged_representation.h) - Range-validated representation example
<!-- markdownlint-enable MD013 -->
