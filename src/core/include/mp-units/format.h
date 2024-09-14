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

// IWYU pragma: always_keep

#pragma once

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/fmt.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/unit.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <locale>
#endif
#endif

namespace mp_units::detail {

template<std::forward_iterator It>
[[nodiscard]] constexpr It at_most_one_of(It begin, It end, std::string_view modifiers)
{
  const It it = mp_units::detail::find_first_of(begin, end, modifiers.begin(), modifiers.end());
  if (it != end && mp_units::detail::find_first_of(it + 1, end, modifiers.begin(), modifiers.end()) != end)
    throw MP_UNITS_STD_FMT::format_error("only one of '" + std::string(modifiers) +
                                         "' unit modifiers may be used in the format spec");
  return it;
}

// TODO the below should be exposed by the C++ Standard Library (used in our examples)
MP_UNITS_EXPORT_BEGIN

template<typename Char>
struct fill_align_width_format_specs {
  fill_t<Char> fill;
  fmt_align align : 4 = fmt_align::none;
  int width = 0;
  fmt_arg_ref<Char> width_ref;
};

template<std::forward_iterator It, typename Specs>
[[nodiscard]] constexpr It parse_fill_align_width(
  MP_UNITS_STD_FMT::basic_format_parse_context<std::iter_value_t<It>>& ctx, It begin, It end, Specs& specs,
  fmt_align default_align = fmt_align::none)
{
  auto it = begin;
  if (it == end || *it == '}') return it;

  it = mp_units::detail::parse_align(it, end, specs, default_align);
  if (it == end) return it;

  return mp_units::detail::parse_dynamic_spec(it, end, specs.width, specs.width_ref, ctx);
}

template<typename Char, std::output_iterator<Char> It>
constexpr It format_global_buffer(It out, const fill_align_width_format_specs<Char>& specs)
{
  MP_UNITS_STD_FMT::format_to(out, "{{:");
  if (specs.fill.size() != 1 || specs.fill[0] != ' ') {
    MP_UNITS_STD_FMT::format_to(out, "{}", specs.fill.data());
  }
  switch (specs.align) {
    case fmt_align::left:
      MP_UNITS_STD_FMT::format_to(out, "<");
      break;
    case fmt_align::right:
      MP_UNITS_STD_FMT::format_to(out, ">");
      break;
    case fmt_align::center:
      MP_UNITS_STD_FMT::format_to(out, "^");
      break;
    default:
      break;
  }
  if (specs.width >= 1) MP_UNITS_STD_FMT::format_to(out, "{}", specs.width);
  return MP_UNITS_STD_FMT::format_to(out, "}}");
}

MP_UNITS_EXPORT_END

}  // namespace mp_units::detail

//
// Grammar
//
// dimension-format-spec = [fill-and-align], [width], [dimension-spec];
// dimension-spec        = [text-encoding];
// text-encoding         = 'U' | 'A';
//
template<mp_units::Dimension D, typename Char>
class MP_UNITS_STD_FMT::formatter<D, Char> {
  struct format_specs : mp_units::detail::fill_align_width_format_specs<Char>, mp_units::dimension_symbol_formatting {};
  format_specs specs_{};
  std::basic_string_view<Char> fill_align_width_format_str_;

  template<std::forward_iterator It>
  constexpr It parse_dimension_specs(It begin, It end)
  {
    auto it = begin;
    if (it == end || *it == '}') return begin;

    constexpr auto valid_modifiers = std::string_view{"UA"};
    for (; it != end && *it != '}'; ++it) {
      if (valid_modifiers.find(*it) == std::string_view::npos)
        throw MP_UNITS_STD_FMT::format_error("invalid dimension modifier specified");
    }
    end = it;

    if (it = mp_units::detail::at_most_one_of(begin, end, "UA"); it != end)
      specs_.encoding = (*it == 'U') ? mp_units::text_encoding::unicode : mp_units::text_encoding::ascii;

    return end;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    const auto begin = ctx.begin();
    auto end = ctx.end();

    auto it = parse_fill_align_width(ctx, begin, end, specs_);
    fill_align_width_format_str_ = {begin, it};
    if (it == end) return it;

    return parse_dimension_specs(it, end);
  }

