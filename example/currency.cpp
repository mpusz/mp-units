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

#include <mp-units/compat_macros.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <chrono>
#include <concepts>
#include <iostream>
#include <map>
#include <string_view>
#include <utility>
#endif
#ifdef MP_UNITS_MODULES
import mp_units.core;
#else
#include <mp-units/framework.h>
#include <mp-units/ostream.h>
#endif

using namespace mp_units;

// clang-format off
inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;

QUANTITY_SPEC(currency, dim_currency);

inline constexpr struct euro final : named_unit<"EUR", kind_of<currency>> {} euro;
inline constexpr struct us_dollar final : named_unit<"USD", kind_of<currency>> {} us_dollar;
inline constexpr struct great_british_pound final : named_unit<"GBP", kind_of<currency>> {} great_british_pound;
inline constexpr struct japanese_jen final : named_unit<"JPY", kind_of<currency>> {} japanese_jen;
// clang-format on

namespace unit_symbols {

inline constexpr auto EUR = euro;
inline constexpr auto USD = us_dollar;
inline constexpr auto GBP = great_british_pound;
inline constexpr auto JPY = japanese_jen;

}  // namespace unit_symbols

static_assert(!std::equality_comparable_with<quantity<euro, int>, quantity<us_dollar, int>>);

template<Unit auto From, Unit auto To>
[[nodiscard]] double exchange_rate(std::chrono::sys_seconds timestamp)
{
  (void)timestamp;  // get conversion ratios for this timestamp
  static const std::map<std::pair<std::string_view, std::string_view>, double> rates = {
    {{"USD", "EUR"}, 0.9215}, {{"EUR", "USD"}, 1.0848},
    // ...
  };

  return rates.at(std::make_pair(unit_symbol(From), unit_symbol(To)));
}

template<UnitOf<currency> auto To, QuantityOf<currency> From>
QuantityOf<currency> auto exchange_to(From q, std::chrono::sys_seconds timestamp)
{
  const auto rate = static_cast<From::rep>(exchange_rate<From::unit, To>(timestamp) * q.numerical_value_in(q.unit));
  return rate * From::quantity_spec[To];
}

template<UnitOf<currency> auto To, QuantityPointOf<currency> From>
QuantityPointOf<currency> auto exchange_to(From qp, std::chrono::sys_seconds timestamp)
{
  const auto rate = static_cast<From::rep>(exchange_rate<From::unit, To>(timestamp) *
                                           qp.quantity_from_zero().numerical_value_in(qp.unit));
  return quantity_point{rate * From::quantity_spec[To], From::point_origin};
}

int main()
{
  using namespace unit_symbols;
  using namespace std::chrono;

  const auto timestamp = time_point_cast<seconds>(system_clock::now() - hours{24});
  const quantity_point price_usd{100 * USD};
  const quantity_point price_euro = exchange_to<euro>(price_usd, timestamp);

  std::cout << price_usd.quantity_from_zero() << " -> " << price_euro.quantity_from_zero() << "\n";
  // std::cout << price_usd.quantity_from_zero() + price_euro.quantity_from_zero() << "\n";  // does
  // not compile
}
