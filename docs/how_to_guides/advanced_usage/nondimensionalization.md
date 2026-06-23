# Working With Nondimensionalized Physics

<!-- markdownlint-disable MD013 MD018 -->
<!-- markdownlint parses this file's C++ code fences inconsistently and mis-reads `#include`
     lines and long code/math lines as prose; disabled file-wide. Prose is wrapped to 90. -->

This guide answers a common question from physicists and engineers who already
nondimensionalize their equations:
_"Do I still need **mp-units**?"_

The short answer is **yes** — and this guide explains why through a concrete worked
example and discusses how much of the library you should use.

For background on related concepts:

- [Dimensionless Quantities](../../users_guide/framework_basics/dimensionless_quantities.md) —
  how the library handles quantities of dimension one
- [Preventing Integer Overflow](../../blog/posts/preventing-integer-overflow.md) —
  why overflow hazards don't disappear with dimensionless numbers
- [Natural Units](../../users_guide/systems/natural_units.md) —
  for particle physics where characteristic constants ($\hbar$, $c$) are compile-time


## What Is Nondimensionalization?

**Nondimensionalization** is the process of expressing a physical problem in dimensionless
form by dividing each variable by a representative *characteristic scale*. A _position_ $x$
in metres becomes $x_* = x / X$ — a pure number relative to a reference _length_ $X$; a
_velocity_ $v$ becomes $v_* = v / V$; a _time_ $t$ becomes $t_* = t / T$.

The goals:

- **Numerical stability.** ODE and PDE solvers work best when all variables and their
  derivatives stay near order 1. Mixing $G \approx 6.7 \times 10^{-11}$ with
  $M \approx 6 \times 10^{24}$ in the same arithmetic risks catastrophic cancellation or
  floating-point underflow.
- **Equation simplification.** Choosing scales so that the governing equations lose their
  explicit constant prefactors reduces both the number of free parameters and the
  opportunity for coding mistakes.
- **Universality.** A single nondimensionalized solver describes any instance of a problem
  family — Earth orbit, Moon orbit, Jupiter orbit — with only the characteristic scales
  $X$, $T$, $V$ changing between runs.
- **Interoperability.** Most numerical libraries expect plain floating-point arrays.
  Nondimensionalization is the clean interface between typed physical inputs and untyped
  numerical kernels.

??? info "What is an ODE/PDE solver?"

    An **Ordinary Differential Equation (ODE)** relates a function to its derivatives
    with respect to a _single_ independent variable — typically time.  The gravity system
    $\dot{x} = v$, $\dot{v} = -GM/x^2$ is a classic two-variable ODE.

    A **Partial Differential Equation (PDE)** involves derivatives with respect to
    _multiple_ independent variables (e.g., both space and time in fluid dynamics or heat
    transfer).

    An **ODE/PDE solver** is a numerical integration routine — algorithms such as
    Runge-Kutta advance the solution step by step from known initial conditions, producing
    an approximate trajectory.  These routines typically operate on plain arrays of
    floating-point numbers, which is why nondimensionalization is the natural bridge
    between typed physical inputs and the solver's interface.


## A 1-D Gravity Simulation

Consider a particle falling under gravity from an _initial radius_ $R$ above a _mass_ $M$:

$$\frac{dx}{dt} = v, \qquad \frac{dv}{dt} = -\frac{GM}{x^2}$$

$G \approx 6.674 \times 10^{-11}\,\text{m}^3\,\text{kg}^{-1}\,\text{s}^{-2}$ is
_Newton's gravitational constant_ and $M \approx 5.972 \times 10^{24}\,\text{kg}$ is
the _mass of the central body_ (e.g. Earth) — one very small, the other very large.

The standard remedy is to introduce characteristic scales — reference values $X$, $V$,
and $T$ that are representative of the problem's typical _length_, _speed_, and _time_ —
and express every variable as a dimensionless multiple of its scale:

$$x(t) = X\,x_*(t_*), \quad v(t) = V\,v_*(t_*), \quad t = T\,t_*$$

Here $x_*$, $v_*$, and $t_*$ are the new dimensionless variables; $X$, $V$, $T$ are
constants with dimensions that we are free to choose.

Substituting into the first equation of motion, using $x = X x_*$ and $t = T t_*$:

