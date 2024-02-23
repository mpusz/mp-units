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

#include <mp-units/bits/external/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <type_traits>
#include <utility>
#endif

namespace mp_units {

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

MP_UNITS_EXPORT_BEGIN

template<bool B, typename T, typename F>
using conditional = MP_UNITS_TYPENAME detail::conditional_impl<B>::template type<T, F>;

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

template<typename T, template<auto...> typename Type>
inline constexpr bool is_specialization_of_v = false;

template<auto... Params, template<auto...> typename Type>
inline constexpr bool is_specialization_of_v<Type<Params...>, Type> = true;

MP_UNITS_EXPORT_END

// is_derived_from_specialization_of
namespace detail {

template<template<typename...> typename Type, typename... Params>
void to_base_specialization_of(const volatile Type<Params...>*);

}  // namespace detail

template<typename T, template<typename...> typename Type>
// inline constexpr bool // TODO: Replace with concept when it works with MSVC
concept is_derived_from_specialization_of = requires(T* t) { detail::to_base_specialization_of<Type>(t); };


namespace detail {

template<typename T>
struct get_value_type {
  using type = MP_UNITS_TYPENAME T::value_type;
};

template<typename T>
struct get_element_type {
  using type = std::remove_reference_t<typename T::element_type>;
};

}  // namespace detail

template<typename T>
struct underlying_type {
  using type = T;
};

template<typename T>
  requires requires { typename T::value_type; } || requires { typename T::element_type; }
struct underlying_type<T> {
  using type = MP_UNITS_TYPENAME
    conditional<requires { typename T::value_type; }, detail::get_value_type<T>, detail::get_element_type<T>>::type;
};

template<typename T>
using underlying_type_t = MP_UNITS_TYPENAME underlying_type<T>::type;

template<typename T, typename... Ts>
concept one_of = (false || ... || std::same_as<T, Ts>);

template<typename T, auto... Vs>
[[nodiscard]] consteval bool contains()
{
  return (false || ... || is_same_v<std::remove_const_t<decltype(Vs)>, T>);
}

template<template<typename...> typename T, typename... Ts>
[[nodiscard]] consteval bool contains()
{
  return (false || ... || is_specialization_of<Ts, T>);
}

template<template<auto...> typename T, typename... Ts>
[[nodiscard]] consteval bool contains()
{
  return (false || ... || is_specialization_of_v<Ts, T>);
}

template<typename T, std::same_as<T> auto V>
[[nodiscard]] consteval auto get()
{
  return V;
}

template<typename T, auto V1, auto V2, auto... Vs>
[[nodiscard]] consteval auto get()
{
  if constexpr (is_same_v<T, std::remove_const_t<decltype(V1)>>)
    return V1;
  else
    return get<T, V2, Vs...>();
}

template<template<typename...> typename T, typename T1>
  requires is_specialization_of<T1, T>
[[nodiscard]] consteval auto get()
{
  return T1{};
}

template<template<typename...> typename T, typename T1, typename T2, typename... Ts>
[[nodiscard]] consteval auto get()
{
  if constexpr (is_specialization_of<T1, T>)
    return T1{};
  else
    return get<T, T2, Ts...>();
}

template<template<auto...> typename T, typename T1>
  requires is_specialization_of_v<T1, T>
[[nodiscard]] consteval auto get()
{
  return T1{};
}

template<template<auto...> typename T, typename T1, typename T2, typename... Ts>
[[nodiscard]] consteval auto get()
{
  if constexpr (is_specialization_of_v<T1, T>)
    return T1{};
  else
    return get<T, T2, Ts...>();
}

}  // namespace mp_units
