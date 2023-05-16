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

#include <mp_units/iostream.h>
#include <mp_units/quantity.h>
#include <mp_units/quantity_point.h>
#include <iostream>
#include <map>

using namespace mp_units;

// clang-format off
inline constexpr struct dim_currency : base_dimension<"$"> {} dim_currency;

QUANTITY_SPEC(currency, dim_currency);

inline constexpr struct euro : named_unit<"EUR", kind_of<currency>> {} euro;
inline constexpr struct us_dollar : named_unit<"USD", kind_of<currency>> {} us_dollar;
inline constexpr struct great_british_pound : named_unit<"GBP", kind_of<currency>> {} great_british_pound;
inline constexpr struct japanese_jen : named_unit<"JPY", kind_of<currency>> {} japanese_jen;
// clang-format on

#if 0

// if you have only a few currencies to handle
template<Unit auto From, Unit auto To>
[[nodiscard]] double exchange_rate()
{
  if constexpr (From == us_dollar && To == euro) return 0.9215;
  else if constexpr (From == euro && To == us_dollar) return 1.0848;
  // ...
}

#else

template<AssociatedUnit auto From, AssociatedUnit auto To>
  requires(get_quantity_spec(From) == currency && get_quantity_spec(To) == currency)
inline constexpr is_unit_convertible_result mp_units::is_unit_convertible<From, To> =
  is_unit_convertible_result::non_integral_factor;

[[nodiscard]] std::string_view to_string_view(Unit auto u) { return u.symbol.ascii().c_str(); }

template<Unit auto From, Unit auto To>
[[nodiscard]] double exchange_rate()
{
  static std::map<std::pair<std::string_view, std::string_view>, double> rates = {
    {{"USD", "EUR"}, 0.9215}, {{"EUR", "USD"}, 1.0848},
    // ...
  };

  return rates[std::make_pair(to_string_view(From), to_string_view(To))];
}

#endif

template<typename Rep, AssociatedUnit From, AssociatedUnit To>
[[nodiscard]] Rep scale_quantity_number(Rep v, From from, To to)
  requires(get_quantity_spec(from) == currency && get_quantity_spec(to) == currency)
{
  return static_cast<Rep>(exchange_rate<from, to>() * v);
}

static_assert(!std::equality_comparable_with<quantity<euro>, quantity<us_dollar>>);

int main()
{
  auto price_usd = quantity_point{100. * us_dollar};
  auto price_euro = quantity_point{price_usd[euro]};

  std::cout << price_usd.absolute() << " -> " << price_euro.absolute() << "\n";
  // std::cout << price_usd.absolute() + price_euro.absolute() << "\n";  // does not compile
  // std::cout << price_euro.absolute() + price_usd.absolute() << "\n";  // does not compile
}
