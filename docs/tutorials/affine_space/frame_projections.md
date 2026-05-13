# Frame Projections

Convert `quantity_point` objects between independent reference frames.

**Goal**: Define `frame_projection` specializations and use `point_for` across unrelated origins  
**Time**: ~20 minutes

**Prerequisites**: Complete the [Point Origins](point_origins.md) tutorial first

## The Problem: Origins That Don't Share a Root

The [Point Origins tutorial](point_origins.md) showed that `relative_point_origin` lets
you define and convert between origins that share the same absolute root — for example,
`pst` and `jst` both rooted at `utc`.

But some real-world relationships **cannot** be expressed as a compile-time offset:

| Relationship                | Why `relative_point_origin` fails                             |
|:----------------------------|:--------------------------------------------------------------|
| Altitude ↔ Depth            | Same magnitude, opposite sign — `depth = −altitude`           |
| Geometric azimuth ↔ Bearing | `bearing = 90° − azimuth` involves negation, not a pure shift |
| World frame ↔ Camera frame  | Transformation matrix known only at runtime                   |

All three involve two **independent** `absolute_point_origin` types connected by a
user-defined formula. This is what `frame_projection` is for.


## Try It: Altitude ↔ Depth

In aviation/oceanography, _altitude_ is measured upward from sea level (positive = up),
while _depth_ is measured downward from the ocean surface (positive = down). The two axes
point in opposite directions — a pure offset cannot capture that.

Register a `frame_projection` by specializing a variable template in `namespace mp_units`
with a callable for each conversion direction:

```cpp
// ce-embed height=880 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

inline constexpr struct sea_level : absolute_point_origin<isq::altitude> {} sea_level;
inline constexpr struct ocean_surface : absolute_point_origin<isq::altitude> {} ocean_surface;

// Altitude → depth: negate (down is positive in the depth frame)
template<>
inline constexpr auto mp_units::frame_projection<sea_level, ocean_surface> =
    [](QuantityPointOf<isq::altitude> auto qp) { return ocean_surface - qp.quantity_from(sea_level); };

// Depth → altitude: inverse (must be defined explicitly — never derived automatically)
template<>
inline constexpr auto mp_units::frame_projection<ocean_surface, sea_level> =
    [](QuantityPointOf<isq::altitude> auto qp) { return sea_level - qp.quantity_from(ocean_surface); };

int main()
{
  using namespace mp_units::si::unit_symbols;

  quantity_point altitude = sea_level + (-100. * m);            // 100 m below sea level
  quantity_point depth    = altitude.point_for(ocean_surface);  // → depth 100 m (positive downward)
  quantity_point alt2     = depth.point_for(sea_level);         // → altitude −100 m

  std::cout << "altitude from sea_level:   " << altitude.quantity_from(sea_level) << "\n";
  std::cout << "depth from ocean_surface:  " << depth.quantity_from(ocean_surface) << "\n";
  std::cout << "back to sea_level:         " << alt2.quantity_from(sea_level) << "\n";
}
```

**Key insight**: Call `.point_for(target_origin)` exactly as you would for same-origin
conversions — the library finds the registered `frame_projection` automatically. Both
directions must be defined explicitly; the library never derives an inverse on your behalf.

!!! note "Where to put the specializations"

    `frame_projection` lives in `namespace mp_units`. Explicit specializations must be in a
    namespace that encloses the primary template — either inside
    `namespace mp_units { ... }` or at global scope using qualified names, as shown above.


## Try It: Chaining Through a Relative Origin

If the target is a `relative_point_origin` whose absolute root is reachable via a
`frame_projection`, the library projects to that root first and then walks the
relative-origin chain automatically:

```cpp
// ce-embed height=800 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

inline constexpr struct sea_level : absolute_point_origin<isq::altitude> {} sea_level;
inline constexpr struct ocean_surface : absolute_point_origin<isq::altitude> {} ocean_surface;

template<>
inline constexpr auto mp_units::frame_projection<sea_level, ocean_surface> =
    [](QuantityPointOf<isq::altitude> auto qp) { return ocean_surface - qp.quantity_from(sea_level); };

// A relative origin 10 m below ocean_surface in the depth frame
inline constexpr struct shallow_water : relative_point_origin<ocean_surface + 10. * si::metre> {} shallow_water;

int main()
{
  using namespace mp_units::si::unit_symbols;

  quantity_point altitude     = sea_level + (-100. * m);           // 100 m below sea level
  quantity_point from_shallow = altitude.point_for(shallow_water);

  // Steps performed internally:
  //   1. Project sea_level → ocean_surface  (depth = 100 m)
  //   2. Walk down ocean_surface → shallow_water (100 − 10 = 90 m)
  std::cout << "depth from shallow_water: " << from_shallow.quantity_from(shallow_water) << "\n";
}
```

