
#pragma once

#include <units/isq/si/force.h>

namespace units{
namespace experimental{

namespace force {

template<typename Rep = double>
using N = units::isq::si::force<units::isq::si::newton, Rep>;

}

}} // units::experimental
