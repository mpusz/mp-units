

#pragma once

#include <units/physical/si/density.h>

namespace units{
namespace experimental{

namespace density {

template<typename Rep = double>
using kg_per_m3 = units::si::density<units::si::kilogram_per_metre_cub, Rep>;

}

}} // units::experimental


