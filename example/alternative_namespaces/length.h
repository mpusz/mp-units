

#pragma once

#include <units/physical/si/length.h>
#include <units/physical/imperial/length.h>
#include <units/physical/international/length.h>
#include <units/physical/typographic/length.h>
#include <units/physical/us/length.h>
#include <units/physical/iau/length.h>

namespace units{
namespace experimental{

namespace length {

template<typename Rep = double>
using m = units::si::length<units::si::metre, Rep>;

template<typename Rep = double>
using mm = units::si::length<units::si::millimetre, Rep>;

template<typename Rep = double>
using fm = units::si::length<units::si::femtometre, Rep>;

template<typename Rep = double>
using km = units::si::length<units::si::kilometre, Rep>;

template<typename Rep = double>
using AU = units::si::length<units::si::astronomical_unit, Rep>;

template<typename Rep = double>
using in = units::si::length<units::international::inch, Rep>;

template<typename Rep = double>
using angstrom = units::si::length<units::iau::angstrom, Rep>;

template<typename Rep = double>
using ch = units::si::length<units::imperial::chain, Rep>;

template<typename Rep = double>
using fathom = units::si::length<units::international::fathom, Rep>;

template<typename Rep = double>
using fathom_us = units::si::length<units::us::fathom, Rep>;

template<typename Rep = double>
using ft = units::si::length<units::international::foot, Rep>;

template<typename Rep = double>
using ft_us = units::si::length<units::us::foot, Rep>;

template<typename Rep = double>
using ly = units::si::length<units::iau::light_year, Rep>;

template<typename Rep = double>
using mi = units::si::length<units::international::mile, Rep>;

template<typename Rep = double>
using mi_naut = units::si::length<units::international::nautical_mile, Rep>;

template<typename Rep = double>
using pc = units::si::length<units::iau::parsec, Rep>;

template<typename Rep = double>
using pica_comp = units::si::length<units::typographic::pica_comp, Rep>;

template<typename Rep = double>
using pica_prn = units::si::length<units::typographic::pica_prn, Rep>;

template<typename Rep = double>
using point_comp = units::si::length<units::typographic::point_comp, Rep>;

template<typename Rep = double>
using point_prn = units::si::length<units::typographic::point_prn, Rep>;

template<typename Rep = double>
using rd = units::si::length<units::imperial::rod, Rep>;

template<typename Rep = double>
using yd = units::si::length<units::international::yard, Rep>;


}  // namespace length

}} // units::experimental


