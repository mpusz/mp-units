
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

#include <mp-units/bits/module_macros.h>
#include <mp-units/quantity.h>
#include <mp-units/unit.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <sstream>
#endif

namespace mp_units {

namespace detail {

template<typename CharT, class Traits, Dimension D>
void to_stream_impl(std::basic_ostream<CharT, Traits>& os, D d)
{
  dimension_symbol_to<CharT>(std::ostream_iterator<CharT>(os), d);
}

template<typename CharT, class Traits, Unit U>
void to_stream_impl(std::basic_ostream<CharT, Traits>& os, U u)
{
  unit_symbol_to<CharT>(std::ostream_iterator<CharT>(os), u);
}

template<typename CharT, class Traits, auto R, typename Rep>
void to_stream_impl(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
  requires requires { os << q.numerical_value_ref_in(q.unit); }
{
  if constexpr (is_same_v<Rep, std::uint8_t> || is_same_v<Rep, std::int8_t>)
    // promote the value to int
    os << +q.numerical_value_ref_in(q.unit);
  else
    os << q.numerical_value_ref_in(q.unit);
  if constexpr (space_before_unit_symbol<get_unit(R)>) os << " ";
  to_stream_impl(os, get_unit(R));
}

template<typename CharT, class Traits, typename T>
std::basic_ostream<CharT, Traits>& to_stream(std::basic_ostream<CharT, Traits>& os, const T& v)
  requires requires { detail::to_stream_impl(os, v); }
{
  if (os.width()) {
    // std::setw() applies to the whole output so it has to be first put into std::string
    std::basic_ostringstream<CharT, Traits> oss;
    oss.flags(os.flags());
    oss.imbue(os.getloc());
    oss.precision(os.precision());
    detail::to_stream_impl(oss, v);
    return os << std::move(oss).str();
  }

  detail::to_stream_impl(os, v);
  return os;
}

}  //  namespace detail

MP_UNITS_EXPORT_BEGIN

template<typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const T& v)
  requires requires { detail::to_stream_impl(os, v); }
{
  return detail::to_stream(os, v);
}

MP_UNITS_EXPORT_END

}  // namespace mp_units
