# Point Origins

Learn to define and use custom reference points for your affine spaces.

**Goal**: Create custom point origins for absolute measurements  
**Time**: ~15 minutes

## What is a Point Origin?

A **point origin** (or **reference origin**) defines the "zero point" of an affine space.

Common examples:

- **_Time_**: Midnight, Unix epoch (Jan 1, 1970), app startup
- **_Temperature_**: Absolute zero, freezing point, room temperature
- **_Position_**: GPS datum, building entrance, vehicle center
- **_Altitude_**: Sea level, ground level, aircraft cruising altitude

!!! note "Implicit vs Explicit Origins"

    In the [previous tutorial](points_and_quantities.md), you used **implicit origins**
    automatically provided by the library when constructing `quantity_point{value * unit}`
    or `point<unit>(value)`. This tutorial teaches you how to define your own
    **explicit origins** for better semantic clarity and domain-specific meaning.


## Absolute Point Origins

When you need meaningful reference points for your domain, define custom **absolute origins**.
Different absolute origins create incompatible types - preventing accidental mixing:

```cpp
// ce-embed height=750 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// Define two unrelated absolute origins
inline constexpr struct unix_epoch final : absolute_point_origin<isq::time> {} unix_epoch;
inline constexpr struct system_boot final : absolute_point_origin<isq::time> {} system_boot;

int main()
{
  using namespace mp_units::si::unit_symbols;

  // Events from Unix epoch (Jan 1, 1970)
  quantity_point unix_event1 = unix_epoch + 11'574 * d;  // ~Sep 2001
  quantity_point unix_event2 = unix_epoch + 17'361 * d;  // ~Jul 2017

  // Events from system boot
  quantity_point boot_event1 = system_boot + 10 * s;     // 10s after boot
  quantity_point boot_event2 = system_boot + 5 * min;    // 5 min after boot

  // Can subtract within same origin
  quantity unix_duration = unix_event2 - unix_event1;
  quantity boot_duration = boot_event2 - boot_event1;

  std::cout << "Unix events separated by: " << unix_duration << "\n";
  std::cout << "Boot events separated by: " << boot_duration << "\n";

  // These would NOT compile - different origins!
  // auto wrong = unix_event1 - boot_event1;  // ❌ Can't mix different origins!
  // auto wrong2 = unix_event1 + boot_event1; // ❌ Can't add points anyway!

  std::cout << "\n✓ Type safety: Unix time and system uptime can't be mixed!\n";
}
```

**Key insight**: Absolute origins create incompatible types that cannot be mixed. This is
intentional - it prevents nonsensical operations like "Unix epoch time minus system boot
time", which would be mathematically valid but semantically meaningless. Each absolute
origin defines its own affine space.

## Relative Point Origins and Conversions

**Relative origins** are defined in terms of other origins, enabling conversions between
different reference frames. All relative origins branching from the same absolute origin
can convert between each other (through that common base):

```cpp
// ce-embed height=900 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// Absolute origin: UTC midnight
inline constexpr struct utc final : absolute_point_origin<isq::time> {} utc;

// Multiple relative origins branching from UTC
inline constexpr struct pst final : relative_point_origin<utc + 8 * si::hour> {} pst; // PST is UTC-8 (behind)
inline constexpr struct jst final : relative_point_origin<utc - 9 * si::hour> {} jst; // JST is UTC+9 (ahead)

int main()
{
  using namespace mp_units::si::unit_symbols;

  // Event at 2 PM PST
  quantity_point pst_event = pst + 14 * h;

  // Convert to UTC reference frame
  quantity_point utc_event = pst_event.point_for(utc);

  // Convert to JST reference frame (goes through the common UTC base)
  quantity_point<si::hour, jst> jst_event = pst_event;

  std::cout << "Same time point in different timezones:\n";
  std::cout << "  PST: " << (pst_event - pst) << " after PST midnight\n";
  std::cout << "  UTC: " << utc_event.quantity_from(utc) << " after UTC midnight\n";
  std::cout << "  JST: " << jst_event.quantity_ref_from(jst) << " after JST midnight\n";
  std::cout << "(2 PM PST = 10 PM UTC = 7 AM JST next day)\n\n";

  std::cout << "Both from UTC:\n";
  std::cout << "  PST: " << pst_event.quantity_from(utc) << "\n";
  std::cout << "  JST: " << jst_event.quantity_from_zero() << "\n\n";

  // Prove they're the same point: subtract them
  quantity time_diff = jst_event - pst_event;
  std::cout << "Time difference: " << time_diff << "\n";
  std::cout << "(Zero difference proves same moment in time)\n\n";

  std::cout << "\n✓ All relative origins sharing the same absolute base can interoperate!\n";
}
```

**Key insight**: Relative origins establish mathematical relationships between reference frames.
All origins branching from the same absolute origin share the same affine space, which means:

- **Arithmetic just works**: You can subtract points from different origins
- **Flexible queries**: Use `.quantity_from(origin)` to get the quantity from any origin
  in the same space, or `.quantity_from(other_point)` to avoid sign issues with subtraction
  order
- **Implicit conversions**: Points from different related origins convert automatically;
  use `.point_for(origin)` to explicitly change the internal reference point if needed

## Challenges

1. **Two absolute origins**: Define `sea_level` and `ground_level` as absolute origins for
   `isq::altitude`. Create _altitude_ measurements from each. Try to subtract points from
   different origins - observe the compile error showing they're incompatible.

2. **Relative origin conversion**: Define `sea_level` as absolute origin and `airport_ground`
   as relative origin (`sea_level + 500 * si::metre`). Create a point for aircraft at
   2000 m above airport ground, then use `.quantity_from(sea_level)` to get the altitude
   above sea level.

3. **Practice arithmetic**: Create two _altitude_ points from different relative origins
   (both based on the same absolute origin). Subtract them to prove they can interoperate,
   and use `.quantity_from_zero()` to show they reference the same absolute origin.

## What You Learned?

✅ Point origins define the "zero point" of measurements  
✅ Absolute origins create incompatible types (prevents mixing)  
✅ Relative origins enable conversions between reference frames  
✅ Use `.point_for(origin)` to convert between related origins  
✅ Built-in origins: `unix_epoch`, `absolute_zero`, etc.  
