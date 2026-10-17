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
#include <mp-units/ext/type_traits.h>
#include <mp-units/framework.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif

namespace {

using namespace mp_units;

using dimensionless_ = struct dimensionless;
using dim_one_ = struct dimension_one;

// clang-format off
inline constexpr struct dim_length_ final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass_ final : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time_ final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_current_ final : base_dimension<"I"> {} dim_electric_current;
inline constexpr struct dim_thermodynamic_temperature_ final : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_thermodynamic_temperature;
inline constexpr struct dim_amount_of_substance_ final : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity_ final : base_dimension<"J"> {} dim_luminous_intensity;

// quantities specification
QUANTITY_SPEC_(length, dim_length, non_negative);
QUANTITY_SPEC_(mass, dim_mass, non_negative);
QUANTITY_SPEC_(time, dim_time, non_negative);
inline constexpr auto duration = time;
QUANTITY_SPEC_(thermodynamic_temperature, dim_thermodynamic_temperature, non_negative);
QUANTITY_SPEC_(amount_of_substance, dim_amount_of_substance, non_negative);
QUANTITY_SPEC_(luminous_intensity, dim_luminous_intensity, non_negative);
QUANTITY_SPEC_(electric_current, dim_electric_current);

inline constexpr struct second_ final : named_unit<"s", kind_of<time>> {} second;

QUANTITY_SPEC_(horizontal_length, length);
QUANTITY_SPEC_(height, length);
QUANTITY_SPEC_(width, length);
QUANTITY_SPEC_(radius, width);
QUANTITY_SPEC_(path_length, length);
inline constexpr auto arc_length = path_length;
QUANTITY_SPEC_(distance, path_length);
QUANTITY_SPEC_(wavelength, length);
QUANTITY_SPEC_(position_vector, length, quantity_tensor_order::vector);
QUANTITY_SPEC_(displacement, length, quantity_tensor_order::vector);
QUANTITY_SPEC_(period_duration, time);
QUANTITY_SPEC_(rotation, dimensionless);
QUANTITY_SPEC_(repetency, inverse(wavelength), non_negative);
QUANTITY_SPEC_(frequency, inverse(period_duration), non_negative);
QUANTITY_SPEC_(activity, inverse(time), non_negative);
QUANTITY_SPEC_(area, pow<2>(length), non_negative);
QUANTITY_SPEC_(volume, pow<3>(length), non_negative);
QUANTITY_SPEC_(angular_measure, dimensionless, arc_length / radius, is_kind);
QUANTITY_SPEC_(special_angular_measure, angular_measure);
QUANTITY_SPEC_(rotational_displacement, angular_measure, path_length / radius);
QUANTITY_SPEC_(phase_angle, angular_measure);
QUANTITY_SPEC_(solid_angular_measure, dimensionless, area / pow<2>(radius), is_kind, non_negative);
QUANTITY_SPEC_(dimensionless_rate, dimensionless / time);
QUANTITY_SPEC_(angular_measure_rate, angular_measure / time);
QUANTITY_SPEC_(solid_angular_measure_rate, solid_angular_measure / time);
QUANTITY_SPEC_(speed, length / time, non_negative);
QUANTITY_SPEC_(velocity, speed, displacement / time);
QUANTITY_SPEC_(special_speed, speed);
QUANTITY_SPEC_(north_speed, speed);
QUANTITY_SPEC_(east_speed, speed);
QUANTITY_SPEC_(horizontal_speed, speed, horizontal_length / time);
QUANTITY_SPEC_(rate_of_climb, speed, height / time);
QUANTITY_SPEC_(special_rate_of_climb, rate_of_climb);
QUANTITY_SPEC_(acceleration, velocity / time);
QUANTITY_SPEC_(acceleration_of_free_fall, acceleration);
QUANTITY_SPEC_(mass_density, mass / volume, non_negative);
QUANTITY_SPEC_(force, mass * acceleration);
QUANTITY_SPEC_(weight, force, mass * acceleration_of_free_fall);
QUANTITY_SPEC_(moment_of_force, position_vector* force);
QUANTITY_SPEC_(torque, moment_of_force, quantity_tensor_order::scalar);
QUANTITY_SPEC_(pressure, force / area, quantity_tensor_order::scalar);
QUANTITY_SPEC_(stress, pressure, quantity_tensor_order::tensor);
QUANTITY_SPEC_(normal_stress, stress, quantity_tensor_order::scalar);
QUANTITY_SPEC_(strain, dimensionless, quantity_tensor_order::tensor);
QUANTITY_SPEC_(relative_linear_strain, length / length);
QUANTITY_SPEC_(modulus_of_elasticity, normal_stress / relative_linear_strain);
QUANTITY_SPEC_(dynamic_viscosity, pressure* time);
QUANTITY_SPEC_(power, force* velocity, quantity_tensor_order::scalar);
QUANTITY_SPEC_(efficiency, power / power);
QUANTITY_SPEC_(energy, mass * pow<2>(length) / pow<2>(time), non_negative);
QUANTITY_SPEC_(mechanical_work, energy, force* displacement, quantity_tensor_order::scalar);
QUANTITY_SPEC_(mechanical_energy, mechanical_work, mass* pow<2>(length) / pow<2>(time));
QUANTITY_SPEC_(potential_energy, mechanical_energy);
QUANTITY_SPEC_(gravitational_potential_energy, potential_energy, mass * acceleration_of_free_fall * height,
               quantity_tensor_order::scalar);
QUANTITY_SPEC_(gravitational_constant, force * pow<2>(length) / pow<2>(mass));
QUANTITY_SPEC_(newtonian_gravitational_potential_energy, potential_energy, gravitational_constant * pow<2>(mass) / length,
               quantity_tensor_order::scalar);
QUANTITY_SPEC_(kinetic_energy, mechanical_energy, mass* pow<2>(speed));
QUANTITY_SPEC_(electric_charge, electric_current* time);
QUANTITY_SPEC_(electric_field_strength, force / electric_charge);  // vector
QUANTITY_SPEC_(electric_potential, electric_field_strength* length, quantity_tensor_order::scalar);
QUANTITY_SPEC_(voltage, electric_potential);
QUANTITY_SPEC_(electromagnetism_power, power, voltage* electric_current);
QUANTITY_SPEC_(electric_current_phasor, electric_current, quantity_field::complex);
QUANTITY_SPEC_(voltage_phasor, voltage, quantity_field::complex);
QUANTITY_SPEC_(active_power, power, inverse(period_duration) * (electromagnetism_power * time));
QUANTITY_SPEC_(complex_power, voltage_phasor* electric_current_phasor);  // separate kind
QUANTITY_SPEC_(apparent_power, complex_power, quantity_field::real);
QUANTITY_SPEC_(capacitance, electric_charge / voltage);
QUANTITY_SPEC_(impedance, voltage_phasor / electric_current_phasor);    // complex
QUANTITY_SPEC_(resistance, impedance, voltage / electric_current);
QUANTITY_SPEC_(storage_capacity, dimensionless, is_kind);
QUANTITY_SPEC_(transfer_rate, storage_capacity / time);
QUANTITY_SPEC_(pixel_count, dimensionless);
QUANTITY_SPEC_(pixel_x, pixel_count);
QUANTITY_SPEC_(pixel_y, pixel_count);
QUANTITY_SPEC_(pixel_area, dimensionless, pixel_x * pixel_y);

// V2 workaround: altitude and depth are signed coordinates (can be negative)
QUANTITY_SPEC_(altitude, length);                    // signed vertical coordinate
inline constexpr auto depth = altitude;              // signed vertical coordinate

// Override is_non_negative() for signed coordinates
// Even though they inherit from non-negative length, they represent signed positions
[[nodiscard]] consteval bool is_non_negative(decltype(altitude)) { return false; }

// clang-format on
static_assert(QuantitySpec<length_>);
static_assert(detail::NamedQuantitySpec<length_>);
static_assert(!detail::DerivedQuantitySpec<length_>);
static_assert(!detail::QuantityKindSpec<length_>);

static_assert(QuantitySpec<frequency_>);
static_assert(detail::NamedQuantitySpec<frequency_>);
static_assert(!detail::DerivedQuantitySpec<frequency_>);
static_assert(!detail::QuantityKindSpec<frequency_>);

static_assert(QuantitySpec<decltype(inverse(time))>);
static_assert(!detail::NamedQuantitySpec<decltype(inverse(time))>);
static_assert(detail::DerivedQuantitySpec<decltype(inverse(time))>);
static_assert(!detail::QuantityKindSpec<decltype(inverse(time))>);

static_assert(QuantitySpec<kind_of_<length_>>);
static_assert(!detail::NamedQuantitySpec<kind_of_<length_>>);
static_assert(!detail::DerivedQuantitySpec<kind_of_<length_>>);
static_assert(detail::QuantityKindSpec<kind_of_<length_>>);

static_assert(QuantitySpec<frequency_>);
static_assert(detail::NamedQuantitySpec<frequency_>);
static_assert(!detail::DerivedQuantitySpec<frequency_>);
static_assert(!detail::QuantityKindSpec<frequency_>);

static_assert(QuantitySpec<decltype(inverse(time))>);
static_assert(!detail::NamedQuantitySpec<decltype(inverse(time))>);
static_assert(detail::DerivedQuantitySpec<decltype(inverse(time))>);
static_assert(!detail::QuantityKindSpec<decltype(inverse(time))>);

static_assert(QuantitySpec<decltype(length / time)>);
static_assert(!detail::NamedQuantitySpec<decltype(length / time)>);
static_assert(detail::DerivedQuantitySpec<decltype(length / time)>);
static_assert(!detail::QuantityKindSpec<decltype(length / time)>);

static_assert(QuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(!detail::NamedQuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(detail::DerivedQuantitySpec<kind_of_<decltype(length / time)>>);
static_assert(detail::QuantityKindSpec<kind_of_<decltype(length / time)>>);

static_assert(QuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> / kind_of<time>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> / kind_of<time>)>);

static_assert(QuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> * kind_of<time>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> * kind_of<time>)>);

static_assert(QuantitySpec<decltype(kind_of<length> * time)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> * time)>);
static_assert(!detail::QuantityKindSpec<decltype(kind_of<length> * time)>);

