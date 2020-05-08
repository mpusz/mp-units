
#pragma once

#include <units/physical/si/time.h>

// named qtime due to conflict with time_t time(time_t*)
namespace q_time {

template<typename Rep = double>
using s = units::physical::si::time<units::physical::si::second, Rep>;

template<typename Rep = double>
using ms = units::physical::si::time<units::physical::si::millisecond, Rep>;

template<typename Rep = double>
using min = units::physical::si::time<units::physical::si::minute, Rep>;

template<typename Rep = double>
using h = units::physical::si::time<units::physical::si::hour, Rep>;

}  // namespace time
