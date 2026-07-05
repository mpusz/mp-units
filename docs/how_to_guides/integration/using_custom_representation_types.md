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
  [[nodiscard]] constexpr T value() const { return value_; }

  // Required: Arithmetic operations
  [[nodiscard]] constexpr my_scalar_type operator-() const { return my_scalar_type{-value_}; }

  [[nodiscard]] friend constexpr my_scalar_type operator+(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ + rhs.value_};
  }

  [[nodiscard]] friend constexpr my_scalar_type operator-(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ - rhs.value_};
  }

  [[nodiscard]] friend constexpr my_scalar_type operator*(const my_scalar_type& v, T factor)
  {
    return my_scalar_type{v.value_ * factor};
  }

  [[nodiscard]] friend constexpr my_scalar_type operator*(T factor, const my_scalar_type& v)
  {
    return my_scalar_type{factor * v.value_};
  }

  [[nodiscard]] friend constexpr my_scalar_type operator/(const my_scalar_type& v, T factor)
  {
    return my_scalar_type{v.value_ / factor};
  }

  // Required for scalar types: Self-scaling operations (multiply/divide by same type)
  [[nodiscard]] friend constexpr my_scalar_type operator*(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ * rhs.value_};
  }

  [[nodiscard]] friend constexpr my_scalar_type operator/(const my_scalar_type& lhs, const my_scalar_type& rhs)
  {
    return my_scalar_type{lhs.value_ / rhs.value_};
  }

  // Required for real scalar types: Equality & Total ordering
  [[nodiscard]] constexpr auto operator<=>(const my_scalar_type&) const = default;
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
  [[nodiscard]] constexpr T real() const { return real_; }
  [[nodiscard]] constexpr T imag() const { return imag_; }
  [[nodiscard]] constexpr T modulus() const { return std::hypot(real_, imag_); }

  // ... other required operations (addition, scaling, equality)
};
```

Or via free functions found through ADL:

```cpp
template<typename T>
[[nodiscard]] constexpr T real(const my_complex_type<T>& c) { return c.get_real(); }

template<typename T>
[[nodiscard]] constexpr T imag(const my_complex_type<T>& c) { return c.get_imag(); }

template<typename T>
[[nodiscard]] constexpr T modulus(const my_complex_type<T>& c) { return c.get_magnitude(); }
```

For **vectors**, provide the `magnitude()` customization point function, or a `norm()` fallback:

```cpp
template<typename T>
class my_vector_type {
  // ... implementation
public:
  using value_type = T;

  // Preferred: explicit name matching the library's CPO
  constexpr auto magnitude() const { /* compute Euclidean norm */ }
  // ... other required operations
};
```

Or via a free function found by ADL:

```cpp
template<typename T>
[[nodiscard]] constexpr auto magnitude(const my_vector_type<T>& v) { return v.compute_norm(); }
```

If your type already provides a `norm()` member or free function (e.g. because it follows
Eigen/NumPy conventions), no adaptation is needed — the `magnitude()` CPO recognizes `norm()`
as a fallback for vector types automatically:

```cpp
template<typename T>
class my_eigen_like_type {
public:
  using value_type = T;

