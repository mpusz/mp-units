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

#include <mp-units/bits/config.h>
#include <mp-units/bits/diagnostics.h>
#include <mp-units/compat_macros.h>
#include <mp-units/systems/codata/adopted_values.h>
#include <mp-units/systems/iec.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#if MP_UNITS_HOSTED
#include <mp-units/utility/cartesian_tensor.h>
#include <mp-units/utility/cartesian_vector.h>
#endif
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <optional>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <chrono>
#include <complex>
#include <string>
#endif
#endif

namespace {

using namespace mp_units;

inline constexpr struct my_origin final : absolute_point_origin<isq::length> {
} my_origin;
inline constexpr struct my_relative_origin final : relative_point_origin<my_origin + isq::length(42 * si::metre)> {
} my_relative_origin;

inline constexpr auto dim_speed = isq::dim_length / isq::dim_time;

// BaseDimension
static_assert(detail::BaseDimension<struct isq::dim_length>);
static_assert(!detail::BaseDimension<decltype(isq::dim_length / isq::dim_time)>);
static_assert(!detail::BaseDimension<decltype(inverse(isq::dim_time))>);
static_assert(!detail::BaseDimension<decltype(pow<2>(isq::dim_length))>);
static_assert(!detail::BaseDimension<derived_dimension<struct isq::dim_length, per<struct isq::dim_time>>>);
static_assert(!detail::BaseDimension<decltype(dim_speed)>);
static_assert(!detail::BaseDimension<base_dimension<"L">>);
static_assert(!detail::BaseDimension<struct si::metre>);
static_assert(!detail::BaseDimension<int>);

// Dimension
static_assert(Dimension<struct isq::dim_length>);
static_assert(Dimension<decltype(isq::dim_length / isq::dim_time)>);
static_assert(Dimension<decltype(inverse(isq::dim_time))>);
static_assert(Dimension<decltype(pow<2>(isq::dim_length))>);
static_assert(Dimension<derived_dimension<struct isq::dim_length, per<struct isq::dim_time>>>);
static_assert(Dimension<struct dimension_one>);
static_assert(Dimension<decltype(dim_speed)>);
static_assert(!Dimension<base_dimension<"L">>);
static_assert(!Dimension<struct si::metre>);
static_assert(!Dimension<int>);

// DimensionOf
static_assert(DimensionOf<struct isq::dim_length, isq::dim_length>);
static_assert(DimensionOf<struct isq::dim_length, get_dimension(isq::height)>);
static_assert(DimensionOf<struct isq::dim_length, get_dimension(isq::radius)>);
static_assert(!DimensionOf<struct isq::dim_length, isq::length>);
static_assert(!DimensionOf<struct isq::length, isq::dim_length>);
static_assert(!DimensionOf<struct isq::length, isq::length>);
static_assert(!DimensionOf<struct isq::dim_length, isq::dim_time>);
static_assert(!DimensionOf<struct isq::dim_length, isq::duration>);
static_assert(!DimensionOf<struct isq::dim_time, isq::dim_length>);
static_assert(!DimensionOf<struct isq::dim_time, isq::length>);
static_assert(!DimensionOf<struct isq::length, isq::dim_time>);
static_assert(!DimensionOf<struct isq::length, isq::duration>);
static_assert(!DimensionOf<struct isq::duration, isq::dim_length>);
static_assert(!DimensionOf<struct isq::duration, isq::length>);
static_assert(DimensionOf<decltype(isq::dim_length / isq::dim_time), get_dimension(isq::speed)>);
static_assert(
  DimensionOf<decltype(get_dimension(isq::force) * get_dimension(isq::duration)), get_dimension(isq::impulse)>);
static_assert(DimensionOf<decltype(get_dimension(isq::angular_momentum) / get_dimension(isq::angular_velocity)),
                          get_dimension(isq::moment_of_inertia)>);

// QuantitySpec
inline constexpr auto speed = isq::length / isq::duration;

static_assert(QuantitySpec<struct isq::length>);
static_assert(QuantitySpec<struct isq::radius>);
static_assert(QuantitySpec<struct isq::speed>);
static_assert(QuantitySpec<kind_of_<struct isq::length>>);
static_assert(QuantitySpec<decltype(isq::length / isq::duration)>);
static_assert(QuantitySpec<decltype(pow<2>(isq::length))>);
static_assert(QuantitySpec<struct dimensionless>);
static_assert(QuantitySpec<decltype(speed)>);
static_assert(!QuantitySpec<struct isq::dim_length>);
static_assert(!QuantitySpec<int>);

// QuantitySpecOf
static_assert(QuantitySpecOf<struct isq::length, isq::length>);
static_assert(QuantitySpecOf<struct isq::height, isq::length>);
static_assert(!QuantitySpecOf<struct isq::length, isq::height>);
static_assert(QuantitySpecOf<struct isq::displacement, isq::length>);
static_assert(!QuantitySpecOf<struct isq::length, isq::displacement>);
static_assert(QuantitySpecOf<struct isq::thickness, isq::width>);
static_assert(!QuantitySpecOf<struct isq::width, isq::thickness>);
static_assert(QuantitySpecOf<kind_of_<struct isq::length>, isq::height>);
static_assert(QuantitySpecOf<kind_of_<struct isq::length>, isq::displacement>);

static_assert(!QuantitySpecOf<struct isq::angular_measure, dimensionless>);
static_assert(!QuantitySpecOf<struct isq::angular_measure, kind_of<dimensionless>>);
static_assert(!QuantitySpecOf<kind_of_<struct isq::angular_measure>, dimensionless>);
static_assert(!QuantitySpecOf<kind_of_<struct isq::angular_measure>, kind_of<dimensionless>>);

static_assert(!QuantitySpecOf<struct dimensionless, isq::angular_measure>);
static_assert(!QuantitySpecOf<struct dimensionless, kind_of<isq::angular_measure>>);
static_assert(QuantitySpecOf<kind_of_<struct dimensionless>, isq::angular_measure>);
static_assert(QuantitySpecOf<kind_of_<struct dimensionless>, kind_of<isq::angular_measure>>);

static_assert(!QuantitySpecOf<struct isq::solid_angular_measure, isq::angular_measure>);
static_assert(!QuantitySpecOf<struct isq::solid_angular_measure, kind_of<isq::angular_measure>>);
static_assert(!QuantitySpecOf<kind_of_<struct isq::solid_angular_measure>, isq::angular_measure>);
static_assert(!QuantitySpecOf<kind_of_<struct isq::solid_angular_measure>, kind_of<isq::angular_measure>>);

// NamedQuantitySpec
static_assert(detail::NamedQuantitySpec<struct isq::length>);
static_assert(detail::NamedQuantitySpec<struct isq::radius>);
static_assert(detail::NamedQuantitySpec<struct isq::speed>);
static_assert(!detail::NamedQuantitySpec<kind_of_<struct isq::length>>);
static_assert(!detail::NamedQuantitySpec<decltype(isq::length / isq::duration)>);
static_assert(!detail::NamedQuantitySpec<decltype(pow<2>(isq::length))>);
static_assert(detail::NamedQuantitySpec<struct dimensionless>);
static_assert(!detail::NamedQuantitySpec<decltype(speed)>);
static_assert(!detail::NamedQuantitySpec<struct isq::dim_length>);
static_assert(!detail::NamedQuantitySpec<int>);

// DerivedQuantitySpec
static_assert(!detail::DerivedQuantitySpec<struct isq::length>);
static_assert(!detail::DerivedQuantitySpec<struct isq::radius>);
static_assert(!detail::DerivedQuantitySpec<kind_of_<struct isq::length>>);
static_assert(!detail::DerivedQuantitySpec<struct isq::speed>);
static_assert(detail::DerivedQuantitySpec<decltype(isq::length / isq::duration)>);
static_assert(detail::DerivedQuantitySpec<decltype(pow<2>(isq::length))>);
static_assert(!detail::DerivedQuantitySpec<struct dimensionless>);
static_assert(detail::DerivedQuantitySpec<decltype(speed)>);
static_assert(!detail::DerivedQuantitySpec<struct isq::dim_length>);
static_assert(!detail::DerivedQuantitySpec<int>);

// QuantityKindSpec
static_assert(!detail::QuantityKindSpec<struct isq::length>);
static_assert(!detail::QuantityKindSpec<struct isq::radius>);
static_assert(detail::QuantityKindSpec<kind_of_<struct isq::length>>);
static_assert(!detail::QuantityKindSpec<struct isq::speed>);
static_assert(!detail::QuantityKindSpec<decltype(isq::length / isq::duration)>);
static_assert(!detail::QuantityKindSpec<decltype(pow<2>(isq::length))>);
static_assert(!detail::QuantityKindSpec<struct dimensionless>);
static_assert(!detail::QuantityKindSpec<decltype(speed)>);
static_assert(!detail::QuantityKindSpec<struct isq::dim_length>);
static_assert(!detail::QuantityKindSpec<int>);

// Unit
static_assert(Unit<struct si::metre>);
static_assert(Unit<decltype(si::kilogram)>);
static_assert(Unit<si::kilo_<struct si::gram>>);
static_assert(Unit<decltype(si::metre / si::second)>);
static_assert(Unit<decltype(inverse(si::second))>);
static_assert(Unit<decltype(mag<10> * si::second)>);
static_assert(Unit<decltype(square(si::metre))>);
static_assert(Unit<decltype(pow<2>(si::metre))>);
static_assert(Unit<struct codata::standard_gravity>);
static_assert(Unit<scaled_unit<mag<10>, struct si::second>>);
static_assert(Unit<derived_unit<struct si::metre, per<struct si::second>>>);
static_assert(Unit<struct one>);
static_assert(!Unit<named_unit<"?", kind_of<isq::length>>>);
static_assert(!Unit<named_unit<"?", si::metre / si::second>>);
static_assert(!Unit<named_unit<"?", si::metre, kind_of<isq::length>>>);
static_assert(!Unit<prefixed_unit<"?", mag<10>, si::second>>);
static_assert(!Unit<struct isq::dim_length>);
static_assert(!Unit<int>);
#if MP_UNITS_HOSTED
static_assert(!Unit<std::chrono::seconds>);
#endif

// PrefixableUnit
static_assert(PrefixableUnit<struct si::metre>);
static_assert(!PrefixableUnit<decltype(si::kilogram)>);
static_assert(!PrefixableUnit<si::kilo_<struct si::gram>>);
static_assert(!PrefixableUnit<decltype(si::metre / si::second)>);
static_assert(!PrefixableUnit<decltype(inverse(si::second))>);
static_assert(!PrefixableUnit<decltype(mag<10> * si::second)>);
static_assert(!PrefixableUnit<decltype(square(si::metre))>);
static_assert(!PrefixableUnit<decltype(pow<2>(si::metre))>);
static_assert(!PrefixableUnit<struct codata::standard_gravity>);
static_assert(!PrefixableUnit<scaled_unit<mag<10>, struct si::second>>);
static_assert(!PrefixableUnit<derived_unit<struct si::metre, per<struct si::second>>>);
static_assert(!PrefixableUnit<struct one>);
static_assert(!PrefixableUnit<named_unit<"?", kind_of<isq::length>>>);
static_assert(!PrefixableUnit<named_unit<"?", si::metre / si::second>>);
static_assert(!PrefixableUnit<named_unit<"?", si::metre, kind_of<isq::length>>>);
static_assert(!PrefixableUnit<prefixed_unit<"?", mag<10>, si::second>>);
static_assert(!PrefixableUnit<struct isq::dim_length>);
static_assert(!PrefixableUnit<int>);
#if MP_UNITS_HOSTED
static_assert(!PrefixableUnit<std::chrono::seconds>);
#endif

// Unit
static_assert(Unit<struct si::metre>);
static_assert(Unit<decltype(si::kilogram)>);
static_assert(Unit<si::kilo_<struct si::gram>>);
static_assert(Unit<decltype(si::metre / si::second)>);
static_assert(Unit<decltype(inverse(si::second))>);
static_assert(Unit<decltype(mag<10> * si::second)>);
static_assert(Unit<decltype(square(si::metre))>);
static_assert(Unit<decltype(pow<2>(si::metre))>);
static_assert(Unit<struct codata::standard_gravity>);
static_assert(Unit<scaled_unit<mag<10>, struct si::second>>);
static_assert(Unit<derived_unit<struct si::metre, per<struct si::second>>>);
static_assert(Unit<struct one>);
static_assert(Unit<decltype(get_common_unit(si::kilo<si::metre> / si::hour, si::metre / si::second))>);
static_assert(!Unit<named_unit<"?", kind_of<isq::length>>>);
static_assert(!Unit<named_unit<"?", si::metre / si::second>>);
static_assert(!Unit<named_unit<"?", si::metre, kind_of<isq::length>>>);
static_assert(!Unit<prefixed_unit<"?", mag<10>, si::second>>);
static_assert(!Unit<struct isq::dim_length>);
static_assert(!Unit<int>);
#if MP_UNITS_HOSTED
static_assert(!Unit<std::chrono::seconds>);
#endif

// UnitOf
static_assert(UnitOf<struct si::metre, isq::length>);
static_assert(UnitOf<struct si::metre, isq::radius>);
static_assert(UnitOf<decltype(si::kilogram), isq::mass>);
static_assert(UnitOf<struct si::hertz, isq::frequency>);
static_assert(UnitOf<struct si::hertz, inverse(isq::duration)>);
static_assert(UnitOf<struct one, dimensionless>);
static_assert(UnitOf<struct percent, dimensionless>);
static_assert(UnitOf<struct si::radian, isq::angular_measure>);
static_assert(UnitOf<struct si::degree, isq::angular_measure>);
static_assert(UnitOf<struct one, isq::angular_measure>);
static_assert(UnitOf<struct percent, isq::angular_measure>);
static_assert(UnitOf<decltype(si::radian / si::second), isq::angular_velocity>);
static_assert(UnitOf<decltype(one / si::second), isq::angular_velocity>);
static_assert(!UnitOf<struct si::radian, dimensionless>);
static_assert(!UnitOf<struct si::metre, isq::duration>);

// Reference
static_assert(Reference<struct si::metre>);
static_assert(Reference<decltype(si::metre / si::second)>);
static_assert(Reference<decltype(isq::length[si::metre])>);
static_assert(Reference<decltype(isq::radius[si::metre])>);
static_assert(Reference<decltype(isq::radius[si::metre] / isq::duration[si::second])>);
static_assert(!Reference<struct isq::length>);
static_assert(!Reference<kind_of_<struct isq::length>>);
static_assert(!Reference<struct isq::dim_length>);
static_assert(!Reference<int>);

// ReferenceOf
static_assert(ReferenceOf<struct si::metre, isq::length>);
static_assert(ReferenceOf<struct si::metre, isq::radius>);
static_assert(!ReferenceOf<struct si::second, isq::length>);
static_assert(ReferenceOf<decltype(isq::length[si::metre]), isq::length>);
static_assert(!ReferenceOf<decltype(isq::length[si::metre]), isq::radius>);
static_assert(ReferenceOf<decltype(isq::radius[si::metre]), isq::length>);
static_assert(ReferenceOf<decltype(isq::radius[si::metre]), isq::radius>);
static_assert(!ReferenceOf<struct si::second, isq::dim_length>);

static_assert(ReferenceOf<struct one, dimensionless>);
static_assert(ReferenceOf<decltype(dimensionless[one]), dimensionless>);

// Overload sets that rank by these concepts. `Unit` is a disjunct of `Reference`, so a unit picks the
// `Unit` overload and any other reference the `Reference` one. `QuantityOf` is `Quantity` plus a
// constraint, so a `Quantity` overload is the fallback for quantities of other kinds.
namespace concept_subsumption {

constexpr int unit_or_reference(Unit auto) { return 1; }
constexpr int unit_or_reference(Reference auto) { return 2; }
static_assert(unit_or_reference(si::metre) == 1);
static_assert(unit_or_reference(si::metre / si::second) == 1);
static_assert(unit_or_reference(isq::height[si::metre]) == 2);

// GCC 12 to 15 do not rank templates whose class-type non-type parameters are constrained with
// `Unit auto` and `Reference auto` (a reproducer without the library fails there too), GCC 16 does.
#if !MP_UNITS_COMP_GCC || MP_UNITS_COMP_GCC >= 16
template<Unit auto U>
constexpr int quantity_in_unit_or_reference(quantity<U, double>)
{
  return 1;
}
template<Reference auto R>
constexpr int quantity_in_unit_or_reference(quantity<R, double>)
{
  return 2;
}
static_assert(quantity_in_unit_or_reference(1. * si::metre) == 1);
static_assert(quantity_in_unit_or_reference(isq::height(1. * si::metre)) == 2);
#endif

constexpr int length_or_any(Quantity auto) { return 1; }
constexpr int length_or_any(QuantityOf<isq::length> auto) { return 2; }
static_assert(length_or_any(isq::height(1. * si::metre)) == 2);
static_assert(length_or_any(1. * si::metre) == 2);
static_assert(length_or_any(1. * si::second) == 1);

// A height overload with a length fallback. With concrete parameter types the exact match wins, so a
// height picks the height overload and every other length, the bare `1 * m` included, the fallback.
constexpr int height_or_length(quantity<si::metre, double>) { return 1; }
constexpr int height_or_length(quantity<isq::height[si::metre], double>) { return 2; }
static_assert(height_or_length(isq::height(1. * si::metre)) == 2);
static_assert(height_or_length(isq::width(1. * si::metre)) == 1);
static_assert(height_or_length(isq::length(1. * si::metre)) == 1);
static_assert(height_or_length(1. * si::metre) == 1);

// Two `QuantityOf` overloads do not rank: both are the same atom with a different quantity
// specification, and subsumption does not see that `isq::height` is a `isq::length`. A height, and a
// bare `1 * m`, which converts to a height as well, match both.
constexpr int generic_height_or_length(QuantityOf<isq::length> auto) { return 1; }
constexpr int generic_height_or_length(QuantityOf<isq::height> auto) { return 2; }
template<typename Q>
concept generic_height_or_length_ambiguous = !requires(Q qty) { generic_height_or_length(qty); };
static_assert(generic_height_or_length_ambiguous<quantity<isq::height[si::metre]>>);
static_assert(generic_height_or_length_ambiguous<quantity<si::metre>>);
static_assert(generic_height_or_length(isq::width(1. * si::metre)) == 1);

// Asking for the kind instead of `isq::length` does not change that: the two constraints are still the
// same concept with different values.
constexpr int generic_height_or_length_kind(QuantityOf<kind_of<isq::length>> auto) { return 1; }
constexpr int generic_height_or_length_kind(QuantityOf<isq::height> auto) { return 2; }
template<typename Q>
concept generic_height_or_length_kind_ambiguous = !requires(Q qty) { generic_height_or_length_kind(qty); };
static_assert(generic_height_or_length_kind_ambiguous<quantity<isq::height[si::metre]>>);
static_assert(generic_height_or_length_kind_ambiguous<quantity<si::metre>>);
static_assert(generic_height_or_length_kind(isq::width(1. * si::metre)) == 1);

// The fallback has to exclude the specific case. A bare `1 * m` then goes to the height overload,
// because a quantity of the length kind is also a height.
template<QuantityOf<isq::length> Q>
  requires(!QuantityOf<Q, isq::height>)
constexpr int generic_height_or_other_length(Q)
{
  return 1;
}
constexpr int generic_height_or_other_length(QuantityOf<isq::height> auto) { return 2; }
static_assert(generic_height_or_other_length(isq::height(1. * si::metre)) == 2);
static_assert(generic_height_or_other_length(isq::width(1. * si::metre)) == 1);
static_assert(generic_height_or_other_length(isq::length(1. * si::metre)) == 1);
static_assert(generic_height_or_other_length(1. * si::metre) == 2);

}  // namespace concept_subsumption
static_assert(ReferenceOf<decltype(isq::rotation[one]), isq::rotation>);
static_assert(ReferenceOf<decltype(isq::rotation[one]), dimensionless>);
static_assert(ReferenceOf<struct si::radian, isq::angular_measure>);
static_assert(ReferenceOf<struct si::degree, isq::angular_measure>);
static_assert(ReferenceOf<decltype(get_common_unit(si::degree, si::radian)), isq::angular_measure>);
static_assert(!ReferenceOf<struct si::radian, dimensionless>);
static_assert(!ReferenceOf<struct si::degree, dimensionless>);
static_assert(!ReferenceOf<decltype(get_common_unit(si::degree, si::radian)), dimensionless>);
static_assert(ReferenceOf<decltype(isq::angular_measure[si::radian]), isq::angular_measure>);
static_assert(ReferenceOf<decltype(isq::angular_measure[si::degree]), isq::angular_measure>);
static_assert(!ReferenceOf<decltype(isq::angular_measure[si::radian]), dimensionless>);
static_assert(!ReferenceOf<decltype(isq::angular_measure[si::degree]), dimensionless>);
static_assert(ReferenceOf<struct one, isq::rotation>);
static_assert(ReferenceOf<struct one, isq::angular_measure>);
static_assert(!ReferenceOf<decltype(dimensionless[one]), isq::rotation>);
static_assert(!ReferenceOf<decltype(dimensionless[one]), isq::angular_measure>);

// RepresentationOf
// int: real scalar and, as degenerate lower-rank cases, also vector and tensor, but not complex.
// A tensor of order zero is a scalar and a tensor of order one is a vector (clause 18 of ISO 80000-2:2019).
// The library generalizes that: a lower-order representation can stand in for a higher-order quantity
// (never the reverse).
static_assert(RepresentationOf<int, quantity_field::real>);
static_assert(!RepresentationOf<int, quantity_field::complex>);
static_assert(RepresentationOf<int, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<int, quantity_tensor_order::vector>);
static_assert(RepresentationOf<int, quantity_tensor_order::tensor>);

// double: same as int
static_assert(RepresentationOf<double, quantity_field::real>);
static_assert(!RepresentationOf<double, quantity_field::complex>);
static_assert(RepresentationOf<double, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<double, quantity_tensor_order::vector>);
static_assert(RepresentationOf<double, quantity_tensor_order::tensor>);

// bool: opted out via disable_representation<bool>
static_assert(!RepresentationOf<bool, quantity_field::real>);
static_assert(!RepresentationOf<bool, quantity_field::complex>);
static_assert(!RepresentationOf<bool, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<bool, quantity_tensor_order::vector>);
static_assert(!RepresentationOf<bool, quantity_tensor_order::tensor>);

// non-numeric type: not a representation at all
static_assert(!RepresentationOf<std::optional<int>, quantity_tensor_order::scalar>);

#if MP_UNITS_HOSTED
// std::complex: a complex scalar. A bare order axis is field-agnostic, so it fills the scalar order
// slot and (degenerately) the vector and tensor order slots; its field is complex.
static_assert(!RepresentationOf<std::complex<double>, quantity_field::real>);
static_assert(RepresentationOf<std::complex<double>, quantity_field::complex>);
static_assert(RepresentationOf<std::complex<double>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<std::complex<double>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<std::complex<double>, quantity_tensor_order::tensor>);

// utility::cartesian_vector<double>: 3D vector, not scalar; also a degenerate tensor (a vector is a
// tensor of the first order)
static_assert(RepresentationOf<utility::cartesian_vector<double>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_vector<double>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_vector<double>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<utility::cartesian_vector<double>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_vector<double>, quantity_tensor_order::tensor>);

// utility::cartesian_vector<int>: integer element type is supported (norm() returns double via std::hypot)
static_assert(RepresentationOf<utility::cartesian_vector<int>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_vector<int>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_vector<int>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<utility::cartesian_vector<int>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_vector<int>, quantity_tensor_order::tensor>);

// utility::cartesian_vector<complex<double>>: a complex 3-vector. It exposes real()/imag(), so its field is
// complex; it fills the vector order slot (and degenerately the tensor order slot) but not the
// scalar one.
static_assert(!RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_field::real>);
static_assert(RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_tensor_order::tensor>);

// Field classification of a vector/tensor takes the field of its scalar element, but a *complex*
// element additionally requires the container to expose the `real()`/`imag()` decomposition API on
// its own surface. A complex-element container that lacks that API cannot be used as complex, so its
// numeric_field is left *unspecified* (like an ambiguous tensor_order) rather than guessed - the type
// is then not a representation until the author exposes the API or specializes numeric_field. Only the
// presence of `real()`/`imag()` matters for detection; their return type is irrelevant here.
struct complex_vec_no_api {
  std::complex<double> data[2];
  constexpr const std::complex<double>& operator[](std::size_t i) const { return data[i]; }
};
struct complex_vec_with_api {
  std::complex<double> data[2];
  constexpr const std::complex<double>& operator[](std::size_t i) const { return data[i]; }
  constexpr double real() const { return data[0].real(); }
  constexpr double imag() const { return data[0].imag(); }
};
struct complex_mat_no_api {
  std::complex<double> data[4];
  constexpr const std::complex<double>& operator()(std::size_t r, std::size_t c) const { return data[2 * r + c]; }
};
// A real container that exposes element-wise real()/imag() (as a real Eigen/Blaze matrix does) stays
// real, because its field is read off the (real) element, not the container surface.
struct real_vec_with_api {
  double data[2];
  constexpr const double& operator[](std::size_t i) const { return data[i]; }
  constexpr double real() const { return data[0]; }
  constexpr double imag() const { return 0.0; }
};

// complex element + decomposition API -> complex; real element -> real (API on the container is moot)
static_assert(numeric_field<complex_vec_with_api> == quantity_field::complex);
static_assert(numeric_field<real_vec_with_api> == quantity_field::real);
// complex element without the API -> field left unspecified (not classified, not a representation)
static_assert(!utility::specified<decltype(numeric_field<complex_vec_no_api>)>);
static_assert(!utility::specified<decltype(numeric_field<complex_mat_no_api>)>);

// utility::cartesian_tensor<double>: second-order tensor; tensor-only - never a (lower-rank) vector or scalar
static_assert(RepresentationOf<utility::cartesian_tensor<double>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_tensor<double>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_tensor<double>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<utility::cartesian_tensor<double>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_tensor<double>, quantity_tensor_order::tensor>);

// utility::cartesian_tensor<int>: integer element type is supported (Frobenius norm returns double)
static_assert(RepresentationOf<utility::cartesian_tensor<int>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_tensor<int>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_tensor<int>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<utility::cartesian_tensor<int>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_tensor<int>, quantity_tensor_order::tensor>);

// utility::cartesian_tensor<complex<double>>: a complex second-order tensor. Field complex (via real()/imag());
// fills the tensor order slot only, never the vector or scalar one.
static_assert(!RepresentationOf<utility::cartesian_tensor<std::complex<double>>, quantity_field::real>);
static_assert(RepresentationOf<utility::cartesian_tensor<std::complex<double>>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_tensor<std::complex<double>>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<utility::cartesian_tensor<std::complex<double>>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<utility::cartesian_tensor<std::complex<double>>, quantity_tensor_order::tensor>);

// quantity types must never themselves be a representation (disable_representation guard)
static_assert(!RepresentationOf<quantity<si::metre>, quantity_field::real>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_field::complex>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::vector>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::tensor>);

// cartesian_tensor whose element is a quantity must not be a representation
static_assert(!RepresentationOf<utility::cartesian_tensor<quantity<si::metre>>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_tensor<quantity<si::metre>>, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<utility::cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::vector>);
static_assert(!RepresentationOf<utility::cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::tensor>);

// a tensor-character ISQ quantity accepts a second-order tensor representation
static_assert(Quantity<quantity<isq::stress[si::pascal], utility::cartesian_tensor<double>>>);

// `VectorRepresentation` subsumes `TensorRepresentation` (the latter is defined as the former plus
// the order-2 case), so a VectorRepresentation-constrained overload is preferred over a
// TensorRepresentation-constrained one for vector representations, while a genuine tensor matches
// only the TensorRepresentation overload. This locks the subsumption ordering that a user's overload
// set would rely on.
//
// clang 21 needs 6 to 16 minutes for this namespace depending on -std, clang 20 under a minute.
// clang 22 and 23 show the same growth on a reduced reproducer. The subsumption check normalizes
// the three-deep nested disjunctions of the representation concepts, and from clang 21 on the cost
// grows about twentyfold for every ten atomic constraints (a 71-line reproducer without includes
// shows it). Nothing in the library ranks overloads by these concepts, so the lock is kept for the
// other compilers.
#if !(MP_UNITS_COMP_CLANG >= 21 && MP_UNITS_COMP_CLANG <= 23)
namespace subsumption_check {
template<detail::TensorRepresentation>
consteval int grade()
{
  return 2;
}
template<detail::VectorRepresentation>
consteval int grade()
{
  return 1;
}
template<detail::ScalarRepresentation>
consteval int grade()
{
  return 0;
}
static_assert(grade<utility::cartesian_tensor<double>>() == 2);  // only TensorRepresentation matches
static_assert(grade<utility::cartesian_vector<double>>() == 1);  // VectorRepresentation wins (subsumes Tensor)
static_assert(grade<double>() == 0);                             // ScalarRepresentation wins (subsumes Vector)
static_assert(grade<std::complex<double>>() == 0);               // a complex scalar is order 0 too
}  // namespace subsumption_check
#endif

// cartesian_vector whose element is a quantity must not be a representation
static_assert(!RepresentationOf<utility::cartesian_vector<quantity<si::metre>>, quantity_tensor_order::vector>);

static_assert(!RepresentationOf<std::chrono::seconds, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<std::string, quantity_tensor_order::scalar>);

// `tensor_order` is detected from a type's structure: single-index access `t[i]` is a vector
// (order 1), two-index access `t(i, j)` a tensor (order 2), otherwise a scalar (order 0). A type
// that exposes *both* shapes (an Eigen N×1 column matrix models a vector yet also offers `t(i, j)`)
// is ambiguous and has no default; it must specialize `tensor_order` (the Eigen integration does).
// The single-shape types below classify without a specialization; the ambiguous both-shape case
// (`ambiguous_shaped`) is checked afterwards.
namespace order_detection {
struct scalar_shaped {};
struct vector_shaped {
  double operator[](std::size_t) const;
};
struct matrix_shaped {
  double operator()(std::size_t, std::size_t) const;
};
// Exposes *both* indexing styles (an N x 1 column matrix, as Eigen models a vector). Its order is
// ambiguous, so the primary `tensor_order` is left undefined for it - it has no default.
struct ambiguous_shaped {
  double operator[](std::size_t) const;
  double operator()(std::size_t, std::size_t) const;
};
// Whether `tensor_order<T>` yields a usable value. For the ambiguous case the primary is left
// undefined, so this is `false` via a substitution failure (SFINAE-friendly), not a hard error.
template<typename T>
concept order_defined = (tensor_order<T> < 3);
// GCC 12 cannot probe for `t[i, j]` (see `MP_UNITS_MULTIDIM_SUBSCRIPT_HACK`), so the library skips
// that probe there (see `has_matrix_indexing`). Keep this test in step.
#if MP_UNITS_HAS_MULTIDIM_SUBSCRIPT
struct multidim_subscript_shaped {
  double operator[](std::size_t, std::size_t) const;
};
#endif
}  // namespace order_detection
static_assert(tensor_order<order_detection::scalar_shaped> == 0);
static_assert(tensor_order<order_detection::vector_shaped> == 1);
static_assert(tensor_order<order_detection::matrix_shaped> == 2);
#if MP_UNITS_HAS_MULTIDIM_SUBSCRIPT
static_assert(tensor_order<order_detection::multidim_subscript_shaped> == 2);  // C++23 t[i, j]
#endif
// Single-shape types have a defined order; the ambiguous both-shape type does not (soft-rejected).
static_assert(order_detection::order_defined<order_detection::vector_shaped>);
static_assert(order_detection::order_defined<order_detection::matrix_shaped>);
static_assert(!order_detection::order_defined<order_detection::ambiguous_shaped>);

// The legacy flat spelling still selects the right (order, field): `vector` -> (vector, real),
// `complex_scalar` -> (scalar, complex), etc. (converted at a function argument in `order_of` /
// `field_of`, never an NTTP boundary).
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(RepresentationOf<double, quantity_character::real_scalar>);
static_assert(RepresentationOf<std::complex<double>, quantity_character::complex_scalar>);
static_assert(!RepresentationOf<double, quantity_character::complex_scalar>);
static_assert(RepresentationOf<utility::cartesian_vector<double>, quantity_character::vector>);
static_assert(RepresentationOf<utility::cartesian_tensor<double>, quantity_character::tensor>);
MP_UNITS_DIAGNOSTIC_POP

// Field is exact (disjoint) and detected from the values, not the `real()`/`imag()` API: a genuinely
// complex representation does not satisfy a real character, and a real one does not satisfy a complex
// character. The detection recurses through wrapped elements.
static_assert(!RepresentationOf<std::complex<double>, quantity_field::real>);
static_assert(!RepresentationOf<utility::cartesian_vector<std::complex<double>>, quantity_field::real>);
static_assert(!RepresentationOf<double, quantity_field::complex>);
static_assert(!RepresentationOf<utility::cartesian_vector<double>, quantity_field::complex>);
#endif

// Character concepts (the `detail::` two-tier surface) classify representation types directly and
// stay consistent with `RepresentationOf` above: the field axis (`Real` / `Complex`) is exact and
// the order axis (`Scalar` / `Vector` / `Tensor`) is rank-ordered (a lower order fills a higher
// slot), with each `XxxRepresentation` being its character plus representation validity.

// real scalar -> real field; scalar, and (degenerate) vector and tensor
static_assert(detail::Real<double> && !detail::Complex<double>);
static_assert(detail::Scalar<double> && detail::Vector<double> && detail::Tensor<double>);
static_assert(detail::ScalarRepresentation<double> && detail::VectorRepresentation<double> &&
              detail::TensorRepresentation<double>);
static_assert(detail::Real<int> && !detail::Complex<int>);
static_assert(detail::Scalar<int> && detail::Vector<int> && detail::Tensor<int>);
static_assert(detail::ScalarRepresentation<int> && detail::VectorRepresentation<int> &&
              detail::TensorRepresentation<int>);

#if MP_UNITS_HOSTED
// real vector -> real field; not a scalar, is a vector and (degenerate) tensor
static_assert(detail::Real<utility::cartesian_vector<double>> && !detail::Complex<utility::cartesian_vector<double>>);
static_assert(!detail::Scalar<utility::cartesian_vector<double>> && detail::Vector<utility::cartesian_vector<double>> &&
              detail::Tensor<utility::cartesian_vector<double>>);
static_assert(!detail::ScalarRepresentation<utility::cartesian_vector<double>> &&
              detail::VectorRepresentation<utility::cartesian_vector<double>> &&
              detail::TensorRepresentation<utility::cartesian_vector<double>>);

// real tensor -> real field; only a tensor
static_assert(detail::Real<utility::cartesian_tensor<double>> && !detail::Complex<utility::cartesian_tensor<double>>);
static_assert(!detail::Scalar<utility::cartesian_tensor<double>> &&
              !detail::Vector<utility::cartesian_tensor<double>> && detail::Tensor<utility::cartesian_tensor<double>>);
static_assert(!detail::ScalarRepresentation<utility::cartesian_tensor<double>> &&
              !detail::VectorRepresentation<utility::cartesian_tensor<double>> &&
              detail::TensorRepresentation<utility::cartesian_tensor<double>>);

// complex scalar -> complex field; scalar, and (degenerate) vector and tensor
static_assert(!detail::Real<std::complex<double>> && detail::Complex<std::complex<double>>);
static_assert(detail::Scalar<std::complex<double>> && detail::Vector<std::complex<double>> &&
              detail::Tensor<std::complex<double>>);
static_assert(detail::ScalarRepresentation<std::complex<double>> &&
              detail::VectorRepresentation<std::complex<double>> && detail::TensorRepresentation<std::complex<double>>);

// complex vector / complex tensor -> complex field; order unchanged from their real counterparts
static_assert(!detail::Real<utility::cartesian_vector<std::complex<double>>> &&
              detail::Complex<utility::cartesian_vector<std::complex<double>>>);
static_assert(!detail::Scalar<utility::cartesian_vector<std::complex<double>>> &&
              detail::Vector<utility::cartesian_vector<std::complex<double>>> &&
              detail::Tensor<utility::cartesian_vector<std::complex<double>>>);
static_assert(!detail::Real<utility::cartesian_tensor<std::complex<double>>> &&
              detail::Complex<utility::cartesian_tensor<std::complex<double>>>);
static_assert(!detail::Vector<utility::cartesian_tensor<std::complex<double>>> &&
              detail::Tensor<utility::cartesian_tensor<std::complex<double>>>);

// `Real` / `Complex` are field-only: they classify the field axis without implying a usable
// representation. `bool` has a real field and is even a `Scalar` by its math, yet it is opted out of
// being a representation by `disable_representation<bool>`, so no `*Representation` concept accepts it.
static_assert(detail::Real<bool> && detail::Scalar<bool>);
static_assert(disable_representation<bool> && !detail::ScalarRepresentation<bool>);
#endif

// Quantity
static_assert(Quantity<quantity<si::metre>>);
static_assert(Quantity<quantity<isq::length[si::metre]>>);
static_assert(Quantity<quantity<si::metre, int>>);
static_assert(Quantity<quantity<isq::length[si::metre], int>>);
#if MP_UNITS_HOSTED
static_assert(!Quantity<std::chrono::seconds>);
#endif
static_assert(!Quantity<quantity_point<si::metre, my_origin>>);
static_assert(!Quantity<decltype(isq::length[si::metre])>);

// QuantityOf
static_assert(QuantityOf<quantity<si::metre>, isq::length>);
static_assert(QuantityOf<quantity<si::metre>, isq::radius>);
static_assert(!QuantityOf<quantity<si::second>, isq::length>);
static_assert(QuantityOf<quantity<isq::length[si::metre]>, isq::length>);
static_assert(!QuantityOf<quantity<isq::length[si::metre]>, isq::radius>);
static_assert(QuantityOf<quantity<isq::radius[si::metre]>, isq::length>);
static_assert(QuantityOf<quantity<isq::radius[si::metre]>, isq::radius>);
static_assert(!QuantityOf<quantity<si::second>, isq::dim_length>);

static_assert(QuantityOf<quantity<one>, dimensionless>);
static_assert(QuantityOf<quantity<dimensionless[one]>, dimensionless>);
static_assert(QuantityOf<quantity<isq::rotation[one]>, isq::rotation>);
static_assert(QuantityOf<quantity<isq::rotation[one]>, dimensionless>);
static_assert(QuantityOf<quantity<si::radian>, isq::angular_measure>);
static_assert(!QuantityOf<quantity<si::radian>, dimensionless>);
static_assert(QuantityOf<quantity<isq::angular_measure[si::radian]>, isq::angular_measure>);
static_assert(!QuantityOf<quantity<isq::angular_measure[si::radian]>, dimensionless>);
static_assert(QuantityOf<quantity<one>, isq::rotation>);
static_assert(QuantityOf<quantity<one>, isq::angular_measure>);
static_assert(!QuantityOf<quantity<dimensionless[one]>, isq::rotation>);
static_assert(!QuantityOf<quantity<dimensionless[one]>, isq::angular_measure>);

// QuantityLike
#if MP_UNITS_HOSTED
static_assert(QuantityLike<std::chrono::seconds>);
static_assert(QuantityLike<std::chrono::hours>);
#endif
static_assert(!QuantityLike<quantity<isq::duration[si::second]>>);
static_assert(!QuantityLike<quantity_point<isq::length[si::metre], my_origin>>);
static_assert(!QuantityLike<int>);

// QuantityPoint
static_assert(QuantityPoint<quantity_point<si::metre, my_origin>>);
static_assert(QuantityPoint<quantity_point<si::metre, my_relative_origin>>);
static_assert(QuantityPoint<quantity_point<isq::length[si::metre], my_origin>>);
static_assert(QuantityPoint<quantity_point<isq::length[si::metre], my_relative_origin, int>>);
static_assert(QuantityPoint<quantity_point<isq::radius[si::metre], my_origin>>);
static_assert(QuantityPoint<quantity_point<isq::radius[si::metre], my_relative_origin>>);
static_assert(!QuantityPoint<decltype(isq::length[si::metre])>);
static_assert(!QuantityPoint<absolute_point_origin<isq::length>>);
static_assert(!QuantityPoint<struct my_origin>);
static_assert(!QuantityPoint<struct my_relative_origin>);
#if MP_UNITS_HOSTED
static_assert(!QuantityPoint<std::chrono::seconds>);
static_assert(!QuantityPoint<std::chrono::time_point<std::chrono::system_clock>>);
#endif
static_assert(!QuantityPoint<int>);

// QuantityPointOf
static_assert(QuantityPointOf<quantity_point<si::metre, my_origin>, isq::length>);
static_assert(QuantityPointOf<quantity_point<si::metre, my_origin>, isq::radius>);
static_assert(QuantityPointOf<quantity_point<isq::length[si::metre], my_origin>, isq::length>);
static_assert(!QuantityPointOf<quantity_point<isq::length[si::metre], my_origin>, isq::radius>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_origin>, isq::length>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_origin>, isq::radius>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_relative_origin>, isq::length>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_relative_origin>, isq::radius>);
static_assert(QuantityPointOf<quantity_point<si::metre, my_origin>, my_origin>);
static_assert(QuantityPointOf<quantity_point<si::metre, my_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<si::metre, my_relative_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<si::metre, my_relative_origin>, my_origin>);
static_assert(QuantityPointOf<quantity_point<isq::length[si::metre], my_origin>, my_origin>);
static_assert(QuantityPointOf<quantity_point<isq::length[si::metre], my_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<isq::length[si::metre], my_relative_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<isq::length[si::metre], my_relative_origin>, my_origin>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_origin>, my_origin>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_relative_origin>, my_relative_origin>);
static_assert(QuantityPointOf<quantity_point<isq::radius[si::metre], my_relative_origin>, my_origin>);

// PointOrigin
static_assert(PointOrigin<struct my_origin>);
static_assert(PointOrigin<struct my_relative_origin>);
static_assert(!PointOrigin<absolute_point_origin<isq::length>>);
static_assert(!PointOrigin<relative_point_origin<my_origin + 42 * si::metre>>);
static_assert(!PointOrigin<quantity_point<si::metre, my_origin>>);
static_assert(!PointOrigin<quantity_point<isq::length[si::metre], my_origin>>);
static_assert(!PointOrigin<quantity_point<isq::radius[si::metre], my_origin>>);
static_assert(!PointOrigin<decltype(isq::length[si::metre])>);
#if MP_UNITS_HOSTED
static_assert(!PointOrigin<std::chrono::seconds>);
static_assert(!PointOrigin<std::chrono::time_point<std::chrono::system_clock>>);
#endif
static_assert(!PointOrigin<int>);

// PointOriginFor
static_assert(PointOriginFor<struct my_origin, isq::length>);
static_assert(PointOriginFor<struct my_origin, isq::radius>);
static_assert(!PointOriginFor<struct my_origin, isq::time>);
static_assert(PointOriginFor<struct my_relative_origin, isq::length>);
static_assert(PointOriginFor<struct my_relative_origin, isq::radius>);
static_assert(!PointOriginFor<struct my_relative_origin, isq::time>);
static_assert(!PointOriginFor<quantity_point<si::metre, my_origin>, isq::length>);
static_assert(!PointOriginFor<quantity_point<si::metre, my_origin>, isq::radius>);
static_assert(!PointOriginFor<quantity_point<si::metre, my_origin>, isq::time>);
static_assert(!PointOriginFor<quantity_point<isq::length[si::metre], my_origin>, isq::length>);
static_assert(!PointOriginFor<quantity_point<isq::length[si::metre], my_origin>, isq::radius>);
static_assert(!PointOriginFor<quantity_point<isq::length[si::metre], my_origin>, isq::time>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_origin>, isq::length>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_origin>, isq::radius>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_origin>, isq::time>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_relative_origin>, isq::length>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_relative_origin>, isq::radius>);
static_assert(!PointOriginFor<quantity_point<isq::radius[si::metre], my_relative_origin>, isq::time>);
static_assert(!PointOriginFor<decltype(isq::length[si::metre]), isq::length>);
#if MP_UNITS_HOSTED
static_assert(!PointOriginFor<std::chrono::seconds, isq::length>);
static_assert(!PointOriginFor<std::chrono::time_point<std::chrono::system_clock>, isq::length>);
#endif
static_assert(!PointOriginFor<int, isq::length>);

// QuantityPointLike
#if MP_UNITS_HOSTED
static_assert(QuantityPointLike<std::chrono::time_point<std::chrono::system_clock>>);
static_assert(!QuantityPointLike<std::chrono::seconds>);
#endif
static_assert(!QuantityPointLike<quantity<isq::time[si::second]>>);
static_assert(!QuantityPointLike<quantity_point<si::metre, my_origin>>);
static_assert(!QuantityPointLike<int>);

// Quantity Character Concepts

#if MP_UNITS_HOSTED
// TODO provide support for the below when quantity specifications expressions are done
static_assert(detail::Scalar<quantity<one>>);
static_assert(detail::Scalar<quantity<one, int>>);
static_assert(detail::Scalar<quantity<si::metre>>);
static_assert(detail::Scalar<quantity<isq::speed[si::metre / si::second], int>>);
// static_assert(detail::Scalar<quantity<isq::complex_power[si::volt * si::ampere], std::complex<double>>>);

static_assert(!detail::Scalar<quantity<one, utility::cartesian_vector<double>>>);
static_assert(!detail::Scalar<quantity<si::metre, utility::cartesian_vector<double>>>);
static_assert(!detail::Scalar<quantity<isq::velocity[si::metre / si::second], utility::cartesian_vector<double>>>);
static_assert(!detail::Scalar<quantity_point<one>>);
static_assert(!detail::Scalar<quantity_point<si::metre>>);

static_assert(detail::RealScalar<quantity<one>>);
static_assert(detail::RealScalar<quantity<one, int>>);
static_assert(detail::RealScalar<quantity<si::metre>>);
static_assert(detail::RealScalar<quantity<isq::speed[si::metre / si::second], int>>);
// static_assert(!detail::RealScalar<quantity<isq::velocity[si::metre / si::second], int>>);
static_assert(!detail::RealScalar<quantity<isq::complex_power[si::volt * si::ampere], std::complex<double>>>);

static_assert(!detail::ComplexScalar<quantity<one>>);
static_assert(!detail::ComplexScalar<quantity<one, int>>);
static_assert(!detail::ComplexScalar<quantity<si::metre>>);
static_assert(!detail::ComplexScalar<quantity<isq::speed[si::metre / si::second], int>>);
static_assert(!detail::ComplexScalar<quantity<isq::velocity[si::metre / si::second], int>>);
// static_assert(detail::ComplexScalar<quantity<isq::complex_power[si::volt * si::ampere], std::complex<double>>>);

// static_assert(detail::Vector<quantity<one>>);
// static_assert(detail::Vector<quantity<one, int>>);
// static_assert(detail::Vector<quantity<si::metre>>);
static_assert(!detail::VectorRepresentation<quantity<isq::speed[si::metre / si::second], int>>);
// static_assert(detail::VectorRepresentation<quantity<isq::velocity[si::metre / si::second], int>>);
static_assert(!detail::VectorRepresentation<quantity<isq::complex_power[si::volt * si::ampere], std::complex<double>>>);
// static_assert(detail::Vector<quantity<one>, utility::cartesian_vector<double>>);
// static_assert(detail::Vector<quantity<one, utility::cartesian_vector<int>>>);
// static_assert(detail::Vector<quantity<si::metre>, utility::cartesian_vector<double>>);
// static_assert(detail::Vector<quantity<isq::velocity[si::metre / si::second], utility::cartesian_vector<int>>>);

// Scaling concept classification
// double: floating-point → UsesFloatingPointScaling only
static_assert(detail::UsesFloatingPointScaling<double>);
static_assert(!detail::UsesIntegerScaling<double>);
// int: integral scalar → UsesIntegerScaling only
static_assert(!detail::UsesFloatingPointScaling<int>);
static_assert(detail::UsesIntegerScaling<int>);
// utility::cartesian_vector<double>: FP element type → UsesFloatingPointScaling
static_assert(detail::UsesFloatingPointScaling<utility::cartesian_vector<double>>);
static_assert(!detail::UsesIntegerScaling<utility::cartesian_vector<double>>);
// utility::cartesian_vector<int>: integral element → UsesIntegerScaling
static_assert(!detail::UsesFloatingPointScaling<utility::cartesian_vector<int>>);
static_assert(detail::UsesIntegerScaling<utility::cartesian_vector<int>>);

#endif

// A const-qualified symbolic type is still classified, as the standard's type categories accept a
// const-qualified type. `decltype` of a named entity is const, so every check below takes one.
inline constexpr struct my_measured_constant final :
    named_constant<"k", mag<2> * si::metre, relative_standard_uncertainty{mag_ratio<1, 100>}> {
} my_measured_constant;
inline constexpr auto length_in_metres = isq::length[si::metre];

static_assert(detail::SymbolicConstant<decltype(si::metre)>);

static_assert(Dimension<decltype(isq::dim_length)>);
static_assert(Dimension<decltype(dim_speed)>);
static_assert(!Dimension<decltype(isq::length)>);
static_assert(DimensionOf<decltype(isq::dim_length), isq::dim_length>);
static_assert(!DimensionOf<decltype(isq::dim_length), isq::dim_time>);
static_assert(detail::BaseDimension<decltype(isq::dim_length)>);
static_assert(!detail::BaseDimension<decltype(dim_speed)>);

static_assert(QuantitySpec<decltype(isq::length)>);
static_assert(QuantitySpec<decltype(speed)>);
static_assert(QuantitySpec<decltype(kind_of<isq::length>)>);
static_assert(!QuantitySpec<decltype(si::metre)>);
static_assert(QuantitySpecOf<decltype(isq::radius), isq::length>);
static_assert(!QuantitySpecOf<decltype(isq::length), isq::duration>);
static_assert(detail::NamedQuantitySpec<decltype(isq::length)>);
static_assert(!detail::NamedQuantitySpec<decltype(speed)>);
static_assert(detail::DerivedQuantitySpec<decltype(speed)>);
static_assert(!detail::DerivedQuantitySpec<decltype(isq::length)>);
static_assert(detail::QuantityKindSpec<decltype(kind_of<isq::length>)>);
static_assert(!detail::QuantityKindSpec<decltype(isq::length)>);

static_assert(Unit<decltype(si::metre)>);
static_assert(Unit<decltype(si::kilogram)>);
static_assert(Unit<decltype(my_measured_constant)>);
static_assert(!Unit<decltype(isq::length)>);
static_assert(PrefixableUnit<decltype(si::metre)>);
static_assert(!PrefixableUnit<decltype(si::kilogram)>);
static_assert(UnitOf<decltype(si::metre), isq::length>);
static_assert(!UnitOf<decltype(si::metre), isq::duration>);
static_assert(detail::OffsetUnit<decltype(si::degree_Celsius)>);
static_assert(!detail::OffsetUnit<decltype(si::metre)>);
static_assert(MeasuredConstant<decltype(my_measured_constant)>);
static_assert(!MeasuredConstant<decltype(si::metre)>);
static_assert(UnitMagnitude<decltype(mag<10>)>);
static_assert(!UnitMagnitude<decltype(si::metre)>);

static_assert(Reference<decltype(si::metre)>);
static_assert(Reference<decltype(length_in_metres)>);
static_assert(!Reference<decltype(isq::length)>);
static_assert(ReferenceOf<decltype(isq::radius[si::metre]), isq::length>);
static_assert(!ReferenceOf<decltype(si::second), isq::length>);

static_assert(PointOrigin<decltype(my_origin)>);
static_assert(PointOrigin<decltype(my_relative_origin)>);
static_assert(!PointOrigin<decltype(si::metre)>);
static_assert(PointOriginFor<decltype(my_origin), isq::length>);
static_assert(!PointOriginFor<decltype(my_origin), isq::duration>);
static_assert(detail::AbsolutePointOrigin<decltype(my_origin)>);
static_assert(!detail::AbsolutePointOrigin<decltype(my_relative_origin)>);

static_assert(Quantity<const quantity<si::metre>>);
static_assert(QuantityOf<const quantity<si::metre>, isq::length>);
static_assert(QuantityPoint<const quantity_point<si::metre, my_origin>>);
static_assert(QuantityPointOf<const quantity_point<si::metre, my_origin>, isq::length>);

// neither a reference nor a volatile type is a symbolic type
static_assert(!detail::SymbolicConstant<struct si::metre&>);
static_assert(!detail::SymbolicConstant<volatile struct si::metre>);
static_assert(!Dimension<const struct isq::dim_length&>);
static_assert(!Dimension<volatile struct isq::dim_length>);
static_assert(!QuantitySpec<const struct isq::length&>);
static_assert(!QuantitySpec<volatile struct isq::length>);
static_assert(!Unit<const struct si::metre&>);
static_assert(!Unit<volatile struct si::metre>);
static_assert(!Unit<const volatile struct si::metre>);
static_assert(!Reference<const struct si::metre&>);
static_assert(!Reference<const decltype(length_in_metres)&>);
static_assert(!Reference<volatile decltype(isq::length[si::metre])>);
static_assert(!Reference<volatile decltype(length_in_metres)>);
static_assert(!PointOrigin<const struct my_origin&>);
static_assert(!PointOrigin<volatile struct my_origin>);
static_assert(!UnitMagnitude<const decltype(mag<10>)&>);
static_assert(!UnitMagnitude<volatile decltype(mag<10>)>);

// const does not hide the bases that the concepts exclude
static_assert(!Dimension<const base_dimension<"L">>);
static_assert(!Unit<const named_unit<"?", kind_of<isq::length>>>);
static_assert(!Unit<const prefixed_unit<"?", mag<10>, si::second>>);
static_assert(!PointOrigin<const absolute_point_origin<isq::length>>);
static_assert(!PointOrigin<const relative_point_origin<my_origin + 42 * si::metre>>);

// Only a non-const type may enter a symbolic expression, or a `const` second divided by a second
// would not simplify. Each position is checked on its own, and `valid_symbolic_terms` proves that
// the same expressions are well-formed with non-const arguments.
template<typename U, typename QS, typename D, typename R, typename M>
concept valid_symbolic_terms = requires {
  typename detail::type_list<U>;
  typename per<U>;
  typename power<U, 2>;
  typename derived_unit<U, per<struct si::second>>;
  typename derived_dimension<D, per<struct isq::dim_time>>;
  typename derived_quantity_spec<QS, per<struct isq::duration>>;
  typename scaled_unit<mag<10>, U>;
  typename common_unit<U, si::centi_<struct si::metre>>;
  typename common_unit<si::centi_<struct si::metre>, U>;
  typename common_unit<si::centi_<struct si::metre>, si::milli_<struct si::metre>, U>;
  typename reference<QS, struct si::metre>;
  typename reference<struct isq::length, U>;
  kind_of_<QS>{};
  typename delta_<U>;
  typename delta_<R>;
  typename point_<U>;
  typename point_<R>;
  typename standard_uncertainty<U>;
  typename relative_standard_uncertainty<M>;
  typename si::quecto_<U>;
  typename si::ronto_<U>;
  typename si::yocto_<U>;
  typename si::zepto_<U>;
  typename si::atto_<U>;
  typename si::femto_<U>;
  typename si::pico_<U>;
  typename si::nano_<U>;
  typename si::micro_<U>;
  typename si::milli_<U>;
  typename si::centi_<U>;
  typename si::deci_<U>;
  typename si::deca_<U>;
  typename si::hecto_<U>;
  typename si::kilo_<U>;
  typename si::mega_<U>;
  typename si::giga_<U>;
  typename si::tera_<U>;
  typename si::peta_<U>;
  typename si::exa_<U>;
  typename si::zetta_<U>;
  typename si::yotta_<U>;
  typename si::ronna_<U>;
  typename si::quetta_<U>;
  typename iec::kibi_<U>;
  typename iec::mebi_<U>;
  typename iec::gibi_<U>;
  typename iec::tebi_<U>;
  typename iec::pebi_<U>;
  typename iec::exbi_<U>;
  typename iec::zebi_<U>;
  typename iec::yobi_<U>;
};
static_assert(
  valid_symbolic_terms<struct si::metre, struct isq::length, struct isq::dim_length,
                       std::remove_const_t<decltype(length_in_metres)>, std::remove_const_t<decltype(mag<10>)>>);

template<typename U, typename QS, typename D, typename R, typename M>
concept invalid_symbolic_terms = requires {
  requires !requires { typename detail::type_list<U>; };
  requires !requires { typename per<U>; };
  requires !requires { typename power<U, 2>; };
  requires !requires { typename derived_unit<U, per<struct si::second>>; };
  requires !requires { typename derived_dimension<D, per<struct isq::dim_time>>; };
  requires !requires { typename derived_quantity_spec<QS, per<struct isq::duration>>; };
  requires !requires { typename scaled_unit<mag<10>, U>; };
  requires !requires { typename common_unit<U, si::centi_<struct si::metre>>; };
  requires !requires { typename common_unit<si::centi_<struct si::metre>, U>; };
  requires !requires { typename common_unit<si::centi_<struct si::metre>, si::milli_<struct si::metre>, U>; };
  requires !requires { typename reference<QS, struct si::metre>; };
  requires !requires { typename reference<struct isq::length, U>; };
  requires !requires { kind_of_<QS>{}; };  // the primary is unconstrained, so only completing it fails
  requires !requires { typename delta_<U>; };
  requires !requires { typename delta_<R>; };
  requires !requires { typename point_<U>; };
  requires !requires { typename point_<R>; };
  requires !requires { typename standard_uncertainty<U>; };
  requires !requires { typename relative_standard_uncertainty<M>; };
  requires !requires { typename si::quecto_<U>; };
  requires !requires { typename si::ronto_<U>; };
  requires !requires { typename si::yocto_<U>; };
  requires !requires { typename si::zepto_<U>; };
  requires !requires { typename si::atto_<U>; };
  requires !requires { typename si::femto_<U>; };
  requires !requires { typename si::pico_<U>; };
  requires !requires { typename si::nano_<U>; };
  requires !requires { typename si::micro_<U>; };
  requires !requires { typename si::milli_<U>; };
  requires !requires { typename si::centi_<U>; };
  requires !requires { typename si::deci_<U>; };
  requires !requires { typename si::deca_<U>; };
  requires !requires { typename si::hecto_<U>; };
  requires !requires { typename si::kilo_<U>; };
  requires !requires { typename si::mega_<U>; };
  requires !requires { typename si::giga_<U>; };
  requires !requires { typename si::tera_<U>; };
  requires !requires { typename si::peta_<U>; };
  requires !requires { typename si::exa_<U>; };
  requires !requires { typename si::zetta_<U>; };
  requires !requires { typename si::yotta_<U>; };
  requires !requires { typename si::ronna_<U>; };
  requires !requires { typename si::quetta_<U>; };
  requires !requires { typename iec::kibi_<U>; };
  requires !requires { typename iec::mebi_<U>; };
  requires !requires { typename iec::gibi_<U>; };
  requires !requires { typename iec::tebi_<U>; };
  requires !requires { typename iec::pebi_<U>; };
  requires !requires { typename iec::exbi_<U>; };
  requires !requires { typename iec::zebi_<U>; };
  requires !requires { typename iec::yobi_<U>; };
};
static_assert(invalid_symbolic_terms<decltype(si::metre), decltype(isq::length), decltype(isq::dim_length),
                                     decltype(length_in_metres), decltype(mag<10>)>);

// an operand's const is dropped by deduction, so the expression still simplifies
static_assert(decltype(si::second){} / si::second == one);
static_assert(si::kilo<si::metre> == si::kilo<decltype(si::metre){}>);
static_assert(si::kilo<si::metre> * si::second / decltype(si::second){} == si::kilo<si::metre>);

}  // namespace

// A full explicit specialization of `tensor_order` for an ambiguous type is permitted by the
// undefined primary (a constrained-out primary would reject `template<>` with "does not match any
// declaration"). At global scope, which encloses `mp_units`, so it can specialize the trait.
namespace order_spec_test {
struct ambiguous {
  double operator[](std::size_t) const;
  double operator()(std::size_t, std::size_t) const;
};
}  // namespace order_spec_test
template<>
constexpr std::size_t mp_units::tensor_order<order_spec_test::ambiguous> = 1;
static_assert(mp_units::tensor_order<order_spec_test::ambiguous> == 1);
