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
//                          one of "nt%"
// units-rep-modifier  ::=  [sign] [#] [precision] [units-rep-type]
// units-rep-type      ::=  one of "aAbBdeEfFgGoxX"
// units-unit-modifier ::=  'A'

// Guide for editing
//
// If you want to add a new `units-type` terminal character (e.g. 'Q', 'q'):
// -   If needed, write a new `specs` class (e.g. `global_format_specs`)
// -   Add the new symbol in the `units_types` variable in the `parse_units_format` function
// -   Add a new case in the `if` following the format_error in `parse_units_format` function;
//     this should invoke `handler.on_[...]`
// -   Edit `fmt::formatter`:
//     - Add a new field for the flag/specs
//     - Add to the `fmt::formatter::spec_handler` a `on_[...]` function that set the flag/specs if needed
// -   Edit `units_formatter`:
//     - Add a new field for the flag/specs
//     - write a `on_[...]` function that writes to the `out` iterator the correct output
// 
// If you want to add a new `units-rep-type`:
// -   Add the new symbol in the `valid_rep_types` variable (which is in the
//         fmt::formatter::spec_handler::on_type member function)
//     NB: currently this function forward the modifier to the value that must be formatted;
//         if the symbol has no meaning for fmt::formatter<Rep>, this behavior should be disabled manually
//         (as is done for '\0')
// -   Implement the effect of the new flag in `format_units_quantity_value`
// 
// If you want to add a new `units-unit-modifier`:
// -   Add the new symbol in the `valid_modifiers` variable (which is in the
//         fmt::formatter::spec_handler::on_modifier member function)
// -   Implement the effect of the new flag in the `units_formatter::on_quantity_unit` member function

namespace units {

  namespace detail {

    // Holds specs about the whole object
    template <typename CharT>
    struct global_format_specs
    {
      fmt::internal::fill_t<CharT> fill = fmt::internal::fill_t<CharT>::make();
      fmt::align_t align = fmt::align_t::none;
      int width = 0;
    };

    // Holds specs about the representation (%[specs]Q)
    struct rep_format_specs
    {
      fmt::sign_t sign = fmt::sign_t::none;
      bool alt = false;
      int precision = -1;
      char type = '\0';
    };

    // Holds specs about the unit (%[specs]q)
    struct unit_format_specs
    {
      char modifier = '\0';
    };

    // Parse a `units-rep-modifier`
    template <typename CharT, typename Handler>
    constexpr const CharT* parse_units_rep(const CharT* begin, const CharT* end, Handler&& handler, bool treat_as_floating_point)
    {
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
        return begin;

      // parse #
      if (*begin == '#') {
        handler.on_alt();
        if (++begin == end) return begin;
      }

      // parse precision if a floating point
      if(*begin == '.') {
        if (treat_as_floating_point)
          begin = fmt::internal::parse_precision(begin, end, handler);
        else
          handler.on_error("precision not allowed for integral quantity representation");
      }

      if(begin != end && *begin != '}' && *begin != '%') {
        handler.on_type(*begin++);
      }
      return begin;
    }

    // parse units-specs
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
        begin = ++ptr;  // consume '%'
        if(ptr == end)
          throw fmt::format_error("invalid format");
        c = *ptr++;

