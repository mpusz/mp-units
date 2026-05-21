# Bounded Quantity Points

Attach domain constraints to point origins so the library enforces valid ranges automatically.

**Goal**: Use bounds policies on point origins to keep `quantity_point` values within
physical limits  
**Time**: ~15 minutes

**Prerequisites**: Complete the [Point Origins](point_origins.md) tutorial first

## The Problem: Invalid Values at the Wrong Layer

You have learned how to define custom point origins. But many of those origins have
**physically meaningful bounds**. A drone's altitude above ground cannot be negative.
A latitude cannot exceed ±90°. A body-temperature sensor should reject 51 °C at the
API boundary, not silently propagate it downstream.

Without additional tooling every function that receives a `quantity_point` would need
its own range check — and those checks would inevitably be duplicated, forgotten, or
bypassed. **mp-units** lets you attach the constraint _once_ to the origin and rely on
the library to enforce it.

## Passing Bounds to Point Origins

Pass a **bounds policy** as a template parameter to your origin
to attach range validation:

```cpp
#include <mp-units/core.h>

inline constexpr struct your_origin :
    absolute_point_origin<your_quantity_spec, clamp_to_range{min_val, max_val}> {} your_origin;
```

The library calls the policy automatically every time a `quantity_point` bound to that
origin is constructed or mutated. No `if-else` in application code is needed.

## Six Built-In Policies

Six concrete policies are available, all in `<mp-units/overflow_policies.h>`:

| Policy                       | Behaviour                                                         |
|------------------------------|-------------------------------------------------------------------|
| `clamp_to_range{min, max}`   | Saturate to nearest boundary (silent correction)                  |
| `wrap_to_range{min, max}`    | Cyclic modulo into `[min, max)`, e.g. angles                      |
| `reflect_in_range{min, max}` | Bounce/fold at both boundaries, e.g. latitude                     |
| `check_in_range{min, max}`   | Report a violation via contract / handler                         |
| `check_non_negative{}`       | Report a violation when the value drops below zero                |
| `clamp_non_negative{}`       | Saturate to zero for values below zero (tolerates rounding noise) |

Bounds are expressed as `quantity` **displacements** from the origin, not as absolute
`quantity_point` values. A policy written in one unit works equally for any unit the
`quantity_point` uses — bounds in metres apply transparently to a point in kilometres.

The last two are half-line policies for inherently non-negative domains. They carry no
upper bound — only a `min` member (the natural zero).

In general, whenever a policy exposes a `min` or `max` member, `quantity_point::min()`,
`quantity_point::max()`, and the corresponding `std::numeric_limits` functions reflect
those bounds instead of the representation type's own extremes.

## Try It: Drone Altitude Zones

```cpp
// ce-embed height=880 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr struct sea_level :
    absolute_point_origin<isq::altitude, clamp_to_range{-500 * m, 12'000 * m}> {} sea_level;
inline constexpr struct ground_level :
    absolute_point_origin<isq::altitude, clamp_to_range{0 * m, 500 * m}> {} ground_level;

int main()
{
  quantity_point cruise   = sea_level    + 8'500.0 * m;
  quantity_point too_high = sea_level    + 15'000.0 * m;   // clamped!
  quantity_point hover    = ground_level + 50.0 * m;
  quantity_point below    = ground_level - 10.0 * m;       // clamped!

  std::cout << "Cruise MSL:  " << (cruise   - sea_level)    << "\n";   // 8500 m
  std::cout << "Too-high:    " << (too_high - sea_level)    << "\n";   // 12000 m ← clamped
  std::cout << "Hover AGL:   " << (hover    - ground_level) << "\n";   // 50 m
  std::cout << "Below AGL:   " << (below    - ground_level) << "\n";   // 0 m ← clamped
}
```

**Key insight**: Bounds are enforced at the point of construction. `too_high` is never
15'000 m — it is already clamped to 12'000 m at the moment it is first assigned.

## `min()` and `max()` Reflect the Bounds

When bounds are defined the `quantity_point` type exposes static `min()` and `max()`
functions and a matching `std::numeric_limits` specialization:

```cpp
using msl = quantity_point<si::metre, sea_level, double>;

static_assert(msl::min().quantity_from(sea_level) == -500.0    * m);
static_assert(msl::max().quantity_from(sea_level) == 12'000.0  * m);
```

Without bounds, `min()` and `max()` are only available if the representation type itself
provides them.

## Origin Inheritance

A `relative_point_origin` that defines **no own bounds** automatically inherits the
parent's enforcement. When own bounds are provided they are checked at compile time to
nest strictly inside the parent's range:

```cpp
// Relative origin — no own bounds; inherits clamping from sea_level.
inline constexpr struct airport_elevation : relative_point_origin<sea_level + 200 * m> {} airport_elevation;

// Value 12100 m MSL would exceed the sea_level cap of 12000 m → clamped.
quantity_point takeoff = airport_elevation + 11'900.0 * m;
```

## Automatic Bounds for Non-Negative Quantities

Many quantity specifications are inherently non-negative — _length_, _mass_, _duration_,
_speed_, _area_, etc. The ISQ base quantities `isq::length`, `isq::mass`, `isq::duration`,
`isq::thermodynamic_temperature`, `isq::amount_of_substance`, and `isq::luminous_intensity`
are all tagged `non_negative` in the library. The library tracks this through the
`is_non_negative()` query:

