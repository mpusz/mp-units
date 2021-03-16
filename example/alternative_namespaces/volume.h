

#pragma once

#include <units/isq/si/volume.h>

namespace units{
namespace experimental{

namespace volume {

template<typename Rep = double>
using m3 = units::isq::si::volume<units::isq::si::cubic_metre, Rep>;

}

}} // units::experimental