  template<typename FormatContext>
  constexpr auto format(const D& d, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0)
      // Avoid extra copying if width is not specified
      return mp_units::dimension_symbol_to<Char>(ctx.out(), d, specs);
    std::basic_string<Char> unit_buffer;
    mp_units::dimension_symbol_to<Char>(std::back_inserter(unit_buffer), d, specs);

    const std::basic_string<Char> global_format_buffer =
      "{:" + std::basic_string<Char>{fill_align_width_format_str_} + "}";
    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                        MP_UNITS_STD_FMT::make_format_args(unit_buffer));
  }
};


//
// Grammar
//
// unit-format-spec      = [fill-and-align], [width], [unit-spec];
// unit-spec             = [text-encoding], [unit-symbol-solidus], [unit-symbol-separator], [L]
//                       | [text-encoding], [unit-symbol-separator], [unit-symbol-solidus], [L]
//                       | [unit-symbol-solidus], [text-encoding], [unit-symbol-separator], [L]
//                       | [unit-symbol-solidus], [unit-symbol-separator], [text-encoding], [L]
//                       | [unit-symbol-separator], [text-encoding], [unit-symbol-solidus], [L]
//                       | [unit-symbol-separator], [unit-symbol-solidus], [text-encoding], [L];
// unit-symbol-solidus   = '1' | 'a' | 'n';
// unit-symbol-separator = 's' | 'd';
//
template<mp_units::Unit U, typename Char>
class MP_UNITS_STD_FMT::formatter<U, Char> {
  struct format_specs : mp_units::detail::fill_align_width_format_specs<Char>, mp_units::unit_symbol_formatting {};
  format_specs specs_{};

  std::basic_string_view<Char> fill_align_width_format_str_;

  template<std::forward_iterator It>
  constexpr It parse_unit_specs(It begin, It end)
  {
    auto it = begin;
    if (it == end || *it == '}') return begin;

    constexpr auto valid_modifiers = std::string_view{"UA1ansd"};
    for (; it != end && *it != '}'; ++it) {
      if (valid_modifiers.find(*it) == std::string_view::npos)
        throw MP_UNITS_STD_FMT::format_error("invalid unit modifier specified");
    }
    end = it;

    if (it = mp_units::detail::at_most_one_of(begin, end, "UA"); it != end)
      specs_.encoding = (*it == 'U') ? mp_units::text_encoding::unicode : mp_units::text_encoding::ascii;
    if (it = mp_units::detail::at_most_one_of(begin, end, "1an"); it != end) {
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
    if (it = mp_units::detail::at_most_one_of(begin, end, "sd"); it != end) {
      if (*it == 'd' && specs_.encoding == mp_units::text_encoding::ascii)
        throw MP_UNITS_STD_FMT::format_error("half_high_dot unit separator allowed only for Unicode encoding");
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
    fill_align_width_format_str_ = {begin, it};
    if (it == end) return it;

    return parse_unit_specs(it, end);
  }

  template<typename FormatContext>
  constexpr auto format(const U& u, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0)
      // Avoid extra copying if width is not specified
      return mp_units::unit_symbol_to<Char>(ctx.out(), u, specs);
    std::basic_string<Char> unit_buffer;
    mp_units::unit_symbol_to<Char>(std::back_inserter(unit_buffer), u, specs);

    const std::basic_string<Char> global_format_buffer =
      "{:" + std::basic_string<Char>{fill_align_width_format_str_} + "}";
    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                        MP_UNITS_STD_FMT::make_format_args(unit_buffer));
  }
};


