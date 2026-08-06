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

#include "test_tools.h"
#include <mp-units/systems/codata.h>
#include <mp-units/systems/hep.h>
#include <mp-units/systems/iau.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/uncertain.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <complex>
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>
#endif

namespace {

using namespace mp_units;
using mp_units::utility::measurement_of;
using mp_units::utility::uncertain;

// a valid representation type for scalar quantities (and their degenerate vector/tensor uses)
static_assert(RepresentationOf<uncertain<int>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<uncertain<int>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<uncertain<double>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<uncertain<double>, quantity_tensor_order::vector>);

// an exact value converts implicitly (value ± 0); uncertainty requires explicit construction
static_assert(std::convertible_to<double, uncertain<double>>);
static_assert(!std::constructible_from<uncertain<double>, double, double, double>);

// the framework requires every real scalar representation to be totally ordered
static_assert(std::totally_ordered<uncertain<double>>);

// the element type is constrained, so an unsupported one is rejected at instantiation rather
// than deep inside a propagation formula
template<typename T>
concept can_be_uncertain = requires { typename uncertain<T>; };
static_assert(can_be_uncertain<double>);
static_assert(can_be_uncertain<int>);
static_assert(!can_be_uncertain<std::string>);
// a complex element type is rejected because the propagation formulas are real-valued and the
// uncertainty precondition needs an ordering
static_assert(!can_be_uncertain<std::complex<double>>);
// a type the library refuses to store in a `quantity` cannot become a valid representation by
// being wrapped in `uncertain`
static_assert(!RepresentationOf<bool, quantity_tensor_order::scalar>);
static_assert(!can_be_uncertain<bool>);

// mixed-representation arithmetic converges on `uncertain`
static_assert(std::is_same_v<std::common_type_t<uncertain<float>, uncertain<double>>, uncertain<double>>);
static_assert(std::is_same_v<std::common_type_t<uncertain<double>, double>, uncertain<double>>);
static_assert(std::is_same_v<std::common_type_t<int, uncertain<double>>, uncertain<double>>);

using uncertain_length = quantity<si::metre, uncertain<double>>;
using exact_length = quantity<si::metre, double>;
static_assert(std::is_same_v<decltype(std::declval<uncertain_length>() * std::declval<uncertain_length>()),
                             quantity<square(si::metre), uncertain<double>>>);
static_assert(std::is_same_v<decltype(std::declval<uncertain_length>() + std::declval<exact_length>()),
                             quantity<si::metre, uncertain<double>>>);

// only constants defined with an uncertainty wrapper (`relative_standard_uncertainty` or
// `standard_uncertainty`) are measured constants
static_assert(MeasuredConstant<std::remove_const_t<decltype(iau::newtonian_constant_of_gravitation)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(si::si2019::speed_of_light_in_vacuum)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(iau::nominal_solar_mass_parameter)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(si::metre)>>);

// CODATA 2018: G = 6.674 30(15) × 10⁻¹¹ m³ kg⁻¹ s⁻². The definition stores the published absolute
// uncertainty, and the relative form derives from it as an exact ratio of canonical magnitudes:
// u_r = 1.5e-15 / 6.6743e-11 = 15/66743 × 10⁻¹, so it can be compared exactly.
static_assert(get_relative_standard_uncertainty(iau::newtonian_constant_of_gravitation) ==
              mag_ratio<15, 66'743> * mag_power<10, -1>);

// the stored absolute form is returned verbatim
static_assert(get_standard_uncertainty(iau::newtonian_constant_of_gravitation) ==
              mag_ratio<15, 10> * mag_power<10, -15> * cubic(si::metre) / si::kilogram / square(si::second));

// `measurement_of` returns exactly 1 of the constant in an uncertainty-capable representation;
// the uncertainty is relational and materializes only when a conversion leaves units in which
// the constant cancels
constexpr quantity G_measured = measurement_of(iau::newtonian_constant_of_gravitation);
static_assert(std::is_same_v<std::remove_const_t<decltype(G_measured)>,
                             quantity<iau::newtonian_constant_of_gravitation, uncertain<double>>>);
static_assert(G_measured.numerical_value_ref_in(G_measured.unit).value() == 1.0);
static_assert(G_measured.numerical_value_ref_in(G_measured.unit).uncertainty() == 0.0);

constexpr quantity G_measured_float = measurement_of<float>(iau::newtonian_constant_of_gravitation);
static_assert(std::is_same_v<std::remove_const_t<decltype(G_measured_float)>::rep, uncertain<float>>);

// asking for the measurement of a constant that is exact by definition is a compile-time error
template<auto C>
concept can_be_measured = requires { measurement_of(C); };
static_assert(can_be_measured<iau::newtonian_constant_of_gravitation>);
static_assert(!can_be_measured<si::si2019::speed_of_light_in_vacuum>);
static_assert(!can_be_measured<si::metre>);

// likewise, there is no valid relative standard uncertainty for an exact constant
template<auto C>
concept has_relative_standard_uncertainty = requires { get_relative_standard_uncertainty(C); };
static_assert(has_relative_standard_uncertainty<iau::newtonian_constant_of_gravitation>);
static_assert(!has_relative_standard_uncertainty<si::si2019::speed_of_light_in_vacuum>);

// HEP measured constants carry the uncertainty published by their own CODATA release, so the same
// constant has a different one per namespace. The derived `u_r` is the exact ratio of the two
// published magnitudes; materializing it into a floating-point type need not reproduce a decimal
// literal bit for bit on every platform, so these are compared with `approx_equal`.
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::electron_mass)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2022::electron_mass)>>);
// CODATA 2018: 0.510 998 950 00(15) MeV/c²
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2018::electron_mass)) * one,
                           1.5e-10 / 0.51099895 * one));
