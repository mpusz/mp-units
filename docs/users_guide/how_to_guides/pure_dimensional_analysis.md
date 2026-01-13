# Pure Dimensional Analysis

## Overview

The **mp-units** library is not limited to working with complete quantities (i.e., a numerical
value paired with a unit). It also provides powerful support for **pure dimensional analysis**,
where you can track and validate dimensions and quantity types without concerning yourself
with actual quantity values.

This feature allows users to embed `Dimension` or `QuantitySpec` types into their own custom
arithmetic types and leverage the library's dimensional analysis capabilities. By propagating
arithmetic operations to these embedded types through custom overloaded operators, the library
automatically performs full dimensional analysis and type checking—all at compile-time, with
zero runtime overhead.


## Use Cases

Pure dimensional analysis is particularly useful when:

- Designing symbolic computation systems that manipulate physical formulas
- Validating dimensional consistency in equation systems before numerical evaluation
- Working with templates or compile-time computations where actual values aren't available
  yet
- Implementing custom arithmetic types (automatic differentiation, interval arithmetic,
  symbolic expressions) while maintaining dimensional correctness
- Documenting and enforcing dimensional relationships in your API without committing to
  specific units


## Working with Dimensions

A dimension represents the physical nature of a quantity without specifying its unit.
In **mp-units**, dimensions can be used independently to verify dimensional consistency:

```cpp
// Extract dimensions from quantity specifications
constexpr auto length_dim = isq::length.dimension;
constexpr auto time_dim = isq::duration.dimension;

// Dimensions support arithmetic operations
constexpr auto velocity_dim = length_dim / time_dim;
constexpr auto acceleration_dim = velocity_dim / time_dim;

// Verify dimensional consistency at compile-time
static_assert(velocity_dim == isq::velocity.dimension);
static_assert(acceleration_dim == isq::acceleration.dimension);
```


## Working with Quantity Specifications

While dimensions track the fundamental physical nature (like _length_, _time_, _mass_),
quantity specifications (`quantity_spec`) provide a more refined type system that
distinguishes between different quantities sharing the same dimension:

```cpp
#include <mp-units/systems/isq.h>

using namespace mp_units;

// Different quantities can have the same dimension
static_assert(isq::width.dimension == isq::height.dimension);
static_assert(isq::width.dimension == isq::radius.dimension);

// But they remain distinct types
static_assert(isq::width != isq::height);
static_assert(isq::width != isq::radius);

// Quantity specs also support arithmetic operations
constexpr auto area_spec = isq::width * isq::height;
constexpr auto volume_spec = area_spec * isq::depth;

// The derived specs use more specialized quantities than the library's canonical definitions,
// but they are implicitly convertible to them
static_assert(area_spec != isq::area);
static_assert(volume_spec != isq::volume);
static_assert(implicitly_convertible(area_spec, isq::area));
static_assert(implicitly_convertible(volume_spec, isq::volume));
```


## Embedding in Custom Types

The real power of pure dimensional analysis emerges when you embed dimensions or quantity
specs into your own types. Here's an example with a simple symbolic expression type:

```cpp
#include <mp-units/systems/isq.h>
#include <string>
#include <format>

using namespace mp_units;

// A symbolic expression that tracks its physical dimension
template<QuantitySpec auto QS>
class symbolic_expression {
  std::string expr_;
public:
  explicit symbolic_expression(std::string expr) : expr_(std::move(expr)) {}
  [[nodiscard]] const std::string& expression() const { return expr_; }
  [[nodiscard]] static constexpr auto quantity_spec() { return QS; }
  [[nodiscard]] static constexpr auto dimension() { return QS.dimension; }
};

// Operator overloads that propagate dimensional analysis
template<QuantitySpec auto QS1, QuantitySpec auto QS2>
auto operator*(const symbolic_expression<QS1>& lhs, const symbolic_expression<QS2>& rhs)
{
  constexpr auto result_qs = QS1 * QS2;
  return symbolic_expression<result_qs>(std::format("({} * {})", lhs.expression(), rhs.expression()));
}

template<QuantitySpec auto QS1, QuantitySpec auto QS2>
auto operator/(const symbolic_expression<QS1>& lhs, const symbolic_expression<QS2>& rhs)
{
  constexpr auto result_qs = QS1 / QS2;
  return symbolic_expression<result_qs>(std::format("({} / {})", lhs.expression(), rhs.expression()));
}

template<QuantitySpec auto QS1, QuantitySpec auto QS2>
auto operator+(const symbolic_expression<QS1>& lhs, const symbolic_expression<QS2>& rhs)
  requires requires { QS1 + QS2; }  // Enforce dimensional compatibility for addition
{
  constexpr auto result_qs = QS1 + QS2;
  return symbolic_expression<result_qs>(std::format("({} + {})", lhs.expression(), rhs.expression()));
}

void example()
{
  symbolic_expression<isq::length> distance("d");
  symbolic_expression<isq::duration> duration("t");
  symbolic_expression<isq::mass> mass("m");

  // These operations are dimensionally valid
  auto velocity = distance / duration;
  auto acceleration = velocity / duration;
  auto force = mass * acceleration;
  auto length = distance + distance;

  static_assert(implicitly_convertible(velocity.quantity_spec(), isq::speed));
  static_assert(force.dimension() == isq::force.dimension);

  // This would fail to compile due to dimensional mismatch:
  // auto invalid = distance + duration;  // Error: cannot add length and time
}
```


## Advanced Example: Automatic Differentiation

Here's a more sophisticated example showing how pure dimensional analysis can be integrated
with automatic differentiation:

```cpp
#include <mp-units/systems/isq.h>
#include <iostream>

using namespace mp_units;

template<QuantitySpec auto ValueQS, QuantitySpec auto DerivativeQS, typename T = double>
class dual_number {
  T value_;
  T derivative_;

public:
  dual_number(T value, T derivative) : value_(value), derivative_(derivative) {}

  [[nodiscard]] T value() const { return value_; }
  [[nodiscard]] T derivative() const { return derivative_; }
  [[nodiscard]] static constexpr auto value_quantity_spec() { return ValueQS; }
  [[nodiscard]] static constexpr auto derivative_quantity_spec() { return DerivativeQS; }
};

template<QuantitySpec auto VQS1, QuantitySpec auto DQS1,
         QuantitySpec auto VQS2, QuantitySpec auto DQS2, typename T>
auto operator*(const dual_number<VQS1, DQS1, T>& lhs, const dual_number<VQS2, DQS2, T>& rhs)
{
  constexpr auto result_vqs = VQS1 * VQS2;
  constexpr auto result_dqs = DQS1 * VQS2;  // Simplified: assumes same differentiation variable
  // Product rule: (f·g)' = f'·g + f·g'
  return dual_number<result_vqs, result_dqs, T>(
    lhs.value() * rhs.value(),
    lhs.derivative() * rhs.value() + lhs.value() * rhs.derivative()
  );
}

template<QuantitySpec auto VQS1, QuantitySpec auto DQS1,
         QuantitySpec auto VQS2, QuantitySpec auto DQS2, typename T>
auto operator/(const dual_number<VQS1, DQS1, T>& lhs, const dual_number<VQS2, DQS2, T>& rhs)
{
  constexpr auto result_vqs = VQS1 / VQS2;
  constexpr auto result_dqs = DQS1 / VQS2;  // Simplified: assumes same differentiation variable
  // Quotient rule: (f/g)' = (f'·g - f·g') / g²
  return dual_number<result_vqs, result_dqs, T>(
    lhs.value() / rhs.value(),
    (lhs.derivative() * rhs.value() - lhs.value() * rhs.derivative()) / (rhs.value() * rhs.value())
  );
}

int main()
{
  // Compute kinetic energy E = (1/2) * m * v² and its derivative with respect to velocity

  // Mass: m = 2 kg, dm/dv = 0 (mass doesn't change with velocity)
  dual_number<isq::mass, isq::mass / isq::speed> m(2.0, 0.0);

  // Velocity: v = 10 m/s, dv/dv = 1 (dimensionless)
  dual_number<isq::speed, dimensionless> v(10.0, 1.0);

  // Compute v² (value: speed², derivative: speed)
  auto v_squared = v * v;

  // Compute (1/2) * m
  auto half_m = dual_number<isq::mass, isq::mass / isq::speed>(m.value() / 2.0, 0.0);

  // Final kinetic energy: E = (1/2) * m * v²
  auto kinetic_energy = half_m * v_squared;

  // Display results:
  // Energy value: E = 100 J (dimension: L²MT⁻²)
  // Energy derivative: dE/dv = 20 kg⋅m/s (dimension: LMT⁻¹ - momentum)
  std::cout << kinetic_energy.value() << " of " << kinetic_energy.value_quantity_spec().dimension << '\n';
  std::cout << kinetic_energy.derivative() << " of " << kinetic_energy.derivative_quantity_spec().dimension << '\n';
}
```

