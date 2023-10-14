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

#include <mp-units/bits/external/math_concepts.h>
#include <mp-units/bits/external/type_list.h>
#include <mp-units/bits/external/type_traits.h>
#include <mp-units/bits/ratio.h>

namespace mp_units {

/**
 * @brief Type list type used by the expression template framework
 *
 * @tparam Ts The list of types
 */
template<typename... Ts>
struct type_list {};

/**
 * @brief Type list type storing the list of components with negative exponents
 *
 * @note Can't be empty
 */
template<typename T, typename... Ts>
struct per {};

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_per = false;

template<typename... Ts>
inline constexpr bool is_specialization_of_per<per<Ts...>> = true;

template<int Num, int... Den>
inline constexpr bool valid_ratio = true;

template<int... Den>
inline constexpr bool valid_ratio<0, Den...> = false;

template<int Num>
inline constexpr bool valid_ratio<Num, 0> = false;

template<>
inline constexpr bool valid_ratio<0, 0> = false;

template<int Num, int... Den>
inline constexpr bool positive_ratio = gt_zero<Num>;

template<int Num, int Den>
inline constexpr bool positive_ratio<Num, Den> = gt_zero<Num * Den>;

template<int Num, int... Den>
inline constexpr bool ratio_one = false;

template<>
inline constexpr bool ratio_one<1> = true;

template<int N>
inline constexpr bool ratio_one<N, N> = true;

}  // namespace detail


/**
 * @brief Type container for exponents with ratio different than `1`
 *
 * Ratio must be mathematically valid and non-negative. Negative exponents are passed to
 * `per<...>` with inverted sign (as positive exponents).
 *
 * @tparam F factor to be raised to specified power
 * @tparam Num Power ratio numerator
 * @tparam Den [optional] Power ration denominator
 *
 * @note @p Den is an optional parameter to shorten the types presented to the user in the case when  @p Den equals `1`.
 */
template<typename F, int Num, int... Den>
  requires(detail::valid_ratio<Num, Den...> && detail::positive_ratio<Num, Den...> && !detail::ratio_one<Num, Den...>)
struct power {
  using factor = F;
  static constexpr ratio exponent{Num, Den...};
};

namespace detail {

template<typename T>
struct expr_type_impl : std::type_identity<T> {};

template<typename T, int... Ints>
struct expr_type_impl<power<T, Ints...>> : std::type_identity<T> {};

}  // namespace detail

template<typename T>
using expr_type = MP_UNITS_TYPENAME detail::expr_type_impl<T>::type;

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_power = false;

template<typename F, int... Ints>
inline constexpr bool is_specialization_of_power<power<F, Ints...>> = true;

template<typename T, ratio R>
consteval auto power_or_T_impl()
{
  if constexpr (is_specialization_of_power<T>) {
    return power_or_T_impl<typename T::factor, T::exponent * R>();
  } else {
    if constexpr (R.den == 1) {
      if constexpr (R.num == 1)
        return T{};
      else
        return power<T, R.num>{};
    } else {
      return power<T, R.num, R.den>{};
    }
  }
}

template<typename T, auto R>
// template<typename T, ratio R>  // TODO ICE gcc 12
using power_or_T = decltype(power_or_T_impl<T, R>());


/**
 * @brief Consolidates contiguous ranges of exponents of the same type
 *
 * If there is more than one exponent with the same type they are aggregated into one type by adding
 * their powers.
 */
template<typename List>
struct expr_consolidate_impl;

template<>
struct expr_consolidate_impl<type_list<>> {
  using type = type_list<>;
};

template<typename T>
struct expr_consolidate_impl<type_list<T>> {
  using type = type_list<T>;
};

template<typename T, typename... Rest>
struct expr_consolidate_impl<type_list<T, Rest...>> {
  using type = type_list_push_front<typename expr_consolidate_impl<type_list<Rest...>>::type, T>;
};

// replaces two instances of a type with one having the power of `2`
template<typename T, typename... Rest>
  requires(!is_specialization_of_power<T>)
