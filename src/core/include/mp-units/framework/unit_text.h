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

// IWYU pragma: private, include <mp-units/framework/unit.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/bits/text_tools.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/fixed_string.h>
#include <mp-units/ext/inplace_vector.h>
#include <mp-units/framework/symbol_text.h>
#include <mp-units/framework/symbolic_expression.h>
#include <mp-units/framework/unit_concepts.h>
#include <mp-units/framework/unit_conversion.h>
#include <mp-units/framework/unit_definitions.h>
#include <mp-units/framework/unit_symbol_formatting.h>
#if MP_UNITS_HOSTED
#include <mp-units/bits/ostream.h>
#include <mp-units/utility/format.h>
#endif

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iterator>
#include <string_view>
#include <tuple>
#if MP_UNITS_HOSTED
#include <string>
#endif
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

MP_UNITS_EXPORT_BEGIN

/**
 * @brief Puts a space ' ' sign before a unit symbol
 *
 * Quantities of some units (e.g. degree, arcminute, arcsecond) should not be printed with the
 * space between a number and a unit. For those a partial specialization with the value `false` should
 * be provided.
 */
template<Unit auto U>
constexpr bool space_before_unit_symbol = true;

template<>
MP_UNITS_INLINE constexpr bool space_before_unit_symbol<one> = false;

MP_UNITS_EXPORT_END

namespace detail {

template<typename CharT, std::output_iterator<CharT> Out, Unit U>
  requires requires { U::_symbol_; }
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, U, const unit_symbol_formatting& fmt, bool negative_power)
{
  return copy_symbol<CharT>(U::_symbol_, fmt.char_set, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, auto M, typename U>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const scaled_unit_impl<M, U>& u,
                                             const unit_symbol_formatting& fmt, bool negative_power)
{
  *out++ = '(';
  out = magnitude_symbol<CharT>(out, M, fmt);
  if constexpr (space_before_unit_symbol<scaled_unit<M, U>::_reference_unit_>) *out++ = ' ';
  out = unit_symbol_impl<CharT>(out, u._reference_unit_, fmt, negative_power);
  *out++ = ')';
  return out;
}

template<typename... Us, Unit U>
[[nodiscard]] consteval Unit auto get_common_unit_in(common_unit<Us...>, U)
{
  auto get_magnitude = [&]() {
    if constexpr (requires { common_unit<Us...>::_mag_; })
      return common_unit<Us...>::_mag_;
    else
      return mag<1>;
  };
  constexpr auto canonical_u = mp_units::get_canonical_unit(U{});
  constexpr UnitMagnitude auto cmag = get_magnitude() / canonical_u.mag;
  if constexpr (cmag == mag<1>)
    return U{};
  else
    return scaled_unit<cmag, U>{};
}

template<typename CharT, std::output_iterator<CharT> Out, typename U, typename... Rest>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const common_unit<U, Rest...>&, const unit_symbol_formatting& fmt,
                                             bool negative_power)
{
  constexpr std::string_view prefix("[");
  constexpr std::string_view separator(", ");
  auto print_unit = [&]<Unit Arg>(Arg) {
    constexpr auto u = get_common_unit_in(common_unit<U, Rest...>{}, Arg{});
    out = unit_symbol_impl<CharT>(out, u, fmt, negative_power);
  };
  out = detail::copy(std::begin(prefix), std::end(prefix), out);
  print_unit(U{});
  for_each(std::tuple<Rest...>{}, [&]<Unit Arg>(Arg) {
    out = detail::copy(std::begin(separator), std::end(separator), out);
    print_unit(Arg{});
  });
  *out++ = ']';
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, typename F, int Num, int... Den>
[[nodiscard]] constexpr auto unit_symbol_impl(Out out, const power<F, Num, Den...>&, const unit_symbol_formatting& fmt,
                                              bool negative_power)
{
  out = unit_symbol_impl<CharT>(out, F{}, fmt, false);  // negative power component will be added below if needed
  return copy_symbol_exponent<CharT, Num, Den...>(fmt.char_set, negative_power, out);
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Us>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const type_list<>&, const unit_symbol_formatting&, bool)
{
  return out;
}

template<typename CharT, std::output_iterator<CharT> Out, typename U, typename... Rest>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const type_list<U, Rest...>&, const unit_symbol_formatting& fmt,
                                             bool negative_power)
{
  return ((out = unit_symbol_impl<CharT>(out, U{}, fmt, negative_power)), ...,
          (out = print_separator<CharT>(out, fmt), out = unit_symbol_impl<CharT>(out, Rest{}, fmt, negative_power)));
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Nums, typename... Dens>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const type_list<Nums...>& nums, const type_list<Dens...>& dens,
                                             const unit_symbol_formatting& fmt)
{
  if constexpr (sizeof...(Nums) == 0 && sizeof...(Dens) == 0) {
    // dimensionless quantity
    return out;
  } else if constexpr (sizeof...(Dens) == 0) {
    // no denominator
    return unit_symbol_impl<CharT>(out, nums, fmt, false);
  } else {
    using enum unit_symbol_solidus;
    if constexpr (sizeof...(Nums) > 0) out = unit_symbol_impl<CharT>(out, nums, fmt, false);

    if (fmt.solidus == always || (fmt.solidus == one_denominator && sizeof...(Dens) == 1)) {
      if constexpr (sizeof...(Nums) == 0) *out++ = '1';
      *out++ = '/';
      if constexpr (sizeof...(Dens) > 1) *out++ = '(';
    } else if constexpr (sizeof...(Nums) > 0) {
      out = print_separator<CharT>(out, fmt);
    }

    const bool negative_power = fmt.solidus == never || (fmt.solidus == one_denominator && sizeof...(Dens) > 1);
    out = unit_symbol_impl<CharT>(out, dens, fmt, negative_power);
    if (fmt.solidus == always && sizeof...(Dens) > 1) *out++ = ')';
    return out;
  }
}

