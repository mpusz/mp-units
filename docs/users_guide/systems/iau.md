# IAU Astronomical Units

## Overview

The **International Astronomical Union (IAU)** has established standardized conversion constants
for astronomical measurements through formal resolutions. These provide consistent, well-defined
values for solar and planetary parameters used across astronomy and astrophysics.

The **mp-units** IAU system implements:

- **IAU Resolution B3 (2015)**: Nominal solar and planetary conversion constants, exact by
  definition
- **IAU Resolution B2 (2012)**: astronomical_unit = 149,597,870,700 m (from SI), exact by
  definition
- **CODATA 2018**: Measured values for the Newtonian gravitational constant G and derived
  _masses_

!!! tip "When to Use"

    - Use **`iau`** for professional astronomy/astrophysics calculations requiring IAU standard values
    - Use **`astronomy`** utilities (see tip at the end) for common astronomy units like light-years,
      jansky, or sidereal days
    - The two can be used together when needed

## Resolution B3: Nominal Values

IAU Resolution B3 (2015) defines **nominal conversion constants** that are exact by definition.
These are not meant to be the most accurate values of physical parameters, but rather fixed
reference values for consistent conversions.

!!! question "Why Nominal Values"

    As telescopes and instruments improve, measured values of astronomical parameters (like the
    _solar radius_) change slightly. To prevent scientific papers from becoming outdated when
    measurements improve, the IAU established fixed "nominal" values for conversions. Think of
    them as standardized scaling factors, similar to how the metre is defined by fixing the
    _speed of light_.

### Solar Nominal Values

All solar nominal constants have suffix `_N` and symbol suffix `ᴺ`:

```cpp
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::iau::unit_symbols;
using namespace mp_units::si::unit_symbols;

// Solar nominal radius: R☉ᴺ = 6.957 × 10⁸ m
quantity R_sun = 1. * R_SUN_N;
std::println("Solar radius: {} = {::N[.3e]}", R_sun, R_sun.in(m));

// Solar nominal irradiance (at 1 au): S☉ᴺ = 1361 W/m²
quantity solar_constant = 1. * S_SUN_N;
std::println("Solar irradiance: {} = {::N[.0f]}", solar_constant, solar_constant.in(W / m2));

// Solar nominal luminosity: L☉ᴺ = 3.828 × 10²⁶ W
quantity solar_power = 1. * L_SUN_N;
std::println("Solar luminosity: {} = {::N[.3e]}", solar_power, solar_power.in(W));

// Solar nominal effective temperature: Tₑff,☉ᴺ = 5772 K
quantity_point sun_temp = point<T_EFF_SUN_N>(1.);
std::println("Solar temperature: {} = {::N[.0f]}", 
             sun_temp.quantity_from_zero(), 
             sun_temp.quantity_from_zero().in(K));

// Solar nominal gravitational parameter: (GM)☉ᴺ = 1.3271244 × 10²⁰ m³/s²
quantity mu_sun = 1. * GM_SUN_N;
std::println("Solar GM: {} = {::N[.7e]}", mu_sun, mu_sun.in(m3 / s2));
```

```text
Solar radius: 1 R_☉ᴺ = 6.957e+08 m
Solar irradiance: 1 S_☉ᴺ = 1361 W/m²
Solar luminosity: 1 L_☉ᴺ = 3.828e+26 W
Solar temperature: 1 Tₑff,☉ᴺ = 5772 K
Solar GM: 1 (GM)_☉ᴺ = 1.3271244e+20 m³/s²
```

### Planetary Nominal Values

The IAU also defines nominal values for Earth and Jupiter:

