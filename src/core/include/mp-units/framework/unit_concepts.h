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
#include <mp-units/bits/unsatisfied.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/framework/point_origin_concepts.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_magnitude.h>

namespace mp_units {

namespace detail {

struct unit_interface;

template<typename T>
constexpr bool is_specialization_of_prefixed_unit = false;

// A unit measures a quantity kind, so a base unit or a unit restricted to a quantity takes
// `kind_of<isq::length>`, not `isq::length`. Forgetting `kind_of` is the common mistake, which
// otherwise fails as an incomplete type. Only a mistake names `named_unit` with arguments that fail
// this, so the sentence is not gated.
template<auto V>
concept QuantitySpecIsAKind =
  !QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> || QuantityKindSpec<MP_UNITS_NTTP_HACK(decltype(V))> ||
  unsatisfied<"A unit is defined for a quantity kind, so it takes 'kind_of<{}>' rather than the quantity itself">(
    type_name(V));

}  // namespace detail

/**
 * Writes a unit's symbol directly after the number, with no space between them
 *
 * The SI separates a number from a unit symbol with a space, and makes an exception only for
 * the degree, minute and second of plane angle: `90°`, `30′`, `15″`. A unit that follows the
 * same convention passes this tag to @ref named_unit, after the other arguments and before or
 * after a point origin:
 *
 * @code{.cpp}
 * // right ascension in astronomy is written as 5ʰ 35ᵐ, with no space
 * inline constexpr struct hour_angle final :
 *   named_unit<symbol_text{u8"ʰ", "h"}, mag<15> * non_si::degree, no_space_before_unit_symbol> {} hour_angle;
 *
 * void print_right_ascension() { std::cout << 5 * hour_angle << '\n'; }  // 5ʰ
 * @endcode
 *
 * A prefixed unit, or another @ref named_unit defined in terms of a unit that passes the tag,
 * has a symbol of its own, so it is printed with the space unless it passes the tag too.
 * A format specification without `%?`, such as `{:%N %U}`, ignores the tag.
 *
 * @see [Text Output](docs:users_guide/framework_basics/text_output#no-space-before-the-unit-symbol)
 * in the User's Guide
 */
MP_UNITS_EXPORT inline constexpr struct no_space_before_unit_symbol {
} no_space_before_unit_symbol;

namespace detail {

// Whatever `named_unit` accepts after the unit's definition, in any order.
template<typename T>
concept NamedUnitProperty =
  PointOrigin<T> || std::is_same_v<std::remove_const_t<T>, struct mp_units::no_space_before_unit_symbol>;

template<auto V>
concept NamedUnitArgument =
  std::is_base_of_v<unit_interface, MP_UNITS_NTTP_HACK(decltype(V))> ||
  (QuantitySpec<MP_UNITS_NTTP_HACK(decltype(V))> && QuantitySpecIsAKind<V>) ||
  NamedUnitProperty<MP_UNITS_NTTP_HACK(decltype(V))> ||
  unsatisfied<"'{}' is neither a unit, a quantity kind, a point origin nor 'no_space_before_unit_symbol'">(
    type_name(V));

template<auto... Args>
concept AtMostOnePointOrigin =
  (std::size_t{0} + ... + std::size_t{PointOrigin<MP_UNITS_NTTP_HACK(decltype(Args))>}) <= 1 ||
  unsatisfied<"A unit takes at most one point origin">();

template<auto... Args>
concept AtMostOneNoSpaceTag = (std::size_t{0} + ... +
                               std::size_t{std::is_same_v<std::remove_const_t<MP_UNITS_NTTP_HACK(decltype(Args))>,
                                                          struct mp_units::no_space_before_unit_symbol>}) <= 1 ||
                              unsatisfied<"'no_space_before_unit_symbol' is given more than once">();

template<auto... Args>
[[nodiscard]] consteval bool properties_come_last()
{
  bool property_seen = false;
  bool in_order = true;
  ((in_order = in_order && (!property_seen || NamedUnitProperty<MP_UNITS_NTTP_HACK(decltype(Args))>),
    property_seen = property_seen || NamedUnitProperty<MP_UNITS_NTTP_HACK(decltype(Args))>),
   ...);
  return in_order;
}

// Most units take no property, and the fold spares them the function instantiation.
template<auto... Args>
concept PropertiesComeLast =
  !(... || NamedUnitProperty<MP_UNITS_NTTP_HACK(decltype(Args))>) || properties_come_last<Args...>() ||
  unsatisfied<"A point origin and 'no_space_before_unit_symbol' come after the unit and the quantity kind">();

// Counted with a fold, not `sizeof...`: Clang 22 and later take a constraint in a concept that reads a non-type pack
// only through `sizeof...` as satisfied (llvm/llvm-project#246501).
// A single argument is the unit's definition, so most units skip the checks on the properties.
template<auto... Args>
concept NamedUnitArguments =
  (... && NamedUnitArgument<Args>) &&
  ((std::size_t{0} + ... + ((void)Args, std::size_t{1})) == 1 ||
   (AtMostOnePointOrigin<Args...> && AtMostOneNoSpaceTag<Args...> && PropertiesComeLast<Args...>));

}  // namespace detail

MP_UNITS_EXPORT template<symbol_text Symbol, auto... Args>
  requires detail::NamedUnitArguments<Args...>
struct named_unit;

// `is_base_of_v` rather than `derived_from`: one constraint atom instead of two, evaluated per type; the base is
// always public and unambiguous, so the convertibility atom `derived_from` adds can never fail here.
/**
 * A concept matching every unit
 *
 * Satisfied by a named unit defined with `named_unit`, such as `si::metre`, by a prefixed unit,
 * such as `si::kilo<si::metre>`, by the unnamed unit a unit equation produces, such as
 * `si::metre / si::second`, and by a physical constant defined with `named_constant`.
 *
 * @code{.cpp}
 * // the raw number a protocol field holds, in the unit and the type the caller names; a
 * // conversion that would lose information doesn't compile
 * template<Unit auto U, typename Rep>
 * constexpr auto encode(QuantityOf<get_quantity_spec(U)> auto q)
 *   requires requires { q.template numerical_value_in<Rep>(U); }
 * {
 *   return q.template numerical_value_in<Rep>(U);
 * }
 *
 * constexpr std::int64_t field = encode<mm, std::int64_t>(2 * m);  // 2000
 * // doesn't compile: 1.5 km truncates
 * // auto bad = encode<km, std::int64_t>(1500 * m);
 * @endcode
 *
 * @code{.cpp}
 * // a trait keyed by unit
 * template<Unit auto U>
 * constexpr std::string_view spelled_name = "";
 *
 * template<>
 * constexpr std::string_view spelled_name<si::metre> = "metre";
 * template<>
 * constexpr std::string_view spelled_name<si::second> = "second";
 *
 * constexpr std::string_view name = spelled_name<si::metre>;  // "metre"
 * // doesn't compile; unconstrained, it would key the trait by a quantity
 * // template<>
 * // constexpr std::string_view spelled_name<isq::length> = "length";
 * @endcode
 *
 * @code{.cpp}
 * // values of types nobody spells, so naming what each one is helps where `auto` alone doesn't
 * constexpr QuantitySpec auto power_spec = isq::energy / isq::duration;
 * constexpr Dimension auto power_dimension = get_dimension(power_spec);
 * constexpr Unit auto power_unit = si::joule / si::second;
 * constexpr Reference auto power_reference = power_spec[power_unit];
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#Unit) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept Unit = std::is_base_of_v<detail::unit_interface, T> && detail::SymbolicConstant<T> &&
               !is_specialization_of_v<std::remove_const_t<T>, named_unit> &&
               !detail::is_specialization_of_prefixed_unit<std::remove_const_t<T>>;

/**
 * A concept matching the units that `prefixed_unit` accepts
 *
 * Satisfied by a unit derived from `named_unit`, such as `si::metre` or `si::gram`. A prefixed
 * unit, such as `si::kilogram`, and an unnamed derived unit, such as `si::metre / si::second`,
 * don't satisfy it. The concept doesn't encode which prefixes a system's conventions allow, so
 * `si::kilo<si::hour>` compiles, although the SI doesn't prefix the hour.
 *
 * @code{.cpp}
 * // a prefix of 10⁴ that the SI doesn't define, declared the way the SI prefixes are
 * template<PrefixableUnit U>
 * struct myria_ final : prefixed_unit<"my", mag_power<10, 4>, U{}> {};
 *
 * template<PrefixableUnit auto U>
 * constexpr myria_<decltype(U)> myria;
 *
 * constexpr quantity distance = 1 * myria<si::metre>;  // 1 mym, the same as 10 km
 * // doesn't compile: the kilogram already has a prefix
 * // auto bad = myria<si::kilogram>;
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#PrefixableUnit) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept PrefixableUnit = Unit<T> && is_derived_from_specialization_of_v<T, named_unit>;

/**
 * A concept matching the units of a given quantity
 *
 * Satisfied by a unit whose quantity converts implicitly to `QS`. `si::metre` is a unit of
 * `isq::height`, while `si::hertz` is not a unit of `isq::activity`, which has the becquerel.
 *
 * @code{.cpp}
 * // a motion profile whose members all derive their units from the unit of length it is given
 * template<UnitOf<isq::length> auto L>
 * struct motion_profile {
 *   quantity<isq::distance[L]> distance;
 *   quantity<isq::speed[L / si::second]> speed;
 *   quantity<isq::acceleration[L / square(si::second)]> acceleration;
 * };
 *
 * constexpr motion_profile<km> launch{isq::distance(3. * km), isq::speed(1.2 * km / s),
 *                                     isq::acceleration(0.03 * km / s2)};
 * // doesn't compile: not a unit of length
 * // motion_profile<s> bad;
 * @endcode
 *
 * @code{.cpp}
 * // the label of a plot axis that shows lengths, in the unit its user picked
 * template<UnitOf<isq::length> auto U>
 * constexpr std::string_view axis_label() { return unit_symbol(U); }
 *
 * constexpr std::string_view label = axis_label<km>();  // "km"
 * // doesn't compile; unconstrained, it would label the axis "s"
 * // auto bad = axis_label<s>();
 * @endcode
 *
 * @code{.cpp}
 * // values of types nobody spells, so naming what each one is and which quantity it belongs to
 * // helps where `auto` alone doesn't
 * constexpr QuantitySpecOf<isq::power> auto power_spec = isq::energy / isq::duration;
 * constexpr UnitOf<isq::power> auto power_unit = si::joule / si::second;
 * constexpr ReferenceOf<isq::power> auto power_reference = power_spec[power_unit];
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#UnitOf) in the User's Guide
 */
MP_UNITS_EXPORT template<typename U, auto QS>
concept UnitOf =
  Unit<U> && QuantitySpec<MP_UNITS_NTTP_HACK(decltype(QS))> &&
  (mp_units::utility::implicitly_convertible(get_quantity_spec(U{}), QS) ||
   (unsatisfied<"Unit '{}' is associated with quantity of kind '{}' which is not convertible to the '{}' quantity">(
     U{}, type_name(get_quantity_spec(U{})._quantity_spec_), type_name(QS))));

MP_UNITS_EXPORT [[nodiscard]] consteval auto get_canonical_unit(Unit auto u);

namespace detail {

template<auto U1, auto U2>
concept UnitsOfCompatibleQuantities =
  mp_units::utility::explicitly_convertible(get_quantity_spec(U1), get_quantity_spec(U2)) ||
  unsatisfied<"'{}' and '{}' units are of quantities of incompatible kinds ('{}' and '{}')">(
    U1, U2, type_name(get_quantity_spec(U1)._quantity_spec_), type_name(get_quantity_spec(U2)._quantity_spec_));

template<auto U1, auto U2>
concept ConvertibleUnits =
  (mp_units::get_canonical_unit(U1).reference_unit == mp_units::get_canonical_unit(U2).reference_unit) ||
  unsatisfied<
    "Units '{}' and '{}' are not convertible because they are defined in terms of "
    "different reference units ('{}' and '{}')">(U1, U2, mp_units::get_canonical_unit(U1).reference_unit,
                                                 mp_units::get_canonical_unit(U2).reference_unit);

template<typename U1, auto U2>
concept UnitConvertibleTo =
  Unit<U1> && Unit<MP_UNITS_NTTP_HACK(decltype(U2))> &&
  ((U1{} == U2) || ((!Unit<U1> || !Unit<MP_UNITS_NTTP_HACK(decltype(U2))> || UnitsOfCompatibleQuantities<U1{}, U2>) &&
                    ConvertibleUnits<U1{}, U2>));

template<typename T>
concept OffsetUnit = Unit<T> && requires { T::_point_origin_; };

}  // namespace detail

/**
 * A concept matching constants that are measured rather than exact by definition
 *
 * Satisfied by all constants defined with a `relative_standard_uncertainty` or a
 * `standard_uncertainty` argument. Exact constants don't have such an argument, so the
 * distinction is visible to the type system.
 *
 * @code{.cpp}
 * // a lab's calibrated sensor constant, with the uncertainty of its calibration
 * inline constexpr struct probe_sensitivity final :
 *   named_constant<"S_p", mag_ratio<41, 1000> * si::milli<si::volt> / si::kelvin,
 *                  standard_uncertainty{mag_ratio<2, 10000> * si::milli<si::volt> / si::kelvin}> {
 * } probe_sensitivity;
 *
 * // the standard uncertainty an input brings to an uncertainty budget, where an exact one
 * // brings none
 * constexpr Quantity auto standard_uncertainty_of(Unit auto constant) { return 0 * constant; }
 * constexpr Quantity auto standard_uncertainty_of(MeasuredConstant auto constant)
 * {
 *   return 1 * get_standard_uncertainty(constant);
 * }
 *
 * constexpr quantity of_probe = standard_uncertainty_of(probe_sensitivity);         // 200 nV/K
 * constexpr quantity of_c = standard_uncertainty_of(si::speed_of_light_in_vacuum);  // 0 c
 * @endcode
 *
 * @see [Concepts](docs:users_guide/advanced_topics/concepts#MeasuredConstant) in the User's Guide
 */
MP_UNITS_EXPORT template<typename T>
concept MeasuredConstant =
  Unit<T> && (requires { requires UnitMagnitude<decltype(T::_relative_standard_uncertainty_)>; } ||
              requires { requires Unit<decltype(T::_standard_uncertainty_)>; });

}  // namespace mp_units