// CODATA 2022: 0.510 998 950 69(16) MeV/c²
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2022::electron_mass)) * one,
                           1.6e-10 / 0.51099895069 * one));
// CODATA 2014: 0.510 998 9461(31) MeV/c²
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2014::electron_mass)) * one,
                           3.1e-9 / 0.5109989461 * one));

// the fine-structure constant is dimensionless, so its uncertainty is still ordinary metadata
// (CODATA 2022: 7.297 352 5643(11) × 10⁻³)
static_assert(
  approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2022::fine_structure_constant)) * one,
               1.1e-12 / 7.2973525643e-3 * one));

// k_B is exact since the 2019 SI redefinition, so the post-2019 releases carry no uncertainty while
// the pre-2019 one does
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2014::boltzmann_constant)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::boltzmann_constant)>>);

// μ₀ went the other way: exact by definition until the ampere was redefined through `e` in the
// 2019 SI, measured ever since
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::codata2014::permeability_of_vacuum)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::permeability_of_vacuum)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2022::permeability_of_vacuum)>>);
// CODATA 2018: 1.256 637 062 12(19) × 10⁻⁶ N/A²; CODATA 2022: 1.256 637 061 27(20) × 10⁻⁶ N/A²
static_assert(
  approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2018::permeability_of_vacuum)) * one,
               1.9e-16 / 1.25663706212e-6 * one));
static_assert(
  approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2022::permeability_of_vacuum)) * one,
               2.0e-16 / 1.25663706127e-6 * one));

// The SI-expressed values track the same history. They are separate CODATA table entries from the
// HEP ones above: `hep` declares its own system of quantities, so those constants are not even the
// same type, and before 2019 the two forms of a constant could carry different published `u_r`.
static_assert(!MeasuredConstant<std::remove_const_t<decltype(codata::codata2014::magnetic_constant)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::codata2018::magnetic_constant)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::codata2022::magnetic_constant)>>);
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(codata::codata2018::magnetic_constant)) *
                             one,
                           1.9e-16 / 1.25663706212e-6 * one));
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(codata::codata2022::magnetic_constant)) *
                             one,
                           2.0e-16 / 1.25663706127e-6 * one));

