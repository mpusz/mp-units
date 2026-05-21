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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
#include <mp-units/core.h>
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
  // Paired rates are not exact inverses — the asymmetry encodes the bid/ask spread
  static const std::map<std::pair<std::string_view, std::string_view>, double> rates = {
    {{"USD", "EUR"}, 0.9215}, {{"EUR", "USD"}, 1.0848}, {{"USD", "GBP"}, 0.7897},
    {{"GBP", "USD"}, 1.2650}, {{"USD", "JPY"}, 142.88}, {{"JPY", "USD"}, 0.00693},
    // ...
  };

  return rates.at(std::make_pair(unit_symbol(From), unit_symbol(To)));
}

template<UnitOf<currency> auto To, QuantityOf<currency> From>
QuantityOf<currency> auto exchange_to(From q, std::chrono::sys_seconds timestamp)
{
  const double rate = exchange_rate<From::unit, To>(timestamp) * q.numerical_value_in(q.unit);
  return rate * From::quantity_spec[To];
}

template<UnitOf<currency> auto To, QuantityPointOf<currency> From>
QuantityPointOf<currency> auto exchange_to(From qp, std::chrono::sys_seconds timestamp)
{
  return quantity_point{exchange_to<To>(qp.quantity_from_unit_zero(), timestamp), From::point_origin};
}

int main()
{
  using namespace unit_symbols;
  using namespace std::chrono;

  const auto timestamp = time_point_cast<seconds>(system_clock::now() - hours{24});

  // FX conversion of a single price (quantity_point: $0 is a meaningful absolute reference)
  const quantity_point price_usd{100 * USD};
  const quantity_point price_eur = exchange_to<euro>(price_usd, timestamp);
  std::cout << price_usd << " -> " << price_eur << "\n";

  // the below don't compile
  // std::cout << (price_usd + price_usd).quantity_from_unit_zero() << "\n";
  // std::cout << price_usd.quantity_from_unit_zero() + price_eur.quantity_from_unit_zero() << "\n";

  // Multi-currency portfolio: positions held in native currencies (quantities, not prices)
  const quantity pos_usd = 14'230 * USD;
  const quantity pos_eur = 4'902 * EUR;
  const quantity pos_gbp = 1'464 * GBP;
  const quantity pos_jpy = 890'000 * JPY;

  // Cross-currency arithmetic does not compile — must go through explicit FX conversion:
  // const quantity bad = pos_usd + pos_eur;  // does not compile

  const quantity pos_eur_usd = round<USD>(exchange_to<us_dollar>(pos_eur, timestamp)).force_in<int>();
  const quantity pos_gbp_usd = round<USD>(exchange_to<us_dollar>(pos_gbp, timestamp)).force_in<int>();
  const quantity pos_jpy_usd = round<USD>(exchange_to<us_dollar>(pos_jpy, timestamp)).force_in<int>();
  const quantity total_usd = pos_usd + pos_eur_usd + pos_gbp_usd + pos_jpy_usd;

  std::cout << "Portfolio positions:\n";
  std::cout << "  " << pos_usd << "\n";
  std::cout << "  " << pos_eur << "  =>  " << pos_eur_usd << "\n";
  std::cout << "  " << pos_gbp << "  =>  " << pos_gbp_usd << "\n";
  std::cout << "  " << pos_jpy << "  =>  " << pos_jpy_usd << "\n";
  std::cout << "Portfolio total (USD): " << total_usd << "\n";
}
