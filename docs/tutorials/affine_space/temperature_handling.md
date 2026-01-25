# Temperature Handling

Understand the critical difference between absolute and relative _temperatures_.

**Goal**: Learn when to use absolute vs relative _temperature_ quantities  
**Time**: ~15 minutes

## The Temperature Problem

**Critical question**: What does `20 °C + 30 °C` mean?

**Possible interpretations:**

1. **_Temperature_ changes**: "Increase by 20 °C, then by 30 °C" → Total change: 50 °C ✅
  (Makes sense!)
2. **Absolute _temperatures_**: "Room at 20 °C plus water at 30 °C" → Result: 50 °C ❌ (Meaningless!)
3. **Point + change**: "Start at 20 °C, increase by 30 °C" → Result: 50 °C ✅ (Makes sense!)

Let's prove this with Kelvin:

```cpp
// Interpretation 1: Temperature CHANGES (makes sense)
// 20 °C change + 30 °C change = 50 °C change
// 20 K + 30 K = 50 K  ✅ Correct!

// Interpretation 2: Absolute TEMPERATURES (wrong!)
// 20 °C = 293.15 K, 30 °C = 303.15 K
// 293.15 K + 303.15 K = 596.30 K
// But 50 °C = 323.15 K
// 596.30 K ≠ 323.15 K  ❌ Numbers don't match!

// Interpretation 3: Point + change (makes sense)
// Start at 20 °C = 293.15 K, increase by 30 K
// 293.15 K + 30 K = 323.15 K = 50 °C  ✅ Correct!

// The library prevents ambiguity through types:
// quantity + quantity              // ✅ Changes add
// quantity_point + quantity_point  // ❌ Points don't add (won't compile!)
// quantity_point + quantity        // ✅ Point + change works
```

**The solution**: Use `quantity` for changes, `quantity_point` for absolute values!

!!! note "Construction Helpers for Temperatures"

    The direct multiplication syntax `20 * deg_C` is ambiguous (is it a change or absolute value?)
    and is not allowed (does not compile) when working with temperatures.

    To avoid the ambiguity shown above, use explicit construction helpers:

    ```cpp
    // For temperature differences (quantities):
    quantity temp_change = delta<deg_C>(20);  // 20 °C change = 20 K

    // For absolute temperatures (points):
    quantity_point temp = point<deg_C>(20);   // 20 °C absolute = 293.15 K
    ```

## Absolute vs Relative

- **Absolute temperature**: A specific point (20 °C, 293 K)
- **Relative temperature**: A difference (increase by 5 °C, ΔT = 5 K)

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Relative temperature (a difference)
  quantity temp_diff = delta<K>(10);  // Changed by 10 K

  std::cout << "Temperature difference: " << temp_diff << "\n";
  std::cout << "In Celsius: " << temp_diff.in(deg_C) << "\n";  // Also 10 °C change

  // You can add/subtract relative temperatures
  quantity total_change = temp_diff + delta<K>(5);
  std::cout << "Total change: " << total_change << "\n";

  // You can multiply relative temperatures
  quantity double_change = 2.0 * temp_diff;
  std::cout << "Double change: " << double_change << "\n";

  // For absolute temperatures, we need quantity_point (next example)
}
```

**Key insight**: Temperature differences (ΔT) are quantities that can be added, subtracted,
and multiplied by scalars. A 10 K change equals a 10 °C change - the magnitude is the
same because Kelvin and Celsius have the same scale, just different zero points.

!!! important "Remember"

    Differences in temperature are quantities; absolute temperatures are points!

## Absolute Temperature: `quantity_point`

For absolute temperature readings, use `quantity_point`:

```cpp
// ce-embed height=700 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Absolute temperatures (specific points)
  quantity_point room_temp = point<deg_C>(20);  // 293.15 K
  quantity_point body_temp = point<deg_C>(37);  // 310.15 K

  // Subtract two absolute temperatures → get a difference (quantity)
  quantity fever = body_temp - room_temp;
  std::cout << "Temperature difference: " << fever << "\n";
  std::cout << "In Kelvins: " << fever.in(K) << "\n";

  // You CANNOT add two absolute temperatures (doesn't make sense!)
  // auto bad = room_temp + body_temp;  // ❌ Won't compile

  // You CAN add a difference to an absolute temperature
  quantity_point fever_temp = body_temp + delta<deg_C>(2);
  std::cout << "Fever temperature: " << fever_temp - si::zeroth_degree_Celsius << "\n";
}
```

**Key insight**: Absolute temperatures are `quantity_point`s that represent specific points
on the temperature scale. You can only subtract them (to get a difference) or add a
quantity (temperature change) to them. Adding two absolute temperatures is prevented by
the type system because it's physically meaningless.

## Understanding `quantity_from_zero()` with Temperatures

For temperatures (and other offset units), `.quantity_from_zero()` has special
behavior - it returns the quantity from the **unit's zero**, not absolute zero:

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <mp-units/systems/usc.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::usc::unit_symbols;

  // Temperature point: 20°C
  quantity_point temp = point<deg_C>(20.);

  // quantity_from_zero() returns quantity from the UNIT'S zero
  std::cout << "From Celsius zero (0 °C): " << temp.in(deg_C).quantity_from_zero() << "\n";
  std::cout << "From Fahrenheit zero (0 °F): " << temp.in(deg_F).quantity_from_zero() << "\n";
  std::cout << "From Kelvin zero (0 K): " << temp.in(K).quantity_from_zero() << "\n";

  // For Celsius: unit's zero is 0°C (= 273.15 K)
  // For Kelvin: unit's zero is 0 K (absolute zero)
  std::cout << "Are they equal? " << (temp.in(K) == temp.in(deg_C) ? "Yes" : "No") << "\n";
  std::cout << "Difference: " << (temp.in(K) - temp.in(deg_C)) << "\n";

  std::cout << "\n✅ quantity_from_zero() depends on the unit's reference point!\n";
}
```

**Key insight**: `.quantity_from_zero()` for offset units returns the quantity from
the **unit's zero**, which for Celsius is 0 °C (273.15 K), not absolute zero!


## Challenges

1. **Absolute vs Relative**: Why can't you add two absolute temperatures?
2. **Daily temp range**: Morning 15°C, afternoon 25°C. What's the change? (relative)
3. **Temperature conversion**: Create 98.6°F, convert to Celsius and Kelvin

## What You Learned?

✅ Relative temperature = difference (quantity)  
✅ Absolute temperature = specific point (quantity_point)  
✅ You cannot add two absolute temperatures  
✅ For differences: 1 K = 1°C (same magnitude)  
✅ Celsius zero is not the same as absolute zero (Kelvin)  
✅ `.quantity_from_zero()` depends on the unit's reference point  