struct expr_consolidate_impl<type_list<T, T, Rest...>> {
  using type = MP_UNITS_TYPENAME expr_consolidate_impl<type_list<power<T, 2>, Rest...>>::type;
};

// replaces the instance of a type and a power of it with one with incremented power
template<typename T, int... Ints, typename... Rest>
struct expr_consolidate_impl<type_list<T, power<T, Ints...>, Rest...>> {
  using type =
    MP_UNITS_TYPENAME expr_consolidate_impl<type_list<power_or_T<T, power<T, Ints...>::exponent + 1>, Rest...>>::type;
};

// accumulates the powers of instances of the same type (removes the element in case the accumulation result is `0`)
template<typename T, int... Ints1, int... Ints2, typename... Rest>
struct expr_consolidate_impl<type_list<power<T, Ints1...>, power<T, Ints2...>, Rest...>> {
  static constexpr ratio r = power<T, Ints1...>::exponent + power<T, Ints2...>::exponent;
  using type = MP_UNITS_TYPENAME expr_consolidate_impl<type_list<power_or_T<T, r>, Rest...>>::type;
};

template<typename List>
using expr_consolidate = MP_UNITS_TYPENAME expr_consolidate_impl<List>::type;


/**
 * @brief Simplifies the expression template
 *
 * Analyzes provided numerator and denominator type lists and simplifies elements with the same type.
 * If the same power exists in both list, this elements gets omitted. Otherwise, the power of its
 * exponent gets subtracted according to numerator and denominator elements powers.
 *
 * @tparam NumList type list for expression numerator
 * @tparam DenList type list for expression denominator
 * @tparam Pred predicate to be used for elements comparisons
 */
template<typename NumList, typename DenList, template<typename, typename> typename Pred>
struct expr_simplify;

// when one of the lists is empty there is nothing to do
template<typename NumList, typename DenList, template<typename, typename> typename Pred>
  requires(type_list_size<NumList> == 0) || (type_list_size<DenList> == 0)
struct expr_simplify<NumList, DenList, Pred> {
  using num = NumList;
  using den = DenList;
};

// in case when front elements are different progress to the next element
template<typename Num, typename... NRest, typename Den, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<Num, NRest...>, type_list<Den, DRest...>, Pred> {
  using impl = conditional<Pred<Num, Den>::value, expr_simplify<type_list<NRest...>, type_list<Den, DRest...>, Pred>,
                           expr_simplify<type_list<Num, NRest...>, type_list<DRest...>, Pred>>;
  using num = conditional<Pred<Num, Den>::value, type_list_push_front<typename impl::num, Num>, typename impl::num>;
  using den = conditional<Pred<Num, Den>::value, typename impl::den, type_list_push_front<typename impl::den, Den>>;
};

// in case two elements are of the same power such element gets omitted
template<typename T, typename... NRest, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<T, NRest...>, type_list<T, DRest...>, Pred> :
    expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred> {};

template<typename T, ratio Num, ratio Den>
struct expr_simplify_power {
  static constexpr ratio r = Num - Den;
  using type = power_or_T<T, ratio{abs(r.num), r.den}>;
  using num = conditional<(r > 0), type_list<type>, type_list<>>;
  using den = conditional<(r < 0), type_list<type>, type_list<>>;
};

// in case there are different powers for the same element simplify the power
template<typename T, typename... NRest, int... Ints, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<power<T, Ints...>, NRest...>, type_list<T, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, power<T, Ints...>::exponent, ratio{1}>;
  using num = type_list_join<typename type::num, typename impl::num>;
  using den = type_list_join<typename type::den, typename impl::den>;
};

// in case there are different powers for the same element simplify the power
template<typename T, typename... NRest, typename... DRest, int... Ints, template<typename, typename> typename Pred>
struct expr_simplify<type_list<T, NRest...>, type_list<power<T, Ints...>, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, ratio{1}, power<T, Ints...>::exponent>;
  using num = type_list_join<typename type::num, typename impl::num>;
  using den = type_list_join<typename type::den, typename impl::den>;
};

