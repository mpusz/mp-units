

#pragma once

#include <units/physical/si/voltage.h>

namespace units{
namespace experimental{

namespace voltage {

template<typename Rep = double>
using V = units::si::voltage<units::si::volt, Rep>;

template<typename Rep = double>
using mV = units::si::voltage<units::si::millivolt, Rep>;

template<typename Rep = double>
using uV = units::si::voltage<units::si::microvolt, Rep>;

template<typename Rep = double>
using nV = units::si::voltage<units::si::nanovolt, Rep>;

template<typename Rep = double>
using pV = units::si::voltage<units::si::picovolt, Rep>;

}  // namespace voltage

}} // units::experimental


