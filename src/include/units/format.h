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

#include <units/quantity.h>
#include <fmt/format.h>

// units-format-spec:
//      fill-and-align[opt] width[opt] precision[opt] units-specs[opt]
// units-specs:
//      conversion-spec
//      units-specs conversion-spec
//      units-specs literal-char
// literal-char:
//      any character other than { or }
// conversion-spec:
//      % modifier[opt] type
// modifier: one of
//      E O
// type: one of
//      q Q %


template<typename U, typename Rep, typename CharT>
struct fmt::formatter<units::quantity<U, Rep>, CharT> {
private:
  fmt::basic_format_specs<CharT> specs;
  int precision = -1;
  using arg_ref_type = fmt::internal::arg_ref<CharT>;
  arg_ref_type width_ref;
  arg_ref_type precision_ref;
  mutable basic_string_view<CharT> format_str;
  using quantity = units::quantity<U, Rep>;

  // auto parse_unit_format() {
  //   if (s != ctx.end() && *s == 'q') {
  //     quantity = true;
  //     return ++s;
  //   }
  // }

public:
  constexpr auto parse(fmt::basic_parse_context<CharT>& ctx)
  {
    // [ctx.begin(), ctx.end()) is a range of CharTs containing format-specs,
    // e.g. in format("{:%Q %q}", ...) it is "%Q %q}" (format string after ':')
    // auto begin = ctx.begin(), end = ctx.end();
    // Look at do_parse in fmt/chrono.h and provide replacement for parse_chrono_format.
    // fill-and-align_opt ...
    // begin = fmt::internal::parse_align(begin, end, handler);
    // parse_unit_format();
    return ctx.end();
  }
  // format("{:{}}", 'x', 10)

  template<typename FormatContext>
  auto format(const units::quantity<U, Rep>& q, FormatContext& ctx)
  {
    // ctx.out() - output iterator you write to.
    // auto s = format("{0:.{1}} {2}", q.count(), precision, unit(q));
    // return format_to(ctx.out(), "{:{}}", s, width);
    return format_to(ctx.out(), "{} {}", q.count(), units::detail::unit_text<typename quantity::unit>().c_str());
  }
};
