# Using Custom Representation Types

This guide shows you how to create and integrate your own custom representation types
with **mp-units**. You'll learn the steps needed to make your type work seamlessly with
the library's quantity system.

For background on representation type design and requirements, see the
[Representation Types](../../users_guide/framework_basics/representation_types.md)
section in the User's Guide.

## Quick Start

Creating a quantity with a custom representation type is straightforward:

```cpp
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

my_custom_type value{42};
auto distance = value * m;  // quantity<si::metre, my_custom_type>
```

Your custom type must satisfy the library's
[`RepresentationOf`](../../users_guide/framework_basics/concepts.md#RepresentationOf) concept,
which verifies your type provides the operations needed for its intended
[quantity character](../../users_guide/framework_basics/character_of_a_quantity.md).

## Creating Your Own Representation Type

Follow these steps to create a custom representation type that works with **mp-units**.

### Step 1: Define Your Type with Required Operations

Create a class with value semantics and the operations your character needs. Here's a template
for a **real scalar** type:

```cpp
template<typename T>
class my_scalar_type {
  T value_;
public:
  using value_type = T;  // Helps library determine scaling factor type

  constexpr explicit my_scalar_type(T v) : value_(v) {}
  constexpr T value() const { return value_; }

  // Required: Arithmetic operations
  constexpr my_scalar_type operator-() const { return my_scalar_type{-value_}; }

  friend constexpr my_scalar_type operator+(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ + rhs.value_};
  }

  friend constexpr my_scalar_type operator-(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ - rhs.value_};
  }

  // Required for floating-point T: scaling operations used by the built-in conversion engine.
  // For integer T, implicit conversions to/from value_type_t<T> are used instead (see note below).
  // Either path satisfies MagnitudeScalable; alternatively provide a scaling_traits specialisation.
  friend constexpr my_scalar_type operator*(const my_scalar_type& v, T factor)
  {
    return my_scalar_type{v.value_ * factor};
  }

  friend constexpr my_scalar_type operator*(T factor, const my_scalar_type& v)
  {
    return my_scalar_type{factor * v.value_};
  }

  friend constexpr my_scalar_type operator/(const my_scalar_type& v, T factor)
  {
    return my_scalar_type{v.value_ / factor};
  }

  // Required for scalar types: Self-scaling operations (multiply/divide by same type)
  friend constexpr my_scalar_type operator*(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ * rhs.value_};
  }

  friend constexpr my_scalar_type operator/(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ / rhs.value_};
  }

  // Required: Equality comparison
  constexpr bool operator==(const my_scalar_type&) const = default;

  // Required for real scalar types: Total ordering
  constexpr auto operator<=>(const my_scalar_type&) const = default;
};
```

### Step 2: Provide Character-Specific Customization Points (if needed)

!!! info "CPOs vs Customization Point Functions"

    The library provides **Customization Point Objects (CPOs)** like `mp_units::real`, `mp_units::imag`,
    `mp_units::norm`, etc. You provide **customization point functions** (as member functions or
    ADL-findable free functions) that these CPOs will find and invoke.

For **complex scalars**, provide the required customization point functions via member functions:

```cpp
template<typename T>
class my_complex_type {
  T real_, imag_;
public:
  using value_type = T;

  constexpr my_complex_type(T r, T i) : real_(r), imag_(i) {}

  // Required customization point functions as member functions
  constexpr T real() const { return real_; }
  constexpr T imag() const { return imag_; }
  constexpr T modulus() const { return std::hypot(real_, imag_); }

  // ... other required operations (addition, scaling, equality)
};
```

Or via free functions found through ADL:

```cpp
template<typename T>
constexpr T real(const my_complex_type<T>& c) { return c.get_real(); }

template<typename T>
constexpr T imag(const my_complex_type<T>& c) { return c.get_imag(); }

template<typename T>
constexpr T modulus(const my_complex_type<T>& c) { return c.get_magnitude(); }
// Note: You can also provide abs() instead of modulus()
```

For **vectors**, provide the `norm()` customization point function:

```cpp
template<typename T>
class my_vector_type {
  // ... implementation
public:
  using value_type = T;

  constexpr T norm() const { /* compute magnitude */ }
  // ... other required operations
};
```

Or via a free function:

```cpp
template<typename T>
constexpr T norm(const my_vector_type<T>& v) { return v.compute_norm(); }
```

!!! tip "Use `norm()` for vectors"

    While `magnitude()` is also supported for compatibility, prefer implementing `norm()` to match
    industry standard libraries (Eigen, NumPy, MATLAB, Armadillo).

### Step 3: Add Formatting Support (optional)

Enable formatting with `std::format`:

```cpp
template<typename T, typename Char>
struct std::formatter<my_scalar_type<T>, Char> : std::formatter<T, Char> {
  template<typename FormatContext>
  auto format(const my_scalar_type<T>& v, FormatContext& ctx) const {
    return std::formatter<T, Char>::format(v.value(), ctx);
  }
};
```

### Step 4: Specialize `representation_values` (if needed)

If your type needs custom special values (see the
[`representation_values<Rep>`](../../users_guide/framework_basics/representation_types.md#representation_values)
documentation):

```cpp
template<typename T>
struct mp_units::representation_values<my_scalar_type<T>> {
  static constexpr my_scalar_type<T> zero() noexcept {
    return my_scalar_type<T>{T{0}};
  }
  static constexpr my_scalar_type<T> one() noexcept {
    return my_scalar_type<T>{T{1}};
  }
  static constexpr my_scalar_type<T> min() noexcept {
    return my_scalar_type<T>{std::numeric_limits<T>::lowest()};
  }
  static constexpr my_scalar_type<T> max() noexcept {
    return my_scalar_type<T>{std::numeric_limits<T>::max()};
  }
};
```

### Step 5: Specialize `scaling_traits` (if needed) { #scaling_traits }

The library provides built-in scaling for two broad categories of types (see the
[`scaling_traits`](../../users_guide/framework_basics/representation_types.md#scaling_traits)
reference):

- **Floating-point types**: any type where `treat_as_floating_point` is `true` and
  `value * value_type_t<T>` compiles — no specialization needed.
- **Integer-like types**: types whose `value_type_t` is a non-floating-point integer type
  and that are convertible to/from that type — no specialization needed. The built-in
  implementation uses exact double-width integer arithmetic for rational factors, avoiding
  floating-point precision loss.

You need to specialize `scaling_traits` **only** when scaling must operate on multiple
internal fields simultaneously and the built-in single-value approach doesn't apply. A
common example is a type that carries both a value and an uncertainty:

```cpp
template<typename T, typename U>
struct mp_units::scaling_traits<my_measurement<T>, my_measurement<U>> {
  template<auto M>
  [[nodiscard]] static constexpr my_measurement<U> scale(const my_measurement<T>& v)
  {
    return my_measurement<U>(
      mp_units::scale<U>(M, v.value()),
      mp_units::scale<U>(M, v.uncertainty()));
  }
};
```

### Step 6: Specialize `implicitly_scalable` (if needed) { #implicitly_scalable }

By default, a conversion between two quantity types is implicit only when it is
non-truncating: the target representation is floating-point, or both representations are
integer-like and the unit ratio is an integer multiplier (e.g. `m → mm`). All other
integer-to-integer conversions (fractional ratios such as `mm → m`) are explicit and
require `value_cast` or `force_in`.

If your type has different implicit-conversion semantics, specialize
[`mp_units::implicitly_scalable`](../../users_guide/framework_basics/representation_types.md#implicitly_scalable):

```cpp
// Allow implicit narrowing from double to my_safe_decimal (it can represent any double value)
template<auto FromUnit, auto ToUnit>
constexpr bool mp_units::implicitly_scalable<FromUnit, double, ToUnit, my_safe_decimal> = true;

// Keep double → my_safe_decimal explicit (my_safe_decimal has higher precision)
template<auto FromUnit, auto ToUnit>
constexpr bool mp_units::implicitly_scalable<FromUnit, my_safe_decimal, ToUnit, double> = false;
```

You can use `mp_units::is_integral_scaling(from, to)` in your specialization to reuse the
library's "is the ratio an integer?" predicate.

### Step 7: Use It with Quantities

```cpp
my_scalar_type value{42.0};
auto length = value * m;  // quantity<si::metre, my_scalar_type<double>>

auto area = length * length;  // Quantities compose naturally
```

!!! tip "Validate with `static_assert`"

    Verify your type satisfies the expected concepts:

    ```cpp
    static_assert(RepresentationOf<my_scalar_type<double>, quantity_character::real_scalar>);
    static_assert(treat_as_floating_point<my_scalar_type<double>>);
    ```


## Practical Examples

### Range-Validated Representation

The library examples include a `ranged_representation` type that ensures values stay within
specified bounds:

```cpp
template<std::movable T, auto Min, auto Max>
class ranged_representation {
  T value_;
public:
  constexpr ranged_representation(T v) : value_(std::clamp(v, T{Min}, T{Max})) {}

  constexpr T value() const { return value_; }
  constexpr operator T() const { return value_; }  // Conversion to underlying type
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

**Implementation reference:**
[`ranged_representation.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/example/include/ranged_representation.h)

### Vector Representation

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

The `cartesian_vector` implementation demonstrates how to create a full-featured vector type with:

- Arithmetic operations (`+`, `-`, `*`, `/`)
- `norm()` member function (and `magnitude()` alias)
- Support for scalar and vector products
- Integration with quantity characters

**Implementation reference:**
[`cartesian_vector.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h)


## Common Pitfalls

### Provide `value_type` for Wrapper Types

The library needs to scale your type by a numeric factor during unit conversions. Make
sure wrapper types provide a `value_type` member to help determine the correct scaling
factor type:

```cpp
template<typename T>
class my_wrapper {
public:
  using value_type = T;  // ✅ Helps library determine scaling factor type
  // ...
};
```

See [`value_type` or `element_type`](../../users_guide/framework_basics/representation_types.md#value_type-or-element_type)
for complete details.

### Place Customization Point Functions in the Same Namespace (ADL)

When providing customization point functions (like `real()`, `imag()`, `norm()`),
place them in the same namespace as your type to ensure Argument-Dependent Lookup (ADL)
finds them:

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


### Assuming Implicit Conversions that Are Actually Explicit

The default `implicitly_scalable` only allows implicit conversion when the unit ratio is a
non-truncating integer multiplier (or the target representation is floating-point). If you
expect implicit conversion between, say, `mm` and `m` with an integer representation, you'll
get a compilation error:

```cpp
quantity<si::millimetre, int> a = 1000 * mm;
quantity<si::metre, int> b = a;        // ❌ explicit conversion required (truncating: 1000 → 1)
quantity<si::metre, int> c = value_cast<si::metre>(a);  // ✅ OK
```

This is intentional — the conversion is truncating. Specialize `mp_units::implicitly_scalable`
only when you are certain your type handles the conversion without data loss.

## Summary

To create a custom representation type:

1. **Implement required operations** for your character (scalar/complex/vector/tensor)
2. **Provide character-specific functions** (if needed): `real()`, `imag()`, `norm()`, etc.,
   via member functions or ADL-findable free functions
3. **Add formatting support** (optional) via `std::formatter`
4. **Add `value_type`** to help the library determine the scaling factor type
5. **Specialize `representation_values<Rep>`** (if needed) for custom special values
6. **Specialize `scaling_traits<From, To>`** (if needed) when scaling must operate on
   multiple internal fields simultaneously (e.g. value + uncertainty types)
7. **Specialize `implicitly_scalable`** (if needed) to control implicit vs. explicit
   conversion semantics
8. **Verify with concepts** using `static_assert`

The library handles the rest, providing strong type safety and dimensional analysis for your
custom types.


## See Also

<!-- markdownlint-disable MD013 -->
**User's Guide:**

- [Representation Types](../../users_guide/framework_basics/representation_types.md) - Complete design and requirements reference
- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) - Understanding quantity characters
- [Value Conversions](../../users_guide/framework_basics/value_conversions.md) - How `treat_as_floating_point` and `implicitly_scalable` affect conversions
- [Concepts](../../users_guide/framework_basics/concepts.md#RepresentationOf) - The `RepresentationOf` concept definition

**Implementation References:**

- [`representation_concepts.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/representation_concepts.h) - Concept definitions
- [`scaling.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/scaling.h) - `scaling_traits` and built-in scaling implementation
- [`value_cast.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/value_cast.h) - `implicitly_scalable` and `is_integral_scaling`
- [`customization_points.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/framework/customization_points.h) - CPO implementations
- [`cartesian_vector.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h) - Vector implementation example
- [`ranged_representation.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/example/include/ranged_representation.h) - Range-validated representation example
<!-- markdownlint-enable MD013 -->
