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

#include <chrono>
#include <complex>
#include <concepts>
#include <type_traits>
#include <utility>

namespace mp_units {

template<typename T>
struct vector_scalar;

template<typename T>
using number_difference_t = decltype(std::declval<const T>() - std::declval<const T>());
template<typename T>
using vector_scalar_t = vector_scalar<T>::type;

namespace detail {

template<typename T>
struct inferred_number_zero {};
template<typename T>
struct inferred_number_one {};

template<typename T>
concept is_inferred_number = std::regular<vector_scalar_t<T>>;

}  // namespace detail

template<typename T>
struct is_number : std::bool_constant<detail::is_inferred_number<T>> {};
template<typename T>
struct is_complex_number : std::false_type {};
template<typename T>
struct number_zero : detail::inferred_number_zero<T> {};
template<typename T>
struct number_one : detail::inferred_number_one<T> {};
template<typename T>
struct vector_scalar {};

template<typename T>
constexpr bool is_number_v = is_number<T>::value;
template<typename T>
constexpr bool is_complex_number_v = is_complex_number<T>::value;

template<typename T>
constexpr T number_zero_v = number_zero<T>::value;
template<typename T>
constexpr T number_one_v = number_one<T>::value;

template<typename T>
concept number = is_number_v<T> && std::regular<T>;

template<typename T, typename U>
concept common_number_with = number<T> && number<U> && std::common_with<T, U> && number<std::common_type_t<T, U>>;

template<typename T>
concept ordered_number = number<T> && std::totally_ordered<T>;

template<typename T>
concept number_line =  // clang-format off
  ordered_number<T> &&
  requires(T& v) {
    number_one_v<number_difference_t<T>>;
    { ++v } -> std::same_as<T&>;
    { --v } -> std::same_as<T&>;
    { v-- } -> std::same_as<T>;
    { v-- } -> std::same_as<T>;
  };  // clang-format on

namespace detail {

template<typename T, typename U>
concept addition_with =  // clang-format off
  number<T> &&
  number<U> &&
  requires( const T & c,  const U & d) {
    { c + d } -> common_number_with<T>;
    { d + c } -> common_number_with<T>;
  };  // clang-format on

template<typename T, typename U>
concept compound_addition_with =  // clang-format off
  addition_with<T,U> &&
  requires(T& l,  const U & d) {
    { l += d } -> std::same_as<T&>;
  };  // clang-format on

template<typename T, typename U>
concept subtraction_with =  // clang-format off
  addition_with<T,U> &&
  requires( const T & c,  const U & d) {
    { c - d } -> common_number_with<T>;
  };  // clang-format on

template<typename T, typename U>
concept compound_subtraction_with =  // clang-format off
  subtraction_with<T,U> &&
  compound_addition_with<T,U> &&
  requires(T& l,  const U & d) {
    { l -= d } -> std::same_as<T&>;
  };  // clang-format on

template<typename T, typename U, typename V>
concept multiplication_with =  // clang-format off
  number<T> &&
  number<U> &&
  number<V> &&
  requires( const T & c,  const U & u) {
    { (c * u) } -> common_number_with<V>;
  };  // clang-format on

template<typename T, typename U>
concept compound_multiplication_with =  // clang-format off
  multiplication_with<T,U,T> &&
  requires(T& l,  const U & u) {
    { l *= u } -> std::same_as<T&>;
  };  // clang-format on

template<typename T, typename U>
concept division_with =  // clang-format off
  multiplication_with<T,U,T> &&
  multiplication_with<U,T,T> &&
  requires( const T & c,  const U & u) {
    { c / u } -> common_number_with<T>;
  };  // clang-format on

template<typename T, typename U>
concept compound_division_with =  // clang-format off
  division_with<T,U> &&
  compound_multiplication_with<T,U> &&
  requires(T& l,  const U & u) {
    { l /= u } -> std::same_as<T&>;
  };  // clang-format on

}  // namespace detail

template<typename T, typename U>
concept modulo_with =  // clang-format off
  number<T> &&
  number<U> &&
  requires( const T & c,  const U & u) {
    { (c % u) } -> common_number_with<T>;
  };  // clang-format on

template<typename T, typename U>
concept compound_modulo_with =  // clang-format off
  modulo_with<T,U> &&
  requires(T& l,  const U & u) {
    { l %= u } -> std::same_as<T&>;
  };  // clang-format on

template<typename T, typename U>
concept modulus_for = modulo_with<U, T>;
template<typename T, typename U>
concept compound_modulus_for = compound_modulo_with<U, T>;

template<typename T>
concept negative =  // clang-format off
  detail::compound_addition_with<T,T> &&
  requires( const T & c) {
    number_zero_v<T>;
    { -c } -> common_number_with<T>;
  };  // clang-format on

template<typename T>
concept set_with_inverse =  // clang-format off
  detail::compound_multiplication_with<T,T> &&
  requires( const T & c) {
    { number_one_v<T> / c } -> std::common_with<T>;
  };  // clang-format on

template<typename T, typename U>
concept point_space_for =
  detail::subtraction_with<T, U> && negative<U> && common_number_with<number_difference_t<T>, U>;
template<typename T, typename U>
concept compound_point_space_for = point_space_for<T, U> && detail::compound_subtraction_with<T, U>;
template<typename T>
concept point_space = compound_point_space_for<T, number_difference_t<T>>;

template<typename T, typename U>
concept vector_space_for = point_space_for<U, T>;
template<typename T, typename U>
concept compound_vector_space_for = compound_point_space_for<U, T>;

namespace detail {

template<typename T>
concept weak_scalar = common_number_with<T, number_difference_t<T>> && point_space<T> && negative<T>;

template<typename T, typename U>
concept scales_with =
  common_number_with<U, vector_scalar_t<T>> && weak_scalar<U> && multiplication_with<T, U, T> && set_with_inverse<U>;
template<typename T, typename U>
concept compound_scales_with = scales_with<T, U> && detail::compound_multiplication_with<T, U>;

}  // namespace detail

template<typename T, typename U>
concept scalar_for = detail::scales_with<U, T>;
template<typename T, typename U>
concept field_for = scalar_for<T, U> && detail::division_with<U, T>;
template<typename T, typename U>
concept compound_scalar_for = detail::compound_scales_with<U, T>;
template<typename T, typename U>
concept compound_field_for = compound_scalar_for<T, U> && detail::compound_division_with<U, T>;

template<typename T>
concept vector_space = point_space<T> && detail::compound_scales_with<T, vector_scalar_t<T>>;
template<typename T>
concept f_vector_space = vector_space<T> && detail::compound_division_with<T, vector_scalar_t<T>>;

template<typename T>
concept field_number = f_vector_space<T> && detail::compound_scales_with<T, T>;

template<typename T>
concept field_number_line = field_number<T> && number_line<T>;

template<typename T>
concept scalar_number = field_number<T> && (field_number_line<T> || is_complex_number_v<T>);

template<typename T>
struct is_number<const T> : is_number<T> {};
template<typename T, typename U>
struct is_number<std::chrono::time_point<T, U>> : std::true_type {};
// #if __cpp_lib_chrono >= 201803
template<>
struct is_number<std::chrono::day> : std::true_type {};
template<>
struct is_number<std::chrono::month> : std::true_type {};
template<>
struct is_number<std::chrono::year> : std::true_type {};
template<>
struct is_number<std::chrono::weekday> : std::true_type {};
template<>
struct is_number<std::chrono::year_month> : std::true_type {};
// #endif

template<typename T>
struct is_complex_number<const T> : is_complex_number<T> {};
template<typename T>
struct is_complex_number<std::complex<T>> : std::true_type {};

namespace detail {

template<typename T>
concept inferable_identities = common_number_with<T, number_difference_t<T>> && std::constructible_from<T, int>;

template<detail::inferable_identities T>
struct inferred_number_zero<T> {
  static constexpr auto value = T(0);
};
template<detail::inferable_identities T>
struct inferred_number_one<T> {
  static constexpr auto value = T(1);
};

}  // namespace detail

template<typename T>
struct number_zero<const T> : number_zero<T> {};
template<typename T>
struct number_one<const T> : number_one<T> {};

template<typename T>
struct vector_scalar<const T> : vector_scalar<T> {};
template<std::integral T>
struct vector_scalar<T> : std::type_identity<T> {};
template<std::floating_point T>
struct vector_scalar<T> : std::type_identity<T> {};
template<typename T>
struct vector_scalar<std::complex<T>> : std::type_identity<T> {};
template<typename T, typename U>
struct vector_scalar<std::chrono::duration<T, U>> : std::type_identity<T> {};

}  // namespace mp_units