        switch(c) {
        // units-type
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
        default:
          constexpr auto units_types = std::string_view{"Qq"};
          auto const new_end = std::find_first_of(begin, end, units_types.begin(), units_types.end());
          if (new_end == end) {
            throw fmt::format_error("invalid format");
          }
          if (*new_end == 'Q') {
            handler.on_quantity_value(begin, new_end); // Edit `on_quantity_value` to add rep modifiers
          } else {
            handler.on_quantity_unit(*begin);          // Edit `on_quantity_unit` to add an unit modifier
          }
          ptr = new_end + 1;
        }
        begin = ptr;
      }
      if(begin != ptr)
        handler.on_text(begin, ptr);
      return ptr;
    }

    // build the 'representation' as requested in the format string, applying only units-rep-modifiers
    template<typename CharT, typename Rep, typename OutputIt>
    inline OutputIt format_units_quantity_value(OutputIt out, const Rep& val, const rep_format_specs& rep_specs)
    {
      fmt::basic_memory_buffer<CharT> buffer;
      auto to_buffer = std::back_inserter(buffer);

      fmt::format_to(to_buffer, "{{:");
      switch(rep_specs.sign) {
      case fmt::sign::none:
        break;
      case fmt::sign::plus:
        format_to(to_buffer, "+");
        break;
      case fmt::sign::minus:
        format_to(to_buffer, "-");
        break;
      case fmt::sign::space:
        format_to(to_buffer, " ");
        break;
      }

      if (rep_specs.alt) {
        format_to(to_buffer, "#");
      }
      auto type = rep_specs.type;
      if (auto precision = rep_specs.precision; precision >= 0) {
        format_to(to_buffer, ".{}{}", precision, type == '\0' ? 'f' : type);
      } else if constexpr (treat_as_floating_point<Rep>) {
        format_to(to_buffer, "{}", type == '\0' ? 'g' : type);
      } else {
        if (type != '\0') {
          format_to(to_buffer, "{}", type);
        }
      }
      fmt::format_to(to_buffer, "}}");
      return format_to(out, fmt::to_string(buffer), val);
    }

    template<typename OutputIt, typename in_dimension, typename in_unit, typename Rep, typename CharT>
    struct units_formatter {
      OutputIt out;
      Rep val;
      global_format_specs<CharT> const & global_specs;
      rep_format_specs const & rep_specs;
      unit_format_specs const & unit_specs;

      explicit units_formatter(
        OutputIt o, quantity<in_dimension, in_unit, Rep> q,
        global_format_specs<CharT> const & gspecs,
        rep_format_specs const & rspecs, unit_format_specs const & uspecs
      ):
        out(o), val(q.count()), global_specs(gspecs), rep_specs(rspecs), unit_specs(uspecs)
      {
      }

      template<typename CharT2>
      void on_text(const CharT2* begin, const CharT2* end)
      {
        std::copy(begin, end, out);
      }

      void on_quantity_value([[maybe_unused]] const CharT*, [[maybe_unused]] const CharT*)
      {
        out = format_units_quantity_value<CharT>(out, val, rep_specs);
      }

      void on_quantity_unit([[maybe_unused]] const CharT)
      {
        auto txt = unit_text<in_dimension, in_unit>();
        auto txt_c_str = unit_specs.modifier == 'A' ? txt.ascii().c_str() : txt.standard().c_str();
        format_to(out, "{}", txt_c_str);
      }
    };

  }  // namespace detail

}  // namespace units

template<typename in_dimension, typename in_unit, typename Rep, typename CharT>
struct fmt::formatter<units::quantity<in_dimension, in_unit, Rep>, CharT> {
private:
  using quantity = units::quantity<in_dimension, in_unit, Rep>;
  using iterator = fmt::basic_format_parse_context<CharT>::iterator;
  using arg_ref_type = fmt::internal::arg_ref<CharT>;

  units::detail::global_format_specs<CharT> global_specs;
  units::detail::rep_format_specs  rep_specs;
  units::detail::unit_format_specs unit_specs;
  bool quantity_value = false;
  bool quantity_unit = false;
  bool quantity_unit_ascii_only = false;
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
    constexpr void on_fill(basic_string_view<CharT> fill) { f.global_specs.fill = fill; }    // global
    constexpr void on_align(align_t align)                { f.global_specs.align = align; }  // global
    constexpr void on_width(int width)                    { f.global_specs.width = width; }  // global
    constexpr void on_plus()  { f.rep_specs.sign = fmt::sign::plus; }     // rep
    constexpr void on_minus() { f.rep_specs.sign = fmt::sign::minus; }    // rep
    constexpr void on_space() { f.rep_specs.sign = fmt::sign::space; }    // rep
    constexpr void on_alt()  { f.rep_specs.alt  = true; }                // rep
    constexpr void on_precision(int precision) { f.rep_specs.precision = precision; } // rep
    constexpr void on_type(char type)                                     // rep
    {
      constexpr auto valid_rep_types = std::string_view{"aAbBdeEfFgGoxX"};
      if (valid_rep_types.find(type) != std::string_view::npos) {
        f.rep_specs.type = type;
      } else {
        on_error("invalid quantity type specifier");
      }
    }
    constexpr void on_modifier(char mod) {
      constexpr auto valid_modifiers = std::string_view{"A"};
      if (valid_modifiers.find(mod) != std::string_view::npos) {
        f.unit_specs.modifier = mod;
      } else {
        on_error("invalid unit modifier specified");
      }
    } // unit
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
    constexpr void on_quantity_value(const CharT* begin, const CharT* end)
    {
      if (begin != end) {
        units::detail::parse_units_rep(begin, end, *this, units::treat_as_floating_point<Rep>);
      }
      f.quantity_value = true;
    }
    constexpr void on_quantity_unit(const CharT mod)
    {
      if (mod != 'q') {
        f.unit_specs.modifier = mod;
      }
      f.quantity_unit = true;
    }

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

