

#pragma once

#include <units/physical/si/derived/voltage.h>

namespace units{
namespace experimental{

namespace voltage {

template<typename Rep = double>
using V = units::physical::si::voltage<units::physical::si::volt, Rep>;

template<typename Rep = double>
using mV = units::physical::si::voltage<units::physical::si::millivolt, Rep>;

template<typename Rep = double>
using uV = units::physical::si::voltage<units::physical::si::microvolt, Rep>;

template<typename Rep = double>
using nV = units::physical::si::voltage<units::physical::si::nanovolt, Rep>;

template<typename Rep = double>
using pV = units::physical::si::voltage<units::physical::si::picovolt, Rep>;

}  // namespace voltage

}} // units::experimental