// in case there are different powers for the same element simplify the power
template<typename T, typename... NRest, int... Ints1, typename... DRest, int... Ints2,
         template<typename, typename> typename Pred>
  requires(!std::same_as<power<T, Ints1...>, power<T, Ints2...>>)
struct expr_simplify<type_list<power<T, Ints1...>, NRest...>, type_list<power<T, Ints2...>, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, power<T, Ints1...>::exponent, power<T, Ints2...>::exponent>;
  using num = type_list_join<typename type::num, typename impl::num>;
  using den = type_list_join<typename type::den, typename impl::den>;
};


// expr_less
template<typename Lhs, typename Rhs, template<typename, typename, auto...> typename Pred>
struct expr_less_impl : Pred<expr_type<Lhs>, expr_type<Rhs>> {};

template<typename T, int... Ints, template<typename, typename, auto...> typename Pred>
struct expr_less_impl<T, power<T, Ints...>, Pred> : std::true_type {};

/**
 * @brief Compares two types with a given predicate
 *
 * Algorithm accounts not only for explicit types but also for the case when they
 * are wrapped within `power<T, Num, Den>`.
 */
template<typename Lhs, typename Rhs, template<typename, typename, auto...> typename Pred>
using expr_less = expr_less_impl<Lhs, Rhs, Pred>;


// expr_fractions
template<typename Num = type_list<>, typename Den = type_list<>>
struct expr_fractions_result {
  using _num_ = Num;  // exposition only
  using _den_ = Den;  // exposition only
};

template<template<typename> typename OneType, typename List>
[[nodiscard]] consteval auto expr_fractions_impl()
{
  constexpr std::size_t size = type_list_size<List>;

  if constexpr (size == 0)
    return expr_fractions_result<>{};
  else if constexpr (size == 1)
    return expr_fractions_result<List>{};
  else {
    using last_element = type_list_back<List>;

    if constexpr (is_specialization_of_per<last_element>) {
      if constexpr (size == 2 && OneType<type_list_front<List>>::value)
        return expr_fractions_result<type_list<>, type_list_map<last_element, type_list>>{};
      else {
        using split = type_list_split<List, size - 1>;
        return expr_fractions_result<typename split::first_list, type_list_map<last_element, type_list>>{};
      }
    } else {
      return expr_fractions_result<List>{};
    }
  }
}

/**
 * @brief Divides expression template spec to numerator and denominator parts
 */
template<template<typename> typename OneType, typename... Ts>
struct expr_fractions : decltype(expr_fractions_impl<OneType, type_list<Ts...>>()) {};

// expr_make_spec
template<typename NumList, typename DenList, typename OneType, template<typename...> typename To>
[[nodiscard]] consteval auto expr_make_spec_impl()
{
  constexpr std::size_t num = type_list_size<NumList>;
  constexpr std::size_t den = type_list_size<DenList>;

  if constexpr (num == 0 && den == 0) {
    return OneType{};
  } else if constexpr (num > 0 && den > 0) {
    return type_list_map<type_list_push_back<NumList, type_list_map<DenList, per>>, To>{};
  } else if constexpr (den > 0) {
    return To<OneType, type_list_map<DenList, per>>{};
  } else {
    if constexpr (num == 1 && !is_specialization_of_power<type_list_front<NumList>>)
      // temporary derived type not needed -> just return the original one
      return type_list_front<NumList>{};
    else
      return type_list_map<NumList, To>{};
  }
}

/**
 * @brief Creates an expression template spec based on provided numerator and denominator parts
 */
template<typename NumList, typename DenList, typename OneType, template<typename, typename> typename Pred,
         template<typename...> typename To>
[[nodiscard]] consteval auto get_optimized_expression()
{
  using num_list = expr_consolidate<NumList>;
  using den_list = expr_consolidate<DenList>;
  using simple = expr_simplify<num_list, den_list, Pred>;
  // the usage of `std::identity` below helps to resolve an using alias identifier to the actual
  // type identifier in the clang compile-time errors
  return std::identity{}(expr_make_spec_impl<typename simple::num, typename simple::den, OneType, To>());
}

