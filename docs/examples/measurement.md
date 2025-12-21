---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- Feature - Custom Representation
- Feature - Faster-Than-Lightspeed Constants
- Feature - Unit Conversions
- Domain - Physics
- Domain - Metrology
---

# Measurement with Uncertainty

[Try it live on Compiler Explorer](https://godbolt.org/z/e8eEz56Mr){ .md-button }


## Overview

Scientific and engineering measurements always carry uncertainty. This example demonstrates
how to create a custom representation type that pairs measured values with their
uncertainties and automatically propagates errors through calculations using the **mp-units**
library.

The implementation provides first-order uncertainty propagation following standard error
analysis formulas, making it suitable for experimental physics, metrology, and engineering
applications. It's particularly relevant for systems like IAU (International Astronomical
Union) where constants are defined with explicit uncertainties.

The example also demonstrates the **Faster-than-lightspeed constants** feature, where
mathematical constants like π are treated as exact symbolic values that don't contribute
to numerical uncertainty, ensuring that only measurement errors propagate through
geometric calculations.

## The `measurement` Class

### Header File

The complete implementation is provided in a separate header file that can be reused across
projects:

```cpp title="measurement.h" linenums="1"
--8<-- "example/include/measurement.h:23"
```

!!! info "Key implementation details"

    - **Value and uncertainty storage**: Stores both the measured value and its absolute uncertainty (σ)
    - **Automatic error propagation**: All arithmetic operators implement proper uncertainty formulas
    - **Mathematical functions**: Includes `pow`, `sqrt`, `exp`, `log` with correct uncertainty derivatives
    - **First-order approximation**: Uses linear Taylor expansion for uncertainty propagation
    - **Independent variables assumption**: Does not track correlations between measurements

### Integration with mp-units

To use a custom type as a quantity representation, it must satisfy the `RepresentationOf`
concept. The library verifies this at compile time:

```cpp title="measurement.cpp"
--8<-- "example/measurement.cpp:47:48"
```

This allows `measurement<T>` to be used seamlessly with any **mp-units** quantity type.

## Example Usage

### Basic Operations

The example demonstrates various uncertainty propagation scenarios:

```cpp title="measurement.cpp"
--8<-- "example/measurement.cpp:50:77"
```

This showcases:

1. **Astronomical constants** - Using magnitudes with uncertainties for large-scale values
2. **Kinematic calculations** - Multiplying _acceleration_ by _time_ with automatic unit
   conversions
3. **Scalar multiplication** - Exact scaling preserves relative uncertainty
4. **Faster-than-lightspeed constants** - Using `mag<π>` keeps π as an exact symbolic value,
   not a numeric approximation
5. **Geometric calculations** - Computing _circumference_ and _area_ where π contributes
   zero uncertainty
6. **Inverse operations** - Computing _radius_ from _area_ using `sqrt`, with π canceling
   out exactly

**Sample Output:**

```text
Mass of the Sun:        M_sun = 19884 ± 2 (10²⁶ kg)
Velocity calculation:   V = 9.8 ± 0.1 m/s² * 1.2 ± 0.1 s = 11.76 ± 0.98732 m/s = 42.336 ± 3.55435 km/h
Scalar multiplication:  d = 10 * 123 ± 1 m = 1230 ± 10 m
Radius:                 r = 5 ± 0.1 m
Circular circumference: 2πr = 5 ± 0.1 (2 π) m = 31.4159 ± 0.628319 m
Circular area:          πr² = 25 ± 1 (π) m² = 78.5398 ± 3.14159 m²
Radius from area:       A = 25 ± 1 (π m²) -> r = √(A/π) = 5 ± 0.1 m
```

### Uncertainty Propagation Analysis

Notice how uncertainties propagate through the calculations:

- **Velocity**: Combining two independent measurements (_acceleration_ and _time_),
  both with ~1-2% relative uncertainty, yields a _velocity_ with ~8.4% relative uncertainty
  (`0.98732/11.76 ≈ 0.084`)
