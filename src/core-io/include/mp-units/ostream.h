
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

#include <mp-units/quantity.h>
#include <mp-units/unit.h>
#include <sstream>

namespace mp_units {

namespace detail {

template<typename CharT, class Traits, auto R, typename Rep>
void to_stream(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
{
  if constexpr (is_same_v<Rep, std::uint8_t> || is_same_v<Rep, std::int8_t>)
    // promote the value to int
    os << +q.numerical_value();
  else
    os << q.numerical_value();
  if constexpr (has_unit_symbol(get_unit(R))) {
    if constexpr (space_before_unit_symbol<get_unit(R)>) os << " ";
    unit_symbol_to<CharT>(std::ostream_iterator<CharT>(os), get_unit(R));
  }
}

}  //  namespace detail

template<typename CharT, typename Traits, auto R, typename Rep>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity<R, Rep>& q)
  requires requires { os << q.numerical_value(); }
{
  if (os.width()) {
    // std::setw() applies to the whole quantity output so it has to be first put into std::string
    std::basic_ostringstream<CharT, Traits> oss;
    oss.flags(os.flags());
    oss.imbue(os.getloc());
    oss.precision(os.precision());
    detail::to_stream(oss, q);
    return os << std::move(oss).str();
  }

  detail::to_stream(os, q);
  return os;
}

}  // namespace mp_units
