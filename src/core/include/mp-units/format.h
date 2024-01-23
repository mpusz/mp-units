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

#include <mp-units/bits/external/algorithm.h>
#include <mp-units/bits/fmt.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>
#include <mp-units/unit.h>
#include <cstdint>


namespace mp_units::detail {

template<typename Char>
struct fill_align_width_format_specs {
  fill_t<Char> fill;
  fmt_align align : 4 = fmt_align::none;
  int width = 0;
  fmt_arg_ref<Char> width_ref;
};

template<typename OutputIt, typename Char>
OutputIt format_global_buffer(OutputIt out, const fill_align_width_format_specs<Char>& specs)
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

template<typename Char>
[[nodiscard]] constexpr const Char* at_most_one_of(const Char* begin, const Char* end, std::string_view modifiers)
{
  auto it = find_first_of(begin, end, modifiers.begin(), modifiers.end());
  if (it != end && find_first_of(it + 1, end, modifiers.begin(), modifiers.end()) != end)
    throw MP_UNITS_STD_FMT::format_error("only one of '" + std::string(modifiers) +
                                         "' unit modifiers may be used in the format spec");
  return it;
}

}  // namespace mp_units::detail

//
// Grammar
//
// dimension-format-spec       ::=  [fill-and-align] [width] [dimension-spec]
// dimension-spec              ::=  [text-encoding]
// text-encoding               ::=  one of
//                                  U A

// template<typename Char>
// struct dimension_format_specs : fill_align_width_format_specs<Char>, dimension_symbol_formatting {};


//
// Grammar
//
// unit-format-spec            ::=  [fill-and-align] [width] [unit-spec]
// unit-spec                   ::=  [text-encoding] [unit-symbol-solidus] [unit-symbol-separator] [L]
//                                  [text-encoding] [unit-symbol-separator] [unit-symbol-solidus] [L]
//                                  [unit-symbol-solidus] [text-encoding] [unit-symbol-separator] [L]
//                                  [unit-symbol-solidus] [unit-symbol-separator] [text-encoding] [L]
//                                  [unit-symbol-separator] [text-encoding] [unit-symbol-solidus] [L]
//                                  [unit-symbol-separator] [unit-symbol-solidus] [text-encoding] [L]
// unit-symbol-solidus         ::=  one of
//                                  o a n
// unit-symbol-separator       ::=  one of
//                                  s d
template<mp_units::Unit U, typename Char>
class MP_UNITS_STD_FMT::formatter<U, Char> {
  struct format_specs : mp_units::detail::fill_align_width_format_specs<Char>, mp_units::unit_symbol_formatting {};

  std::basic_string_view<Char> fill_align_width_format_str_;
  std::basic_string_view<Char> modifiers_format_str_;
  format_specs specs_{};

  struct format_checker {
    using enum mp_units::text_encoding;

    mp_units::text_encoding encoding = unicode;

    constexpr void on_text_encoding(Char val) { encoding = (val == 'U') ? unicode : ascii; }
    constexpr void on_unit_symbol_solidus(Char) const {}
    constexpr void on_unit_symbol_separator(Char val) const
    {
      if (val == 'd' && encoding == ascii)
        throw MP_UNITS_STD_FMT::format_error("half_high_dot unit separator allowed only for Unicode encoding");
    }
  };

  struct unit_formatter {
    format_specs specs;

    using enum mp_units::text_encoding;
    using enum mp_units::unit_symbol_solidus;
    using enum mp_units::unit_symbol_separator;

    constexpr void on_text_encoding(Char val) { specs.encoding = (val == 'U') ? unicode : ascii; }
    constexpr void on_unit_symbol_solidus(Char val)
    {
      switch (val) {
        case 'o':
          specs.solidus = one_denominator;
          break;
        case 'a':
          specs.solidus = always;
          break;
        case 'n':
          specs.solidus = never;
          break;
      }
    }
    constexpr void on_unit_symbol_separator(Char val) { specs.separator = (val == 's') ? space : half_high_dot; }
  };

  template<typename Handler>
  constexpr const Char* parse_unit_specs(const Char* begin, const Char* end, Handler&& handler) const
  {
    auto it = begin;
    if (it == end || *it == '}') return begin;

    constexpr auto valid_modifiers = std::string_view{"UAoansd"};
    for (; it != end && *it != '}'; ++it) {
      if (valid_modifiers.find(*it) == std::string_view::npos)
        throw MP_UNITS_STD_FMT::format_error("invalid unit modifier specified");
    }
    end = it;

    if (it = mp_units::detail::at_most_one_of(begin, end, "UA"); it != end) handler.on_text_encoding(*it);
    if (it = mp_units::detail::at_most_one_of(begin, end, "oan"); it != end) handler.on_unit_symbol_solidus(*it);
    if (it = mp_units::detail::at_most_one_of(begin, end, "sd"); it != end) handler.on_unit_symbol_separator(*it);
    return end;
  }

public:
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    const auto begin = ctx.begin();
    auto it = begin, end = ctx.end();
    if (it == end || *it == '}') return it;

