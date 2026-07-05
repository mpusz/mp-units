# Working with Polar and Spherical Coordinates

The built-in `cartesian_vector` is the representation the library uses for vector
quantities, and all vector arithmetic happens in Cartesian components. Many problems,
though, are stated in polar or spherical coordinates: a radar contact at a _range_ and a
_bearing_, a _launch velocity_ given as a _speed_ and two _angles_, a _joystick deflection_
as a magnitude and a direction.

`mp_units::utility::polar_vector` and `mp_units::utility::spherical_vector` bridge that
gap. They are **conversion facades**, not quantity representations: you build one from
native parts (a radius and typed angles) or from a `cartesian_vector`, read the parts
back, and convert to Cartesian for any actual math. They deliberately do not define `+`,
`-`, dot, or cross: none of those are component-wise in polar or spherical coordinates, so
the only correct implementation would convert to Cartesian and back on every call. Rather
than hide that cost behind an operator that looks cheap, the facades leave the Cartesian
round-trip explicit.

The Cartesian side is not tied to `cartesian_vector`. Construction accepts any vector
quantity of the right dimension whose representation is tuple-like (the dimension is read
from `std::tuple_size`, the components with `get`) and offers the `magnitude` customization
point, and `to_cartesian<To>()` takes the destination representation as a parameter,
defaulting to `cartesian_vector`. A Blaze `StaticVector` works on both sides with nothing
extra, because Blaze already ships that tuple protocol for it. An Eigen fixed-size vector
works too, because `mp-units/integrations/eigen.h` adds the tuple protocol that Eigen itself
does not provide. See [Using an Eigen or Blaze vector](#using-an-eigen-or-blaze-vector)
below.

## Why not a polar quantity?

A `quantity`'s representation has to support addition and subtraction (it must be
`RegularAddable`), and in polar and spherical coordinates those are not component-wise: you
cannot add two radii and two angles and get the sum of the vectors. Implementing them
correctly would mean hiding a Cartesian round-trip behind every `+`, and we would rather
keep that conversion explicit. Scaling is *not* the obstacle: it only needs to touch the
radius and leave the angle alone, which is perfectly well-defined, so a unit conversion on
such a representation would behave correctly. It is the additive structure that is missing.
So quantities stay `cartesian_vector`-backed, and polar/spherical live one layer out as
explicit converters.

The _radius_ is a scalar `quantity` and each _angle_ is an angular `quantity`, so a value
is fully typed and `to_cartesian()` hands back an ordinary vector quantity.

## 2-D: combining displacements given in polar form

Two legs of a path are each given as a distance and a heading. The sum is not a polar
operation, so you convert both to Cartesian, add there, and convert the resultant back.

```cpp
#include <mp-units/utility/polar_vector.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// angles from +x, counter-clockwise; the radius is a length quantity
utility::polar_vector leg1{100.0 * m, 30.0 * deg};
utility::polar_vector leg2{60.0 * m, 120.0 * deg};

// addition is Cartesian; the facades convert explicitly
quantity resultant = leg1.to_cartesian() + leg2.to_cartesian();  // quantity over a vector

// back to polar for a human-readable answer
utility::polar_vector total{resultant};
std::cout << "distance " << total.radius()             // quantity<m>
          << ", heading " << total.theta().in(deg) << '\n';
```

Both the _radius_ (`quantity<m>`) and the _angle_ are typed, so units are explicit on every
side and there is no chance of feeding degrees where radians are expected. `deg` here is
`si::degree`, the angle unit defaults to `si::radian`, and SI's `degree`, `arcminute`, and
`arcsecond` share that kind, so they interconvert freely.

Constructing from a cartesian vector quantity deduces the _radius_ unit from that quantity
(`m` here) and the angle unit as the default `si::radian`: the Cartesian value carries no
_angle_, so the legs' `degree` unit does not survive the round-trip. That is why the _heading_
is read back with an explicit `.in(deg)`. To *store* the result in degrees, name the type
instead of relying on class template argument deduction:
`polar_vector<si::metre, si::degree> total{resultant};`.

## 3-D: a _launch velocity_ from _speed_ and _angles_, used as a quantity

A projectile leaves at 20 m/s, 40° above the _horizontal_, on a 30° _azimuth_. The direction
is natural in spherical form (`theta` is the _inclination_ from the vertical +z axis,
`phi` the _azimuth_ from +x). Multiplying that _velocity_ by a _duration_ is a radial
scaling - it scales the _radius_ (m/s → m) and leaves the direction alone - so it stays
in spherical form, no Cartesian round-trip. You convert to Cartesian only when you actually
need the components.

