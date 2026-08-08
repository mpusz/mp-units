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

#include <mp-units/systems/isq/light_and_radiation.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::isq;

template<auto QS, auto U>
concept can_use_unit = requires { QS[U]; };

// a user-provided condition (e.g. a CIE S 026 alpha-opic weighting)
inline constexpr struct melanopic final : photometric_condition_base {
} melanopic;

// the concept accepts only photometric conditions
static_assert(PhotometricCondition<std::remove_const_t<decltype(photopic_vision)>>);
static_assert(PhotometricCondition<std::remove_const_t<decltype(scotopic_vision)>>);
static_assert(PhotometricCondition<mesopic_vision_<50>>);
static_assert(PhotometricCondition<std::remove_const_t<decltype(melanopic)>>);
static_assert(!PhotometricCondition<int>);
static_assert(!PhotometricCondition<std::remove_const_t<decltype(isq::length)>>);

// plain names are the photopic default instantiations (ISO 80000-7 clause 0.3)
static_assert(luminous_intensity_of<> == luminous_intensity);
static_assert(luminous_flux_of<> == luminous_flux);
static_assert(illuminance_of<> == illuminance);
static_assert(luminous_flux_of<photopic_vision> == luminous_flux);

// the photopic per-condition units are the SI units themselves
static_assert(
  std::same_as<std::remove_const_t<decltype(si::candela_of<>)>, std::remove_const_t<decltype(si::candela)>>);
static_assert(std::same_as<std::remove_const_t<decltype(si::lumen_of<>)>, std::remove_const_t<decltype(si::lumen)>>);
static_assert(std::same_as<std::remove_const_t<decltype(si::lux_of<>)>, std::remove_const_t<decltype(si::lux)>>);

// every condition roots its own kind tree over the same dimension
static_assert(get_dimension(luminous_intensity_of<scotopic_vision>) == get_dimension(luminous_intensity));
static_assert(get_kind(luminous_intensity_of<scotopic_vision>) != get_kind(luminous_intensity));
static_assert(get_kind(luminous_flux_of<scotopic_vision>) != get_kind(luminous_flux));
static_assert(get_kind(luminous_flux_of<melanopic>) != get_kind(luminous_flux));
static_assert(get_kind(luminous_flux_of<melanopic>) != get_kind(luminous_flux_of<scotopic_vision>));

// mesopic quantities at different adaptation levels are distinct as well
static_assert(get_kind(luminance_of<mesopic_vision<50>>) != get_kind(luminance_of<mesopic_vision<10>>));
static_assert(get_kind(luminance_of<mesopic_vision<50>>) != get_kind(luminance));

// quantities of different photometric conditions can never be mixed or converted
static_assert(!implicitly_convertible(luminous_flux_of<scotopic_vision>, luminous_flux));
static_assert(!explicitly_convertible(luminous_flux_of<scotopic_vision>, luminous_flux));
static_assert(!castable(luminous_flux_of<scotopic_vision>, luminous_flux));
static_assert(!castable(illuminance_of<scotopic_vision>, illuminance));

// each condition's tree accepts only its own units
static_assert(can_use_unit<luminous_intensity_of<scotopic_vision>, si::candela_of<scotopic_vision>>);
static_assert(can_use_unit<luminous_flux_of<scotopic_vision>, si::lumen_of<scotopic_vision>>);
static_assert(can_use_unit<illuminance_of<scotopic_vision>, si::lux_of<scotopic_vision>>);
static_assert(can_use_unit<luminous_flux_of<melanopic>, si::lumen_of<melanopic>>);
static_assert(!can_use_unit<luminous_intensity_of<scotopic_vision>, si::candela>);
static_assert(!can_use_unit<luminous_flux_of<scotopic_vision>, si::lumen>);
static_assert(!can_use_unit<luminous_intensity, si::candela_of<scotopic_vision>>);
static_assert(!can_use_unit<luminous_flux, si::lumen_of<scotopic_vision>>);
static_assert(!can_use_unit<illuminance, si::lux_of<scotopic_vision>>);

// the whole derived tree works within one condition
static_assert(implicitly_convertible(luminous_intensity_of<scotopic_vision> * isq::solid_angular_measure,
                                     luminous_flux_of<scotopic_vision>));
static_assert(implicitly_convertible(luminous_flux_of<scotopic_vision> / isq::area, illuminance_of<scotopic_vision>));
static_assert(implicitly_convertible(luminous_flux_of<scotopic_vision> * isq::time,
                                     luminous_energy_of<scotopic_vision>));
static_assert(implicitly_convertible(illuminance_of<scotopic_vision> * isq::time,
                                     luminous_exposure_of<scotopic_vision>));
static_assert(implicitly_convertible(maximum_luminous_efficacy_of<scotopic_vision>,
                                     luminous_efficacy_of_radiation_of<scotopic_vision>));
static_assert(!implicitly_convertible(maximum_luminous_efficacy_of<scotopic_vision>, luminous_efficacy_of_radiation));

// quotients across conditions stay well-formed and reduce to a pure number (e.g. the S/P ratio)
constexpr quantity sp_ratio =
  (luminous_flux_of<scotopic_vision>(1700. * si::lumen_of<scotopic_vision>) / luminous_flux(1000. * si::lumen)).in(one);
static_assert(sp_ratio.numerical_value_in(one) == 1.7);

// the two "luminous efficacy" items of ISO 80000-7 share lm/W but are distinct subkinds:
// crossing between them requires the radiant efficiency of the source (like fluid and water head)
static_assert(get_kind(luminous_efficacy_of_radiation) != get_kind(luminous_efficacy_of_source));
static_assert(!implicitly_convertible(luminous_efficacy_of_radiation, luminous_efficacy_of_source));
static_assert(!explicitly_convertible(luminous_efficacy_of_radiation, luminous_efficacy_of_source));
static_assert(!castable(luminous_efficacy_of_radiation, luminous_efficacy_of_source));
static_assert(explicitly_convertible(luminous_efficacy_of_radiation, luminous_efficacy));
static_assert(explicitly_convertible(luminous_efficacy_of_source, luminous_efficacy));
static_assert(!implicitly_convertible(luminous_efficacy_of_radiation, luminous_efficacy));
static_assert(can_use_unit<luminous_efficacy_of_radiation, si::lumen / si::watt>);
static_assert(can_use_unit<luminous_efficacy_of_source, si::lumen / si::watt>);
static_assert(can_use_unit<luminous_efficacy_of_source_of<scotopic_vision>, si::lumen_of<scotopic_vision> / si::watt>);
// the subkinds still separate the photometric conditions
static_assert(get_kind(luminous_efficacy_of_source_of<scotopic_vision>) != get_kind(luminous_efficacy_of_source));

// dimensionless ratios share the dimensionless kind (like other efficiencies and factors)
// but stay distinct specs with no sideways implicit conversion
static_assert(!implicitly_convertible(luminous_reflectance_of<scotopic_vision>, luminous_reflectance));
static_assert(!implicitly_convertible(luminous_reflectance, luminous_reflectance_of<scotopic_vision>));
static_assert(implicitly_convertible(luminous_reflectance_of<scotopic_vision>, dimensionless));

}  // namespace