    it = mp_units::detail::parse_align(it, end, specs_);
    if (it == end) return it;

    it = mp_units::detail::parse_dynamic_spec(it, end, specs_.width, specs_.width_ref, ctx);
    if (it == end) return it;

    fill_align_width_format_str_ = {begin, it};

    format_checker checker;
    end = parse_unit_specs(it, end, checker);
    modifiers_format_str_ = {it, end};
    return end;
  }

  template<typename FormatContext>
  auto format(const U& u, FormatContext& ctx) const -> decltype(ctx.out())
  {
    unit_formatter f{specs_};
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(f.specs.width, f.specs.width_ref, ctx);

    parse_unit_specs(modifiers_format_str_.begin(), modifiers_format_str_.end(), f);

    if (f.specs.width == 0) {
      // Avoid extra copying if width is not specified
      return mp_units::unit_symbol_to<Char>(ctx.out(), u, f.specs);
    } else {
      std::basic_string<Char> unit_buffer;
      mp_units::unit_symbol_to<Char>(std::back_inserter(unit_buffer), u, f.specs);

      std::basic_string<Char> global_format_buffer = "{:" + std::basic_string<Char>{fill_align_width_format_str_} + "}";
      return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                          MP_UNITS_STD_FMT::make_format_args(unit_buffer));
    }
  }
};


//
// Grammar
//
// quantity-format-spec        ::=  [fill-and-align] [width] [quantity-specs]
// quantity-specs              ::=  conversion-spec
//                                  quantity-specs conversion-spec
//                                  quantity-specs literal-char
// literal-char                ::=  any character other than '{', '}', or '%'
// conversion-spec             ::=  placement-spec
//                                  subentity-replacement-field
// placement-spec              ::=  '%' placement-type
// placement-type              ::=  one of
//                                  N U D ? %
// subentity-replacement-field ::= { % subentity-id [format-specifier] }
// subentity-id                ::= any character other than {, }, or %
// format-specifier            ::= : format-spec
// format-spec                 ::= as specified by the formatter for the argument type; cannot start with }
//
template<auto Reference, typename Rep, typename Char>
class MP_UNITS_STD_FMT::formatter<mp_units::quantity<Reference, Rep>, Char> {
  static constexpr auto unit = get_unit(Reference);
  static constexpr auto dimension = get_quantity_spec(Reference).dimension;

  using quantity_t = mp_units::quantity<Reference, Rep>;
  using unit_t = std::remove_const_t<decltype(unit)>;
  using dimension_t = std::remove_const_t<decltype(dimension)>;

  using format_specs = mp_units::detail::fill_align_width_format_specs<Char>;

  std::basic_string_view<Char> modifiers_format_str_;
  std::vector<size_t> format_str_lengths_;
  format_specs specs_{};

  struct format_checker {
    MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx;
    std::vector<size_t>& format_str_lengths;

    constexpr void on_number(std::basic_string_view<Char>) const {}
    constexpr void on_maybe_space() const {}
    constexpr void on_unit(std::basic_string_view<Char>) const {}
    constexpr void on_dimension(std::basic_string_view<Char>) const {}
    constexpr void on_text(const Char*, const Char*) const {}

    constexpr const Char* on_replacement_field(std::basic_string_view<Char> id, const Char* begin)
    {
      if (id == "N")
        return on_replacement_field<Rep>(begin);
      else if (id == "U")
        return on_replacement_field<unit_t>(begin);
      else if (id == "D") {
        return begin;
        // on_replacement_field<dimension_t>(begin);
      } else
        throw MP_UNITS_STD_FMT::format_error("unknown replacement field '" + std::string(id) + "'");
    }

  private:
    template<typename T>
    constexpr const Char* on_replacement_field(const Char* begin) const
    {
      MP_UNITS_STD_FMT::formatter<T> sf;
      ctx.advance_to(begin);
      auto ptr = sf.parse(ctx);
      if (*ptr != '}') throw MP_UNITS_STD_FMT::format_error("unmatched '}' in format string");
      format_str_lengths.push_back(mp_units::detail::to_unsigned(ptr - begin));
      return ptr;
    }
  };

  template<typename OutputIt>
  struct quantity_formatter {
    OutputIt out;
    const quantity_t& q;
    std::vector<size_t>::const_iterator format_str_lengths_it;
    std::locale locale;

