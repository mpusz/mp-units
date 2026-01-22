# Tutorial 13: Incremental Migration from Legacy Units

Many codebases use home-grown strong-type wrappers or established units libraries like
Boost.Units or nholthaus/units. Rewriting everything at once is risky—massive code churn,
difficult validation, and team disruption. This tutorial demonstrates how to **safely extend
and incrementally migrate** using **mp-units' interoperability layer**, allowing old and
new code to coexist during transition.

You'll learn the **bridge pattern**: specialize `quantity_like_traits` to enable seamless
conversion between legacy types and **mp-units**, then **add new features** using mp-units
while keeping existing code working unchanged. For complete details on `quantity_like_traits`,
see the [Interoperability with Other Libraries](../how_to_guides/interoperability_with_other_libraries.md)
guide.

**Key migration strategy**: This tutorial demonstrates an asymmetric approach to interoperability.
Legacy types can flow naturally into new **mp-units** code, gaining safety and modern features.
However, when passing **mp-units** quantities back to legacy code, explicit conversions are
required. This intentional asymmetry provides visible integration points, makes the migration
measurable, and ensures the transition is gradual and safe.


## Problem Statement

Consider a physics simulation codebase that has evolved over 15 years using simple strong-type
wrappers for safety:

```cpp
// Home-grown strong types used throughout the codebase
struct Meter {
  double value;
};

struct Second {
  double value;
};

struct MeterPerSecond {
  double value;
};

// Thousands of functions use these types
Meter legacy_calculate_distance(MeterPerSecond velocity, Second time)
{
  return Meter{velocity.value * time.value};
}

bool legacy_is_safe_velocity(MeterPerSecond v)
{
  return v.value < 100.0;  // Safety limit: 100 m/s
}

// Typical usage throughout the codebase
void simulate_motion()
{
  MeterPerSecond speed{30.0};
  Second duration{5.0};
  Meter distance = legacy_calculate_distance(speed, duration);

  // Converting units requires manual factors
  double distance_km = distance.value / 1000.0;  // Manual conversion!

  // Velocity validation
  if (!legacy_is_safe_velocity(speed)) {
    // handle unsafe velocity
  }
}
```

**Problems with this approach:**

1. **No dimensional analysis**: `Meter{100} * Second{50}` compiles but produces nonsense
2. **Manual arithmetic**: Every calculation requires `.value` extraction and re-wrapping
3. **No unit conversions**: Converting km → m or km/h → m/s requires manual factors
4. **Limited scalability**: Fixed types for derived units (`MeterPerSecond`) don't
  scale—**mp-units** uses unit equations (`m/s`, `km/h`) that compose automatically
5. **Verbose**: Simple physics becomes cluttered with `.value` everywhere

Your team wants to **extend** the library with new physics features (_mass_, _energy_)
using **mp-units**:

- ✅ Automatic dimensional analysis
- ✅ Natural arithmetic (`distance = velocity * time` just works)
- ✅ Built-in unit conversions (`.in(km)`, `.in(mi)`, `.in(kJ)`)
- ✅ Derived units automatically handled (_kinetic energy_ from `mass × speed²`)
- ✅ Modern C++20 features (concepts, formatting)

**But** you need the new features to work seamlessly with existing code:

- Existing motion calculations continue working unchanged
- New _energy_/_mass_ features use **mp-units**
- Legacy _velocity_ types can be passed to new functions
- Seamless interoperability at boundaries


## Your Task

Your goal is to **enable interoperability** between the legacy `Meter`, `Second`, and
`MeterPerSecond` types and **mp-units**, allowing the provided `compute_kinetic_energy()`
functions to work with legacy velocity types.

**Step 1**: Implement `quantity_like_traits` specializations for the three legacy types.

- Map `Meter` → `si::metre`, `Second` → `si::second`, `MeterPerSecond` → `si::metre / si::second`
- Set `explicit_import = false` (allow implicit legacy → **mp-units** conversion)
- Set `explicit_export = true` (require explicit **mp-units** → legacy conversion)

**Step 2**: Complete the provided functions to demonstrate new → old integration:

- In both `compute_kinetic_energy()` functions, convert `speed` to `MeterPerSecond` for
  calling `legacy_is_safe_velocity()`
- This demonstrates explicit export: `MeterPerSecond(speed)` or
  `MeterPerSecond(speed.in(m / s))`
- Observe: New **mp-units** code calls legacy validation seamlessly!

**Step 3**: Call the functions to see interoperability in action:

