

#pragma once

#include <units/isq/si/density.h>

namespace units{
namespace experimental{

namespace density {

template<typename Rep = double>
using kg_per_m3 = units::isq::si::density<units::isq::si::kilogram_per_metre_cub, Rep>;

}

}} // units::experimental