    // parse width
    begin = fmt::internal::parse_width(begin, end, handler);
    if(begin == end)
      return {begin, begin};

    // parse units-specific specification
    end = units::detail::parse_units_format(begin, end, handler);

    if(global_specs.align == fmt::align_t::none && (!quantity_unit || quantity_value))
      // quantity values should behave like numbers (by default aligned to right)
      global_specs.align = fmt::align_t::right;

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
  auto format(const units::quantity<in_dimension, in_unit, Rep>& q, FormatContext& ctx)
  {
    auto begin = format_str.begin(), end = format_str.end();

    // process dynamic width and precision
    fmt::internal::handle_dynamic_spec<fmt::internal::width_checker>(global_specs.width, width_ref, ctx);
    fmt::internal::handle_dynamic_spec<fmt::internal::precision_checker>(rep_specs.precision, precision_ref, ctx);

    // In `global_format_buffer` we will create a global format string
    //  e.g. "{:*^10%.1Q_%q}, 1.23q_m" => "{:*^10}"
    fmt::basic_memory_buffer<CharT> global_format_buffer;
    auto to_gfb = std::back_inserter(global_format_buffer);
    format_to(to_gfb, "{{:");
    if (global_specs.fill.size() != 1 || global_specs.fill[0] != ' ') {
      format_to(to_gfb, "{}", global_specs.fill.data());
    }
    if (auto align = global_specs.align; align != fmt::align_t::none) {
      switch (align) {
      case fmt::align_t::left:
        format_to(to_gfb, "<");
        break;
      case fmt::align_t::right:
        format_to(to_gfb, ">");
        break;
      case fmt::align_t::center:
        format_to(to_gfb, "^");
        break;
      default:
        break;
      }
    }
    if (auto width = global_specs.width; width >= 1) {
      format_to(to_gfb, "{}", width);
    }
    format_to(to_gfb, "}}");

    // In `quantity_buffer` we will have the representation and the unit formatted according to their
    //  specification, ignoring global specifiers
    //  e.g. "{:*^10%.1Q_%q}, 1.23q_m" => "1.2_m"
    // TODO Avoid extra copying if width is not specified
    fmt::basic_memory_buffer<CharT> quantity_buffer;
    auto to_quantity_buffer = std::back_inserter(quantity_buffer);

    // deal with quantity content
    if(begin == end || *begin == '}') {
      // default format should print value followed by the unit separated with 1 space
      to_quantity_buffer = units::detail::format_units_quantity_value<CharT>(to_quantity_buffer, q.count(), rep_specs);
      constexpr auto symbol = units::detail::unit_text<in_dimension, in_unit>();
      if(symbol.standard().size()) {
        *to_quantity_buffer++ = CharT(' ');
        format_to(to_quantity_buffer, "{}", symbol.standard().c_str());
      }
    }
    else {
      // user provided format
      units::detail::units_formatter f(to_quantity_buffer, q, global_specs, rep_specs, unit_specs);
      parse_units_format(begin, end, f);
    }
    // Format the `quantity buffer` using specs from `global_format_buffer`
    // In the example, equivalent to fmt::format("{:*^10}", "1.2_m")
    return format_to(ctx.out(), fmt::to_string(global_format_buffer), fmt::to_string(quantity_buffer));
  }
};