  // The magnitude() CPO will find this automatically
  constexpr T norm() const { /* compute Euclidean norm */ }
  // ... other required operations
};
```

!!! tip "When to provide `norm()` vs `magnitude()`"

    Prefer providing `magnitude()` explicitly — it matches the library's CPO name directly.
    Use `norm()` when you are wrapping or adapting an existing type that already provides it
    (e.g. Eigen, Armadillo), so you avoid redundant adapters. Both names produce identical
    behavior; `norm()` is simply recognized as a zero-friction fallback.

Once your representation type provides `magnitude()` (or a `norm()` fallback), a
**vector or tensor quantity** built on it supports `magnitude()` automatically, returning
the magnitude as a scalar quantity (Euclidean for a real vector, Frobenius for a tensor,
Hermitian for a complex one):

```cpp
quantity v = my_vector_type<double>{3, 4, 0} * isq::velocity[m / s];
quantity speed = magnitude(v);  // 5 m/s
```

!!! warning "V2 limitation: result type"

    The magnitude result drops the precise _quantity spec_ down to the unit's kind, and
    quantity-level `scalar_product()` / `vector_product()` (dot/cross) are **not** provided in
    V2 — they would need to return a _different_ quantity kind, which V2 _references_ cannot
    express. Compute those on the raw representation and re-attach the reference yourself.

!!! info "Enabling decomposition into named components"

    To let a vector _quantity_ built on your type split into named component
    _quantities_, the type must be **indexable by a compile-time constant index** (the
    index is a template argument, and the access need not be `constexpr`), through either
    a tuple-like `get<Idx>` or a subscript `operator[]`. `cartesian_vector` and the
    supported third-party vectors already qualify, so no extra code is needed there. See
    [Decompose a Vector Quantity into Components](../advanced_usage/decompose_vector_quantity.md)
    for the full recipe.

#### Adapting an existing vector library

Any _weakly-regular_ vector type (copyable, with `bool`-returning equality) that exposes a
Euclidean norm can serve as a representation. The bundled integrations for
[Eigen](https://eigen.tuxfamily.org), [GLM](https://github.com/g-truc/glm), and
[Blaze](https://bitbucket.org/blaze-lib/blaze) wire up exactly the customization points
above, and they are the template for adapting any other library. Three things vary between
libraries:

- **Underlying type.** Eigen and GLM expose a `value_type` member, so
  `representation_underlying_type` detects it automatically. Blaze names it `ElementType`
  instead, so its plugin specializes `representation_underlying_type` explicitly.
- **Magnitude.** Eigen and Blaze provide `norm()`, which the `magnitude()` CPO uses
  directly. GLM spells it `length()`, so its plugin adds a one-line `magnitude()` overload
  (found by ADL) that forwards to it.
- **Order.** A library whose structural shape is ambiguous declares its order explicitly.
  An Eigen column vector is an `N×1` matrix, so it exposes *both* single-index `operator[]`
  and two-index `operator()`, making its order ambiguous, so
  [`tensor_order`](../../users_guide/framework_basics/representation_types.md#tensor_order)
  has no default for it and its plugin specializes it from the compile-time shape. GLM,
  whose vectors and matrices are structurally distinct, needs no such override.
- **Field.** Eigen and Blaze expose `real()`/`imag()` on their real matrices too, but no
  override is needed:
  [`numeric_field`](../../users_guide/framework_basics/representation_types.md#numeric_field)
  reads the field off a scalar element rather than the container's surface, so a real matrix
  of `double` is correctly classified real on its own.
- **Materializing expression templates.** Eigen and Blaze return lazy proxy types from
  their arithmetic operators. A proxy holds references to its operands, so storing one
  inside a `quantity` would leave dangling references once those operands expire. Their
  plugins specialize
  [`representation_canonical_type`](../../users_guide/framework_basics/representation_types.md#representation_canonical_type)
  to map each proxy to its evaluated concrete type (Eigen's `PlainObject`, Blaze's
  `ResultType`), so a `quantity` always stores a materialized value. GLM evaluates
  eagerly, so it needs no such specialization.

```cpp
// materialize an expression-template proxy before a quantity stores it
template<typename T>
  requires /* T is one of your library's lazy expression types */
