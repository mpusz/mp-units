# Points and Quantities

Understand the difference between points (absolute positions) and deltas (relative differences).

**Goal**: Learn when to use points vs deltas  
**Time**: ~15 minutes

## The Problem: Position vs Displacement

Consider these statements:

- "The temperature is 20°C" - This is a **point** (absolute position)
- "The temperature increased by 5°C" - This is a **delta** (relative change)

These are fundamentally different! You can't add two absolute positions, but you can add
a displacement to a position.

## Affine Space Basics

An **affine space** distinguishes between:

| Concept      | Example                           | Operations                                                 | mp-units type    |
|--------------|-----------------------------------|------------------------------------------------------------|------------------|
| **Point**    | 3:00 PM, 20 °C, GPS coordinates   | Can subtract (to get difference)                           | `quantity_point` |
| **Quantity** | 2 hours, 5 °C change, 10 km north | Can add, subtract, multiply by scalar and other quantities | `quantity`       |

**Key insight**: `point - point = quantity`, but `point + point` is **invalid**!

## Try It: Time Points

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

int main()
{
  using namespace mp_units::si::unit_symbols;

  // Create time points (absolute positions in time) using implicit origin
  quantity_point meeting_start{14 * h};  // 14:00 (2:00 PM)
  quantity_point meeting_end{16 * h};    // 16:00 (4:00 PM)

  // Calculate duration (difference between points)
  quantity meeting_duration = meeting_end - meeting_start;

  std::cout << "Meeting duration: " << meeting_duration << "\n";

  // Calculate next week's meeting (same time, 7 days later)
  quantity_point next_meeting = meeting_start + 7 * non_si::day;
  std::cout << "Next week's meeting: "
            << next_meeting.quantity_from_zero() << " from origin\n";

  // These would NOT compile (uncomment to see):
  // auto wrong = meeting_start + meeting_end;  // ❌ Can't add two points!

  // But this works:
  quantity time_until_next = next_meeting - meeting_end;
  std::cout << "Time until next meeting: " << time_until_next << "\n";
}
```

**Key insight**: `quantity_point` represents absolute positions in an affine space, while
`quantity` represents relative differences (vectors). The type system enforces valid
operations: points can be subtracted (yielding a quantity), and quantities can be added
to points (yielding a new point), but points cannot be added together.

!!! tip "Alternative Construction Syntax"

    There are two ways to create quantity points and quantities:

    **Direct construction** (used in the example above):

    ```cpp
    quantity duration = 2 * h;        // Duration of 2 hours
    quantity_point meeting{14 * h};   // Point at 14:00
    ```

    **Helper functions**:

    ```cpp
    quantity duration = delta<h>(2);        // Duration of 2 hours
    quantity_point meeting = point<h>(14);  // Point at 14:00
    ```

    Both approaches for points use the **implicit origin** automatically provided by the
    library - a generic zero point for the quantity type. Learn how to define custom origins
    in the [next tutorial](point_origins.md).

## Real-World Examples

### Temperature (next tutorial)

- Point: "It's 20 °C outside"
- Delta: "It warmed up by 5 °C"

### Locations

- Point: GPS coordinates (40.7128° N, 74.0060° W)
- Delta: Displacement (10 km north)

### Time

- Point: 3:00 PM on Jan 15, 2024
- Delta: 2 hours, 30 minutes

## Why This Matters?

Without affine space distinctions:

```cpp
// Wrong: treating temperatures as simple numbers
double temp1 = 20.0;  // 20 °C
double temp2 = 30.0;  // 30 °C
double wrong = (temp1 + temp2) / 2;  // 25 °C - correct average!

// But what about Fahrenheit?
double temp1_f = 68.0;   // 20 °C in °F
double temp2_f = 86.0;   // 30 °C in °F
double wrong_f = (temp1_f + temp2_f) / 2;  // 77 °F = 25 °C ✓

// Wait, that's not right for absolute zero!
// -273.15 °C + 100 °C ≠ average of absolute temperatures!
```

With affine space:

```cpp
// Points can't be averaged directly - must subtract first!
quantity_point temp1 = ...;
quantity_point temp2 = ...;
quantity diff = temp2 - temp1;  // 10 °C difference (quantity)
quantity_point avg_point = temp1 + diff / 2;  // 20 °C + 5 °C = 25 °C ✓
```

## Challenges

1. **Create a schedule**: Define points for breakfast (8:00), lunch (12:00), dinner (18:00)
   using `point<h>(...)`
2. **Calculate gaps**: Find time between breakfast and lunch
3. **Tomorrow's schedule**: Add 1 day to each point
4. **Try both syntaxes**: Recreate one time point using both `point<h>(...)` and
  `quantity_point{... * h}`

## What You Learned?

✅ Points represent absolute positions  
✅ Deltas represent relative differences  
✅ `point - point = delta`, `point + delta = point`  
✅ Can't add two points (prevented at compile time)  
✅ Affine space prevents category errors
