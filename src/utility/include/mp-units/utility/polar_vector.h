// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/module_macros.h>
#include <mp-units/utility/cartesian_vector.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/overflow_policies.h>       // wrap_to_range - reused to keep angles canonical
#include <mp-units/systems/angular.h>         // opt-in strong angular system (angular::radian/degree/...)
#include <mp-units/systems/si/units.h>        // si::radian - the default, and the SI radian to scale through
#include <mp-units/utility/representation.h>  // utility::Vector - the exported vector-representation concept
#include <mp-units/utility/unspecified.h>     // utility::unspecified / utility::specified
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <concepts>
#include <numbers>
#include <tuple>
#include <type_traits>
#include <utility>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units::utility {

// `polar_vector` and `spherical_vector` are *conversion facades*, not quantity representations. The
// radius is a scalar `quantity` and each angle is an angle `quantity`, so a value carries two
// different units and cannot be wrapped by a single `quantity<unit, ...>`. Conversion to and from a
// Cartesian vector quantity (`quantity<U, cartesian_vector<T, N>>`) is explicit. The operations that
// are exact and cheap in these coordinates - radial scaling and rotation - are provided natively;
// `+`, `-`, dot, and cross are not (they are not component-wise here), so convert to Cartesian for
// those.

/// @brief Maps an angle quantity kind to the unit that measures one radian in that kind's system.
///
/// The facades need the radian of whatever angle system a stored angle belongs to, to reach the
/// plain-number `std::sin`/`cos`/`atan2`. This customization point supplies it, keyed on the angle
/// *kind*. The lookup is `radian_of<get_quantity_spec(unit)>`, and `get_quantity_spec` of a bare unit
/// yields a `kind_of<...>`, so a registration must be keyed on that same kind - the `QS == get_kind(QS)`
/// constraint enforces this, turning an accidental key on a concrete spec (which would never match)
/// into a clean compile error. The two built-in angle kinds are registered right below: SI's
/// dimensionless angle (`si::radian`/`degree`/`arcminute`/...) and the opt-in strong `angular` system
/// (`angular::radian`/`degree`/...). To use the facades with an angle system of your own, register its
/// radian with a single specialization keyed on its kind (see the "Working with Polar and Spherical
/// Coordinates" guide):
/// @code
/// template<>
/// constexpr auto mp_units::utility::radian_of<kind_of<my::angle>> = my::radian;  // == get_quantity_spec(my::radian)
/// @endcode
/// It is left `unspecified` for every other kind, which is exactly what `detail::AngleUnit` rejects - so
/// a non-angle unit is a clean compile error, not a silently wrong conversion.
MP_UNITS_EXPORT template<QuantitySpec auto QS>
  requires(QS == get_kind(QS))
constexpr auto radian_of = unspecified;

template<>
constexpr auto radian_of<get_quantity_spec(angular::radian)> = angular::radian;

template<>
constexpr auto radian_of<get_quantity_spec(si::radian)> = si::radian;

