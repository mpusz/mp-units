
#pragma once

#include <units/isq/si/acceleration.h>

namespace units{
namespace experimental{

namespace acceleration {

template<typename Rep = double>
using m_per_s2 = units::isq::si::acceleration<units::isq::si::metre_per_second_sq, Rep>;

template<typename Rep = double>
constexpr m_per_s2<Rep> g{static_cast<Rep>(9.80665)};

}  // namespace acceleration

}} // units::experimental