template<typename CharT, std::output_iterator<CharT> Out, typename... Expr>
[[nodiscard]] constexpr Out unit_symbol_impl(Out out, const derived_unit_impl<Expr...>&,
                                             const unit_symbol_formatting& fmt, bool negative_power)
{
  (void)negative_power;
  MP_UNITS_PRECONDITION(negative_power == false);
  return unit_symbol_impl<CharT>(out, typename derived_unit_impl<Expr...>::_num_{},
                                 typename derived_unit_impl<Expr...>::_den_{}, fmt);
}

}  // namespace detail

MP_UNITS_EXPORT template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, const unit_symbol_formatting& fmt = unit_symbol_formatting{})
{
  return detail::unit_symbol_impl<CharT>(out, u, fmt, false);
}

namespace detail {

MP_UNITS_EXPORT template<unit_symbol_formatting fmt, typename CharT, Unit U>
[[nodiscard]] consteval auto unit_symbol_impl(U)
{
  constexpr auto oversized_symbol_text = []() consteval {
    // std::basic_string<CharT> text;  // TODO uncomment when https://wg21.link/P3032 is supported
    detail::inplace_vector<CharT, 128> text;
    unit_symbol_to<CharT>(std::back_inserter(text), U{}, fmt);
    return text;
  }();
  return basic_fixed_string<CharT, oversized_symbol_text.size()>(std::from_range, oversized_symbol_text);
}

template<unit_symbol_formatting fmt, typename CharT, Unit U>
constexpr auto unit_symbol_result = unit_symbol_impl<fmt, CharT>(U{});

}  // namespace detail

// TODO Refactor to `unit_symbol(U, fmt)` when P1045: constexpr Function Parameters is available
MP_UNITS_EXPORT template<unit_symbol_formatting fmt = unit_symbol_formatting{}, typename CharT = char, Unit U>
[[nodiscard]] consteval std::basic_string_view<CharT> unit_symbol(U)
{
  return detail::unit_symbol_result<fmt, CharT, U>.view();
}

#if MP_UNITS_HOSTED