static_assert(QuantitySpec<decltype(length * kind_of<time>)>);
static_assert(!detail::NamedQuantitySpec<decltype(length * kind_of<time>)>);
static_assert(detail::DerivedQuantitySpec<decltype(length * kind_of<time>)>);
static_assert(!detail::QuantityKindSpec<decltype(length * kind_of<time>)>);

// dimensionless
static_assert(QuantitySpec<dimensionless_>);
static_assert(detail::NamedQuantitySpec<dimensionless_>);
static_assert(!detail::DerivedQuantitySpec<dimensionless_>);
static_assert(!detail::QuantityKindSpec<dimensionless_>);

static_assert(QuantitySpec<decltype(length / length)>);
static_assert(detail::NamedQuantitySpec<decltype(length / length)>);
static_assert(!detail::DerivedQuantitySpec<decltype(length / length)>);
static_assert(!detail::QuantityKindSpec<decltype(length / length)>);

static_assert(QuantitySpec<decltype(width / length)>);
static_assert(!detail::NamedQuantitySpec<decltype(width / length)>);
static_assert(detail::DerivedQuantitySpec<decltype(width / length)>);
static_assert(!detail::QuantityKindSpec<decltype(width / length)>);

static_assert(QuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(!detail::DerivedQuantitySpec<decltype(kind_of<length> / kind_of<length>)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<length> / kind_of<length>)>);

// a kind factor and a typed factor of one tree are different factors, like `width / length`
static_assert(QuantitySpec<decltype(kind_of<length> / length)>);
static_assert(!detail::NamedQuantitySpec<decltype(kind_of<length> / length)>);
static_assert(detail::DerivedQuantitySpec<decltype(kind_of<length> / length)>);
static_assert(!detail::QuantityKindSpec<decltype(kind_of<length> / length)>);

static_assert(QuantitySpec<decltype(length / kind_of<length>)>);
static_assert(!detail::NamedQuantitySpec<decltype(length / kind_of<length>)>);
static_assert(detail::DerivedQuantitySpec<decltype(length / kind_of<length>)>);
static_assert(!detail::QuantityKindSpec<decltype(length / kind_of<length>)>);

static_assert(QuantitySpec<decltype(width / kind_of<length>)>);
static_assert(!detail::NamedQuantitySpec<decltype(width / kind_of<length>)>);
static_assert(detail::DerivedQuantitySpec<decltype(width / kind_of<length>)>);
static_assert(!detail::QuantityKindSpec<decltype(width / kind_of<length>)>);

