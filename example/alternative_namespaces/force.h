
#pragma once

#include <units/physical/si/force.h>

namespace units{
namespace experimental{

namespace force {

template<typename Rep = double>
using N = units::si::force<units::si::newton, Rep>;

}

}} // units::experimental