/**
 * @brief Multiplies two sorted expression template specs
 *
 * @tparam To destination type list to put the result to
 * @tparam OneType type that represents the value `1`
 * @tparam Pred binary less then predicate
 * @tparam Lhs lhs of the operation
 * @tparam Rhs rhs of the operation
 */
template<template<typename...> typename To, typename OneType, template<typename, typename> typename Pred, typename Lhs,
         typename Rhs>
[[nodiscard]] consteval auto expr_multiply(Lhs, Rhs)
{
  if constexpr (is_same_v<Lhs, OneType>) {
    return Rhs{};
  } else if constexpr (is_same_v<Rhs, OneType>) {
    return Lhs{};
  } else if constexpr (is_specialization_of<Lhs, To> && is_specialization_of<Rhs, To>) {
    // two derived dimensions
    return get_optimized_expression<type_list_merge_sorted<typename Lhs::_num_, typename Rhs::_num_, Pred>,
                                    type_list_merge_sorted<typename Lhs::_den_, typename Rhs::_den_, Pred>, OneType,
                                    Pred, To>();
  } else if constexpr (is_specialization_of<Lhs, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename Lhs::_num_, type_list<Rhs>, Pred>,
                                    typename Lhs::_den_, OneType, Pred, To>();
  } else if constexpr (is_specialization_of<Rhs, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename Rhs::_num_, type_list<Lhs>, Pred>,
                                    typename Rhs::_den_, OneType, Pred, To>();
  } else {
    // two base dimensions
    return get_optimized_expression<type_list_merge_sorted<type_list<Lhs>, type_list<Rhs>, Pred>, type_list<>, OneType,
                                    Pred, To>();
  }
}

/**
 * @brief Divides two sorted expression template specs
 *
 * @tparam To destination type list to put the result to
 * @tparam OneType type that represents the value `1`
 * @tparam Pred binary less then predicate
 * @tparam Lhs lhs of the operation
 * @tparam Rhs rhs of the operation
 */
template<template<typename...> typename To, typename OneType, template<typename, typename> typename Pred, typename Lhs,
         typename Rhs>
[[nodiscard]] consteval auto expr_divide(Lhs lhs, Rhs rhs)
{
  if constexpr (is_same_v<Lhs, Rhs>) {
    return OneType{};
  } else if constexpr (is_same_v<Rhs, OneType>) {
    return lhs;
  } else if constexpr (is_same_v<Lhs, OneType>) {
    return expr_divide<To, OneType, Pred>(To<>{}, rhs);
  } else if constexpr (is_specialization_of<Lhs, To> && is_specialization_of<Rhs, To>) {
    // two derived entities
    return get_optimized_expression<type_list_merge_sorted<typename Lhs::_num_, typename Rhs::_den_, Pred>,
                                    type_list_merge_sorted<typename Lhs::_den_, typename Rhs::_num_, Pred>, OneType,
                                    Pred, To>();
  } else if constexpr (is_specialization_of<Lhs, To>) {
    return get_optimized_expression<
      typename Lhs::_num_, type_list_merge_sorted<typename Lhs::_den_, type_list<Rhs>, Pred>, OneType, Pred, To>();
  } else if constexpr (is_specialization_of<Rhs, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename Rhs::_den_, type_list<Lhs>, Pred>,
                                    typename Rhs::_num_, OneType, Pred, To>();
  } else {
    // two named entities
    return To<Lhs, per<Rhs>>{};
  }
}

/**
 * @brief Inverts the expression template spec
 *
 * @tparam T expression template spec to invert
 * @tparam OneType type that represents the value `1`
 * @tparam To destination type list to put the result to
 */
template<template<typename...> typename To, typename OneType, typename T>
[[nodiscard]] consteval auto expr_invert(T)
{
  if constexpr (is_specialization_of<T, To>)
    // the usage of `std::identity` below helps to resolve an using alias identifier to the actual
    // type identifier in the clang compile-time errors
    return std::identity{}(expr_make_spec_impl<typename T::_den_, typename T::_num_, OneType, To>());
  else
    return To<OneType, per<T>>{};
}


