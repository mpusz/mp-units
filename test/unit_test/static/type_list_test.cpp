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

#include <mp-units/bits/external/type_list.h>
#include <mp-units/bits/external/type_traits.h>

namespace {

using namespace mp_units;

template<typename... Types>
struct type_list;

// TypeList

static_assert(TypeList<type_list<>>);
static_assert(TypeList<type_list<int>>);
static_assert(TypeList<type_list<int, float>>);

// type_list_size

static_assert(type_list_size<type_list<>> == 0);
static_assert(type_list_size<type_list<int>> == 1);
static_assert(type_list_size<type_list<int, int>> == 2);
static_assert(type_list_size<type_list<int, long, int>> == 3);

// type_list_map

template<typename... Types>
struct other_list;

static_assert(is_same_v<type_list_map<type_list<>, other_list>, other_list<>>);
static_assert(is_same_v<type_list_map<type_list<int>, other_list>, other_list<int>>);
static_assert(is_same_v<type_list_map<type_list<double, int>, other_list>, other_list<double, int>>);

// type_list_element

static_assert(is_same_v<type_list_element<type_list<int>, 0>, int>);
static_assert(is_same_v<type_list_element<type_list<int, double, float, long>, 0>, int>);
static_assert(is_same_v<type_list_element<type_list<int, double, float, long>, 1>, double>);
static_assert(is_same_v<type_list_element<type_list<int, double, float, long>, 2>, float>);
static_assert(is_same_v<type_list_element<type_list<int, double, float, long>, 3>, long>);

// type_list_front

template<template<typename...> typename List>
concept type_list_front_invalid_for_empty_list = !requires { typename type_list_front<List<>>; };

static_assert(type_list_front_invalid_for_empty_list<type_list>);
static_assert(is_same_v<type_list_front<type_list<int>>, int>);
static_assert(is_same_v<type_list_front<type_list<long, int>>, long>);
static_assert(is_same_v<type_list_front<type_list<float, long, int>>, float>);

// type_list_back

template<template<typename...> typename List>
concept type_list_back_invalid_for_empty_list = !requires { typename type_list_back<List<>>; };

static_assert(type_list_back_invalid_for_empty_list<type_list>);
static_assert(is_same_v<type_list_back<type_list<int>>, int>);
static_assert(is_same_v<type_list_back<type_list<int, long>>, long>);
static_assert(is_same_v<type_list_back<type_list<int, long, float>>, float>);

// type_list_push_front

static_assert(is_same_v<type_list_push_front<type_list<>, int>, type_list<int>>);
static_assert(is_same_v<type_list_push_front<type_list<>, int, long, double>, type_list<int, long, double>>);
static_assert(is_same_v<type_list_push_front<type_list<double>, int, long>, type_list<int, long, double>>);

// type_list_push_back

static_assert(is_same_v<type_list_push_back<type_list<>, int>, type_list<int>>);
static_assert(is_same_v<type_list_push_back<type_list<>, int, long, double>, type_list<int, long, double>>);
static_assert(is_same_v<type_list_push_back<type_list<double>, int, long>, type_list<double, int, long>>);

static_assert(is_same_v<type_list_join<type_list<>>, type_list<>>);
static_assert(is_same_v<type_list_join<type_list<>, type_list<>>, type_list<>>);
static_assert(is_same_v<type_list_join<type_list<>, type_list<>, type_list<>>, type_list<>>);
static_assert(is_same_v<type_list_join<type_list<int>, type_list<>, type_list<>>, type_list<int>>);
static_assert(is_same_v<type_list_join<type_list<>, type_list<int>, type_list<>>, type_list<int>>);
static_assert(is_same_v<type_list_join<type_list<>, type_list<>, type_list<int>>, type_list<int>>);
static_assert(
  is_same_v<type_list_join<type_list<int>, type_list<float>, type_list<bool>>, type_list<int, float, bool>>);
static_assert(is_same_v<type_list_join<type_list<int, short>, type_list<float, double>, type_list<bool>>,
                        type_list<int, short, float, double, bool>>);

// type_list_join
static_assert(is_same_v<type_list_join<type_list<>, type_list<>>, type_list<>>);
static_assert(is_same_v<type_list_join<type_list<int>, type_list<>>, type_list<int>>);
static_assert(is_same_v<type_list_join<type_list<>, type_list<int>>, type_list<int>>);
static_assert(is_same_v<type_list_join<type_list<int>, type_list<int>>, type_list<int, int>>);
static_assert(is_same_v<type_list_join<type_list<int>, type_list<long>>, type_list<int, long>>);
static_assert(
  is_same_v<type_list_join<type_list<int, long>, type_list<float, double>>, type_list<int, long, float, double>>);

// type_list_split

static_assert(is_same_v<type_list_split<type_list<int>, 0>::first_list, type_list<>>);
static_assert(is_same_v<type_list_split<type_list<int>, 0>::second_list, type_list<int>>);

static_assert(is_same_v<type_list_split<type_list<int>, 1>::first_list, type_list<int>>);
static_assert(is_same_v<type_list_split<type_list<int>, 1>::second_list, type_list<>>);

static_assert(is_same_v<type_list_split<type_list<int, long>, 0>::first_list, type_list<>>);
static_assert(is_same_v<type_list_split<type_list<int, long>, 0>::second_list, type_list<int, long>>);

static_assert(is_same_v<type_list_split<type_list<int, long>, 1>::first_list, type_list<int>>);
static_assert(is_same_v<type_list_split<type_list<int, long>, 1>::second_list, type_list<long>>);

static_assert(is_same_v<type_list_split<type_list<int, long>, 2>::first_list, type_list<int, long>>);
static_assert(is_same_v<type_list_split<type_list<int, long>, 2>::second_list, type_list<>>);

static_assert(is_same_v<type_list_split<type_list<int, long, double>, 1>::first_list, type_list<int>>);
static_assert(is_same_v<type_list_split<type_list<int, long, double>, 1>::second_list, type_list<long, double>>);

static_assert(is_same_v<type_list_split<type_list<int, long, double>, 2>::first_list, type_list<int, long>>);
static_assert(is_same_v<type_list_split<type_list<int, long, double>, 2>::second_list, type_list<double>>);

// type_list_split_half

static_assert(is_same_v<type_list_split_half<type_list<int>>::first_list, type_list<int>>);
static_assert(is_same_v<type_list_split_half<type_list<int>>::second_list, type_list<>>);

static_assert(is_same_v<type_list_split_half<type_list<int, long>>::first_list, type_list<int>>);
static_assert(is_same_v<type_list_split_half<type_list<int, long>>::second_list, type_list<long>>);

static_assert(is_same_v<type_list_split_half<type_list<int, long, double>>::first_list, type_list<int, long>>);
static_assert(is_same_v<type_list_split_half<type_list<int, long, double>>::second_list, type_list<double>>);

static_assert(is_same_v<type_list_split_half<type_list<int, long, double, float>>::first_list, type_list<int, long>>);
static_assert(
  is_same_v<type_list_split_half<type_list<int, long, double, float>>::second_list, type_list<double, float>>);

template<auto V>
struct constant {
  static constexpr auto value = V;
};

struct v1 : constant<1> {};
struct v2 : constant<2> {};
struct v3 : constant<3> {};
struct v4 : constant<4> {};

template<typename T1, typename T2>
struct constant_less : std::bool_constant<(T1::value < T2::value)> {};

// type_list_merge_sorted

static_assert(is_same_v<type_list_merge_sorted<type_list<>, type_list<>, constant_less>, type_list<>>);
static_assert(is_same_v<type_list_merge_sorted<type_list<v1>, type_list<>, constant_less>, type_list<v1>>);
static_assert(is_same_v<type_list_merge_sorted<type_list<>, type_list<v1>, constant_less>, type_list<v1>>);
static_assert(is_same_v<type_list_merge_sorted<type_list<v1>, type_list<v2>, constant_less>, type_list<v1, v2>>);
static_assert(
  is_same_v<type_list_merge_sorted<type_list<v1, v3>, type_list<v2, v4>, constant_less>, type_list<v1, v2, v3, v4>>);
static_assert(is_same_v<type_list_merge_sorted<type_list<v1, v2, v3>, type_list<v1, v2, v4>, constant_less>,
                        type_list<v1, v1, v2, v2, v3, v4>>);

// type_list_sort

static_assert(is_same_v<type_list_sort<type_list<>, constant_less>, type_list<>>);
static_assert(is_same_v<type_list_sort<type_list<v1>, constant_less>, type_list<v1>>);
static_assert(is_same_v<type_list_sort<type_list<v1, v2>, constant_less>, type_list<v1, v2>>);
static_assert(is_same_v<type_list_sort<type_list<v2, v1>, constant_less>, type_list<v1, v2>>);
static_assert(is_same_v<type_list_sort<type_list<v2, v1, v3>, constant_less>, type_list<v1, v2, v3>>);
static_assert(is_same_v<type_list_sort<type_list<v4, v3, v2, v1>, constant_less>, type_list<v1, v2, v3, v4>>);

}  // namespace
