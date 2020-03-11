

#pragma once

#include <units/physical/si/length.h>

namespace units{
namespace experimental{

namespace length {

template<typename Rep = double>
using m = units::si::length<units::si::metre, Rep>;

template<typename Rep = double>
using mm = units::si::length<units::si::millimetre, Rep>;

}  // namespace length

}} // units::experimental


