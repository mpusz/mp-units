---
date: 2026-07-08
authors:
 - mpusz
categories:
 - HEP
 - Metrology
comments: true
---

# Bringing Safety to HEP

High-Energy Physics software is routinely trusted with complex detector
geometries spanning millions of volumes, critical tracking and reconstruction
algorithms, and multi-day simulations running on the Grid. A single silent unit
mismatch in a `double` can invalidate an entire analysis or produce detector
geometry that is 10× the wrong size. This post shows how **mp-units** can bring
compile-time safety to HEP codebases (including a dedicated HEP system of
quantities and units) and how large projects like ATLAS can adopt it
incrementally without requiring a big-bang rewrite.

<!-- more -->

!!! info "Related Material"

    This article is based on a talk **"Bringing Safety to HEP: HEP-Centric System of Quantities
    and Units with mp-units"** presented at the HSF Seminar at CERN on February 25, 2026.

    - **Recording**: [Watch on YouTube](https://www.youtube.com/watch?v=ROUiCy7rorU)
    - **Slides**: [GitHub repository](https://github.com/train-it-eu/conf-slides/tree/master/2026.02%20-%20HSF%20Seminar)

    The concepts discussed here are part of ongoing collaboration with the ATLAS Offline
    Software team and feed into the ISO C++ standardization effort for quantities and
    units ([P3045](https://wg21.link/p3045)).


## The Problem

### "Argument Soup"

The root cause is that everything in HEP code is a raw `double`. Consider the
Geant4 `G4Trap` constructor:

```cpp
G4Trap(const G4String& name,
       G4double pDz, G4double pTheta, G4double pPhi,
       G4double pDy1, G4double pDx1, G4double pDx2, G4double pAlp1,
       G4double pDy2, G4double pDx3, G4double pDx4, G4double pAlp2);
```

Eleven `G4double` parameters, some lengths, some angles. Swapping `pDx2` and
`pAlp1` compiles without a warning.

The ROOT `TGeoMedium` constructor is equally problematic:

```cpp
TGeoMedium(const char *name, Int_t numed, TGeoMaterial *mat,
           Double_t fieldm, Double_t tmaxfd, Double_t stemax,
           Double_t deemax, Double_t epsil, Double_t stmin);
```

Six `Double_t` parameters after the material:

- `fieldm`: magnetic field (Gauss)
- `tmaxfd`: angle (degrees)
- `stemax`: length (cm)
- `deemax`: energy ratio (dimensionless)
- `epsil`: (dimensionless)
- `stmin`: length (cm)

All silently convertible to each other!

The Geant4 particle definition takes 21 parameters, including `mass`, `width`,
`lifetime`, and `magneticMoment` all as `G4double`.

The real-world consequence:

```cpp
using namespace CLHEP;  // MeV, GeV are just 'constexpr double' scale factors

double mass  = 3.872 * GeV;  // rest energy
double width = 1.2 * MeV;    // decay width

// Expected order: G4ParticleDefinition(name, mass, width, charge, ...)
G4ParticleDefinition("X3872", width, mass, 0, /* ... */);  // mass/width swapped!
// Compiles fine (both are G4double): rest energy becomes 1.2 MeV instead of
// 3872 MeV (3000× wrong), and the decay width becomes absurdly large.
```

### The "Magic 1.0" Problem

Different HEP frameworks choose different internal base units:

| Quantity   | Geant4            | ROOT                 |
|------------|-------------------|----------------------|
| **Length** | `1.0` = **1 mm**  | `1.0` = **1 cm**     |
| **Time**   | `1.0` = **1 ns**  | `1.0` = **1 s**      |
| **Angle**  | `1.0` = **1 rad** | `1.0` = **1 degree** |
| **Energy** | `1.0` = **1 MeV** | `1.0` = **1 GeV**    |

Bridge code between frameworks requires manual scaling factors. One missing
`/ CLHEP::cm` makes your detector geometry 10× smaller, at runtime, silently:

```cpp
double radius = rootVolume->GetRmax();  // Returns 10.0, meaning 10 cm
G4Tubs g4Tube("Tube", 0, radius, ...);  // Geant4 interprets as 10 mm!
```

### The CODATA Drift Problem

Physical constants change as measurements improve. Different frameworks are
pinned to different CODATA recommendations:

| Framework         | CODATA Version | Electron Mass (MeV/c²) |
|-------------------|:--------------:|------------------------|
| **CLHEP / Gaudi** |      2014      | 0.5109989461           |
| **Geant4**        |      2018      | 0.51099895000          |
| **ROOT (latest)** |      2022      | 0.51099895069          |

Small differences accumulate in precision calculations, and updates are tied to
framework release cycles.

As noted by John Chapman (currently one of the ATLAS Offline Software coordinators):

!!! quote "HEP community feedback"

    "I would like to find a way to avoid this. If they all depended on a single external
    library which we could easily configure to use a particular set of recommendations,
    then that would make things easier."

### Why Training Alone Will Not Solve This

ATLAS Athena alone has **2.5 million lines of code**, much of it written by
physicists rather than software engineers. Silent bugs from `double`
proliferation are structural: the compiler simply cannot help when every
physical quantity, regardless of its dimension, is represented the same way.

### Historical Context: Walter Brown's SI Library (1998)

The need for compile-time dimensional analysis in HEP was recognized early.
Walter Brown presented ["SI Library of Unit-Based Computation"](https://digital.library.unt.edu/ark:/67531/metadc668099)
at CHEP '98 (International Conference in High Energy Physics, Chicago, IL,
August 31 - September 4, 1998), **the first systematic approach to
compile-time dimensional analysis in C++**. Nearly three decades later, the
fundamental problems remain: most HEP code still uses raw `double` values,
making dimensional errors invisible to the compiler.

---

## The mp-units HEP System

**mp-units** is a Modern C++ (C++20) library for physical quantities and units
that provides compile-time dimensional safety with zero runtime overhead. It was
developed in collaboration with the ATLAS Offline Software team at CERN to
address the problems described above, and it serves as the reference
implementation for the ISO C++ standardization effort ([P3045](https://wg21.link/p3045),
targeting C++29).

### A Dedicated System of Quantities

HEP physics does not naturally fit the International System of Quantities
(ISQ). The ISQ takes _mass_ as a base dimension. Particle physicists think in
_energy_ (the _electron mass_ is 0.511 MeV). The ISQ takes _electric current_ as
base. Particle physics counts discrete _charge_ quanta. The **mp-units** HEP
system reflects this:

| Base Quantity                   | Dim |        vs. ISQ?        |
|---------------------------------|:---:|:----------------------:|
| **_length_**                    |  L  |         ✓ same         |
| **_time_**                      |  T  |         ✓ same         |
| **_energy_**                    |  E  |  ✗ ISQ uses _mass_ M   |
| **_electric charge_**           |  Q  | ✗ ISQ uses _current_ I |
| **_thermodynamic temperature_** |  Θ  |         ✓ same         |
| **_amount of substance_**       |  N  |         ✓ same         |
| **_luminous intensity_**        |  I  |         ✓ same         |
| **_angle_**                     |  α  | ✗ dimensionless in ISQ |

The two systems are **mutually incompatible by design**, mixing ISQ and HEP
quantities is a compile error:

```cpp
quantity<isq::length[si::metre]>  L_isq = 1. * si::metre;
quantity<hep::length[hep::metre]> L_hep = 1. * hep::metre;

// auto bad = L_isq + L_hep;  // Compile error: different dimensional systems ✓
```

The HEP system is implemented using **mp-units**' core abstractions:

```cpp
namespace mp_units::hep {
using namespace ::mp_units::angular;

// Base dimensions
inline constexpr struct dim_length : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_time : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_charge : base_dimension<"Q"> {} dim_electric_charge;
inline constexpr struct dim_energy : base_dimension<"E"> {} dim_energy;
inline constexpr struct dim_temperature : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_temperature;
inline constexpr struct dim_amount_of_substance : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity : base_dimension<"I"> {} dim_luminous_intensity;

// Base quantities
inline constexpr struct length : quantity_spec<dim_length, non_negative> {} length;
inline constexpr struct duration : quantity_spec<dim_time, non_negative> {} duration;
inline constexpr struct electric_charge : quantity_spec<dim_electric_charge> {} electric_charge;
inline constexpr struct energy : quantity_spec<dim_energy, non_negative> {} energy;
inline constexpr struct temperature : quantity_spec<dim_temperature, non_negative> {} temperature;
inline constexpr struct amount_of_substance : quantity_spec<dim_amount_of_substance, non_negative> {} amount_of_substance;
inline constexpr struct luminous_intensity : quantity_spec<dim_luminous_intensity, non_negative> {} luminous_intensity;
}
```

### Commonly Used Units in HEP

The HEP system provides units that match the conventions used by CLHEP, Gaudi,
and Geant4, the majority of the CERN ecosystem. **Unlike some libraries,
mp-units does not force conversion to privileged "base units"**, quantities can
natively store values in any unit without automatic conversion. The units listed
below simply reflect common HEP practice for numerical convenience:

| Quantity              | Typical Unit      | Symbol | Why commonly used?                               |
|-----------------------|-------------------|:------:|--------------------------------------------------|
| **_Length_**          | millimetre        |   mm   | Detector geometry is mm-scale                    |
| **_Time_**            | nanosecond        |   ns   | Particle lifetimes and timing are ns-scale       |
| **_Energy_**          | megaelectronvolt  |  MeV   | MeV covers the full range MeV–TeV comfortably    |
| **_Electric Charge_** | elementary charge | eplus  | Charge is quantized; quanta are the natural unit |

_Temperature_ (K), _amount_ (mol), _luminosity_ (cd), and _angle_ (rad) follow SI.

You are free to use any units you prefer. For example, `quantity<hep::length[hep::metre]>`
and `quantity<hep::length[hep::millimetre]>` work equally well, with automatic conversions
when needed.

The unit definitions:

```cpp
inline constexpr struct meter : named_unit<"m", kind_of<length>> {} meter;
inline constexpr struct second : named_unit<"s", kind_of<duration>> {} second;
inline constexpr struct eplus : named_unit<symbol_text{u8"e⁺", "e+"}, kind_of<electric_charge>> {} eplus;
inline constexpr struct electronvolt : named_unit<"eV", kind_of<energy>> {} electronvolt;

// SI units
inline constexpr struct coulomb : named_unit<"C", mag<6'241'509'074> * mag_power<10, 9> * eplus> {} coulomb;
inline constexpr struct joule : named_unit<"J", electronvolt * coulomb / eplus> {} joule;
inline constexpr struct gram : named_unit<"g", mag_ratio<1, 1000> * joule * square(second) / square(meter)> {} gram;

// ...

namespace unit_symbols {
inline constexpr auto m = meter;
inline constexpr auto mm = si::milli<meter>;
inline constexpr auto ns = si::nano<second>;
inline constexpr auto eV = electronvolt;
inline constexpr auto MeV = si::mega<electronvolt>;
inline constexpr auto GeV = si::giga<electronvolt>;
// ...
}
```

### Derived Quantities

With _energy_ as a base, _mass_, _momentum_, and other familiar quantities are derived:

- **_Mass_**: M = E·T²/L² (from $E = mc^2$ where $c = L/T$)
- **_Momentum_**: p = E·T/L
- **_Force_**: F = E/L
- **_Electric Current_**: I = Q/T
- **_Electric Potential_**: V = E/Q

The _speed of light_ `c` is a **named-constant unit**, not a floating-point
value. It stays symbolic in expressions and cancels in the type system, giving
natural-unit semantics with full dimensional safety:

```cpp
using namespace mp_units::hep::unit_symbols;

quantity p    = 4. * GeV / c;    // type: quantity<GeV/c>   (c stays symbolic)
quantity mass = 3. * GeV / c2;   // type: quantity<GeV/c²>  (c² stays symbolic)
```

We put this to work below in
[Faster-than-Lightspeed Constants](#faster-than-lightspeed-constants).

### Specialized Quantities

Beyond basic dimensional safety, the HEP system provides **named sub-quantities**
for physically distinct concepts that share the same dimension. The full catalogue
includes:

- **Energy**: `total_energy`, `kinetic_energy`, `rest_mass_energy`,
  `binding_energy`, `excitation_energy`, `ionization_energy`,
  `threshold_energy`, `Q_value`, `transverse_energy`, `missing_energy`, …
- **Length**: `path_length`, `displacement`, `decay_length`,
  `radiation_length`, `interaction_length`, `impact_parameter`,
  `wavelength`, `range`, …
- **Time**: `proper_time`, `coordinate_time`, `mean_lifetime`, `half_life`,
  `time_of_flight`
- **Mass**: `rest_mass`, `invariant_mass`, `effective_mass`, `reduced_mass`
- **Momentum**: `momentum`, `transverse_momentum`
- **Angle**: `scattering_angle`, `opening_angle`, `azimuthal_angle`,
  `polar_angle`
- **Dimensionless**: `lorentz_factor` (γ), `relativistic_beta` (β), `phase`
- **Interaction**: `cross_section` (σ), `number_density` (n),
  `decay_constant` (λ)

For _energy_, the sub-quantity hierarchy encodes the physics directly: adding
_kinetic energy_ and _rest-mass energy_ yields their common parent,
_total energy_:

```cpp
using namespace mp_units::hep::unit_symbols;

quantity KE = hep::kinetic_energy(100. * GeV);
quantity E0 = hep::rest_mass_energy(938.27 * MeV);  // proton

quantity E_total = KE + E0;  // quantity<hep::total_energy[...]>

// The result cannot be silently narrowed back to a child type:
// quantity<hep::kinetic_energy[MeV]>   bad1 = E_total;  // Compile error!
// quantity<hep::rest_mass_energy[MeV]> bad2 = E_total;  // Compile error!
```

### Distinct Quantity Subkinds

Some quantities are physically incompatible even though they share the same
dimension, unit, *and* parent quantity. The HEP system uses the `is_kind`
specifier to make mixing them a compile error.

**_Relativistic time_.** `proper_time` (τ, Lorentz-invariant) and
`coordinate_time` (t, frame-dependent) are related by the _Lorentz factor_, not
by addition:

```cpp
quantity tau   = hep::proper_time(0.385 * ns);    // B⁰ meson rest-frame lifetime
quantity t_lab = hep::coordinate_time(4.2 * ns);  // observed lab time of flight

// auto wrong = tau + t_lab;  // Compile error: different kinds ✓

quantity gamma = hep::lorentz_factor(t_lab / tau);  // γ ≈ 10.9 ✓
```

**Material _lengths_.** `radiation_length` (X₀) and `interaction_length` (λ_I)
are material properties that appear as denominators, never as addends with
geometric _lengths_:

```cpp
quantity X0       = hep::radiation_length(88.97 * mm);   // lead X₀
quantity lambda_I = hep::interaction_length(194.4 * mm); // lead λ_I
quantity path     = hep::path_length(50. * mm);

// auto bad1 = X0 + lambda_I;  // Compile error ✓
// auto bad2 = X0 + path;      // Compile error ✓

quantity n_X0 = path / X0;  // number of radiation lengths traversed ✓
```

**Dimensionless relativistic quantities.** `lorentz_factor` (γ),
`relativistic_beta` (β), and `phase` (φ) are all dimensionless but physically
incompatible:

```cpp
quantity gamma = hep::lorentz_factor(10. * one);
quantity beta  = hep::relativistic_beta(0.995 * one);
quantity phi   = hep::phase(1.57 * one);

// auto bad1 = gamma + beta;  // Compile error ✓
// auto bad2 = gamma + 1.;    // Compile error: not a plain dimensionless ✓
```

---

## Six Levels of Safety

The protections that **mp-units** provides can be grouped into six levels, each
catching a distinct class of bug.

!!! info "The six levels in general"

    This section applies each level to HEP. For the general treatment, plus a
    feature-by-feature comparison of **mp-units** against Boost.Units, nholthaus,
    SI, and Au, see [Understanding Safety Levels](understanding-safety-levels.md).

### Level 1: Dimension Safety

Prevents mixing incompatible physical dimensions:

```cpp
using namespace mp_units::hep::unit_symbols;

quantity<mm / ns> average_speed(quantity<mm> distance, quantity<ns> time);

quantity bunch_time   = 25 * ns;
quantity track_length = 120 * mm;

quantity speed = average_speed(track_length, bunch_time);  // ✅

// average_speed(bunch_time, track_length);  // ❌ args swapped
// average_speed(track_length, 45 * MeV);    // ❌ energy ≠ time
```

### Level 2: Unit Safety

Automatic, zero-cost unit conversions, no manual scaling factors required:

```cpp
quantity g4_radius   = 150. * mm;           // Geant4: mm
quantity root_radius = g4_radius.in(cm);    // ROOT:   cm

assert(g4_radius == root_radius);           // ✅ same physical value

double g4_val   = g4_radius.numerical_value_in(mm);   // 150.0
double root_val = root_radius.numerical_value_in(cm); //  15.0
```

The ROOT / Geant4 bridge bugs described earlier become impossible: the compiler
handles conversions, and when units already match, the conversion optimizes away
entirely.

### Level 3: Representation Safety

Prevents truncation and precision loss:

```cpp
quantity<one, std::uint16_t> raw_adc = 3500;             // 12-bit ADC

constexpr quantity adc_to_voltage = 2500.0 * mV / 4096;
constexpr quantity energy_scale   = 45.0 * MeV / V;
quantity voltage = raw_adc * adc_to_voltage;             // auto-upgrades to double
quantity energy  = voltage * energy_scale;               // ~96 MeV

// quantity<mV, std::uint16_t> v_int = voltage;          // ❌ Compile error: narrowing
quantity<mV, std::uint16_t> v_int = voltage.force_in<std::uint16_t>(mV);  // ✅ explicit
```

!!! tip "Runtime overflow protection with `safe_int`"

    For runtime arithmetic overflow detection, **mp-units** provides a built-in `safe_int<T>`
    wrapper that can be used as the representation type:

    ```cpp
    #include <mp-units/safe_int.h>

    quantity<m, safe_i16> distance = 100 * m;
    quantity<m, safe_i16> overflow = distance * 1'000;  // throws std::overflow_error
    ```

    See [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md)
    for more details on runtime safety infrastructure.

### Level 4: Quantity Kind Safety

Prevents arithmetic on quantities that share a dimension but have different
physical meanings:

```cpp
quantity<hep::lorentz_factor[one]>
calculate_lorentz_factor(quantity<hep::coordinate_time[ns]> t_lab,
                         quantity<hep::proper_time[ns]>     tau);

quantity tau   = hep::proper_time(0.385 * ns);
quantity t_lab = hep::coordinate_time(4.2 * ns);

quantity gamma = calculate_lorentz_factor(t_lab, tau);  // ✅
// calculate_lorentz_factor(tau, t_lab);                // ❌ args swapped: kind mismatch
// auto sum = tau + t_lab;                              // ❌ different kinds
```

### Level 5: Quantity Safety

Enforces quantity hierarchies and equation relationships. Beyond the energy
hierarchy shown earlier, this level can turn the 11-`double` soup into a
self-documenting API.

Consider the `G4Trap` constructor again. With mp-units, each geometric axis and
angle role gets its own sub-quantity:

```cpp
inline constexpr struct trap_half_z      : quantity_spec<hep::length> {} trap_half_z;
inline constexpr struct trap_half_y      : quantity_spec<hep::length> {} trap_half_y;
inline constexpr struct trap_half_x      : quantity_spec<hep::length> {} trap_half_x;
inline constexpr struct trap_tilt_angle  : quantity_spec<hep::angle>  {} trap_tilt_angle;
inline constexpr struct trap_shear_angle : quantity_spec<hep::angle>  {} trap_shear_angle;
```

The function signature becomes self-documenting:

```cpp
void MakeG4Trap(quantity<trap_half_z[mm]>       pDz,
                quantity<trap_tilt_angle[rad]>  pTheta,
                quantity<trap_half_y[mm]>       pDy1,
                quantity<trap_half_x[mm]>       pDx1,
                quantity<trap_shear_angle[rad]> pAlp1,
                /* ... */);
```

Now the compiler prevents argument confusion:

```cpp
quantity pDz   = trap_half_z(150. * mm);
quantity pDy1  = trap_half_y(25. * mm);
quantity theta = trap_tilt_angle(0. * rad);

// MakeG4Trap(pDz, pDy1, ...);     // ❌ mm where rad expected
// MakeG4Trap(pDy1, theta, ...);   // ❌ pDy where pDz expected (both mm!)

// ✅ Lengths of different axes promote to common hep::length
quantity half_diag = pDz + pDy1;  // Safe for clearance calculations
static_assert(half_diag.quantity_spec == hep::length);
```

### Level 6: Mathematical Space Safety

Distinguishes absolute positions from displacements:

```cpp
quantity_point z_vertex = z_vertex_zero + 15.0 * mm;
quantity_point z_decay  = z_vertex_zero + 20.2 * mm;

quantity decay_length = z_decay - z_vertex;             // Point − Point = delta ✅
quantity_point z_next = z_vertex + decay_length;        // Point + delta = Point ✅

// auto bad = z_vertex + z_decay;  // ❌ Cannot add two absolute positions
```

---

## Beyond the Six Levels: Advanced Type Safety

### Generic Interfaces

**Unit-agnostic algorithms** preserve input units without forcing conversions,
enabling natural interop between frameworks:

```cpp
using namespace mp_units::hep::unit_symbols;

// Algorithm accepts any position unit (mm, cm, m, etc.)
QuantityPointOf<hep::position_vector> auto
decay_vertex_position(QuantityPointOf<hep::position_vector> auto production_z,
                      QuantityOf<hep::decay_length> auto decay_length,
                      QuantityOf<hep::angle> auto theta)
{
  return production_z + decay_length * hep::cos(theta);  // Returns position in same unit
}

// Call with mm (Geant4) → returns position in mm
quantity_point z_decay_g4 = decay_vertex_position(point<mm>(150), 25 * mm, 18 * hep::degree);

// Call with cm (ROOT) → returns position in cm
quantity_point z_decay_root = decay_vertex_position(point<cm>(15), 2.5 * cm, 0.314 * hep::radian);
```

**Benefits:** Zero conversion overhead, natural units preserved, type-safe.

### Strongly-Typed Dimensionless Quantities

Even dimensionless values can be distinguished by physical meaning:

```cpp
inline constexpr struct efficiency : quantity_spec<dimensionless, is_kind> {} efficiency;
inline constexpr struct branching_ratio : quantity_spec<dimensionless, is_kind> {} branching_ratio;
inline constexpr struct event_count : quantity_spec<dimensionless, is_kind> {} event_count;
inline constexpr struct yield : quantity_spec<dimensionless, is_kind> {} yield;

quantity<yield[one]> compute_yield(quantity<efficiency[one]> eff,
                                   quantity<event_count[one], int> n_events);

quantity trigger_eff = efficiency(0.95);
quantity selection_eff = efficiency(0.87);
quantity BR_Hgg = branching_ratio(2.27e-3);

// auto bad = trigger_eff + BR_Hgg;     // ❌ Cannot add incompatible kinds
// bool bad2 = trigger_eff > BR_Hgg;    // ❌ Cannot compare incompatible kinds

// ✅ Function enforces kind at call site
quantity res = compute_yield(trigger_eff, event_count(1'000'000));
// compute_yield(BR_Hgg, event_count(1'000'000));  // ❌ Compile error!
```

Also useful for: purity, asymmetries, χ²/ndf, ratios, confidence levels.

### Faster-than-Lightspeed Constants

`c` is a compile-time unit in **mp-units**, not a floating-point value, it stays
symbolic and multiplies at runtime only when an explicit conversion is performed
by the user:

```cpp
using namespace mp_units::hep::unit_symbols;

quantity p = 4. * GeV / c;      // type: quantity<GeV/c>   (c stays symbolic)
quantity mass = 3. * GeV / c2;  // type: quantity<GeV/c²>  (c² stays symbolic)

quantity E = hypot(p * c, mass * c2);   // E² = (pc)² + (mc²)²  →  4² + 3² = 25  →  E = 5 GeV

std::println("p = {} = {::N[.5g]}", p, p.in(kg * m / s));  // "p = 4 GeV/c = 2.1377e-18 m kg/s"
std::println("m = {} = {::N[.5g]}", mass, mass.in(kg));    // "m = 3 GeV/c² = 5.348e-27 kg"
std::println("E = {} = {::N[.5g]}", E, E.in(hep::joule));  // "E = 5 GeV = 8.0109e-10 J"
```

The `c` factors (and all other constants like `π`) cancel in the type system,
identical to working in natural units, with full dimensional safety.

---

## Type Safety in Practice

To make the protections concrete, consider this typical HEP API with a legacy
signature and its type-safe counterpart:

=== "Legacy API"

    ```cpp
    // All parameters documented in comments, unenforced
    void setParticleProperties(double mass_MeV_per_c2,
                               double width_MeV,
                               double charge);
    ```

=== "Type-safe API"

    ```cpp
    void setParticleProperties(quantity<mp::rest_mass[mp::MeV / mp::c2]> mass,
                               quantity<mp::energy[mp::MeV]>             width,
                               quantity<mp::electric_charge[mp::eplus]>  charge);
    ```

!!! info "What is `mp`?"

    The code above uses an `mp::` prefix for HEP quantities, units, and constants.
    `mp` is a short namespace alias you introduce yourself for `mp_units::hep` and
    `mp_units::hep::unit_symbols`. [Phase 1](#phase-1-integrate-mp-units) of the
    migration shows the setup, and you are free to pick any name that suits your
    project.

### Wrong Dimensions

=== "Legacy"

    ```cpp
    double higgs_mass  = 125.0 * GeV;  // actually stores 125'000 MeV, not mass!
    double higgs_width = 4.0 * MeV;
    setParticleProperties(higgs_mass, higgs_width, 0.0);
    // Passes 125'000 as mass_MeV_per_c2  ✓ Compiles  ☠️ Factor-1000 wrong  ❌ No warning
    ```

=== "mp-units"

    ```cpp
    quantity higgs_mass  = 125.0 * mp::GeV;  // compiler knows this is energy
    quantity higgs_width = 4.0 * mp::MeV;
    // setParticleProperties(higgs_mass, higgs_width, 0.0 * mp::eplus);        // ❌ Compile error: cannot convert energy to rest_mass
    setParticleProperties(higgs_mass / mp::c2, higgs_width, 0.0 * mp::eplus);  // ✅
    ```

### Arguments Swapped

=== "Legacy"

    ```cpp
    setParticleProperties(higgs_width, higgs_mass, 0.0);  // silently swapped
    // mass = 4 MeV/c² (off by 31'000×), width = 125'000 MeV  ✓ Compiles  ☠️  ❌
    ```

=== "mp-units"

    ```cpp
    // setParticleProperties(higgs_width, higgs_mass, 0.0 * mp::eplus);        // ❌ Compile error: arguments in wrong order (type mismatch)
    setParticleProperties(higgs_mass / mp::c2, higgs_width, 0.0 * mp::eplus);  // ✅
    ```

### Wrong Unit Scale (Framework Mismatch)

=== "Legacy"

    ```cpp
    double root_width = get_from_ROOT();  // Returns 0.004 (ROOT uses GeV=1)
    setParticleProperties(higgs_mass, root_width, 0.0);
    // API expects MeV, receives 0.004 → width = 0.004 MeV (off by 1000×)  ✓ Compiles  ☠️  ❌
    ```

=== "mp-units"

    ```cpp
    double root_width = get_from_ROOT();   // 0.004, bare double, no unit info
    // setParticleProperties(higgs_mass, root_width, 0.0 * mp::eplus);        // ❌ Compile error: cannot convert double to quantity, forces explicit unit attachment
    setParticleProperties(higgs_mass, root_width * mp::GeV, 0.0 * mp::eplus); // ✅  0.004 * GeV = 4 MeV, automatic conversion
    ```

### Wrong Quantity of the Same Kind

=== "Legacy"

    ```cpp
    double m_inv = compute_invariant_mass();  // event-by-event reconstruction
    setParticleProperties(m_inv, 4.0 * MeV, 0.0);
    // invariant_mass ≠ rest_mass, but both are MeV/c²  ✓ Compiles  ☠️ Wrong physics  ❌
    ```

=== "mp-units"

    ```cpp
    quantity<hep::invariant_mass[mp::GeV / mp::c2]> compute_invariant_mass();

    quantity m_inv = compute_invariant_mass();
    // setParticleProperties(m_inv, 4.0 * mp::MeV, 0.0 * mp::eplus);  // ❌ Compile error: no conversion from invariant_mass to rest_mass
    ```

---

## Physical Constants with CODATA Version Control

The **mp-units** HEP system organizes constants by CODATA release using nested
namespaces. The default (inline) namespace is CODATA 2018, matching Geant4,
CLHEP, and Gaudi:

```cpp
#include <mp-units/systems/hep.h>

using namespace mp_units;
using namespace mp_units::hep::unit_symbols;

// Default: CODATA 2018
quantity beta = 10. * MeV / (hep::electron_mass * c2);

// Matching ROOT (CODATA 2022)
quantity rest_energy_22 = hep::codata2022::electron_mass * c2;  // 0.51099895069 MeV/c²

// Reproducing legacy results (CODATA 2014)
quantity threshold = 2. * hep::codata2014::electron_mass * c2;  // pair production threshold
```

| Constant                       | CODATA 2014       | CODATA 2018               | CODATA 2022               |
|--------------------------------|-------------------|---------------------------|---------------------------|
| **Electron mass (MeV/c²)**     | 0.5109989461      | 0.51099895000             | 0.51099895069             |
| **Proton mass (MeV/c²)**       | 938.2720813       | 938.27208816              | 938.27208943              |
| **Neutron mass (MeV/c²)**      | 939.5654133       | 939.56542052              | 939.56542194              |
| **Fine structure constant**    | 7.2973525664×10⁻³ | 7.2973525693×10⁻³         | 7.2973525643×10⁻³         |
| **Boltzmann constant (MeV/K)** | 8.6173303×10⁻¹¹   | 8.617333262×10⁻¹¹ (exact) | 8.617333262×10⁻¹¹ (exact) |

Constants that are exact since the 2019 SI redefinition (`speed_of_light_in_vacuum`,
`planck_constant`, `elementary_charge`, `avogadro_constant`) are common to all
CODATA releases and live directly in the `hep` namespace.

---

## Interoperability and Migration

ATLAS Athena has 2.5 million lines of code. It cannot be rewritten overnight.
The migration strategy is evolutionary: inside-out, driven by normal development
rather than a big-bang rewrite.

### The Legacy Pattern

All major HEP frameworks store units as `constexpr double` scaling factors:

=== "CLHEP / Geant4"

    ```cpp
    namespace CLHEP {
      static constexpr double mm  = 1.;
      static constexpr double cm  = 10. * mm;   // = 10.0
      static constexpr double MeV = 1.;
      static constexpr double GeV = 1.e+3 * MeV; // = 1000.0
    }
    ```

=== "ROOT"

    ```cpp
    namespace TGeoUnit {
      static constexpr double cm  = 1.;       // base unit, different from CLHEP!
      static constexpr double mm  = 0.1;
      static constexpr double GeV = 1.;       // base unit, different from CLHEP!
      static constexpr double MeV = 1.e-3;
    }
    ```

The "multiply to set, divide to get" idiom is ubiquitous:

```cpp
double particle_energy = 50.0 * GeV;   // stores 50000.0 (base: MeV)
double track_length    = 2.5 * meter;  // stores 2500.0  (base: mm)

double val_in_gev = particle_energy / GeV;   // → 50.0
double val_in_cm  = track_length / cm;       // → 250.0

void processParticle(double energy_MeV, double length_mm);  // units in comments only!
processParticle(particle_energy, track_length);  // passes 50000.0, 2500.0
```

This pattern has four structural weaknesses (no dimensional safety, API
ambiguity, conversion confusion, and silent misunit bugs), none of which the
compiler can detect.

### Seven-Phase Migration

#### Phase 1: Integrate mp-units

Set up the namespace without touching any existing code:

```cpp
#include <mp-units/math.h>
#include <mp-units/systems/hep.h>

namespace CLHEP {
  using namespace mp_units;

  namespace mp {
    using namespace mp_units::hep;
    using namespace mp_units::hep::unit_symbols;
  }
}
```

Pick any namespace name that suits your project and does not collide with existing
identifiers. The block above shows `mp` inside `CLHEP`, but it can be at any scope.

#### Phase 2: Drop-in Replacements for Unit Constant Headers

Replace hand-maintained `constexpr double` constants with mp-units-derived
values. **All existing code continues to compile and produce identical
numerical results**: no other changes are needed at this stage.

=== "Before"

    ```cpp
    namespace CLHEP {
      static constexpr double mm  = 1.;
      static constexpr double cm  = 10. * mm;
      static constexpr double MeV = 1.;
      static constexpr double GeV = 1.e+3 * MeV;
    }
    ```

=== "After"

    ```cpp
    namespace CLHEP {
      constexpr double mm  = (1. * mp::mm).numerical_value_in(mp::mm);    // = 1.0
      constexpr double cm  = (1. * mp::cm).numerical_value_in(mp::mm);    // = 10.0
      constexpr double MeV = (1. * mp::MeV).numerical_value_in(mp::MeV);  // = 1.0
      constexpr double GeV = (1. * mp::GeV).numerical_value_in(mp::MeV);  // = 1000.0
    }
    ```

Existing `double energy = 50.0 * GeV;` compiles unchanged.

#### Phase 3: New Code Uses Typed Quantities from the Start

All newly written functions and classes use `quantity<...>` types directly.
Legacy code is untouched. New call sites get full type safety:

```cpp
quantity<mp::GeV / mp::c> calculate_momentum(quantity<mp::GeV> energy,
                                             quantity<mp::GeV / mp::c2> mass)
{
  return sqrt(pow<2>(energy) - pow<2>(mass * mp::c2)) / mp::c;
}

// Call site
quantity p = calculate_momentum(125. * mp::GeV, 4.18 * mp::GeV / mp::c2);  // ✅
// calculate_momentum(125. * mp::GeV, 4.18 * mp::GeV);                     // ❌ mass expected
// calculate_momentum(4.18 * mp::GeV / mp::c2, 125. * mp::GeV);            // ❌ arg order wrong
```

#### Phase 4: mp-units as a Vocabulary Type at Library Boundaries

Before touching your own classes, use **mp-units** quantities to *tag* values as they
cross between third-party framework APIs. Attaching the correct unit at the
boundary is exactly what eliminates the ROOT / Geant4 bridge bugs described
earlier, no manual scaling factors, no multiply-vs-divide guesswork:

```cpp
#include <TGeoVolume.h>  // ROOT geometry
#include <G4Tubs.hh>     // Geant4 geometry

using namespace mp_units::hep::unit_symbols;

// Tag the value with its actual unit: ROOT returns cm
quantity radius = rootVolume->GetRmax() * cm;  // 10 cm, unambiguous

// Convert to mm for Geant4 explicitly: the type system does the scaling
G4Tubs g4Tube("Tube", 0,
              radius.numerical_value_in(mm),          // 100.0 ✅
              halfLength.numerical_value_in(mm), 0, 2 * std::numbers::pi);
```

State the target unit at each boundary and the conversion is handled for you.
Existing legacy signatures stay untouched. Only the call sites become
self-documenting.

#### Phase 5: Wrap Existing Interfaces with Type-Safe Overloads

Add typed setter overloads to existing classes. The class internals (data
members, private implementation) are left completely unchanged. The new typed
overloads convert at the public boundary and delegate to the existing
`double`-based methods. New callers use the typed API; old callers with raw
`double`s still compile without modification.

!!! note

    Typed **getter** overloads cannot be added at this stage: C++ does not allow
    overloading on return type alone, so the existing `double` getters must remain
    unchanged. Only setters, which differ by parameter type, can be overloaded.

```cpp
class Particle {
  double m_mass{};    // Still raw double, internals untouched
  double m_width{};
  double m_charge{};
public:
  // Existing double-based interface, unchanged, still works:
  double getMass()   const { return m_mass; }
  double getWidth()  const { return m_width; }
  double getCharge() const { return m_charge; }

  void setMass(double mass_MeV_per_c2)  { m_mass   = mass_MeV_per_c2; }
  void setWidth(double width_MeV)       { m_width  = width_MeV; }
  void setCharge(double charge_eplus)   { m_charge = charge_eplus; }

  // New typed overloads, convert at the boundary, delegate to existing setters:
  void setMass(quantity<mp::rest_mass[mp::MeV / mp::c2]> mass)    { setMass(mass.numerical_value_in(mp::MeV / mp::c2)); }
  void setWidth(quantity<mp::energy[mp::MeV]> width)              { setWidth(width.numerical_value_in(mp::MeV)); }
  void setCharge(quantity<mp::electric_charge[mp::eplus]> charge) { setCharge(charge.numerical_value_in(mp::eplus)); }
};
```

#### Phase 6: Refactor Internals, Add Typed Getters, Deprecate `double` API

Store typed quantities as data members. Add typed getters with a `_qty` suffix
alongside the existing `double` getters, and deprecate the old `double` API:

```cpp
class Particle {
  quantity<mp::rest_mass[mp::MeV / mp::c2]>  m_mass{};
  quantity<mp::energy[mp::MeV]>              m_width{};
  quantity<mp::electric_charge[mp::eplus]>   m_charge{};
public:
  // New typed interface:
  quantity<mp::rest_mass[mp::MeV / mp::c2]> getMass_qty()   const { return m_mass; }
  quantity<mp::energy[mp::MeV]>             getWidth_qty()  const { return m_width; }
  quantity<mp::electric_charge[mp::eplus]>  getCharge_qty() const { return m_charge; }

  void setMass(quantity<mp::rest_mass[mp::MeV / mp::c2]> mass)    { m_mass   = mass; }
  void setWidth(quantity<mp::energy[mp::MeV]> width)              { m_width  = width; }
  void setCharge(quantity<mp::electric_charge[mp::eplus]> charge) { m_charge = charge; }

  // Old double interface, deprecated:
  [[deprecated("Use getMass_qty() instead")]]   double getMass()   const { return m_mass.numerical_value_in(mp::MeV / mp::c2); }
  [[deprecated("Use getWidth_qty() instead")]]  double getWidth()  const { return m_width.numerical_value_in(mp::MeV); }
  [[deprecated("Use getCharge_qty() instead")]] double getCharge() const { return m_charge.numerical_value_in(mp::eplus); }

  [[deprecated("Pass quantity instead")]] void setMass(double mass)      { setMass(mass * mp::MeV / mp::c2); }
  [[deprecated("Pass quantity instead")]] void setWidth(double width)    { setWidth(width * mp::MeV); }
  [[deprecated("Pass quantity instead")]] void setCharge(double charge)  { setCharge(charge * mp::eplus); }
};
```

!!! note

    Because C++ cannot overload on return type alone, typed getters must use
    different names. The `_qty` suffix enables scripted migration in the final phase:
    consumers can mechanically replace `getMass_qty()` → `getMass()`.

#### Phase 7: Complete the Migration, Remove `_qty` Suffix

Once all call sites use the typed API, remove the deprecated overloads and
rename the typed getters to their final names:

```cpp
class Particle {
  quantity<mp::rest_mass[mp::MeV / mp::c2]> m_mass{};
  quantity<mp::energy[mp::MeV]>             m_width{};
  quantity<mp::electric_charge[mp::eplus]>  m_charge{};
public:
  quantity<mp::rest_mass[mp::MeV / mp::c2]> getMass()   const { return m_mass; }
  quantity<mp::energy[mp::MeV]>             getWidth()  const { return m_width; }
  quantity<mp::electric_charge[mp::eplus]>  getCharge() const { return m_charge; }

  void setMass(quantity<mp::rest_mass[mp::MeV / mp::c2]> mass)    { m_mass   = mass; }
  void setWidth(quantity<mp::energy[mp::MeV]> width)              { m_width  = width; }
  void setCharge(quantity<mp::electric_charge[mp::eplus]> charge) { m_charge = charge; }
};
```

!!! tip

    This phase is a simple mechanical rename: `sed -i 's/_qty()/()/g'`. All the
    heavy lifting was done in Phase 6 when consumers switched to the typed API.

---

## Zero Runtime Overhead

A common concern is performance. The mp-units safety model is **entirely
compile-time**: quantities are thin wrappers around their numeric value. The
generated assembly is identical to hand-written `double` code:

=== "No units library"

    ```cpp
    double ttg_s(double d_m, double v_mps) { return d_m / v_mps; }
    ```

=== "mp-units"

    ```cpp
    quantity<s> ttg(quantity<m> d, quantity<m/s> v) { return d / v; }
    ```

Both compile to:

```asm
ttg(double, double):
  divsd xmm0, xmm1
  ret
```

Unit conversions that are compile-time-known also optimize away completely:
converting from mm to mm is a no-op. Converting mm to cm folds to a single
constant multiply that the compiler can further constant-fold if the value is
known.

---

## Standardization

**mp-units** is the reference implementation for ongoing ISO C++ proposals:

- [**P1935**](https://wg21.link/p1935): A C++ Approach to Physical Units
- [**P2980**](https://wg21.link/p2980): Motivation, scope, and plan for a
  quantities and units library
- [**P3045**](https://wg21.link/p3045): Quantities and units library
- [**P4185**](https://wg21.link/p4185): Completing the Mathematical Model for
  C++ Quantities and Units

The entire proposed scope of **P3045** has been **unanimously accepted by LEWG**
(the Library Evolution Working Group), with **mp-units** serving as the reference
implementation for the discussion, and it is now a **priority for C++29**.
Adopting **mp-units** today means investing in skills and idioms that
will become part of the standard library, and benefiting from broad community
review of the design along the way.

---

## Summary

| Problem                             | Status with raw `double` |  Status with mp-units   |
|-------------------------------------|:------------------------:|:-----------------------:|
| Wrong dimensions at call site       |    ❌ Runtime, if ever    |     ✅ Compile error     |
| Arguments swapped                   |    ❌ Runtime, if ever    |     ✅ Compile error     |
| Framework unit mismatch (10×/1000×) |    ❌ Runtime, if ever    |     ✅ Compile error     |
| Same-dimension quantity confusion   |    ❌ Runtime, if ever    |     ✅ Compile error     |
| CODATA version inconsistencies      |    ❌ Manual tracking     |  ✅ Namespace-selected   |
| Performance overhead                |            —             | ✅ Zero (same assembly)  |
| Migration of 2.5M-line codebases    |            —             | ✅ Incremental, 7 phases |

The compiler can protect scientific integrity, if we give it the information.
The **mp-units** HEP system provides exactly that information, built in.

---

## Further Reading

- [HEP System User Guide](../../users_guide/systems/hep.md)
- [HEP System Reference](../../reference/systems_reference/systems/hep.md)
- [Understanding Safety Levels](understanding-safety-levels.md)
- [mp-units on GitHub](https://github.com/mpusz/mp-units)
- [CODATA 2022 constants](https://physics.nist.gov/cuu/Constants/)
- [Particle Data Group](https://pdg.lbl.gov)
- ISO C++ paper [P3045](https://wg21.link/P3045): Quantities and units library