    void on_number(std::basic_string_view<Char> format_str)
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, format_str,
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
    }
    void on_maybe_space()
    {
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
    }
    void on_unit(std::basic_string_view<Char> format_str)
    {
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, format_str, MP_UNITS_STD_FMT::make_format_args(q.unit));
    }
    void on_dimension(std::basic_string_view<Char>) {}
    void on_text(const Char* begin, const Char* end) const { std::copy(begin, end, out); }

    constexpr const Char* on_replacement_field(std::basic_string_view<Char> id, const Char* begin)
    {
      auto format_str = [&] { return "{:" + std::string(begin, *format_str_lengths_it + 1); };
      if (id == "N")
        on_number(format_str());
      else if (id == "U")
        on_unit(format_str());
      else if (id == "D")
        on_dimension(format_str());
      else
        throw MP_UNITS_STD_FMT::format_error("unknown replacement field '" + std::string(id) + "'");
      return begin + *format_str_lengths_it++;
    }
  };
  template<typename OutputIt, typename... Args>
  quantity_formatter(OutputIt, Args...) -> quantity_formatter<OutputIt>;

  template<typename Handler>
  constexpr const Char* parse_quantity_specs(const Char* begin, const Char* end, Handler&& handler) const
  {
    if (begin == end || *begin == '}') return begin;
    if (*begin != '%' && *begin != '{')
      throw MP_UNITS_STD_FMT::format_error(
        "`quantity-specs` should start with a `conversion-spec` ('%' or '{' characters expected)})");
    auto ptr = begin;
    while (ptr != end) {
      auto c = *ptr;
      if (c == '}') break;
      if (c == '{') {
        if (begin != ptr) handler.on_text(begin, ptr);
        begin = ptr = mp_units::detail::parse_subentity_replacement_field(ptr, end, handler);
        continue;
      }
      if (c != '%') {
        ++ptr;
        continue;
      }
      if (begin != ptr) handler.on_text(begin, ptr);
      ++ptr;  // consume '%'
      if (ptr == end) throw MP_UNITS_STD_FMT::format_error("invalid `placement-spec` format");

      c = *ptr++;
      switch (c) {
        case 'N':
          handler.on_number("{}");
          break;
        case 'U':
          handler.on_unit("{}");
          break;
        case 'D':
          handler.on_dimension("{}");
          break;
        case '?':
          handler.on_maybe_space();
          break;
        case '%':
          handler.on_text(ptr - 1, ptr);
          break;
        default:
          throw MP_UNITS_STD_FMT::format_error(std::string("unknown `placement-spec` token '") + c + "'");
      }
      begin = ptr;
    }
    if (begin != ptr) handler.on_text(begin, ptr);
    return ptr;
  }

  template<typename OutputIt, typename FormatContext>
  OutputIt format_quantity(OutputIt out, const quantity_t& q, FormatContext& ctx) const
  {
    std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    if (modifiers_format_str_.empty()) {
      // default format should print value followed by the unit separated with 1 space
      out = MP_UNITS_STD_FMT::vformat_to(out, locale, "{}",
                                         MP_UNITS_STD_FMT::make_format_args(q.numerical_value_ref_in(q.unit)));
      if constexpr (mp_units::space_before_unit_symbol<unit>) *out++ = ' ';
      return MP_UNITS_STD_FMT::vformat_to(out, locale, "{}", MP_UNITS_STD_FMT::make_format_args(q.unit));
    } else {
      // user provided format
      quantity_formatter f{out, q, format_str_lengths_.cbegin(), locale};
      parse_quantity_specs(modifiers_format_str_.begin(), modifiers_format_str_.end(), f);
      return f.out;
    }
  }

public:
  // parse quantity-format-specs
  constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx) -> decltype(ctx.begin())
  {
    const auto begin = ctx.begin();
    auto it = begin, end = ctx.end();
    if (it == end || *it == '}') return it;

    it = mp_units::detail::parse_align(it, end, specs_, mp_units::detail::fmt_align::right);
    if (it == end) return it;

    it = mp_units::detail::parse_dynamic_spec(it, end, specs_.width, specs_.width_ref, ctx);
    if (it == end) return it;

    format_checker checker(ctx, format_str_lengths_);
    end = parse_quantity_specs(it, end, checker);
    modifiers_format_str_ = {it, end};
    return end;
  }

  template<typename FormatContext>
  auto format(const quantity_t& q, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0) {
      // Avoid extra copying if width is not specified
      return format_quantity(ctx.out(), q, ctx);
    } else {
      std::basic_string<Char> quantity_buffer;
      format_quantity(std::back_inserter(quantity_buffer), q, ctx);

      std::basic_string<Char> fill_align_width_format_str;
      mp_units::detail::format_global_buffer(std::back_inserter(fill_align_width_format_str), specs);
      return MP_UNITS_STD_FMT::vformat_to(ctx.out(), fill_align_width_format_str,
                                          MP_UNITS_STD_FMT::make_format_args(quantity_buffer));
    }
  }
};