MP_UNITS_EXPORT template<typename CharT, typename Traits, Unit U>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, U u)
{
  return detail::to_stream(
    os, [&](std::basic_ostream<CharT, Traits>& oss) { unit_symbol_to<CharT>(std::ostream_iterator<CharT>(oss), u); });
}

#endif  // MP_UNITS_HOSTED

}  // namespace mp_units

#if MP_UNITS_HOSTED

//
// Grammar
//
// unit-format-spec      = [fill-and-align], [width], [unit-spec];
// unit-spec             = [character-set], [unit-symbol-solidus], [unit-symbol-separator], [L]
//                       | [character-set], [unit-symbol-separator], [unit-symbol-solidus], [L]
//                       | [unit-symbol-solidus], [character-set], [unit-symbol-separator], [L]
//                       | [unit-symbol-solidus], [unit-symbol-separator], [character-set], [L]
//                       | [unit-symbol-separator], [character-set], [unit-symbol-solidus], [L]
//                       | [unit-symbol-separator], [unit-symbol-solidus], [character-set], [L];
// unit-symbol-solidus   = '1' | 'a' | 'n';
// unit-symbol-separator = 's' | 'd';
//
template<typename U, typename Char>
  requires mp_units::detail::GCC_120625_is_complete<U> && mp_units::Unit<U>
class MP_UNITS_STD_FMT::formatter<U, Char> {
  struct format_specs : mp_units::utility::fill_align_width_format_specs<Char>, mp_units::unit_symbol_formatting {};
  format_specs specs_{};

  template<std::forward_iterator It>
  constexpr It parse_unit_specs(It begin, It end)
  {
    auto it = begin;
    if (it == end || *it == '}') return begin;

    constexpr auto valid_modifiers = std::string_view{"UAP1ansd"};
    for (; it != end && *it != '}'; ++it) {
      if (valid_modifiers.find(*it) == std::string_view::npos)
        throw MP_UNITS_STD_FMT::format_error("invalid unit modifier specified");
    }
    end = it;

    if (it = mp_units::utility::at_most_one_of(begin, end, "UAP"); it != end)
      // TODO 'A' stands for an old and deprecated ASCII encoding
      specs_.char_set = (*it == 'U') ? mp_units::character_set::utf8 : mp_units::character_set::portable;
    if (it = mp_units::utility::at_most_one_of(begin, end, "1an"); it != end) {
      switch (*it) {
        case '1':
          specs_.solidus = mp_units::unit_symbol_solidus::one_denominator;
          break;
        case 'a':
          specs_.solidus = mp_units::unit_symbol_solidus::always;
          break;
        case 'n':
          specs_.solidus = mp_units::unit_symbol_solidus::never;
          break;
      }
    }
    if (it = mp_units::utility::at_most_one_of(begin, end, "sd"); it != end) {
      if (*it == 'd' && specs_.char_set == mp_units::character_set::portable)
        throw MP_UNITS_STD_FMT::format_error("half_high_dot unit separator allowed only for UTF-8 encoding");
      specs_.separator =
        (*it == 's') ? mp_units::unit_symbol_separator::space : mp_units::unit_symbol_separator::half_high_dot;
    }
    return end;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    const auto begin = ctx.begin();
    auto end = ctx.end();

    auto it = parse_fill_align_width(ctx, begin, end, specs_);
    if (it == end) return it;

    return parse_unit_specs(it, end);
  }

  template<typename FormatContext>
  constexpr auto format(const U& u, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto specs = specs_;
    mp_units::utility::handle_dynamic_spec<mp_units::utility::width_checker>(specs.width, specs.width_ref, ctx);

    // Use a fixed-size stack buffer so that unit_symbol_to is always instantiated
    // with Char* regardless of the FormatContext iterator type. This collapses all
    // call-site instantiations of unit_symbol_impl to a single one (Char*).
    Char buf[128];
    const Char* const end = mp_units::unit_symbol_to<Char>(buf, u, specs);
    return mp_units::utility::write_padded<Char>(ctx.out(), std::basic_string_view<Char>{buf, end}, specs.width,
                                                 specs.align, specs.fill);
  }
};

#endif  // MP_UNITS_HOSTED