**Key insight**: The automatic walk-down is the same compile-time `relative_point_origin`
chain traversal used by ordinary same-origin `point_for`. A `frame_projection` only needs
to target the absolute root — the library handles the rest of the tree automatically.


## Try It: Runtime Arguments

Extra arguments passed to `point_for` are forwarded directly to the projection functor.
This lets the projection depend on data that varies at the call site — no global state
required:

```cpp
// ce-embed height=880 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

inline constexpr struct world_frame : absolute_point_origin<isq::length> {} world_frame;
inline constexpr struct camera_frame : absolute_point_origin<isq::length> {} camera_frame;

struct CameraCalibration {
  quantity<isq::length[si::metre]> offset;
};

template<>
inline constexpr auto mp_units::frame_projection<world_frame, camera_frame> =
    [](QuantityPointOf<isq::length> auto qp, const CameraCalibration& cal) {
        return camera_frame + (qp.quantity_from(world_frame) + cal.offset);
    };

int main()
{
  using namespace mp_units::si::unit_symbols;

  quantity_point world_pt = world_frame + 5.0 * m;

  // Two different cameras, two different calibrations:
  CameraCalibration cam1{.offset = -2.0 * m};
  CameraCalibration cam2{.offset =  1.0 * m};

  quantity_point in_cam1 = world_pt.point_for(camera_frame, cam1);
  quantity_point in_cam2 = world_pt.point_for(camera_frame, cam2);

  std::cout << "cam1: " << in_cam1.quantity_from(camera_frame) << "\n";  // 3 m
  std::cout << "cam2: " << in_cam2.quantity_from(camera_frame) << "\n";  // 6 m
}
```

**Key insight**: The extra `cal` argument travels through `point_for` into the lambda
without any wrapping or global state. This makes each call site independent and the
functor stateless — important for thread-safety and testability. When runtime arguments
are needed, `.point_for()` is the only option; the explicit constructor (below) accepts
no extra arguments.


## Explicit Constructor Shorthand

When the destination type is spelled out explicitly and no runtime arguments are needed,
the `quantity_point` explicit constructor is a compact alternative to `.point_for()`:

```cpp
// Requires frame_projection<sea_level, ocean_surface> to be defined
quantity_point<isq::altitude[si::metre], ocean_surface> depth{altitude};
// identical to: altitude.point_for(ocean_surface)
```

The constructor is always `explicit` — accidental implicit conversions across independent
origins are never permitted. Chain walk-down through relative origins works exactly as
with `point_for`.

Use `.point_for()` when you prefer to let the compiler deduce the destination type, or when
runtime arguments must be forwarded to the functor.


## Challenges

1. **Bearing ↔ Azimuth**: Define `north_cw` (bearing, N/CW+) and `east_ccw` (geometric
   azimuth, E/CCW+) as independent absolute origins for `isq::angular_measure`. Implement
   `frame_projection` for `bearing = 90° − azimuth` and verify that 0° azimuth maps to
   90° bearing.

2. **QNH altitude**: In aviation, _pressure altitude_ is computed from the barometric sensor
   alone; _MSL altitude_ (QNH altitude) corrects for the current altimeter setting (QNH —
   barometric pressure normalized to mean sea level).
   Define `pressure_altitude` and `msl_altitude` as independent absolute origins for
   `isq::altitude`. Specialize `frame_projection` so that the conversion accepts a runtime
   QNH value (in hPa) and applies the simplified correction
   `msl_alt ≈ pressure_alt + (qnh − 1013.25 hPa) × 8.5 m/hPa`. Verify that with
   QNH = 1023.25 hPa a pressure altitude of 1000 m reads 1085 m MSL. Could this have been
   implemented as a `relative_point_origin`? Why not?

3. **Walk-down chain**: Starting from challenge 2, define `circuit_height` as a
   `relative_point_origin<msl_altitude + 300 * m>` (traffic pattern altitude). Use
   `.point_for(circuit_height, qnh)` on a pressure-altitude point and verify that the
   QNH correction and the 300 m walk-down are both applied.


## What You Learned

✅ `frame_projection` connects independent `absolute_point_origin` types via user callables  
✅ Specialize `mp_units::frame_projection<From, To>` with a callable at global scope  
✅ Call `.point_for(target_origin)` the same way as for same-origin conversions  
✅ Walk-down through `relative_point_origin` chains is automatic after projection  
✅ Both directions must always be defined explicitly — no automatic inverse  
✅ Extra `point_for` arguments are forwarded to the functor for runtime parameters  
✅ The explicit constructor works for no-arg projections and is never implicit  
