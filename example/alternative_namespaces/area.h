
#pragma once

#include <units/physical/si/area.h>

namespace area {

template<typename Rep = double>
using m2 = units::si::area<units::si::square_metre, Rep>;

template<typename Rep = double>
using fm2 = units::si::area<units::si::square_femtometre, Rep>;

}  // namespace area
