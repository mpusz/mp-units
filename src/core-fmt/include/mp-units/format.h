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

#include <mp-units/bits/algorithm.h>
#include <mp-units/bits/fmt.h>
#include <mp-units/customization_points.h>
#include <mp-units/quantity.h>
#include <mp-units/unit.h>
#include <cstdint>

// Grammar
//
// units-format-spec   ::=  [fill-and-align] [width] [units-specs]
// units-specs         ::=  conversion-spec
//                          units-specs conversion-spec
//                          units-specs literal-char
// literal-char        ::=  any character other than '{' or '}'
// conversion-spec     ::=  '%' units-type
// units-type          ::=  [units-rep-modifier] 'Q'
//                          [units-unit-modifier] 'q'
// units-rep-modifier  ::=  [sign] [#] [precision] [L] [units-rep-type]
// units-rep-type      ::=  one of "aAbBdeEfFgGoxX"
// units-unit-modifier ::=  [units-text-encoding, units-unit-symbol-denominator, units-unit-symbol-separator]
// units-text-encoding ::=  one of "UA"
// units-unit-symbol-solidus   ::=  one of "oan"
// units-unit-symbol-separator ::=  one of "sd"

namespace mp_units::detail {

// Holds specs about the whole object
template<typename CharT>
struct quantity_global_format_specs {
  fill_t<CharT> fill;
  fmt_align align = fmt_align::none;
  int width = 0;
  int dynamic_width_index = -1;
};

// Holds specs about the representation (%[specs]Q)
struct quantity_rep_format_specs {
  fmt_sign sign = fmt_sign::none;
  int precision = -1;
  int dynamic_precision_index = -1;
  char type = '\0';
  bool alt = false;
  bool localized = false;
};

// Holds specs about the unit (%[specs]q)
struct quantity_unit_format_specs : unit_symbol_formatting {};

template<typename CharT>
struct quantity_format_specs {
  quantity_global_format_specs<CharT> global;
  quantity_rep_format_specs rep;
  quantity_unit_format_specs unit;
};

// Parse a `units-rep-modifier`
template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
constexpr It parse_units_rep(It begin, S end, Handler&& handler, bool treat_as_floating_point)
{
  // parse sign
  begin = parse_sign(begin, end, handler);
  if (begin == end) return begin;

  // parse #
  if (*begin == '#') {
    handler.on_hash();
    if (++begin == end) return begin;
  }

  // parse precision if a floating point
  if (*begin == '.') {
    if (treat_as_floating_point) {
      begin = parse_precision(begin, end, handler);
    } else
      throw MP_UNITS_STD_FMT::format_error("precision not allowed for integral quantity representation");
    if (begin == end) return begin;
  }

  // parse L to enable the locale-specific form
  if (*begin == 'L') {
    handler.on_localized();
    ++begin;
  }

  if (begin != end && *begin != '}' && *begin != '%') {
    handler.on_type(*begin++);
  }
  return begin;
}

// parse units-specs
template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
constexpr It parse_units_format(It begin, S end, Handler&& handler)
{
  auto ptr = begin;
  while (ptr != end) {
    auto c = *ptr;
    if (c == '}') break;
    if (c != '%') {
      ++ptr;
      continue;
    }
    if (begin != ptr) handler.on_text(begin, ptr);
    begin = ++ptr;  // consume '%'
    if (ptr == end) throw MP_UNITS_STD_FMT::format_error("invalid format");
    c = *ptr++;

    constexpr auto units_types = std::string_view{"Qq"};
    const auto new_end = find_first_of(begin, end, units_types.begin(), units_types.end());
    if (new_end == end) throw MP_UNITS_STD_FMT::format_error("invalid format");
    if (*new_end == 'Q') {
      handler.on_quantity_value(begin, new_end);  // Edit `on_quantity_value` to add rep modifiers
    } else {
      handler.on_quantity_unit(begin, new_end);  // Edit `on_quantity_unit` to add an unit modifier
    }
    ptr = new_end + 1;
    begin = ptr;
  }
  if (begin != ptr) handler.on_text(begin, ptr);
  return ptr;
}

// build the 'representation' as requested in the format string, applying only units-rep-modifiers
template<typename CharT, typename Rep, typename OutputIt, typename Locale>
[[nodiscard]] OutputIt format_units_quantity_value(OutputIt out, const Rep& val,
                                                   const quantity_rep_format_specs& rep_specs, const Locale& loc)
{
  std::basic_string<CharT> buffer;
  auto to_buffer = std::back_inserter(buffer);

  MP_UNITS_STD_FMT::format_to(to_buffer, "{{:");
  switch (rep_specs.sign) {
    case fmt_sign::none:
      break;
    case fmt_sign::plus:
      MP_UNITS_STD_FMT::format_to(to_buffer, "+");
      break;
    case fmt_sign::minus:
      MP_UNITS_STD_FMT::format_to(to_buffer, "-");
      break;
    case fmt_sign::space:
      MP_UNITS_STD_FMT::format_to(to_buffer, " ");
      break;
  }

  if (rep_specs.alt) {
    MP_UNITS_STD_FMT::format_to(to_buffer, "#");
  }
  auto type = rep_specs.type;
  if (auto precision = rep_specs.precision; precision >= 0) {
    MP_UNITS_STD_FMT::format_to(to_buffer, ".{}{}", precision, type == '\0' ? 'f' : type);
  } else if constexpr (treat_as_floating_point<Rep>) {
    MP_UNITS_STD_FMT::format_to(to_buffer, "{}", type == '\0' ? 'g' : type);
  } else {
    if (type != '\0') {
      MP_UNITS_STD_FMT::format_to(to_buffer, "{}", type);
    }
  }
  if (rep_specs.localized) {
    MP_UNITS_STD_FMT::format_to(to_buffer, "L");
  }

  MP_UNITS_STD_FMT::format_to(to_buffer, "}}");
  if (rep_specs.localized) {
    return MP_UNITS_STD_FMT::vformat_to(out, MP_UNITS_FMT_LOCALE(loc), buffer, MP_UNITS_STD_FMT::make_format_args(val));
  }
  return MP_UNITS_STD_FMT::vformat_to(out, buffer, MP_UNITS_STD_FMT::make_format_args(val));
}

// Creates a global format string
//  e.g. "{:*^10%.1Q_%q}, 1.23_q_m" => "{:*^10}"
template<typename CharT, typename OutputIt>
OutputIt format_global_buffer(OutputIt out, const quantity_global_format_specs<CharT>& specs)
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

template<auto Reference, typename Rep, typename Locale, typename CharT, typename OutputIt>
struct quantity_formatter {
  OutputIt out;
  Rep val;
  const quantity_format_specs<CharT>& specs;
  Locale loc;

