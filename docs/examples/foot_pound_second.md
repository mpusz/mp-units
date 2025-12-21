---
tags:
- Level - Intermediate
- System - Imperial
- System - SI
- Feature - Multi-System Units
- Feature - Unit Conversions
- Feature - Text Formatting
- Domain - Engineering
---

# Working with Imperial and FPS Units

[Try it live on Compiler Explorer](https://godbolt.org/z/aWvvfMPMf){ .md-button }

## Overview

Many engineering domains, particularly naval and aerospace applications, use the imperial
foot-pound-second (FPS) unit system. This example demonstrates seamless interoperability
between imperial/FPS units and SI, using World War II battleship specifications as
a practical case study.

## Key Concepts

### Multi-System Definitions

Real-world applications often need to work with data defined in different unit systems:

```cpp
--8<-- "example/foot_pound_second.cpp:52:65"
```

The `Ship` structure naturally expresses specifications in FPS units, as they would appear
in historical naval documentation.

### Transparent Unit Conversion

The example shows how to display the same quantity in multiple unit systems simultaneously:

```cpp
--8<-- "example/foot_pound_second.cpp:67:73"
```

This `fmt_line` helper function prints a quantity in its native units plus converted to
alternative systems, making it easy to compare specifications across different engineering
traditions.

### Real-World Application

The example models three WWII battleships, each defined in their historically appropriate units:

```cpp
--8<-- "example/foot_pound_second.cpp:97:128"
```

Notice:

- **KMS Bismarck**: Defined in SI/metric units (meters, kg, km/h) as used by Germany
- **USS Iowa & HMS King George V**: Defined in FPS units (feet, inches, pounds, knots) as used by USA and UK
- All mixed types work seamlessly without manual conversions

### Multi-Column Output

The `print_details` function demonstrates displaying quantities in original, alternative imperial, and SI units:

```cpp
--8<-- "example/foot_pound_second.cpp:75:91"
```

**Sample Output:**

```text
KMS Bismark, defined in appropriate units from the SI system
length               :              823.49 ft,           274.50 yd,            251.00 m
draft                :               30.51 ft,            10.17 yd,              9.30 m
beam                 :              118.11 ft,            39.37 yd,             36.00 m
mass                 :        110892517.88 lb,          49505.59 t,          50300.00 t
speed                :             51.04 ft/s,            30.24 kn,          56.00 km/h
power                :    2621018.31 ft pdl/s,        148.12 hp(I),           110.45 kW
main guns            :               14.96 in,            14.96 in,           380.00 mm
fire shells weighing :             1763.70 lb,              0.79 t,           800.00 kg
fire shells at       :           2690.29 ft/s,        1834.29 mi/h,        2952.00 km/h
volume underwater    :         1777123.68 ft³,         50322.54 m³,       50322538.73 L
```

## Why This Matters

- **Legacy Data**: Integrate historical or international data expressed in various unit systems
- **Industry Standards**: Naval, aerospace, and construction industries often use imperial/FPS units
- **International Collaboration**: Teams across different countries can work with their native units
- **No Manual Conversions**: The library handles all conversions automatically with full type safety

This capability is essential for applications that must bridge different engineering traditions
or work with legacy specifications while maintaining modern safety standards.
