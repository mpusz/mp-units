---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- System - Natural Units
- Feature - Physical Constants
- Feature - Faster-Than-Lightspeed Constants
- Feature - Quantity Concepts
- Feature - Unit Conversions
- Domain - Physics
---

# Relativistic Energy Calculations: SI vs Natural Units

[Try it live on Compiler Explorer](https://godbolt.org/z/K18hGKWcd){ .md-button }

## Overview

This example demonstrates the relativistic _energy-momentum_ relation $E^2 = (pc)^2 + (mc^2)^2$
using both SI units (with explicit c factors) and natural units (where c = 1). It
showcases how **mp-units** supports multiple systems of quantities and how natural
units simplify complex relativistic formulas while maintaining dimensional correctness.

## Key Concepts

### The Energy-Momentum Relation

In special relativity, the total energy of a particle is related to its momentum and
rest mass through:

**SI units (explicit constants):**

$$E^2 = (pc)^2 + (mc^2)^2$$

$$E = \sqrt{(pc)^2 + (mc^2)^2}$$

**Natural units (c = ℏ = 1):**

$$E^2 = p^2 + m^2$$

$$E = \sqrt{p^2 + m^2}$$

### Dual Implementation Strategy

The example provides two implementations of the same physics:

**SI units version** - requires explicit speed of light:

```cpp
--8<-- "example/total_energy.cpp:48:52"
```

**Natural units version** - c = 1 by definition, no parameter needed:

```cpp
--8<-- "example/total_energy.cpp:54:58"
```

Notice how the natural units version is simpler: no `c` parameter, and the formula
directly matches the mathematical notation used by physicists.

### SI Units: Multiple Representations

The SI example demonstrates the same calculation in three different unit representations:

**Using GeV and c:**

```cpp
--8<-- "example/total_energy.cpp:70:79"
```

**Converting to pure GeV:**

```cpp
--8<-- "example/total_energy.cpp:81:88"
```

**Using SI base units (kg, m, s):**

```cpp
--8<-- "example/total_energy.cpp:90:100"
```

Each representation is physically equivalent, but the choice affects numerical values
and readability.

**Sample Output:**

```text
*** SI units (c = 1 c = 2.99792e+08 m/s) ***

[in `GeV` and `c`]
p = 4 GeV/c
m = 3 GeV/c²
E = 5 GeV

[in `GeV`]
p = 1.33426e-08 GeV s/m
m = 3.33795e-17 GeV s²/m²
E = 5 GeV

[in SI base units]
p = 2.13771e-18 kg m/s
m = 5.34799e-27 kg
E = 8.01088e-10 J

[converted from SI units back to GeV]
E = 5 GeV
```

### Natural Units: Simplified Physics

The natural units example shows the elegance of setting c = 1:

```cpp
--8<-- "example/total_energy.cpp:108:115"
```

**Sample Output:**

```text
*** Natural units (c = 1) ***
p = 4 GeV
m = 3 GeV
E = 5 GeV
```

Notice:

- _Momentum_, _mass_, and _energy_ all have the same dimension (_energy_) in natural units
- No conversion factors needed between quantities
- The Pythagorean relation $E^2 = p^2 + m^2$ is immediately visible
- The formula matches exactly what appears in physics textbooks

## Why This Matters

### SI Units Advantages

- **Explicit Constants**: All physical constants are visible in formulas
- **Intuitive Scaling**: Familiar units (meters, seconds, kilograms)
- **Experimental Context**: Direct connection to laboratory measurements
- **Engineering Applications**: Practical for real-world calculations

### Natural Units Advantages

- **Simplified Formulas**: Match theoretical physics notation exactly
- **Dimensional Insights**: Relationships like "mass IS energy" become explicit
- **Particle Physics**: Standard in high-energy physics research
- **Reduced Clutter**: Eliminates repetitive factors of c and ℏ

### Library Support for Both

**mp-units** uniquely supports both paradigms:

- SI uses ISQ quantity types with explicit dimensional relationships
- Natural units use a separate quantity system based on energy dimensions
- The two systems are **incompatible** - conversions require extracting numerical values
  and manually applying conversion factors (no automatic type-safe conversion)
- Both provide compile-time dimensional checking within their respective systems

## Practical Applications

This pattern is essential for:

- **Particle Physics**: Calculating particle _energies_, decay rates, cross-sections
- **Relativistic Mechanics**: Any application where $v \approx c$
- **Quantum Field Theory**: Natural units are the standard
- **Educational Code**: Teaching relativity with simplified formulas
- **Cross-Domain Work**: Working with both experimental (SI) and theoretical (natural) contexts

## Related

- [Natural Units Documentation](../users_guide/systems/natural_units.md)
- [SI Units Documentation](../users_guide/systems/si.md)
- [Faster-Than-Lightspeed Constants](../users_guide/framework_basics/faster_than_lightspeed_constants.md)
