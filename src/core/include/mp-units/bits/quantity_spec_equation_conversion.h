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

// IWYU pragma: private, include <mp-units/framework/quantity_spec.h>
// The equation part of the conversion engine: quantity equation complexity, type explosion,
// and the ingredient-matching algorithm for derived quantities. This is what the C++ type
// system cannot express, so it stays in V3 (planned to be reimplemented in the value domain
// with C++26 reflection).
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/quantity_spec_hierarchy_conversion.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/framework/quantity_spec_concepts.h>
#include <mp-units/framework/quantity_spec_definitions.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string_view>
#include <utility>
#endif
#endif

namespace mp_units::detail {

/**
 * @brief @c get_complexity specifies how many type explosions can be done on a quantity
 */
template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q);

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval int get_complexity_impl(power<Q, Ints...>);

template<typename Q>
[[nodiscard]] consteval int get_complexity_impl(kind_of_<Q>);

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity_impl(Q);

template<typename... Ts>
[[nodiscard]] consteval int get_complexity_impl(type_list<Ts...>)
{
  if constexpr (sizeof...(Ts) == 0)
    return 0;
  else
    return detail::max({detail::get_complexity_impl(Ts{})...});
}

template<QuantitySpec Q, int... Ints>
[[nodiscard]] consteval int get_complexity_impl(power<Q, Ints...>)
{
  return detail::get_complexity(Q{});
}

template<typename Q>
[[nodiscard]] consteval int get_complexity_impl(kind_of_<Q>)
{
  return detail::get_complexity(Q{});
}

template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity_impl(Q)
{
  if constexpr (DerivedQuantitySpec<Q>)
    return detail::max(detail::get_complexity_impl(typename Q::_num_{}),
                       detail::get_complexity_impl(typename Q::_den_{}));
  else if constexpr (requires { Q::_equation_; } && Q{} != dimensionless)
    return 1 + detail::get_complexity(Q::_equation_);
  else
    return 0;
}

template<QuantitySpec Q>
constexpr auto get_complexity_result = detail::get_complexity_impl(Q{});

/**
 * @brief Get the complexity of a quantity spec
 *
 * Complexity of the quantity spec specifies how many times a given quantity can be exploded
 * to the underlying equation. For the derived quantities, the largest complexity of the
 * ingredients is returned.
 *
 * @tparam Q Quantity specification to be analyzed
 * @return int Complexity of the quantity spec
 */
template<QuantitySpec Q>
[[nodiscard]] consteval int get_complexity(Q)
{
  return get_complexity_result<Q>;
}

template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval bool defines_equation(Q)
{
  if constexpr (requires { Q::_parent_._equation_; })
    return Q::_parent_._equation_ != Q::_equation_;
  else
    return true;
}

template<QuantitySpec Q>
struct [[nodiscard]] explode_result {
  Q equation;
  specs_convertible_result result;
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<QuantitySpec Q>
explode_result(Q, specs_convertible_result) -> explode_result<Q>;

#endif


template<QuantitySpec Q>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode(Q)
{
  if constexpr (defines_equation(Q{}))
    return explode_result{Q::_equation_, specs_convertible_result::yes};
  else {
    constexpr bool q_is_kind = requires { defined_as_kind_impl(Q{}); } && defined_as_kind_impl(Q{});
    return explode_result{Q::_equation_, q_is_kind ? specs_convertible_result::explicit_conversion_beyond_kind
                                                   : specs_convertible_result::explicit_conversion};
  }
}

template<QuantitySpec Q, int... Ints>
  requires requires { Q::_equation_; }
[[nodiscard]] consteval auto explode(power<Q, Ints...>)
{
  constexpr ratio exp = power<Q, Ints...>::_exponent_;
  if constexpr (defines_equation(Q{}))
    return explode_result{pow<exp.num, exp.den>(Q::_equation_), specs_convertible_result::yes};
  else {
    constexpr bool q_is_kind = requires { defined_as_kind_impl(Q{}); } && defined_as_kind_impl(Q{});
    return explode_result{pow<exp.num, exp.den>(Q::_equation_),
                          q_is_kind ? specs_convertible_result::explicit_conversion_beyond_kind
                                    : specs_convertible_result::explicit_conversion};
  }
}

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] constexpr specs_convertible_result are_ingredients_convertible(NumFrom num_from, DenFrom den_from,
                                                                             NumTo num_to, DenTo den_to);

