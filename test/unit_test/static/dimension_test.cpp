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
#include <units/dimension.h>

namespace {

using namespace units;

using dimensionless_ = struct dimensionless;

// clang-format off
BASE_DIMENSION_(length, "L");
BASE_DIMENSION_(time, "T");
BASE_DIMENSION_(mass, "M");

DERIVED_DIMENSION_(frequency, decltype(1 / time));
DERIVED_DIMENSION_(action, decltype(1 / time));
DERIVED_DIMENSION_(area, decltype(length * length));
DERIVED_DIMENSION_(volume, decltype(area * length));
DERIVED_DIMENSION_(speed, decltype(length / time));
inline constexpr struct velocity_ : speed_ {} velocity;
DERIVED_DIMENSION_(acceleration, decltype(speed / time));
DERIVED_DIMENSION_(force, decltype(mass * acceleration));
DERIVED_DIMENSION_(moment_of_force, decltype(length * force));
DERIVED_DIMENSION_(torque, decltype(moment_of_force));
DERIVED_DIMENSION_(pressure, decltype(force / area));
DERIVED_DIMENSION_(stress, decltype(pressure));
DERIVED_DIMENSION_(strain, decltype(stress / stress));
DERIVED_DIMENSION_(power, decltype(force * speed));
DERIVED_DIMENSION_(efficiency, decltype(power / power));
DERIVED_DIMENSION_(energy, decltype(force * length));
// clang-format on

// concepts verification
static_assert(BaseDimension<length_>);
static_assert(!BaseDimension<frequency_>);
static_assert(!DerivedDimension<length_>);
static_assert(DerivedDimension<frequency_>);
static_assert(Dimension<length_>);
static_assert(Dimension<frequency_>);

static_assert(DerivedDimension<dimensionless_>);
static_assert(DerivedDimension<decltype(length / length)>);  // dimensionless
static_assert(BaseDimension<decltype(speed * time)>);        // length

// derived dimension expression template syntax verification
static_assert(is_of_type<1 / time, derived_dimension<dimensionless_, per<time_>>>);
static_assert(is_of_type<1 / (1 / time), time_>);

static_assert(is_of_type<dimensionless * time, time_>);
static_assert(is_of_type<time * dimensionless, time_>);
static_assert(is_of_type<dimensionless * (1 / time), derived_dimension<dimensionless_, per<time_>>>);
static_assert(is_of_type<1 / time * dimensionless, derived_dimension<dimensionless_, per<time_>>>);

static_assert(is_of_type<length * time, derived_dimension<length_, time_>>);
static_assert(is_of_type<length * length, derived_dimension<units::power<length_, 2>>>);

static_assert(is_of_type<length * length * time, derived_dimension<units::power<length_, 2>, time_>>);
static_assert(is_of_type<length * time * length, derived_dimension<units::power<length_, 2>, time_>>);

static_assert(is_of_type<length*(time* length), derived_dimension<units::power<length_, 2>, time_>>);
static_assert(is_of_type<time*(length* length), derived_dimension<units::power<length_, 2>, time_>>);

static_assert(is_of_type<1 / time * length, derived_dimension<length_, per<time_>>>);
static_assert(is_of_type<1 / time * time, dimensionless_>);

static_assert(is_of_type<time / dimensionless, time_>);
static_assert(is_of_type<1 / time / dimensionless, derived_dimension<dimensionless_, per<time_>>>);

static_assert(is_of_type<length / time * time, length_>);
static_assert(is_of_type<1 / time * (1 / time), derived_dimension<dimensionless_, per<units::power<time_, 2>>>>);
static_assert(is_of_type<1 / (time * time), derived_dimension<dimensionless_, per<units::power<time_, 2>>>>);
static_assert(is_of_type<1 / (1 / (time * time)), derived_dimension<units::power<time_, 2>>>);

static_assert(is_of_type<length / time * (1 / time), derived_dimension<length_, per<units::power<time_, 2>>>>);
static_assert(
  is_of_type<length / time*(length / time), derived_dimension<units::power<length_, 2>, per<units::power<time_, 2>>>>);
static_assert(is_of_type<length / time*(time / length), dimensionless_>);

static_assert(is_of_type<speed / acceleration, time_>);
static_assert(is_of_type<acceleration / speed, derived_dimension<dimensionless_, per<time_>>>);
static_assert(is_of_type<speed * speed / length, derived_dimension<length_, per<units::power<time_, 2>>>>);
static_assert(is_of_type<1 / (speed * speed) * length, derived_dimension<units::power<time_, 2>, per<length_>>>);

// comparisons of the same dimensions
static_assert(length == length);
static_assert(speed == speed);

// comparisons of equivalent dimensions (named vs unnamed/derived)
static_assert(length / length == dimensionless);

static_assert(1 / time != frequency);
static_assert(convertible(1 / time, frequency));
static_assert(1 / frequency == time);
static_assert(frequency * time == dimensionless);
static_assert(std::is_same_v<std::common_type_t<decltype(1 / time), decltype(frequency)>, frequency_>);
static_assert(std::is_same_v<std::common_type_t<decltype(frequency), decltype(1 / time)>, frequency_>);

static_assert(length * length != area);
static_assert(convertible(length * length, area));
static_assert(length * length != volume);
static_assert(area / length == length);
static_assert(std::is_same_v<std::common_type_t<decltype(length * length), decltype(area)>, area_>);
static_assert(std::is_same_v<std::common_type_t<decltype(area), decltype(length * length)>, area_>);

static_assert(length * length * length != volume);
static_assert(area * length != volume);
static_assert(volume / length != area);
static_assert(volume / length / length == length);
static_assert(area * area / length != volume);
static_assert(area * (area / length) != volume);
static_assert(volume / (length * length) == length);

static_assert(length / time != speed);
static_assert(length * time != speed);
static_assert(length / time / time != speed);
static_assert(length / speed == time);
static_assert(speed * time == length);
static_assert(std::is_same_v<std::common_type_t<decltype(length / time), decltype(speed)>, speed_>);
static_assert(std::is_same_v<std::common_type_t<decltype(speed), decltype(length / time)>, speed_>);
static_assert(
  std::is_same_v<std::common_type_t<decltype(length / time), decltype(length / time)>, decltype(length / time)>);

static_assert(length / time / time != acceleration);
static_assert(length / (time * time) != acceleration);
static_assert(speed / time != acceleration);
static_assert(speed / acceleration == time);
static_assert(acceleration * time != speed);
static_assert(acceleration * (time * time) == length);
static_assert(acceleration / speed != frequency);

// comparison of convertible named dimensions
static_assert(velocity != speed);
static_assert(convertible(speed, velocity));
static_assert(std::is_same_v<std::common_type_t<decltype(velocity), decltype(speed)>, velocity_>);
static_assert(std::is_same_v<std::common_type_t<decltype(speed), decltype(velocity)>, velocity_>);

// comparison of convertible unnamed dimensions
static_assert(is_of_type<mass * acceleration, derived_dimension<length_, mass_, per<units::power<time_, 2>>>>);
static_assert(is_of_type<acceleration * mass, derived_dimension<length_, mass_, per<units::power<time_, 2>>>>);
static_assert(mass * acceleration == acceleration * mass);
static_assert(convertible(mass * acceleration, acceleration* mass));

// comparisons of equivalent but not convertible dimensions
static_assert(energy != torque);
static_assert(!convertible(energy, torque));

static_assert(force * length != energy);
static_assert(force * length != torque);
static_assert(convertible(force * length, energy));
static_assert(convertible(force * length, torque));
template<auto T1, auto T2>
concept no_common_type = requires {
                           requires !requires { typename std::common_type_t<decltype(T1), decltype(T2)>; };
                           requires !requires { typename std::common_type_t<decltype(T2), decltype(T1)>; };
                         };
static_assert(no_common_type<energy, torque>);

static_assert(frequency != action);
static_assert(!convertible(frequency, action));
static_assert(no_common_type<frequency, action>);

// Dimensionless
static_assert(convertible(power / power, efficiency));
static_assert(power / power != efficiency);
static_assert(dimensionless != efficiency);

static_assert(!convertible(efficiency, strain));
static_assert(efficiency != strain);

static_assert(stress / stress != strain);
static_assert(stress / stress != efficiency);
static_assert(convertible(stress / stress, strain));
static_assert(convertible(stress / stress, efficiency));

// comparison of not equivalent dimensions
static_assert(length != time);
static_assert(!convertible(length, time));

static_assert(acceleration != speed);
static_assert(!convertible(acceleration, speed));

// power
static_assert(is_of_type<pow<2>(length), derived_dimension<units::power<length_, 2>>>);
static_assert(is_of_type<pow<1, 2>(length), derived_dimension<units::power<length_, 1, 2>>>);
static_assert(is_of_type<pow<1, 2>(length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length* length), length_>);
static_assert(is_of_type<pow<1, 3>(length* length), derived_dimension<units::power<length_, 2, 3>>>);
static_assert(
  is_of_type<pow<1, 2>(length / time), derived_dimension<units::power<length_, 1, 2>, per<units::power<time_, 1, 2>>>>);
static_assert(
  is_of_type<pow<1, 2>(length / (time * time)), derived_dimension<units::power<length_, 1, 2>, per<time_>>>);

static_assert(is_same_v<decltype(pow<2>(length)), decltype(length * length)>);
static_assert(is_same_v<decltype(pow<2>(length / time)), decltype(length * length / time / time)>);

}  // namespace