// length
static_assert(QuantitySpec<decltype(speed * time)>);
static_assert(!detail::NamedQuantitySpec<decltype(speed * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(speed * time)>);

// derived QuantitySpec expression template syntax verification
static_assert(!detail::NamedQuantitySpec<decltype(speed * time)>);
static_assert(detail::DerivedQuantitySpec<decltype(speed * time)>);

static_assert(is_of_type<dimensionless * time, time_>);
static_assert(is_of_type<time * dimensionless, time_>);
static_assert(is_of_type<dimensionless*(inverse(time)), derived_quantity_spec<dimensionless_, per<time_>>>);
static_assert(is_of_type<inverse(time) * dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / length, dimensionless_>);
static_assert(is_of_type<pow<2>(length / length), dimensionless_>);
static_assert(is_of_type<pow<-1>(time), derived_quantity_spec<dimensionless_, per<time_>>>);
static_assert(is_of_type<pow<-1>(length / time), derived_quantity_spec<time_, per<length_>>>);
static_assert(is_of_type<pow<-1>(pow<-1>(time)), time_>);
static_assert(std::is_same_v<decltype(pow<-1>(time)), decltype(inverse(time))>);
static_assert(pow<-1>(kind_of<length>) == kind_of<dimensionless / length>);
static_assert(is_of_type<pow<2>(length) / pow<2>(length), dimensionless_>);

static_assert(is_of_type<pow<2>(length) / length, length_>);
static_assert(is_of_type<length / pow<2>(length), derived_quantity_spec<dimensionless_, per<length_>>>);

static_assert(is_of_type<length / height, derived_quantity_spec<length_, per<height_>>>);

static_assert(is_of_type<length * time, derived_quantity_spec<length_, time_>>);
static_assert(is_of_type<length * length, derived_quantity_spec<mp_units::power<length_, 2>>>);

static_assert(is_of_type<length * length * time, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<length * time * length, derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<pixel_x * pixel_y, derived_quantity_spec<pixel_x_, pixel_y_>>);

static_assert(is_of_type<length*(time* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);
static_assert(is_of_type<time*(length* length), derived_quantity_spec<mp_units::power<length_, 2>, time_>>);

static_assert(is_of_type<inverse(time) * length, derived_quantity_spec<length_, per<time_>>>);
static_assert(is_of_type<length * inverse(time), derived_quantity_spec<length_, per<time_>>>);
static_assert(is_of_type<inverse(time) * time, dimensionless_>);

static_assert(is_of_type<inverse(length) / inverse(width), derived_quantity_spec<width_, per<length_>>>);
static_assert(is_of_type<dimensionless / (time / length), derived_quantity_spec<length_, per<time_>>>);

static_assert(is_of_type<time / dimensionless, time_>);
static_assert(is_of_type<inverse(time) / dimensionless, derived_quantity_spec<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / time * time, length_>);
static_assert(
  is_of_type<inverse(time) * inverse(time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(time* time), derived_quantity_spec<dimensionless_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<inverse(inverse(time* time)), derived_quantity_spec<mp_units::power<time_, 2>>>);

static_assert(
  is_of_type<length / time * inverse(time), derived_quantity_spec<length_, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(length / time),
                         derived_quantity_spec<mp_units::power<length_, 2>, per<mp_units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(time / length), dimensionless_>);

static_assert(is_of_type<speed / acceleration, derived_quantity_spec<speed_, per<acceleration_>>>);
static_assert(is_of_type<get_dimension(speed / acceleration), dim_time_>);
static_assert(is_of_type<acceleration / speed, derived_quantity_spec<acceleration_, per<speed_>>>);
static_assert(is_of_type<get_dimension(acceleration / speed), derived_dimension<dim_one_, per<dim_time_>>>);
static_assert(is_of_type<speed * speed / length, derived_quantity_spec<mp_units::power<speed_, 2>, per<length_>>>);
static_assert(
  is_of_type<get_dimension(speed* speed / length), derived_dimension<dim_length_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(
  is_of_type<inverse(speed* speed) * length, derived_quantity_spec<length_, per<mp_units::power<speed_, 2>>>>);
static_assert(is_of_type<get_dimension(inverse(speed* speed) * length),
                         derived_dimension<mp_units::power<dim_time_, 2>, per<dim_length_>>>);

static_assert(is_of_type<(length * length) * (time * time),
                         derived_quantity_spec<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);
static_assert(is_of_type<(time * time) * (length * length),
                         derived_quantity_spec<mp_units::power<length_, 2>, mp_units::power<time_, 2>>>);

static_assert(is_of_type<length * time * time, derived_quantity_spec<length_, mp_units::power<time_, 2>>>);
static_assert(
  is_of_type<mass / length / time / time, derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / (length * time * time), derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);
static_assert(
  is_of_type<mass / length / (time * time), derived_quantity_spec<mass_, per<length_, mp_units::power<time_, 2>>>>);

static_assert(is_of_type<force / area, derived_quantity_spec<force_, per<area_>>>);
static_assert(is_of_type<get_dimension(force / area),
                         derived_dimension<dim_mass_, per<dim_length_, mp_units::power<dim_time_, 2>>>>);

static_assert(is_of_type<mass * acceleration, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<get_dimension(mass* acceleration),
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);
static_assert(is_of_type<acceleration * mass, derived_quantity_spec<acceleration_, mass_>>);
static_assert(is_of_type<get_dimension(acceleration* mass),
                         derived_dimension<dim_length_, dim_mass_, per<mp_units::power<dim_time_, 2>>>>);

// exponents of a base dimension contributed by three or more factors (both directly and through
// the `per<>` part of another factor's derived dimension) must consolidate
static_assert(is_of_type<get_dimension(length* speed / pressure),
                         derived_dimension<mp_units::power<dim_length_, 3>, dim_time_, per<dim_mass_>>>);
// the Reynolds number equation: ρvl/η
static_assert(is_of_type<get_dimension(mass_density * speed * length / dynamic_viscosity), dim_one_>);
static_assert(get_dimension(mass_density * speed * length / dynamic_viscosity) == dimension_one);

static_assert(is_of_type<kind_of<length> / kind_of<time>, kind_of_<derived_quantity_spec<length_, per<time_>>>>);
static_assert(is_of_type<kind_of<length / time>, kind_of_<derived_quantity_spec<length_, per<time_>>>>);

// power
static_assert(is_of_type<pow<0>(length), dimensionless_>);
static_assert(is_of_type<pow<1>(length), length_>);
static_assert(is_of_type<pow<2, 2>(length), length_>);
static_assert(is_of_type<pow<2>(dimensionless), dimensionless_>);
static_assert(is_of_type<pow<2>(length), derived_quantity_spec<mp_units::power<length_, 2>>>);
static_assert(is_of_type<pow<1, 2>(length), derived_quantity_spec<mp_units::power<length_, 1, 2>>>);
static_assert(is_of_type<pow<1, 2>(length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length * length * length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length), derived_quantity_spec<mp_units::power<length_, 2, 3>>>);
static_assert(is_of_type<pow<1, 2>(length / time),
                         derived_quantity_spec<mp_units::power<length_, 1, 2>, per<mp_units::power<time_, 1, 2>>>>);
static_assert(
  is_of_type<pow<1, 2>(length / (time * time)), derived_quantity_spec<mp_units::power<length_, 1, 2>, per<time_>>>);

static_assert(std::is_same_v<decltype(pow<2>(length)), decltype(length * length)>);
static_assert(std::is_same_v<decltype(pow<2>(length / time)), decltype(length * length / time / time)>);

static_assert(dimensionless * dimensionless == dimensionless);
static_assert(is_of_type<dimensionless * dimensionless, dimensionless_>);


template<auto& t>
concept invalid_operations = requires {
  requires !requires { t < t; };
  requires !requires { t / 2; };
  requires !requires { 2 * t; };
  requires !requires { t * 2; };
  requires !requires { t + 2; };
  requires !requires { 2 + t; };
  requires !requires { t + t; };
  requires !requires { t - 2; };
  requires !requires { 2 - t; };
  requires !requires { t - t; };
  requires !requires { t == 2; };
  requires !requires { 2 == t; };
  requires !requires { t < 2; };
  requires !requires { 2 < t; };
  requires !requires { t + time[second]; };
  requires !requires { t - time[second]; };
  requires !requires { t * time[second]; };
  requires !requires { t / time[second]; };
  requires !requires { t == time[second]; };
  requires !requires { t < time[second]; };
  requires !requires { time[second] + t; };
  requires !requires { time[second] - t; };
  requires !requires { time[second] * t; };
  requires !requires { time[second] / t; };
  requires !requires { time[second] == t; };
  requires !requires { time[second] < t; };
  requires !requires { t + 1 * time[second]; };
  requires !requires { t - 1 * time[second]; };
  requires !requires { t * (1 * time[second]); };
  requires !requires { t / (1 * time[second]); };
  requires !requires { t == 1 * time[second]; };
  requires !requires { t < 1 * time[second]; };
  requires !requires { 1 * time[second] + t; };
  requires !requires { 1 * time[second] - t; };
  requires !requires { 1 * time[second] * t; };
  requires !requires { 1 * time[second] == t; };
  requires !requires { 1 * time[second] < t; };
};
static_assert(invalid_operations<time>);

// comparisons
static_assert(mass * acceleration == acceleration * mass);
static_assert(length / length == width / width);
static_assert(length / length == time / time);
static_assert(length * time / time == length);

static_assert(length != time);
static_assert(length != width);
static_assert(acceleration != speed);

static_assert(frequency != activity);
static_assert(efficiency != strain);
static_assert(dimensionless != efficiency);
static_assert(power / power != efficiency);
static_assert(stress / stress != strain);
static_assert(stress / stress != efficiency);

static_assert(rate_of_climb != speed);
static_assert(rate_of_climb != distance / time);
static_assert(rate_of_climb != length / time);

static_assert(velocity != speed);
static_assert(energy != torque);

static_assert(inverse(time) != frequency);
static_assert(inverse(frequency) != time);
static_assert(frequency * time != dimensionless);
static_assert(length * length != area);
static_assert(length * length != volume);
static_assert(area / length != length);
static_assert(length * length * length != volume);
static_assert(area * length != volume);
static_assert(volume / length != area);
static_assert(volume / length / length != length);
static_assert(area * area / length != volume);
static_assert(area * (area / length) != volume);
static_assert(volume / (length * length) != length);
static_assert(force * length != energy);
static_assert(force * length != torque);
static_assert(force * position_vector != energy);
static_assert(force * position_vector != torque);
static_assert(length / time != speed);
static_assert(displacement / time != speed);
static_assert(length / time != velocity);
static_assert(displacement / time != velocity);

static_assert(length * time / period_duration != time);
static_assert(length * height / width != length);

static_assert(length * time != speed);
static_assert(length / time / time != speed);
static_assert(length / speed != time);
static_assert(speed * time != length);

static_assert(length / time / time != acceleration);
static_assert(displacement / time / time != acceleration);
static_assert(displacement / (time * time) != acceleration);
static_assert(velocity / time != acceleration);
static_assert(velocity / acceleration != time);
static_assert(acceleration * time != velocity);
static_assert(acceleration * (time * time) != displacement);
static_assert(acceleration / speed != frequency);

// get_kind
static_assert(get_kind(length) == kind_of<length>);
static_assert(get_kind(distance) == kind_of<length>);
static_assert(get_kind(time) == kind_of<time>);
static_assert(get_kind(period_duration) == kind_of<time>);
static_assert(get_kind(length / time) == kind_of<length / time>);
static_assert(get_kind(speed) == kind_of<speed>);
static_assert(get_kind(height / time) == kind_of<length / time>);
static_assert(get_kind(inverse(time)) == kind_of<inverse(time)>);
static_assert(get_kind(inverse(period_duration)) == kind_of<inverse(time)>);
// three or more factors mapping to the same kind must consolidate
static_assert(get_kind(width * height * path_length) == kind_of<pow<3>(length)>);
static_assert(get_kind(frequency) == kind_of<frequency>);
static_assert(get_kind(mass * frequency) == kind_of<mass * frequency>);
static_assert(get_kind(moment_of_force) == kind_of<moment_of_force>);
static_assert(get_kind(energy) == kind_of<energy>);
static_assert(get_kind(potential_energy) == kind_of<energy>);
static_assert(get_kind(kinetic_energy) == kind_of<energy>);
static_assert(get_kind(pow<1, 2>(area)) == kind_of<pow<1, 2>(area)>);
static_assert(get_kind(dimensionless) == kind_of<dimensionless>);
static_assert(get_kind(angular_measure) == kind_of<angular_measure>);
static_assert(get_kind(phase_angle) == kind_of<angular_measure>);
static_assert(get_kind(rotational_displacement) == kind_of<angular_measure>);

// kind factors: a quantity equation keeps what it knows about every factor. A typed operand stays
// typed and a kind operand stays open (a `kind_of_` factor), and only a result whose factors are
// all open is a kind of their product. Nothing a user states about one factor changes what is
// known about another.
static_assert(is_of_type<kind_of<length> / time, derived_quantity_spec<kind_of_<length_>, per<time_>>>);
static_assert(is_of_type<height / kind_of<time>, derived_quantity_spec<height_, per<kind_of_<time_>>>>);
static_assert(is_of_type<kind_of<length> * time, derived_quantity_spec<kind_of_<length_>, time_>>);
static_assert(is_of_type<time * kind_of<length>, derived_quantity_spec<kind_of_<length_>, time_>>);
// a kind factor and a typed factor of one tree are different factors, like `width / length`: they
// never merge, so the result of an equation does not depend on how it is bracketed
static_assert(is_of_type<kind_of<length> / length, derived_quantity_spec<kind_of_<length_>, per<length_>>>);
static_assert(is_of_type<kind_of<length> / height, derived_quantity_spec<kind_of_<length_>, per<height_>>>);
static_assert(is_of_type<kind_of<length> * length, derived_quantity_spec<length_, kind_of_<length_>>>);
static_assert(
  is_of_type<kind_of<length / time> * time, derived_quantity_spec<kind_of_<length_>, time_, per<kind_of_<time_>>>>);
static_assert(is_of_type<(kind_of<length> / length) * length, kind_of_<length_>>);
static_assert(is_of_type<kind_of<length>*(length / length), kind_of_<length_>>);
static_assert(
  is_of_type<(kind_of<length> / length) * height, derived_quantity_spec<height_, kind_of_<length_>, per<length_>>>);
static_assert(
  is_of_type<kind_of<length>*(height / length), derived_quantity_spec<height_, kind_of_<length_>, per<length_>>>);
static_assert(is_of_type<(height * kind_of<time>) / time, derived_quantity_spec<height_, kind_of_<time_>, per<time_>>>);
static_assert(is_of_type<(kind_of<length> / time) * (time / kind_of<length>), dimensionless_>);
static_assert(is_of_type<(height / kind_of<time>)*kind_of<time>, height_>);
static_assert(is_of_type<pow<2>(height / kind_of<time>),
                         derived_quantity_spec<mp_units::power<height_, 2>, per<mp_units::power<kind_of_<time_>, 2>>>>);
// all factors open: the kind of the product, as two kinds multiplied have always produced
static_assert(is_of_type<kind_of<length> * kind_of<time>, kind_of_<derived_quantity_spec<length_, time_>>>);
static_assert(is_of_type<(kind_of<length> * time) / time, kind_of_<length_>>);
static_assert(
  is_of_type<(height / kind_of<time>) / height, kind_of_<derived_quantity_spec<dimensionless_, per<time_>>>>);
static_assert(is_of_type<inverse(kind_of<length>), kind_of_<derived_quantity_spec<dimensionless_, per<length_>>>>);
static_assert(is_of_type<pow<-1>(kind_of<length>), kind_of_<derived_quantity_spec<dimensionless_, per<length_>>>>);
// the kind of `dimensionless` is the identity of the algebra, like `dimensionless` itself
static_assert(is_of_type<kind_of<dimensionless> * height, height_>);
static_assert(is_of_type<kind_of<dimensionless> * kind_of<length>, kind_of_<length_>>);
static_assert(is_of_type<kind_of<dimensionless> * kind_of<dimensionless>, kind_of_<dimensionless_>>);

static_assert(get_kind(kind_of<length> / time) == kind_of<length / time>);
static_assert(get_kind(height / kind_of<time>) == kind_of<length / time>);

// comparisons of the same dimensions
static_assert(length == length);
static_assert(speed == speed);

// comparisons of equivalent dimensions (named vs unnamed/derived)
static_assert(length / length == dimensionless);

using namespace detail;

using enum conversion_rank;

// an open factor converts as a kind does, a typed factor as a typed quantity does
static_assert(convertible(height / kind_of<time>, rate_of_climb) == implicit);
static_assert(convertible(height / kind_of<time>, speed) == implicit);
static_assert(convertible(height / kind_of<time>, horizontal_speed) == cast);  // the stated `height` is kept
static_assert(convertible(height / kind_of<time>, special_speed) == explicit_only);
static_assert(convertible(kind_of<length> / time, rate_of_climb) == implicit);  // the open `length` may be a `height`
static_assert(convertible(kind_of<length> / time, horizontal_speed) == implicit);
static_assert(convertible(kind_of<length> / time, special_speed) == explicit_only);
static_assert(convertible(kind_of<length> / time, kind_of<length> / time) == implicit);
static_assert(convertible(kind_of<length / time>, height / kind_of<time>) == implicit);
static_assert(convertible(height / kind_of<time>, kind_of<length / time>) == implicit);
static_assert(convertible(height / kind_of<time>, height / time) == implicit);
static_assert(convertible(height / time, height / kind_of<time>) == implicit);
static_assert(convertible(height / kind_of<time>, height / period_duration) == implicit);
static_assert(convertible(height / time, height / period_duration) == explicit_only);
static_assert(convertible(kind_of<length> / length, dimensionless) == implicit);
static_assert(convertible(dimensionless, kind_of<length> / length) == explicit_only);
static_assert(convertible(kind_of<length> / height, dimensionless) == implicit);
static_assert(convertible(dimensionless, kind_of<length> / height) == explicit_only);
// an open angle is still an angle
static_assert(convertible(height / kind_of<angular_measure>, height) == explicit_only);
static_assert(convertible(height / kind_of<angular_measure>, height / angular_measure) == implicit);
static_assert(convertible(height / kind_of<angular_measure>, height / phase_angle) == implicit);
static_assert(convertible(height / kind_of<angular_measure>, height / solid_angular_measure) == none);

// different dimensions
static_assert(convertible(mass, length) == none);
static_assert(convertible(speed, length) == none);
static_assert(convertible(length, speed) == none);
static_assert(convertible(energy, speed) == none);
static_assert(convertible(length, kind_of<time>) == none);
static_assert(convertible(kind_of<time>, length) == none);
static_assert(convertible(energy, kind_of<speed>) == none);
static_assert(convertible(kind_of<speed>, energy) == none);

// the same types
static_assert(convertible(length, length) == implicit);
static_assert(convertible(width, width) == implicit);
static_assert(convertible(energy, energy) == implicit);
static_assert(convertible(kind_of<length>, kind_of<length>) == implicit);
static_assert(convertible(kind_of<energy>, kind_of<energy>) == implicit);
static_assert(convertible(get_kind(moment_of_force), get_kind(moment_of_force)) == implicit);

// converting to a different branch
static_assert(convertible(height, width) == cast);
static_assert(convertible(potential_energy, kinetic_energy) == cast);
static_assert(convertible(kinetic_energy, potential_energy) == cast);
// two user-defined gravitational PE forms (different equations, same parent) require cast between each other
static_assert(convertible(gravitational_potential_energy, newtonian_gravitational_potential_energy) == cast);
static_assert(convertible(newtonian_gravitational_potential_energy, gravitational_potential_energy) == cast);
static_assert(convertible(rate_of_climb, velocity) == cast);
static_assert(convertible(rate_of_climb, horizontal_speed) == cast);
// two siblings that both inherit the parent equation are still a cast (Fix 4)
static_assert(convertible(north_speed, east_speed) == cast);
static_assert(convertible(east_speed, north_speed) == cast);

// converting to a different kind
static_assert(convertible(frequency, activity) == none);
static_assert(convertible(activity, frequency) == none);
static_assert(convertible(energy, moment_of_force) == none);
static_assert(convertible(energy, torque) == none);
static_assert(convertible(angular_measure, solid_angular_measure) == none);

// upcasting same hierarchy branch
static_assert(convertible(width, length) == implicit);
static_assert(convertible(path_length, length) == implicit);
static_assert(convertible(distance, length) == implicit);
static_assert(convertible(distance, path_length) == implicit);
static_assert(convertible(special_speed, speed) == implicit);
static_assert(convertible(north_speed, speed) == implicit);
static_assert(convertible(east_speed, speed) == implicit);
static_assert(convertible(rate_of_climb, speed) == implicit);
static_assert(convertible(special_rate_of_climb, speed) == implicit);
static_assert(convertible(velocity, speed) == implicit);
static_assert(convertible(potential_energy, energy) == implicit);
static_assert(convertible(kinetic_energy, energy) == implicit);
static_assert(convertible(newtonian_gravitational_potential_energy, potential_energy) == implicit);
static_assert(convertible(newtonian_gravitational_potential_energy, energy) == implicit);

// upcasting beyond the hierarchy/kind
static_assert(convertible(angular_measure, dimensionless) == explicit_only);
static_assert(convertible(angular_measure * time / period_duration, dimensionless) == explicit_only);
static_assert(convertible(storage_capacity, dimensionless) == explicit_only);

// upcasting to the derived quantity
static_assert(convertible(frequency, inverse(time)) == implicit);
static_assert(convertible(speed, length / time) == implicit);
static_assert(convertible(speed, length / time) == implicit);
static_assert(convertible(velocity, length / time) == implicit);
static_assert(convertible(rate_of_climb, length / time) == implicit);
static_assert(convertible(rate_of_climb, height / time) == implicit);
static_assert(convertible(gravitational_potential_energy, mass * acceleration * length) == implicit);
static_assert(convertible(angular_measure, arc_length / radius) == explicit_only);
static_assert(convertible(solid_angular_measure, area / pow<2>(radius)) == explicit_only);
static_assert(convertible(pixel_area, pixel_x* pixel_y) == implicit);
static_assert(convertible(pixel_area, pixel_count* pixel_y) == implicit);
static_assert(convertible(pixel_area, pixel_count* pixel_count) == implicit);

// downcasting same hierarchy branch
static_assert(convertible(length, width) == explicit_only);
static_assert(convertible(path_length, distance) == explicit_only);
static_assert(convertible(length, distance) == explicit_only);
static_assert(convertible(path_length, distance) == explicit_only);
static_assert(convertible(speed, special_speed) == explicit_only);
static_assert(convertible(speed, north_speed) == explicit_only);
static_assert(convertible(speed, east_speed) == explicit_only);
static_assert(convertible(speed, rate_of_climb) == explicit_only);
static_assert(convertible(speed, special_rate_of_climb) == explicit_only);
static_assert(convertible(rate_of_climb, special_rate_of_climb) == explicit_only);
static_assert(convertible(energy, potential_energy) == explicit_only);
static_assert(convertible(energy, kinetic_energy) == explicit_only);
static_assert(convertible(energy, newtonian_gravitational_potential_energy) == explicit_only);
static_assert(convertible(potential_energy, newtonian_gravitational_potential_energy) == explicit_only);
static_assert(convertible(dimensionless, rotation) == explicit_only);
static_assert(convertible(dimensionless, rotational_displacement) == explicit_only);

// derived quantities to type
static_assert(convertible(inverse(frequency), time) == implicit);
static_assert(convertible(inverse(period_duration), frequency) == implicit);
static_assert(convertible(length * length, area) == implicit);
static_assert(convertible(length / time, speed) == implicit);
static_assert(convertible(displacement / time, speed) == implicit);
static_assert(convertible(displacement / time, velocity) == implicit);
static_assert(convertible(height / time, speed) == implicit);
static_assert(convertible(height / time, rate_of_climb) == implicit);
static_assert(convertible(area / length, length) == implicit);
static_assert(convertible(length * length * length, volume) == implicit);
static_assert(convertible(area * length, volume) == implicit);
static_assert(convertible(volume / length, area) == implicit);
static_assert(convertible(volume / area, length) == implicit);
static_assert(convertible(volume / length / length, length) == implicit);
static_assert(convertible(area * area / length, volume) == implicit);
static_assert(convertible(area * (area / length), volume) == implicit);
static_assert(convertible(volume / (length * length), length) == implicit);
static_assert(convertible(length / speed, time) == implicit);
static_assert(convertible(speed * time, length) == implicit);
static_assert(convertible(displacement / time / time, acceleration) == implicit);
static_assert(convertible(displacement / (time * time), acceleration) == implicit);
static_assert(convertible(velocity / time, acceleration) == implicit);
static_assert(convertible(velocity / acceleration, time) == implicit);
static_assert(convertible(acceleration * time, velocity) == implicit);
static_assert(convertible(acceleration * (time * time), displacement) == implicit);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), energy) == implicit);
static_assert(convertible(force * length, energy) == implicit);
static_assert(convertible(force * position_vector, moment_of_force) == implicit);
static_assert(convertible(width * height, area) == implicit);
static_assert(convertible(area, width* height) == explicit_only);
static_assert(convertible(pow<1, 2>(area), length) == implicit);
static_assert(convertible(length, pow<1, 2>(area)) == implicit);
static_assert(convertible(mass * acceleration_of_free_fall * height, gravitational_potential_energy) == implicit);
static_assert(convertible(gravitational_constant * pow<2>(mass) / length, newtonian_gravitational_potential_energy) ==
              implicit);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), kinetic_energy) == implicit);
