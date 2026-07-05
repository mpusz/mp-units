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
#include <mp-units/utility/polar_vector.h>  // shared angle helpers + the AngleUnit/RadialUnit concepts and radian_of

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/systems/angular.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <concepts>
#if MP_UNITS_HOSTED
#include <ostream>
#endif
#endif
#endif

namespace mp_units::utility {

/// @brief A 3-D spherical coordinate facade (ISO physics convention): a scalar-`quantity` radius
/// `r`, inclination `theta` from +z in `[0, pi]`, and azimuth `phi` from +x in `[-pi, pi]`.
///
/// Like `polar_vector` it is a conversion facade, not a quantity representation. Construct from
/// native parts or explicitly from a 3-D vector quantity, and convert back with `to_cartesian()`.
MP_UNITS_EXPORT template<detail::RadialReference auto RadiusRef, detail::AngleUnit auto AngleUnit = si::radian,
                         std::floating_point Rep = double>
class spherical_vector {
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
  angle_type _phi_{};

  spherical_vector() = default;

  // The angles are normalized to the canonical (inclination in [0, pi], azimuth wrapped) form, so
  // out-of-range or pole-crossing inputs become the same physical direction expressed canonically.
  constexpr spherical_vector(radius_type r, angle_type theta, angle_type phi) : _r_(r)
  {
    const auto [t, p] = detail::canonical_spherical(theta, phi);
    _theta_ = t;
    _phi_ = p;
  }

  // Explicit construction from a 3-D vector quantity: r = |v|, theta = acos(z / r), phi = atan2(y, x).
  // For r == 0 the angles are left at zero (the direction is undefined). Works with any tuple-like
  // vector representation of dimension 3 that offers the `magnitude` CPO - not only `cartesian_vector`
  // - reading its components via structured bindings (see `detail::VectorRepOf`). The trig is done in
  // the vector's own element type and `static_cast` to `Rep`, so a narrowing target stays
  // warning-clean. Always `explicit` - a Cartesian round-trip is never implicit.
  template<auto VR, detail::VectorRepOf<3> V>
    requires requires(const quantity<VR, V>& v) { v.numerical_value_in(get_unit(RadiusRef)); }
  constexpr explicit spherical_vector(const quantity<VR, V>& v)
  {
    using std::acos;
    using std::atan2;
    using elem = std::tuple_element_t<0, V>;
    const V c = v.numerical_value_in(get_unit(RadiusRef));
    const auto& [x, y, z] = c;
    const elem rv = static_cast<elem>(::mp_units::magnitude(c));
    _r_ = quantity{static_cast<Rep>(rv), RadiusRef};
    const auto theta_raw =
      rv != elem{0} ? detail::radians_to_angle<AngleUnit, Rep>(static_cast<Rep>(acos(z / rv))) : angle_type{};
    const auto [t, p] =
      detail::canonical_spherical(theta_raw, detail::radians_to_angle<AngleUnit, Rep>(static_cast<Rep>(atan2(y, x))));
    _theta_ = t;
    _phi_ = p;
  }

  // Converting constructor from a spherical_vector over different units and/or a different rep. The
  // radius and angle quantity kinds must match and the units be inter-convertible (the
  // `numerical_value_in` constraints). The rep follows exactly the `cartesian_vector` policy:
  // constrained by `constructible_from`, `explicit` unless the rep conversion is value-preserving
  // (`ImplicitlyConvertibleScalar`), and `static_cast` in the body so a narrowing target stays
  // warning-clean. The source is already canonical and unit/rep conversion preserves that (no re-wrap).
  // (`RU`/`AU` are plain `auto`: the source `spherical_vector` already constrains them, so re-checking
  // here is redundant - and a *constrained* `auto` NTTP deduced from a class specialization ICEs GCC 15.)
  template<auto RU, auto AU, typename Rep2>
    requires std::constructible_from<Rep, Rep2> && requires(const quantity<RU, Rep2>& r, const quantity<AU, Rep2>& a) {
      r.numerical_value_in(get_unit(RadiusRef));
      a.numerical_value_in(AngleUnit);
    }
  constexpr explicit(!detail::ImplicitlyConvertibleScalar<Rep2, Rep>)
    spherical_vector(const spherical_vector<RU, AU, Rep2>& other) :
      _r_(static_cast<Rep>(other.radius().numerical_value_in(get_unit(RadiusRef))), RadiusRef),
      _theta_(static_cast<Rep>(other.theta().numerical_value_in(AngleUnit)), AngleUnit),
      _phi_(static_cast<Rep>(other.phi().numerical_value_in(AngleUnit)), AngleUnit)
  {
  }

