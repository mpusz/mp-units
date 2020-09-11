

#pragma once

#include <units/physical/si/si.h>

namespace units{
namespace experimental{

namespace mass {

template<typename Rep = double>
using kg = units::physical::si::mass<units::physical::si::kilogram, Rep>;

}

}} // units::experimental