static_assert(convertible(arc_length / radius, angular_measure) == explicit_only);
static_assert(convertible(area / pow<2>(radius), solid_angular_measure) == explicit_only);
static_assert(convertible(pixel_x * pixel_y, pixel_area) == implicit);

// additional dimensionless remainder
static_assert(convertible(length / speed, time) == implicit);

// derived quantities to more constrained type
static_assert(convertible(inverse(time), frequency) == explicit_only);
static_assert(convertible(length / time / time, acceleration) == explicit_only);
static_assert(convertible(length / time, velocity) == explicit_only);
static_assert(convertible(length / time, rate_of_climb) == explicit_only);
static_assert(convertible(acceleration / velocity, frequency) == explicit_only);
static_assert(convertible(force * length, torque) == explicit_only);
static_assert(convertible(mass * acceleration * length, gravitational_potential_energy) == explicit_only);
// force * length is the simplified form of the newtonian GPE equation
// (gravitational_constant * mass²/length expands to force * pow<2>(length)/pow<2>(mass) * mass²/length = force *
// length)
static_assert(convertible(force * length, newtonian_gravitational_potential_energy) == implicit);
static_assert(convertible(length / radius, angular_measure) == explicit_only);
static_assert(convertible(length / length, angular_measure) == explicit_only);
static_assert(convertible(arc_length / length, angular_measure) == explicit_only);
static_assert(convertible(area / pow<2>(length), solid_angular_measure) == explicit_only);
static_assert(convertible(pixel_count * pixel_y, pixel_area) == explicit_only);
static_assert(convertible(pixel_count * pixel_count, pixel_area) == explicit_only);