  explicit quantity_formatter(OutputIt o, quantity<Reference, Rep> q, const quantity_format_specs<CharT>& fspecs,
                              Locale lc) :
      out(o), val(std::move(q).numerical_value()), specs(fspecs), loc(std::move(lc))
  {
  }

  template<std::input_iterator It, std::sentinel_for<It> S>
  void on_text(It begin, S end)
  {
    std::copy(begin, end, out);
  }

  template<std::input_iterator It, std::sentinel_for<It> S>
  void on_quantity_value([[maybe_unused]] It, [[maybe_unused]] S)
  {
    out = format_units_quantity_value<CharT>(out, val, specs.rep, loc);
  }

  template<std::input_iterator It, std::sentinel_for<It> S>
  void on_quantity_unit(It, S)
  {
    out = unit_symbol_to<CharT>(out, get_unit(Reference), specs.unit);
  }
};

template<std::input_iterator It, std::sentinel_for<It> S>
[[nodiscard]] constexpr It at_most_one_of(It begin, S end, std::string_view modifiers)
{
  auto it = find_first_of(begin, end, modifiers.begin(), modifiers.end());
  if (it != end && find_first_of(it + 1, end, modifiers.begin(), modifiers.end()) != end)
    throw MP_UNITS_STD_FMT::format_error("only one of '" + std::string(modifiers) +
                                         "' unit modifiers may be used in the format spec");
  return it;
}

}  // namespace mp_units::detail

