

#pragma once

#include <units/physical/si/derived/density.h>

namespace units{
namespace experimental{

namespace density {

template<typename Rep = double>
using kg_per_m3 = units::physical::si::density<units::physical::si::kilogram_per_metre_cub, Rep>;

}

}} // units::experimental