```cpp
// Earth equatorial radius: R⊕ₑᴺ = 6.3781 × 10⁶ m
quantity earth_eq_radius = 1. * R_EARTH_E_N;
std::println("Earth equatorial radius: {} = {::N[.4e]}", earth_eq_radius, earth_eq_radius.in(m));

// Earth polar radius: R⊕ₚᴺ = 6.3568 × 10⁶ m
quantity earth_polar_radius = 1. * R_EARTH_P_N;
std::println("Earth polar radius: {} = {::N[.4e]}", earth_polar_radius, earth_polar_radius.in(m));

// Earth gravitational parameter: (GM)⊕ᴺ = 3.986004 × 10¹⁴ m³/s²
quantity mu_earth = 1. * GM_EARTH_N;
std::println("Earth GM: {::N[.6e]}", mu_earth.in(m3 / s2));

// Jupiter equatorial radius: R♃ₑᴺ = 7.1492 × 10⁷ m
quantity jupiter_eq_radius = 1. * R_JUP_E_N;
std::println("Jupiter equatorial radius: {} = {::N[.4e]}", jupiter_eq_radius, jupiter_eq_radius.in(m));

// Jupiter polar radius: R♃ₚᴺ = 6.6854 × 10⁷ m
quantity jupiter_polar_radius = 1. * R_JUP_P_N;
std::println("Jupiter polar radius: {} = {::N[.4e]}", jupiter_polar_radius, jupiter_polar_radius.in(m));

// Jupiter gravitational parameter: (GM)♃ᴺ = 1.2668653 × 10¹⁷ m³/s²
quantity mu_jupiter = 1. * GM_JUP_N;
std::println("Jupiter GM: {::N[.7e]}", mu_jupiter.in(m3 / s2));
```

```text
Earth equatorial radius: 1 R_⊕ₑᴺ = 6.3781e+06 m
Earth polar radius: 1 R_⊕ₚᴺ = 6.3568e+06 m
Earth GM: 3.986004e+14 m³/s²
Jupiter equatorial radius: 1 R_♃ₑᴺ = 7.1492e+07 m
Jupiter polar radius: 1 R_♃ₚᴺ = 6.6854e+07 m
Jupiter GM: 1.2668653e+17 m³/s²
```

!!! note "Exact by Definition"

    All nominal values are **exact**. They will never change, ensuring long-term stability
    of calculations and publications that use them.

## Resolution B2: Astronomical Unit

The astronomical unit (au) is defined by IAU Resolution B2 (2012) as:

```cpp
// astronomical_unit = 149,597,870,700 m (exact)
quantity distance_to_neptune = 30.1 * iau::astronomical_unit;
std::println("Neptune distance: {} = {::N[.3e]}", distance_to_neptune, distance_to_neptune.in(km));
```

```text
Neptune distance: 30.1 au = 4.503e+09 km
```

The parsec is derived from the astronomical unit:

```cpp
// parsec ≈ 648000/π au (by definition)
quantity<iau::parsec> distance = 1. * iau::parsec;
std::println("1 parsec = {::N[.4e]}", distance.in(km));
```

```text
1 parsec = 3.0857e+13 km
```

!!! info "SI Integration"

    The astronomical unit is also available in the SI system through `si::astronomical_unit`
    with symbol `au`. The IAU system uses `using si::astronomical_unit;` to make it available
    as `iau::astronomical_unit` while avoiding symbol collisions.

## CODATA 2018: Measured Constants

While nominal values are exact by definition, some physical constants must be measured.
These are provided in the `codata2018` inline namespace:

### Newtonian Constant of Gravitation

```cpp
using namespace mp_units::iau::codata2018;

// Newtonian constant of gravitation: G = 6.67430(15) × 10⁻¹¹ m³/(kg·s²)
quantity G_value = 1. * newtonian_constant_of_gravitation;
std::println("G = {::N[.5e]}", G_value.in(m3 / (kg * s2)));
```

```text
G = 6.67430e-11 m³ kg⁻¹ s⁻²
```

### Derived Masses

The solar, terrestrial, and jovian masses are derived from the nominal GM values and
CODATA G:

