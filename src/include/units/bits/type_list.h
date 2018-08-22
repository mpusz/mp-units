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

#include "tools.h"
#include <type_traits>

namespace mp {

  // type_list

  template<typename... Types>
  struct type_list;

  // push_front

  template<typename TypeList, typename... Types>
  struct type_list_push_front;

  template<typename... OldTypes, typename... NewTypes>
  struct type_list_push_front<type_list<OldTypes...>, NewTypes...> {
    using type = type_list<NewTypes..., OldTypes...>;
  };

  template<typename TypeList, typename... Types>
  using type_list_push_front_t = typename type_list_push_front<TypeList, Types...>::type;

  // push_back

  template<typename TypeList, typename... Types>
  struct type_list_push_back;

  template<typename... OldTypes, typename... NewTypes>
  struct type_list_push_back<type_list<OldTypes...>, NewTypes...> {
    using type = type_list<OldTypes..., NewTypes...>;
  };

  template<typename TypeList, typename... Types>
  using type_list_push_back_t = typename type_list_push_back<TypeList, Types...>::type;

  // split

  namespace detail {

    template<std::size_t Idx, std::size_t N, typename... Types>
    struct split_impl;

    template<std::size_t Idx, std::size_t N>
    struct split_impl<Idx, N> {
      using first_list = type_list<>;
      using second_list = type_list<>;
    };

    template<std::size_t Idx, std::size_t N, typename T, typename... Rest>
    struct split_impl<Idx, N, T, Rest...> : split_impl<Idx + 1, N, Rest...> {
      using base = split_impl<Idx + 1, N, Rest...>;
      using base_first = typename base::first_list;
      using base_second = typename base::second_list;
      using first_list = std::conditional_t < Idx<N, type_list_push_front_t<base_first, T>, base_first>;
      using second_list = std::conditional_t < Idx<N, base_second, type_list_push_front_t<base_second, T>>;
    };

  }  // namespace detail

  template<typename T, std::size_t N>
  struct type_list_split;

  template<std::size_t N, typename... Types>
  struct type_list_split<type_list<Types...>, N> {
    using split = detail::split_impl<0, N, Types...>;
    using first_list = typename split::first_list;
    using second_list = typename split::second_list;
  };

  // split_half

  template<typename T>
  struct type_list_split_half;

  template<typename... Types>
  struct type_list_split_half<type_list<Types...>> : type_list_split<type_list<Types...>, (sizeof...(Types) + 1) / 2> {
  };

  // merge_sorted

  template<typename T1, typename T2, template<typename, typename> typename Pred>
  struct type_list_merge_sorted;

  template<typename T1, typename T2, template<typename, typename> typename Pred>
  using type_list_merge_sorted_t = typename type_list_merge_sorted<T1, T2, Pred>::type;

  template<typename... Lhs, template<typename, typename> typename Pred>
  struct type_list_merge_sorted<type_list<Lhs...>, type_list<>, Pred> {
    using type = type_list<Lhs...>;
  };

  template<typename... Rhs, template<typename, typename> typename Pred>
  struct type_list_merge_sorted<type_list<>, type_list<Rhs...>, Pred> {
    using type = type_list<Rhs...>;
  };

  template<typename Lhs1, typename... LhsRest, typename Rhs1, typename... RhsRest,
           template<typename, typename> typename Pred>
  struct type_list_merge_sorted<type_list<Lhs1, LhsRest...>, type_list<Rhs1, RhsRest...>, Pred> {
    using type = std::conditional_t<
        Pred<Lhs1, Rhs1>::value,
        type_list_push_front_t<type_list_merge_sorted_t<type_list<LhsRest...>, type_list<Rhs1, RhsRest...>, Pred>,
                               Lhs1>,
        type_list_push_front_t<type_list_merge_sorted_t<type_list<Lhs1, LhsRest...>, type_list<RhsRest...>, Pred>,
                               Rhs1>>;
  };

  // sort

  template<typename T, template<typename, typename> typename Pred>
  struct type_list_sort;

  template<template<typename, typename> typename Pred>
  struct type_list_sort<type_list<>, Pred> {
    using type = type_list<>;
  };

  template<typename T, template<typename, typename> typename Pred>
  struct type_list_sort<type_list<T>, Pred> {
    using type = type_list<T>;
  };

  template<typename... Types, template<typename, typename> typename Pred>
  struct type_list_sort<type_list<Types...>, Pred> {
    using types = type_list<Types...>;
    using split = type_list_split_half<type_list<Types...>>;
    using left = typename type_list_sort<typename split::first_list, Pred>::type;
    using right = typename type_list_sort<typename split::second_list, Pred>::type;
    using type = type_list_merge_sorted_t<left, right, Pred>;
  };

  template<typename T, template<typename, typename> typename Pred>
  using type_list_sort_t = typename type_list_sort<T, Pred>::type;

}  // namespace mp