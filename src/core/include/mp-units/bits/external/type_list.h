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

#include <mp-units/bits/external/hacks.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <cstddef>
#include <type_traits>
#include <utility>
#endif

MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_EXPR_ALWAYS_TF

namespace mp_units {

namespace detail {

template<typename T>
inline constexpr bool is_type_list = false;

template<template<typename...> typename T, typename... Types>
inline constexpr bool is_type_list<T<Types...>> = true;

}  // namespace detail

template<typename T>
concept TypeList = detail::is_type_list<T>;

// size

namespace detail {

template<typename List>
struct type_list_size_impl;

template<template<typename...> typename List, typename... Types>
struct type_list_size_impl<List<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

}  // namespace detail

template<TypeList List>
inline constexpr std::size_t type_list_size = detail::type_list_size_impl<List>::value;


// map

namespace detail {

template<typename T, template<typename...> typename To>
struct type_list_map_impl;

template<template<typename...> typename From, template<typename...> typename To, typename... Args>
struct type_list_map_impl<From<Args...>, To> {
  using type = To<Args...>;
};

}  // namespace detail

template<TypeList From, template<typename...> typename To>
using type_list_map = MP_UNITS_TYPENAME detail::type_list_map_impl<From, To>::type;


// element

namespace detail {

template<std::size_t I, typename T>
struct type_list_leaf {
  using type = T;
};

template<typename Seq, typename...>
struct indexed_type_list_impl;

template<std::size_t... Is, typename... Ts>
struct indexed_type_list_impl<std::index_sequence<Is...>, Ts...> : type_list_leaf<Is, Ts>... {};

template<std::size_t I, typename T>
std::type_identity<T> type_list_element_func(const type_list_leaf<I, T>&);

}  // namespace detail

template<typename... Ts>
struct indexed_type_list : detail::indexed_type_list_impl<std::index_sequence_for<Ts...>, Ts...> {};

template<TypeList List, std::size_t I>
using type_list_element_indexed = typename decltype(detail::type_list_element_func<I>(std::declval<List>()))::type;

template<TypeList List, std::size_t I>
using type_list_element = type_list_element_indexed<type_list_map<List, indexed_type_list>, I>;


// front

namespace detail {

template<typename List>
struct type_list_front_impl;

template<template<typename...> typename List, typename T, typename... Ts>
struct type_list_front_impl<List<T, Ts...>> {
  using type = T;
};

}  // namespace detail

template<TypeList List>
using type_list_front = MP_UNITS_TYPENAME detail::type_list_front_impl<List>::type;


// back

template<TypeList List>
using type_list_back = type_list_element<List, type_list_size<List> - 1>;


// push_front

namespace detail {

template<typename List, typename... Types>
struct type_list_push_front_impl;

template<template<typename...> typename List, typename... OldTypes, typename... NewTypes>
struct type_list_push_front_impl<List<OldTypes...>, NewTypes...> {
  using type = List<NewTypes..., OldTypes...>;
};

}  // namespace detail

template<TypeList List, typename... Types>
using type_list_push_front = MP_UNITS_TYPENAME detail::type_list_push_front_impl<List, Types...>::type;


// push_back

namespace detail {

template<typename List, typename... Types>
struct type_list_push_back_impl;

template<template<typename...> typename List, typename... OldTypes, typename... NewTypes>
struct type_list_push_back_impl<List<OldTypes...>, NewTypes...> {
  using type = List<OldTypes..., NewTypes...>;
};

}  // namespace detail

template<TypeList List, typename... Types>
using type_list_push_back = MP_UNITS_TYPENAME detail::type_list_push_back_impl<List, Types...>::type;


// join

namespace detail {

template<typename List, typename... Rest>
struct type_list_join_impl {
  using type = List;
};

template<template<typename...> typename List, typename... First, typename... Second, typename... Rest>
struct type_list_join_impl<List<First...>, List<Second...>, Rest...> {
  using type = MP_UNITS_TYPENAME type_list_join_impl<List<First..., Second...>, Rest...>::type;
};

}  // namespace detail

template<TypeList... Lists>
using type_list_join = MP_UNITS_TYPENAME detail::type_list_join_impl<Lists...>::type;


// split

namespace detail {

template<typename List, typename First, typename Second>
struct type_list_split_impl;

template<template<typename...> typename List, typename... Args, std::size_t... First, std::size_t... Second>
struct type_list_split_impl<List<Args...>, std::index_sequence<First...>, std::index_sequence<Second...>> {
  using indexed_list = indexed_type_list<Args...>;
  using first_list = List<type_list_element_indexed<indexed_list, First>...>;
  using second_list = List<type_list_element_indexed<indexed_list, sizeof...(First) + Second>...>;
};

}  // namespace detail

template<TypeList List, std::size_t N>
  requires(N <= type_list_size<List>)
struct type_list_split :
    detail::type_list_split_impl<List, std::make_index_sequence<N>,
                                 std::make_index_sequence<type_list_size<List> - N>> {};


// split_half

template<TypeList List>
struct type_list_split_half;

template<template<typename...> typename List, typename... Types>
struct type_list_split_half<List<Types...>> : type_list_split<List<Types...>, (sizeof...(Types) + 1) / 2> {};


// merge_sorted

namespace detail {

template<typename SortedList1, typename SortedList2, template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl;

template<template<typename...> typename List, template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl<List<>, List<>, Pred> {
  using type = List<>;
};

template<template<typename...> typename List, typename... Lhs, template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl<List<Lhs...>, List<>, Pred> {
  using type = List<Lhs...>;
};

template<template<typename...> typename List, typename... Rhs, template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl<List<>, List<Rhs...>, Pred> {
  using type = List<Rhs...>;
};

template<template<typename...> typename List, typename Lhs1, typename... LhsRest, typename Rhs1, typename... RhsRest,
         template<typename, typename> typename Pred>
  requires Pred<Lhs1, Rhs1>::value
struct type_list_merge_sorted_impl<List<Lhs1, LhsRest...>, List<Rhs1, RhsRest...>, Pred> {
  using type = MP_UNITS_TYPENAME type_list_push_front_impl<
    typename type_list_merge_sorted_impl<List<LhsRest...>, List<Rhs1, RhsRest...>, Pred>::type, Lhs1>::type;
};

template<template<typename...> typename List, typename Lhs1, typename... LhsRest, typename Rhs1, typename... RhsRest,
         template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl<List<Lhs1, LhsRest...>, List<Rhs1, RhsRest...>, Pred> {
  using type = MP_UNITS_TYPENAME type_list_push_front_impl<
    typename type_list_merge_sorted_impl<List<Lhs1, LhsRest...>, List<RhsRest...>, Pred>::type, Rhs1>::type;
};

}  // namespace detail

template<TypeList SortedList1, TypeList SortedList2, template<typename, typename> typename Pred>
using type_list_merge_sorted =
  MP_UNITS_TYPENAME detail::type_list_merge_sorted_impl<SortedList1, SortedList2, Pred>::type;


// sort

namespace detail {

template<typename List, template<typename, typename> typename Pred>
struct type_list_sort_impl;

template<template<typename...> typename List, template<typename, typename> typename Pred>
struct type_list_sort_impl<List<>, Pred> {
  using type = List<>;
};

template<template<typename...> typename List, typename T, template<typename, typename> typename Pred>
struct type_list_sort_impl<List<T>, Pred> {
  using type = List<T>;
};

template<template<typename...> typename List, typename... Types, template<typename, typename> typename Pred>
struct type_list_sort_impl<List<Types...>, Pred> {
  using types = List<Types...>;
  using split = type_list_split_half<List<Types...>>;
  using sorted_left = MP_UNITS_TYPENAME type_list_sort_impl<typename split::first_list, Pred>::type;
  using sorted_right = MP_UNITS_TYPENAME type_list_sort_impl<typename split::second_list, Pred>::type;
  using type = MP_UNITS_TYPENAME type_list_merge_sorted_impl<sorted_left, sorted_right, Pred>::type;
};

}  // namespace detail

template<TypeList List, template<typename, typename> typename Pred>
using type_list_sort = MP_UNITS_TYPENAME detail::type_list_sort_impl<List, Pred>::type;

}  // namespace mp_units

MP_UNITS_DIAGNOSTIC_POP