  [[nodiscard]] constexpr radius_type radius() const { return _r_; }
  [[nodiscard]] constexpr radius_type magnitude() const { return _r_; }
  [[nodiscard]] constexpr angle_type theta() const { return _theta_; }
  [[nodiscard]] constexpr angle_type phi() const { return _phi_; }

  // Explicit conversion to a 3-D Cartesian vector quantity:
  //   (r sin(theta) cos(phi), r sin(theta) sin(phi), r cos(theta)).
  // The destination representation `To` defaults to `cartesian_vector`, but may be any 3-D vector
  // representation (`detail::VectorRepOf<To, 3>`, the same contract accepted on input) that is
  // initializable from the three components. Initialization uses parenthesized init (aggregate or
  // N-argument constructor, e.g. Eigen) or braced init (an `initializer_list` constructor, e.g.
  // Blaze); see `detail::make_vector`.
  template<typename To = cartesian_vector<Rep, 3>>
    requires detail::VectorRepOf<To, 3> && detail::InitializableFrom<To, Rep, Rep, Rep>
  [[nodiscard]] constexpr auto to_cartesian() const
  {
    using std::cos;
    using std::sin;
    const Rep th = detail::angle_in_radians(_theta_);
    const Rep ph = detail::angle_in_radians(_phi_);
    const Rep r = _r_.numerical_value_in(get_unit(RadiusRef));
    return quantity{detail::make_vector<To>(static_cast<Rep>(r * sin(th) * cos(ph)),
                                            static_cast<Rep>(r * sin(th) * sin(ph)), static_cast<Rep>(r * cos(th))),
                    get_unit(RadiusRef)};
  }

  // -- Natively-correct operations --

  // Radial scaling: multiply/divide the radius, leaving the angles untouched - exact and cheap, no
  // Cartesian round-trip. A dimensioned factor changes the radius unit (e.g. a velocity
  // spherical_vector times a duration yields a length one). Negative factors are allowed.
  template<Quantity Factor>
  [[nodiscard]] friend constexpr auto operator*(const spherical_vector& v, const Factor& f)
  {
    using scaled = decltype(v._r_ * f);
    return spherical_vector<scaled::reference, AngleUnit, typename scaled::rep>{v._r_ * f, v._theta_, v._phi_};
  }
  template<Quantity Factor>
  [[nodiscard]] friend constexpr auto operator/(const spherical_vector& v, const Factor& f)
  {
    using scaled = decltype(v._r_ / f);
    return spherical_vector<scaled::reference, AngleUnit, typename scaled::rep>{v._r_ / f, v._theta_, v._phi_};
  }

  // Dimensionless scaling by a plain number keeps the radius unit unchanged.
  [[nodiscard]] friend constexpr spherical_vector operator*(const Rep& s, const spherical_vector& v)
  {
    return {s * v._r_, v._theta_, v._phi_};
  }
  [[nodiscard]] friend constexpr spherical_vector operator*(const spherical_vector& v, const Rep& s) { return s * v; }
  [[nodiscard]] friend constexpr spherical_vector operator/(const spherical_vector& v, const Rep& s)
  {
    return {v._r_ / s, v._theta_, v._phi_};
  }

  // Rotation about the +z (polar) axis: adds to the azimuth (wrapped to canonical by the ctor).
  [[nodiscard]] constexpr spherical_vector rotated_about_z(angle_type delta) const
  {
    return {_r_, _theta_, _phi_ + delta};
  }

  // Change the inclination by `delta` (rotation in the meridian plane). Crossing a pole is handled
  // correctly by the ctor's canonicalization: the inclination reflects and the azimuth flips by pi.
  [[nodiscard]] constexpr spherical_vector tilted_by(angle_type delta) const { return {_r_, _theta_ + delta, _phi_}; }

  // Coordinate-wise equality on the canonical (r, theta, phi) - equal iff the same direction.
  [[nodiscard]] friend constexpr bool operator==(const spherical_vector&, const spherical_vector&) = default;

#if MP_UNITS_HOSTED
  friend std::ostream& operator<<(std::ostream& os, const spherical_vector& v)
  {
    return os << "(r = " << v._r_ << ", θ = " << v._theta_ << ", φ = " << v._phi_ << ')';
  }
#endif
};

template<Reference auto RR, Unit auto AU, std::floating_point Rep>
spherical_vector(quantity<RR, Rep>, quantity<AU, Rep>, quantity<AU, Rep>) -> spherical_vector<RR, AU, Rep>;

template<Reference auto VR, detail::VectorRepOf<3> V>
spherical_vector(quantity<VR, V>)
  -> spherical_vector<detail::magnitude_reference_of<VR, V>, si::radian, std::tuple_element_t<0, V>>;

}  // namespace mp_units::utility