// The newest adjustment is `inline`, so the unqualified name is the current value. `si` keeps a
// deprecated `magnetic_constant` holding the pre-2019 exact value (not named here, since referring
// to it would trip the deprecation warning under `-Werror`).
static_assert(codata::magnetic_constant == codata::codata2022::magnetic_constant);
static_assert(codata::codata2018::magnetic_constant != codata::codata2022::magnetic_constant);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::magnetic_constant)>>);

// IAU imports G rather than duplicating it. This must stay a using-declaration: the planetary mass
// units divide by G, and `M_SUN -> M_EARTH` is exact only because the very same type cancels.
static_assert(std::is_same_v<std::remove_const_t<decltype(iau::newtonian_constant_of_gravitation)>,
                             std::remove_const_t<decltype(codata::newtonian_constant_of_gravitation)>>);
static_assert(detail::conversion_relative_uncertainty(iau::solar_mass, iau::terrestrial_mass) == 0.0L);
// the unqualified name resolves through the inline namespace to the CODATA 2018 one
static_assert(hep::permeability_of_vacuum == hep::codata2018::permeability_of_vacuum);

// the constants that are exact by definition stay unannotated
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::speed_of_light_in_vacuum)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::planck_constant)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::avogadro_constant)>>);

// The derivation of a conversion factor's relative standard uncertainty is fully `consteval`,
// so its rules are pinned at compile time here. The fold into `uncertain` itself goes through
// `hypot` (not constexpr before C++26) and is covered by the runtime tests.
namespace conversion_uncertainty {

// u_r(solar_mass -> kg) is exactly G's contribution: (GM)ᴺ is exact, |−1| · u_r(G), where
// u_r(G) derives from the published absolute uncertainty as 1.5e-15 / 6.6743e-11
static_assert(approx_equal(detail::conversion_relative_uncertainty(iau::solar_mass, si::kilogram) * one,
                           1.5e-15L / 6.6743e-11L * one, 1e-6));

// a constant shared by both units cancels symbolically: (GM)☉ᴺ/G vs (GM)⊕ᴺ/G
static_assert(detail::conversion_relative_uncertainty(iau::solar_mass, iau::terrestrial_mass) == 0.0L);

// the direction of the conversion does not matter
static_assert(detail::conversion_relative_uncertainty(si::kilogram, iau::solar_mass) ==
              detail::conversion_relative_uncertainty(iau::solar_mass, si::kilogram));

// units free of measured constants convert exactly
static_assert(detail::conversion_relative_uncertainty(si::kilo<si::metre>, si::metre) == 0.0L);

// a fractional power scales the constant's contribution
static_assert(approx_equal(detail::conversion_relative_uncertainty(sqrt(iau::newtonian_constant_of_gravitation),
                                                                   sqrt(cubic(si::metre) / si::kilogram /
                                                                        square(si::second))) *
                             one,
                           0.5L * (1.5e-15L / 6.6743e-11L) * one, 1e-6));

// local constants exercising the tree-walk rules directly
inline constexpr struct measured_base final :
    named_constant<"tmb", mag<2> * si::metre, relative_standard_uncertainty{mag_ratio<1, 1> * mag_power<10, -3>}> {
} measured_base;

// an unannotated constant defined in terms of a measured one inherits its uncertainty by recursion
inline constexpr struct unannotated_wrapper final : named_constant<"tuw", mag<3> * measured_base> {
} unannotated_wrapper;
static_assert(approx_equal(detail::conversion_relative_uncertainty(unannotated_wrapper, si::metre) * one, 1e-3L * one,
                           1e-6));

// an annotated constant is NOT recursed: its published uncertainty covers the whole value, so the
// nested measured constant must not be double-counted (5e-3, not hypot(5e-3, 1e-3))
inline constexpr struct annotated_wrapper final :
    named_constant<"taw", mag<3> * measured_base, relative_standard_uncertainty{mag_ratio<5, 1> * mag_power<10, -3>}> {
} annotated_wrapper;
static_assert(approx_equal(detail::conversion_relative_uncertainty(annotated_wrapper, si::metre) * one, 5e-3L * one,
                           1e-6));

// independent measured constants combine in quadrature: u_r = √((1e-3)² + (5e-3)²) ≈ 5.0990e-3
inline constexpr struct another_measured final :
    named_constant<"tam", mag<7> * si::second, relative_standard_uncertainty{mag_ratio<5, 1> * mag_power<10, -3>}> {
} another_measured;
static_assert(
  approx_equal(detail::conversion_relative_uncertainty(measured_base * another_measured, si::metre* si::second) * one,
               5.0990195e-3L * one, 1e-6));

// the same constant on both sides of a derived unit cancels within a single unit expression
static_assert(detail::conversion_relative_uncertainty(measured_base / measured_base, one) == 0.0L);

}  // namespace conversion_uncertainty