Output:

```text
100 of L²MT⁻²
20 of LMT⁻¹
```

With this approach, you get both automatic differentiation **and** compile-time
dimensional analysis, ensuring that your derivatives maintain proper physical dimensions
throughout the computation.


## Real-World Example: Kalman Filter

The library includes a complete [Kalman filter example](../../examples/kalman_filter.md)
demonstrating pure dimensional analysis in a practical application. The implementation
uses dimensions to enforce compile-time constraints on state variables:

```cpp
template<mp_units::Dimension auto... Ds>
constexpr bool are_time_derivatives = false;

template<mp_units::Dimension auto D>
constexpr bool are_time_derivatives<D> = true;

template<mp_units::Dimension auto D1, mp_units::Dimension auto D2, mp_units::Dimension auto... Ds>
constexpr bool are_time_derivatives<D1, D2, Ds...> =
  (D1 / D2 == mp_units::isq::dim_time) && are_time_derivatives<D2, Ds...>;
```

This metafunction validates that a sequence of dimensions represents successive time
derivatives (e.g., _position_ → _velocity_ → _acceleration)_, where each dimension divided
by the next equals time. The constraint ensures the Kalman filter state can only be
instantiated with physically meaningful derivative chains:

```cpp
template<mp_units::QuantityPoint... QPs>
  requires(sizeof...(QPs) > 0) && (sizeof...(QPs) <= 3) && detail::are_time_derivatives<QPs::dimension...>
class system_state {
  // ...
};
```

This is conceptually related to the automatic differentiation example above, but serves
a different purpose: while automatic differentiation computes mathematical derivatives of
functions, this validates that state variables represent successive physical time
derivatives of each other. Both demonstrate how pure dimensional analysis enables
sophisticated compile-time validation without requiring actual quantity values.


## Benefits

Using pure dimensional analysis in **mp-units** provides several advantages:

1. **Type Safety**: Dimensional errors are caught at compile-time, preventing entire
  classes of bugs
2. **Zero Runtime Overhead**: All dimensional analysis happens at compile-time with no
  runtime cost
3. **Flexibility**: You can use dimensions/quantity specs with any arithmetic type, not
  just built-in numerics
4. **Expressiveness**: Code clearly documents the physical nature of computations
5. **Composability**: Easily integrate with other compile-time programming techniques
6. **Unit Independence**: Focus on dimensional relationships without committing to
  specific units


## Summary

Pure dimensional analysis in **mp-units** enables you to leverage the library's powerful
type system for dimensional validation even when you're not working with complete quantities.
By embedding `Dimension` or `QuantitySpec` types into your custom arithmetic types and
propagating operations to them, you achieve full compile-time dimensional analysis with
zero runtime overhead.

This feature unlocks new possibilities for symbolic computation, automatic differentiation,
interval arithmetic, and other advanced use cases while maintaining the strong dimensional
correctness guarantees that **mp-units** is known for.