template<std::intmax_t Num, std::intmax_t Den, template<typename...> typename To, typename OneType,
         template<typename, typename> typename Pred, typename... Nums, typename... Dens>
  requires detail::non_zero<Den>
[[nodiscard]] consteval auto expr_pow_impl(type_list<Nums...>, type_list<Dens...>)
{
  return detail::get_optimized_expression<type_list<power_or_T<Nums, ratio{Num, Den}>...>,
                                          type_list<power_or_T<Dens, ratio{Num, Den}>...>, OneType, Pred, To>();
}


/**
 * @brief Computes the value of an expression raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @tparam To destination type list to put the result to
 * @tparam OneType type that represents the value `1`
 * @tparam Pred binary less then predicate
 * @tparam T Expression being the base of the operation
 */
template<std::intmax_t Num, std::intmax_t Den, template<typename...> typename To, typename OneType,
         template<typename, typename> typename Pred, typename T>
  requires detail::non_zero<Den>
[[nodiscard]] consteval auto expr_pow(T)
{
  return expr_pow_impl<Num, Den, To, OneType, Pred>(typename T::_num_{}, typename T::_den_{});
}


// expr_map
template<typename T, template<typename> typename Proj>
struct expr_type_map;

template<typename T, template<typename> typename Proj>
  requires requires { typename Proj<T>; }
struct expr_type_map<T, Proj> {
  using type = Proj<T>;
};

template<typename F, int... Ints, template<typename> typename Proj>
  requires requires { typename Proj<F>; }
struct expr_type_map<power<F, Ints...>, Proj> {
  using type = power<Proj<F>, Ints...>;
};

template<typename T, template<typename> typename Proj>
concept expr_type_projectable = (requires { typename Proj<T>; } ||
                                 (is_specialization_of_power<T> && requires { typename Proj<typename T::factor>; }));

template<typename T, template<typename> typename Proj>
inline constexpr bool expr_projectable_impl = false;

template<typename... Ts, template<typename> typename Proj>
inline constexpr bool expr_projectable_impl<type_list<Ts...>, Proj> = (... && expr_type_projectable<Ts, Proj>);

template<typename T, template<typename> typename Proj>
concept expr_projectable = requires {
  typename T::_num_;
  typename T::_den_;
  requires expr_projectable_impl<typename T::_num_, Proj>;
  requires expr_projectable_impl<typename T::_den_, Proj>;
};

template<typename T>
[[nodiscard]] consteval auto map_power(T t)
{
  return t;
}

template<typename T, auto... Ints>
[[nodiscard]] consteval auto map_power(power<T, Ints...>)
{
  return pow<Ints...>(T{});
}

template<template<typename> typename Proj, template<typename...> typename To, typename OneType,
         template<typename, typename> typename Pred, expr_type_projectable<Proj>... Nums,
         expr_type_projectable<Proj>... Dens>
[[nodiscard]] consteval auto expr_map_impl(type_list<Nums...>, type_list<Dens...>)
{
  return (OneType{} * ... * map_power(typename expr_type_map<std::remove_const_t<Nums>, Proj>::type{})) /
         (OneType{} * ... * map_power(typename expr_type_map<std::remove_const_t<Dens>, Proj>::type{}));
}

/**
 * @brief Maps contents of one expression template to another resulting in a different type list
 *
 * @tparam Proj Projection to be used for mapping
 * @tparam To destination type list to put the result to
 * @tparam OneType type that represents the value `1`
 * @tparam Pred binary less then predicate
 * @tparam T expression template to map from
 */
template<template<typename> typename Proj, template<typename...> typename To, typename OneType,
         template<typename, typename> typename Pred, expr_projectable<Proj> T>
[[nodiscard]] consteval auto expr_map(T)
{
  if constexpr (type_list_size<typename T::_num_> + type_list_size<typename T::_den_> == 0)
    return OneType{};
  else
    return expr_map_impl<Proj, To, OneType, Pred>(typename T::_num_{}, typename T::_den_{});
}

}  // namespace detail

}  // namespace mp_units