```cpp
static_assert(is_non_negative(isq::length));         // ✅ tagged in ISQ
static_assert(is_non_negative(isq::mass));           // ✅ tagged in ISQ
static_assert(is_non_negative(isq::speed));          // ✅ derived: length / duration
static_assert(!is_non_negative(isq::displacement));  // ❌ vector character — excluded
```

When a `quantity_point` uses a `natural_point_origin` whose quantity spec is non-negative,
the library **automatically attaches `check_non_negative`** — no explicit bounds
definition is required:

```cpp
// ce-embed compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr struct distance_traveled : quantity_spec<isq::length> {} distance_traveled;

int main()
{
  // natural_point_origin<distance_traveled> is auto-protected by check_non_negative
  // because it inherits non_negative from isq::length.
  quantity_point start(distance_traveled(0.0 * m));    // fine: 0 m is the boundary
  quantity_point finish(distance_traveled(5.0 * km));  // fine: 5 km ≥ 0

  std::cout << "Start:  " << start << "\n";
  std::cout << "Finish: " << finish << "\n";

  // quantity_point bad(distance_traveled(-1.0 * cm));  // ❌ contract violation
  // std::cout << "Bad:    " << bad << "\n";
}
```

You can override the default by defining a custom origin with different bounds. For example,
to silently clamp rounding-noise negatives in a computed result rather than failing:

```cpp
inline constexpr struct clamped_distance_origin :
    absolute_point_origin<distance_traveled, clamp_non_negative{}> {} clamped_distance_origin;
```

### Non-Negative Bounds Through Relative Origins

The same inheritance applies when the parent bounds come from the automatic
`check_non_negative` protection.  A relative origin sitting *above* the ground
floor may hold negative values — as long as the absolute position stays ≥ 0:

```cpp
// inherits non_negative from ISQ
inline constexpr struct height_spec : quantity_spec<isq::height> {} height_spec;

// natural_point_origin<height_spec> auto-gets check_non_negative — no explicit bounds needed.
// The relative origin at +1700 m inherits that constraint.
inline constexpr struct average_height_origin :
    relative_point_origin<natural_point_origin<height_spec> + 1700.0 * m> {} average_height_origin;

// −1500 m relative = 200 m absolute (≥ 0) → valid.
quantity_point low    = average_height_origin - 1500.0 * m;

// −1700 m relative = 0 m absolute → valid (boundary).
quantity_point ground = average_height_origin - 1700.0 * m;

// −1701 m relative = −1 m absolute → check_non_negative fires.
// quantity_point bad = average_height_origin - 1701.0 * m;  // ❌
```

The enforcement is a single flat translation: the library converts the relative value
to the absolute frame, applies `check_non_negative` there, then converts back.  The
result is always the original value (no clamping), so the violation is *reported* but
the stored value is unchanged — unless you use a `constrained<T, Policy>` representation
that throws or terminates unconditionally.

### Kinds are never auto-bounded

`kind_of<QS>` represents the *entire* quantity tree rooted at `QS` — including vector
quantities and signed coordinates. It is therefore never `non_negative`, even when `QS`
itself is:

```cpp
static_assert(!is_non_negative(kind_of<isq::length>));  // kind encompasses signed subtypes
```

This matters in practice because **CTAD from a bare SI unit** deduces `kind_of`:

```cpp
// quantity_point{5.0 * m} has origin = natural_point_origin<kind_of<isq::length>>.
// isq::length is tagged non_negative, but that origin is NOT auto-bounded because
// kind_of<isq::length> includes displacement and other signed/vector subtypes.
quantity_point generic_len(5.0 * m);               // ← no auto-check, kind_of origin
quantity_point dist(distance_traveled(5.0 * m));   // ← auto-checked, named spec
```

Always use an explicit quantity reference when you need the auto-protection guarantee.

## Challenges

1. **Coordinate system**: Define an `equator` absolute origin for `isq::angular_measure`.
   Attach `reflect_in_range{-90 * deg, 90 * deg}` and verify that constructing a point
   at 95° gives 85°.

2. **Wrapping longitude**: Define a `prime_meridian` absolute origin. Attach
   `wrap_to_range{-180.0 * deg, 180.0 * deg}` and verify that 200° wraps to −160°.

3. **Nested bounds**: Define `airport_elev` as
   `relative_point_origin<sea_level + 1'000 * m>`. Attach
   `clamp_to_range{-50 * m, 4'000 * m}` to `airport_elev`. Verify that 4100 m above
   the airport clamps to 4000 m.

## What You Learned

✅ Passing a bounds policy as a template parameter to a point origin attaches validation
    once  
✅ Six built-in policies: `clamp_to_range`, `wrap_to_range`, `reflect_in_range`,
    `check_in_range`, `check_non_negative`, `clamp_non_negative`  
✅ Bounds are expressed as quantity displacements — independent of unit or representation  
✅ Relative origins inherit bounds from the nearest ancestor that has them  
✅ Non-negative ISQ specs automatically guard their natural origins with
    `check_non_negative`  
✅ You can define custom origins with different bounds to override the defaults  

## Next Steps

**Ready for more control?** Learn how to customize what happens when bounds are violated:

<!-- markdownlint-disable MD013 -->
- [Tutorial: Custom Contract Handlers](custom_contract_handlers.md) - Implement custom error policies
  (logging, recovery, auditing) for `constrained<T, ErrorPolicy>` representation types
- [Workshop: Bounds Validation](../../workshops/advanced/bounds_validation.md) - Practice building
  a weather monitoring system with custom error handling and violation tracking
<!-- markdownlint-enable MD013 -->
