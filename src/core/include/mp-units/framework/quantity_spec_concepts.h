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
#include <mp-units/framework/dimension_concepts.h>
#include <mp-units/framework/expression_template.h>

namespace mp_units {

namespace detail {

struct quantity_spec_interface_base;

}

MP_UNITS_EXPORT template<typename T>
concept QuantitySpec = std::derived_from<T, detail::quantity_spec_interface_base> && std::is_final_v<T>;

MP_UNITS_EXPORT
#if MP_UNITS_API_NO_CRTP
template<auto...>
#else
template<typename, auto...>
#endif
struct quantity_spec;

template<typename Q>
struct kind_of_;

namespace detail {

template<typename T>
constexpr bool is_specialization_of_kind_of = false;

template<typename Q>
constexpr bool is_specialization_of_kind_of<kind_of_<Q>> = true;

template<typename T>
concept QuantityKindSpec = is_specialization_of_kind_of<T>;

#if MP_UNITS_API_NO_CRTP
template<auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<Args...>*);
#else
template<typename T, auto... Args>
void to_base_specialization_of_quantity_spec(const volatile quantity_spec<T, Args...>*);
#endif

template<typename T>
constexpr bool is_derived_from_specialization_of_quantity_spec =
  requires(T* type) { to_base_specialization_of_quantity_spec(type); };

/**
 * @brief Concept matching all named quantity specification types
 *
 * Satisfied by all types that derive from `quantity_spec`.
 */
template<typename T>
concept NamedQuantitySpec =
  QuantitySpec<T> && is_derived_from_specialization_of_quantity_spec<T> && (!QuantityKindSpec<T>);

template<typename T>
struct is_dimensionless : std::false_type {};

template<typename T>
concept IsPowerOfQuantitySpec = is_specialization_of_power<T> &&
                                (NamedQuantitySpec<typename T::factor> || is_dimensionless<typename T::factor>::value);


template<typename T>
constexpr bool is_per_of_quantity_specs = false;

template<typename... Ts>
constexpr bool is_per_of_quantity_specs<per<Ts...>> =
  (... && (NamedQuantitySpec<Ts> || is_dimensionless<Ts>::value || IsPowerOfQuantitySpec<Ts>));

template<typename T>
concept DerivedQuantitySpecExpr = detail::NamedQuantitySpec<T> || detail::is_dimensionless<T>::value ||
                                  detail::IsPowerOfQuantitySpec<T> || detail::is_per_of_quantity_specs<T>;

}  // namespace detail

template<detail::DerivedQuantitySpecExpr... Expr>
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
concept DerivedQuantitySpec =
  QuantitySpec<T> && (is_specialization_of<T, derived_quantity_spec> ||
                      (QuantityKindSpec<T> &&
                       is_specialization_of<std::remove_const_t<decltype(T::_quantity_spec_)>, derived_quantity_spec>));

}  // namespace detail


MP_UNITS_EXPORT template<QuantitySpec Q>
[[nodiscard]] consteval detail::QuantityKindSpec auto get_kind(Q q);

namespace detail {

template<auto QS1, auto QS2>
concept SameQuantitySpec = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS1))> &&
                           QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS2))> && (QS1 == QS2);

template<QuantitySpec Child, QuantitySpec Parent>
[[nodiscard]] consteval bool is_child_of(Child ch, Parent p);

template<auto Child, auto Parent>
concept ChildQuantitySpecOf = (is_child_of(Child, Parent));

template<auto To, auto From>
concept NestedQuantityKindSpecOf =
  QuantitySpec<decltype(From)> && QuantitySpec<decltype(To)> && (!SameQuantitySpec<get_kind(From), get_kind(To)>) &&
  ChildQuantitySpecOf<To, get_kind(From)._quantity_spec_>;

template<auto From, auto To>
concept QuantitySpecConvertibleTo =
  QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(From))> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(To))> &&
  implicitly_convertible(From, To);

template<auto From, auto To>
concept QuantitySpecExplicitlyConvertibleTo =
  QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(From))> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(To))> &&
  explicitly_convertible(From, To);

template<auto From, auto To>
concept QuantitySpecCastableTo = QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(From))> &&
                                 QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(To))> && castable(From, To);

}  // namespace detail

MP_UNITS_EXPORT template<typename T, auto QS>
concept QuantitySpecOf =
  QuantitySpec<T> && QuantitySpec<MP_UNITS_REMOVE_CONST(decltype(QS))> && detail::QuantitySpecConvertibleTo<T{}, QS> &&
  // the below is to make the following work
  // static_assert(ReferenceOf<si::radian, isq::angular_measure>);
  // static_assert(!ReferenceOf<si::radian, dimensionless>);
  // static_assert(!ReferenceOf<isq::angular_measure[si::radian], dimensionless>
  // static_assert(ReferenceOf<one, isq::angular_measure>);
  // static_assert(!ReferenceOf<dimensionless[one], isq::angular_measure>);
  !detail::NestedQuantityKindSpecOf<T{}, QS> &&
  (detail::QuantityKindSpec<T> || !detail::NestedQuantityKindSpecOf<QS, T{}>);

}  // namespace mp_units
