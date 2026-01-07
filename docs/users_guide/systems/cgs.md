# Centimetre-Gram-Second (CGS) System

## Overview

The **Centimetre-Gram-Second (CGS)** system is a metric system of physical units based on
the centimetre, gram, and second as base units. It was widely used before the adoption of
the SI system and is still employed in certain scientific fields, particularly in theoretical
physics, astronomy, and some areas of chemistry.


## Historical Context

The CGS system was introduced in the 1870s and became the dominant metric system in science
for many decades. While it has been largely superseded by the SI for most applications, it
remains popular in:

- **Astrophysics and cosmology**: Many astronomical calculations and constants
- **Electrodynamics**: Gaussian and electrostatic CGS variants
- **Plasma physics**: Natural unit scales align with CGS
- **Physical chemistry**: Some molecular and atomic calculations


## Base Units

The CGS system uses three base units, all derived from SI units in **mp-units**:

```cpp
namespace mp_units::cgs {

inline constexpr auto centimetre = si::centi<si::metre>;   // 10⁻² m
inline constexpr auto gram = si::gram;                     // 10⁻³ kg
inline constexpr auto second = si::second;                 // 1 s

}
```

| Base Quantity | CGS Unit   | Symbol | Relation to SI |
|---------------|------------|--------|----------------|
| length        | centimetre | cm     | 1 cm = 0.01 m  |
| mass          | gram       | g      | 1 g = 0.001 kg |
| time          | second     | s      | 1 s = 1 s      |

!!! note "Base Unit: Gram vs Kilogram"

    Unlike SI, where kilogram is the base unit, CGS uses gram as its base mass unit.
    This makes the CGS system more consistent with its length scale (centimetre) for
    everyday laboratory measurements.


## Derived Units

The CGS system defines several derived units with special names, particularly in mechanics:

| Quantity            | CGS Unit | Symbol | Definition | SI Equivalent |
|---------------------|----------|--------|------------|---------------|
| acceleration        | gal      | Gal    | cm/s²      | 0.01 m/s²     |
| force               | dyne     | dyn    | g·cm/s²    | 10⁻⁵ N        |
| energy/work         | erg      | erg    | dyn·cm     | 10⁻⁷ J        |
| pressure            | barye    | Ba     | dyn/cm²    | 0.1 Pa        |
| dynamic viscosity   | poise    | P      | g/(cm·s)   | 0.1 Pa·s      |
| kinematic viscosity | stokes   | St     | cm²/s      | 10⁻⁴ m²/s     |
| wavenumber          | kayser   | K      | cm⁻¹       | 100 m⁻¹       |


```cpp
namespace mp_units::cgs {

inline constexpr struct gal final : named_unit<"Gal", centimetre / square(second)> {} gal;
inline constexpr struct dyne final : named_unit<"dyn", gram * centimetre / square(second)> {} dyne;
inline constexpr struct erg final : named_unit<"erg", dyne * centimetre> {} erg;
inline constexpr struct barye final : named_unit<"Ba", gram / (centimetre * square(second))> {} barye;
inline constexpr struct poise final : named_unit<"P", gram / (centimetre * second)> {} poise;
inline constexpr struct stokes final : named_unit<"St", square(centimetre) / second> {} stokes;
inline constexpr struct kayser final : named_unit<"K", one / centimetre> {} kayser;

}
```

## Unit Symbols

Convenient unit symbols are provided for common usage:

```cpp
namespace mp_units::cgs::unit_symbols {

inline constexpr auto cm = centimetre;
inline constexpr auto g = gram;
inline constexpr auto s = second;
inline constexpr auto Gal = gal;
inline constexpr auto dyn = dyne;
inline constexpr auto Ba = barye;
inline constexpr auto P = poise;
inline constexpr auto St = stokes;
inline constexpr auto K = kayser;

// Commonly used squared and cubic units
inline constexpr auto cm2 = square(centimetre);
inline constexpr auto cm3 = cubic(centimetre);
inline constexpr auto s2 = square(second);
inline constexpr auto s3 = cubic(second);

}
```

## Usage Examples

### Basic Mechanics

```cpp
using namespace mp_units;
using namespace mp_units::cgs::unit_symbols;

// Force calculation
quantity mass = 500 * g;
quantity acceleration = 980 * cm / s2;
quantity force = mass * acceleration;
std::cout << force.in(dyn) << "\n";      // 490000 dyn

// Energy calculation
quantity distance = 10 * cm;
quantity work = force * distance;
std::cout << work.in(cgs::erg) << "\n";  // 4900000 erg
```

### Gravitational Acceleration

