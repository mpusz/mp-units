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

#include <units/bits/external/type_list.h>
#include <units/bits/external/type_traits.h>
#include <units/ratio.h>

namespace units {

template<typename... Ts>
struct type_list {};

template<typename... Ts>
struct per {};

template<typename F, int...>
struct power;

// TODO make_power() that will simplify cases like <dim_length, 2, 2>?

namespace detail {

template<typename T>
inline constexpr bool is_specialization_of_power = false;

template<typename F, int... Ints>
inline constexpr bool is_specialization_of_power<power<F, Ints...>> = true;

}  // namespace detail

template<typename F, int Num>
struct power<F, Num> {
  using factor = F;
  static constexpr int num = Num;
  static constexpr int den = 1;
};

template<typename F, int Num, int Den>
struct power<F, Num, Den> {
  using factor = F;
  static constexpr int num = Num;
  static constexpr int den = Den;
};

namespace detail {

template<typename T, int Num, int Den>
constexpr auto power_or_T_impl()
{
  constexpr ratio r{Num, Den};
  if constexpr (r.den == 1) {
    if constexpr (r.num == 1)
      return T{};
    else
      return power<T, r.num>{};
  } else {
    return power<T, r.num, r.den>{};
  }
};

template<typename T, int Num, int Den>
using power_or_T = decltype(power_or_T_impl<T, Num, Den>());

// type_power
template<typename T, int Num, int Den>
struct type_power {
  using type = conditional<Den == 1, power<T, Num>, power<T, Num, Den>>;
};

template<typename T, int Num2, int Num>
struct type_power<power<T, Num2>, Num, 1> {
  using type = power<T, Num * Num2>;
};

template<typename T, int Num, int Den, int... Ints>
struct type_power<power<T, Ints...>, Num, Den> {
  static constexpr ratio r = ratio(power<T, Ints...>::num, power<T, Ints...>::den) * ratio(Num, Den);
  using type = power_or_T<T, r.num, r.den>;
};

// expr_power
template<typename List, int Num, int Den>
struct expr_power;

template<typename... Ts, int Num, int Den>
struct expr_power<type_list<Ts...>, Num, Den> {
  using type = type_list<typename type_power<Ts, Num, Den>::type...>;
};

/**
 * @brief Consolidates contiguous ranges of exponents of the same dimension
 *
 * If there is more than one exponent with the same dimension they are aggregated into one exponent by adding
 * their exponents. If this accumulation will result with 0, such a dimension is removed from the list.
 *
 * @tparam D derived dimension to consolidate
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

template<typename T, typename... Rest>
  requires(!is_specialization_of_power<T>)
struct expr_consolidate_impl<type_list<T, T, Rest...>> {
  using type = expr_consolidate_impl<type_list<power<T, 2>, Rest...>>::type;
};

template<typename T, int... Ints, typename... Rest>
struct expr_consolidate_impl<type_list<T, power<T, Ints...>, Rest...>> {
  using type = expr_consolidate_impl<
    type_list<power_or_T<T, power<T, Ints...>::num + power<T, Ints...>::den, power<T, Ints...>::den>, Rest...>>::type;
};

template<typename T, int... Ints1, int... Ints2, typename... Rest>
struct expr_consolidate_impl<type_list<power<T, Ints1...>, power<T, Ints2...>, Rest...>> {
  static constexpr ratio r =
    ratio(power<T, Ints1...>::num, power<T, Ints1...>::den) + ratio(power<T, Ints2...>::num, power<T, Ints2...>::den);
  using type = conditional<r.num == 0, typename expr_consolidate_impl<type_list<Rest...>>::type,
                           typename expr_consolidate_impl<type_list<power_or_T<T, r.num, r.den>, Rest...>>::type>;
};

template<typename List>
using expr_consolidate = typename expr_consolidate_impl<List>::type;


// expr_simplify

template<typename T, typename powerNum, typename powerDen>
struct expr_simplify_power {
  static constexpr ratio r = ratio(powerNum::num, powerNum::den) - ratio(powerDen::num, powerDen::den);
  using type = power_or_T<T, abs(r.num), r.den>;
  using num = conditional<(r > 0), type_list<type>, type_list<>>;
  using den = conditional<(r < 0), type_list<type>, type_list<>>;
};

template<typename NumList, typename DenList, template<typename, typename> typename Pred>
struct expr_simplify;

template<typename NumList, typename DenList, template<typename, typename> typename Pred>
  requires(type_list_size<NumList> == 0) || (type_list_size<DenList> == 0)
struct expr_simplify<NumList, DenList, Pred> {
  using num = NumList;
  using den = DenList;
};

template<typename Num, typename... NRest, typename Den, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<Num, NRest...>, type_list<Den, DRest...>, Pred> {
  using impl = conditional<Pred<Num, Den>::value, expr_simplify<type_list<NRest...>, type_list<Den, DRest...>, Pred>,
                           expr_simplify<type_list<Num, NRest...>, type_list<DRest...>, Pred>>;
  using num = conditional<Pred<Num, Den>::value, type_list_push_front<typename impl::num, Num>, typename impl::num>;
  using den = conditional<Pred<Num, Den>::value, typename impl::den, type_list_push_front<typename impl::den, Den>>;
};

template<typename T, typename... NRest, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<T, NRest...>, type_list<T, DRest...>, Pred> :
    expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred> {};

template<typename T, typename... NRest, int... Ints, typename... DRest, template<typename, typename> typename Pred>
struct expr_simplify<type_list<power<T, Ints...>, NRest...>, type_list<T, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, power<T, Ints...>, power<T, 1>>;
  using num = type_list_join<typename type::num, typename impl::num>;
  using den = type_list_join<typename type::den, typename impl::den>;
};

template<typename T, typename... NRest, typename... DRest, int... Ints, template<typename, typename> typename Pred>
struct expr_simplify<type_list<T, NRest...>, type_list<power<T, Ints...>, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, power<T, 1>, power<T, Ints...>>;
  using num = type_list_join<typename impl::num, typename type::num>;
  using den = type_list_join<typename impl::den, typename type::den>;
};

template<typename T, typename... NRest, int... Ints1, typename... DRest, int... Ints2,
         template<typename, typename> typename Pred>
  requires(!std::same_as<power<T, Ints1...>, power<T, Ints2...>>)
struct expr_simplify<type_list<power<T, Ints1...>, NRest...>, type_list<power<T, Ints2...>, DRest...>, Pred> {
  using impl = expr_simplify<type_list<NRest...>, type_list<DRest...>, Pred>;
  using type = expr_simplify_power<T, power<T, Ints1...>, power<T, Ints2...>>;
  using num = type_list_join<typename impl::num, typename type::num>;
  using den = type_list_join<typename impl::den, typename type::den>;
};


// expr_less
template<typename T1, typename T2, template<typename, typename> typename Pred>
struct expr_less_impl : Pred<T1, T2> {};

template<typename T1, int... Ints1, typename T2, int... Ints2, template<typename, typename> typename Pred>
struct expr_less_impl<power<T1, Ints1...>, power<T2, Ints2...>, Pred> : Pred<T1, T2> {};

template<typename T1, int... Ints, typename T2, template<typename, typename> typename Pred>
struct expr_less_impl<power<T1, Ints...>, T2, Pred> : Pred<T1, T2> {};

template<typename T1, typename T2, int... Ints, template<typename, typename> typename Pred>
struct expr_less_impl<T1, power<T2, Ints...>, Pred> : Pred<T1, T2> {};

template<typename T, int... Ints, template<typename, typename> typename Pred>
struct expr_less_impl<T, power<T, Ints...>, Pred> : std::true_type {};

template<typename T1, typename T2, template<typename, typename> typename Pred>
using expr_less = expr_less_impl<T1, T2, Pred>;


// expr_fractions
template<typename OneTypeBase, bool PerFound, typename... Ts>
struct expr_fractions_impl;

template<typename OneTypeBase>
struct expr_fractions_impl<OneTypeBase, true> {
  using den = type_list<>;
};

template<typename OneTypeBase, typename T, typename... Rest>
struct expr_fractions_impl<OneTypeBase, true, T, Rest...> {
  using den = type_list_push_front<typename expr_fractions_impl<OneTypeBase, true, Rest...>::den, T>;
};

template<typename OneTypeBase, std::derived_from<OneTypeBase> T, typename... Rest>
struct expr_fractions_impl<OneTypeBase, true, T, Rest...> :
    TYPENAME expr_fractions_impl<OneTypeBase, true, Rest...>::den {};

template<typename OneTypeBase>
struct expr_fractions_impl<OneTypeBase, false> {
  using num = type_list<>;
  using den = type_list<>;
};

template<typename OneTypeBase, typename... Rest>
struct expr_fractions_impl<OneTypeBase, false, per<Rest...>> {
  using num = type_list<>;
  using den = TYPENAME expr_fractions_impl<OneTypeBase, true, Rest...>::den;
};

template<typename OneTypeBase, std::derived_from<OneTypeBase> T, typename... Rest>
struct expr_fractions_impl<OneTypeBase, false, T, Rest...> : expr_fractions_impl<OneTypeBase, false, Rest...> {};

template<typename OneTypeBase, typename T, typename... Rest>
struct expr_fractions_impl<OneTypeBase, false, T, Rest...> {
  using impl = expr_fractions_impl<OneTypeBase, false, Rest...>;
  using num = type_list_push_front<typename impl::num, T>;
  using den = TYPENAME impl::den;
};

template<typename OneTypeBase, typename... Ts>
struct expr_fractions {
private:
  using impl = expr_fractions_impl<OneTypeBase, false, Ts...>;
public:
  using num = TYPENAME impl::num;
  using den = TYPENAME impl::den;
};

template<typename NumList, typename DenList, typename OneType, template<typename...> typename To>
[[nodiscard]] constexpr auto expr_expression_impl()
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

template<typename NumList, typename DenList, typename OneType, template<typename...> typename To>
using expr_expression = decltype(expr_expression_impl<NumList, DenList, OneType, To>());

template<typename NumList, typename DenList, typename OneType, template<typename, typename> typename Pred,
         template<typename...> typename To>
[[nodiscard]] constexpr auto get_optimized_expression()
{
  using num_list = expr_consolidate<NumList>;
  using den_list = expr_consolidate<DenList>;
  using simple = expr_simplify<num_list, den_list, Pred>;
  using expr = expr_expression<typename simple::num, typename simple::den, OneType, To>;
  return expr{};
}

/**
 * @brief Merges 2 sorted derived dimensions into one units::normalized_dimension
 *
 * A result of a dimensional calculation may result with many exponents of the same base dimension originated
 * from different parts of the equation. As the exponents lists of both operands it is enough to merge them
 * into one list and consolidate duplicates. Also it is possible that final exponents list will contain only
 * one element being a base dimension with exponent 1. In such a case the final dimension should be the base
 * dimension itself.
 */
template<typename T1, typename T2, typename OneType, template<typename, typename> typename Pred,
         template<typename...> typename To>
[[nodiscard]] constexpr auto expr_multiply()
{
  if constexpr (is_same_v<T1, OneType>) {
    return T2{};
  } else if constexpr (is_same_v<T2, OneType>) {
    return T1{};
  } else if constexpr (is_specialization_of<T1, To> && is_specialization_of<T2, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename T1::num, typename T2::num, Pred>,
                                    type_list_merge_sorted<typename T1::den, typename T2::den, Pred>, OneType, Pred,
                                    To>();
  } else if constexpr (is_specialization_of<T1, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename T1::num, type_list<T2>, Pred>, typename T1::den,
                                    OneType, Pred, To>();
  } else if constexpr (is_specialization_of<T2, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename T2::num, type_list<T1>, Pred>, typename T2::den,
                                    OneType, Pred, To>();
  } else {
    return get_optimized_expression<type_list_merge_sorted<type_list<T1>, type_list<T2>, Pred>, type_list<>, OneType,
                                    Pred, To>();
  }
}

template<typename T1, typename T2, typename OneType, template<typename, typename> typename Pred,
         template<typename...> typename To>
[[nodiscard]] constexpr auto expr_divide()
{
  if constexpr (is_same_v<T1, T2>) {
    return OneType{};
  } else if constexpr (is_same_v<T2, OneType>) {
    return T1{};
  } else if constexpr (is_specialization_of<T1, To> && is_specialization_of<T2, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename T1::num, typename T2::den, Pred>,
                                    type_list_merge_sorted<typename T1::den, typename T2::num, Pred>, OneType, Pred,
                                    To>();
  } else if constexpr (is_specialization_of<T1, To>) {
    return get_optimized_expression<typename T1::num, type_list_merge_sorted<typename T1::den, type_list<T2>, Pred>,
                                    OneType, Pred, To>();
  } else if constexpr (is_specialization_of<T2, To>) {
    return get_optimized_expression<type_list_merge_sorted<typename T2::den, type_list<T1>, Pred>, typename T2::num,
                                    OneType, Pred, To>();
  } else {
    return To<T1, per<T2>>{};
  }
}

template<typename T, typename OneType, template<typename...> typename To>
[[nodiscard]] constexpr auto expr_invert()
{
  if constexpr (is_specialization_of<T, To>)
    return expr_expression<typename T::den, typename T::num, OneType, To>{};
  else
    return To<OneType, per<T>>{};
}

}  // namespace detail

}  // namespace units
