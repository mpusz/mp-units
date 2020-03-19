
#pragma once

#include <units/physical/si/time.h>

// named qtime due to conflict with time_t time(time_t*)
namespace q_time {

template<typename Rep = double>
using s = units::si::time<units::si::second, Rep>;

template<typename Rep = double>
using ms = units::si::time<units::si::millisecond, Rep>;

template<typename Rep = double>
using min = units::si::time<units::si::minute, Rep>;

template<typename Rep = double>
using h = units::si::time<units::si::hour, Rep>;

}  // namespace time
