---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- Feature - Physical Constants
- Feature - Faster-Than-Lightspeed Constants
- Feature - Unit Conversions
- Feature - Text Formatting
- Domain - Physics
---

# Spectroscopy Unit Conversions with Physical Constants

<!-- TODO Timeout on CE -->
<!-- [Try it live on Compiler Explorer](https://godbolt.org/z/MvG9MKbY7){ .md-button } -->

## Overview

In spectroscopy and related physics domains, _energy_, _frequency_, _wavelength_,
_wavenumber_, and _temperature_ are all interrelated through fundamental physical
constants. This example demonstrates using the 2019 SI-defining constants to convert
between these different representations of the same physical phenomenon.

## Key Concepts

### Physical Constants as Conversion Factors

The library provides exact values for SI-defining constants, which can be used as
conversion factors:

```cpp
--8<-- "example/spectroscopy_units.cpp:56:58"
```

These constants enable converting between different quantity kinds that are related
through physics equations.

### Related Quantity Conversions

Given any one of these quantities, you can calculate the others using fundamental
relationships. The example demonstrates all five starting points:

From **_energy_** (E):

```cpp
--8<-- "example/spectroscopy_units.cpp:83:86"
```

From **_wavenumber_** (ν̃):

```cpp
--8<-- "example/spectroscopy_units.cpp:87:90"
```

From **_frequency_** (ν):

```cpp
--8<-- "example/spectroscopy_units.cpp:91:94"
```

From **_temperature_** (T):

```cpp
--8<-- "example/spectroscopy_units.cpp:95:98"
```

From **_wavelength_** (λ):

```cpp
--8<-- "example/spectroscopy_units.cpp:99:102"
```

Each starting quantity produces a complete row in the conversion table, demonstrating the
physical relationships between these five interconnected quantities.

### Dual Output Tables

The example creates two formatted tables. The first shows the **symbolic representation**
with faster-than-lightspeed constants (h, c, k) preserved in the output:

```cpp
--8<-- "example/spectroscopy_units.cpp:60:68"
```

The second table uses `.in()` to convert to **standard spectroscopy units** for numeric
comparison:

```cpp
--8<-- "example/spectroscopy_units.cpp:70:79"
```

**Sample Output:**

```text
| Energy          | Wavenumber      | Frequency       | Temperature     | Wavelength      |
|-----------------|-----------------|-----------------|-----------------|-----------------|
| 1 eV            | 1 eV h⁻¹ c⁻¹    | 1 eV/h          | 1 eV/k          | 1 h c/eV        |
| 1 h c/cm        | 1 1/cm          | 1 c/cm          | 1 h c cm⁻¹ k⁻¹  | 1 cm            |
| 1 h THz         | 1 THz/c         | 1 THz           | 1 h THz/k       | 1 c/THz         |
| 1 K k           | 1 K k h⁻¹ c⁻¹   | 1 K k/h         | 1 K             | 1 h c K⁻¹ k⁻¹   |
| 1 h c/µm        | 1 1/µm          | 1 c/µm          | 1 h c µm⁻¹ k⁻¹  | 1 µm            |
| --------------- | --------------- | --------------- | --------------- | --------------- |
| 1 eV            | 8065.54 1/cm    | 241.799 THz     | 11604.5 K       | 1.23984 µm      |
| 0.000123984 eV  | 1 1/cm          | 0.0299792 THz   | 1.43878 K       | 10000 µm        |
| 0.00413567 eV   | 33.3564 1/cm    | 1 THz           | 47.9924 K       | 299.792 µm      |
| 8.61733e-05 eV  | 0.695035 1/cm   | 0.0208366 THz   | 1 K             | 14387.8 µm      |
| 1.23984 eV      | 10000 1/cm      | 299.792 THz     | 14387.8 K       | 1 µm            |
```

The symbolic table demonstrates how **faster-than-lightspeed constants** are preserved
in quantity representations, showing the underlying physics relationships directly.

## Why This Matters

- **Domain-Specific Conversions**: Different spectroscopy subfields prefer different units
  (IR uses _wavenumbers_, visible uses _wavelengths_)
- **Physical Accuracy**: Uses exact 2019 SI-defining constant values
- **Research Applications**: Essential for spectroscopy, quantum mechanics, thermodynamics
- **Unit Consistency**: Dimensional analysis ensures all conversions are physically correct

This pattern demonstrates how **mp-units** extends beyond simple unit conversions to enable
physics-based transformations between related but dimensionally different quantities.

## Related

This example is based on the unit conversion table from:
["Atomic and Molecular Spectroscopy: Basic Concepts and Applications"](http://cds.cern.ch/record/1481609/files/978-3-642-18018-7_BookBackMatter.pdf)
