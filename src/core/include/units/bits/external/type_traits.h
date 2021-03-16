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

#include <units/bits/external/hacks.h>
#include <type_traits>
#include <utility>

namespace units {

// conditional
namespace detail {

template<bool>
struct conditional_impl {
  template<typename T, typename F>
  using type = F;
};

template<>
struct conditional_impl<true> {
  template<typename T, typename F>
  using type = T;
};

}  // namespace detail

template<bool B, typename T, typename F>
using conditional = TYPENAME detail::conditional_impl<B>::template type<T, F>;

// is_same
template<class T, class U>
inline constexpr bool is_same_v = false;

template<class T>
inline constexpr bool is_same_v<T, T> = true;

template<class T, class U>
using is_same = std::bool_constant<is_same_v<T, U>>;

// is_specialization_of
template<typename T, template<typename...> typename Type>
inline constexpr bool is_specialization_of = false;

template<typename... Params, template<typename...> typename Type>
inline constexpr bool is_specialization_of<Type<Params...>, Type> = true;

// is_derived_from_specialization_of
namespace detail {

template<template<typename...> typename Type, typename... Params>
void to_base_specialization_of(const volatile Type<Params...>*);

}  // namespace detail

template<typename T, template<typename...> typename Type>
// inline constexpr bool // TODO: Replace with concept when it works with MSVC
concept is_derived_from_specialization_of = requires(T* t) { detail::to_base_specialization_of<Type>(t); };

}  // namespace units
