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

#include <units/customization_points.h>
#include <units/quantity.h>
#include <fmt/format.h>
#include <string_view>

namespace units {

  namespace detail {

    // units-format-spec:
    //      fill-and-align[opt] sign[opt] width[opt] precision[opt] type[opt] units-specs[opt]
    // units-specs:
    //      conversion-spec
    //      units-specs conversion-spec
    //      units-specs literal-char
    // literal-char:
    //      any character other than { or }
    // conversion-spec:
    //      % modifier[opt] units-type
    // modifier:
    //      A
    // units-type: one of
    //      n q Q t %

    template<typename CharT, typename Handler>
    constexpr const CharT* parse_units_format(const CharT* begin, const CharT* end, Handler&& handler)
    {
      auto ptr = begin;
      while(ptr != end) {
        auto c = *ptr;
        if(c == '}')
          break;
        if(c != '%') {
          ++ptr;
          continue;
        }
        if(begin != ptr)
          handler.on_text(begin, ptr);
        ++ptr;  // consume '%'
        if(ptr == end)
          throw fmt::format_error("invalid format");
        c = *ptr++;
        switch(c) {
        case '%':
          handler.on_text(ptr - 1, ptr);
          break;
        case 'n': {
          const char newline[] = "\n";
          handler.on_text(newline, newline + 1);
          break;
        }
        case 't': {
          const char tab[] = "\t";
          handler.on_text(tab, tab + 1);
          break;
        }
        case 'Q':
          handler.on_quantity_value();
          break;
        case 'q':
          handler.on_quantity_unit();
          break;
        default:
          throw fmt::format_error("invalid format");
        }
        begin = ptr;
      }
      if(begin != ptr)
        handler.on_text(begin, ptr);
      return ptr;
    }

    template<typename Rep, typename OutputIt>
    inline OutputIt format_units_quantity_value(OutputIt out, const Rep& val, fmt::sign_t sign, int precision, char type)
    {
      std::string sign_text;
      switch(sign) {
        case fmt::sign::none:
          break;
        case fmt::sign::plus:
          sign_text = "+";
          break;
        case fmt::sign::minus:
          sign_text = "-";
          break;
        case fmt::sign::space:
          sign_text = " ";
          break;
      }
      if(precision >= 0) {
        if (type == '\0') type = 'f';
        return format_to(out, "{:" + sign_text + ".{}" + type + "}", val, precision);
      }
      if constexpr (treat_as_floating_point<Rep>) {
        if (type == '\0') type = 'g';
        return format_to(out, "{:" + sign_text + type + "}", val);
      }
      else {
        if (type == '\0') {
          return format_to(out, "{:" + sign_text + "}", val);
        }
        return format_to(out, "{:" + sign_text + type + "}", val);
      }
    }

    template<typename OutputIt, typename Dimension, typename Unit, typename Rep>
    struct units_formatter {
      OutputIt out;
      Rep val;
      fmt::sign_t sign;
      int precision;
      char type;

      explicit units_formatter(OutputIt o, quantity<Dimension, Unit, Rep> q, fmt::sign_t s, int prec, char tp):
        out(o), val(q.count()), sign(s), precision(prec), type(tp)
      {
      }

      template<typename CharT>
      void on_text(const CharT* begin, const CharT* end)
      {
        std::copy(begin, end, out);
      }

      void on_quantity_value()
      {
        out = format_units_quantity_value(out, val, sign, precision, type);
      }

      void on_quantity_unit()
      {
        format_to(out, "{}", unit_text<Dimension, Unit>().c_str());
      }
    };

  }  // namespace detail

}  // namespace units

template<typename Dimension, typename Unit, typename Rep, typename CharT>
struct fmt::formatter<units::quantity<Dimension, Unit, Rep>, CharT> {
private:
  using quantity = units::quantity<Dimension, Unit, Rep>;
  using iterator = fmt::basic_format_parse_context<CharT>::iterator;
  using arg_ref_type = fmt::internal::arg_ref<CharT>;

  fmt::basic_format_specs<CharT> specs;
  int precision = -1;
  char type = '\0';
  bool quantity_value = false;
  bool quantity_unit = false;
  arg_ref_type width_ref;
  arg_ref_type precision_ref;
  fmt::basic_string_view<CharT> format_str;

  struct spec_handler {
    formatter& f;
    fmt::basic_format_parse_context<CharT>& context;
    fmt::basic_string_view<CharT> format_str;

    template<typename Id>
    constexpr arg_ref_type make_arg_ref(Id arg_id)
    {
      context.check_arg_id(arg_id);
      return arg_ref_type(arg_id);
    }

    constexpr arg_ref_type make_arg_ref(fmt::basic_string_view<CharT> arg_id)
    {
      context.check_arg_id(arg_id);
      return arg_ref_type(arg_id);
    }

    constexpr arg_ref_type make_arg_ref(fmt::internal::auto_id)
    {
      return arg_ref_type(context.next_arg_id());
    }

