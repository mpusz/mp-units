# Tutorial 3: Extracting Numeric Values for Legacy APIs

When working with **mp-units**, you'll often need to interface with legacy code or
external APIs that expect raw numeric values in specific units. This tutorial shows how
to safely extract numeric values from strongly-typed quantities while maintaining the
benefits of compile-time unit checking.

We'll explore common scenarios where you need to "break out" of the type-safe world to
communicate with legacy systems, and then safely "break back in" when processing their
responses.

## Problem statement

This tutorial demonstrates how to:

- Compute a physical quantity (_speed_) in one unit system (e.g., miles per hour),
- Convert and pass it as a numeric value in another unit (e.g., km/h) to a legacy API,
- Accept and convert values back from the legacy API as needed.

## Your task

The legacy code below:

1. Calculates _speed_ in mph from _distance_ in miles and _duration_ in minutes.
2. Converts and passes the _speed_ as km/h to a legacy API that expects km/h.
3. Accepts a new _speed_ in km/h from the legacy API,
   converts it back to mph, and uses it in your code.

**Refactor the code in the `main()` function to use modern, strongly-typed quantities:**

- Use `.numerical_value_in(Unit)` to extract values for the legacy API.
- Use `.numerical_value_ref_in(Unit)` to provide a reference for legacy APIs
  that directly modify underlying values.
- Keep extraction and conversion localized at API boundaries;
  keep the rest of the code strongly typed.
- Do not modify the legacy functions.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::yard_pound::unit_symbols;

#define MIN_TO_H(d) ((d) / 60.0)           // Convert minutes to hours
#define MPH_TO_KMH(s) ((s) * 1.609344)     // Convert mph to km/h
#define KMH_TO_MPH(s) ((s) / 1.609344)     // Convert km/h to mph

// Simulate a legacy API that expects speed in km/h
bool legacy_check_speed_limit(double speed_kmh, double limit_kmh)
{
  std::cout << "[legacy] speed: " << speed_kmh << " km/h\n";
  std::cout << "[legacy] limit: " << limit_kmh << " km/h\n";
  return speed_kmh <= limit_kmh;
}

// Simulate a legacy API that sets speed in km/h
void legacy_set_cruise_control_speed(double& speed_kmh)
{
  speed_kmh = 90;
}

int main()
{
  const double speed_limit_kmh = 90;
  double distance_mi = 120;
  double duration_min = 120;
  double speed_mph = distance_mi / MIN_TO_H(duration_min);
  if (!legacy_check_speed_limit(MPH_TO_KMH(speed_mph), speed_limit_kmh)) {
    double new_speed_kmh;
    legacy_set_cruise_control_speed(new_speed_kmh);
    speed_mph = KMH_TO_MPH(new_speed_kmh);
    std::cout << "Speed adjusted to " << speed_mph << " mi/h\n";
  }
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/yard_pound.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;
    using namespace mp_units::yard_pound::unit_symbols;

    // Simulate a legacy API that expects speed in km/h
    bool legacy_check_speed_limit(double speed_kmh, double limit_kmh)
    {
      std::cout << "[legacy] speed: " << speed_kmh << " km/h\n";
      std::cout << "[legacy] limit: " << limit_kmh << " km/h\n";
      return speed_kmh <= limit_kmh;
    }

    // Simulate a legacy API that sets speed in km/h
    void legacy_set_cruise_control_speed(double& speed_kmh)
    {
      speed_kmh = 90;
    }

    int main()
    {
      const quantity speed_limit = 90. * km / h;
      quantity distance = 120. * mi;
      quantity duration = 120. * min;
      quantity speed = (distance / duration).in(mph);
      if (!legacy_check_speed_limit(speed.numerical_value_in(km / h), speed_limit.numerical_value_in(km / h))) {
        quantity<km / h> new_speed;
        legacy_set_cruise_control_speed(new_speed.numerical_value_ref_in(km / h));
        speed = new_speed.in(mph);
        std::cout << "Speed adjusted to " << speed << '\n';
      }
    }
    ```


??? abstract "What you learned?"

    ### Safe extraction with `.numerical_value_in(unit)`

    To pass values to legacy APIs, extract the numeric value in a specific unit:

    ```cpp
    speed.numerical_value_in(km / h)  // Returns double with the value in km/h
    ```

    - ✅ Explicit unit specification prevents accidental unit mismatches
    - ✅ Conversion happens at the API boundary, keeping internal code type-safe
    - ✅ Clear documentation of what unit the legacy API expects

    ### Mutable references with `.numerical_value_ref_in()`

    For legacy APIs that modify values through references:

    ```cpp
    quantity<km / h> new_speed;
    legacy_set_cruise_control_speed(new_speed.numerical_value_ref_in(km / h));
    ```

    - Returns a mutable reference to the underlying numeric value
    - The quantity must already have the correct unit (no conversion on return)
    - Useful for C-style APIs that use output parameters

    ### API boundary pattern

    The recommended pattern for legacy API interaction:

    1. **Keep internal code strongly-typed**: Use `quantity` throughout your logic
    2. **Extract at boundaries**: Use `.numerical_value_in()` only when calling legacy APIs
    3. **Wrap returns immediately**: Convert raw values back to `quantity` as soon as possible
    4. **Document units**: Make it explicit what units the legacy API expects/returns

    This minimizes the "unsafe zone" and maximizes type safety benefits.


## References

- [User's Guide: Working with Legacy Interfaces](../how_to_guides/working_with_legacy_interfaces.md)
- [API Reference](../reference/api_reference.md)


## Takeaways

- Always extract and convert numeric values at API boundaries,
  not throughout your codebase.
- Use explicit conversion macros or functions to document and control unit changes.
- Keep your internal logic type-safe and unit-aware;
  only use raw numbers when required by external APIs.
- Comment your conversions and API boundaries for clarity and maintainability.