```cpp
#include <mp-units/utility/spherical_vector.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// 40° above horizontal -> 50° inclination from +z (up)
utility::spherical_vector v{20.0 * m / s, 50.0 * deg, 30.0 * deg};

// scale by a duration: radius m/s -> m, angles unchanged; still spherical, no conversion
utility::spherical_vector displacement = v * (3.0 * s);
std::cout << "range " << displacement.radius() << '\n';  // quantity<m>

quantity xyz = displacement.to_cartesian();  // convert only when you need x/y/z components
```

!!! note "Scalar radius in, vector quantity out"

    The radius is a *scalar* quantity (a _speed_, a _length_, …) and `to_cartesian()` returns a
    *vector* quantity. That crosses the tensor-order axis of the [quantity
    character](../../users_guide/framework_basics/character_of_a_quantity.md), and the two
    directions are not symmetric. Going from a vector to its scalar magnitude is well defined (it
    is what the `magnitude` customization point computes), but the framework cannot derive *which*
    vector quantity a scalar belongs to — there is no way to know that a _speed_'s vector sibling
    is _velocity_.

    So `to_cartesian()` deliberately does **not** try to guess. It emits the vector over the bare
    unit (a `kind_of<…>` reference), which is compatible with *every* quantity of that kind, and
    lets you name the concrete vector quantity on assignment:

    ```cpp
    quantity speed = isq::speed(20.0 * m / s);
    spherical_vector v{speed, 50.0 * deg, 30.0 * deg};

    // the result kind matches any length/time vector; you pin it to isq::velocity yourself
    quantity<isq::velocity[m / s], cartesian_vector<double, 3>> velocity = v.to_cartesian();
    ```

    The reverse direction is better off, because it *is* a magnitude. When a facade is deduced from
    a vector quantity its radius is taken from that vector's own `magnitude()`: in V2 that is a
    `kind_of<…>` scalar over the unit, and in V3 - once `magnitude()` names the strong scalar (e.g.
    `isq::speed` for an `isq::velocity` vector, or a `magnitude<…>` expression) - the deduced radius
    upgrades to it automatically, with no change here. You can always pin a specific scalar radius
    spec yourself (e.g. `spherical_vector<isq::speed[m/s]>`).

## Rotating and scaling without leaving the coordinate system

Rotation and radial scaling are exact and cheap in polar/spherical form, and clumsy in
Cartesian, so the facades provide them directly. Rotation adds to an _angle_ (no trig) and
the _angle_ is kept canonical (so a full turn is the identity). Radial scaling touches only
the _radius_, and its factor may be a plain number or a quantity. A dimensioned factor also
changes the _radius_ unit (see the velocity-times-duration case above):

```cpp
polar_vector p{2.0 * m, 30.0 * deg};
polar_vector turned = p.rotated_by(90.0 * deg);  // theta += 90° (wrapped), exact
polar_vector longer = 2.0 * p;                   // scales the radius to 4 m
polar_vector shorter = p / 2.0;                  // radius 1 m
polar_vector rate = p / (4.0 * s);               // dimensioned: radius m -> m/s, angle kept

spherical_vector s{2.0 * m, 30.0 * deg, 30.0 * deg};
spherical_vector spun = s.rotated_about_z(90.0 * deg);  // azimuth += 90° (wrapped)
spherical_vector tilted = s.tilted_by(30.0 * deg);      // inclination += 30° (meridian plane)
```

`rotated_about_z` turns the _azimuth_, and `tilted_by` changes the _inclination_. When
`tilted_by` sends the _inclination_ past a pole, the result stays canonical. The _inclination_
reflects back into `[0, π]` and the _azimuth_ flips by π, the same physical direction.
`+`, `-`, dot, and cross stay Cartesian-only.

## Choosing the stored angle unit

The _radius_ type comes from the quantity you pass. Its template parameter is a reference,
so it can be a bare unit like `si::metre` or a specific `quantity_spec[unit]` such as
`isq::radius[m]`, and the _radius_ is then stored as that quantity. `to_cartesian()` drops
the scalar spec and hands back a bare-unit vector quantity (a vector is not itself "a radius").
The _angle_ unit is a template parameter defaulting to `si::radian`, deduced from the _angle_
you construct with. Any SI _angle_ unit works out of the box, and the strong `angular` system
is accepted as an opt-in:

```cpp
polar_vector a{2.0 * m, 90.0 * deg};               // polar_vector<si::metre, si::degree, double>
polar_vector b{2.0 * m, half_pi * rad};            // ... si::radian
polar_vector c{2.0 * m, 30.0 * arcmin};            // ... si::arcminute
polar_vector d{2.0 * m, 90.0 * angular::degree};   // opt-in strong angular system
```

`radius()` (also `magnitude()`) returns the radius `quantity`, and `theta()` / `phi()` return
the typed angles.

## Using your own angle system

To convert to and from Cartesian, the facades need one system-specific fact: which unit is
a *radian* in the angle system you are using. That cannot be discovered generically, because
a from-scratch angle system defines its own radian, and the number of angle systems is open.
So the facades look it up through a small customization point, `mp_units::utility::radian_of`,
keyed on the angle *kind* (a `kind_of<…>`, i.e. what `get_quantity_spec` of a bare angle unit
yields). The two built-in angle kinds are registered by the library: SI's dimensionless angle
and the strong `angular` system. A unit whose kind is not registered is rejected at compile
time rather than silently mis-scaled, and the customization point itself only accepts a *kind*
as its key, so keying it on a concrete quantity spec by mistake is also a compile error.

If you define an angle system of your own, register its radian with a single specialization
keyed on its kind. The facades then work with it just like the built-in ones:

```cpp
namespace my {
inline constexpr struct dim_angle : base_dimension<"A"> {} dim_angle;
inline constexpr struct angle : quantity_spec<dim_angle> {} angle;
inline constexpr struct radian : named_unit<"rad", kind_of<angle>> {} radian;
inline constexpr struct degree : named_unit<"deg", mag_ratio<1, 180> * π * radian> {} degree;
}

// the whole opt-in, one line - keyed on the angle *kind*:
template<>
constexpr auto mp_units::utility::radian_of<kind_of<my::angle>> = my::radian;

polar_vector p{2.0 * m, 90.0 * my::degree};  // converts through my::radian
```

This is the only extension point the facades expose, and it lives with them rather than in
the core customization-point header. It is specific to these converters, not a general
library facility.

## Using an Eigen or Blaze vector

The facades read a vector representation's dimension from `std::tuple_size` and its
components with `get`, and `to_cartesian<To>()` builds whatever representation you name. So
they work with any linear algebra backend whose vector is tuple-like and provides the
`magnitude` customization point, on both sides of the conversion:

```cpp
#include <mp-units/integrations/eigen.h>
#include <mp-units/utility/spherical_vector.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// an Eigen vector quantity -> spherical and back
quantity<si::metre, Eigen::Vector3d> xyz{Eigen::Vector3d{3.0, 4.0, 0.0}, si::metre};
utility::spherical_vector s{xyz};        // r = 5 m, theta = pi/2, phi = atan2(4, 3)
Eigen::Vector3d back = s.to_cartesian<Eigen::Vector3d>().numerical_value_in(m);
```

The two supported backends need different amounts of glue, and the reason is instructive:

- **Blaze** ships the tuple protocol (`std::tuple_size`, `std::tuple_element`, and a `get`
  found by argument-dependent lookup) for its `StaticVector` itself, so a
  `quantity<si::metre, blaze::StaticVector<double, 3>>` is a valid Cartesian source and
  target as soon as `mp-units/integrations/blaze.h` is included. That header adds nothing
  for it.
- **Eigen** provides none of that for its matrices, so `mp-units/integrations/eigen.h` adds
  it for fixed-size Eigen vectors (an N-element row or column known at compile time).
  Dynamic-size vectors are excluded on purpose, because `std::tuple_size` needs a
  compile-time size.

`to_cartesian<To>()` constructs `To` from the components with parentheses when it can (an
aggregate or an N-argument constructor, as Eigen and `cartesian_vector` offer) and falls
back to braces for a representation whose only such constructor takes an `initializer_list`,
as Blaze's `StaticVector` does. The parenthesized form is preferred because it permits a
narrowing element conversion (building a `float` vector from `double` components), which
braces reject.

## Limitations (by design)

- **No `+`, `-`, dot, or cross.** Those are not component-wise here, so convert to
  `cartesian_vector` and do them there. (Radial scaling and rotation *are* provided, above.)
- **No implicit conversion.** Both directions are explicit: the constructor from a vector
  quantity and the `to_cartesian()` member.

The _angles_ are always kept canonical (polar `theta` wrapped, spherical `theta ∈ [0, π]`
with `phi` wrapped), so equality is **geometric**: `(r, theta)` and `(r, theta + 2π)` compare
equal, and a value beyond a pole equals its reflected-and-flipped canonical form.