// A constant may declare its uncertainty in either the relative or the absolute form, whichever
// its source publishes, and each accessor derives the missing form on demand.
namespace uncertainty_wrappers {

// the absolute form derives the relative one for the RSU-declared constant above:
// u(x) = u_r · |x| = 1e-3 · 2 m = 2e-3 m
static_assert(equivalent(get_standard_uncertainty(conversion_uncertainty::measured_base),
                         mag_ratio<2, 1000>* si::metre));

// electron magnetic moment, CODATA 2022: −9.284 764 6917(29) × 10⁻²⁴ J/T; a negative-valued
// constant declared with the absolute form
inline constexpr struct electron_magnetic_moment final :
    named_constant<symbol_text{u8"μ_e", "mu_e"},
                   -mag_ratio<92'847'646'917, 10'000'000'000> * mag_power<10, -24> * si::joule / si::tesla,
                   standard_uncertainty{mag_ratio<29, 10> * mag_power<10, -33> * si::joule / si::tesla}> {
} electron_magnetic_moment;

static_assert(MeasuredConstant<std::remove_const_t<decltype(electron_magnetic_moment)>>);

// the declared uncertainty is returned verbatim and is positive by construction
static_assert(get_standard_uncertainty(electron_magnetic_moment) ==
              mag_ratio<29, 10> * mag_power<10, -33> * si::joule / si::tesla);

// `u_r = u(x)/|x|` is a magnitude, never negative (GUM/VIM), and derives as the exact ratio
// 2.9e-33 / 9.2847646917e-24 = 29/92'847'646'917
static_assert(get_relative_standard_uncertainty(electron_magnetic_moment) == mag_ratio<29, 92'847'646'917>);
static_assert(detail::magnitude_is_positive<get_relative_standard_uncertainty(electron_magnetic_moment)>);

// a negative-valued constant declared with the relative form derives a positive absolute one:
// u(x) = u_r · |x| = 1e-3 · 2 m = 2e-3 m
inline constexpr struct negative_measured final :
    named_constant<"tnm", -mag<2> * si::metre, relative_standard_uncertainty{mag_ratio<1, 1> * mag_power<10, -3>}> {
} negative_measured;
static_assert(equivalent(get_standard_uncertainty(negative_measured), mag_ratio<2, 1000>* si::metre));
static_assert(detail::magnitude_is_positive<get_relative_standard_uncertainty(negative_measured)>);

// the conversion engine folds the uncertainty of an absolute-form constant exactly as it does
// for a relative-form one
static_assert(approx_equal(detail::conversion_relative_uncertainty(electron_magnetic_moment, si::joule / si::tesla) *
                             one,
                           2.9e-33L / 9.2847646917e-24L * one, 1e-6));

// an uncertainty of a different dimension than the constant, or a non-positive one, is not a
// valid definition
template<auto U, auto SU>
concept valid_measured_constant = requires { named_constant<"test", U, standard_uncertainty{SU}>{}; };
static_assert(valid_measured_constant<mag<2> * si::metre, mag_ratio<1, 1000> * si::metre>);
static_assert(!valid_measured_constant<mag<2> * si::metre, mag_ratio<1, 1000> * si::second>);
static_assert(!valid_measured_constant<mag<2> * si::metre, -mag_ratio<1, 1000> * si::metre>);

}  // namespace uncertainty_wrappers

}  // namespace