    void on_error(const char* msg) { throw fmt::format_error(msg); }
    constexpr void on_fill(CharT fill) { f.specs.fill[0] = fill; }
    constexpr void on_plus() { f.specs.sign = fmt::sign::plus; }
    constexpr void on_minus() { f.specs.sign = fmt::sign::minus; }
    constexpr void on_space() { f.specs.sign = fmt::sign::space; }
    constexpr void on_align(align_t align) { f.specs.align = align; }
    constexpr void on_width(int width) { f.specs.width = width; }
    constexpr void on_precision(int precision) { f.precision = precision; }
    constexpr void on_type(char type)
    {
        constexpr auto good_types = std::string_view{"aAbBcdeEfFgGopsxX"};
        if (good_types.find(type) != std::string_view::npos) {
            f.type = type;
        }
    }
    constexpr void end_precision() {}

    template<typename Id>
    constexpr void on_dynamic_width(Id arg_id)
    {
      f.width_ref = make_arg_ref(arg_id);
    }

    template<typename Id>
    constexpr void on_dynamic_precision(Id arg_id)
    {
      f.precision_ref = make_arg_ref(arg_id);
    }

    constexpr void on_text(const CharT*, const CharT*) {}
    constexpr void on_quantity_value() { f.quantity_value = true; }
    constexpr void on_quantity_unit() { f.quantity_unit = true; }
  };

  struct parse_range {
    iterator begin;
    iterator end;
  };

  constexpr parse_range do_parse(fmt::basic_format_parse_context<CharT>& ctx)
  {
    auto begin = ctx.begin(), end = ctx.end();
    if(begin == end || *begin == '}')
      return {begin, begin};

    // handler to assign parsed data to formatter data members
    spec_handler handler{*this, ctx, format_str};

    // parse alignment
    begin = fmt::internal::parse_align(begin, end, handler);
    if(begin == end)
      return {begin, begin};

    // parse sign
    switch(static_cast<char>(*begin)) {
    case '+':
      handler.on_plus();
      ++begin;
      break;
    case '-':
      handler.on_minus();
      ++begin;
      break;
    case ' ':
      handler.on_space();
      ++begin;
      break;
    }
    if(begin == end)
      return {begin, begin};

    // parse width
    begin = fmt::internal::parse_width(begin, end, handler);
    if(begin == end)
      return {begin, begin};

    // parse precision if a floating point
    if(*begin == '.') {
      if constexpr(units::treat_as_floating_point<Rep>)
        begin = fmt::internal::parse_precision(begin, end, handler);
      else
        handler.on_error("precision not allowed for integral quantity representation");
    }

    if(*begin != '}' && *begin != '%') {
        handler.on_type(*begin++);
    }

    // parse units-specific specification
    end = units::detail::parse_units_format(begin, end, handler);

    if(specs.align == fmt::align_t::none && (!quantity_unit || quantity_value))
      // quantity values should behave like numbers (by default aligned to right)
      specs.align = fmt::align_t::right;

    if((quantity_unit && !quantity_value) && (specs.sign == fmt::sign::plus || specs.sign == fmt::sign::minus))
      handler.on_error("sign not allowed for a quantity unit");

    return {begin, end};
  }

public:
  constexpr auto parse(fmt::basic_format_parse_context<CharT>& ctx)
  {
    auto range = do_parse(ctx);
    format_str = fmt::basic_string_view<CharT>(&*range.begin, fmt::internal::to_unsigned(range.end - range.begin));
    return range.end;
  }

  template<typename FormatContext>
  auto format(const units::quantity<Dimension, Unit, Rep>& q, FormatContext& ctx)
  {
    auto begin = format_str.begin(), end = format_str.end();

    // TODO Avoid extra copying if width is not specified
    fmt::basic_memory_buffer<CharT> buf;
    auto out = std::back_inserter(buf);

    // process dynamic width and precision
    fmt::internal::handle_dynamic_spec<fmt::internal::width_checker>(specs.width, width_ref, ctx);
    fmt::internal::handle_dynamic_spec<fmt::internal::precision_checker>(precision, precision_ref, ctx);

    // deal with quantity content
    if(begin == end || *begin == '}') {
      // default format should print value followed by the unit separated with 1 space
      out = units::detail::format_units_quantity_value(out, q.count(), specs.sign, precision, type);
      constexpr auto symbol = units::detail::unit_text<Dimension, Unit>();
      if(symbol.size()) {
        *out++ = CharT(' ');
        format_to(out, "{}", symbol.c_str());
      }
    }
    else {
      // user provided format
      units::detail::units_formatter f(out, q, specs.sign, precision, type);
      parse_units_format(begin, end, f);
    }

    // form a final text
    using range = fmt::internal::output_range<decltype(ctx.out()), CharT>;
    fmt::internal::basic_writer<range> w(range(ctx.out()));
    w.write(buf.data(), buf.size(), specs);

    return w.out();
  }
};