//
// Grammar
//
// quantity-format-spec        = [fill-and-align], [width], [quantity-specs], [defaults-specs];
// quantity-specs              = conversion-spec;
//                             | quantity-specs, conversion-spec;
//                             | quantity-specs, literal-char;
// literal-char                = ? any character other than '{', '}', or '%' ?;
// conversion-spec             = '%', placement-type;
// placement-type              = subentity-id | '?' | '%';
// defaults-specs              = ':', default-spec-list;
// default-spec-list           = default-spec;
//                             | default-spec-list, default-spec;
// default-spec                = subentity-id, '[' format-spec ']';
// subentity-id                = 'N' | 'U' | 'D';
// format-spec                 = ? as specified by the formatter for the argument type ?;
//
#if __cpp_lib_format_ranges && !MP_UNITS_USE_FMTLIB
template<auto Reference, typename Char, std::formattable<Char> Rep>
#else
template<auto Reference, typename Rep, typename Char>
#endif
class MP_UNITS_STD_FMT::formatter<mp_units::quantity<Reference, Rep>, Char> {
  static constexpr auto unit = get_unit(Reference);
  static constexpr auto dimension = get_quantity_spec(Reference).dimension;

  using quantity_t = mp_units::quantity<Reference, Rep>;
  using unit_t = std::remove_const_t<decltype(unit)>;
  using dimension_t = std::remove_const_t<decltype(dimension)>;
  using format_specs = mp_units::detail::fill_align_width_format_specs<Char>;

  format_specs specs_{};

  std::basic_string_view<Char> modifiers_format_str_;
  std::basic_string<Char> rep_format_str_ = "{}";
  std::basic_string<Char> unit_format_str_ = "{}";
  std::basic_string<Char> dimension_format_str_ = "{}";

  MP_UNITS_STD_FMT::formatter<Rep> rep_formatter_;
  MP_UNITS_STD_FMT::formatter<unit_t> unit_formatter_;
  MP_UNITS_STD_FMT::formatter<dimension_t> dimension_formatter_;

  struct format_checker {
    constexpr void on_number() const {}
    constexpr void on_maybe_space() const {}
    constexpr void on_unit() const {}
    constexpr void on_dimension() const {}
    template<std::forward_iterator It>
    constexpr void on_text(It, It) const
    {
    }
  };

  template<typename OutputIt>
  struct quantity_formatter {
    const formatter& f;
    OutputIt out;
    const quantity_t& q;
    std::locale locale;