$$\frac{dx}{dt} = v \implies \frac{d(X x_*)}{d(T t_*)} = V v_* \implies \frac{X}{T}\frac{dx_*}{dt_*} = V v_*$$

Multiplying through by $T/X$:

$$\frac{dx_*}{dt_*} = \frac{VT}{X}\,v_*$$

For the second equation, using $v = V v_*$, $x = X x_*$, $t = T t_*$:

$$\frac{dv}{dt} = -\frac{GM}{x^2} \implies \frac{d(V v_*)}{d(T t_*)} = -\frac{GM}{(X x_*)^2}
\implies \frac{V}{T}\frac{dv_*}{dt_*} = -\frac{GM}{X^2 x_*^2}$$

Multiplying through by $T/V$:

$$\frac{dv_*}{dt_*} = -\frac{GMT}{VX^2}\,\frac{1}{x_*^2}$$

So the substituted system is:

$$\frac{dx_*}{dt_*} = \frac{VT}{X}\,v_*, \qquad \frac{dv_*}{dt_*} = -\frac{GMT}{V X^2}\,\frac{1}{x_*^2}$$

The two prefactor coefficients $\dfrac{VT}{X}$ and $\dfrac{GMT}{VX^2}$ still carry
all the physics. We now choose $X$, $V$, and $T$ to make both coefficients equal to 1,
so that they disappear from the equations entirely:

$$\frac{VT}{X} = 1 \implies V = \frac{X}{T}$$

$$\frac{GMT}{VX^2} = 1 \xrightarrow{V = X/T} \frac{GMT}{(X/T)X^2} = \frac{GMT^2}{X^3} = 1 \implies T = \sqrt{\frac{X^3}{GM}}$$

With these choices, the equations reduce to the clean dimensionless system

$$\frac{dx_*}{dt_*} = v_*, \qquad \frac{dv_*}{dt_*} = -\frac{1}{x_*^2}$$

which contains no physical constants at all — $G$ and $M$ have been absorbed into the
scale $T$. The remaining free parameter is $X$: any length scale works, and a
natural choice is $X = R$ (the initial position), so that $x_*(0) = 1$.

The dimensionless system is what you actually hand to your ODE solver.
It has no awkward constants, all variables start near order 1,
and the same equations describe
_any_ central-body problem regardless of the actual mass or distance — only the scales
$X$, $T$, $V$ differ between problems.

<figure markdown>
  ![Free-fall from 1.5 R⊕: dimensional vs dimensionless](nondim_gravity_comparison.svg)
  <figcaption>The same free-fall trajectory from 1.5&thinsp;R<sub>⊕</sub> to the surface,
  expressed in SI units (left — megametres and seconds) and in dimensionless form (right —
  all values near order 1).  Numbers near 1 are friendlier to floating-point arithmetic
  than numbers spanning 10<sup>−11</sup>–10<sup>24</sup>.</figcaption>
</figure>

Once the solver has produced dimensionless output trajectories $t_*$, $x_*$, $v_*$, you
need to convert them back to physical quantities before passing them to anything in the
real world — a plotting library, a telemetry system, a guidance computer.
This is done by multiplying each dimensionless result back by its characteristic scale:
$t = T\,t_*$, $x = X\,x_*$, $v = V\,v_*$.


## The Same Problem in mp-units

Three approaches are shown in progression, each building on the previous:

- **Traditional approach** — characteristic scales are typed SI quantities; the
  boundaries perform explicit dimension-checked arithmetic.
- [**Faster-than-lightspeed approach**](../../users_guide/framework_basics/faster_than_lightspeed_constants.md)
  — compile-time named constants absorb the scales into the type system, eliminating
  runtime divide and multiply at the boundaries.
- [**Strong dimensionless types approach**](../../users_guide/framework_basics/dimensionless_quantities.md)
  — dimensionless specs derived from the ISQ hierarchy extend type safety into the solver
  itself, making argument-order swaps and wrong-scale pairings compile errors.

### Traditional Approach

Characteristic scales `X`, `T`, `V` are ordinary typed quantities with SI numerical
values. The entry boundary divides inputs by those large numbers; the exit boundary
multiplies results back.

#### Step 1: Physical Parameters

