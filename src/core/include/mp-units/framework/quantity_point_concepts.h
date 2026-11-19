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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/point_origin_concepts.h>
#include <mp-units/framework/quantity_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/reference_concepts.h>
#include <mp-units/framework/representation_concepts.h>

namespace mp_units {

MP_UNITS_EXPORT template<QuantitySpec auto QS, auto... Args>
struct absolute_point_origin;

namespace detail {

// exposition-only: matches types derived from absolute_point_origin
template<typename T>
concept AbsolutePointOrigin = detail::PointOrigin<T> && is_derived_from_specialization_of_v<T, absolute_point_origin>;

}  // namespace detail

/**
 * User-specializable variable template for runtime coordinate frame transformations.
 *
 * Connects pairs of absolute point origin values with user-supplied callables. Specialize per
 * (From, To) pair with a callable invocable as:
 *   frame_projection<From, To>(quantity_point_at_From, extra_args...) -> quantity_point_at_To
 *
 * Both directions must be explicitly specialized — the library never derives an inverse.
 * The primary template defaults to `utility::unspecified_t{}` so that `detail::HasFrameProjection`
 * can detect whether a specialization exists.
 */
MP_UNITS_EXPORT template<detail::AbsolutePointOrigin auto From, detail::AbsolutePointOrigin auto To>
  requires(From != To)
inline constexpr utility::unspecified_t frame_projection;

namespace detail {

template<typename T>
constexpr bool is_quantity_point = false;

}  // namespace detail

/**
 * A concept matching every quantity point
 *
 * Satisfied by every specialization of @ref quantity_point and every type derived from one. Its
 * counterpart for deltas is @ref Quantity, and the two tell apart overloads that a delta and a
 * point need to handle differently:
 *
 * @code{.cpp}
 * // one name for printing a temperature and a temperature difference
 * void print_temp(std::string_view title, QuantityPoint auto temp)
 * {
 *   std::cout << title << ": " << temp << " (" << temp.delta_from(si::absolute_zero).in(K)
 *             << " above absolute zero)\n";
 * }
 *
 * void print_temp(std::string_view title, Quantity auto temp)
 * {
 *   std::cout << title << ": " << temp << " (" << temp.in(K) << ")\n";
 * }
 *
 * void report()
 * {
 *   quantity_point today = point<deg_C>(19.);
 *   quantity_point tomorrow = point<deg_C>(21.);
 *   print_temp("tomorrow", tomorrow);        // tomorrow: 21 °C (294.15 K above absolute zero)
 *   print_temp("change", tomorrow - today);  // change: 2 °C (2 K)
 * }
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#QuantityPoint) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept QuantityPoint = detail::is_quantity_point<T>;

MP_UNITS_EXPORT template<QuantityPoint auto QP, auto... Args>
struct relative_point_origin;

/**
 * A concept matching the point origins of a given quantity
 *
 * Satisfied by a point origin defined for a quantity that `QS` converts to implicitly. So an
 * origin of _altitude_ serves _altitude_ and _height_, but not _length_ or _thermodynamic
 * temperature_.
 *
 * @code{.cpp}
 * inline constexpr struct mean_sea_level final :
 *   absolute_point_origin<isq::altitude> {} mean_sea_level;
 *
 * // a runway surveyed against one datum, which both of its points are measured from
 * template<PointOriginFor<isq::altitude> auto Datum>
 * struct runway_survey {
 *   quantity_point<isq::altitude[m], Datum> threshold;
 *   quantity_point<isq::altitude[m], Datum> far_end;
 * };
 *
 * constexpr runway_survey<mean_sea_level> runway{mean_sea_level + isq::altitude(110. * m),
 *                                                mean_sea_level + isq::altitude(116. * m)};
 * // doesn't compile: not an origin of altitude
 * // runway_survey<si::absolute_zero> bad;
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#PointOriginFor) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T, auto QS>
concept PointOriginFor = detail::PointOrigin<T> && QuantitySpecOf<MP_UNITS_NTTP_HACK(decltype(QS)), T::_quantity_spec_>;

MP_UNITS_EXPORT template<Reference auto R, PointOriginFor<get_quantity_spec(R)> auto PO,
                         RepresentationOf<get_quantity_spec(R)> Rep>
class quantity_point;

// Matched by pattern, as for `quantity`: a point is rejected as a representation before anything is
// evaluated on it.
template<auto R, auto PO, typename Rep>
inline constexpr bool disable_representation<quantity_point<R, PO, Rep>> = true;

namespace detail {

template<auto R, auto PO, typename Rep>
void to_base_specialization_of_quantity_point(const volatile quantity_point<R, PO, Rep>*);

template<typename T>
constexpr bool is_derived_from_specialization_of_quantity_point =
  requires(T* type) { to_base_specialization_of_quantity_point(type); };

template<typename T>
  requires is_derived_from_specialization_of_quantity_point<T>
constexpr bool is_quantity_point<T> = true;

template<detail::PointOrigin PO1, detail::PointOrigin PO2>
[[nodiscard]] consteval bool same_absolute_point_origins(PO1 po1, PO2 po2)
{
  if constexpr (is_derived_from_specialization_of_v<PO1, absolute_point_origin> &&
                is_derived_from_specialization_of_v<PO2, absolute_point_origin>)
    return po1 == po2;
  else if constexpr (is_derived_from_specialization_of_v<PO1, relative_point_origin> &&
                     is_derived_from_specialization_of_v<PO2, relative_point_origin>)
    return po1._absolute_point_origin_ == po2._absolute_point_origin_;
  else if constexpr (is_derived_from_specialization_of_v<PO1, relative_point_origin>)
    return po1._absolute_point_origin_ == po2;
  else if constexpr (is_derived_from_specialization_of_v<PO2, relative_point_origin>)
    return po1 == po2._absolute_point_origin_;
  else
    return false;
}

template<typename T, auto V>
concept SameAbsolutePointOriginAs =
  detail::PointOrigin<T> && detail::PointOrigin<MP_UNITS_NTTP_HACK(decltype(V))> && same_absolute_point_origins(T{}, V);

}  // namespace detail


/**
 * A concept matching the quantity points of a given quantity or origin
 *
 * `V` may be a quantity specification, in which case this is satisfied by every point whose
 * reference satisfies @ref ReferenceOf with `V`, a point origin, in which case it is satisfied by
 * every point measured from the same absolute origin, or a character (a bare
 * `quantity_tensor_order`, a bare `quantity_field`, or a full `quantity_character`), in which
 * case it is satisfied by every point of that character.
 *
 * A point isn't a @ref Quantity, so `QuantityPointOf` is where its character is asked.
 *
 * @code{.cpp}
 * // the ideal gas law needs a temperature measured from absolute zero, never a difference
 * constexpr QuantityOf<isq::pressure> auto gas_pressure(
 *   QuantityOf<dimensionless> auto particles, QuantityPointOf<si::absolute_zero> auto temp,
 *   QuantityOf<isq::volume> auto volume)
 * {
 *   return particles * si::boltzmann_constant * temp.delta_from(si::absolute_zero) / volume;
 * }
 *
 * constexpr quantity particles = 2.5e22 * one;
 * constexpr quantity volume = 1. * L;
 * constexpr quantity pressure = gas_pressure(particles, point<K>(300.), volume);   // ≈ 103.5 kPa
 * constexpr quantity same = gas_pressure(particles, point<deg_C>(26.85), volume);  // in Celsius
 * // doesn't compile: a temperature difference isn't a temperature
 * // auto bad = gas_pressure(particles, delta<deg_C>(26.85), volume);
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#QuantityPointOf) in the User's Guide
 */
MP_UNITS_EXPORT template<typename QP, auto V>
concept QuantityPointOf =
  QuantityPoint<QP> &&
  ((QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> && ReferenceOf<decltype(QP::reference), V>) ||
   detail::SameAbsolutePointOriginAs<decltype(QP::absolute_point_origin), V> ||
   // One atomic requires-expression, as in `QuantityOf`, so MSVC 19.51 doesn't run out of heap comparing
   // constraints.
   (detail::CharacterValue<V> &&
    requires { requires QuantitySpecOf<decltype(QP::quantity_spec), V> && RepresentationOf<typename QP::rep, V>; }));

namespace detail {

// An external type that a `quantity_point_like_traits` specialization describes as a quantity point.
// Exposition-only, as `QuantityLike` is: users specialize the traits and never need to name the concept.
template<typename T>
concept QuantityPointLike = !QuantityPoint<T> && QuantityLikeImpl<T, quantity_point_like_traits> && requires {
  typename quantity_point<quantity_point_like_traits<T>::reference, quantity_point_like_traits<T>::point_origin,
                          typename quantity_point_like_traits<T>::rep>;
};

template<auto From, auto To>
concept HasFrameProjection =
  AbsolutePointOrigin<MP_UNITS_NTTP_HACK(decltype(From))> && AbsolutePointOrigin<MP_UNITS_NTTP_HACK(decltype(To))> &&
  utility::specified<decltype(frame_projection<From, To>)>;

}  // namespace detail

}  // namespace mp_units
