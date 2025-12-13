---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- Feature - Custom Quantities
- Feature - Faster-Than-Lightspeed Constants
- Feature - Physical Constants
- Feature - Quantity Concepts
- Feature - Unit Conversions
- Feature - Value Truncation
- Feature - Text Formatting
- Domain - Engineering
---

# Storage Tank Calculations with Custom Quantity Types

[Try it live on Compiler Explorer](https://godbolt.org/z/r57s5Gon1){ .md-button }

## Overview

This example demonstrates how to create domain-specific quantity types with constrained
quantity equations, modeling a practical engineering problem: calculating _fill levels_,
_capacities_, and _flow rates_ for storage tanks.

## Key Concepts

### Custom Quantity Specifications

The library allows defining specialized quantity types that are more specific than
the base ISQ quantities:

```cpp
--8<-- "example/storage_tank.cpp:52:57"
```

**Why constrain quantity equations?**

- `horizontal_length` is explicitly a kind of `isq::length`, but it represents specifically
  horizontal measurements
- `horizontal_area` must be calculated as `horizontal_length * isq::width`, not just any
  two _lengths_
- This prevents mixing incompatible physical interpretations (e.g., vertical × vertical areas)

### Engineering Calculations with Quantities

The `StorageTank` class demonstrates practical engineering calculations with full dimensional
analysis:

```cpp
--8<-- "example/storage_tank.cpp:60:94"
```

Notice how:

- `filled_weight()` properly multiplies volume by density and gravitational acceleration
- `fill_level()` inverts the calculation, and **dimensional analysis automatically cancels `g`**
  from both numerator and denominator: `(measured_mass × g) / (density × volume × g)` → `measured_mass / (density × volume)`
- `spare_capacity()` computes remaining volume from geometric constraints

### Polymorphic Tank Shapes

The example shows how object-oriented design works naturally with quantities:

```cpp
--8<-- "example/storage_tank.cpp:97:112"
```

Different tank shapes can be modeled through inheritance while maintaining type safety and
dimensional correctness.

## Example Application

Monitoring a rectangular tank being filled with water:

```cpp
--8<-- "example/storage_tank.cpp:118"
```

**Sample Output:**

```text
fill height at 200 s = 0.04 m (20 % full)
fill weight at 200 s = 100 kg g₀ (980.665 N)
spare capacity at 200 s = 0.08 m³
input flow rate = 0.1 kg/s
float rise rate = 2e-04 m/s
tank full E.T.A. at current flow rate = 800 s
```

## Why This Matters

- **Domain Modeling**: Custom quantity types encode domain knowledge (horizontal vs vertical measurements)
- **Compile-Time Safety**: Invalid quantity equations are caught at compile time
- **Engineering Accuracy**: Complex formulas are automatically verified for dimensional correctness
- **Practical Applications**: Tank monitoring, fluid management, industrial process control

This pattern is valuable for any domain where specialized quantity types improve clarity and safety.
