
#pragma once

#include <units/isq/si/time.h>

// named qtime due to conflict with time_t time(time_t*)
namespace q_time {

template<typename Rep = double>
using s = units::isq::si::time<units::isq::si::second, Rep>;

template<typename Rep = double>
using ms = units::isq::si::time<units::isq::si::millisecond, Rep>;

template<typename Rep = double>
using min = units::isq::si::time<units::isq::si::minute, Rep>;

template<typename Rep = double>
using h = units::isq::si::time<units::isq::si::hour, Rep>;

}  // namespace time
