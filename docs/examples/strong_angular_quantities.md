---
tags:
- Level - Intermediate
- System - ISQ
- System - SI
- System - Strong Angular
- Feature - Quantity Concepts
- Feature - Unit Conversions
- Domain - Mechanics
---

# Angular Quantities and Torque Calculations

[Try it live on Compiler Explorer](https://godbolt.org/z/8G4KPsW8a){ .md-button }

## Overview

Angular mechanics requires careful handling of _angles_, _angular positions_, and
angular-dependent quantities like _torque_. This example demonstrates using the
dedicated ISQ angular system to calculate _torque_ from _force,_ lever arm _position_,
and _angle_ with proper dimensional analysis.

## Key Concepts

### The ISQ Angular System

The library provides `isq_angle`, an alternative ISQ system that treats angles as
dimensional quantities rather than dimensionless:

```cpp
--8<-- "example/strong_angular_quantities.cpp:42:50"
```

In this system:

- `isq_angle::position_vector` represents a _position_ with angular dependence
- `isq_angle::force` represents _force_ with angular considerations
- `isq_angle::angular_measure` explicitly represents _angles_ (not dimensionless)
- `isq_angle::torque` has dimensions that include angular components

### Dimensional Torque Calculation

The torque formula **τ = r × F × sin(θ)** is implemented with full dimensional checking:

```cpp
--8<-- "example/strong_angular_quantities.cpp:50:50"
```

The division by `cotes_angle` normalizes the angular measure to work correctly with
the dimensional analysis system, ensuring that the resulting torque has the proper
dimensions.

### Working with Angles as Quantities

Unlike treating _angles_ as dimensionless (radians as pure numbers), this approach makes
_angles_ first-class quantities:

```cpp
const quantity angle = isq_angle::angular_measure(90. * deg);
```

This enables:

- Compile-time checking that _angles_ are used correctly
- Explicit conversion between _angle_ units (degrees, radians, etc.)
- Prevention of accidentally treating _angles_ as scalars

### Output Formatting

The result is displayed with explicit angular units:

```cpp
--8<-- "example/strong_angular_quantities.cpp:51:52"
```

**Sample Output:**

```text
Applying a perpendicular force of 500 N to a 20 cm long lever results in 100 m N/rad of torque.
```

## Why This Matters

- **Dimensional Correctness**: Angles are strong quantities, not just numbers — this
  system enforces that
- **Rotational Mechanics**: Essential for robotics, mechanical engineering, aerospace
  applications
- **Type Safety**: Prevents mixing angular and linear quantities inappropriately
- **ISQ Compliance**: Follows international standards for quantity systems

## Alternative Approach

For simpler cases where angles can be treated as dimensionless, the standard `isq` system
can be used. The `isq_angle` system is valuable when you need stronger type checking for
angular quantities or when working with standards that treat angles dimensionally.

## Related Concepts

- _Angular velocity and acceleration_
- _Moment of inertia_
- _Angular momentum_
- _Rotational kinetic energy_

All of these benefit from treating angles as dimensional quantities rather than pure scalars.

For more details on the theoretical background and design decisions, see the
[Strong Angular System](../../users_guide/systems/strong_angular_system.md) chapter
in the user's guide.