struct mp_units::representation_canonical_type<T> {
  using type = std::remove_cvref_t<typename T::evaluated_type>;
};
```

!!! warning "Element-wise `operator==` is disqualifying"

    [Armadillo](https://arma.sourceforge.net) is the notable type that does **not** qualify: its
    `operator==` returns an element-wise mask rather than a `bool`, so it is not
    `std::equality_comparable` and cannot satisfy the representation requirements. See
    [Third-Party Library Integrations](../../users_guide/framework_basics/representation_types.md#third-party-library-integrations)
    for the complete picture.

### Step 3: Add Formatting Support (optional)

Enable formatting with `std::format`:

```cpp
template<typename T, typename Char>
struct std::formatter<my_scalar_type<T>, Char> : std::formatter<T, Char> {
  template<typename FormatContext>
  auto format(const my_scalar_type<T>& v, FormatContext& ctx) const
  {
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
  [[nodiscard]] static constexpr my_scalar_type<T> zero() noexcept { return my_scalar_type<T>{T{0}}; }
  [[nodiscard]] static constexpr my_scalar_type<T> one() noexcept { return my_scalar_type<T>{T{1}}; }
  [[nodiscard]] static constexpr my_scalar_type<T> min() noexcept
  {
    return my_scalar_type<T>{std::numeric_limits<T>::lowest()};
  }
  [[nodiscard]] static constexpr my_scalar_type<T> max() noexcept
  {
    return my_scalar_type<T>{std::numeric_limits<T>::max()};
  }
};
```

If your type will be used with bounds checking or other constraint enforcement mechanisms,
you may also want to specialize `constraint_violation_handler` to control error reporting:

```cpp
template<typename T>
struct mp_units::constraint_violation_handler<my_scalar_type<T>> {
  static void on_violation(std::string_view msg) {
    // Custom error handling: throw, log, abort, etc.
    throw std::runtime_error(std::string(msg));
  }
};
```

See the [Ensure Ultimate Safety](../advanced_usage/ultimate_safety.md) guide for details
on using bounds checking with quantity points.

### Step 5: Enable scaling { #scale }

The library applies a unit magnitude to a representation value internally when performing
unit conversions. Three built-in paths handle this automatically — see
[How Scaling Works](../../users_guide/framework_basics/representation_types.md#how-scaling-works)
for the full concept definitions and algorithm.

`my_scalar_type<T>` already satisfies the floating-point or integer scaling path
(`UsesFloatingPointScaling` or `UsesIntegerScaling`) through its existing
`operator*(my_scalar_type, T)` and `operator/(my_scalar_type, T)` —
no further customization is needed.

If your type is not automatically recognized (e.g., a third-party floating-point type with
no `value_type` member), expose the underlying type via
[`representation_underlying_type`](../../users_guide/framework_basics/representation_types.md#representation_underlying_type)
— `treat_as_floating_point` will then default to `true` automatically, with no further
specialization needed. The example below shows this typical case. Only specialize
[`treat_as_floating_point`](../../users_guide/framework_basics/representation_types.md#treat_as_floating_point)
directly when there is genuinely no meaningful underlying type to expose.

#### Unit-magnitude-aware scaling (optional) { #unit-magnitude-aware-scaling }

If your representation type needs to transform its **type** during unit
conversion — not just its value — provide an `operator*(T, UnitMagnitude)` hidden friend.
This is checked **before** the built-in paths and may return a different type.

A typical use case is a range-validated representation whose bounds are unit-specific:

```cpp
#include <mp-units/framework/unit_magnitude.h>

// Example custom type (not provided by the library)
template<std::movable T, auto Min, auto Max, typename Policy>
class bounded_value {
  // ...
public:
  template<mp_units::UnitMagnitude M>
    requires mp_units::treat_as_floating_point<T>
  [[nodiscard]] friend constexpr auto operator*(const bounded_value& val, M m)
  {
    constexpr T new_lo = mp_units::scale<T>(M{}, T{Min});
    constexpr T new_hi = mp_units::scale<T>(M{}, T{Max});

    const T scaled = mp_units::scale<T>(m, val.value());

    if constexpr (new_lo <= new_hi)
      return bounded_value<T, new_lo, new_hi, Policy>(scaled);  // different type!
    else
      return bounded_value<T, new_hi, new_lo, Policy>(scaled);
  }
};
```

See [Unit-magnitude-aware scaling](../../users_guide/framework_basics/representation_types.md#unit-magnitude-aware-scaling)
in the User's Guide for the complete pattern and design rationale.

??? example "`MyFloat` — integrating a third-party floating-point type"

    Suppose a third-party library provides a high-precision floating-point type that you
    cannot modify:

    ```cpp
    // Third-party type — you cannot modify the source.
    class MyFloat {
      long double v_;
    public:
      explicit(false) MyFloat(long double v) : v_(v) {}