// invalid derived quantities
static_assert(convertible(mass * acceleration_of_free_fall * width, gravitational_potential_energy) == cast);

// derived quantities to more specialized type
static_assert(convertible(force * position_vector, torque) == explicit_only);
static_assert(convertible(length / time, special_speed) == explicit_only);
static_assert(convertible(length / time, north_speed) == explicit_only);
static_assert(convertible(length / time, east_speed) == explicit_only);
static_assert(convertible(height / time, special_rate_of_climb) == explicit_only);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), mechanical_energy) == implicit);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), potential_energy) == explicit_only);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), gravitational_potential_energy) == explicit_only);

static_assert(convertible(dimensionless, dimensionless / relative_linear_strain) == explicit_only);
static_assert(convertible(length, height) == explicit_only);
static_assert(convertible(length, height / relative_linear_strain) == explicit_only);
static_assert(convertible(mass / length / pow<2>(time), pressure) == explicit_only);
static_assert(convertible(mass / length / pow<2>(time), normal_stress) == explicit_only);
static_assert(convertible(mass / length / pow<2>(time), normal_stress / relative_linear_strain) == explicit_only);
static_assert(convertible(mass / length / pow<2>(time), modulus_of_elasticity) == explicit_only);

// quantities derived from dimensionless
static_assert(convertible(power / power, efficiency) == explicit_only);
static_assert(convertible(stress / stress, strain) == explicit_only);
static_assert(convertible(stress / stress, efficiency) == explicit_only);

// specialized dimensionless to dimensionless and back
static_assert(convertible(height / width, dimensionless) == implicit);
static_assert(convertible(dimensionless, height / width) == explicit_only);

// derived with dimensionless remainder after explosion
static_assert(convertible(distance / speed, time) == implicit);

// derived quantities to incompatible type
static_assert(convertible(height / time, velocity) == cast);
static_assert(convertible(displacement / time, rate_of_climb) == cast);
static_assert(convertible(pixel_x * pixel_x, pixel_area) == cast);

// type to compatible derived
static_assert(convertible(distance, speed* time) == implicit);

// type to more specialized derived quantity
static_assert(convertible(speed, height / time) == explicit_only);
static_assert(convertible(speed, displacement / time) == explicit_only);

// type to a derived quantity on a different branch
static_assert(convertible(velocity, height / time) == cast);
static_assert(convertible(rate_of_climb, displacement / time) == cast);

// derived quantities requiring explosion to a type
static_assert(convertible(acceleration * time, velocity) == implicit);
static_assert(convertible(acceleration * period_duration, velocity) == implicit);
static_assert(convertible(velocity * time / period_duration, velocity) == implicit);
static_assert(convertible(mass * acceleration_of_free_fall * height / weight, height) == implicit);

// derived quantities to more generic derived compatible type
static_assert(convertible(inverse(width * height), inverse(area)) == implicit);
static_assert(convertible(path_length * distance, pow<2>(path_length)) == implicit);

// derived to compatible derived
static_assert(convertible(inverse(length * length), inverse(area)) == implicit);
static_assert(convertible(velocity * time, acceleration* pow<2>(time)) == implicit);
static_assert(convertible(height / period_duration, length / time) == implicit);
static_assert(convertible(height / width, length / length) == implicit);
static_assert(convertible(height * width, length* length) == implicit);
static_assert(convertible(inverse(path_length * distance), inverse(pow<2>(path_length))) == implicit);
static_assert(convertible(path_length * period_duration, length* time) == implicit);

static_assert(convertible(volume * length, pow<2>(area)) == implicit);
static_assert(convertible(pow<4>(length), pow<2>(area)) == implicit);
static_assert(convertible(pow<2>(radius), pow<2>(length)) == implicit);

static_assert(convertible(height * time / period_duration, length) == implicit);
static_assert(convertible(angular_measure * time / period_duration, arc_length / radius) == explicit_only);
static_assert(convertible(special_angular_measure * solid_angular_measure, angular_measure* solid_angular_measure) ==
              implicit);
static_assert(convertible(voltage * width / height, voltage) == implicit);
static_assert(convertible(voltage * time / (capacitance * resistance), voltage) == implicit);

// derived to more specialized derived
static_assert(convertible(length / time, height / period_duration) == explicit_only);
static_assert(convertible(length * length, height* width) == explicit_only);
static_assert(convertible(length * time, radius* period_duration) == explicit_only);

// derived to incompatible specialized derived
static_assert(convertible(height / time, distance / time) == cast);
static_assert(convertible(path_length * time, radius* period_duration) == cast);

// when more than one pairing of same-tree factors is possible, the best one counts
static_assert(convertible(width * height, pow<2>(height)) == cast);
static_assert(convertible(inverse(width * height), inverse(pow<2>(height))) == cast);
static_assert(convertible(width * distance, path_length* width) == implicit);
static_assert(convertible(height * distance, path_length* height) == implicit);
// `length` is on both sides, so this relates as `width -> height` does
static_assert(convertible(width * length, length* height) == cast);
static_assert(convertible(length * distance, path_length* height) == explicit_only);
static_assert(convertible(width * distance, width* path_length) == implicit);
static_assert(convertible(length * distance, height* path_length) == explicit_only);
static_assert(convertible(height * path_length, distance* height) == explicit_only);
// the first pairing that ranks `implicit` for one factor is not always part of the best complete pairing:
// `radius -> length` is `implicit` but leaves the siblings `height -> width`, while `radius -> width`
// leaves `height -> length`
static_assert(convertible(radius * height, width* length) == implicit);
static_assert(convertible(radius * height, length* width) == implicit);

// kind to its type
static_assert(convertible(kind_of<length>, length) == implicit);
static_assert(convertible(kind_of<length>, width) == implicit);
static_assert(convertible(kind_of<length>, position_vector) == implicit);
static_assert(convertible(kind_of<frequency>, frequency) == implicit);
static_assert(convertible(kind_of<speed>, velocity) == implicit);
static_assert(convertible(kind_of<energy>, energy) == implicit);
static_assert(convertible(kind_of<energy>, potential_energy) == implicit);
static_assert(convertible(kind_of<energy>, kinetic_energy) == implicit);
static_assert(convertible(kind_of<dimensionless>, rotation) == implicit);
static_assert(convertible(kind_of<dimensionless>, angular_measure) == implicit);
static_assert(convertible(kind_of<dimensionless>, rotational_displacement) == implicit);

// kind to a type of a different dimension
static_assert(convertible(kind_of<length>, mass) == none);
static_assert(convertible(kind_of<length>, speed) == none);
static_assert(convertible(kind_of<energy>, length) == none);
static_assert(convertible(kind_of<energy>, speed) == none);

// kind to a type of another kind but the same dimension
static_assert(convertible(kind_of<energy>, moment_of_force) == none);
static_assert(convertible(kind_of<activity>, frequency) == none);
static_assert(convertible(kind_of<frequency>, activity) == none);

// derived kind to a compatible type
static_assert(convertible(kind_of<length / time>, speed) == implicit);
static_assert(convertible(kind_of<length / time>, velocity) == implicit);
static_assert(convertible(kind_of<length / pow<2>(time)>, acceleration) == implicit);
static_assert(convertible(kind_of<inverse(time)>, frequency) == implicit);
static_assert(convertible(kind_of<inverse(time)>, activity) == implicit);
static_assert(convertible(kind_of<mass * pow<2>(length) / pow<2>(time)>, energy) == implicit);
static_assert(convertible(kind_of<mass * pow<2>(length) / pow<2>(time)>, moment_of_force) == implicit);
static_assert(convertible(kind_of<frequency * time / length>, repetency) == implicit);

// type to a kind of a different kind
static_assert(convertible(mass, kind_of<length>) == none);
static_assert(convertible(speed, kind_of<length>) == none);
static_assert(convertible(length, kind_of<energy>) == none);
static_assert(convertible(speed, kind_of<energy>) == none);
static_assert(convertible(moment_of_force, kind_of<energy>) == none);
static_assert(convertible(frequency, kind_of<activity>) == none);
static_assert(convertible(activity, kind_of<frequency>) == none);

// converting type to a kind
static_assert(convertible(length, kind_of<length>) == implicit);
static_assert(convertible(width, kind_of<length>) == implicit);
static_assert(convertible(frequency, kind_of<frequency>) == implicit);
static_assert(convertible(frequency, kind_of<inverse(time)>) == implicit);
static_assert(convertible(frequency, kind_of<activity>) == none);
static_assert(convertible(energy, kind_of<energy>) == implicit);
static_assert(convertible(potential_energy, kind_of<energy>) == implicit);
static_assert(convertible(kinetic_energy, kind_of<energy>) == implicit);
static_assert(convertible(rotation, kind_of<dimensionless>) == implicit);
static_assert(convertible(angular_measure, kind_of<dimensionless>) == explicit_only);
static_assert(convertible(rotational_displacement, kind_of<dimensionless>) == explicit_only);