The gal unit is named after Galileo and is commonly used in gravimetry:

```cpp
using namespace mp_units;
using namespace mp_units::cgs::unit_symbols;

// Standard gravity in CGS
quantity g_earth = 980.665 * Gal;                 // 980.665 cm/s²

// Gravity variation measurements (microgal precision)
quantity gravity_anomaly = 50 * si::micro<Gal>;   // 50 μGal
```

### Viscosity Measurements

```cpp
using namespace mp_units;
using namespace mp_units::cgs::unit_symbols;

// Dynamic viscosity (poise)
quantity water_viscosity = 0.01 * P;          // Water at 20°C: 0.01 P = 1 cP

// Kinematic viscosity (stokes)
quantity water_kin_viscosity = 0.01 * St;     // Water at 20°C: 0.01 St = 1 cSt
```

### Spectroscopy

The kayser (reciprocal centimetre) is used in spectroscopy for wavenumbers:

```cpp
using namespace mp_units;
using namespace mp_units::cgs::unit_symbols;

// Wavenumber of a spectral line
quantity wavenumber = 15000 * K;                           // 15000 cm⁻¹

// Convert to wavelength
quantity wavelength = 1. / wavenumber;                     
std::cout << wavelength.in(si::nano<si::metre>) << "\n";   // 666.667 nm
```

## Converting Between CGS and SI

**mp-units** handles conversions automatically:

```cpp
using namespace mp_units;

// CGS to SI
quantity force_cgs = 1000. * cgs::dyne;
quantity force_si = force_cgs.in(si::newton);              // 0.00001 N

quantity energy_cgs = 1e7 * cgs::erg;
quantity energy_si = energy_cgs.in(si::joule);             // 1 J

// SI to CGS
quantity force_newton = 1 * si::newton;
quantity force_dyne = force_newton.in(cgs::dyne);          // 100000 dyn

quantity pressure_pascal = 100 * si::pascal;
quantity pressure_barye = pressure_pascal.in(cgs::barye);  // 1000 Ba
```

### Mixed Calculations

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Start with SI
quantity velocity_si = 10 * m / s;

// Convert to CGS for calculation
quantity velocity_cgs = velocity_si.in(cgs::centimetre / si::second);  // 1000 cm/s

// Work with CGS units
quantity mass_cgs = 50 * cgs::gram;
quantity momentum = mass_cgs * velocity_cgs;               // 50000 g·cm/s

// Convert result back to SI
quantity momentum_si = momentum.in<double>(kg * m / s);    // 0.5 kg·m/s
```


## CGS Variants

!!! note "Electromagnetic Units"

    The CGS system has several variants for electromagnetic quantities:
    
    - **Gaussian CGS**: Uses statcoulomb and statvolt (no separate electrical base unit)
    - **ESU (Electrostatic)**: Based on Coulomb's law
    - **EMU (Electromagnetic)**: Based on Ampère's force law
    
    The **mp-units** library currently provides only the mechanical CGS units shown above.
    Electromagnetic CGS variants may be added in future versions.

## Advantages and Limitations

### Advantages

- **Natural scale**: CGS units align well with laboratory measurements
- **Simpler formulas**: Some physics formulas are simpler (e.g., ε₀ = μ₀ = 1 in Gaussian CGS)
- **Historical compatibility**: Allows working with older scientific literature
- **Convenient magnitudes**: Many atomic and molecular quantities are order-of-unity

### Limitations

- **Not standard**: Largely replaced by SI in modern usage
- **Electromagnetic confusion**: Multiple incompatible variants for E&M quantities
- **Limited adoption**: Fewer tools and standards support CGS

!!! note "Conversion Safety with mp-units"

    While manual CGS-to-SI conversions historically required careful tracking of powers
    of 10 (e.g., 1 erg = 10⁻⁷ J, 1 dyne = 10⁻⁵ N), **mp-units** handles all conversions
    automatically and safely. The library ensures dimensional correctness and applies
    the correct conversion factors, eliminating this traditional source of errors.

## When to Use CGS

Consider using CGS units when:

- Working with astrophysical calculations where CGS is standard
- Reading or reproducing results from older scientific papers
- Interfacing with legacy code or data in CGS units
- Working in fields where CGS remains conventional (e.g., plasma physics)

For general engineering and most modern scientific work, [SI units](si.md) are recommended.

## References

- [CGS Units on Wikipedia](https://en.wikipedia.org/wiki/Centimetre%E2%80%93gram%E2%80%93second_system_of_units)
- [International System of Units (SI)](si.md)
- [Systems of Units](../framework_basics/systems_of_units.md) - Framework basics
