// The MIT License (MIT)
//
// Copyright (c) 2020 Yves Delley
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

#include <units/origin.h>
#include <units/physical/si/base/thermodynamic_temperature.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity_io.h>
#include <units/quantity_point.h>
#include <units/ratio.h>
#include <iostream>

using namespace units;

namespace si = units::physical::si;

namespace detail {

struct centikelvin : scaled_unit<ratio(1, 100), si::kelvin> {
};

}  // namespace detail

struct celsius : alias_unit<si::kelvin, basic_symbol_text{"\u00b0C", "`C"}, no_prefix> {
};

struct celsius_temperature_origin : point_origin<celsius_temperature_origin, si::kelvin> {
  using reference_origin = si::kelvin_temperature_origin;
  static constexpr auto offset_to_reference = si::thermodynamic_temperature<::detail::centikelvin, long int>(27315);
};

template<UnitOf<si::dim_thermodynamic_temperature> U = celsius, QuantityValue Rep = double>
using celsius_temperature_point = quantity_point<si::dim_thermodynamic_temperature, U, Rep, celsius_temperature_origin>;

struct fahrenheit : named_scaled_unit<fahrenheit, basic_symbol_text{"\u00b0F", "`F"}, no_prefix, ratio(5, 9), si::kelvin> {
};

namespace detail {

struct millifahrenheit : scaled_unit<ratio(5, 9000), si::kelvin> {
};

}  // namespace detail

struct fahrenheit_temperature_origin : point_origin<fahrenheit_temperature_origin, si::kelvin> {
  using reference_origin = si::kelvin_temperature_origin;
  static constexpr auto offset_to_reference =
      si::thermodynamic_temperature<::detail::millifahrenheit, long int>(273150 * 9 / 5 + 32000);
};

template<UnitOf<si::dim_thermodynamic_temperature> U = fahrenheit, QuantityValue Rep = double>
using fahrenheit_temperature_point =
    quantity_point<si::dim_thermodynamic_temperature, U, Rep, fahrenheit_temperature_origin>;


template<QuantityPoint QP1, QuantityPoint QP2>
 requires fixed_known_offset<typename QP1::origin, typename QP2::origin>
QP1 quantity_point_offset_cast(const QP2& qp)
{
  using Q = typename QP1::quantity_type;
  using Orig1 = typename QP1::origin;
  using Orig2 = typename QP2::origin;
  return QP1(quantity_cast<Q>(qp.relative() + offset_between_origins<Q,Orig2,Orig1>));
}

using namespace si::literals;
using namespace si::unit_constants;

template<typename T>
void debug_type()
{
  static_assert(!std::is_same_v<T, T>);
}


int main()
{
  // somehow my GCC 10.2 is ICEing in CTAD here, so I specify them explicitly.
  celsius_temperature_point<> freezing(0 * K);
  celsius_temperature_point<> boiling(100 * K);

  auto f_C = quantity_point_offset_cast<celsius_temperature_point<>>(freezing);
  auto b_C = quantity_point_offset_cast<celsius_temperature_point<>>(boiling);

  auto f_F = quantity_point_offset_cast<fahrenheit_temperature_point<>>(freezing);
  auto b_F = quantity_point_offset_cast<fahrenheit_temperature_point<>>(boiling);

  auto f_K = quantity_point_offset_cast<si::kelvin_temperature_point<>>(freezing);
  auto b_K = quantity_point_offset_cast<si::kelvin_temperature_point<>>(boiling);

  std::cout << "Freezing point: " << f_C.relative() << "; Boiling point: " << b_C.relative() << std::endl;
  std::cout << "Freezing point: " << f_F.relative() << "; Boiling point: " << b_F.relative() << std::endl;
  std::cout << "Freezing point: " << f_K.relative() << "; Boiling point: " << b_K.relative() << std::endl;
}
