
#pragma once

#include <units/physical/si/acceleration.h>

namespace units{
namespace experimental{

namespace acceleration {

template<typename Rep = double>
using m_per_s2 = units::physical::si::acceleration<units::physical::si::metre_per_second_sq, Rep>;

template<typename Rep = double>
constexpr m_per_s2<Rep> g{static_cast<Rep>(9.80665)};

}  // namespace acceleration

}} // units::experimental