```cpp
#include <mp-units/math.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

constexpr quantity G = 6.674e-11 * m3 / (kg * s2);  // Newton's gravitational constant
constexpr quantity M = 5.972e24 * kg;               // Earth's mass
constexpr quantity R = 6.3781e6 * m;                // ≈ R⊕
quantity r_initial = 1.5 * R;                       // = 1.5 R⊕
quantity v_initial = 0. * m / s;                    // radial velocity (signed)
```

#### Step 2: Characteristic Scales

The library verifies dimensional consistency of the scale definitions at compile time:

!!! tip "`QuantityOf<...> auto` is optional — plain `auto` works too"

    Throughout this guide, declarations like `QuantityOf<isq::duration> auto T = ...` are
    used to *document and verify* the expected quantity type at the point of definition.
    Plain `auto` or CTAD (`quantity T = ...`) is equally valid and produces identical
    code — the constraint only adds a compile-time assertion that the inferred type
    satisfies the concept.

    You only *need* a named type at API boundaries: function parameters, return types,
    and data members.  At those sites the compiler will enforce the type regardless — if
    you used plain `auto` everywhere in between and made an error in quantity calculus
    (e.g., divided by the wrong scale), the mismatch will still be caught the moment you
    try to pass or store the result.

```cpp
const QuantityOf<isq::position_vector> auto X = R;                         // characteristic position scale
const QuantityOf<isq::duration>        auto T = sqrt(pow<3>(X) / (G * M)); // ≈ 806 s
const QuantityOf<isq::velocity>        auto V = X / T;                     // ≈ 7909 m/s
```

Writing `sqrt(X / (G * M))` instead of `sqrt(pow<3>(X) / (G * M))` would fail to satisfy
`QuantityOf<isq::duration>` at that declaration — a compile error rather than a silently
wrong scale.

#### Step 3: Entry Boundary

Divide each input by its characteristic scale to obtain dimensionless numbers:

```cpp
double x0 = (r_initial / X).numerical_value_in(one);  // 1.5 R⊕ / R⊕ = 1.5
double v0 = (v_initial / V).numerical_value_in(one);  // 0 / 7909 = 0.0
```

A dimension mismatch is also caught at compile time: `(r_initial / T)` would yield a
_velocity ratio_, not the dimensionless position intended.

#### Step 4: Dimensionless Solver

```cpp
// Dimensionless RHS: dx*/dt* = v*,  dv*/dt* = -1/x*²
auto rhs = [](double x_nd, double v_nd) { return std::pair{ v_nd, -1.0 / (x_nd * x_nd) }; };

// Raw doubles are perfectly fine inside the solver
auto [t_result, x_result, v_result] = integrate(rhs, x0, v0, /*steps=*/10'000);
```

**This is the direct answer to "do I use `quantity<>` throughout?"**
For the integration loop itself, raw doubles are idiomatic and efficient — though
returning untyped `double`s means the exit boundary retains one unavoidable blind spot,
described below.

#### Step 5: Exit Boundary

Multiply each dimensionless result back by its typed characteristic scale:

```cpp
QuantityOf<isq::position_vector> auto x_phys = X * x_result;  // R⊕ × 1.0
QuantityOf<isq::duration>        auto t_phys = T * t_result;  // 806 s × 1414
QuantityOf<isq::velocity>        auto v_phys = V * v_result;  // 7909 m/s × (−0.817)

std::println("t = {::N[.1f]}, x = {::N[.0f]}, v = {::N[.2f]}", t_phys.in(s), x_phys.in(km), v_phys.in(km / s));
// Output: t = 1139.2 s, x = 6378 km, v = -6.46 km/s
```

The `QuantityOf` constraints guard against **dimension mismatches**: multiplying `x_result`
by `T` (duration) instead of `X` (position) would yield a `quantity<isq::duration>` where
`quantity<isq::position_vector>` is expected — a compile error, not a silent wrong answer.