- Call both `compute_kinetic_energy()` functions with `70 * kg` and `legacy_velocity`
- Observe the difference: concrete type allows implicit conversion, generic requires explicit

**Requirements**:

- Implement `quantity_like_traits` for `Meter`, `Second`, and `MeterPerSecond`
  (use `explicit_import = false` and `explicit_export = true`)
- Complete the energy functions by converting `speed` to `MeterPerSecond` for legacy validation
- Complete the `main()` function by calling the _energy_ calculation functions
- Don't modify the legacy struct definitions

```cpp
// ce-embed height=800 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/math.h>
#include <iostream>

using namespace mp_units;

// Legacy strong types (DO NOT MODIFY)
struct Meter {
  double value;
};

struct Second {
  double value;
};

struct MeterPerSecond {
  double value;
};

// Legacy functions (DO NOT MODIFY)
Meter legacy_calculate_distance(MeterPerSecond velocity, Second time)
{
  return Meter{velocity.value * time.value};
}

bool legacy_is_safe_velocity(MeterPerSecond v)
{
  return v.value < 100.0;  // Safety limit: 100 m/s
}

// TODO: Step 1 - Specialize quantity_like_traits for Meter, Second, MeterPerSecond
// Hint:
// - Set reference to si::metre, si::second, si::metre / si::second
// - Set explicit_import = false (allow implicit legacy → mp-units)
// - Set explicit_export = true (require explicit mp-units → legacy)

// Modern functions (PARTIALLY PROVIDED - complete the legacy integration)
quantity<si::joule> compute_kinetic_energy(quantity<si::kilo<si::gram>> mass,
                                           quantity<si::metre / si::second> speed)
{
  using namespace mp_units::si::unit_symbols;

  // TODO: Step 2a - Convert speed to MeterPerSecond to call legacy function
  if (!legacy_is_safe_velocity(...))
    throw std::runtime_error("Velocity exceeds safety limit!");
  return 0.5 * mass * pow<2>(speed);
}

QuantityOf<isq::kinetic_energy> auto compute_kinetic_energy_generic(QuantityOf<isq::mass> auto mass,
                                                                    QuantityOf<isq::speed> auto speed)
{
  using namespace mp_units::si::unit_symbols;

  // TODO: Step 2b - Convert speed to MeterPerSecond to call legacy function
  if (!legacy_is_safe_velocity(...))
    throw std::runtime_error("Velocity exceeds safety limit!");
  return 0.5 * mass * pow<2>(speed);
}

int main()
{
  MeterPerSecond legacy_velocity{25.0};  // 25 m/s

  using namespace mp_units::si::unit_symbols;
  // TODO: Step 3 - Call both energy functions with 70*kg and legacy_velocity
  quantity energy = ...;
  quantity energy_generic = ...;
  std::cout << "KE = " << energy << " = " << energy.in(kJ) << "\n";
  std::cout << "KE = " << energy_generic << " = " << energy_generic.in(kJ) << "\n";
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <mp-units/math.h>
    #include <iostream>

    using namespace mp_units;

    // Legacy strong types (DO NOT MODIFY)
    struct Meter {
      double value;
    };

    struct Second {
      double value;
    };

    struct MeterPerSecond {
      double value;
    };

    // Legacy functions (DO NOT MODIFY)
    Meter legacy_calculate_distance(MeterPerSecond velocity, Second time)
    {
      return Meter{velocity.value * time.value};
    }

    bool legacy_is_safe_velocity(MeterPerSecond v)
    {
      return v.value < 100.0;  // Safety limit: 100 m/s
    }

    // Specialize quantity_like_traits for legacy types
    template<typename T>
    constexpr auto reference_for = std::ignore;

    template<> constexpr auto reference_for<Meter> = si::metre;
    template<> constexpr auto reference_for<Second> = si::second;
    template<> constexpr auto reference_for<MeterPerSecond> = si::metre / si::second;

    template<typename T>
      requires (!std::same_as<decltype(reference_for<T>), decltype(std::ignore)>)
    struct  mp_units::quantity_like_traits<T> {
      using rep = double;
      static constexpr auto reference = reference_for<T>;

      // Allow implicit T → quantity (safe direction: legacy → modern)
      static constexpr bool explicit_import = false;

      // Require explicit quantity → T (make legacy conversions obvious)
      static constexpr bool explicit_export = true;

      static constexpr rep to_numerical_value(T m) { return m.value; }
      static constexpr T from_numerical_value(rep v) { return T{v}; }
    };

    // Modern functions
    quantity<si::joule> compute_kinetic_energy(quantity<si::kilo<si::gram>> mass,
                                               quantity<si::metre / si::second> speed)
    {
      using namespace mp_units::si::unit_symbols;

      // Demonstrate new → old integration: call legacy validation
      // Need explicit export since explicit_export = true
      if (!legacy_is_safe_velocity(MeterPerSecond(speed)))
        throw std::runtime_error("Velocity exceeds safety limit!");
      return 0.5 * mass * pow<2>(speed);
    }

    QuantityOf<isq::kinetic_energy> auto compute_kinetic_energy_generic(QuantityOf<isq::mass> auto mass,
                                                                        QuantityOf<isq::speed> auto speed)
    {
      using namespace mp_units::si::unit_symbols;

      // Same validation, same explicit export needed
      if (!legacy_is_safe_velocity(MeterPerSecond(speed)))
        throw std::runtime_error("Velocity exceeds safety limit!");
      return 0.5 * mass * pow<2>(speed);
    }

    int main()
    {
      MeterPerSecond legacy_velocity{25.0};  // 25 m/s

      using namespace mp_units::si::unit_symbols;
      quantity energy = compute_kinetic_energy(70. * kg, legacy_velocity);
      quantity energy_generic = compute_kinetic_energy_generic(70. * kg, quantity{legacy_velocity});
      std::cout << "KE = " << energy << " = " << energy.in(kJ) << "\n";
      std::cout << "KE = " << energy_generic << " = " << energy_generic.in(kJ) << "\n";
    }
    ```


