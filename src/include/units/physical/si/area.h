// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <units/physical/dimensions.h>
#include <units/physical/si/length.h>
#include <units/quantity.h>

namespace units::physical::si {

struct square_metre : unit<square_metre> {};
struct dim_area : physical::dim_area<dim_area, square_metre, dim_length> {};

struct square_yoctometre : deduced_unit<square_yoctometre, dim_area, yoctometre> {};
struct square_zeptometre : deduced_unit<square_zeptometre, dim_area, zeptometre> {};
struct square_attometre : deduced_unit<square_attometre, dim_area, attometre> {};
struct square_femtometre : deduced_unit<square_femtometre, dim_area, femtometre> {};
struct square_picometre : deduced_unit<square_picometre, dim_area, picometre> {};
struct square_nanometre : deduced_unit<square_nanometre, dim_area, nanometre> {};
struct square_micrometre : deduced_unit<square_micrometre, dim_area, micrometre> {};
struct square_millimetre : deduced_unit<square_millimetre, dim_area, millimetre> {};
struct square_centimetre : deduced_unit<square_centimetre, dim_area, centimetre> {};
struct square_decimetre : deduced_unit<square_decimetre, dim_area, decimetre> {};
struct square_decametre : deduced_unit<square_decametre, dim_area, decametre> {};
struct square_hectometre : deduced_unit<square_hectometre, dim_area, hectometre> {};
struct square_kilometre : deduced_unit<square_kilometre, dim_area, kilometre> {};
struct square_megametre : deduced_unit<square_megametre, dim_area, megametre> {};
struct square_gigametre : deduced_unit<square_gigametre, dim_area, gigametre> {};
struct square_terametre : deduced_unit<square_terametre, dim_area, terametre> {};
struct square_petametre : deduced_unit<square_petametre, dim_area, petametre> {};
struct square_exametre : deduced_unit<square_exametre, dim_area, exametre> {};
struct square_zettametre : deduced_unit<square_zettametre, dim_area, zettametre> {};
struct square_yottametre : deduced_unit<square_yottametre, dim_area, yottametre> {};

struct hectare : alias_unit<square_hectometre, "ha", no_prefix> {};

template<Unit U, ScalableNumber Rep = double>
using area = quantity<dim_area, U, Rep>;

inline namespace literals {

// m2
constexpr auto operator"" _q_m2(unsigned long long l) { return area<square_metre, std::int64_t>(l); }
constexpr auto operator"" _q_m2(long double l) { return area<square_metre, long double>(l); }

// ym2
constexpr auto operator"" _q_ym2(unsigned long long l) { return area<square_yoctometre, std::int64_t>(l); }
constexpr auto operator"" _q_ym2(long double l) { return area<square_yoctometre, long double>(l); }

// zm2
constexpr auto operator"" _q_zm2(unsigned long long l) { return area<square_zeptometre, std::int64_t>(l); }
constexpr auto operator"" _q_zm2(long double l) { return area<square_zeptometre, long double>(l); }

// am2
constexpr auto operator"" _q_am2(unsigned long long l) { return area<square_attometre, std::int64_t>(l); }
constexpr auto operator"" _q_am2(long double l) { return area<square_attometre, long double>(l); }

// fm2
constexpr auto operator"" _q_fm2(unsigned long long l) { return area<square_femtometre, std::int64_t>(l); }
constexpr auto operator"" _q_fm2(long double l) { return area<square_femtometre, long double>(l); }

// pm2
constexpr auto operator"" _q_pm2(unsigned long long l) { return area<square_picometre, std::int64_t>(l); }
constexpr auto operator"" _q_pm2(long double l) { return area<square_picometre, long double>(l); }

// nm2
constexpr auto operator"" _q_nm2(unsigned long long l) { return area<square_nanometre, std::int64_t>(l); }
constexpr auto operator"" _q_nm2(long double l) { return area<square_nanometre, long double>(l); }

// um2
constexpr auto operator"" _q_um2(unsigned long long l) { return area<square_micrometre, std::int64_t>(l); }
constexpr auto operator"" _q_um2(long double l) { return area<square_micrometre, long double>(l); }

// mm2
constexpr auto operator"" _q_mm2(unsigned long long l) { return area<square_millimetre, std::int64_t>(l); }
constexpr auto operator"" _q_mm2(long double l) { return area<square_millimetre, long double>(l); }

// cm2
constexpr auto operator"" _q_cm2(unsigned long long l) { return area<square_centimetre, std::int64_t>(l); }
constexpr auto operator"" _q_cm2(long double l) { return area<square_centimetre, long double>(l); }

// dm2
constexpr auto operator"" _q_dm2(unsigned long long l) { return area<square_decimetre, std::int64_t>(l); }
constexpr auto operator"" _q_dm2(long double l) { return area<square_decimetre, long double>(l); }

// dam2
constexpr auto operator"" _q_dam2(unsigned long long l) { return area<square_decametre, std::int64_t>(l); }
constexpr auto operator"" _q_dam2(long double l) { return area<square_decametre, long double>(l); }

// hm2
constexpr auto operator"" _q_hm2(unsigned long long l) { return area<square_hectometre, std::int64_t>(l); }
constexpr auto operator"" _q_hm2(long double l) { return area<square_hectometre, long double>(l); }

// km2
constexpr auto operator"" _q_km2(unsigned long long l) { return area<square_kilometre, std::int64_t>(l); }
constexpr auto operator"" _q_km2(long double l) { return area<square_kilometre, long double>(l); }

// Mm2
constexpr auto operator"" _q_Mm2(unsigned long long l) { return area<square_megametre, std::int64_t>(l); }
constexpr auto operator"" _q_Mm2(long double l) { return area<square_megametre, long double>(l); }

// Gm2
constexpr auto operator"" _q_Gm2(unsigned long long l) { return area<square_gigametre, std::int64_t>(l); }
constexpr auto operator"" _q_Gm2(long double l) { return area<square_gigametre, long double>(l); }

// Tm2
constexpr auto operator"" _q_Tm2(unsigned long long l) { return area<square_terametre, std::int64_t>(l); }
constexpr auto operator"" _q_Tm2(long double l) { return area<square_terametre, long double>(l); }

// Pm2
constexpr auto operator"" _q_Pm2(unsigned long long l) { return area<square_petametre, std::int64_t>(l); }
constexpr auto operator"" _q_Pm2(long double l) { return area<square_petametre, long double>(l); }

// Em2
constexpr auto operator"" _q_Em2(unsigned long long l) { return area<square_exametre, std::int64_t>(l); }
constexpr auto operator"" _q_Em2(long double l) { return area<square_exametre, long double>(l); }

// Zm2
constexpr auto operator"" _q_Zm2(unsigned long long l) { return area<square_zettametre, std::int64_t>(l); }
constexpr auto operator"" _q_Zm2(long double l) { return area<square_zettametre, long double>(l); }

// Ym2
constexpr auto operator"" _q_Ym2(unsigned long long l) { return area<square_yottametre, std::int64_t>(l); }
constexpr auto operator"" _q_Ym2(long double l) { return area<square_yottametre, long double>(l); }

// ha
constexpr auto operator"" _q_ha(unsigned long long l) { return area<hectare, std::int64_t>(l); }
constexpr auto operator"" _q_ha(long double l) { return area<hectare, long double>(l); }

}  // namespace literals

}  // namespace units::physical::si
