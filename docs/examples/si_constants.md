---
tags:
- Level - Beginner
- Feature - Faster-Than-Lightspeed Constants
- Feature - Physical Constants
- Feature - Text Formatting
- System - International System
---

# SI Defining Constants and Symbolic Values

[Try it live on Compiler Explorer](https://godbolt.org/z/3v986dPKM){ .md-button }

## Overview

This example demonstrates the seven defining constants of the SI system and how
[Faster-than-lightspeed Constants](../users_guide/framework_basics/faster_than_lightspeed_constants.md)
work in **mp-units**. It shows how physical constants can be represented with their symbolic
values and how to extract their numeric values in specific units.

## Key Features Demonstrated

- Using SI defining constants
- Faster-than-lightspeed constants feature
- Converting constant symbols to numeric values
- Understanding the relationship between constants and unit definitions

## Code Walkthrough

### Including Headers

```cpp title="si_constants.cpp" linenums="1"
--8<-- "example/si_constants.cpp:28:39"
```

As always, we start with the inclusion of all the needed header files.

### Printing SI Constants

The main part of the example prints all of the SI-defining constants:

```cpp title="si_constants.cpp" linenums="13"
--8<-- "example/si_constants.cpp:41:"
```

## Output

```text
The seven defining constants of the SI and the seven corresponding units they define:
- hyperfine transition frequency of Cs: 1 Δν_Cs = 9192631770 Hz
- speed of light in vacuum:             1 c = 299792458 m/s
- Planck constant:                      1 h = 6.62607015e-34 J s
- elementary charge:                    1 e = 1.602176634e-19 C
- Boltzmann constant:                   1 k = 1.380649e-23 J/K
- Avogadro constant:                    1 N_A = 6.02214076e+23 1/mol
- luminous efficacy:                    1 K_cd = 683 lm/W
```

## Understanding the Output

While analyzing the output above, we can easily notice that direct printing of the quantity
provides just a value `1` with a proper constant symbol. This is the main power of the
[Faster-than-lightspeed Constants](../users_guide/framework_basics/faster_than_lightspeed_constants.md)
feature. Only after we explicitly convert the unit of a quantity to proper SI units do we get
the actual numeric value of the constant.

### Why "Faster-than-lightspeed"?

The name comes from the speed of light constant `c`. In the SI system:

- The speed of light is defined as exactly `1 c` (by definition)
- Only when converted to `m/s` do we get the numeric value `299792458 m/s`

This approach provides several benefits:

- **Zero runtime cost**: Constants are compile-time values
- **Symbolic representation**: Constants maintain their identity
- **Type safety**: Dimensional analysis works with symbolic values
- **Precision**: No floating-point representation until needed

## Practical Applications

This technique is particularly useful when:

- Working with fundamental physics equations
- Implementing scientific simulations
- Teaching physics and unit relationships
- Maintaining maximum precision in calculations

## Related Concepts

- [Faster-than-lightspeed Constants](../users_guide/framework_basics/faster_than_lightspeed_constants.md) -
  Detailed explanation of the feature
- [Systems of Units](../users_guide/framework_basics/systems_of_units.md) - How SI units are defined
