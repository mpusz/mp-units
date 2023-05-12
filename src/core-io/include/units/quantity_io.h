
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

// IWYU pragma: begin_exports
#include <units/bits/external/fixed_string_io.h>
#include <units/bits/unit_text.h>
#include <sstream>
// IWYU pragma: end_exports

namespace units {

namespace detail {

template<typename CharT, class Traits, typename D, typename U, typename Rep>
void to_stream(std::basic_ostream<CharT, Traits>& os, const quantity<D, U, Rep>& q)
{
  os << q.number();
  constexpr auto symbol = detail::unit_text<D, U>();
  if constexpr (!symbol.standard().empty()) {
    os << " " << symbol.standard();
  }
}

}  //  namespace detail

template<typename CharT, typename Traits, typename D, typename U, typename Rep>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity<D, U, Rep>& q)
  requires requires { os << q.number(); }
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

}  // namespace units