- **Scalar multiplication**: Multiplying by exact scalar 10 preserves the relative
  uncertainty (~0.8%)
- **Circumference (2πr)**: The factor 2π is exact (no uncertainty), so only the _radius_
  uncertainty propagates: `σ_c = 2π × 0.1 m = 0.628... m`
- **Area (πr²)**: Squaring doubles the relative uncertainty (r: 2% → r²: 4%), while π
  remains exact: `σ_A = π × |2r × σ_r| = π × 1 m² = 3.14... m²`
- **Round-trip verification**: Computing _radius_ from _area_ with `√(A/π)` recovers
  the original `5 ± 0.1 m` because π cancels exactly—this demonstrates the power of
  symbolic constants in avoiding accumulated numerical errors

## Error Propagation Formulas

The implementation uses standard first-order uncertainty propagation:

<!-- markdownlint-disable MD056 -->
| Operation                    | Formula                          | Implementation                       |
|------------------------------|----------------------------------|--------------------------------------|
| Addition: `z = x + y`        | `σ_z² = σ_x² + σ_y²`             | Quadrature sum (Pythagorean)         |
| Subtraction: `z = x - y`     | `σ_z² = σ_x² + σ_y²`             | Same as addition                     |
| Multiplication: `z = x × y`  | `(σ_z/z)² = (σ_x/x)² + (σ_y/y)²` | Relative uncertainties in quadrature |
| Division: `z = x / y`        | `(σ_z/z)² = (σ_x/x)² + (σ_y/y)²` | Same as multiplication               |
| Scalar multiply: `z = k × x` | `σ_z = |k| × σ_x`                | Exact scaling                        |
| Power: `z = x^n`             | `σ_z = |n × z/x × σ_x|`          | Derivative method                    |
| Square root: `z = √x`        | `σ_z = σ_x / (2√x)`              | Special case of power                |
| Exponential: `z = exp(x)`    | `σ_z = |z × σ_x|`                | Derivative method                    |
| Logarithm: `z = ln(x)`       | `σ_z = |σ_x / x|`                | Derivative method                    |
<!-- markdownlint-enable MD056 -->

## Limitations and Caveats

!!! warning "Independent measurements assumption"

    The `measurement` class assumes all values are statistically independent. Operations like
    `x - x` will incorrectly give non-zero uncertainty. For correlated measurements, a more
    sophisticated approach with covariance matrices is needed.

!!! note "First-order approximation"

    Uncertainty propagation uses linear approximation (first derivative only). This is accurate
    when relative uncertainties are small (<10%). For larger uncertainties or highly non-linear
    functions, higher-order terms may be significant.

!!! tip "When to use this implementation"

    - ✅ Combining independent measurements from different instruments
    - ✅ Standard metrology and experimental physics calculations
    - ✅ Systems like IAU where constants have defined uncertainties
    - ✅ Small to moderate relative uncertainties (<10%)
    - ❌ Correlated measurements (same source used multiple times)
    - ❌ Large relative uncertainties (>10%)
    - ❌ Systematic errors (requires different treatment)

## Why This Matters

- **Automatic Error Propagation**: No manual uncertainty calculations needed—formulas are built into operators
- **Type Safety**: The dimensional analysis works with uncertainties seamlessly through **mp-units**
- **Scientific Accuracy**: Properly tracks measurement precision through complex calculations
- **Standard Compliant**: Follows ISO GUM (Guide to the Expression of Uncertainty in Measurement) principles
- **Extensibility**: Demonstrates how to integrate domain-specific numeric types with **mp-units**
- **Real-World Applicability**: Suitable for IAU astronomical constants, NIST physical constants, and laboratory measurements

This pattern is essential for scientific computing, metrology, laboratory measurements, and any application
where measurement precision and traceability matter.