namespace detail {

// An angle unit is usable iff its kind has a registered `radian_of` (the two built-in angle kinds
// above, plus any a user registers). The radian <-> angle conversions scale through that radian,
// so e.g. `si::degree` is converted rather than mistaken for radians. Because the map is keyed on the
// kind, a bare dimensionless unit (`one`, `percent`) is correctly *not* an angle unit.
template<typename U>
concept AngleUnit = Unit<U> && specified<decltype(radian_of<get_quantity_spec(U{})>)>;

// The radius is a *reference* - a unit, or a `quantity_spec[unit]` such as `isq::radius[m]` - carrying
// a radial *scalar* magnitude (a length, a speed, ...), never a vector/tensor and never an angle. That
// keeps the facade a genuine magnitude + direction decomposition and blocks category errors like a
// velocity-typed or angle-typed radius. The scalar character is required explicitly rather than left to
// the representation check: `double` is a valid 1-D vector rep, so `quantity<velocity[m/s], double>` is
// well-formed and a vector-spec radius would otherwise slip through in both V2 and V3.
template<typename R>
concept RadialReference =
  Reference<R> && (get_character(get_quantity_spec(R{})).order == quantity_tensor_order::scalar) &&
  !AngleUnit<MP_UNITS_NONCONST_TYPE(get_unit(R{}))>;

// Whether a vector representation `To` can be initialized from its components. Two spellings are
// possible, and `std::constructible_from` covers only the first: parenthesized init (aggregates since
// C++20, and reps with an N-argument constructor - e.g. Eigen and `cartesian_vector`) and braced init
// (reps whose only N-value constructor is an `initializer_list` one - e.g. Blaze's `StaticVector`).
template<typename To, typename... Cs>
concept InitializableFrom = requires(Cs... cs) { To(cs...); } || requires(Cs... cs) { To{cs...}; };

// Build a vector representation `To` from its components, preferring parenthesized init: it is the one
// that permits a narrowing element conversion (e.g. building a `float` vector from `double`
// components), which braced init rejects. Falls back to braced init for the `initializer_list`-only
// reps. `InitializableFrom` (checked by the callers' `requires`) guarantees at least one spelling works.
template<typename To, typename... Cs>
[[nodiscard]] constexpr To make_vector(Cs... cs)
{
  if constexpr (requires { To(cs...); })
    return To(cs...);
  else
    return To{cs...};
}

// A representation usable as a facade's Cartesian source *and* target: an `N`-dimensional vector
// representation that is also structured-bindings-compliant. We use the exported `utility::Vector`
// concept (order-1, magnitude-bearing, addable) rather than `RepresentationOf<V, ...vector>`: the
// latter is semantically a touch stronger (it also carries `RepresentationBaseline`) but its
// `order_of(get_character(V))` sub-expression hard-errors under GCC when the character-keyed concept is
// *nested* inside another one like this (fine at top level, not when normalized here). The output
// side still gets its full representation check from the `quantity{...}` construction in the body.
// Dimension reads from `std::tuple_size`, element type from `std::tuple_element`, components via
// `get`; those probes guard the reads below. `cartesian_vector<T, N>` models it, and so does any such
// vector rep (e.g. an Eigen/Blaze one) - the facades are not tied to `cartesian_vector`.
template<typename V, std::size_t N>
concept VectorRepOf = Vector<V> && requires {
  std::tuple_size<V>::value;
  typename std::tuple_element<0, V>::type;
} && (std::tuple_size_v<V> == N) && requires(const V& c) { get<0>(c); };

// The radius reference a facade takes when deduced (CTAD) from a vector quantity. A vector-character
// quantity (e.g. `isq::velocity[m/s]`) has a `magnitude()`, so we take that magnitude's reference: a
// kind over the unit in V2, upgrading for free to the strong scalar sibling (e.g. `isq::speed`, or a
// `magnitude<...>` expression) once V3 names it. A bare-unit vector quantity (`cartesian_vector * m`)
// has a scalar-character spec with a vector rep and thus no `magnitude()`; there is no strong scalar
// to recover, so we fall back to the plain unit.
template<Reference auto VR, typename V>
constexpr Reference auto magnitude_reference_of = [] {
  if constexpr (requires(quantity<VR, V> v) { v.magnitude(); })
    return decltype(std::declval<quantity<VR, V>>().magnitude())::reference;
  else
    return get_unit(VR);
}();

// Plain radian value of a stored angle.
template<Unit auto U, std::floating_point T>
[[nodiscard]] constexpr T angle_in_radians(const quantity<U, T>& a)
{
  return a.numerical_value_in(radian_of<get_quantity_spec(U)>);
}

// Build an angle in unit `U` from a plain radian value.
template<Unit auto U, std::floating_point T>
[[nodiscard]] constexpr quantity<U, T> radians_to_angle(T rad)
{
  return quantity{rad, radian_of<get_quantity_spec(U)>}.in(U);
}

// A half turn (pi radians) expressed in the angle unit `U` - the modulus for angle normalization.
template<Unit auto U, std::floating_point T>
[[nodiscard]] constexpr quantity<U, T> half_turn()
{
  return radians_to_angle<U, T>(std::numbers::pi_v<T>);
}

// Canonical azimuth: wrap into [-pi, pi) so directions that differ by full turns compare equal.
template<Unit auto U, std::floating_point T>
[[nodiscard]] constexpr quantity<U, T> wrap_azimuth(quantity<U, T> phi)
{
  const auto half = half_turn<U, T>();
  return wrap_to_range{-half, half}(phi);
}

// Canonical spherical angles: inclination folded into [0, pi], azimuth wrapped. Folding past a pole
// reflects the inclination AND flips the azimuth by pi (a point beyond a pole lies on the opposite
// meridian), so the result is the same physical direction expressed canonically.
template<Unit auto U, std::floating_point T>
[[nodiscard]] constexpr std::pair<quantity<U, T>, quantity<U, T>> canonical_spherical(quantity<U, T> theta,
                                                                                      quantity<U, T> phi)
{
  const auto half = half_turn<U, T>();
  const auto full = half + half;
  quantity<U, T> incl = wrap_to_range{quantity<U, T>{}, full}(theta);  // reduce inclination into [0, 2pi)
  if (incl > half) {                                                   // crossed a pole
    incl = full - incl;
    phi += half;
  }
  return {incl, wrap_azimuth(phi)};
}

}  // namespace detail

