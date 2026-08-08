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

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/framework/dimension.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_spec.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#endif

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
// Defines one quantity spec per photometric condition (ISO 80000-7:2019, clauses 0.3 to 0.5)
// so that every equation of the luminous block is written only once. The equation provided in
// `__VA_ARGS__` may refer to the current condition as `Condition`. Besides the class template,
// the macro provides a `name_of<condition>` variable template and a plain `name` variable being
// the photopic instantiation (if the condition is not specified, photopic vision is assumed).
// The macro name deliberately does not contain the `QUANTITY_SPEC(` token so that
// `scripts/systems_reference.py` does not parse its definition and usages as regular specs.
#if MP_UNITS_API_NO_CRTP

#define MP_UNITS_PHOTOMETRIC_QSPEC(name, ...)                                                       \
  template<::mp_units::isq::PhotometricCondition auto Condition = ::mp_units::isq::photopic_vision> \
  struct name##_ : ::mp_units::quantity_spec<__VA_ARGS__> {};                                       \
  template<::mp_units::isq::PhotometricCondition auto Condition = ::mp_units::isq::photopic_vision> \
  constexpr name##_<Condition> name##_of;                                                           \
  inline constexpr auto name = name##_of<>

#else

#define MP_UNITS_PHOTOMETRIC_QSPEC(name, ...)                                                       \
  template<::mp_units::isq::PhotometricCondition auto Condition = ::mp_units::isq::photopic_vision> \
  struct name##_ : ::mp_units::quantity_spec<name##_<Condition>, __VA_ARGS__> {};                   \
  template<::mp_units::isq::PhotometricCondition auto Condition = ::mp_units::isq::photopic_vision> \
  constexpr name##_<Condition> name##_of;                                                           \
  inline constexpr auto name = name##_of<>

#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

MP_UNITS_EXPORT
namespace mp_units::isq {

// photometric conditions (ISO 80000-7:2019, clauses 0.3 to 0.5)
// Each condition weights the radiometric spectrum with a different spectral luminous efficiency
// function, so quantities of different conditions form separate kinds and can never be mixed
// in `+`, `-`, comparisons, or conversions (their products and quotients stay well-formed).
// Users may provide custom conditions (e.g. CIE S 026 alpha-opic weightings) by deriving from
// `photometric_condition_base` (the `_base` postfix leaves the plain name free for the concept).
struct photometric_condition_base {};

template<typename T>
concept PhotometricCondition = std::derived_from<T, photometric_condition_base>;

// clang-format off
inline constexpr struct photopic_vision final : photometric_condition_base {} photopic_vision;
inline constexpr struct scotopic_vision final : photometric_condition_base {} scotopic_vision;

// every adaptation level defines a different spectral weighting, so it is a part of the type;
// the library does not interpret the value - any structural type identifying the level works
template<auto AdaptationLevel> struct mesopic_vision_ final : photometric_condition_base {};
template<auto AdaptationLevel> constexpr mesopic_vision_<AdaptationLevel> mesopic_vision;

// dimensions of base quantities
inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass final : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_current final : base_dimension<"I"> {} dim_electric_current;
inline constexpr struct dim_thermodynamic_temperature final : base_dimension<symbol_text{u8"Θ", "O"}> {} dim_thermodynamic_temperature;
inline constexpr struct dim_amount_of_substance final : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity final : base_dimension<"J"> {} dim_luminous_intensity;
// clang-format on

// base quantities
QUANTITY_SPEC(length, dim_length, non_negative);
QUANTITY_SPEC(mass, dim_mass, non_negative);
QUANTITY_SPEC(duration, dim_time, non_negative);
inline constexpr auto time = duration;
QUANTITY_SPEC(electric_current, dim_electric_current);
QUANTITY_SPEC(thermodynamic_temperature, dim_thermodynamic_temperature, non_negative);
QUANTITY_SPEC(amount_of_substance, dim_amount_of_substance, non_negative);
// one base quantity per photometric condition; every condition roots its own kind tree
// over `dim_luminous_intensity`, and `luminous_intensity` is the photopic one
MP_UNITS_PHOTOMETRIC_QSPEC(luminous_intensity, dim_luminous_intensity, non_negative);

}  // namespace mp_units::isq
