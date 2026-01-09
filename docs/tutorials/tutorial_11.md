# Tutorial 11: Faster-than-Lightspeed Constants

Physical constants like standard gravity (g₀) or π often appear in calculations where
they multiply in one place and divide in another. Traditional libraries implement these
as constant values (e.g., `9.80665`), requiring runtime floating-point arithmetic even
when the constants mathematically cancel out.

This tutorial demonstrates how **mp-units** implements constants as compile-time units,
enabling automatic simplification when constants cancel, preserving exact arithmetic where
possible, and delaying expensive conversions until necessary.


## Problem statement

Consider rocket stage burn time calculations for mission planning and flight software.

Rocket engineers face a messy mix of units:

- **_Propellant mass_** in kilograms (kg)
- **_Specific impulse_ (Isp)** in seconds - the industry-standard efficiency metric
- **_Thrust_** often specified in **kilogram-force (kgf)** in legacy engine datasheets
- **_Burn time_** needs to be calculated from these parameters

The relationship between _thrust_ (F), _mass flow rate_ ($\dot{m}$), and
_specific impulse_ ($I_{sp}$) involves standard gravity ($g_0$):

$$F = \dot{m} \cdot I_{sp} \cdot g_0$$

Rearranging to find _burn time_ ($t_{burn} = m_{prop} / \dot{m}$), we get:

$$t_{burn} = \frac{m_{prop}}{\dot{m}} = \frac{m_{prop} \cdot I_{sp} \cdot g_0}{F}$$

Here's a traditional implementation with helper functions but no type safety:

```cpp
const double g0 = 9.80665;  // m/s² - magic number!

double mass_flow_rate_kg_s(double thrust_N, double isp_s) { return thrust_N / (isp_s * g0); }
double burn_time_from_flow_s(double mass_kg, double flow_rate_kg_s) { return mass_kg / flow_rate_kg_s; }

int main()
{
  double propellant_kg = 15000.0;
  double isp_s = 311.0;
  double thrust_kgf = 390000.0;  // Legacy engine spec in kgf!
  double thrust_N = thrust_kgf * g0;
  double flow_kg_s = mass_flow_rate_kg_s(thrust_N, isp_s);
  double time_s = burn_time_from_flow_s(propellant_kg, flow_kg_s);
  // Can you spot a missed optimization opportunity?
}
```

**Problems with this approach:**

1. **Missed optimization**: Converting `thrust_kgf * g0` to `thrust_N`, then dividing by
  `g0` in `mass_flow_rate_kg_s` — the `g0` factors cancel but we compute them anyway!
2. **No type safety**: Nothing prevents passing `thrust_kgf` directly to
  `mass_flow_rate_kg_s`, silently producing wrong results
3. **Hidden unit conversions**: kgf is kg × g₀ by definition, but this relationship is
  implicit in the code
4. **Manual burden**: Programmers must track which conversions are needed and which
  constants cancel
5. **Historic disasters**: Unit confusion (mixing lbf and N) has caused real mission
  failures like Mars Climate Orbiter

**Real-world scenario:**

Flight software for a rocket stage must:

- Read engine specifications with _thrust_ in kgf (e.g., legacy Russian engines like RD-180)
- Read _propellant mass_ from fuel tank sensors (in kg)
- Use published _Isp_ values (efficiency metric in seconds)
- Calculate _burn time_ to determine how long the stage will fire
- Handle mixed units safely, as some engines use kgf while others use N

The g₀ constant appears in:

- The definition of kgf (kilogram-force = kg × g₀)
- The _burn time_ formula (numerator has g₀)
- These should cancel perfectly, avoiding any 9.80665 multiplications


## Your task

Refactor the rocket burn time calculator to use **mp-units** with g₀ as a compile-time
constant unit. The helper functions are already provided with type-safe `QuantityOf`
constraints.

Complete the implementation by defining:

1. **standard_gravity** (g₀) — Define `standard_gravity` as a unit which represents
  9.80665 m/s²  as an exact rational 980'665/100'000 (or use predefined `si::standard_gravity`)
2. **kilogram_force** (kgf) — Define as a `named_unit` that embeds g₀: `kg × g₀`

With these definitions in place, observe how:

- **Type safety**: `QuantityOf` constraints prevent passing wrong unit types
- **Automatic optimization**: When thrust is in kgf, g₀ factors cancel at compile-time
  without runtime cost
- **Mixed units**: Both kgf (legacy) and N (modern) thrust specifications work seamlessly

