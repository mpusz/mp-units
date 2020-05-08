

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
using m = units::physical::si::length<units::physical::si::metre, Rep>;

template<typename Rep = double>
using mm = units::physical::si::length<units::physical::si::millimetre, Rep>;

template<typename Rep = double>
using fm = units::physical::si::length<units::physical::si::femtometre, Rep>;

template<typename Rep = double>
using km = units::physical::si::length<units::physical::si::kilometre, Rep>;

template<typename Rep = double>
using AU = units::physical::si::length<units::physical::si::astronomical_unit, Rep>;

template<typename Rep = double>
using in = units::physical::si::length<units::physical::international::inch, Rep>;

template<typename Rep = double>
using angstrom = units::physical::si::length<units::physical::iau::angstrom, Rep>;

template<typename Rep = double>
using ch = units::physical::si::length<units::physical::imperial::chain, Rep>;

template<typename Rep = double>
using fathom = units::physical::si::length<units::physical::international::fathom, Rep>;

template<typename Rep = double>
using fathom_us = units::physical::si::length<units::physical::us::fathom, Rep>;

template<typename Rep = double>
using ft = units::physical::si::length<units::physical::international::foot, Rep>;

template<typename Rep = double>
using ft_us = units::physical::si::length<units::physical::us::foot, Rep>;

template<typename Rep = double>
using ly = units::physical::si::length<units::physical::iau::light_year, Rep>;

template<typename Rep = double>
using mi = units::physical::si::length<units::physical::international::mile, Rep>;

template<typename Rep = double>
using mi_naut = units::physical::si::length<units::physical::international::nautical_mile, Rep>;

template<typename Rep = double>
using pc = units::physical::si::length<units::physical::iau::parsec, Rep>;

template<typename Rep = double>
using pica_comp = units::physical::si::length<units::physical::typographic::pica_comp, Rep>;

template<typename Rep = double>
using pica_prn = units::physical::si::length<units::physical::typographic::pica_prn, Rep>;

template<typename Rep = double>
using point_comp = units::physical::si::length<units::physical::typographic::point_comp, Rep>;

template<typename Rep = double>
using point_prn = units::physical::si::length<units::physical::typographic::point_prn, Rep>;

template<typename Rep = double>
using rd = units::physical::si::length<units::physical::imperial::rod, Rep>;

template<typename Rep = double>
using yd = units::physical::si::length<units::physical::international::yard, Rep>;


}  // namespace length

}} // units::experimental