!!! warning "Undetectable swap of dimensionless scalars"

    The solver returns plain `double`s, so `x_result`, `t_result`, and `v_result` are
    indistinguishable to the type system.  Writing `X * v_result` instead of
    `X * x_result` still satisfies `QuantityOf<isq::position_vector>` — the compiler
    can only enforce the *dimension* of the scale–result pairing, not that you chose the
    *correct* scalar for that scale.  The
    [Strong Dimensionless Types Approach](#strong-dimensionless-types-approach)
    removes this gap.


### Faster-Than-Lightspeed Approach

When the characteristic scales are compile-time
[named constants](../../users_guide/framework_basics/faster_than_lightspeed_constants.md),
their SI magnitudes live in the type and their numerical values are exactly 1.0.  The
boundary steps then simplify:

- **Entry**: pass the characteristic unit directly to `.numerical_value_in(scale)`.  When
  an input is already stored in that unit, no runtime division occurs — the stored value
  is read directly.  Inputs in other units are converted automatically.
- **Exit**: multiply the plain `double` result by the characteristic unit.  No runtime
  multiplication occurs (the unit encodes the scale in its type).  The large-number SI
  arithmetic is deferred to the output call (`.in(s)`, `.in(km)`, etc.) and only happens
  when explicitly requested.

#### Step 1: Physical Parameters

```cpp
#include <mp-units/math.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::iau::unit_symbols;
using namespace mp_units::si::unit_symbols;

// IAU named constants used directly as units — SI magnitudes live in the type
constexpr Unit auto GM = GM_EARTH_N;
constexpr Unit auto R  = R_EARTH_E_N;

// r_initial is expressed directly in the characteristic unit
// its stored numerical value IS the dimensionless coordinate:
quantity r_initial = 1.5 * R;     // numerical_value_in(R_EARTH_E_N) == 1.5
quantity v_initial = 0. * m / s;
```

#### Step 2: Characteristic Scales

`X`, `T`, and `V` are still needed for the exit boundary, but unlike the traditional
approach they are not arrived at through nondimensionalization math — they fall out of
the IAU unit choice directly.  They are pure units; the characteristic scale is encoded
in their type:

```cpp
constexpr Unit auto X = R;                    // characteristic position unit
constexpr Unit auto T = sqrt(pow<3>(X) / GM); // unit R_⊕ᵉᴺ^(3/2)/(GM)_⊕ᴺ^(1/2)
constexpr Unit auto V = X / T;                // unit (GM)_⊕ᴺ^(1/2)/R_⊕ᵉᴺ^(1/2)
```

#### Step 3: Entry Boundary

Pass the characteristic unit directly to `.numerical_value_in()`.  When an input is
already stored in that unit, no runtime division occurs; otherwise the library converts
automatically:

```cpp
double x0 = r_initial.numerical_value_in(X);  // = 1.5, no runtime division
double v0 = v_initial.numerical_value_in(V);  // = 0.0
```

If `v_initial` were provided in SI units (e.g. `100. * m/s`), the same call would
still correctly convert to the characteristic velocity unit — the idiom works
regardless of the input unit.

#### Step 4: Dimensionless Solver

The solver code is identical to the traditional approach — the dimensionless RHS is the
same $\dot{x}_* = v_*$, $\dot{v}_* = -1/x_*^2$.  The reason it works without any
explicit nondimensionalization algebra is that `GM` has numerical value 1.0 in IAU units,
so the coefficient in the original equation $\dot{v} = -GM/x^2$ is already 1:

```cpp
// Original physical RHS: dv/dt = -GM/x²
// GM = 1.0 in IAU units — the coefficient is 1 automatically
auto rhs = [](double x_nd, double v_nd) { return std::pair{ v_nd, -1.0 / (x_nd * x_nd) }; };
auto [t_result, x_result, v_result] = integrate(rhs, x0, v0, /*steps=*/10'000);
```

#### Step 5: Exit Boundary

Multiply each plain `double` result by its characteristic unit to obtain quantity type — the
mirror of the entry step, with no runtime multiplication:

```cpp
QuantityOf<isq::position_vector> auto x_phys = x_result * X;  // ≡ x_result × R_⊕ₑᴺ
QuantityOf<isq::duration>        auto t_phys = t_result * T;  // value stored in T's IAU unit
QuantityOf<isq::velocity>        auto v_phys = v_result * V;  // value stored in V's IAU unit

// Conversion to conventional SI units only happens here, on explicit request
std::println("t = {::N[.1f]}, x = {::N[.0f]}, v = {::N[.2f]}", t_phys.in(s), x_phys.in(km), v_phys.in(km / s));
// Output: t = 1139.2 s, x = 6378 km, v = -6.46 km/s
```

??? note "What is printed without `.in(...)`?"

    Because `x_phys`, `t_phys`, and `v_phys` are typed with the IAU named-constant units
    of `X`, `T`, and `V`, printing without `.in(...)`:

    ```cpp
    std::println("t = {}, x = {}, v = {}", t_phys, x_phys, v_phys);
    ```

    produces:

    ```
    t = 1.4 R_⊕ₑᴺ^(3/2)/(GM)_⊕ᴺ^(1/2), x = 1 R_⊕ₑᴺ, v = -0.82 (GM)_⊕ᴺ^(1/2)/R_⊕ₑᴺ^(1/2)
    ```

    The numerical values are near 1 and the units are the IAU characteristic unit system —
    physically correct and numerically well-conditioned.  The `.in(s)`, `.in(km)`,
    `.in(km / s)` calls perform the SI conversion only when needed.

    The conversion is also *lazy*: all intermediate scaling factors accumulated through
    unit arithmetic are resolved symbolically at compile time and collapsed into a single
    floating-point multiply at the moment the conversion is triggered.  There is no chain
    of sequential multiplications through the computation — and therefore no intermediate
    rounding errors from repeated floating-point operations.

The same `QuantityOf` protection and scalar-swap caveat from the traditional approach
apply here (see the warning above): the FTL approach eliminates runtime cost at the
boundaries but cannot distinguish `x_result` from `v_result` at the exit, because both
are plain `double`s.  The
[Strong Dimensionless Types Approach](#strong-dimensionless-types-approach)
closes that gap by giving every dimensionless variable a distinct type.

*[FTL]: Faster-Than-Lightspeed

### Strong Dimensionless Types Approach

When a simulation uses multiple coexisting nondimensionalizations, any generic ODE engine
accepts raw `double`s — it cannot distinguish a position scalar from a velocity scalar.
Argument-order swaps are therefore silent at every call site.  This approach keeps the
generic engine unchanged: it still receives and returns plain `double`s.  What changes is
the strongly-typed *wrapper* around it: the wrapper's signature accepts `nd_*` quantities,
strips the types to forward raw `double`s to the engine, and repacks raw results back into
`nd_*` quantities before returning to the caller.  The scales build directly on the FTL
approach — the same IAU named constants as characteristic units — promoted to typed
quantities via *characteristic* sub-specs, from which dimensionless specs are derived as
ISQ ratios.

- **Entry**: `isq::position_vector / characteristic_position` resolves directly to
  `nd_position` — no cast, and the wrong pairing is a compile error.
- **Typed wrapper**: accepts `quantity<nd_position[one]>` and `quantity<nd_velocity[one]>`,
  strips types to call the generic engine with raw `double`s, and repacks results into
  `nd_*` quantities on return — argument-order swaps at every call site become compile errors.
- **Exit**: `nd_position × characteristic_position` recovers `isq::position_vector`
  through ISQ algebra.

#### Step 1: Physical Parameters

Define the characteristic and dimensionless specs at namespace scope, alongside the
physical inputs.  The explicit `isq::*` specifiers on `r_initial` and `v_initial` ensure
the numerator spec is exact at the entry boundary:

```cpp
#include <mp-units/math.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::iau::unit_symbols;
using namespace mp_units::si::unit_symbols;

// Sub-specs that identify the characteristic scale quantities
inline constexpr struct characteristic_position : quantity_spec<isq::position_vector> {} characteristic_position;
inline constexpr struct characteristic_velocity : quantity_spec<isq::velocity> {} characteristic_velocity;
inline constexpr struct characteristic_duration : quantity_spec<isq::duration> {} characteristic_duration;

// Dimensionless specs derived from ISQ ratios
inline constexpr struct nd_position final
    : quantity_spec<dimensionless, isq::position_vector / characteristic_position> {} nd_position;
inline constexpr struct nd_velocity final
    : quantity_spec<dimensionless, isq::velocity / characteristic_velocity> {} nd_velocity;
inline constexpr struct nd_time final
    : quantity_spec<dimensionless, isq::duration / characteristic_duration> {} nd_time;

// Physical inputs — explicit ISQ casts ensure the correct spec at the entry boundary
quantity r_initial = isq::position_vector(1.5 * R_EARTH_E_N);
quantity v_initial = isq::velocity(0. * m / s);
```

#### Step 2: Characteristic Scales

Unlike the FTL approach where `X`, `T`, `V` are bare units, here they are typed
quantities — each carrying its `characteristic_*` spec — built from the same IAU
constants.  The library verifies dimensional consistency at compile time:

```cpp
const quantity X = characteristic_position(1 * R_EARTH_E_N);
const quantity T = characteristic_duration(sqrt(pow<3>(X) / (1 * GM_EARTH_N)));
const quantity V = characteristic_velocity(X / T);
```

#### Step 3: Entry Boundary

Divide each typed input by its typed characteristic scale.  The ISQ algebra resolves the
quotient spec automatically — no cast, and the wrong pairing is a compile error:

```cpp
QuantityOf<nd_position> auto x0_nd = r_initial / X;   // position_vector / characteristic_position → nd_position ✓
QuantityOf<nd_velocity> auto v0_nd = v_initial / V;   // velocity / characteristic_velocity         → nd_velocity ✓
// QuantityOf<nd_position> auto bad = r_initial / V;  // ❌ compile error — position_vector / characteristic_velocity ≠ nd_position
```

#### Step 4: Dimensionless Solver

The generic engine is unchanged — it operates on plain `double`s and has no knowledge of
nondimensionalization.  The typed wrapper does the bridgework: it accepts `nd_*` quantities,
strips them with `.numerical_value_in(one)`, calls the engine, and repacks the raw results
back into `nd_*` quantities on return.  The wrapper's typed signature is what enforces
argument order at every call site:

```cpp
// Generic engine — operates on plain doubles, independent of nondimensionalization
std::tuple<double, double, double>
integrate(auto rhs, double x0, double v0, int max_steps = 10'000);

// Typed wrapper — strips nd_* types, calls the generic engine, repacks results
std::tuple<quantity<nd_time[one]>, quantity<nd_position[one]>, quantity<nd_velocity[one]>>
integrate(auto rhs, quantity<nd_position[one]> x0, quantity<nd_velocity[one]> v0, int max_steps = 10'000)
{
  return integrate(rhs, x0.numerical_value_in(one), v0.numerical_value_in(one), max_steps);
}
```

The RHS lambda uses plain `double`s — it is passed unchanged to the raw engine inside the
wrapper:

```cpp
auto rhs = [](double x_nd, double v_nd) { return std::pair{ v_nd, -1.0 / (x_nd * x_nd) }; };

// Compiler enforces argument order — swapping x0_nd and v0_nd is a compile error
auto [t_nd, x_nd, v_nd] = integrate(rhs, x0_nd, v0_nd);
// integrate(rhs, v0_nd, x0_nd);  // ❌ compile error — position and velocity swapped
```

#### Step 5: Exit Boundary

Multiplying `nd_position × characteristic_position` recovers `isq::position_vector`
through ISQ algebra.  The `QuantityOf` constraints verify the result spec at compile time:

```cpp
QuantityOf<isq::position_vector> auto x_phys = x_nd * X;  // nd_position × characteristic_position
QuantityOf<isq::duration>        auto t_phys = t_nd * T;  // nd_time     × characteristic_duration
QuantityOf<isq::velocity>        auto v_phys = v_nd * V;  // nd_velocity × characteristic_velocity
// QuantityOf<isq::position_vector> auto bad = x_nd * V;  // ❌ compile error — nd_position × characteristic_velocity ≠ position_vector

// Conversion to conventional SI only happens here, on explicit request
std::println("t = {::N[.1f]}, x = {::N[.0f]}, v = {::N[.2f]}", t_phys.in(s), x_phys.in(km), v_phys.in(km / s));
// Output: t = 1139.2 s, x = 6378 km, v = -6.46 km/s
```

This approach closes all three safety gaps: wrong-dimension entry divisions, argument-order
swaps in `integrate()`, and wrong-scale exit multiplications.  It is most valuable when
multiple nondimensionalizations coexist (e.g., a coupled thermal-structural simulation)
or when the integrator is called from many sites.


## Overflow Does Not Disappear at the Boundaries

A common misconception: once you've nondimensionalized, all numbers are safe fractions
near 1 and overflow isn't a concern.

This is true _inside_ the dimensionless ODE — but the _formation_ of the dimensionless
ratios at entry can still overflow when using integer types.
The blog post
[Preventing Integer Overflow in Physical Computations](../../blog/posts/preventing-integer-overflow.md)
covers what **mp-units** can catch at compile time vs. what it cannot.

The key takeaway: nondimensionalization moves overflow risk from the inner loop to the
entry and exit boundaries — exactly where **mp-units** provides protection.


## Named Units for Domain-Specific Scales

The IAU constants used throughout this guide — `R_EARTH_E_N`, `GM_EARTH_N` — are
themselves `named_unit` instances inside the library.  Their definitions follow exactly
the same pattern as any other named unit:

```cpp
inline constexpr struct R_earth_equatorial_nominal final
    : named_unit<"R_⊕ₑᴺ", mag_ratio<6'3781, 10> * km> {} R_earth_equatorial_nominal;
```

The same pattern applies to any domain that has a standardized compile-time reference
scale:

- **Quantum chemistry** uses *atomic units*: the Bohr radius
  ($a_0 \approx 52.9\,\text{pm}$) for length, the Hartree ($E_h \approx 27.2\,\text{eV}$)
  for energy.  Defining `named_unit<"a₀", ...>` preserves `isq::length` through a DFT
  solver exactly as `R_EARTH_E_N` preserves `isq::position_vector` here.
- **General relativity** uses *geometric units* ($G = c = 1$), collapsing _mass_, _length_,
  and _time_ to a single scale.
- **Particle physics** uses natural units ($\hbar = c = 1$); the library's built-in
  [Natural Units](../../users_guide/systems/natural_units.md) system covers that case.

The key requirement is that the reference scale must be a **compile-time constant** —
a defined standard value fixed at the problem-class level, not a quantity that varies
with runtime conditions.  A scale that depends on, say, local temperature or altitude
cannot be encoded in a `named_unit`; it must be treated as a typed runtime quantity, as
in the Traditional Approach.


## Tradeoffs at a Glance

Each approach can be evaluated along five axes:

- **Entry/exit dimension** — whether dividing by the wrong-dimensioned characteristic
  scale at entry (e.g., position by a time scale) or multiplying by the wrong-dimensioned
  scale at exit is a compile error.
- **Argument order at call site** — whether swapping arguments of different physical
  meaning when calling the solver (e.g., passing velocity where position is expected) is
  a compile error.  In the Strong Dimensionless Types approach this is enforced by the
  *wrapper's* typed signature; the solver body itself still operates on plain `double`s.
- **Exit scalar identity** — whether using the wrong dimensionless result at the exit
  boundary (e.g., scaling the position characteristic by the velocity result) is a compile
  error.  Traditional and FTL can only verify that the correct-*dimensioned* scale is
  applied; the plain `double` result multiplier is indistinguishable from any other.
- **Zero-cost boundaries** — whether entry and exit have no runtime multiply/divide.
  The FTL approach can achieve this: the scale is encoded purely in the unit type, so
  `.numerical_value_in(X)` on a value already stored in unit `X` is a direct memory read
  with no arithmetic.  However, if the input is stored in a *different* unit (e.g.,
  SI metres when the characteristic unit is `R_EARTH_E_N`), a runtime conversion factor
  is applied — see footnote (¹).  With raw `double`s the user always divides by a large
  SI number explicitly.  The Traditional and Strong Dimensionless Types approaches both
  perform a runtime `double` division or multiplication at the boundary; in the Strong
  Dimensionless Types case the stored value is `1.0` (no precision loss), but the
  operation itself is not eliminated unless the compiler can prove constancy.
- **Overflow protection** — whether the library can detect integer overflow in boundary
  arithmetic involving large-magnitude quantities.

<!-- markdownlint-disable MD013 -->
| Approach                                                              | Entry/exit dimension | Argument order at call site | Exit scalar identity | Zero-cost boundaries | Overflow protection |
|-----------------------------------------------------------------------|----------------------|-----------------------------|----------------------|----------------------|---------------------|
| Raw `double` — no mp-units at all                                     | ❌                    | ❌                           | ❌                    | ❌                    | ❌                   |
| Traditional approach — typed SI scales, raw `double` solver           | ✅                    | ❌                           | ❌                    | ❌                    | ✅                   |
| Faster-than-lightspeed approach — compile-time scales, zero-cost I/O  | ✅                    | ❌                           | ❌                    | ✅ (¹)                | ✅                   |
| Strong dimensionless types approach — typed wrapper around the solver | ✅                    | ✅                           | ✅                    | ❌                    | ✅                   |
| Full SI throughout (no nondimensionalization)                         | N/A                  | ✅                           | ✅                    | N/A                  | ✅                   |
<!-- markdownlint-enable MD013 -->

(¹) Zero-cost only when the input quantity is already stored in the characteristic unit.
If stored in a different unit (e.g., SI metres when the characteristic unit is `R_EARTH_E_N`),
a runtime conversion factor is applied at the boundary.

!!! note

    The Strong Dimensionless Types approach divides by `1.0` at the boundary (because the
    characteristic quantities are built from IAU constants whose stored numerical value
    is 1), so there is no precision loss — but the division is still a runtime `double`
    operation with the same cost as the Traditional approach.

**Use the Traditional or FTL approach when:**

- The inner loop is a generic floating-point solver (Runge-Kutta, CVODE, GSL, …)
- Runtime overhead at the boundaries matters (prefer FTL) or scales are computed at
  runtime (prefer Traditional)
- A single nondimensionalization is in use and argument-order safety in the solver
  is not a concern

**Use the Strong Dimensionless Types approach additionally when:**

- Multiple distinct nondimensionalizations coexist in the same codebase
- You want the compiler to guard against argument-order mistakes at every solver call site
- The integrator is called from many places or by many developers

**Prefer the FTL or Strong Dimensionless Types approach over Traditional when:**

- The characteristic scales are standardized compile-time constants
  (IAU planetary constants, Bohr radius, Hartree energy, etc.)
- You want zero runtime cost at the entry/exit boundaries
- You want full ISQ type information carried without manual SI arithmetic


## Summary

Nondimensionalization and **mp-units** are complementary, not competing.

- **Use typed `quantity<>` at the boundaries**: scale derivation, entry
  (nondimensionalization), and exit (re-dimensionalization). This is where most errors
  hide and where the library gives immediate, zero-overhead value.
- **Use raw `double` inside the solver**: the dimensionless ODE contains no units, and
  a plain ODE integrator operating on doubles is idiomatic and efficient.
- **Optionally use the strong dimensionless types approach** with `characteristic_*`
  sub-specs when multiple nondimensionalizations coexist or when function-signature safety
  matters.
- **When scales are compile-time constants**, define them as `named_unit` values to
  keep the scale in the type rather than multiplying it into a numerical value prematurely.
- **When re-dimensionalizing**: the library's typed characteristic scales prevent
  accidentally multiplying back by the wrong scale.
- **Chaining subsystems**: because re-dimensionalization produces fully-typed ISQ
  quantities, the output of one solver is a valid, type-checked input to any downstream
  mp-units-aware boundary.  Multiple coupled subsystems can each operate in their own
  characteristic units while the typed interface values flow safely between them.

The library handles the tedious bookkeeping at the boundaries of your nondimensionalized
solver, so you can focus on the physics.


## See Also

<!-- markdownlint-disable MD013 -->
- [Dimensionless Quantities](../../users_guide/framework_basics/dimensionless_quantities.md) —
  `is_kind`, `quantity_spec`, and the library's treatment of dimension one
- [Faster-Than-Lightspeed Constants](../../users_guide/framework_basics/faster_than_lightspeed_constants.md) —
  how compile-time named constants eliminate runtime boundary arithmetic
- [IAU System](../../users_guide/systems/iau.md) —
  the built-in IAU units and constants (`R_EARTH_E_N`, `GM_EARTH_N`, etc.) used in this guide
- [Natural Units](../../users_guide/systems/natural_units.md) —
  the built-in ℏ = c = 1 system for particle physics
- [Pure Dimensional Analysis](pure_dimensional_analysis.md) —
  using dimensions without numerical values for compile-time validation
- [Prevent Integer Overflow](../../blog/posts/preventing-integer-overflow.md) —
  how the library protects the re-dimensionalization boundary
<!-- markdownlint-enable MD013 -->