/// @brief A 2-D polar coordinate facade: a scalar-`quantity` radius `r` and an angle `theta` from
/// the +x axis (counter-clockwise).
///
/// `RadiusRef` is the radius reference - a unit like `si::metre`, or a `quantity_spec[unit]` like
/// `isq::radius[m]`; `AngleUnit` is the stored angle unit (default `si::radian`; SI's degree/arcminute
/// and the opt-in strong `angular` system are also accepted); `Rep` is the numeric type. The radius is
/// stored as `quantity<RadiusRef, Rep>`. Construct from native parts or explicitly from a 2-D vector
/// quantity, and convert back with `to_cartesian()`.
MP_UNITS_EXPORT template<detail::RadialReference auto RadiusRef, detail::AngleUnit auto AngleUnit = si::radian,
                         std::floating_point Rep = double>
class polar_vector {
public:
  using rep = Rep;
  using radius_type = quantity<RadiusRef, Rep>;
  using angle_type = quantity<AngleUnit, Rep>;
  static constexpr Reference auto radius_reference = RadiusRef;
  static constexpr Unit auto radius_unit = get_unit(RadiusRef);
  static constexpr Unit auto angle_unit = AngleUnit;

  // The coordinates are public because that keeps the facade a structural type (usable as a non-type
  // template argument); a private data member would make it non-structural - same reason `quantity`
  // keeps its numerical value public.
  radius_type _r_{};
  angle_type _theta_{};

  [[nodiscard]] polar_vector() = default;

  // The angle is normalized to the canonical azimuth range so that directions differing by whole
  // turns are equal.
  [[nodiscard]] constexpr polar_vector(radius_type r, angle_type theta) : _r_(r), _theta_(detail::wrap_azimuth(theta))
  {
  }

  // Explicit construction from a 2-D vector quantity: r = |v|, theta = atan2(y, x). Works with any
  // tuple-like vector representation of dimension 2 that offers the `magnitude` CPO - not only
  // `cartesian_vector` - reading its components via structured bindings (see `detail::VectorRepOf`).
  // The element type may differ from `Rep`; it is `static_cast`, so a narrowing target stays
  // warning-clean. Always `explicit` - a Cartesian round-trip is never implicit. (`VR` is plain `auto`
  // to avoid the GCC 15 constrained-`auto`-NTTP ICE.)
  template<auto VR, detail::VectorRepOf<2> V>
    requires requires(const quantity<VR, V>& v) { v.numerical_value_in(get_unit(RadiusRef)); }
  [[nodiscard]] constexpr explicit polar_vector(const quantity<VR, V>& v) :
      _r_(static_cast<Rep>(::mp_units::magnitude(v.numerical_value_in(get_unit(RadiusRef)))), RadiusRef), _theta_([&] {
        using std::atan2;
        const V c = v.numerical_value_in(get_unit(RadiusRef));
        const auto& [x, y] = c;
        return detail::wrap_azimuth(detail::radians_to_angle<AngleUnit, Rep>(static_cast<Rep>(atan2(y, x))));
      }())
  {
  }

  // Converting constructor from a polar_vector over different units and/or a different rep. The radius
  // and angle quantity kinds must match and the units be inter-convertible (the `numerical_value_in`
  // constraints). The rep follows exactly the `cartesian_vector` policy: constrained by
  // `constructible_from`, `explicit` unless the rep conversion is value-preserving
  // (`ImplicitlyConvertibleScalar`), and `static_cast` in the body so a narrowing target stays
  // warning-clean. The source is already canonical and unit/rep conversion preserves that (no re-wrap).
  // (`RU`/`AU` are plain `auto`: the source `polar_vector` already constrains them, so re-checking here
  // is redundant - and a *constrained* `auto` NTTP deduced from a class specialization ICEs GCC 15.)
  template<auto RU, auto AU, typename Rep2>
    requires std::constructible_from<Rep, Rep2> && requires(const quantity<RU, Rep2>& r, const quantity<AU, Rep2>& a) {
      r.numerical_value_in(get_unit(RadiusRef));
      a.numerical_value_in(AngleUnit);
    }
  [[nodiscard]] constexpr explicit(!detail::ImplicitlyConvertibleScalar<Rep2, Rep>)
    polar_vector(const polar_vector<RU, AU, Rep2>& other) :
      _r_(static_cast<Rep>(other.radius().numerical_value_in(get_unit(RadiusRef))), RadiusRef),
      _theta_(static_cast<Rep>(other.theta().numerical_value_in(AngleUnit)), AngleUnit)
  {
  }

