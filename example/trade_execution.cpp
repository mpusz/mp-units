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
#include <array>
#include <iomanip>
#include <iostream>
#endif
#ifdef MP_UNITS_MODULES
import mp_units.core;
#else
#include <mp-units/core.h>
#endif

using namespace mp_units;

// clang-format off
inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
// clang-format on

QUANTITY_SPEC(currency, dim_currency);
QUANTITY_SPEC(market_quantity, dimensionless, is_kind);
QUANTITY_SPEC(notional_value, currency, currency* market_quantity, is_kind);

inline constexpr auto Qty = market_quantity;

static_assert(!implicitly_convertible(notional_value, currency));
static_assert(!implicitly_convertible(currency, notional_value));
static_assert(explicitly_convertible(notional_value, currency));
static_assert(explicitly_convertible(currency, notional_value));

// clang-format off
inline constexpr struct us_dollar   final : named_unit<"USD",   kind_of<currency>>             {} us_dollar;
inline constexpr struct us_dollar_2 final : named_unit<"USD_2", mag_power<10, -2> * us_dollar> {} us_dollar_2;
inline constexpr struct us_dollar_4 final : named_unit<"USD_4", mag_power<10, -4> * us_dollar> {} us_dollar_4;
inline constexpr struct us_dollar_6 final : named_unit<"USD_6", mag_power<10, -6> * us_dollar> {} us_dollar_6;
inline constexpr struct us_dollar_8 final : named_unit<"USD_8", mag_power<10, -8> * us_dollar> {} us_dollar_8;
// clang-format on

namespace unit_symbols {

inline constexpr auto USD = us_dollar;
inline constexpr auto USD_2 = us_dollar_2;
inline constexpr auto USD_4 = us_dollar_4;
inline constexpr auto USD_6 = us_dollar_6;
inline constexpr auto USD_8 = us_dollar_8;

}  // namespace unit_symbols

// Prices are affine: $0 is a meaningful absolute reference, not an arbitrary offset
using Price = quantity_point<currency[us_dollar]>;
using Shares = quantity<market_quantity[one], int>;
using Notional = quantity<notional_value[us_dollar_8], std::int64_t>;

struct Fill {
  Price price;
  Shares qty;
};

[[nodiscard]] Notional notional(const Fill& f)
{
  const quantity price_in_usd8 = f.price.quantity_from_unit_zero().force_in<std::int64_t>(us_dollar_8);
  return price_in_usd8 * f.qty;
}

std::ostream& operator<<(std::ostream& os, const QuantityPointOf<currency> auto& qp)
{
  return os << qp.quantity_from_unit_zero();
}

std::ostream& operator<<(std::ostream& os, const Fill& f) { return os << f.qty << " @ " << f.price; }

int main()
{
  using namespace unit_symbols;

  // Three partial fills of a single buy order
  const std::array fills = {
    Fill{Price{12.95 * USD}, Qty(137)},
    Fill{Price{12.40 * USD}, Qty(126)},
    Fill{Price{12.70 * USD}, Qty(85)},
  };

  // Notional = price × shares; the derived 'notional_value' quantity spec enforces this
  // Notional bad = fills[0].price.quantity_from_unit_zero() + fills[1].price.quantity_from_unit_zero();  // does not
  // compile

  Notional total_notional = {};
  Shares total_qty = {};
  for (const auto& f : fills) {
    total_notional += notional(f);
    total_qty += f.qty;
  }

  // Dividing notional by shares recovers currency (not notional_value), enforced at compile time
  // Notional bad = total_notional / total_qty;  // does not compile
  const quantity cost_basis_raw = currency(total_notional / total_qty);
  const Price cost_basis{cost_basis_raw};
  const Price cost_basis_r2{currency(round<USD_2>(cost_basis_raw))};
  const Price cost_basis_r4{currency(round<USD_4>(cost_basis_raw))};
  const Price cost_basis_r6{currency(round<USD_6>(cost_basis_raw))};
  const Price cost_basis_r8{currency(round<USD_8>(cost_basis_raw))};

  std::cout << "Fills:\n";
  for (const auto& f : fills) std::cout << "  " << f << "  (notional: " << notional(f) << ")\n";
  std::cout << std::setprecision(12);
  std::cout << "Total:              " << total_qty << " shares,  notional: " << total_notional << "\n";
  std::cout << "Cost basis (exact): " << cost_basis << "\n";
  std::cout << "Cost basis (USD_2): " << cost_basis_r2 << "\n";
  std::cout << "Cost basis (USD_4): " << cost_basis_r4 << "\n";
  std::cout << "Cost basis (USD_6): " << cost_basis_r6 << "\n";
  std::cout << "Cost basis (USD_8): " << cost_basis_r8 << "\n";
}
