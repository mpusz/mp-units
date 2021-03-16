

#pragma once

#include <units/isq/si/length.h>
#include <units/isq/si/imperial/length.h>
#include <units/isq/si/international/length.h>
#include <units/isq/si/typographic/length.h>
#include <units/isq/si/us/length.h>
#include <units/isq/si/iau/length.h>

namespace units{
namespace experimental{

namespace length {

template<typename Rep = double>
using m = units::isq::si::length<units::isq::si::metre, Rep>;

template<typename Rep = double>
using mm = units::isq::si::length<units::isq::si::millimetre, Rep>;

template<typename Rep = double>
using fm = units::isq::si::length<units::isq::si::femtometre, Rep>;

template<typename Rep = double>
using km = units::isq::si::length<units::isq::si::kilometre, Rep>;

template<typename Rep = double>
using AU = units::isq::si::length<units::isq::si::astronomical_unit, Rep>;

template<typename Rep = double>
using in = units::isq::si::length<units::isq::si::international::inch, Rep>;

template<typename Rep = double>
using angstrom = units::isq::si::length<units::isq::si::iau::angstrom, Rep>;

template<typename Rep = double>
using ch = units::isq::si::length<units::isq::si::imperial::chain, Rep>;

template<typename Rep = double>
using fathom = units::isq::si::length<units::isq::si::international::fathom, Rep>;

template<typename Rep = double>
using fathom_us = units::isq::si::length<units::isq::si::us::fathom, Rep>;

template<typename Rep = double>
using ft = units::isq::si::length<units::isq::si::international::foot, Rep>;

template<typename Rep = double>
using ft_us = units::isq::si::length<units::isq::si::us::foot, Rep>;

template<typename Rep = double>
using ly = units::isq::si::length<units::isq::si::iau::light_year, Rep>;

template<typename Rep = double>
using mi = units::isq::si::length<units::isq::si::international::mile, Rep>;

template<typename Rep = double>
using mi_naut = units::isq::si::length<units::isq::si::international::nautical_mile, Rep>;

template<typename Rep = double>
using pc = units::isq::si::length<units::isq::si::iau::parsec, Rep>;

template<typename Rep = double>
using pica_comp = units::isq::si::length<units::isq::si::typographic::pica_comp, Rep>;

template<typename Rep = double>
using pica_prn = units::isq::si::length<units::isq::si::typographic::pica_prn, Rep>;

template<typename Rep = double>
using point_comp = units::isq::si::length<units::isq::si::typographic::point_comp, Rep>;

template<typename Rep = double>
using point_prn = units::isq::si::length<units::isq::si::typographic::point_prn, Rep>;

template<typename Rep = double>
using rd = units::isq::si::length<units::isq::si::imperial::rod, Rep>;

template<typename Rep = double>
using yd = units::isq::si::length<units::isq::si::international::yard, Rep>;


}  // namespace length

}} // units::experimental


