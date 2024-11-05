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

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/fixed_string.h>
#include <mp-units/ext/inplace_vector.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework/dimension_concepts.h>
#include <mp-units/framework/expression_template.h>
#include <mp-units/framework/symbol_text.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <array>
#include <cstdint>
#include <iterator>
#include <string_view>
#if MP_UNITS_HOSTED
#include <string>
#endif
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

template<typename... Expr>
struct derived_dimension;

MP_UNITS_EXPORT struct dimension_one;

namespace detail {

template<typename Lhs, typename Rhs>
struct base_dimension_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

template<typename T1, typename T2>
using type_list_of_base_dimension_less = expr_less<T1, T2, base_dimension_less>;

template<typename... Expr>
struct derived_dimension_impl : expr_fractions<dimension_one, Expr...> {};

struct dimension_interface {
  template<Dimension Lhs, Dimension Rhs>
  [[nodiscard]] friend consteval Dimension auto operator*(Lhs, Rhs)
  {
    return expr_multiply<derived_dimension, struct dimension_one, type_list_of_base_dimension_less>(Lhs{}, Rhs{});
  }

  template<Dimension Lhs, Dimension Rhs>
  [[nodiscard]] friend consteval Dimension auto operator/(Lhs, Rhs)
  {
    return expr_divide<derived_dimension, struct dimension_one, type_list_of_base_dimension_less>(Lhs{}, Rhs{});
  }

  template<Dimension Lhs, Dimension Rhs>
  [[nodiscard]] friend consteval bool operator==(Lhs, Rhs)
  {
    return is_same_v<Lhs, Rhs>;
  }
};

}  // namespace detail

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * `Symbol` template parameter is an unique identifier of the base dimension. The same identifiers can be multiplied
 * and divided which will result with an adjustment of its factor in an exponent of a `derived_dimension`
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion).
 *
 * User should derive a strong type from this class template rather than use it directly in the source code.
 * For example:
 *
 * @code{.cpp}
 * inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
 * inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
 * inline constexpr struct dim_mass final : base_dimension<"M"> {} dim_mass;
 * @endcode
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 */
MP_UNITS_EXPORT template<symbol_text Symbol>
struct base_dimension : detail::dimension_interface {
  static constexpr auto _symbol_ = Symbol;  ///< Unique base dimension identifier
};

/**
 * @brief A dimension of a derived quantity
 *
 * Derived dimension is an expression of the dependence of a quantity on the base quantities of a system of quantities
 * as a product of powers of factors corresponding to the base quantities, omitting any numerical factors.
 *
 * Instead of using a raw list of exponents this library decided to use expression template syntax to make types
 * more digestable for the user. The positive exponents are ordered first and all negative exponents are put as a list
 * into the `per<...>` class template. If a power of exponent is different than `1` the dimension type is enclosed in
 * `power<Dim, Num, Den>` class template. Otherwise, it is just put directly in the list without any wrapper. There
 * is also one special case. In case all of the exponents are negative than the `dimension_one` being a dimension of
 * a dimensionless quantity is put in the front to increase the readability.
 *
 * For example:
 *
 * @code{.cpp}
 * using frequency = decltype(inverse(dim_time));
 * using speed = decltype(dim_length / dim_time);
 * using acceleration = decltype(dim_speed / dim_time);
 * using force = decltype(dim_mass * dim_acceleration);
 * using energy = decltype(dim_force * dim_length);
 * using moment_of_force = decltype(dim_length * dim_force);
 * using torque = decltype(dim_moment_of_force);
 * @endcode
 *
 * - `frequency` will be derived from type `derived_dimension<dimension_one, per<dim_time>>`
 * - `speed` will be derived from type `derived_dimension<dim_length, per<dim_time>>`
 * - `acceleration` will be derived from type `derived_dimension<dim_length, per<power<dim_time, 2>>>`
 * - `force` will be derived from type `derived_dimension<dim_length, dim_mass, per<power<dim_time, 2>>>`
 * - `energy` will be derived from type `derived_dimension<power<dim_length, 2>, dim_mass, per<power<dim_time, 2>>>`
 *
 * @note A common convention in this library is to assign the same name for a type and an object of this type.
 *       Besides defining them user never works with the dimension types in the source code. All operations
 *       are done on the objects. Contrarily, the dimension types are the only one visible in the compilation
 *       errors. Having them of the same names improves user experience and somehow blurs those separate domains.
 *
 * @tparam Ds a parameter pack consisting tokens allowed in the dimension specification
 *         (base dimensions, `dimension_one`, `power<Dim, Num, Den>`, `per<...>`)
 *
 * @note User should not instantiate this type! It is not exported from the C++ module. The library will
 *       instantiate this type automatically based on the dimensional arithmetic equation provided by the user.
 */
template<typename... Expr>
struct derived_dimension final : detail::dimension_interface, detail::derived_dimension_impl<Expr...> {};

/**
 * @brief Dimension one
 *
 * Dimension for which all the exponents of the factors corresponding to the base
 * dimensions are zero. It is a dimension of a quantity of dimension one also known as
 * "dimensionless".
 */
