

#pragma once

#include <units/physical/si/volume.h>

namespace units{
namespace experimental{

namespace volume {

template<typename Rep = double>
using m3 = units::si::volume<units::si::cubic_metre, Rep>;

}

}} // units::experimental