template<auto Reference, typename Rep, typename CharT>
struct MP_UNITS_STD_FMT::formatter<mp_units::quantity<Reference, Rep>, CharT> {
private:
  using quantity = mp_units::quantity<Reference, Rep>;
  using iterator = MP_UNITS_TYPENAME MP_UNITS_STD_FMT::basic_format_parse_context<CharT>::iterator;

  bool quantity_value = false;
  bool quantity_unit = false;
  mp_units::detail::quantity_format_specs<CharT> specs;
  std::basic_string_view<CharT> format_str;

  struct spec_handler {
    formatter& f;
    MP_UNITS_STD_FMT::basic_format_parse_context<CharT>& context;

    constexpr void on_fill(std::basic_string_view<CharT> fill) { f.specs.global.fill = fill; }
    constexpr void on_align(mp_units::detail::fmt_align align) { f.specs.global.align = align; }
    constexpr void on_width(int width) { f.specs.global.width = width; }
    constexpr void on_sign(mp_units::detail::fmt_sign sign) { f.specs.rep.sign = sign; }
    constexpr void on_hash() { f.specs.rep.alt = true; }
    constexpr void on_precision(int precision) { f.specs.rep.precision = precision; }
    constexpr void on_localized() { f.specs.rep.localized = true; }

    constexpr void on_type(char type)
    {
      constexpr auto valid_rep_types = std::string_view{"aAbBdeEfFgGoxX"};
      if (valid_rep_types.find(type) != std::string_view::npos) {
        f.specs.rep.type = type;
      } else {
        throw MP_UNITS_STD_FMT::format_error("invalid quantity type specifier");
      }
    }

    template<typename T>
    constexpr void on_dynamic_width(T t)
    {
      f.specs.global.dynamic_width_index = mp_units::detail::on_dynamic_arg(t, context);
    }

    template<typename T>
    constexpr void on_dynamic_precision(T t)
    {
      f.specs.rep.dynamic_precision_index = mp_units::detail::on_dynamic_arg(t, context);
    }

    template<std::input_iterator It, std::sentinel_for<It> S>
    constexpr void on_text(It, S)
    {
    }

    template<std::input_iterator It, std::sentinel_for<It> S>
    constexpr void on_quantity_value(It begin, S end)
    {
      if (begin != end) mp_units::detail::parse_units_rep(begin, end, *this, mp_units::treat_as_floating_point<Rep>);
      f.quantity_value = true;
    }

    template<std::input_iterator It, std::sentinel_for<It> S>
    constexpr void on_quantity_unit(It begin, S end)
    {
      if (begin == end) return;

      constexpr auto valid_modifiers = std::string_view{"UAoansd"};
      for (auto it = begin; it != end; ++it) {
        if (valid_modifiers.find(*it) == std::string_view::npos)
          throw MP_UNITS_STD_FMT::format_error("invalid unit modifier specified");
      }

      if (auto it = mp_units::detail::at_most_one_of(begin, end, "UA"); it != end) {
        if (*it == 'U')
          f.specs.unit.encoding = mp_units::text_encoding::unicode;
        else
          f.specs.unit.encoding = mp_units::text_encoding::ascii;
      }

      if (auto it = mp_units::detail::at_most_one_of(begin, end, "oan"); it != end) {
        if (*it == 'o')
          f.specs.unit.solidus = mp_units::unit_symbol_solidus::one_denominator;
        else if (*it == 'a')
          f.specs.unit.solidus = mp_units::unit_symbol_solidus::always;
        else
          f.specs.unit.solidus = mp_units::unit_symbol_solidus::never;
      }

      if (auto it = mp_units::detail::at_most_one_of(begin, end, "sd"); it != end) {
        if (*it == 's')
          f.specs.unit.separator = mp_units::unit_symbol_separator::space;
        else {
          if (f.specs.unit.encoding == mp_units::text_encoding::ascii)
            throw MP_UNITS_STD_FMT::format_error("half_high_dot unit separator allowed only for Unicode encoding");
          f.specs.unit.separator = mp_units::unit_symbol_separator::half_high_dot;
        }
      }

      f.quantity_unit = true;
    }
  };

