

#pragma once

#include <units/physical/si/mass.h>

namespace units{
namespace experimental{

namespace mass {

template<typename Rep = double>
using kg = units::si::mass<units::si::kilogram, Rep>;

}

}} // units::experimental