// converting derived type to a kind
static_assert(convertible(inverse(period_duration), kind_of<frequency>) == implicit);
static_assert(convertible(inverse(time), kind_of<frequency>) == explicit_only);
static_assert(convertible(length / time, kind_of<speed>) == implicit);
static_assert(convertible(length / pow<2>(time), kind_of<acceleration>) == explicit_only);
static_assert(convertible(mass * pow<2>(length) / pow<2>(time), kind_of<energy>) == implicit);

// converting kind to a kind
static_assert(convertible(kind_of<dimensionless>, kind_of<angular_measure>) == implicit);
static_assert(convertible(kind_of<angular_measure>, kind_of<dimensionless>) == explicit_only);

// converting derived kind to a kind
static_assert(convertible(kind_of<inverse(time)>, kind_of<frequency>) == implicit);
static_assert(convertible(kind_of<length / time>, kind_of<speed>) == implicit);
static_assert(convertible(kind_of<length / pow<2>(time)>, kind_of<acceleration>) == implicit);

// converting kind to a type
static_assert(convertible(kind_of<mass / length / pow<2>(time)>, pressure) == implicit);
static_assert(convertible(kind_of<mass / length / pow<2>(time)>, normal_stress) == implicit);
static_assert(convertible(kind_of<mass / length / pow<2>(time)>, normal_stress / relative_linear_strain) == implicit);
static_assert(convertible(kind_of<mass / length / pow<2>(time)>, modulus_of_elasticity) == implicit);

// converting type to a derived kind
static_assert(convertible(speed, kind_of<length / time>) == implicit);
static_assert(convertible(velocity, kind_of<length / time>) == implicit);
static_assert(convertible(energy, kind_of<mass * pow<2>(length) / pow<2>(time)>) == implicit);
static_assert(convertible(mass_density, kind_of<mass / pow<3>(length)>) == implicit);
static_assert(convertible(voltage_phasor, kind_of<energy / time / electric_current>) == implicit);

// kinds of different dimensions
static_assert(convertible(kind_of<mass>, kind_of<length>) == none);
static_assert(convertible(kind_of<energy>, kind_of<length>) == none);
static_assert(convertible(kind_of<length>, kind_of<energy>) == none);
static_assert(convertible(kind_of<frequency>, kind_of<energy>) == none);

// derived quantities to dimensionless
static_assert(convertible(frequency * period_duration, dimensionless) == implicit);
static_assert(convertible(frequency * time, dimensionless) == implicit);
static_assert(convertible(length / length, dimensionless) == implicit);
static_assert(convertible(length / width, dimensionless) == implicit);
static_assert(convertible(active_power / apparent_power, dimensionless) == implicit);

// an angle among the ingredients keeps them from being a plain ratio...
static_assert(convertible(angular_measure_rate * radius / speed, dimensionless) == explicit_only);
static_assert(convertible(angular_measure_rate * radius, speed) == explicit_only);
// ...unless it cancels against another angle
static_assert(convertible(angular_measure_rate * radius / angular_measure / speed, dimensionless) == implicit);
static_assert(convertible(angular_measure_rate * radius / angular_measure, speed) == implicit);
static_assert(convertible(angular_measure_rate * time, angular_measure) == implicit);
// the same holds for a quantity in the kind's tree that is not a kind itself
static_assert(convertible(special_angular_measure / time * radius / speed, dimensionless) == explicit_only);
static_assert(convertible(special_angular_measure / time * radius / angular_measure / speed, dimensionless) ==
              implicit);

static_assert(convertible(efficiency, strain) == cast);

// an inherited equation is an `implicit` upcast, in a numerator or a denominator alike
static_assert(convertible(special_speed, length / time) == implicit);
static_assert(convertible(special_speed * time, length) == implicit);
static_assert(convertible(length / special_speed, time) == implicit);
static_assert(convertible(inverse(special_speed), time / length) == implicit);

// a declared kind is a boundary in both directions: losing it or gaining it costs explicit_only,
// and the verdict doesn't depend on which operations are taken
static_assert(convertible(angular_measure * radius, arc_length) == explicit_only);
static_assert(convertible(arc_length, angular_measure* radius) == explicit_only);
static_assert(convertible(angular_measure_rate * radius, speed) == explicit_only);
static_assert(convertible(angular_measure / solid_angular_measure, dimensionless) == explicit_only);
static_assert(convertible(special_angular_measure / angular_measure, dimensionless) == implicit);
static_assert(convertible(angular_measure / time, dimensionless_rate) == explicit_only);
static_assert(convertible(kind_of<dimensionless>, angular_measure) == implicit);
static_assert(convertible(dimensionless / angular_measure, dimensionless) == explicit_only);
static_assert(convertible(kinetic_energy / (angular_measure * mass * pow<2>(speed)), dimensionless) == explicit_only);
static_assert(convertible(inverse(phase_angle), radius / arc_length) == explicit_only);
static_assert(convertible(phase_angle / time, dimensionless / time) == explicit_only);
static_assert(convertible(angular_measure_rate / frequency, dimensionless) == explicit_only);
static_assert(convertible(angular_measure_rate / frequency, angular_measure) == implicit);

// a ratio of quantities of one kind is a plain number, whatever the exponents
static_assert(convertible(width * height / area, dimensionless) == implicit);
static_assert(convertible(pow<2>(width) / (height * length), dimensionless) == implicit);

// two factors of one tree relate as the tree says, even when packed into a product
static_assert(convertible(energy * time / period_duration, kinetic_energy) == explicit_only);
static_assert(convertible(north_speed * energy, east_speed* energy) == cast);

// a quantity converts implicitly from and to its own equation even when the equation mentions its own
// tree
static_assert(convertible(electromagnetism_power * time / period_duration, active_power) == implicit);
static_assert(convertible(active_power, electromagnetism_power* time / period_duration) == implicit);
static_assert(convertible(pow<2>(active_power), pow<2>(electromagnetism_power* time / period_duration)) == implicit);

// a factor common to both sides doesn't change how they relate
static_assert(convertible(north_speed * mass, east_speed* mass) == convertible(north_speed, east_speed));
static_assert(convertible(frequency * mass, activity* mass) == convertible(frequency, activity));
static_assert(convertible(energy * time, torque* time) == convertible(energy, torque));
static_assert(convertible(width * mass, length* mass) == convertible(width, length));
static_assert(convertible(length * mass, width* mass) == convertible(length, width));
static_assert(convertible(angular_measure * mass, dimensionless* mass) == convertible(angular_measure, dimensionless));
static_assert(convertible(velocity * length / time, velocity* speed) == convertible(length / time, speed));
static_assert(convertible(kind_of<length> * kind_of<mass>, width* mass) == convertible(kind_of<length>, width));
static_assert(convertible(width * mass, kind_of<length>* kind_of<mass>) == convertible(width, kind_of<length>));
static_assert(convertible(pow<2>(width) * mass, width * length * mass) == convertible(pow<2>(width), width* length));

// the order of operations: pair first, self-cancel only against an empty side, then the deepest factor
static_assert(convertible(height / width, width / height) == cast);
// the same kind lost in one part and gained in the other is `none`, unlike the reciprocal of a plain
// ratio above
static_assert(convertible(angular_measure, inverse(angular_measure)) == none);
static_assert(convertible(inverse(angular_measure), angular_measure) == none);
static_assert(convertible(angular_measure / solid_angular_measure, solid_angular_measure / angular_measure) == none);
static_assert(convertible(height / width, radius / width) == cast);
static_assert(convertible(height * speed / length, width / time) == cast);
static_assert(convertible(acceleration * time, special_speed) == cast);
static_assert(convertible(speed, speed* time / period_duration) == explicit_only);
static_assert(convertible(speed * time, displacement) == explicit_only);
static_assert(convertible(velocity * time, displacement) == implicit);
static_assert(convertible(mass * speed, mass* velocity) == explicit_only);
static_assert(convertible(area / length, width) == explicit_only);
static_assert(convertible(pow<2>(width), pow<2>(height)) == cast);

// two roots of one dimension left over after pairing block the conversion, but they still convert
// to the base quantities of their dimension, and their ratio is a plain number
static_assert(convertible(frequency * width, activity* length) == none);
static_assert(convertible(pow<2>(frequency) * mass, pow<2>(activity) * mass) == none);
static_assert(convertible(frequency * mass, mass / time) == implicit);
static_assert(convertible(energy / torque, dimensionless) == implicit);

// a kind converts implicitly into the sub-kinds of its tree
static_assert(convertible(kind_of<dimensionless>, storage_capacity) == implicit);
static_assert(convertible(kind_of<acceleration * time>, special_speed) == cast);

// nested kinds
static_assert(convertible(dimensionless, dimensionless) == implicit);
static_assert(convertible(angular_measure, dimensionless) == explicit_only);
static_assert(convertible(solid_angular_measure, dimensionless) == explicit_only);

static_assert(convertible(kind_of<dimensionless>, dimensionless) == implicit);
static_assert(convertible(kind_of<angular_measure>, dimensionless) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure>, dimensionless) == explicit_only);

static_assert(convertible(dimensionless, kind_of<dimensionless>) == implicit);
static_assert(convertible(angular_measure, kind_of<dimensionless>) == explicit_only);
static_assert(convertible(solid_angular_measure, kind_of<dimensionless>) == explicit_only);

static_assert(convertible(kind_of<dimensionless>, kind_of<dimensionless>) == implicit);
static_assert(convertible(kind_of<angular_measure>, kind_of<dimensionless>) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure>, kind_of<dimensionless>) == explicit_only);

static_assert(convertible(angular_measure, solid_angular_measure) == none);
static_assert(convertible(angular_measure, kind_of<solid_angular_measure>) == none);
static_assert(convertible(kind_of<angular_measure>, kind_of<solid_angular_measure>) == none);

static_assert(convertible(dimensionless, angular_measure) == explicit_only);
static_assert(convertible(dimensionless, solid_angular_measure) == explicit_only);

