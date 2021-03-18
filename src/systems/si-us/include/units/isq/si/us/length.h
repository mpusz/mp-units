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

#include <units/isq/si/length.h>

namespace units::isq::si::us {

// https://en.wikipedia.org/wiki/Foot_(unit)#US_survey_foot
// https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors#B6
struct foot : named_scaled_unit<foot, "ft(us)", no_prefix, ratio(1'200, 3'937), si::metre> {};

// https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors#B6
struct fathom : named_scaled_unit<fathom, "fathom(us)", no_prefix, ratio(6), foot> {};

// https://en.wikipedia.org/wiki/Mile#U.S._survey_mile
// https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors#B6
struct mile : named_scaled_unit<mile, "mi(us)", no_prefix, ratio(5280), foot> {};

inline namespace literals {

// ft
constexpr auto operator"" _q_ft_us(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return si::length<units::isq::si::us::foot, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_ft_us(long double l) { return si::length<units::isq::si::us::foot, long double>(l); }

// fathom
constexpr auto operator"" _q_fathom_us(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return si::length<units::isq::si::us::fathom, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_fathom_us(long double l) { return si::length<units::isq::si::us::fathom, long double>(l); }

// mi
constexpr auto operator"" _q_mi_us(unsigned long long l) { gsl_ExpectsAudit(std::in_range<std::int64_t>(l)); return si::length<units::isq::si::us::mile, std::int64_t>(static_cast<std::int64_t>(l)); }
constexpr auto operator"" _q_mi_us(long double l) { return si::length<units::isq::si::us::mile, long double>(l); }

}  // namespace literals

namespace references {

inline constexpr auto ft = reference<si::dim_length, us::foot>{};
inline constexpr auto fathom = reference<si::dim_length, us::fathom>{};
inline constexpr auto mi = reference<si::dim_length, us::mile>{};

}  // namespace references

}  // namespace units::isq::si::us