template<TypeList RestFrom = type_list<>, TypeList RestTo = type_list<>>
struct [[nodiscard]] extract_common_base_result {
  specs_convertible_result result;
  RestFrom rest_from{};
  RestTo rest_to{};
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

extract_common_base_result(specs_convertible_result) -> extract_common_base_result<type_list<>, type_list<>>;

template<TypeList RestFrom, TypeList RestTo>
extract_common_base_result(specs_convertible_result, RestFrom, RestTo) -> extract_common_base_result<RestFrom, RestTo>;

#endif

template<TypeList ProcessedFrom, TypeList RemainingFrom, TypeList ProcessedTo, TypeList RemainingTo>
[[nodiscard]] consteval auto try_extract_common_base(ProcessedFrom, RemainingFrom, ProcessedTo, RemainingTo)
{
  return std::optional<extract_common_base_result<>>(std::nullopt);
}

template<typename... ProcessedFrom, typename From, typename... RemainingFrom, typename... ProcessedTo, typename To,
         typename... RemainingTo>
[[nodiscard]] consteval auto try_extract_common_base(type_list<ProcessedFrom...>, type_list<From, RemainingFrom...>,
                                                     type_list<ProcessedTo...>, type_list<To, RemainingTo...>)
{
  constexpr QuantitySpec auto from_factor = detail::get_factor(From{});
  constexpr QuantitySpec auto to_factor = detail::get_factor(To{});
  constexpr QuantitySpec auto from_root = get_hierarchy_root(from_factor);
  constexpr QuantitySpec auto to_root = get_hierarchy_root(to_factor);
  constexpr std::string_view from_root_name = detail::type_name<MP_UNITS_NONCONST_TYPE(from_root)>();
  constexpr std::string_view to_root_name = detail::type_name<MP_UNITS_NONCONST_TYPE(to_root)>();

  // Type lists are sorted by the root name. This allows us to easily progress through both lists to find the quantities
  // belonging to the same hierarchy tree.
  if constexpr (from_root_name < to_root_name)
    return detail::try_extract_common_base(type_list<ProcessedFrom..., From>{}, type_list<RemainingFrom...>{},
                                           type_list<ProcessedTo...>{}, type_list<To, RemainingTo...>{});
  else if constexpr (from_root_name > to_root_name)
    return detail::try_extract_common_base(type_list<ProcessedFrom...>{}, type_list<From, RemainingFrom...>{},
                                           type_list<ProcessedTo..., To>{}, type_list<RemainingTo...>{});
  else {
    // We are dealing with quantities from the same hierarchy tree.
    constexpr specs_convertible_result res = detail::convertible_common_base(from_factor, to_factor);
    if constexpr (res == specs_convertible_result::no)
      // We do not need to analyze the rest of the lists as the quantities are not convertible when at least one element
      // is not convertible.
      return std::optional(extract_common_base_result{specs_convertible_result::no});
    else if constexpr (res == specs_convertible_result::cast &&
                       (detail::get_complexity(from_factor) > 0 || detail::get_complexity(to_factor) > 0) &&
                       (detail::get_complexity_impl(type_list<ProcessedFrom..., RemainingFrom...>{}) > 0 ||
                        detail::get_complexity_impl(type_list<ProcessedTo..., RemainingTo...>{}) > 0))
      // One or both factors have a defining equation and remaining elements have complexity — defer to
      // the explosion path so higher-complexity elements can be tried first.
      return std::optional<extract_common_base_result<>>(std::nullopt);
    else {
      // We might deal with different powers of quantities. Let's find the biggest common exponent and extract it.
      // The rest of the quantities must be reinserted into the list.
      constexpr ratio from_exp = detail::get_exponent(From{});
      constexpr ratio to_exp = detail::get_exponent(To{});
      if constexpr (from_exp == to_exp)
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., RemainingFrom...>{},
                                                        type_list<ProcessedTo..., RemainingTo...>{}});
      else if constexpr (from_exp > to_exp) {
        using rest = power_or_T<MP_UNITS_NONCONST_TYPE(from_factor), from_exp - to_exp>;
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., rest, RemainingFrom...>{},
                                                        type_list<ProcessedTo..., RemainingTo...>{}});
      } else {
        using rest = power_or_T<MP_UNITS_NONCONST_TYPE(to_factor), to_exp - from_exp>;
        return std::optional(extract_common_base_result{res, type_list<ProcessedFrom..., RemainingFrom...>{},
                                                        type_list<ProcessedTo..., rest, RemainingTo...>{}});
      }
    }
  }
}

template<TypeList From, TypeList To>
[[nodiscard]] consteval auto try_extract_common_base(From from, To to)
{
  return detail::try_extract_common_base(type_list<>{}, from, type_list<>{}, to);
}

struct get_max_complexity_result_base {
  int complexity;
  int index;
};

template<typename... Elems>
[[nodiscard]] consteval get_max_complexity_result_base get_max_complexity(type_list<Elems...>)
{
  if constexpr (sizeof...(Elems) == 0)
    return {-1, -1};
  else {
    const std::initializer_list<int> list{detail::get_complexity_impl(Elems{})...};
    const auto it = detail::max_element(list.begin(), list.end());
    return {*it, static_cast<int>(it - list.begin())};
  }
}

