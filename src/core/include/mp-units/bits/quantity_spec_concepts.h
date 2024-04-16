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

#include <mp-units/bits/dimension_concepts.h>
#include <mp-units/bits/expression_template.h>
#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/module_macros.h>

namespace mp_units {

MP_UNITS_EXPORT
#ifdef MP_UNITS_API_NO_CRTP
template<auto...>
#else
template<typename, auto...>
#endif
struct quantity_spec;

template<typename Q>
struct kind_of_;

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_kind_of = false;

template<typename Q>
inline constexpr bool is_specialization_of_kind_of<kind_of_<Q>> = true;

template<typename T>
concept QuantityKindSpec = is_specialization_of_kind_of<T>;

#ifdef MP_UNITS_API_NO_CRTP
template<auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<Args...>*);
#else
template<typename T, auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<T, Args...>*);
#endif

template<typename T>
inline constexpr bool is_derived_from_specialization_of_quantity_spec =
  requires(T* t) { to_base_specialization_of_quantity_spec(t); };

template<typename T>
inline constexpr bool is_specialization_of_quantity_spec = false;

#ifdef MP_UNITS_API_NO_CRTP
template<auto... Args>
inline constexpr bool is_specialization_of_quantity_spec<quantity_spec<Args...>> = true;
#else
template<typename T, auto... Args>
inline constexpr bool is_specialization_of_quantity_spec<quantity_spec<T, Args...>> = true;
#endif

/**
 * @brief Concept matching all named quantity specification types
 *
 * Satisfied by all types that derive from `quantity_spec`.
 */
template<typename T>
concept NamedQuantitySpec = is_derived_from_specialization_of_quantity_spec<T> &&
                            (!is_specialization_of_quantity_spec<T>)&&(!QuantityKindSpec<T>);

template<typename T>
struct is_dimensionless : std::false_type {};

template<typename T>
inline constexpr bool is_power_of_quantity_spec = requires {
  requires is_specialization_of_power<T> &&
             (NamedQuantitySpec<typename T::factor> || is_dimensionless<typename T::factor>::value);
};

template<typename T>
inline constexpr bool is_per_of_quantity_specs = false;

template<typename... Ts>
inline constexpr bool is_per_of_quantity_specs<per<Ts...>> =
  (... && (NamedQuantitySpec<Ts> || is_dimensionless<Ts>::value || is_power_of_quantity_spec<Ts>));

template<typename T>
concept IntermediateDerivedQuantitySpecExpr =
  detail::NamedQuantitySpec<T> || detail::is_dimensionless<T>::value || detail::is_power_of_quantity_spec<T> ||
  detail::is_per_of_quantity_specs<T>;

}  // namespace detail

template<detail::IntermediateDerivedQuantitySpecExpr... Expr>
struct derived_quantity_spec;

namespace detail {

/**
 * @brief Concept matching all derived quantity specification types
 *
 * Satisfied by all `derived_quantity_spec` specializations.
 *
 * @note Deriving a strong type from it is considered a logic error and thus is
 * explicitly not supported here.
 */
template<typename T>
concept IntermediateDerivedQuantitySpec =
  is_specialization_of<T, derived_quantity_spec> ||
  (QuantityKindSpec<T> &&
   is_specialization_of<std::remove_const_t<decltype(T::_quantity_spec_)>, derived_quantity_spec>);

}  // namespace detail


MP_UNITS_EXPORT template<typename T>
concept QuantitySpec =
  detail::NamedQuantitySpec<T> || detail::IntermediateDerivedQuantitySpec<T> || detail::QuantityKindSpec<T>;

MP_UNITS_EXPORT template<QuantitySpec Q>
[[nodiscard]] consteval detail::QuantityKindSpec auto get_kind(Q q);

namespace detail {

template<auto To, auto From>
concept NestedQuantityKindSpecOf =
  QuantitySpec<std::remove_const_t<decltype(From)>> && QuantitySpec<std::remove_const_t<decltype(To)>> &&
  get_kind(From) != get_kind(To) &&
  std::derived_from<std::remove_cvref_t<decltype(To)>, std::remove_cvref_t<decltype(get_kind(From)._quantity_spec_)>>;

}

MP_UNITS_EXPORT template<typename T, auto QS>
concept QuantitySpecOf =
  QuantitySpec<T> && QuantitySpec<std::remove_const_t<decltype(QS)>> && implicitly_convertible(T{}, QS) &&
  // the below is to make the following work
  // static_assert(ReferenceOf<si::radian, isq::angular_measure>);
  // static_assert(!ReferenceOf<si::radian, dimensionless>);
  // static_assert(!ReferenceOf<isq::angular_measure[si::radian], dimensionless>
  // static_assert(ReferenceOf<one, isq::angular_measure>);
  // static_assert(!ReferenceOf<dimensionless[one], isq::angular_measure>);
  !detail::NestedQuantityKindSpecOf<T{}, QS> &&
  (detail::QuantityKindSpec<T> || !detail::NestedQuantityKindSpecOf<QS, T{}>);

}  // namespace mp_units
