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

#include <type_traits>

namespace mp {

  namespace detail {

    template<typename T>
    inline constexpr bool is_type_list = false;

    template<template<typename...> typename T, typename... Types>
    inline constexpr bool is_type_list<T<Types...>> = true;

  }  // namespace detail

  template<typename T>
  concept bool TypeList = detail::is_type_list<T>;

  // push_front

  template<TypeList List, typename... Types>
  struct type_list_push_front;

  template<template<typename...> typename List, typename... OldTypes, typename... NewTypes>
  struct type_list_push_front<List<OldTypes...>, NewTypes...> {
    using type = List<NewTypes..., OldTypes...>;
  };

  template<TypeList List, typename... Types>
  using type_list_push_front_t = typename type_list_push_front<List, Types...>::type;

  // push_back

  template<TypeList List, typename... Types>
  struct type_list_push_back;

  template<template<typename...> typename List, typename... OldTypes, typename... NewTypes>
  struct type_list_push_back<List<OldTypes...>, NewTypes...> {
    using type = List<OldTypes..., NewTypes...>;
  };

  template<TypeList List, typename... Types>
  using type_list_push_back_t = typename type_list_push_back<List, Types...>::type;

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
    struct split_impl<List, Idx, N, T, Rest...> : split_impl<List, Idx + 1, N, Rest...> {
      using base = split_impl<List, Idx + 1, N, Rest...>;
      using base_first = typename base::first_list;
      using base_second = typename base::second_list;
      using first_list = std::conditional_t < Idx<N, type_list_push_front_t<base_first, T>, base_first>;
      using second_list = std::conditional_t < Idx<N, base_second, type_list_push_front_t<base_second, T>>;
    };

  }  // namespace detail

  template<TypeList List, std::size_t N>
  struct type_list_split;

  template<template<typename...> typename List, std::size_t N, typename... Types>
  struct type_list_split<List<Types...>, N> {
    static_assert(N <= sizeof...(Types), "Invalid index provided");
    using split = detail::split_impl<List, 0, N, Types...>;
    using first_list = typename split::first_list;
    using second_list = typename split::second_list;
  };

  // split_half

  template<TypeList List>
  struct type_list_split_half;

  template<template<typename...> typename List, typename... Types>
  struct type_list_split_half<List<Types...>> : type_list_split<List<Types...>, (sizeof...(Types) + 1) / 2> {
  };

  // merge_sorted

  template<TypeList SortedList1, TypeList SortedList2, template<typename, typename> typename Pred>
  struct type_list_merge_sorted;

  template<TypeList SortedList1, TypeList SortedList2, template<typename, typename> typename Pred>
  using type_list_merge_sorted_t = typename type_list_merge_sorted<SortedList1, SortedList2, Pred>::type;

  template<template<typename...> typename List, typename... Lhs, template<typename, typename> typename Pred>
  struct type_list_merge_sorted<List<Lhs...>, List<>, Pred> {
    using type = List<Lhs...>;
  };

  template<template<typename...> typename List, typename... Rhs, template<typename, typename> typename Pred>
  struct type_list_merge_sorted<List<>, List<Rhs...>, Pred> {
    using type = List<Rhs...>;
  };

  template<template<typename...> typename List, typename Lhs1, typename... LhsRest, typename Rhs1, typename... RhsRest,
           template<typename, typename> typename Pred>
  struct type_list_merge_sorted<List<Lhs1, LhsRest...>, List<Rhs1, RhsRest...>, Pred> {
    using type = std::conditional_t<
        Pred<Lhs1, Rhs1>::value,
        type_list_push_front_t<type_list_merge_sorted_t<List<LhsRest...>, List<Rhs1, RhsRest...>, Pred>, Lhs1>,
        type_list_push_front_t<type_list_merge_sorted_t<List<Lhs1, LhsRest...>, List<RhsRest...>, Pred>, Rhs1>>;
  };

  // sort

  template<TypeList List, template<typename, typename> typename Pred>
  struct type_list_sort;

  template<template<typename...> typename List, template<typename, typename> typename Pred>
  struct type_list_sort<List<>, Pred> {
    using type = List<>;
  };

  template<template<typename...> typename List, typename T, template<typename, typename> typename Pred>
  struct type_list_sort<List<T>, Pred> {
    using type = List<T>;
  };

  template<template<typename...> typename List, typename... Types, template<typename, typename> typename Pred>
  struct type_list_sort<List<Types...>, Pred> {
    using types = List<Types...>;
    using split = type_list_split_half<List<Types...>>;
    using left = typename type_list_sort<typename split::first_list, Pred>::type;
    using right = typename type_list_sort<typename split::second_list, Pred>::type;
    using type = type_list_merge_sorted_t<left, right, Pred>;
  };

  template<TypeList List, template<typename, typename> typename Pred>
  using type_list_sort_t = typename type_list_sort<List, Pred>::type;

}  // namespace mp