```cpp
// ce-embed height=900 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/isq.h>
#include <iostream>

using namespace mp_units;

inline constexpr struct specific_impulse final :
    quantity_spec<isq::duration, isq::force / (isq::mass_flow_rate * isq::acceleration)> {} specific_impulse;

// TODO: Define standard gravity (g₀) as a unit (9.80665 m/s² = 980'665/100'000 m/s²)

// TODO: Define kilogram-force (kgf) as kg × g₀

inline constexpr Unit auto g0 = standard_gravity;
inline constexpr Unit auto kgf = kilogram_force;

QuantityOf<isq::mass_flow_rate> auto mass_flow_rate(QuantityOf<isq::force> auto thrust,
                                                    QuantityOf<specific_impulse> auto isp)
{
  return thrust / (isp * g0);
}

QuantityOf<isq::duration> auto burn_time_from_flow(QuantityOf<isq::mass> auto propellant_mass,
                                                   QuantityOf<isq::mass_flow_rate> auto flow_rate)
{
  return propellant_mass / flow_rate;
}

QuantityOf<isq::speed> auto exhaust_velocity(QuantityOf<specific_impulse> auto isp)
{
  return isp * g0;
}

QuantityOf<isq::duration> auto burn_time_with_stats(QuantityOf<isq::mass> auto propellant_mass,
                                                    QuantityOf<specific_impulse> auto isp,
                                                    QuantityOf<isq::force> auto thrust)
{
  quantity flow_rate = mass_flow_rate(thrust, isp);
  quantity burn_time = burn_time_from_flow(propellant_mass, flow_rate);
  quantity ve = exhaust_velocity(isp);

  using namespace si::unit_symbols;
  std::cout << "  Isp: " << isp << "\n";
  std::cout << "  Thrust: " << thrust << " = " << thrust.template in<double>(kN) << "\n";
  std::cout << "  Exhaust velocity: " << ve << " = " << ve.in(m / s) << "\n";
  std::cout << "  Mass flow rate: " << flow_rate << " = " << flow_rate.in(kg / s) << "\n";
  std::cout << "  Burn time: " << burn_time << " = " << burn_time.in(s) << "\n\n";

  return burn_time;
}

int main()
{
  using namespace si::unit_symbols;

  quantity propellant_mass = 15'000. * kg;

  std::cout << "Rocket Stage Burn Time Analysis\n";
  std::cout << "================================\n";
  std::cout << "Propellant mass: " << propellant_mass << "\n\n";

  // Engine 1: RD-180 (legacy Russian engine with thrust in kgf)
  std::cout << "Engine 1 (RD-180):\n";
  quantity engine1_isp = 311. * s;
  quantity engine1_thrust = 390'000 * kgf;  // Legacy unit!
  quantity burn_time_1 = burn_time_with_stats(propellant_mass, engine1_isp, engine1_thrust);

  // Engine 2: Modern high-efficiency engine with thrust in Newtons
  std::cout << "Engine 2 (Modern high-efficiency):\n";
  quantity engine2_isp = 450. * s;
  quantity engine2_thrust = 500. * kN;  // Modern SI unit
  quantity burn_time_2 = burn_time_with_stats(propellant_mass, engine2_isp, engine2_thrust);

  // Compare burn times
  quantity time_ratio = burn_time_2 / burn_time_1;
  std::cout << "Engine 2 burns " << time_ratio.in(one) << " times longer\n\n";

  // Calculate total impulse - demonstrates seamless mixing of kgf and N
  quantity total_impulse_1 = engine1_thrust * burn_time_1;
  quantity total_impulse_2 = engine2_thrust * burn_time_2;

  std::cout << "Total Impulse:\n";
  std::cout << "  Engine 1: " << total_impulse_1 << " = " << total_impulse_1.in(kN * s) << "\n";
  std::cout << "  Engine 2: " << total_impulse_2 << " = " << total_impulse_2.in(kN * s) << "\n";
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/isq.h>
    #include <iostream>

    using namespace mp_units;

    inline constexpr struct specific_impulse final :
        quantity_spec<isq::duration, isq::force / (isq::mass_flow_rate * isq::acceleration)> {} specific_impulse;

    inline constexpr struct standard_gravity final :
        named_unit<symbol_text{u8"g₀", "g_0"}, mag_ratio<980'665, 100'000> * si::metre / square(si::second)> {} standard_gravity;
    inline constexpr struct kilogram_force final :
        named_unit<"kgf", si::kilogram * standard_gravity> {} kilogram_force;

    inline constexpr Unit auto kgf = kilogram_force;
    inline constexpr Unit auto g0 = standard_gravity;

    QuantityOf<isq::mass_flow_rate> auto mass_flow_rate(QuantityOf<isq::force> auto thrust,
                                                        QuantityOf<specific_impulse> auto isp)
    {
      return thrust / (isp * g0);
    }

    QuantityOf<isq::duration> auto burn_time_from_flow(QuantityOf<isq::mass> auto propellant_mass,
                                                       QuantityOf<isq::mass_flow_rate> auto flow_rate)
    {
      return propellant_mass / flow_rate;
    }

    QuantityOf<isq::speed> auto exhaust_velocity(QuantityOf<specific_impulse> auto isp)
    {
      return isp * g0;
    }

    QuantityOf<isq::duration> auto burn_time_with_stats(QuantityOf<isq::mass> auto propellant_mass,
                                                        QuantityOf<specific_impulse> auto isp,
                                                        QuantityOf<isq::force> auto thrust)
    {
      quantity flow_rate = mass_flow_rate(thrust, isp);
      quantity burn_time = burn_time_from_flow(propellant_mass, flow_rate);
      quantity ve = exhaust_velocity(isp);

      using namespace si::unit_symbols;
      std::cout << "  Isp: " << isp << "\n";
      std::cout << "  Thrust: " << thrust << " = " << thrust.template in<double>(kN) << "\n";
      std::cout << "  Exhaust velocity: " << ve << " = " << ve.in(m / s) << "\n";
      std::cout << "  Mass flow rate: " << flow_rate << " = " << flow_rate.in(kg / s) << "\n";
      std::cout << "  Burn time: " << burn_time << " = " << burn_time.in(s) << "\n\n";

      return burn_time;
    }

    int main()
    {
      using namespace si::unit_symbols;

      quantity propellant_mass = 15'000. * kg;

      std::cout << "Rocket Stage Burn Time Analysis\n";
      std::cout << "================================\n";
      std::cout << "Propellant mass: " << propellant_mass << "\n\n";

      // Engine 1: RD-180 (legacy Russian engine with thrust in kgf)
      std::cout << "Engine 1 (RD-180):\n";
      quantity engine1_isp = 311. * s;
      quantity engine1_thrust = 390'000 * kgf;
      quantity burn_time_1 = burn_time_with_stats(propellant_mass, engine1_isp, engine1_thrust);

      // Engine 2: Modern high-efficiency engine with thrust in Newtons
      std::cout << "Engine 2 (Modern high-efficiency):\n";
      quantity engine2_isp = 450. * s;
      quantity engine2_thrust = 500. * kN;
      quantity burn_time_2 = burn_time_with_stats(propellant_mass, engine2_isp, engine2_thrust);

      // Compare burn times
      quantity time_ratio = burn_time_2 / burn_time_1;
      std::cout << "Engine 2 burns " << time_ratio.in(one) << " times longer\n\n";

      // Calculate total impulse - demonstrates seamless mixing of kgf and N
      quantity total_impulse_1 = engine1_thrust * burn_time_1;
      quantity total_impulse_2 = engine2_thrust * burn_time_2;

      std::cout << "Total Impulse:\n";
      std::cout << "  Engine 1: " << total_impulse_1 << " = " << total_impulse_1.in(kN * s) << "\n";
      std::cout << "  Engine 2: " << total_impulse_2 << " = " << total_impulse_2.in(kN * s) << "\n";
    }
    ```

    The solution defines `kilogram_force` as `kg × standard_gravity`, embedding g₀ directly
    into the unit definition. This enables elegant compile-time optimization:

    - **For Engine 1 (thrust in kgf)**: The _burn time_ formula contains g₀ in the numerator,
      while kgf contains g₀ in its denominator. These factors cancel perfectly at compile-time,
      eliminating any runtime multiplication or division by 9.80665.

    - **For Engine 2 (thrust in N)**: The g₀ factors don't cancel, but **mp-units** automatically
      handles the conversion, applying g₀ exactly where needed without manual intervention.

    Both engines use identical code—the type system ensures correctness regardless of whether
    _thrust_ is specified in legacy (kgf) or modern (N) units.


## References

- [User's Guide: Faster-than-Lightspeed Constants](../users_guide/framework_basics/faster_than_lightspeed_constants.md)


## Takeaways

- **Constants as units**: Physical constants like g₀ become part of the type system rather
  than runtime values
- **Automatic cancellation**: When constants appear in both numerator and denominator,
  they cancel at compile-time without manual intervention
- **Legacy unit support**: kgf (kilogram-force) embeds g₀ in its definition, enabling
  seamless calculations with historical engine specifications
- **Eliminates magic numbers**: No more manual `9.80665` conversions scattered throughout
  code
- **Perfect mathematical cancellation**: The formula t = (m × Isp × g₀) / F simplifies
  automatically when F is in kgf
- **Type safety**: The type system prevents mixing kg and kgf incorrectly
- **Mixed unit support**: Modern (N) and legacy (kgf) _thrust_ specifications work
  together in the same codebase
- **Historical significance**: Prevents the type of unit confusion that caused real
  mission failures (Mars Climate Orbiter)
- **Formula generality**: The same _burn time_ calculation works correctly regardless
  of whether _thrust_ is specified in kgf or N
