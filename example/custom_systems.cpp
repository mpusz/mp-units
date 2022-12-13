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

#include <units/base_dimension.h>
#include <units/isq/si/prefixes.h>
#include <units/quantity.h>
#include <units/quantity_io.h>
#include <units/unit.h>
#include <iostream>
#include <type_traits>

using namespace units;

namespace fps {

struct foot : named_unit<foot, "ft"> {};
struct yard : named_scaled_unit<yard, "yd", mag<3>(), foot> {};

struct dim_length : base_dimension<"L", foot> {};

template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

}  // namespace fps

namespace si {

struct metre : named_unit<metre, "m"> {};
struct kilometre : prefixed_unit<kilometre, units::isq::si::kilo, metre> {};

struct dim_length : base_dimension<"L", metre> {};

template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

namespace fps {

struct foot : named_scaled_unit<foot, "ft", mag<ratio{3'048, 10'000}>(), metre> {};
struct yard : named_scaled_unit<yard, "yd", mag<3>(), foot> {};

struct dim_length : base_dimension<"L", foot> {};

template<UnitOf<dim_length> U, Representation Rep = double>
using length = quantity<dim_length, U, Rep>;

}  // namespace fps
}  // namespace si

template<typename Q, typename U>
concept castable_to = Quantity<Q> && Unit<U> && requires(Q q) { quantity_cast<U>(q); };

void conversions()
{
  // fps::yard is not defined in terms of SI units (or vice-versa)
  // so the conversion between FPS and SI is not possible
  static_assert(!castable_to<fps::length<fps::yard>, si::kilometre>);

  // si::fps::yard is defined in terms of SI units
  // so the conversion between FPS and SI is possible
  static_assert(castable_to<si::fps::length<si::fps::yard>, si::kilometre>);
  constexpr auto si_fps_yard = si::fps::length<si::fps::yard>(1.);
  std::cout << quantity_cast<si::kilometre>(si_fps_yard) << "\n";
}

void unknown_dimensions()
{
  constexpr auto fps_yard = fps::length<fps::yard>(1.);
  constexpr auto fps_area = fps_yard * fps_yard;
  std::cout << fps_yard << "\n";
  std::cout << quantity_cast<decltype(fps_area)::dimension::coherent_unit>(fps_area) << "\n";

  constexpr auto si_fps_yard = si::fps::length<si::fps::yard>(1.);
  constexpr auto si_fps_area = si_fps_yard * si_fps_yard;
  std::cout << si_fps_yard << "\n";
  std::cout << quantity_cast<decltype(si_fps_area)::dimension::coherent_unit>(si_fps_area) << "\n";
}

std::ostream& operator<<(std::ostream& os, const ratio& r) { return os << "ratio{" << r.num << ", " << r.den << "}"; }

template<Unit U>
std::ostream& operator<<(std::ostream& os, const U& u)
{
  using unit_type = std::remove_cvref_t<decltype(u)>;
  return os << as_ratio(unit_type::mag) << " x " << unit_type::reference::symbol.standard();
}

void what_is_your_ratio()
{
  std::cout << "fps:     " << fps::yard() << "\n";
  std::cout << "si::fps: " << si::fps::yard() << "\n";
}

int main()
{
  conversions();
  unknown_dimensions();
  what_is_your_ratio();
}