MP_UNITS_EXPORT inline constexpr struct dimension_one final :
    detail::dimension_interface,
    detail::derived_dimension_impl<> {
} dimension_one;

MP_UNITS_EXPORT_BEGIN

[[nodiscard]] consteval Dimension auto inverse(Dimension auto d) { return dimension_one / d; }

/**
 * @brief Computes the value of a dimension raised to the `Num/Den` power
 *
 * @tparam Num Exponent numerator
 * @tparam Den Exponent denominator
 * @param d Dimension being the base of the operation
 *
 * @return Dimension The result of computation
 */
template<std::intmax_t Num, std::intmax_t Den = 1, Dimension D>
  requires detail::non_zero<Den>
[[nodiscard]] consteval Dimension auto pow(D d)
{
  return detail::expr_pow<Num, Den, derived_dimension, struct dimension_one, detail::type_list_of_base_dimension_less>(
    d);
}

/**
 * @brief Computes the square root of a dimension
 *
 * @param d Dimension being the base of the operation
 *
 * @return Dimension The result of computation
 */
[[nodiscard]] consteval Dimension auto sqrt(Dimension auto d) { return pow<1, 2>(d); }

/**
 * @brief Computes the cubic root of a dimension
 *
 * @param d Dimension being the base of the operation
 *
 * @return Dimension The result of computation
 */
[[nodiscard]] consteval Dimension auto cbrt(Dimension auto d) { return pow<1, 3>(d); }


struct dimension_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
};

MP_UNITS_EXPORT_END

namespace detail {

template<typename CharT, std::output_iterator<CharT> Out, Dimension D>
  requires requires { D::_symbol_; }
constexpr Out dimension_symbol_impl(Out out, D, const dimension_symbol_formatting& fmt, bool negative_power)
{
  return copy_symbol<CharT>(D::_symbol_, fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
constexpr auto dimension_symbol_impl(Out out, const power<F, Num, Den...>&, const dimension_symbol_formatting& fmt,
                                     bool negative_power)
{
  out = dimension_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed
  return copy_symbol_exponent<CharT, Num, Den...>(fmt.encoding, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Ms>
constexpr Out dimension_symbol_impl(Out out, const type_list<Ms...>&, const dimension_symbol_formatting& fmt,
                                    bool negative_power)
{
  return (..., (out = dimension_symbol_impl<CharT>(out, Ms{}, fmt, negative_power)));
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Nums, typename... Dens>
constexpr Out dimension_symbol_impl(Out out, const type_list<Nums...>& nums, const type_list<Dens...>& dens,
                                    const dimension_symbol_formatting& fmt)
{
  if constexpr (sizeof...(Nums) == 0 && sizeof...(Dens) == 0) {
    // dimensionless quantity
    *out++ = '1';
    return out;
  } else if constexpr (sizeof...(Dens) == 0) {
    // no denominator
    return dimension_symbol_impl<CharT>(out, nums, fmt, false);
  } else {
    if constexpr (sizeof...(Nums) > 0) out = dimension_symbol_impl<CharT>(out, nums, fmt, false);
    return dimension_symbol_impl<CharT>(out, dens, fmt, true);
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Expr>
constexpr Out dimension_symbol_impl(Out out, const derived_dimension_impl<Expr...>&,
                                    const dimension_symbol_formatting& fmt, bool negative_power)
{
  (void)negative_power;
  MP_UNITS_EXPECTS(negative_power == false);
  return dimension_symbol_impl<CharT>(out, typename derived_dimension_impl<Expr...>::_num_{},
                                      typename derived_dimension_impl<Expr...>::_den_{}, fmt);
}


}  // namespace detail

MP_UNITS_EXPORT template<typename CharT = char, std::output_iterator<CharT> Out, Dimension D>
constexpr Out dimension_symbol_to(Out out, D d, const dimension_symbol_formatting& fmt = dimension_symbol_formatting{})
{
  return detail::dimension_symbol_impl<CharT>(out, d, fmt, false);
}

// TODO Refactor to `dimension_symbol(D, fmt)` when P1045: constexpr Function Parameters is available
MP_UNITS_EXPORT template<dimension_symbol_formatting fmt = dimension_symbol_formatting{}, typename CharT = char,
                         Dimension D>
#if defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG <= 18
[[nodiscard]] constexpr auto dimension_symbol(D)
#else
[[nodiscard]] consteval auto dimension_symbol(D)
#endif
{
  constexpr auto oversized_symbol_text = []() consteval {
    // std::basic_string<CharT> text;  // TODO uncomment when https://wg21.link/P3032 is supported
    detail::inplace_vector<CharT, 128> text;
    dimension_symbol_to<CharT>(std::back_inserter(text), D{}, fmt);
    return text;
  }();

#if MP_UNITS_API_STRING_VIEW_RET  // Permitting static constexpr variables in constexpr functions
  static constexpr basic_fixed_string<CharT, oversized_symbol_text.size()> storage(std::from_range,
                                                                                   oversized_symbol_text);
  return storage.view();
#else
  return basic_fixed_string<CharT, oversized_symbol_text.size()>(std::from_range, oversized_symbol_text);
#endif
}

}  // namespace mp_units
