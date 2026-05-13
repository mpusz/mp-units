---
date: 2026-04-06
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Range-Validated Quantity Points

Physical units libraries have always been very good at preventing dimensional
errors and unit mismatches. But there is a category of correctness that they have
universally ignored: **domain constraints on quantity point values**.

A _latitude_ is not just a _length_ divided by a _radius_. It is a value that lives in
$[-90°, +90°]$; anything outside that range is physically meaningless. An angle
used in bearing navigation wraps cyclically around a circle; treating it as an
unbounded real number ignores a fundamental property of the domain. A clinical
body-temperature sensor should reject a reading of $44\ \mathrm{°C}$ at the API
boundary, not silently pass it downstream.

Type-level constraint enforcement for quantity points with this level of
flexibility is a relatively unexplored area in mainstream physical units libraries.
The approach we present here is novel and experimental — we are certain there are
edge cases and design considerations we haven't yet discovered.

This article describes the motivation in depth, the design we arrived at, and
the open questions we would love the community's help to answer.

<!-- more -->

## The Problem

### Coordinates and angles in practice

The immediate trigger for this work was a feedback during ISO C++ Committee
working meeting in Croydon and a
[discussion opened by a user](https://github.com/mpusz/mp-units/discussions/782)
who works with geodetic coordinate systems:

!!! quote "How to handle type level difference in coordinate/orientation angle types?"

    We've now used mp-units for a long time, and while we use it for a lot of other
    units, we've not found it possible to use practically with basically anything
    that deals with world coordinates/orientation and coordinate-based angles of measure.

**mp-units** alone — like every other units library — had no way to encode the
different wrapping disciplines that coordinate angles require.

In brief, the three families of domain constraint they needed are:

| Domain                                      | Rule                                      | Behavior when violated             |
|---------------------------------------------|-------------------------------------------|------------------------------------|
| _Latitude_ / _elevation_ on a sphere        | $[-90°, +90°]$ — _reflected_ at the poles | $91° \to 89°$, $270° \to -90°$ ¹   |
| _Longitude_ / _azimuth_ (signed convention) | $(-180°, +180°]$ — _wraps_ cyclically     | $200° \to -160°$, $-200° \to 160°$ |
| _Longitude_ (positive-only convention)      | $[0°, 360°)$ — _wraps_ cyclically         | $370° \to 10°$                     |

¹ **Simplified.** True geodetic latitude reflection also requires shifting
longitude by 180° (crossing a pole puts you on the opposite side of the globe).
We discuss this coupled-axis limitation in
[Polar coordinates and coupled constraints](#polar-coordinates-and-coupled-constraints)
below.

These three rows are genuinely distinct behaviors — reflect, wrap with a signed
interval, wrap with an unsigned interval — not the same rule in different units.
On top of that, mixed _azimuth_/_bearing_ systems require a numeric offset
between reference frames (_heading_ $= 90° -$ _geometric azimuth_), which is
handled by `relative_point_origin`, while the _range enforcement_ on each origin
remains independent. Encoding all of this without leaking raw-integer boilerplate
into application code requires first-class support in the library.

### Domains that need this pattern

Geodesy is far from the only domain. Some representative examples:

| Quantity                | Origin                 | Constraint                                          |
|-------------------------|------------------------|-----------------------------------------------------|
| _Latitude_              | equator                | clamp or reflect to $[-90°, 90°]$                   |
| _Longitude_             | prime meridian         | wrap to $(-180°, 180°]$                             |
| _Body temperature_      | clinical sensor origin | clamp to $[35°C, 42°C]$                             |
| _Flight altitude_ (MSL) | sea level              | clamp to $[-500\ \mathrm{m},\ 12\,000\ \mathrm{m}]$ |
| _Drone altitude_ (AGL)  | terrain level          | clamp to $[0\ \mathrm{m},\ 500\ \mathrm{m}]$        |
| _Time of day_           | midnight               | wrap to $[0\ \mathrm{s},\ 86400\ \mathrm{s})$       |
| AC thermostat setpoint  | reference temperature  | clamp offset to $±3\ \mathrm{°C}$                   |
| Particle _phase angle_  | any reference          | reflect at $[-π, +π]$                               |
| _Heading_ / _bearing_   | geographic north       | wrap to $(-180°, 180°]$ _or_ $[0°, 360°)$           |

What all of these have in common is:

1. There is an **origin** — a reference point that defines the frame of
   measurement.
2. The _displacement_ from that origin is **physically bounded**.
3. The policy for handling out-of-bound values differs per domain: sometimes
   we clamp, sometimes we wrap, sometimes we reflect, sometimes we want a
   hard error.

The existing abstraction of `quantity_point` already captures the origin.  
The missing piece is attaching a policy to the origin and having the library
enforce it automatically.

---

## The Design

### Bounds live on the origin, not on the type

The key design decision is that bounds are a property of the **origin**, not of
the (quantity, unit, rep) triple. This follows from the observation that the
numerical displacement from an origin is bounded by the physics of that origin's
frame, not by the choice of unit or representation.

!!! note

    Strictly speaking, a bounded domain is not a true affine space — an affine
    space has no notion of "out of range."  The `quantity_point` abstraction is
    still modelled on the affine-space pattern (origin + displacement), but the
    bounds enforcement is an additional layer on top of it.  Think of it as an
    affine-space *skeleton* decorated with domain-specific constraints.

Concretely, bounds are passed as a **template parameter** to the origin:

```cpp
inline constexpr struct horizon :
    absolute_point_origin<geo_elevation, reflect_in_range{-90 * deg, 90 * deg}> {} horizon;
```

Because the bounds are a non-type template parameter, they add no runtime
storage — the only runtime cost is the enforcement call itself.

### Bounds values are deltas, not points

A natural question to ask is: should the bounds be expressed as `quantity_point`
values (absolute positions) or as `quantity` values (displacements)?

They are displacements. The reason is architectural:

- The bounds enforcement machinery operates on a `quantity<R, Rep>` (the displacement
  `quantity_from(origin)`) and returns a corrected one. Using point values would
  require embedding a reference origin in the bounds object, entangling the policy
  with the origin hierarchy it is being applied to.
- Relative origins whose bounds are expressed relative to the offset also use
  displacements naturally — there is no notion of "absolute position" in a frame
  that is defined by its offset from a parent.

### Six policies

Six concrete policies ship out of the box; all live in
`<mp-units/overflow_policies.h>`.

Four two-sided policies operate on a closed interval `[min, max]`:

```cpp
// 1. Saturate at the boundaries — silently corrects the value.
template<Quantity Q>
struct clamp_to_range { Q min; Q max; };

// 2. Cyclic wrapping into the half-open interval [min, max).
template<Quantity Q>
struct wrap_to_range { Q min; Q max; };

// 3. Bounce/fold at both boundaries — models physical reflection.
template<Quantity Q>
struct reflect_in_range { Q min; Q max; };

// 4. Report violations — delegates to the representation's error policy.
template<Quantity Q>
struct check_in_range { Q min; Q max; };
```

Two one-sided policies cover the half-line $[0, +\infty)$ for inherently
non-negative quantities:

```cpp
// 5. Report violations if value < 0.
struct check_non_negative { /* zero_quantity_t min; */ };

// 6. Clamp negative values to zero — tolerates floating-point rounding noise.
struct clamp_non_negative { /* zero_quantity_t min; */ };
```

`check_non_negative` and `clamp_non_negative` are **automatically attached** to
any `quantity_point` whose origin is a `natural_point_origin<QS>` for a
non-negative quantity spec (e.g., `isq::length`, `isq::mass`, `isq::duration`).
No explicit bounds definition is required in the typical case.

The first three two-sided policies and `clamp_non_negative` are _error-correcting_:
they silently adjust the value. `check_in_range` and `check_non_negative` are
_error-reporting_ policies: each either calls
`constraint_violation_handler<Rep>::on_violation()` (when the representation type
opts into guaranteed enforcement by specializing that trait) or falls back to
[`MP_UNITS_EXPECTS`](../../how_to_guides/integration/wide_compatibility.md#contract-checking-macros)
otherwise.

All six policies are **function objects**: their `operator()` is a function
template that accepts any compatible `Quantity`, so a bounds object defined with
one unit and representation works equally well regardless of the unit or
representation used by the `quantity_point`. For example, bounds expressed in
seconds apply transparently to a `quantity_point` expressed in hours or
milliseconds. This means a single bounds definition on an origin does not need
to be repeated for every unit a user might choose.

### Custom policies

The six built-in policies are not a closed set. Because a policy is just a
callable that takes and returns a `Quantity`, users can write their own.

**Half-line bounds.** Not every constraint is a closed interval. A hydraulic
system that must maintain at least 50 bar above ambient needs only a lower
bound; a sensor with a ceiling and no floor needs only an upper bound:

```cpp
// lower bound only; upper end is unconstrained
template<Quantity Q>
struct clamp_bottom {
  Q min;
  template<Quantity V>
  constexpr V operator()(V v) const { if (v < V{min}) return V{min}; return v; }
};

// Hydraulic circuit: minimum operating pressure 50 bar above ambient; no upper cap here.
inline constexpr struct ambient_pressure :
    absolute_point_origin<isq::pressure, clamp_bottom{50 * bar}> {} ambient_pressure;
```

**Tolerance-aware clamping.** In realistic floating-point calculations a value
may end up slightly outside the valid range due to accumulated rounding error.
A user may want to clamp $-10^{-12}\ \mathrm{kg}$ to zero while treating
$-1\ \mathrm{kg}$ as a genuine error:

```cpp
template<Quantity Q>
struct tolerance_clamp {
  Q min;
  Q max;
  Q tolerance;
  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < V{min} && v >= V{min} - V{tolerance}) return V{min};
    if (v > V{max} && v <= V{max} + V{tolerance}) return V{max};
    // outside tolerance — delegate to contract checking
    MP_UNITS_EXPECTS(v >= V{min} && v <= V{max});
    return v;
  }
};
```

### Origin inheritance

A `relative_point_origin` that defines **no own bounds** automatically inherits
the enforcement from its nearest ancestor that has bounds:

```cpp
// Absolute origin with physical bounds:
inline constexpr struct prime_meridian :
    absolute_point_origin<geo_longitude, wrap_to_range{-180.0 * deg, 180.0 * deg}> {} prime_meridian;

// Relative origin — no own bounds; inherits from prime_meridian.
// The +21° offset is transparent to the enforcement.
inline constexpr struct warsaw_meridian : mp_units::relative_point_origin<prime_meridian + 21.0 * deg> {
} warsaw_meridian;

// A value of +200° east of Warsaw = +221° from prime → wraps to -139° from prime → -160° from Warsaw.
quantity_point qp = warsaw_meridian + 200.0 * deg;
assert(qp.quantity_from(warsaw_meridian) == -160.0 * deg);
```

When a `relative_point_origin` defines **its own** bounds, those bounds are
enforced directly (the tighter constraint wins). A compile-time
`static_assert` verifies that the relative bounds nest strictly inside the
parent's bounds:

```cpp
// static_assert fires at compile time if relative bounds exceed parent bounds
inline constexpr struct ac_setpoint :
    relative_point_origin<reference_temp + 0 * deg_C,
                          clamp_to_range{delta<deg_C>(-3), delta<deg_C>(+3)}> {} ac_setpoint;
    // ❌ compile error if this would violate the parent origin's physical bounds
```

### The `static_assert` at definition time

The library validates bounds at the point where the origin is first
instantiated by enforcing the following, in order:

1. The bounds object has at least one of `.min` or `.max` — a bare `{}` is
   rejected.
2. For relative origins, if the parent has bounds: the relative bounds (translated
   by the cumulative offset) must nest strictly inside the parent’s range.

Both checks are **compile-time** `static_assert`s. They fire exactly once per
origin definition regardless of how many `quantity_point` variables are constructed.

### Full example: geodetic coordinate types

```cpp
#include <mp-units/core.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr struct geo_latitude : quantity_spec<isq::angular_measure> {} geo_latitude;
inline constexpr struct geo_longitude : quantity_spec<isq::angular_measure> {} geo_longitude;

// Absolute origins
inline constexpr struct equator : absolute_point_origin<geo_latitude>  {} equator;
inline constexpr struct prime_meridian : absolute_point_origin<geo_longitude> {} prime_meridian;
```

Outside the anonymous namespace (so the specializations have external linkage):

```cpp
template<>
inline constexpr auto mp_units::quantity_bounds<equator> =
    mp_units::reflect_in_range{-90.0 * deg, 90.0 * deg};   // simplified — see note below

template<>
inline constexpr auto mp_units::quantity_bounds<prime_meridian> =
    mp_units::wrap_to_range{-180.0 * deg, 180.0 * deg};    // longitude wraps cyclically
```

!!! warning "Simplification"

    The `reflect_in_range` policy on _latitude_ is a single-axis approximation.
    In true geodesy, reflecting _latitude_ at a pole also requires shifting
    _longitude_ by 180° — the two axes are coupled.  The current per-origin
    bounds model cannot express this; see
    [Polar coordinates and coupled constraints](#polar-coordinates-and-coupled-constraints)
    for discussion.

```cpp
// Usage — bounds enforced transparently on construction and assignment.
using latitude  = quantity_point<si::degree, equator,        double>;
using longitude = quantity_point<si::degree, prime_meridian, double>;

constexpr latitude lat  = equator + 91.0 * deg;          // reflects → 89°
constexpr longitude lon = prime_meridian + 200.0 * deg;  // wraps   → -160°

static_assert(lat.quantity_from(equator)        == 89.0  * deg);
static_assert(lon.quantity_from(prime_meridian) == -160.0 * deg);
```

### Full example: bounded-altitude drone types

```cpp
// MSL: physical world — flight level corridor.
inline constexpr struct sea_level :
    absolute_point_origin<isq::altitude, clamp_to_range{-500 * m, 12'000 * m}> {} sea_level;

// AGL: operational drone envelope (non-negative).
inline constexpr struct ground_level :
    absolute_point_origin<isq::altitude, clamp_to_range{0 * m, 500 * m}> {} ground_level;
```

```cpp
using msl_altitude = quantity_point<altitude[m], sea_level,    double>;
using agl_altitude = quantity_point<altitude[m], ground_level, double>;

constexpr auto cruising = msl_altitude{8'000.0 * m, sea_level};   // fine
constexpr auto too_high = msl_altitude{15'000.0 * m, sea_level};  // clamped to 12000 m
constexpr auto below    = agl_altitude{-10.0 * m, ground_level};  // clamped to 0 m
```

### `min()`, `max()`, and `std::numeric_limits`

When bounds are defined on the origin, the `quantity_point` static member
functions and the `std::numeric_limits` specialization reflect those bounds:

```cpp
static_assert(longitude::min().quantity_from(prime_meridian) == -180.0 * deg);
static_assert(longitude::max().quantity_from(prime_meridian) ==  180.0 * deg);
static_assert(std::numeric_limits<longitude>::lowest().quantity_from(prime_meridian) == -180.0 * deg);
```

For unbounded `quantity_point` types the functions are conditionally present:
`min()`/`max()` are only callable when the respective bound exists (or the
representation type provides them), and `lowest()` behaves like the
representation's own `lowest()` when there is no lower bound.

### Guaranteed enforcement with `constrained<T, ErrorPolicy>`

When a representation type does not opt into guaranteed enforcement,
`check_in_range` falls back to
[`MP_UNITS_EXPECTS`](../../how_to_guides/integration/wide_compatibility.md#contract-checking-macros),
which in release builds may be compiled out entirely. In that configuration the
function has a narrow contract: the caller is responsible for providing in-range
values, and the library merely helps catch mistakes during development.

For safety-critical code you need **guaranteed enforcement** — a violation that
always fires, independent of build flags. When a representation type *does*
provide a `constraint_violation_handler` (see below), the behavior is
unconditional: the function has a wide contract and always reports
out-of-range values through the handler. There is no precondition to check
because the policy itself defines what happens on violation.

The `constrained<T, ErrorPolicy>` wrapper, provided in `<mp-units/constrained.h>`,
is the answer. It is a thin, transparent value wrapper around `T` that carries
an error policy as a type parameter. It satisfies `std::regular`, forwards all
arithmetic to `T`, and implicitly converts to and from `T` so it fits where `T`
would.

Two built-in policies ship out of the box:

| Policy             | Effect on violation        | Availability             |
|--------------------|----------------------------|--------------------------|
| `throw_policy`     | throws `std::domain_error` | hosted environments only |
| `terminate_policy` | calls `std::abort()`       | freestanding and hosted  |

On hosted targets `throw_policy` is the default; on freestanding targets
`terminate_policy` is the default.

The connection to `check_in_range` is made through the
`constraint_violation_handler` customization point. **mp-units** ships a
specialization for `constrained<T, EP>` that delegates directly to
`EP::on_constraint_violation()`. This means that whenever `check_in_range`
would fire a contract violation it instead calls the EP — regardless of
`MP_UNITS_EXPECTS` settings:

```cpp
// constraint_violation_handler<constrained<T, EP>> ships in constrained.h
template<typename T, typename ErrorPolicy>
struct constraint_violation_handler<constrained<T, ErrorPolicy>> {
  static constexpr void on_violation(std::string_view msg) { ErrorPolicy::on_constraint_violation(msg); }
};
```

A concrete usage example — a body-temperature sensor that must always throw on
an out-of-range reading, even in a release build.

The key design choice here is to define `clinical_zero` as a
`relative_point_origin` anchored to `si::ice_point` at offset 0 °C rather than
as a bare `absolute_point_origin`. This keeps the origin in the same hierarchy
as the rest of the Celsius/Kelvin/Fahrenheit scale, so a `safe_temp` value can
still be converted to Kelvin or Fahrenheit normally. At the same time the bounds
are attached only to `clinical_zero`, so an ordinary
`quantity_point<deg_C, si::ice_point>` for everyday Celsius temperatures is
completely unaffected — the library enforces the 35–42 °C constraint only where
the type says so.

```cpp
#include <mp-units/core.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

inline constexpr struct body_temp : quantity_spec<isq::thermodynamic_temperature> {} body_temp;

// Anchored to ice_point at 0 °C — shares the Celsius/Kelvin hierarchy,
// so unit conversion to K or deg_F still works.
inline constexpr struct clinical_zero :
            relative_point_origin<point<body_temp[deg_C]>(0),
                                  check_in_range{delta<deg_C>(35.), delta<deg_C>(42.0)}> {} clinical_zero;

// Rep is constrained<double> — violations always throw std::domain_error.
using safe_temp = quantity_point<deg_C, clinical_zero, constrained<double>>;

safe_temp reading = clinical_zero + delta<deg_C>(36.6);  // fine
safe_temp fever   = clinical_zero + delta<deg_C>(44.0);  // throws std::domain_error
```

Because `clinical_zero` is rooted in `ice_point`, `reading.quantity_from_unit_zero()`
and `reading.in(deg_F)` both work — the conversion offsets are known. The bounds, however,
only guard `safe_temp`; a plain `quantity_point<deg_C, si::ice_point>` used elsewhere in
the codebase is unconstrained.

Because the bounds object carries `double`-backed quantities and `constrained<double>`
satisfies the same `Quantity` concept requirements, the unit-flexibility
guarantees described above apply here too: you could use `constrained<float>` or
`constrained<int>` as the representation without changing the
bounds definition on the origin.

### Non-negative quantity annotations

[Absolute quantities](introducing-absolute-quantities.md) are quantities that
live on a ratio scale — always measured from a natural zero — such as _mass_,
_duration_, or _electric charge magnitude_. Non-negativity is the canonical
constraint for all of them, and **mp-units** now implements it at the
quantity-specification level.

The `non_negative` flag can be applied to any real-scalar base or named quantity spec.
Each non-negative root must declare the tag explicitly, and named real-scalar children
inherit it transitively from their parent:

```cpp
static_assert(is_non_negative(isq::length));     // ✅ explicit tag in ISQ definition
static_assert(is_non_negative(isq::mass));       // ✅ explicit tag in ISQ definition
static_assert(is_non_negative(isq::speed));      // ✅ explicit tag in ISQ definition
static_assert(!is_non_negative(isq::velocity));  // ❌ vector character — excluded
```

The flag is **never inferred automatically** from equation factors — even when all
factors are non-negative, the defining equation captures only dimensional relationships,
not the full sign domain of the physical quantity. For example, _reactive power_ is
defined via $Q = U \cdot I \cdot \sin\varphi$, and the _Massieu function_ as $J = -A/T$
— both have all-non-negative dimensional factors yet can take negative values in practice:

```cpp
// All dimensional factors are non-negative, yet the quantities are NOT non_negative:
static_assert(!is_non_negative(isq::reactive_power));   // signed: Q = U·I·sin(φ)
static_assert(!is_non_negative(isq::Massieu_function)); // signed: J = −A/T
```

!!! note

    `kind_of<QS>` is **never** non-negative, even when `QS` itself is tagged `non_negative`,
    because `kind_of<QS>` represents the entire quantity tree including vector quantities
    and signed coordinates. This matters when using CTAD with bare SI units:

    ```cpp
    quantity_point generic{5.0 * m};  // origin uses kind_of<isq::length> — NOT auto-bounded
    quantity_point dist{distance_traveled(5.0 * m)};  // uses isq::distance — auto-bounded
    ```

When a `quantity_point` uses a `natural_point_origin` whose quantity spec is
non-negative, the library **automatically attaches `check_non_negative`** as the
bounds policy — no explicit bounds definition is needed. The
default can always be overridden by defining a custom origin with different bounds:

```cpp
// Define a custom origin with clamp_non_negative instead of the automatic check_non_negative:
inline constexpr struct clamped_length_origin :
    absolute_point_origin<isq::length, clamp_non_negative{}> {} clamped_length_origin;
```

---

## Design Trade-offs and Open Questions

### Should `wrap_to_range` / `reflect_in_range` also affect `min()`/`max()`?

For `clamp_to_range` the answer is obvious. For `wrap_to_range` it is less
clear: semantically the value lives on a circle so `min()` and `max()` together
define the interval, but calling either one for comparison purposes may be
misleading (there is no "smallest" longitude on a wrapped circle; they're all
equivalent modulo 360°). The current implementation does return `min` and `max`
for all policy types that expose these members.

### Why the Checking Policy Is Not Part of the `quantity_point` Type

One might ask: why not make the checking policy a template parameter of
`quantity_point` itself, so that the same origin can be used with different
policies in different parts of a program?

The reason is the **container allocator trap**: two `quantity_point` types
that differ only in their checking policy would be distinct, incompatible types.
Assigning between them, comparing them, or passing them through generic code
would require explicit conversions and propagation strategies — the same
ergonomic burden that `std::vector<T, Allocator>` inflicts when two allocator
types differ.  The interop machinery needed to make this seamless would add
substantial complexity with limited benefit.

The current approach — tying enforcement to the representation type
(`T` vs. `constrained<T>`) — keeps all `quantity_point` values with the same
origin interoperable regardless of whether they are checked.  A GUI front-end
that uses `constrained<double>` and a solver back-end that uses plain `double`
can exchange values through `constrained<double>` conversion operators without
any special machinery.

### Polar coordinates and coupled constraints

Some domains have constraints that **couple multiple axes**. A prominent
example is geodetic latitude: reflecting at a pole also requires shifting
longitude by 180° — the two coordinates are not independent, and a naive
single-axis `reflect_in_range` on an `equator` origin would be incorrect.
Similarly, polar coordinates in general couple $r \geq 0$ with
angular constraints such as $0 \leq \theta < 2\pi$ or
$-\pi < \theta \leq \pi$.

The per-origin bounds model enforces each axis independently and cannot express
inter-axis coupling through the single-value `operator()` interface alone.
However, this does not require library-level support — it is naturally handled
by a **composite type** whose constructor enforces the coupled invariant, much
like `std::complex` enforces its own invariants across two components:

```cpp
class position {
public:
  quantity_point<si::degree, equator, double> lat;
  quantity_point<si::degree, prime_meridian, double> lon;

  constexpr position(quantity_point<si::degree, equator, double> lat_in,
                     quantity_point<si::degree, prime_meridian, double> lon_in) noexcept :
      lon(lon_in)
  {
    quantity lat_q = lat_in.quantity_from(equator);
    // fold lat_q into [-180°, 180°] using fmod, then reflect at the poles
    // (|lat_q| > 90° → lat_q := ±180° - lat_q; lon += 180°).
    // wrap_to_range on prime_meridian normalizes the longitude back into (-180°, 180°].
    // ...
    lat = equator + lat_q;
  }
};
```

In this design, `equator` does **not** use `reflect_in_range` — the coupled
reflection lives in `position`'s constructor. Longitude's `wrap_to_range` on
`prime_meridian` still handles the independent cyclic wrapping as usual. The
single-axis policies and the composite type each do what they are good at.

A complete, working implementation ships with the **mp-units** examples in
[`example/include/geographic.h`](https://github.com/mpusz/mp-units/blob/master/example/include/geographic.h).

### Logarithmic quantities

Logarithmic quantities (e.g. _power ratios_ in decibels) present a different
challenge: the underlying linear quantity lives on a half-line $(0, +\infty)$,
but the logarithmic representation maps it to the full real line
$(-\infty, +\infty)$. Bounds need to be expressed in the appropriate
coordinate system, and it is not yet clear how this interacts with the
current design.

### Should bounds be applied to application-level absolute quantity ranges?

Beyond the automatic non-negativity enforcement described above, do you see real
use cases for attaching _application-specific_ range bounds to absolute quantities
directly — for example, clamping a sensor's _mass_ reading to its physical
measurement range, or bounding a _duration_ to a maximum scheduling window?

---

## We Want Your Feedback

This feature is novel in the units-library space. No prior mainstream library
that we know of provides this capability, so the solution is not yet proven in
production. We have tried to make the design principled and composable, but
there are certainly use cases we have not thought of.

If you work with geodetic coordinates, sensor data pipelines, game physics,
audio DSP, or any domain where your quantity points live in a bounded or periodic
domain, we would love to hear from you:

- Did the design make your use case straightforward to express?
- Were there cases where you reached for bounds but the current design would not
  cover them?
- Do you have a view on either of the two open questions above?

Please join the conversation in the
[GitHub Discussions](https://github.com/mpusz/mp-units/discussions) or open a
[new issue](https://github.com/mpusz/mp-units/issues/new/choose).

---

## Summary

|                                                 | Before                | After                                                  |
|-------------------------------------------------|-----------------------|--------------------------------------------------------|
| Latitude type enforces pole constraint          | ❌ user responsibility | ✅ compile-time, zero-overhead                          |
| Longitude wraps cyclically                      | ❌ manual modulo       | ✅ `wrap_to_range` on origin                            |
| Sensor range clamped at API boundary            | ❌ runtime if-else     | ✅ policy on origin                                     |
| Relative origin inherits parent bounds          | ❌ impossible          | ✅ automatic, static-checked nesting                    |
| `min()`/`max()`/`numeric_limits` reflect bounds | ❌ reports type limits | ✅ reports domain limits                                |
| Half-line (non-negative) bounds                 | ❌ impossible          | ✅ `check_non_negative` / `clamp_non_negative` policies |
| Non-negative QS auto-guards natural origins     | ❌ impossible          | ✅ automatic, no specialization needed                  |

The implementation is already merged and covered by a comprehensive compile-time
test suite. Documentation lives in the
[Affine Space chapter](../../users_guide/framework_basics/the_affine_space.md)
under _Range-Validated Quantity Points_.

## References

<!-- markdownlint-disable MD013 -->
- [The Affine Space — Range-Validated Quantity Points](../../users_guide/framework_basics/the_affine_space.md#range-validated-quantity-points) — user guide section that documents the feature in full
- [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md) — how-to guide on combining `constrained<T, EP>` with `safe_int` for defence-in-depth
- [Introducing Absolute Quantities](introducing-absolute-quantities.md) — the companion blog post on ratio-scale non-negativity and `absolute_point_origin`
- [`safe_int<T>`](../../users_guide/framework_basics/safe_int.md) — overflow-safe integer arithmetic reference
- [Preventing Integer Overflow in Physical Computations](preventing-integer-overflow.md) — in-depth narrative on automatic scaling overflow and how `safe_int<T>` composes with origin bounds policies
- [Understanding Safety Levels](understanding-safety-levels.md) — in-depth survey of all six safety levels; Level 6 covers mathematical space safety
- [GitHub Discussion #782](https://github.com/mpusz/mp-units/discussions/782) — the original user report on geodetic bounds that triggered this work
<!-- markdownlint-enable MD013 -->
