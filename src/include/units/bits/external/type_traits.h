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
using conditional = detail::conditional_impl<B>::template type<T, F>;

// is_instantiation
namespace detail {

template<typename T, template<typename...> typename Type>
inline constexpr bool is_instantiation_impl = false;

template<typename... Params, template<typename...> typename Type>
inline constexpr bool is_instantiation_impl<Type<Params...>, Type> = true;

}  // namespace detail

template<typename T, template<typename...> typename Type>
inline constexpr bool is_instantiation = detail::is_instantiation_impl<T, Type>;

// is_derived_from_instantiation
namespace detail {

template<template<typename...> typename Type>
struct is_derived_from_instantiation_impl {
  template<typename... Params>
  static constexpr std::true_type check_base(const Type<Params...>&);
  static constexpr std::true_type check_base(...);
};

}  // namespace detail

template<typename T, template<typename...> typename Type>
inline constexpr bool is_derived_from_instantiation = decltype(detail::is_derived_from_instantiation_impl<Type>::check_base(std::declval<T>()))::value;

}  // namespace units
