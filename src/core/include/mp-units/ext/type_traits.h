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

#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iterator>
#include <type_traits>
#include <utility>
#endif
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
using conditional = detail::conditional_impl<B>::template type<T, F>;

// is_same
template<class T, class U>
constexpr bool is_same_v = false;

template<class T>
constexpr bool is_same_v<T, T> = true;

template<class T, class U>
using is_same = std::bool_constant<is_same_v<T, U>>;

// is_specialization_of
template<typename T, template<typename...> typename Type>
constexpr bool is_specialization_of = false;

template<typename... Params, template<typename...> typename Type>
constexpr bool is_specialization_of<Type<Params...>, Type> = true;

template<typename T, template<auto...> typename Type>
constexpr bool is_specialization_of_v = false;

template<auto... Params, template<auto...> typename Type>
constexpr bool is_specialization_of_v<Type<Params...>, Type> = true;

MP_UNITS_EXPORT_END

// is_derived_from_specialization_of
namespace detail {

template<template<typename...> typename Type, typename... Params>
void to_base_specialization_of(const volatile Type<Params...>*);

template<template<auto...> typename Type, auto... Params>
void to_base_specialization_of_v(const volatile Type<Params...>*);

}  // namespace detail

template<typename T, template<typename...> typename Type>
constexpr bool is_derived_from_specialization_of = requires(T* t) { detail::to_base_specialization_of<Type>(t); };

template<typename T, template<auto...> typename Type>
constexpr bool is_derived_from_specialization_of_v = requires(T* t) { detail::to_base_specialization_of_v<Type>(t); };

template<typename T>
  requires(!std::is_pointer_v<T> && !std::is_array_v<T>) &&
            requires { typename std::indirectly_readable_traits<T>::value_type; }
using wrapped_type_t = std::indirectly_readable_traits<T>::value_type;

namespace detail {

template<typename T>
struct value_type_impl {
  using type = T;
};

template<typename T>
  requires requires { typename wrapped_type_t<T>; }
struct value_type_impl<T> {
  using type = wrapped_type_t<T>;
};

}  // namespace detail

template<typename T>
  requires std::is_object_v<T>
using value_type_t = detail::value_type_impl<T>::type;

template<typename T, typename... Ts>
concept one_of = (false || ... || std::same_as<T, Ts>);

template<typename T, auto... Vs>
[[nodiscard]] consteval bool contains()
{
  return (false || ... || is_same_v<MP_UNITS_REMOVE_CONST(decltype(Vs)), T>);
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
  if constexpr (is_same_v<T, MP_UNITS_REMOVE_CONST(decltype(V1))>)
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