    void on_number()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.rep_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
    }
    void on_maybe_space()
    {
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
    }
    void on_unit()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.unit_format_str_, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    void on_dimension()
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, f.dimension_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.dimension));
    }
    template<std::forward_iterator It>
    void on_text(It begin, It end) const
    {
      mp_units::detail::copy(begin, end, out);
    }
  };
  template<typename OutputIt, typename... Args>
  quantity_formatter(const formatter&, OutputIt, Args...) -> quantity_formatter<OutputIt>;

  template<std::forward_iterator It, typename Handler>
  constexpr const It parse_quantity_specs(It begin, It end, Handler& handler) const
  {
    if (begin == end || *begin == ':' || *begin == '}') return begin;
    if (*begin != '%')
      throw MP_UNITS_STD_FMT::format_error(
        "`quantity-specs` should start with a `conversion-spec` ('%' characters expected)");
    auto ptr = begin;
    while (ptr != end) {
      auto c = *ptr;
      if (c == '}') break;
      if (c == ':') {
        if (ptr + 1 != end && *(ptr + 1) == ':') {
          handler.on_text(begin, ++ptr);  // account for ':'
          ++ptr;                          // consume the second ':'
          continue;
        }
        // default specs started
        break;
      }
      if (c != '%') {
        ++ptr;
        continue;
      }
      if (begin != ptr) handler.on_text(begin, ptr);
      ++ptr;  // consume '%'
      if (ptr == end) throw MP_UNITS_STD_FMT::format_error("invalid `conversion-spec` format");

      c = *ptr++;
      switch (c) {
        case 'N':
          handler.on_number();
          break;
        case 'U':
          handler.on_unit();
          break;
        case 'D':
          handler.on_dimension();
          break;
        case '?':
          handler.on_maybe_space();
          break;
        case '%':
          handler.on_text(ptr - 1, ptr);
          break;
        default:
          throw MP_UNITS_STD_FMT::format_error(std::string("unknown `placement-type` token '") + c + "'");
      }
      begin = ptr;
    }
    if (begin != ptr) handler.on_text(begin, ptr);
    return ptr;
  }

  template<std::forward_iterator It, typename Formatter>
  constexpr It parse_default_spec(It begin, It end, Formatter& f, std::string& format_str)
  {
    if (begin == end || *begin != '[')
      throw MP_UNITS_STD_FMT::format_error("`default-spec` should contain a `[` character");
    auto it = ++begin;
    for (int nested_brackets = 0; it != end && !(*it == ']' && nested_brackets == 0); it++) {
      if (*it == '[') ++nested_brackets;
      if (*it == ']') {
        if (nested_brackets == 0) throw MP_UNITS_STD_FMT::format_error("unmatched ']' in format string");
        --nested_brackets;
      }
    }
    format_str = "{:" + std::string(begin, it) + '}';
    if (it == end) throw MP_UNITS_STD_FMT::format_error("unmatched '[' in format string");
    MP_UNITS_STD_FMT::basic_format_parse_context<Char> ctx(std::string_view(begin, it));
    auto ptr = f.parse(ctx);
    if (ptr != it) throw MP_UNITS_STD_FMT::format_error("invalid subentity format '" + std::string(begin, it) + "'");
    return ++it;  // skip `]`
  }

  template<std::forward_iterator It>
  [[nodiscard]] constexpr It parse_defaults_specs(It begin, It end)
  {
    if (begin == end || *begin == '}') return begin;
    if (*begin++ != ':') throw MP_UNITS_STD_FMT::format_error("`defaults-specs` should start with a `:`");
    do {
      auto c = *begin++;
      // TODO check if not repeated
      switch (c) {
        case 'N':
          begin = parse_default_spec(begin, end, rep_formatter_, rep_format_str_);
          break;
        case 'U':
          begin = parse_default_spec(begin, end, unit_formatter_, unit_format_str_);
          break;
        case 'D':
          begin = parse_default_spec(begin, end, dimension_formatter_, dimension_format_str_);
          break;
        default:
          throw MP_UNITS_STD_FMT::format_error(std::string("unknown `subentity-id` token '") + c + "'");
      }
    } while (begin != end && *begin != '}');
    return begin;
  }

  template<typename OutputIt, typename FormatContext>
  OutputIt format_quantity(OutputIt out, const quantity_t& q, FormatContext& ctx) const
  {
    const std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    if (modifiers_format_str_.empty()) {
      // default
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, rep_format_str_,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
      return MP_UNITS_STD_FMT::vformat_to(out, locale, unit_format_str_, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    // user provided format
    quantity_formatter f{*this, out, q, locale};
    parse_quantity_specs(modifiers_format_str_.data(), modifiers_format_str_.data() + modifiers_format_str_.size(), f);
    return f.out;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    auto begin = ctx.begin(), end = ctx.end();

    begin = parse_fill_align_width(ctx, begin, end, specs_, mp_units::detail::fmt_align::right);
    if (begin == end) return begin;

    const format_checker checker{};
    auto it = parse_quantity_specs(begin, end, checker);
    modifiers_format_str_ = {begin, it};

    return parse_defaults_specs(it, end);
  }

  template<typename FormatContext>
  auto format(const quantity_t& q, FormatContext& ctx) const
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0) {
      // Avoid extra copying if width is not specified
      format_quantity(ctx.out(), q, ctx);
      return ctx.out();
    }
    std::basic_string<Char> quantity_buffer;
    format_quantity(std::back_inserter(quantity_buffer), q, ctx);

    std::basic_string<Char> fill_align_width_format_str;
    mp_units::detail::format_global_buffer(std::back_inserter(fill_align_width_format_str), specs);
    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), fill_align_width_format_str,
                                        MP_UNITS_STD_FMT::make_format_args(quantity_buffer));
  }
};