```cpp
// M☉ = (GM)☉ᴺ / G
quantity M_sun = 1. * solar_mass;
std::println("Solar mass: {::N[.4e]}", M_sun.in(kg));

// M⊕ = (GM)⊕ᴺ / G  
quantity M_earth = 1. * terrestrial_mass;
std::println("Earth mass: {::N[.4e]}", M_earth.in(kg));

// M♃ = (GM)♃ᴺ / G
quantity M_jupiter = 1. * jovian_mass;
std::println("Jupiter mass: {::N[.4e]}", M_jupiter.in(kg));
```

```text
Solar mass: 1.9884e+30 kg
Earth mass: 5.9722e+24 kg
Jupiter mass: 1.8981e+27 kg
```

!!! info "Versioning Strategy"

    The `codata2018` inline namespace allows for future CODATA versions (e.g., `codata2022`)
    while maintaining backward compatibility. Nominal values remain in the main `iau` namespace
    since they never change.

## Symbol Collision Resolution

To avoid ambiguity when importing multiple `unit_symbols` namespaces, the IAU system uses
`using`-declarations:

```cpp
// In iau namespace:
using si::astronomical_unit;

// In iau::unit_symbols namespace:
using si::unit_symbols::au;
```

This ensures `au` always refers to the SI astronomical unit while making it accessible
through the IAU system.

## Practical Examples

### Calculating Orbital Period

Using Kepler's third law with IAU constants:

```cpp
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si.h>
#include <mp-units/math.h>
#include <numbers>

using namespace mp_units;
using namespace mp_units::iau::unit_symbols;
using namespace mp_units::si::unit_symbols;

// Earth's orbital period from semimajor axis
quantity a_earth = 1. * au;       // Earth's semimajor axis
quantity mu_sun = 1. * GM_SUN_N;  // Solar GM

// T² = 4π²a³ / μ
quantity T_squared = 4. * pow<2>(pi) * pow<3>(a_earth) / mu_sun;
quantity period = sqrt(T_squared);

std::println("Earth orbital period: {} = {::N[.2f]}", period, period.in(d));
```

```text
Earth orbital period: 2 au^(3/2) π/(GM)_☉ᴺ^(1/2) = 365.26 d
```

### Stellar Luminosity Comparison

```cpp
// A star twice as luminous as the Sun
quantity star_luminosity = 2. * L_SUN_N;

// Convert to watts
std::println("Star luminosity: {::N[.3e]}", star_luminosity.in(W));

// How many Suns?
std::println("Luminosity: {::N[.1f]}", star_luminosity.in(L_SUN_N));
```

```text
Star luminosity: 7.656e+26 W
Luminosity: 2.0 L_☉ᴺ
```

### Exoplanet Mass Calculation

```cpp
// Measured GM for an exoplanet system
quantity mu_exoplanet = 5.0e13 * m3 / s2;

// Calculate mass using CODATA G
quantity exoplanet_mass = mu_exoplanet / G;

// Express in Earth masses
std::println("Exoplanet mass: {::N[.2f]}", exoplanet_mass.in(M_EARTH));
```

```text
Exoplanet mass: 0.13 M_⊕
```

## References

- [IAU Resolution B3 (2015)](https://www.iau.org/Iau/Publications/List-of-Resolutions) -
  Nominal conversion constants for selected solar and planetary properties
- [IAU Resolution B2 (2012)](https://www.iau.org/Iau/Publications/List-of-Resolutions) -
  On the re-definition of the astronomical unit of length
- [CODATA 2018](https://physics.nist.gov/cuu/Constants/index.html) -
  Internationally recommended values of the fundamental physical constants

!!! tip "Astronomy Utilities"

    For commonly used astronomy units not standardized by the IAU (such as light-years, 
    jansky, sidereal days, Julian years, etc.), see the `astronomy` utility system:
    
    ```cpp
    #include <mp-units/systems/astronomy.h>
    
    using namespace mp_units::astronomy::unit_symbols;
    
    quantity distance = 4.22 * ly;        // light-year
    quantity flux = 1.5 * Jy;             // jansky (radio astronomy)
    quantity period = 1. * D_sid;         // sidereal day
    quantity age = 13.8e9 * a;            // Julian year
    ```
    
    These units can be freely mixed with IAU and SI units.
