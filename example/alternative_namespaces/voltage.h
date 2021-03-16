

#pragma once

#include <units/isq/si/voltage.h>

namespace units{
namespace experimental{

namespace voltage {

template<typename Rep = double>
using V = units::isq::si::voltage<units::isq::si::volt, Rep>;

template<typename Rep = double>
using mV = units::isq::si::voltage<units::isq::si::millivolt, Rep>;

template<typename Rep = double>
using uV = units::isq::si::voltage<units::isq::si::microvolt, Rep>;

template<typename Rep = double>
using nV = units::isq::si::voltage<units::isq::si::nanovolt, Rep>;

template<typename Rep = double>
using pV = units::isq::si::voltage<units::isq::si::picovolt, Rep>;

}  // namespace voltage

}} // units::experimental


