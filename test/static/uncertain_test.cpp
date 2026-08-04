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

// only constants defined with `relative_standard_uncertainty` are measured constants
static_assert(MeasuredConstant<std::remove_const_t<decltype(iau::newtonian_constant_of_gravitation)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(si::si2019::speed_of_light_in_vacuum)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(iau::nominal_solar_mass_parameter)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(si::metre)>>);

// A published `u_r` is a two-significant-digit decimal, and materializing it into a floating-point
// type from an exact magnitude need not reproduce the literal bit for bit on every platform, so
// these are compared with `approx_equal` rather than with `==`.
// CODATA 2018: G = 6.674 30(15) × 10⁻¹¹ m³ kg⁻¹ s⁻², u_r = 2.2 × 10⁻⁵
static_assert(approx_equal(
  get_value<double>(get_relative_standard_uncertainty(iau::newtonian_constant_of_gravitation)) * one, 2.2e-5 * one));

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
// constant has a different one per namespace
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::electron_mass)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2022::electron_mass)>>);
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2018::electron_mass)) * one,
                           3.0e-10 * one));
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2022::electron_mass)) * one,
                           3.1e-10 * one));
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(hep::codata2014::electron_mass)) * one,
                           6.2e-9 * one));

// the fine-structure constant is dimensionless, so its uncertainty is still ordinary metadata
static_assert(approx_equal(
  get_value<double>(get_relative_standard_uncertainty(hep::codata2022::fine_structure_constant)) * one, 1.6e-10 * one));

// k_B is exact since the 2019 SI redefinition, so the post-2019 releases carry no uncertainty while
// the pre-2019 one does
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2014::boltzmann_constant)>>);
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::boltzmann_constant)>>);

// μ₀ went the other way: exact by definition until the ampere was redefined through `e` in the
// 2019 SI, measured ever since
static_assert(!MeasuredConstant<std::remove_const_t<decltype(hep::codata2014::permeability_of_vacuum)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2018::permeability_of_vacuum)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(hep::codata2022::permeability_of_vacuum)>>);
static_assert(approx_equal(
  get_value<double>(get_relative_standard_uncertainty(hep::codata2018::permeability_of_vacuum)) * one, 1.5e-10 * one));
static_assert(approx_equal(
  get_value<double>(get_relative_standard_uncertainty(hep::codata2022::permeability_of_vacuum)) * one, 1.6e-10 * one));

// The SI-expressed values track the same history. They are separate CODATA table entries from the
// HEP ones above: `hep` declares its own system of quantities, so those constants are not even the
// same type, and before 2019 the two forms of a constant could carry different published `u_r`.
static_assert(!MeasuredConstant<std::remove_const_t<decltype(codata::codata2014::magnetic_constant)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::codata2018::magnetic_constant)>>);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::codata2022::magnetic_constant)>>);
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(codata::codata2018::magnetic_constant)) *
                             one,
                           1.5e-10 * one));
static_assert(approx_equal(get_value<double>(get_relative_standard_uncertainty(codata::codata2022::magnetic_constant)) *
                             one,
                           1.6e-10 * one));

// The newest adjustment is `inline`, so the unqualified name is the current value. `si` keeps a
// deprecated `magnetic_constant` holding the pre-2019 exact value (not named here, since referring
// to it would trip the deprecation warning under `-Werror`).
static_assert(codata::magnetic_constant == codata::codata2022::magnetic_constant);
static_assert(codata::codata2018::magnetic_constant != codata::codata2022::magnetic_constant);
static_assert(MeasuredConstant<std::remove_const_t<decltype(codata::magnetic_constant)>>);
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

// u_r(solar_mass -> kg) is exactly G's contribution: (GM)ᴺ is exact, |−1| · u_r(G)
static_assert(approx_equal(detail::conversion_relative_uncertainty(iau::solar_mass, si::kilogram) * one, 2.2e-5L * one,
                           1e-6));

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
                           0.5L * 2.2e-5L * one, 1e-6));

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

}  // namespace
