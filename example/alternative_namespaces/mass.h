

#pragma once

#include <units/isq/si/mass.h>

namespace units{
namespace experimental{

namespace mass {

template<typename Rep = double>
using kg = units::isq::si::mass<units::isq::si::kilogram, Rep>;

}

}} // units::experimental