enum class ingredient_type : std::int8_t { numerator_from, denominator_from, numerator_to, denominator_to };

struct get_max_complexity_result : get_max_complexity_result_base {
  ingredient_type type;
};

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] consteval get_max_complexity_result get_max_complexity(NumFrom num_from, DenFrom den_from, NumTo num_to,
                                                                     DenTo den_to)
{
  const std::initializer_list<get_max_complexity_result_base> list{
    detail::get_max_complexity(num_from), detail::get_max_complexity(den_from), detail::get_max_complexity(num_to),
    detail::get_max_complexity(den_to)};
  const auto it =
    detail::max_element(list.begin(), list.end(), [](auto lhs, auto rhs) { return lhs.complexity < rhs.complexity; });
  return {*it, static_cast<ingredient_type>(it - list.begin())};
}

template<QuantitySpec Equation, TypeList Num, TypeList Den>
[[nodiscard]] consteval auto merge_with_equation(Equation, Num, Den)
{
  if constexpr (Equation{} == dimensionless)
    return std::pair(Num{}, Den{});
  else {
    constexpr QuantitySpec auto merged_qs = [&]() {
      if constexpr (DerivedQuantitySpec<Equation>)
        return detail::get_optimized_expression<
          type_list_merge_sorted<typename Equation::_num_, Num, type_list_of_quantity_spec_less>,
          type_list_merge_sorted<typename Equation::_den_, Den, type_list_of_quantity_spec_less>, struct dimensionless,
          type_list_of_quantity_spec_less, derived_quantity_spec>();
      else
        return detail::get_optimized_expression<
          type_list_merge_sorted<Num, type_list<Equation>, type_list_of_quantity_spec_less>, Den, struct dimensionless,
          type_list_of_quantity_spec_less, derived_quantity_spec>();
    }();
    using qs_type = MP_UNITS_NONCONST_TYPE(merged_qs);
    if constexpr (DerivedQuantitySpec<qs_type>)
      return std::pair(typename qs_type::_num_{}, typename qs_type::_den_{});
    else
      return std::pair(type_list<qs_type>{}, type_list<>{});
  }
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result convertible(From, To);

template<TypeList NumFrom, TypeList DenFrom, TypeList NumTo, TypeList DenTo>
[[nodiscard]] constexpr specs_convertible_result are_ingredients_convertible(NumFrom num_from, DenFrom den_from,
                                                                             NumTo num_to, DenTo den_to)
{
  constexpr std::size_t num_from_size = type_list_size<NumFrom>;
  constexpr std::size_t den_from_size = type_list_size<DenFrom>;
  constexpr std::size_t num_to_size = type_list_size<NumTo>;
  constexpr std::size_t den_to_size = type_list_size<DenTo>;

  // if at least one of the sides is empty then we compare with dimensionless
  if constexpr (num_from_size == den_from_size && num_to_size + den_to_size == 0)
    return specs_convertible_result::yes;
  else if constexpr (num_from_size + den_from_size == 0 && num_to_size == den_to_size && num_to_size >= 1)
    return specs_convertible_result::explicit_conversion;
  else {
    // otherwise, check if the pairwise ingredients are convertible
    if constexpr (constexpr auto nums = detail::try_extract_common_base(NumFrom{}, NumTo{})) {
      if (nums->result == specs_convertible_result::no) return specs_convertible_result::no;
      return detail::min(nums->result,
                         detail::are_ingredients_convertible(nums->rest_from, den_from, nums->rest_to, den_to));
    } else if constexpr (constexpr auto dens = detail::try_extract_common_base(DenFrom{}, DenTo{})) {
      if (dens->result == specs_convertible_result::no) return specs_convertible_result::no;
      return detail::min(dens->result,
                         detail::are_ingredients_convertible(num_from, dens->rest_from, num_to, dens->rest_to));
    } else {
      // otherwise, get the ingredient with the highest complexity
      constexpr auto max_compl_res = detail::get_max_complexity(NumFrom{}, DenFrom{}, NumTo{}, DenTo{});

      constexpr auto list = [&] {
        if constexpr (max_compl_res.type == ingredient_type::numerator_from)
          return NumFrom{};
        else if constexpr (max_compl_res.type == ingredient_type::denominator_from)
          return DenFrom{};
        else if constexpr (max_compl_res.type == ingredient_type::numerator_to)
          return NumTo{};
        else
          return DenTo{};
      }();
      using extracted = type_list_extract<MP_UNITS_NONCONST_TYPE(list), static_cast<std::size_t>(max_compl_res.index)>;

      // before exploding check if the ingredient is convertible with dimensionless
      if constexpr (max_compl_res.type == ingredient_type::numerator_from && num_to_size + den_to_size == 0) {
        constexpr specs_convertible_result res =
          detail::convertible(detail::get_factor(typename extracted::element{}), dimensionless);
        if constexpr (res != specs_convertible_result::no)
          return detail::min(res,
                             detail::are_ingredients_convertible(typename extracted::rest{}, den_from, num_to, den_to));
      } else if constexpr (max_compl_res.type == ingredient_type::numerator_to && num_from_size + den_from_size == 0) {
        constexpr specs_convertible_result res =
          detail::convertible(dimensionless, detail::get_factor(typename extracted::element{}));
        if constexpr (res != specs_convertible_result::no)
          return detail::min(
            res, detail::are_ingredients_convertible(num_from, den_from, typename extracted::rest{}, den_to));
      }

      if constexpr (max_compl_res.complexity > 0) {
        // explode the ingredient to the underlying equation
        constexpr auto explode_res = detail::explode(typename extracted::element{});
        // kind-boundary cost is asymmetric: the FROM side pays explicit_conversion_beyond_kind,
        // the TO side pays at most explicit_conversion (you are being asked to go INTO a kind).
        constexpr auto to_side_result = explode_res.result == specs_convertible_result::explicit_conversion_beyond_kind
                                          ? specs_convertible_result::explicit_conversion
                                          : explode_res.result;
        if constexpr (max_compl_res.type == ingredient_type::numerator_from) {
          const auto [num, den] =
            detail::merge_with_equation(explode_res.equation, typename extracted::rest{}, den_from);
          return detail::are_ingredients_convertible(num, den, num_to, den_to);
        } else if constexpr (max_compl_res.type == ingredient_type::denominator_from) {
          const auto [num, den] =
            detail::merge_with_equation(dimensionless / explode_res.equation, num_from, typename extracted::rest{});
          return detail::min(explode_res.result, detail::are_ingredients_convertible(num, den, num_to, den_to));
        } else if constexpr (max_compl_res.type == ingredient_type::numerator_to) {
          const auto [num, den] = detail::merge_with_equation(explode_res.equation, typename extracted::rest{}, den_to);
          return detail::min(to_side_result, detail::are_ingredients_convertible(num_from, den_from, num, den));
        } else {
          const auto [num, den] =
            detail::merge_with_equation(dimensionless / explode_res.equation, num_to, typename extracted::rest{});
          return detail::min(to_side_result, detail::are_ingredients_convertible(num_from, den_from, num, den));
        }
      } else
        // this should never happen
        return specs_convertible_result::no;
    }
  }
}

template<QuantitySpec From, QuantitySpec To>
[[nodiscard]] consteval specs_convertible_result are_ingredients_convertible(From, To)
{
  if constexpr (DerivedQuantitySpec<From> && DerivedQuantitySpec<To>)
    return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, typename To::_num_{},
                                               typename To::_den_{});
  else if constexpr (DerivedQuantitySpec<From> && NamedQuantitySpec<To>) {
    if constexpr (To{} == dimensionless)
      return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, type_list<>{},
                                                 type_list<>{});
    // If `To` defines its own equation, try matching the derived `From` against that equation directly.
    // This ensures that a derived expression equivalent to the named quantity's defining equation is
    // convertible to it at the correct level, rather than being prematurely matched via a parent-child
    // common base (which would yield only `explicit_conversion`).
    else if constexpr (requires { To::_equation_; }) {
      if constexpr (detail::defines_equation(To{})) {
        constexpr specs_convertible_result eq_res = detail::convertible(From{}, To::_equation_);
        if constexpr (eq_res != specs_convertible_result::no) return eq_res;
      }
      return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, type_list<To>{},
                                                 type_list<>{});
    } else
      return detail::are_ingredients_convertible(typename From::_num_{}, typename From::_den_{}, type_list<To>{},
                                                 type_list<>{});
  } else {
    if constexpr (From{} == dimensionless)
      return detail::are_ingredients_convertible(type_list<>{}, type_list<>{}, typename To::_num_{},
                                                 typename To::_den_{});
    // Symmetric case: if `From` defines its own equation, try matching that equation against `To` directly.
    else if constexpr (requires { From::_equation_; }) {
      if constexpr (detail::defines_equation(From{})) {
        constexpr specs_convertible_result eq_res = detail::convertible(From::_equation_, To{});
        if constexpr (eq_res != specs_convertible_result::no) return eq_res;
      }
      return detail::are_ingredients_convertible(type_list<From>{}, type_list<>{}, typename To::_num_{},
                                                 typename To::_den_{});
    } else
      return detail::are_ingredients_convertible(type_list<From>{}, type_list<>{}, typename To::_num_{},
                                                 typename To::_den_{});
  }
}

}  // namespace mp_units::detail
