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
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
#include <mp-units/systems/si.h>
#include <mp-units/systems/international.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::international::unit_symbols;

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

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/international.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;
    using namespace mp_units::international::unit_symbols;

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

## Takeaways

- Always extract and convert numeric values at API boundaries,
  not throughout your codebase.
- Use explicit conversion macros or functions to document and control unit changes.
- Keep your internal logic type-safe and unit-aware;
  only use raw numbers when required by external APIs.
- Comment your conversions and API boundaries for clarity and maintainability.
