
#pragma once

#include <units/isq/si/area.h>

namespace area {

template<typename Rep = double>
using m2 = units::isq::si::area<units::isq::si::square_metre, Rep>;

template<typename Rep = double>
using fm2 = units::isq::si::area<units::isq::si::square_femtometre, Rep>;

}  // namespace area
