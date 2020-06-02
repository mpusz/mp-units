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

#include <units/physical/si/mass.h>
#include <units/physical/si/voltage.h>
#include <units/physical/si/time.h>
#include <units/physical/si/frequency.h>
#include <units/math.h>

namespace {

using namespace units;
using namespace units::physical::si;

// power spectral density
struct sq_volt_per_hertz : unit<sq_volt_per_hertz> {};
struct dim_power_spectral_density : derived_dimension<dim_power_spectral_density, sq_volt_per_hertz, units::exp<dim_voltage, 2>, units::exp<dim_frequency, -1>> {};

template<Unit U, Value Rep = double>
using power_spectral_density = quantity<dim_power_spectral_density, U, Rep>;

// amplitude spectral density
struct volt_per_sqrt_hertz : unit<volt_per_sqrt_hertz> {};
struct dim_amplitude_spectral_density : derived_dimension<dim_amplitude_spectral_density, volt_per_sqrt_hertz, units::exp<dim_voltage, 1>, units::exp<dim_frequency, -1, 2>> {};

template<Unit U, Value Rep = double>
using amplitude_spectral_density = quantity<dim_amplitude_spectral_density, U, Rep>;

}

namespace {

static_assert(std::is_same_v<dimension_sqrt<dim_power_spectral_density>, dim_amplitude_spectral_density>);
static_assert(std::is_same_v<dimension_pow<dim_amplitude_spectral_density, 2>, dim_power_spectral_density>);

static_assert(std::is_same_v<decltype(pow<2>(amplitude_spectral_density<volt_per_sqrt_hertz>(4))), decltype(power_spectral_density<sq_volt_per_hertz>(16))>);
static_assert(std::is_same_v<decltype(sqrt(power_spectral_density<sq_volt_per_hertz>(16))), decltype(amplitude_spectral_density<volt_per_sqrt_hertz>(4))>);

}

namespace {

struct kilogram_per_second : unit<kilogram_per_second> {};
struct dim_mass_rate : derived_dimension<dim_mass_rate, kilogram_per_second, units::exp<dim_mass, 1>, units::exp<dim_time, -1>> {};
struct kilogram_per_hour : deduced_unit<kilogram_per_hour, dim_mass_rate, kilogram, hour> {};
constexpr auto a = 1q_kg / 1q_h;
static_assert(std::is_same_v<decltype(a)::unit, kilogram_per_hour>);

}