## References

- [Interoperability with Other Libraries](../how_to_guides/interoperability_with_other_libraries.md) -
  Complete guide to `quantity_like_traits`
- [Working with Legacy Interfaces](../how_to_guides/working_with_legacy_interfaces.md) -
  Extracting numerical values and working with `double`-based APIs


## Takeaways

**Key concepts learned:**

1. **`quantity_like_traits` enables interoperability** with legacy types without modifying them
2. **`QuantityOf` concept vs concrete types** - critical API design choice:
    - `quantity_like` types don't satisfy `QuantityOf` concept (missing static data
       members (e.g., `reference`, `unit`, ...) and many member functions like `.in()`)
    - Generic functions with `QuantityOf` constraints: More flexible (accept any
      compatible units), but require explicit `quantity{...}` construction for
      `quantity_like` types
    - Concrete functions with `quantity<...>` parameters: Less flexible (fixed units),
      but allow implicit conversion from `quantity_like`
    - Choose based on: API flexibility needs vs call-site convenience
3. **Bidirectional integration patterns**:
    - Legacy → New (implicit): `explicit_import = false` allows legacy types to flow naturally
      into **mp-units** code, gaining dimensional safety
      (e.g., `compute_kinetic_energy(70*kg, legacy_velocity)`)
    - New → Legacy (explicit): `explicit_export = true` requires visible syntax when calling
      legacy functions from **mp-units** (e.g., `legacy_is_safe_velocity(MeterPerSecond(speed))`)
    - Unit conversions happen automatically in both directions (e.g., `km/h` ↔ `m/s`)

**Why explicit export helps migration:**

| Aspect                | `explicit_export = true`                                    | `explicit_export = false`                                          |
|-----------------------|-------------------------------------------------------------|--------------------------------------------------------------------|
| **Syntax**            | `LegacyType(quantity)` visible                              | Hidden in function calls                                           |
| **Searchability**     | Grep for `MeterPerSecond(` finds all                        | Hard to find integration points                                    |
| **Refactoring**       | Compiler guards each boundary crossing                      | Silent conversions might hide issues                               |
| **Progress tracking** | Count explicit casts = legacy debt                          | No clear metric                                                    |
| **Team clarity**      | "This crosses into legacy land"                             | Boundary invisible to reviewers                                    |
| **When to use**       | **Recommended for simple legacy wrappers** during migration | Established libraries (Boost.Units) with similar safety guarantees |

**Real-world benefits:**

- ✅ **Zero disruption**: Legacy code continues working unchanged
- ✅ **Immediate value**: New code gains dimensional analysis, unit conversions, modern C++20
- ✅ **Gradual learning**: Team learns **mp-units** while delivering features
- ✅ **Clear metrics**: Track migration progress (% of modules migrated)
- ✅ **Risk reduction**: Validate each module before moving to the next