  [[nodiscard]] constexpr std::pair<iterator, iterator> do_parse(
    MP_UNITS_STD_FMT::basic_format_parse_context<CharT>& ctx)
  {
    auto begin = ctx.begin();
    auto end = ctx.end();

    if (begin == end || *begin == '}') return {begin, begin};

    // handler to assign parsed data to formatter data members
    spec_handler handler{*this, ctx};

    // parse alignment
    begin = mp_units::detail::parse_align(begin, end, handler);
    if (begin == end) return {begin, begin};

    // parse width
    begin = mp_units::detail::parse_width(begin, end, handler);
    if (begin == end) return {begin, begin};

    // parse units-specific specification
    end = mp_units::detail::parse_units_format(begin, end, handler);

    if (specs.global.align == mp_units::detail::fmt_align::none && (!quantity_unit || quantity_value))
      // quantity values should behave like numbers (by default aligned to right)
      specs.global.align = mp_units::detail::fmt_align::right;

    return {begin, end};
  }

  template<typename OutputIt, typename FormatContext>
  OutputIt format_quantity_content(OutputIt out, const quantity& q, FormatContext& ctx)
  {
    auto begin = format_str.begin();
    auto end = format_str.end();

    if (begin == end || *begin == '}') {
      // default format should print value followed by the unit separated with 1 space
      out = mp_units::detail::format_units_quantity_value<CharT>(out, q.numerical_value(), specs.rep, ctx.locale());
      if constexpr (mp_units::detail::has_unit_symbol(get_unit(Reference))) {
        if constexpr (mp_units::space_before_unit_symbol<get_unit(Reference)>) *out++ = CharT(' ');
        out = unit_symbol_to<CharT>(out, get_unit(Reference));
      }
    } else {
      // user provided format
      mp_units::detail::quantity_formatter f(out, q, specs, ctx.locale());
      mp_units::detail::parse_units_format(begin, end, f);
    }
    return out;
  }

public:
  [[nodiscard]] constexpr auto parse(MP_UNITS_STD_FMT::basic_format_parse_context<CharT>& ctx)
  {
    auto range = do_parse(ctx);
    if (range.first != range.second)
      format_str = std::basic_string_view<CharT>(&*range.first, static_cast<size_t>(range.second - range.first));
    return range.second;
  }

  template<typename FormatContext>
  auto format(const quantity& q, FormatContext& ctx)
  {
    // process dynamic width and precision
    if (specs.global.dynamic_width_index >= 0)
      specs.global.width =
        mp_units::detail::get_dynamic_spec<mp_units::detail::width_checker>(specs.global.dynamic_width_index, ctx);
    if (specs.rep.dynamic_precision_index >= 0)
      specs.rep.precision =
        mp_units::detail::get_dynamic_spec<mp_units::detail::precision_checker>(specs.rep.dynamic_precision_index, ctx);

    if (specs.global.width == 0) {
      // Avoid extra copying if width is not specified
      return format_quantity_content(ctx.out(), q, ctx);
    } else {
      // In `quantity_buffer` we will have the representation and the unit formatted according to their
      //  specification, ignoring global specifiers
      //  e.g. "{:*^10%.1Q_%q}, 1.23_q_m" => "1.2_m"
      std::basic_string<CharT> quantity_buffer;

      // deal with quantity content
      format_quantity_content(std::back_inserter(quantity_buffer), q, ctx);

      // In `global_format_buffer` we will create a global format string
      //  e.g. "{:*^10%.1Q_%q}, 1.23_q_m" => "{:*^10}"
      std::basic_string<CharT> global_format_buffer;
      mp_units::detail::format_global_buffer<CharT>(std::back_inserter(global_format_buffer), specs.global);

      // Format the `quantity buffer` using specs from `global_format_buffer`
      // In the example, equivalent to MP_UNITS_STD_FMT::format("{:*^10}", "1.2_m")
      return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                          MP_UNITS_STD_FMT::make_format_args(quantity_buffer));
    }
  }
};
