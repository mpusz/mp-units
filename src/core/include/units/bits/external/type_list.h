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

#include <cstddef>

UNITS_DIAGNOSTIC_PUSH
UNITS_DIAGNOSTIC_IGNORE_EXPR_ALWAYS_TF

namespace units {

namespace detail {

template<typename T>
inline constexpr bool is_type_list = false;

template<template<typename...> typename T, typename... Types>
inline constexpr bool is_type_list<T<Types...>> = true;

}  // namespace detail

template<typename T>
concept TypeList = detail::is_type_list<T>;

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
using type_list_push_front = TYPENAME detail::type_list_push_front_impl<List, Types...>::type;

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
using type_list_push_back = TYPENAME detail::type_list_push_back_impl<List, Types...>::type;

// join

namespace detail {

template<typename List, typename... Rest>
struct type_list_join_impl {
  using type = List;
};

template<template<typename...> typename List, typename... First, typename... Second, typename... Rest>
struct type_list_join_impl<List<First...>, List<Second...>, Rest...> {
  using type = TYPENAME type_list_join_impl<List<First..., Second...>, Rest...>::type;
};

}  // namespace detail

template<TypeList... Lists>
using type_list_join = TYPENAME detail::type_list_join_impl<Lists...>::type;

// split

namespace detail {

template<template<typename...> typename List, std::size_t Idx, std::size_t N, typename... Types>
struct split_impl;

template<template<typename...> typename List, std::size_t Idx, std::size_t N>
struct split_impl<List, Idx, N> {
  using first_list = List<>;
  using second_list = List<>;
};

template<template<typename...> typename List, std::size_t Idx, std::size_t N, typename T, typename... Rest>
  requires(Idx < N)
struct split_impl<List, Idx, N, T, Rest...> : split_impl<List, Idx + 1, N, Rest...> {
  using base = split_impl<List, Idx + 1, N, Rest...>;
  using first_list = TYPENAME type_list_push_front_impl<typename base::first_list, T>::type;
  using second_list = TYPENAME base::second_list;
};

template<template<typename...> typename List, std::size_t Idx, std::size_t N, typename T, typename... Rest>
struct split_impl<List, Idx, N, T, Rest...> : split_impl<List, Idx + 1, N, Rest...> {
  using base = split_impl<List, Idx + 1, N, Rest...>;
  using first_list = TYPENAME base::first_list;
  using second_list = TYPENAME type_list_push_front_impl<typename base::second_list, T>::type;
};

}  // namespace detail

template<TypeList List, std::size_t N>
struct type_list_split;

template<template<typename...> typename List, std::size_t N, typename... Types>
struct type_list_split<List<Types...>, N> {
  static_assert(N <= sizeof...(Types), "Invalid index provided");
  using split = detail::split_impl<List, 0, N, Types...>;
  using first_list = TYPENAME split::first_list;
  using second_list = TYPENAME split::second_list;
};

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
  using type = TYPENAME type_list_push_front_impl<
    typename type_list_merge_sorted_impl<List<LhsRest...>, List<Rhs1, RhsRest...>, Pred>::type, Lhs1>::type;
};

template<template<typename...> typename List, typename Lhs1, typename... LhsRest, typename Rhs1, typename... RhsRest,
         template<typename, typename> typename Pred>
struct type_list_merge_sorted_impl<List<Lhs1, LhsRest...>, List<Rhs1, RhsRest...>, Pred> {
  using type = TYPENAME type_list_push_front_impl<
    typename type_list_merge_sorted_impl<List<Lhs1, LhsRest...>, List<RhsRest...>, Pred>::type, Rhs1>::type;
};

}  // namespace detail

template<TypeList SortedList1, TypeList SortedList2, template<typename, typename> typename Pred>
using type_list_merge_sorted = TYPENAME detail::type_list_merge_sorted_impl<SortedList1, SortedList2, Pred>::type;

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
  using sorted_left = TYPENAME type_list_sort_impl<typename split::first_list, Pred>::type;
  using sorted_right = TYPENAME type_list_sort_impl<typename split::second_list, Pred>::type;
  using type = TYPENAME type_list_merge_sorted_impl<sorted_left, sorted_right, Pred>::type;
};

}  // namespace detail

template<TypeList List, template<typename, typename> typename Pred>
using type_list_sort = TYPENAME detail::type_list_sort_impl<List, Pred>::type;

}  // namespace units

UNITS_DIAGNOSTIC_POP