      MyFloat operator-() const { return MyFloat{-v_}; }
      friend MyFloat operator+(MyFloat a, MyFloat b) { return MyFloat{a.v_ + b.v_}; }
      friend MyFloat operator-(MyFloat a, MyFloat b) { return MyFloat{a.v_ - b.v_}; }
      friend MyFloat operator*(MyFloat a, MyFloat b) { return MyFloat{a.v_ * b.v_}; }
      friend MyFloat operator/(MyFloat a, MyFloat b) { return MyFloat{a.v_ / b.v_}; }
      friend auto operator<=>(MyFloat, MyFloat) = default;
    };
    ```

    `MyFloat` is floating-point in spirit but the library cannot detect this automatically:

    - It has no `value_type` member, so `representation_underlying_type<MyFloat>` is empty
      and `MyFloat` is treated as a leaf (its own underlying type).
    - `treat_as_floating_point<MyFloat>` defaults to `std::is_floating_point_v<MyFloat>` = `false`
      (it is a class, not a fundamental type), so `UnitMagnitudeScalable<MyFloat>` is not satisfied.

    One specialization fixes this:

    ```cpp
    // Expose the underlying type so representation_underlying_type_t<MyFloat> == long double.
    // treat_as_floating_point<MyFloat> then defaults to
    // std::is_floating_point_v<long double> == true, so no further
    // specialization is needed.
    template<>
    struct mp_units::representation_underlying_type<MyFloat> {
      using type = long double;
    };
    ```

    After that specialization `MyFloat` satisfies `UsesFloatingPointScaling` and
    integrates with the library without any further changes:

    ```cpp
    static_assert(mp_units::UnitMagnitudeScalable<MyFloat>);
    static_assert(mp_units::RepresentationOf<MyFloat, mp_units::quantity_character::real_scalar>);

