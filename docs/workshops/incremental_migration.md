# Workshop 14: Incremental Migration from Legacy Units

Many codebases use home-grown strong-type wrappers or established units libraries like
[Boost.Units](https://github.com/boostorg/units) or [nholthaus/units](https://github.com/nholthaus/units).
Rewriting everything at once is risky—massive code churn, difficult validation, and
team disruption. This workshop demonstrates how to **safely extend and incrementally migrate**
using **mp-units' interoperability layer**, allowing old and new code to coexist during
transition.

You'll learn the **bridge pattern**: specialize `quantity_like_traits` to enable seamless
conversion between legacy types and **mp-units**, then **add new features** using mp-units
while keeping existing code working unchanged. For complete details on `quantity_like_traits`,
see the [Interoperability with Other Libraries](../how_to_guides/integration/interoperability_with_other_libraries.md)
guide.

**Key migration strategy**: This workshop demonstrates an asymmetric approach to interoperability.
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
Meter legacy_calculate_distance(MeterPerSecond speed, Second time)
{
  return Meter{speed.value * time.value};
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

  // Speed validation
  if (!legacy_is_safe_velocity(speed)) {
    // handle unsafe speed
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
- ✅ Natural arithmetic (`distance = speed * time` just works)
- ✅ Built-in unit conversions (`.in(km)`, `.in(mi)`, `.in(kJ)`)
- ✅ Derived units automatically handled (_kinetic energy_ from `mass × speed²`)
- ✅ Modern C++20 features (concepts, formatting)

**But** you need the new features to work seamlessly with existing code:

- Existing motion calculations continue working unchanged
- New _energy_/_mass_ features use **mp-units**
- Legacy _speed_ types can be passed to new functions
- Seamless interoperability at boundaries


## Your Task

Your goal is to **enable interoperability** between the legacy `Meter`, `Second`, and
`MeterPerSecond` types and **mp-units**, allowing the provided `compute_kinetic_energy()`
functions to work with legacy speed types.

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
Meter legacy_calculate_distance(MeterPerSecond speed, Second time)
{
  return Meter{speed.value * time.value};
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
    throw std::runtime_error("Speed exceeds safety limit!");
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
  MeterPerSecond legacy_speed{25.0};  // 25 m/s

  using namespace mp_units::si::unit_symbols;
  // TODO: Step 3 - Call both energy functions with 70*kg and legacy_speed
  quantity energy = ...;
  quantity energy_generic = ...;
  std::cout << "KE = " << energy << " = " << energy.in(kJ) << "\n";
  std::cout << "KE = " << energy_generic << " = " << energy_generic.in(kJ) << "\n";
}
```

??? tip "Solution"

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
    Meter legacy_calculate_distance(MeterPerSecond speed, Second time)
    {
      return Meter{speed.value * time.value};
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
        throw std::runtime_error("Speed exceeds safety limit!");
      return 0.5 * mass * pow<2>(speed);
    }

    QuantityOf<isq::kinetic_energy> auto compute_kinetic_energy_generic(QuantityOf<isq::mass> auto mass,
                                                                        QuantityOf<isq::speed> auto speed)
    {
      using namespace mp_units::si::unit_symbols;

      // Same validation, same explicit export needed
      if (!legacy_is_safe_velocity(MeterPerSecond(speed)))
        throw std::runtime_error("Speed exceeds safety limit!");
      return 0.5 * mass * pow<2>(speed);
    }

    int main()
    {
      MeterPerSecond legacy_speed{25.0};  // 25 m/s

      using namespace mp_units::si::unit_symbols;
      quantity energy = compute_kinetic_energy(70. * kg, legacy_speed);
      quantity energy_generic = compute_kinetic_energy_generic(70. * kg, quantity{legacy_speed});
      std::cout << "KE = " << energy << " = " << energy.in(kJ) << "\n";
      std::cout << "KE = " << energy_generic << " = " << energy_generic.in(kJ) << "\n";
    }
    ```

??? abstract "What you learned?"

    ### `quantity_like_traits` enables non-invasive interoperability

    Legacy types remain unchanged:

    ```cpp
    // Original legacy types (DO NOT MODIFY)
    struct Meter { double value; };
    struct Second { double value; };
    struct MeterPerSecond { double value; };
    ```

    Interoperability layer specializes `quantity_like_traits`:

    ```cpp
    template<>
    struct mp_units::quantity_like_traits<Meter> {
      using rep = double;
      static constexpr auto reference = si::metre;
      static constexpr bool explicit_import = false;  // Allow implicit legacy → mp-units
      static constexpr bool explicit_export = true;   // Require explicit mp-units → legacy

      static constexpr rep to_numerical_value(Meter m) { return m.value; }
      static constexpr Meter from_numerical_value(rep v) { return Meter{v}; }
    };
    ```

    Benefits:

    - No changes to legacy code
    - Works with third-party libraries ([Boost.Units](https://github.com/boostorg/units),
      [nholthaus/units](https://github.com/nholthaus/units))
    - Gradual adoption without "big bang" rewrite
    - Both systems coexist during transition

    ### Asymmetric conversion control

    `explicit_import` and `explicit_export` flags control conversion direction:

    ```cpp
    // explicit_import = false: Legacy → mp-units is implicit
    MeterPerSecond legacy_speed{25.0};
    quantity speed = compute_energy(70 * kg, legacy_speed);  // ✅ Just works!

    // explicit_export = true: mp-units → Legacy requires explicit syntax
    quantity<si::metre / si::second> speed_new = 30 * m / s;
    bool safe = legacy_is_safe_velocity(MeterPerSecond(speed_new));  // ✅ Explicit conversion
    // bool wrong = legacy_is_safe_velocity(speed_new);  // ❌ Does not compile!
    ```

    This asymmetry:

    - Makes legacy integration points visible (searchable with grep)
    - Guides developers toward modern code
    - Tracks migration progress (count of explicit conversions)
    - Prevents silent bugs at boundaries

    ### `QuantityOf` concept vs concrete `quantity<>` types

    Critical API design choice affecting interoperability:

    ```cpp
    // Concrete type: Fixed units, allows implicit conversion from quantity_like
    quantity<si::joule> compute_energy_concrete(quantity<si::kilo<si::gram>> mass,
                                                quantity<si::metre / si::second> speed)
    {
      // Legacy types work directly:
      return 0.5 * mass * pow<2>(speed);
    }

    // Generic concept: Flexible units, requires explicit quantity construction
    QuantityOf<isq::kinetic_energy> auto compute_energy_generic(QuantityOf<isq::mass> auto mass,
                                                                QuantityOf<isq::speed> auto speed)
    {
      // quantity_like types don't satisfy QuantityOf concept
      return 0.5 * mass * pow<2>(speed);
    }

    MeterPerSecond legacy_v{25.0};

    compute_energy_concrete(70 * kg, legacy_v);           // ✅ Implicit conversion
    compute_energy_generic(70 * kg, quantity{legacy_v});  // ✅ Explicit construction needed
    ```

    **Why `quantity_like` types don't satisfy `QuantityOf`:**

    - Missing static data members: `reference`, `unit`, `quantity_spec`
    - Missing member functions: `.in()`, `.force_in()`, `.numerical_value_in()`
    - Only conversion functions are defined by `quantity_like_traits`

    Choose based on priorities:

    - **Concrete types**: Better legacy interop, fixed units, simpler call sites
    - **Generic concepts**: More flexible (any compatible units), explicit conversions

    ### Gradual migration strategy

    Phase 1 - Enable interoperability:

    ```cpp
    // Define traits for legacy types
    template<> struct quantity_like_traits<Meter> { /* ... */ };
    template<> struct quantity_like_traits<Second> { /* ... */ };
    template<> struct quantity_like_traits<MeterPerSecond> { /* ... */ };
    ```

    Phase 2 - Add new features with mp-units:

    ```cpp
    // New physics calculations use mp-units
    QuantityOf<isq::kinetic_energy> auto compute_kinetic_energy(QuantityOf<isq::mass> auto mass,
                                                                QuantityOf<isq::speed> auto speed)
    {
      return 0.5 * mass * pow<2>(speed);  // Dimensional analysis just works!
    }
    ```

    Phase 3 - Migrate modules incrementally:

    ```cpp
    // Old module stays unchanged - uses legacy types
    MeterPerSecond legacy_speed{30.0};
    Second legacy_time{5.0};
    Meter legacy_distance = legacy_calculate_distance(legacy_speed, legacy_time);

    // New module uses mp-units - clean arithmetic!
    quantity speed = 30 * m / s;
    quantity time = 5 * s;
    quantity distance = speed * time;  // No special functions needed!

    // Boundary: explicit conversion when passing mp-units to legacy
    Meter result_for_old_code = Meter(distance);
    ```

    Phase 4 - Track and measure:

    ```cpp
    // Search for legacy integration points:
    // grep -r "Meter(" "Second(" "MeterPerSecond(" codebase/

    // Count remaining conversions = migration debt
    // Zero conversions = migration complete!
    ```

    ### Benefits of explicit export for migration

    | Aspect             | `explicit_export = true`                     | `explicit_export = false`                        |
    |--------------------|----------------------------------------------|--------------------------------------------------|
    | **Visibility**     | Every boundary crossing is visible in code   | Silent conversions hide integration points       |
    | **Searchability**  | `grep "MeterPerSecond("` finds all uses      | No clear search pattern                          |
    | **Metrics**        | Count explicit casts = remaining legacy debt | Can't measure progress                           |
    | **Safety**         | Compiler guards each boundary                | Silent bugs possible at boundaries               |
    | **Team awareness** | Code review catches legacy calls             | Reviewers might miss implicit conversions        |
    | **When to use**    | **Simple legacy wrappers during migration**  | **Established libraries with equivalent safety** |

    **Recommendation**: Use `explicit_export = true` for home-grown legacy types during
    migration. Use `explicit_export = false` for established libraries like
    [Boost.Units](https://github.com/boostorg/units) that already provide dimensional safety.

    ### Real-world migration benefits

    **Before migration** (home-grown strong types):

    - ❌ No dimensional analysis
    - ❌ Manual `.value` extraction everywhere
    - ❌ Manual unit conversions (km → m)
    - ❌ No derived units (must define every combination)
    - ❌ Verbose arithmetic

    **After migration** (with interoperability):

    - ✅ Dimensional analysis for new code
    - ✅ Natural arithmetic (`distance = speed * time`)
    - ✅ Automatic unit conversions (`.in(km)`, `.in(mi)`)
    - ✅ Derived units compose automatically (m/s, km/h, etc.)
    - ✅ Legacy code works unchanged
    - ✅ Clear migration path and progress tracking


## References

<!-- markdownlint-disable MD013 -->
- [Interoperability with Other Libraries](../how_to_guides/integration/interoperability_with_other_libraries.md) -
  Complete guide to `quantity_like_traits`
- [Working with Legacy Interfaces](../how_to_guides/integration/working_with_legacy_interfaces.md) -
  Extracting numerical values and working with `double`-based APIs
<!-- markdownlint-enable MD013 -->


## Takeaways

- **Non-invasive migration**: `quantity_like_traits` enables interoperability without modifying
  legacy types
- **Asymmetric conversion control**: `explicit_import = false` (legacy → new is implicit),
  `explicit_export = true` (new → legacy requires visible syntax)
- **Gradual transition**: Legacy code works unchanged while new code gains dimensional analysis,
  unit conversions, and modern C++20 features
- **Measurable progress**: Explicit conversions at boundaries make migration trackable
  (grep for `LegacyType(` to count remaining legacy debt)
- **API design choice**: `QuantityOf` concepts vs concrete `quantity<>` types affects
  interoperability—generic is more flexible but requires explicit construction from `quantity_like`