static_assert(convertible(dimensionless, kind_of<angular_measure>) == explicit_only);
static_assert(convertible(dimensionless, kind_of<solid_angular_measure>) == explicit_only);

static_assert(convertible(kind_of<dimensionless>, angular_measure) == implicit);
static_assert(convertible(kind_of<dimensionless>, solid_angular_measure) == implicit);

static_assert(convertible(kind_of<dimensionless>, kind_of<angular_measure>) == implicit);
static_assert(convertible(kind_of<dimensionless>, kind_of<solid_angular_measure>) == implicit);

// derived nested kinds
static_assert(convertible(dimensionless / time, dimensionless / time) == implicit);
static_assert(convertible(angular_measure / time, dimensionless / time) == explicit_only);
static_assert(convertible(solid_angular_measure / time, dimensionless / time) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, dimensionless / time) == implicit);
static_assert(convertible(kind_of<angular_measure / time>, dimensionless / time) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure / time>, dimensionless / time) == explicit_only);

static_assert(convertible(dimensionless / storage_capacity, dimensionless) == explicit_only);
static_assert(convertible(dimensionless * storage_capacity, dimensionless) == explicit_only);
static_assert(convertible(inverse(transfer_rate), period_duration) == explicit_only);

static_assert(convertible(dimensionless / time, kind_of<dimensionless / time>) == implicit);
static_assert(convertible(angular_measure / time, kind_of<dimensionless / time>) == explicit_only);
static_assert(convertible(solid_angular_measure / time, kind_of<dimensionless / time>) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<dimensionless / time>) == implicit);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<dimensionless / time>) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure / time>, kind_of<dimensionless / time>) == explicit_only);

static_assert(convertible(angular_measure / time, solid_angular_measure / time) == none);
static_assert(convertible(angular_measure / time, kind_of<solid_angular_measure / time>) == none);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<solid_angular_measure / time>) == none);

static_assert(convertible(dimensionless / time, angular_measure / time) == explicit_only);
static_assert(convertible(dimensionless / time, solid_angular_measure / time) == explicit_only);

static_assert(convertible(dimensionless / time, kind_of<angular_measure / time>) == explicit_only);
static_assert(convertible(dimensionless / time, kind_of<solid_angular_measure / time>) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, angular_measure / time) == implicit);
static_assert(convertible(kind_of<dimensionless / time>, solid_angular_measure / time) == implicit);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<angular_measure / time>) == implicit);
static_assert(convertible(kind_of<dimensionless / time>, kind_of<solid_angular_measure / time>) == implicit);

// named derived nested kinds
static_assert(convertible(dimensionless_rate, dimensionless_rate) == implicit);
static_assert(convertible(angular_measure_rate, dimensionless_rate) == none);
static_assert(convertible(solid_angular_measure_rate, dimensionless_rate) == none);

static_assert(convertible(kind_of<dimensionless_rate>, dimensionless_rate) == implicit);
static_assert(convertible(kind_of<angular_measure_rate>, dimensionless_rate) == none);
static_assert(convertible(kind_of<solid_angular_measure_rate>, dimensionless_rate) == none);

static_assert(convertible(dimensionless_rate, kind_of<dimensionless_rate>) == implicit);
static_assert(convertible(angular_measure_rate, kind_of<dimensionless_rate>) == none);
static_assert(convertible(solid_angular_measure_rate, kind_of<dimensionless_rate>) == none);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<dimensionless_rate>) == implicit);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<dimensionless_rate>) == none);
static_assert(convertible(kind_of<solid_angular_measure_rate>, kind_of<dimensionless_rate>) == none);

static_assert(convertible(angular_measure_rate, solid_angular_measure_rate) == none);
static_assert(convertible(angular_measure_rate, kind_of<solid_angular_measure_rate>) == none);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<solid_angular_measure_rate>) == none);

static_assert(convertible(dimensionless_rate, angular_measure_rate) == none);
static_assert(convertible(dimensionless_rate, solid_angular_measure_rate) == none);

static_assert(convertible(dimensionless_rate, kind_of<angular_measure_rate>) == none);
static_assert(convertible(dimensionless_rate, kind_of<solid_angular_measure_rate>) == none);

static_assert(convertible(kind_of<dimensionless_rate>, angular_measure_rate) == none);
static_assert(convertible(kind_of<dimensionless_rate>, solid_angular_measure_rate) == none);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<angular_measure_rate>) == none);
static_assert(convertible(kind_of<dimensionless_rate>, kind_of<solid_angular_measure_rate>) == none);

static_assert(convertible(dimensionless_rate, dimensionless / time) == implicit);
static_assert(convertible(angular_measure_rate, dimensionless / time) == explicit_only);
static_assert(convertible(solid_angular_measure_rate, dimensionless / time) == explicit_only);

static_assert(convertible(kind_of<dimensionless_rate>, dimensionless / time) == implicit);
static_assert(convertible(kind_of<angular_measure_rate>, dimensionless / time) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure_rate>, dimensionless / time) == explicit_only);

static_assert(convertible(dimensionless_rate, kind_of<dimensionless / time>) == implicit);
static_assert(convertible(angular_measure_rate, kind_of<dimensionless / time>) == explicit_only);
static_assert(convertible(solid_angular_measure_rate, kind_of<dimensionless / time>) == explicit_only);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<dimensionless / time>) == implicit);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<dimensionless / time>) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure_rate>, kind_of<dimensionless / time>) == explicit_only);

static_assert(convertible(angular_measure_rate, solid_angular_measure / time) == none);
static_assert(convertible(angular_measure_rate, kind_of<solid_angular_measure / time>) == none);
static_assert(convertible(kind_of<angular_measure_rate>, kind_of<solid_angular_measure / time>) == none);

static_assert(convertible(dimensionless_rate, angular_measure / time) == explicit_only);
static_assert(convertible(dimensionless_rate, kind_of<angular_measure / time>) == explicit_only);
static_assert(convertible(dimensionless_rate, solid_angular_measure / time) == explicit_only);
static_assert(convertible(dimensionless_rate, kind_of<solid_angular_measure / time>) == explicit_only);

static_assert(convertible(kind_of<dimensionless_rate>, angular_measure / time) == implicit);
static_assert(convertible(kind_of<dimensionless_rate>, solid_angular_measure / time) == implicit);

static_assert(convertible(kind_of<dimensionless_rate>, kind_of<angular_measure / time>) == implicit);
static_assert(convertible(kind_of<dimensionless_rate>, kind_of<solid_angular_measure / time>) == implicit);

static_assert(convertible(dimensionless / time, dimensionless_rate) == implicit);
static_assert(convertible(angular_measure / time, dimensionless_rate) == explicit_only);
static_assert(convertible(solid_angular_measure / time, dimensionless_rate) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, dimensionless_rate) == implicit);
static_assert(convertible(kind_of<angular_measure / time>, dimensionless_rate) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure / time>, dimensionless_rate) == explicit_only);

static_assert(convertible(dimensionless / time, kind_of<dimensionless_rate>) == implicit);
static_assert(convertible(angular_measure / time, kind_of<dimensionless_rate>) == explicit_only);
static_assert(convertible(solid_angular_measure / time, kind_of<dimensionless_rate>) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<dimensionless_rate>) == implicit);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<dimensionless_rate>) == explicit_only);
static_assert(convertible(kind_of<solid_angular_measure / time>, kind_of<dimensionless_rate>) == explicit_only);

static_assert(convertible(angular_measure / time, solid_angular_measure_rate) == none);
static_assert(convertible(angular_measure / time, kind_of<solid_angular_measure_rate>) == none);
static_assert(convertible(kind_of<angular_measure / time>, kind_of<solid_angular_measure_rate>) == none);

static_assert(convertible(dimensionless / time, angular_measure_rate) == explicit_only);
static_assert(convertible(dimensionless / time, solid_angular_measure_rate) == explicit_only);

static_assert(convertible(dimensionless / time, kind_of<angular_measure_rate>) == explicit_only);
static_assert(convertible(dimensionless / time, kind_of<solid_angular_measure_rate>) == explicit_only);

static_assert(convertible(kind_of<dimensionless / time>, angular_measure_rate) == implicit);
static_assert(convertible(kind_of<dimensionless / time>, solid_angular_measure_rate) == implicit);

static_assert(convertible(kind_of<dimensionless / time>, kind_of<angular_measure_rate>) == implicit);
static_assert(convertible(kind_of<dimensionless / time>, kind_of<solid_angular_measure_rate>) == implicit);

// get_common_quantity_spec
static_assert(get_common_quantity_spec(length, length) == length);
static_assert(get_common_quantity_spec(kind_of<length>, kind_of<length>) == kind_of<length>);
static_assert(get_common_quantity_spec(kind_of<length>, length) == length);
static_assert(get_common_quantity_spec(length, kind_of<length>) == length);
static_assert(get_common_quantity_spec(width, kind_of<length>) == width);
static_assert(get_common_quantity_spec(kind_of<length>, width) == width);
static_assert(get_common_quantity_spec(kind_of<dimensionless>, kind_of<angular_measure>) == kind_of<angular_measure>);
static_assert(get_common_quantity_spec(kind_of<dimensionless>, angular_measure) == angular_measure);

static_assert(get_common_quantity_spec(width, height) == length);
static_assert(get_common_quantity_spec(distance, path_length) == path_length);
static_assert(get_common_quantity_spec(potential_energy, kinetic_energy) == mechanical_energy);

static_assert(get_common_quantity_spec(length / time, length / time) == length / time);
static_assert(get_common_quantity_spec(length / time, inverse(time / length)) == length / time);