  [[nodiscard]] constexpr radius_type radius() const { return _r_; }
  [[nodiscard]] constexpr radius_type magnitude() const { return _r_; }
  [[nodiscard]] constexpr angle_type theta() const { return _theta_; }

  // Explicit conversion to a 2-D Cartesian vector quantity: (r cos theta, r sin theta). The destination
  // representation `To` defaults to `cartesian_vector`, but may be any 2-D vector representation
  // (`detail::VectorRepOf<To, 2>`, the same contract accepted on input) that is initializable from the
  // two components. Initialization uses parenthesized init (aggregate or N-argument constructor, e.g.
  // Eigen) or braced init (an `initializer_list` constructor, e.g. Blaze); see `detail::make_vector`.
  template<typename To = cartesian_vector<Rep, 2>>
    requires detail::VectorRepOf<To, 2> && detail::InitializableFrom<To, Rep, Rep>
  [[nodiscard]] constexpr auto to_cartesian() const
  {
    using std::cos;
    using std::sin;
    const Rep th = detail::angle_in_radians(_theta_);
    const Rep r = _r_.numerical_value_in(get_unit(RadiusRef));
    return quantity{detail::make_vector<To>(static_cast<Rep>(r * cos(th)), static_cast<Rep>(r * sin(th))),
                    get_unit(RadiusRef)};
  }

  // -- Natively-correct operations (everything additive goes through to_cartesian) --

  // Radial scaling: multiply/divide the radius, leaving the angle untouched - exact and cheap, no
  // Cartesian round-trip. The factor may be a plain number, a dimensionless quantity, or a
  // dimensioned quantity; a dimensioned factor changes the radius unit (e.g. a velocity polar_vector
  // times a duration yields a length one). Negative factors are allowed.
  template<Quantity Factor>
  [[nodiscard]] friend constexpr auto operator*(const polar_vector& v, const Factor& f)
  {
    using scaled = decltype(v._r_ * f);
    return polar_vector<scaled::reference, AngleUnit, typename scaled::rep>{v._r_ * f, v._theta_};
  }

  template<Quantity Factor>
  [[nodiscard]] friend constexpr auto operator/(const polar_vector& v, const Factor& f)
  {
    using scaled = decltype(v._r_ / f);
    return polar_vector<scaled::reference, AngleUnit, typename scaled::rep>{v._r_ / f, v._theta_};
  }

  // Dimensionless scaling by a plain number keeps the radius unit unchanged.
  [[nodiscard]] friend constexpr polar_vector operator*(const Rep& scalar, const polar_vector& v)
  {
    return {scalar * v._r_, v._theta_};
  }

  [[nodiscard]] friend constexpr polar_vector operator*(const polar_vector& v, const Rep& scalar) { return scalar * v; }

  [[nodiscard]] friend constexpr polar_vector operator/(const polar_vector& v, const Rep& scalar)
  {
    return {v._r_ / scalar, v._theta_};
  }

  // Rotation by an angle (exact: adds to theta, no trig). The delta is in the stored angle unit.
  [[nodiscard]] constexpr polar_vector rotated_by(angle_type delta) const { return {_r_, _theta_ + delta}; }

  // Coordinate-wise equality: the stored (r, theta), NOT geometric equivalence.
  [[nodiscard]] friend constexpr bool operator==(const polar_vector&, const polar_vector&) = default;

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const polar_vector& v)
  {
    return os << "(r = " << v._r_ << ", θ = " << v._theta_ << ')';
  }
#endif
};

template<Reference auto RR, Unit auto AU, std::floating_point Rep>
polar_vector(quantity<RR, Rep>, quantity<AU, Rep>) -> polar_vector<RR, AU, Rep>;

template<Reference auto VR, detail::VectorRepOf<2> V>
polar_vector(quantity<VR, V>)
  -> polar_vector<detail::magnitude_reference_of<VR, V>, si::radian, std::tuple_element_t<0, V>>;

}  // namespace mp_units::utility