    const auto q = isq::length(MyFloat{1.0L} * m);
    const auto q_km = q.in(km);  // MyFloat * long double — handled by UsesFloatingPointScaling
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


### Vector Representation

The library provides [`cartesian_vector`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h)
as a vector representation type with full support for vector operations:

```cpp
#include <mp-units/utility/cartesian_vector.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/isq.h>

using namespace mp_units;
using namespace mp_units::utility;  // cartesian_vector lives here
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

The `cartesian_vector` implementation demonstrates how to create a full-featured vector
type with:

- Arithmetic operations (`+`, `-`, `*`, `/`)
- `magnitude()` member function
- Support for scalar and vector products
- Integration with quantity characters
- Element access for [decomposition into named components](../advanced_usage/decompose_vector_quantity.md)

**Implementation reference:**
[`cartesian_vector.h`](https://github.com/mpusz/mp-units/blob/1511c73d362649cca90191cdcfd3b369058c1dc1/src/core/include/mp-units/cartesian_vector.h)


### Tensor Representation

The library also provides
[`cartesian_tensor`](https://github.com/mpusz/mp-units/blob/master/src/utility/include/mp-units/utility/cartesian_tensor.h),
a fixed 3×3 second-order Cartesian tensor with the ISO 80000-2 second-order operations. The
operations are shown here on the representation type directly, which is where the vector and
tensor products live in the current release:

```cpp
#include <mp-units/utility/cartesian_tensor.h>

using namespace mp_units;
using namespace mp_units::utility;  // cartesian_tensor / cartesian_vector live here

void example()
{
  // a 3x3 second-order tensor, stored row-major
  cartesian_tensor<double> t{1, 2, 3,
                             4, 5, 6,
                             7, 8, 9};

  auto t12 = t(1, 2);       // element access by (row, column): 6

  auto sum = t + t;         // elementwise addition
  auto scaled = 2.0 * t;    // scalar multiplication

  // dyadic (tensor) product of two vectors -> tensor (ISO 80000-2, 2-18.21)
  auto outer = tensor_product(cartesian_vector{1., 2., 3.}, cartesian_vector{4., 5., 6.});

  auto product = inner_product(t, t);                            // tensor ⋅ tensor -> tensor (2-18.23)
  auto mapped = inner_product(t, cartesian_vector{1., 2., 3.});  // tensor ⋅ vector -> vector (2-18.24)
  auto contraction = scalar_product(t, t);                       // double-dot t : t -> scalar (2-18.25)
  auto frobenius = t.magnitude();                                // Frobenius norm -> scalar
}
```

`cartesian_tensor` is kept out of the vector character by its intrinsic `tensor_order`
of `2`, so it can never be used where a vector representation is expected. The fourth-order
tensor product `T ⊗ S` (ISO 80000-2, 2-18.22) and `transpose`/`trace` are not part of this
release.

**Implementation reference:**
[`cartesian_tensor.h`](https://github.com/mpusz/mp-units/blob/master/src/utility/include/mp-units/utility/cartesian_tensor.h)


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

See [`value_type` or `element_type`](../../users_guide/framework_basics/representation_types.md#representation_underlying_type)
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
  [[nodiscard]] constexpr T real(const my_complex<T>& c) { return c.get_real(); }

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
   via member functions or ADL-findable free functions. For an expression-template vector
   type, specialize `representation_canonical_type` so lazy proxies are materialized
   before storage
3. **Add formatting support** (optional) via `std::formatter`
4. **Add `value_type`** to help the library determine the scaling factor type
5. **Specialize `representation_values<Rep>`** (if needed) for custom special values;
   optionally specialize `constraint_violation_handler<Rep>` for custom error handling
6. **Implement `operator*(T, representation_underlying_type_t<T>)` and
   `operator/(T, representation_underlying_type_t<T>)`** so
   that scaling correctly updates all internal fields (e.g. for `with_variance<T>` scale
   `value` by `k` and `variance` by `k²`)
7. **Implement `operator*(T, UnitMagnitude)`** (optional) for
   [unit-magnitude-aware scaling](#unit-magnitude-aware-scaling) when bounds or other type-level
   properties must change during unit conversion
8. **Specialize `implicitly_scalable`** (if needed) to control implicit vs. explicit
   conversion semantics
9. **Verify with concepts** using `static_assert`

The library handles the rest, providing strong type safety and dimensional analysis for your
custom types.


## See Also

<!-- markdownlint-disable MD013 -->
**User's Guide:**

- [Representation Types](../../users_guide/framework_basics/representation_types.md) - Complete design and requirements reference
- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) - Understanding quantity characters
- [Value Conversions](../../users_guide/framework_basics/value_conversions.md) - How `treat_as_floating_point` and `implicitly_scalable` affect conversions
- [Concepts](../../users_guide/framework_basics/concepts.md#RepresentationOf) - The `RepresentationOf` concept definition

**How-to Guides:**

- [Ensure Ultimate Safety](../advanced_usage/ultimate_safety.md) - Combining `constrained` reps with `check_in_range` for guaranteed bounds enforcement
- [Decompose a Vector Quantity into Components](../advanced_usage/decompose_vector_quantity.md) - Element-access requirement for splitting a vector quantity into named component quantities

**Examples:**

- [Using a Linear Algebra Library as the Representation](../../examples/linear_algebra.md) - Eigen, GLM, Blaze, and the built-in `cartesian_vector` driven through the same scenario

**Implementation References:**

- [`representation_concepts.h`](https://github.com/mpusz/mp-units/blob/master/src/core/include/mp-units/framework/representation_concepts.h) - The public `RepresentationOf` concept (built on internal, not-yet-public character concepts) and the `magnitude` CPO
- [`customization_points.h`](https://github.com/mpusz/mp-units/blob/master/src/core/include/mp-units/framework/customization_points.h) - User-specializable customization points: the character traits (`numeric_field`, `tensor_order`, `disable_representation`) and CPOs (`real`, `imag`, `modulus`), plus `representation_underlying_type`, `representation_canonical_type`, `treat_as_floating_point`, `representation_values`, `constraint_violation_handler`, `quantity_like_traits`, `quantity_point_like_traits`
- [`quantity_traits.h`](https://github.com/mpusz/mp-units/blob/master/src/core/include/mp-units/framework/quantity_traits.h) - Public helpers (`unit_for`, `reference_for`, `rep_for`)
- [`scaling.h`](https://github.com/mpusz/mp-units/blob/master/src/core/include/mp-units/framework/scaling.h) - Built-in scaling implementation
- [`value_cast.h`](https://github.com/mpusz/mp-units/blob/master/src/core/include/mp-units/framework/value_cast.h) - `value_cast`, `force_in`, `is_integral_scaling`, and `implicitly_scalable`
- [`cartesian_vector.h`](https://github.com/mpusz/mp-units/blob/master/src/utility/include/mp-units/utility/cartesian_vector.h) - Vector implementation example
- [`cartesian_tensor.h`](https://github.com/mpusz/mp-units/blob/master/src/utility/include/mp-units/utility/cartesian_tensor.h) - Second-order tensor implementation example
<!-- markdownlint-enable MD013 -->