static_assert(get_common_quantity_spec(speed, length / time) == speed);
static_assert(get_common_quantity_spec(length / time, speed) == speed);
static_assert(get_common_quantity_spec(area, length* length) == area);
static_assert(get_common_quantity_spec(length * length, area) == area);
static_assert(get_common_quantity_spec(frequency, inverse(time)) == inverse(time));
static_assert(get_common_quantity_spec(inverse(time), frequency) == inverse(time));
static_assert(get_common_quantity_spec(frequency, inverse(period_duration)) == frequency);
static_assert(get_common_quantity_spec(inverse(period_duration), frequency) == frequency);
static_assert(get_common_quantity_spec(kinetic_energy, mass* pow<2>(length) / pow<2>(time)) == kinetic_energy);
static_assert(get_common_quantity_spec(mass * pow<2>(length) / pow<2>(time), kinetic_energy) == kinetic_energy);
static_assert(get_common_quantity_spec(gravitational_potential_energy, mass * acceleration_of_free_fall * height) ==
              gravitational_potential_energy);
static_assert(get_common_quantity_spec(mass * acceleration_of_free_fall * height, gravitational_potential_energy) ==
              gravitational_potential_energy);

static_assert(get_common_quantity_spec(gravitational_potential_energy, mass * acceleration * length) ==
              mass * acceleration * length);
static_assert(get_common_quantity_spec(mass * acceleration * length, gravitational_potential_energy) ==
              mass * acceleration * length);

// two GPE forms (siblings with different equations) share potential_energy as common ancestor
static_assert(get_common_quantity_spec(gravitational_potential_energy, newtonian_gravitational_potential_energy) ==
              potential_energy);
static_assert(get_common_quantity_spec(newtonian_gravitational_potential_energy, gravitational_potential_energy) ==
              potential_energy);

template<auto T1, auto T2>
concept no_common_quantity_spec = requires {
  requires !requires { get_common_quantity_spec(T1, T2); };
  requires !requires { get_common_quantity_spec(T2, T1); };
};
static_assert(no_common_quantity_spec<energy, torque>);
static_assert(no_common_quantity_spec<kind_of<energy>, torque>);
static_assert(no_common_quantity_spec<frequency, activity>);
static_assert(no_common_quantity_spec<kind_of<frequency>, activity>);
static_assert(no_common_quantity_spec<frequency, kind_of<activity>>);
static_assert(no_common_quantity_spec<kind_of<frequency>, kind_of<activity>>);
static_assert(no_common_quantity_spec<dimensionless, angular_measure>);
static_assert(no_common_quantity_spec<dimensionless, kind_of<angular_measure>>);
static_assert(no_common_quantity_spec<angular_measure, solid_angular_measure>);
static_assert(no_common_quantity_spec<kind_of<angular_measure>, solid_angular_measure>);
static_assert(no_common_quantity_spec<angular_measure, kind_of<solid_angular_measure>>);
static_assert(no_common_quantity_spec<kind_of<angular_measure>, kind_of<solid_angular_measure>>);

// ============================================================================
// non_negative property tests
// ============================================================================

// Base quantity with explicit non_negative
QUANTITY_SPEC_(nn_length, dim_length, non_negative);
QUANTITY_SPEC_(nn_mass, dim_mass, non_negative);
QUANTITY_SPEC_(nn_time, dim_time, non_negative);

// Base quantity without non_negative
QUANTITY_SPEC_(signed_current, dim_electric_current);

// Derived without explicit non_negative: spec-2 quantities never infer non_negative from equation factors;
// an explicit non_negative tag is required at every root that should be non-negative.
QUANTITY_SPEC_(nn_area, pow<2>(nn_length));
QUANTITY_SPEC_(nn_speed, nn_length / nn_time);
QUANTITY_SPEC_(nn_density, nn_mass / pow<3>(nn_length));

// Derived: some factors not non_negative → not non_negative
QUANTITY_SPEC_(mixed_quantity, nn_mass* signed_current);

// Named child with equation: inherits from parent only; nn_speed has no explicit tag, so not non_negative
QUANTITY_SPEC_(nn_velocity, nn_speed, nn_length / nn_time);

// Named child without equation: inherits non_negative from parent (nn_length is non_negative)
QUANTITY_SPEC_(nn_child_no_tag, nn_length);

// Named child with explicit non_negative tag
QUANTITY_SPEC_(nn_child_tagged, nn_length, non_negative);

// Vector child of nn_length: cannot be non_negative (direction-sensitive)
QUANTITY_SPEC_(nn_displacement, nn_length, quantity_tensor_order::vector);

// dimensionless is not non_negative (no explicit tag, empty equation)
static_assert(!is_non_negative(dimensionless));

// Explicit non_negative on base quantities
static_assert(is_non_negative(nn_length));
static_assert(is_non_negative(nn_mass));
static_assert(is_non_negative(nn_time));
static_assert(!is_non_negative(signed_current));

// Spec-2 quantities without explicit non_negative are NOT non_negative, regardless of factor signs
static_assert(!is_non_negative(nn_area));
static_assert(!is_non_negative(nn_speed));
static_assert(!is_non_negative(nn_density));
static_assert(!is_non_negative(mixed_quantity));

// Named child with equation inherits from parent; parent (nn_speed) has no explicit tag → not non_negative
static_assert(!is_non_negative(nn_velocity));

// Named child without equation inherits non_negative from parent
static_assert(is_non_negative(nn_child_no_tag));

// Named child with explicit tag
static_assert(is_non_negative(nn_child_tagged));

// Vector child does NOT inherit non_negative (character is not real_scalar)
static_assert(!is_non_negative(nn_displacement));

// kind_of<QS> is NEVER non_negative even when QS is, because a kind encompasses the whole
// quantity tree — including vector quantities and signed coordinates like altitude/depth.
static_assert(!is_non_negative(kind_of<nn_length>));
static_assert(!is_non_negative(kind_of<nn_mass>));

// Spec-2 root WITH explicit non_negative tag: IS non_negative
QUANTITY_SPEC_(tagged_speed, nn_length / nn_time, non_negative);
QUANTITY_SPEC_(tagged_volume, pow<3>(nn_length), non_negative);
static_assert(is_non_negative(tagged_speed));
static_assert(is_non_negative(tagged_volume));

// ISQ base quantities tagged non_negative
static_assert(is_non_negative(length));
static_assert(is_non_negative(mass));
static_assert(is_non_negative(duration));
static_assert(is_non_negative(thermodynamic_temperature));
static_assert(is_non_negative(amount_of_substance));
static_assert(is_non_negative(luminous_intensity));
static_assert(!is_non_negative(electric_current));  // can be negative

// Inheritance of non_negative through hierarchy
static_assert(is_non_negative(height));       // inherits from length
static_assert(is_non_negative(width));        // inherits from length
static_assert(is_non_negative(path_length));  // inherits from length

// V2 workaround: altitude and depth have specialized overloads returning false (signed coordinates)
static_assert(!is_non_negative(altitude));  // overload returns false despite inheriting from length
static_assert(!is_non_negative(depth));     // overload returns false despite inheriting from length

// Derived quantities with explicit non_negative tag are non-negative
static_assert(is_non_negative(tagged_speed));   // nn_length / nn_time, non_negative
static_assert(is_non_negative(tagged_volume));  // pow<3>(nn_length), non_negative

// Vector quantities are not non-negative (character != real_scalar), even when parent is non_negative
static_assert(!is_non_negative(displacement));
static_assert(!is_non_negative(velocity));

// kind_of<QS> is NEVER non-negative, even when QS is (encompasses entire tree including signed variants)
static_assert(!is_non_negative(kind_of<length>));
static_assert(!is_non_negative(kind_of<mass>));
static_assert(!is_non_negative(kind_of<duration>));

// Verify get_kind() for non-root quantities (height, altitude, depth all get kind_of<length>)
static_assert(!is_non_negative(get_kind(height)));
static_assert(!is_non_negative(get_kind(altitude)));
static_assert(!is_non_negative(get_kind(depth)));

// speed with explicit non_negative tag
static_assert(is_non_negative(speed));

// Named child (no equation) inherits non_negative from speed
static_assert(is_non_negative(special_speed));

// Named child with equation inherits from parent (speed) when real_scalar character
static_assert(is_non_negative(horizontal_speed));
static_assert(is_non_negative(rate_of_climb));
static_assert(is_non_negative(special_rate_of_climb));  // inherits via rate_of_climb

// velocity has vector character → NOT non_negative, even though parent speed is non_negative
static_assert(!is_non_negative(velocity));

// area, volume, mass_density with explicit non_negative
static_assert(is_non_negative(area));
static_assert(is_non_negative(volume));
static_assert(is_non_negative(mass_density));

// frequency, activity, repetency with explicit non_negative
static_assert(is_non_negative(frequency));
static_assert(is_non_negative(activity));
static_assert(is_non_negative(repetency));

// solid_angular_measure: is_kind + non_negative coexist; kind_of<> is still NOT non_negative
static_assert(is_non_negative(solid_angular_measure));
static_assert(!is_non_negative(kind_of<solid_angular_measure>));

// spec-2 expressions derived from non_negative quantities are NOT non_negative without explicit tag
static_assert(!is_non_negative(solid_angular_measure_rate));  // solid_angular_measure / time

// energy chain: non_negative propagates down through the named-child hierarchy
static_assert(is_non_negative(energy));
// mechanical_work: real_scalar named child of energy → inherits
static_assert(is_non_negative(mechanical_work));
// mechanical_energy: real_scalar named child of mechanical_work → inherits
static_assert(is_non_negative(mechanical_energy));
// potential_energy: named child (no equation) of mechanical_energy → inherits
static_assert(is_non_negative(potential_energy));
// kinetic_energy and both GPE siblings: named children of non_negative parent → inherit
static_assert(is_non_negative(kinetic_energy));
static_assert(is_non_negative(gravitational_potential_energy));
static_assert(is_non_negative(newtonian_gravitational_potential_energy));

// force is a spec-2 expression without an explicit non_negative tag → NOT non_negative,
// so weight (named child of force) is also NOT non_negative
static_assert(!is_non_negative(force));
static_assert(!is_non_negative(weight));

}  // namespace
