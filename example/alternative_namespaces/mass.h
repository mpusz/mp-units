

#pragma once

#include <units/physical/si/base/mass.h>

namespace units{
namespace experimental{

namespace mass {

template<typename Rep = double>
using kg = units::physical::si::mass<units::physical::si::kilogram, Rep>;

}

}} // units::experimental


