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

#include <units/bits/equivalent.h>
#include <cassert>
#include <concepts>
#include <type_traits>

template<typename T, typename U>
inline constexpr bool compare_impl = false;

template<typename T>
inline constexpr bool compare_impl<T, T> = true;

template<typename T, typename U>
  requires(UNITS_DOWNCAST_MODE == 0)
inline constexpr bool compare_impl<T, U> = units::equivalent<T, U>;

template<typename T, typename U>
inline constexpr bool compare = compare_impl<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template<typename T, typename... Us, typename... Vs>
constexpr bool constructible_from(Vs...)
{
  return std::constructible_from<T, Us..., Vs...>;
}

template<class T>
void convertible_from__(T);

template<typename T, typename... Us>
concept convertible_from_ = requires(Us... us) { convertible_from__<T>({us...}); };

template<typename T, typename... Us, typename... Vs>
constexpr bool convertible_from(Vs...)
{
  if constexpr (sizeof...(Us) + sizeof...(Vs) == 1)
    return std::is_convertible_v<Us..., Vs..., T>;
  else
    return convertible_from_<T, Us..., Vs...>;
}

template<typename T, typename... Us, typename... Vs>
constexpr bool constructible_or_convertible_from(Vs...)
{
  return constructible_from<T, Us..., Vs...>() || convertible_from<T, Us..., Vs...>();
}

template<typename T, typename... Us, typename... Vs>
constexpr bool constructible_and_convertible_from(Vs...)
{
  return constructible_from<T, Us..., Vs...>() && convertible_from<T, Us..., Vs...>();
}

template<typename T, typename... Us>
  requires(constructible_from<T, Us...>())
constexpr T construct_from(Us... us)
{
  return T(us...);
}

template<typename T, typename... Us>
  requires(convertible_from<T, Us...>())
constexpr T convert_from(Us... us)
{
  if constexpr (sizeof...(Us) == 1)
    return [](T t) { return t; }(us...);
  else
    return {us...};
}

template<std::equality_comparable T, typename... Us>
  requires(constructible_from<T, Us...>() && convertible_from<T, Us...>())
constexpr T construct_and_convert_from(Us... us)
{
  T t{construct_from<T>(us...)};
  assert(t == convert_from<T>(us...));
  return t;
}

template<typename T, typename... Us>
  requires(constructible_from<T, Us...>() && !convertible_from<T, Us...>())
constexpr T construct_from_only(Us... us)
{
  return construct_from<T>(us...);
}

#if !defined(UNITS_COMP_GCC)
template<template<typename...> typename T, typename = std::void_t<>, typename... Us>
concept ctad_constructible_from_ = requires(Us... us) { T(us...); };
#else
template<template<typename...> typename T, typename = std::void_t<>, typename... Us>
inline constexpr bool ctad_constructible_from_ = false;

template<template<typename...> typename T, typename... Us>
inline constexpr bool ctad_constructible_from_<T, std::void_t<decltype(T(Us{}...))>, Us...> = true;
#endif

template<template<typename...> typename T, typename... Us, typename... Vs>
constexpr bool ctad_constructible_from(Vs...)
{
  return ctad_constructible_from_<T, void, Us..., Vs...>;
}

constexpr auto same = []<std::equality_comparable T>(T l, T r) { return l == r; };
constexpr auto comp =  // TODO: Fix #205 to use `std::equality_comparable_with<T> U`.
  []<typename T, typename U>(T l, U r)
  requires compare<T, U>
{
  return l == r;
};

template<auto F>
  requires requires { F(); }
inline constexpr bool require_constant_invocation = requires { new int[1][(F(), 1)]; };
