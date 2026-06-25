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

#include <mp-units/compat_macros.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#if MP_UNITS_HOSTED
#include <mp-units/cartesian_tensor.h>
#include <mp-units/cartesian_vector.h>
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
static_assert(!detail::BaseDimension<MP_UNITS_NONCONST_TYPE(dim_speed)>);
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
static_assert(Dimension<MP_UNITS_NONCONST_TYPE(dim_speed)>);
static_assert(!Dimension<base_dimension<"L">>);
static_assert(!Dimension<struct si::metre>);
static_assert(!Dimension<int>);

// DimensionOf
static_assert(DimensionOf<struct isq::dim_length, isq::dim_length>);
static_assert(DimensionOf<struct isq::dim_length, isq::height.dimension>);
static_assert(DimensionOf<struct isq::dim_length, isq::radius.dimension>);
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
static_assert(DimensionOf<decltype(isq::dim_length / isq::dim_time), isq::speed.dimension>);
static_assert(DimensionOf<decltype(isq::force.dimension * isq::duration.dimension), isq::impulse.dimension>);
static_assert(DimensionOf<decltype(isq::angular_momentum.dimension / isq::angular_velocity.dimension),
                          isq::moment_of_inertia.dimension>);

// QuantitySpec
inline constexpr auto speed = isq::length / isq::duration;

static_assert(QuantitySpec<struct isq::length>);
static_assert(QuantitySpec<struct isq::radius>);
static_assert(QuantitySpec<struct isq::speed>);
static_assert(QuantitySpec<kind_of_<struct isq::length>>);
static_assert(QuantitySpec<decltype(isq::length / isq::duration)>);
static_assert(QuantitySpec<decltype(pow<2>(isq::length))>);
static_assert(QuantitySpec<struct dimensionless>);
static_assert(QuantitySpec<MP_UNITS_NONCONST_TYPE(speed)>);
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
static_assert(!detail::NamedQuantitySpec<MP_UNITS_NONCONST_TYPE(speed)>);
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
static_assert(detail::DerivedQuantitySpec<MP_UNITS_NONCONST_TYPE(speed)>);
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
static_assert(!detail::QuantityKindSpec<MP_UNITS_NONCONST_TYPE(speed)>);
static_assert(!detail::QuantityKindSpec<struct isq::dim_length>);
static_assert(!detail::QuantityKindSpec<int>);

// Unit
static_assert(Unit<struct si::metre>);
static_assert(Unit<MP_UNITS_NONCONST_TYPE(si::kilogram)>);
static_assert(Unit<si::kilo_<struct si::gram>>);
static_assert(Unit<decltype(si::metre / si::second)>);
static_assert(Unit<decltype(inverse(si::second))>);
static_assert(Unit<decltype(mag<10> * si::second)>);
static_assert(Unit<decltype(square(si::metre))>);
static_assert(Unit<decltype(pow<2>(si::metre))>);
static_assert(Unit<struct si::standard_gravity>);
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
static_assert(!PrefixableUnit<MP_UNITS_NONCONST_TYPE(si::kilogram)>);
static_assert(!PrefixableUnit<si::kilo_<struct si::gram>>);
static_assert(!PrefixableUnit<decltype(si::metre / si::second)>);
static_assert(!PrefixableUnit<decltype(inverse(si::second))>);
static_assert(!PrefixableUnit<decltype(mag<10> * si::second)>);
static_assert(!PrefixableUnit<decltype(square(si::metre))>);
static_assert(!PrefixableUnit<decltype(pow<2>(si::metre))>);
static_assert(!PrefixableUnit<struct si::standard_gravity>);
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
static_assert(Unit<MP_UNITS_NONCONST_TYPE(si::kilogram)>);
static_assert(Unit<si::kilo_<struct si::gram>>);
static_assert(Unit<decltype(si::metre / si::second)>);
static_assert(Unit<decltype(inverse(si::second))>);
static_assert(Unit<decltype(mag<10> * si::second)>);
static_assert(Unit<decltype(square(si::metre))>);
static_assert(Unit<decltype(pow<2>(si::metre))>);
static_assert(Unit<struct si::standard_gravity>);
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
static_assert(UnitOf<MP_UNITS_NONCONST_TYPE(si::kilogram), isq::mass>);
static_assert(UnitOf<struct si::hertz, isq::frequency>);
static_assert(UnitOf<struct si::hertz, inverse(isq::duration)>);
static_assert(UnitOf<struct one, dimensionless>);
static_assert(UnitOf<struct percent, dimensionless>);
static_assert(UnitOf<struct si::radian, isq::angular_measure>);
static_assert(UnitOf<struct si::degree, isq::angular_measure>);
static_assert(UnitOf<struct one, isq::angular_measure>);
static_assert(UnitOf<struct percent, isq::angular_measure>);
static_assert(UnitOf<MP_UNITS_NONCONST_TYPE(si::radian / si::second), isq::angular_velocity>);
static_assert(UnitOf<MP_UNITS_NONCONST_TYPE(one / si::second), isq::angular_velocity>);
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
// A tensor of order zero is a scalar and a tensor of order one is a vector (ISO 80000-2:2019, 18),
// so a lower-rank representation can stand in for a higher-rank quantity (never the reverse).
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

// cartesian_vector<double>: 3D vector, not scalar; also a degenerate tensor (a vector is a
// tensor of the first order)
static_assert(RepresentationOf<cartesian_vector<double>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_vector<double>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_vector<double>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<cartesian_vector<double>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_vector<double>, quantity_tensor_order::tensor>);

// cartesian_vector<int>: integer element type is supported (norm() returns double via std::hypot)
static_assert(RepresentationOf<cartesian_vector<int>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_vector<int>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_vector<int>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<cartesian_vector<int>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_vector<int>, quantity_tensor_order::tensor>);

// cartesian_vector<complex<double>>: a complex 3-vector. It exposes real()/imag(), so its field is
// complex; it fills the vector order slot (and degenerately the tensor order slot) but not the
// scalar one.
static_assert(!RepresentationOf<cartesian_vector<std::complex<double>>, quantity_field::real>);
static_assert(RepresentationOf<cartesian_vector<std::complex<double>>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_vector<std::complex<double>>, quantity_tensor_order::scalar>);
static_assert(RepresentationOf<cartesian_vector<std::complex<double>>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_vector<std::complex<double>>, quantity_tensor_order::tensor>);

// cartesian_tensor<double>: second-order tensor; tensor-only - never a (lower-rank) vector or scalar
static_assert(RepresentationOf<cartesian_tensor<double>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_tensor<double>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_tensor<double>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<cartesian_tensor<double>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_tensor<double>, quantity_tensor_order::tensor>);

// cartesian_tensor<int>: integer element type is supported (Frobenius norm returns double)
static_assert(RepresentationOf<cartesian_tensor<int>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_tensor<int>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_tensor<int>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<cartesian_tensor<int>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_tensor<int>, quantity_tensor_order::tensor>);

// cartesian_tensor<complex<double>>: a complex second-order tensor. Field complex (via real()/imag());
// fills the tensor order slot only, never the vector or scalar one.
static_assert(!RepresentationOf<cartesian_tensor<std::complex<double>>, quantity_field::real>);
static_assert(RepresentationOf<cartesian_tensor<std::complex<double>>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_tensor<std::complex<double>>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<cartesian_tensor<std::complex<double>>, quantity_tensor_order::vector>);
static_assert(RepresentationOf<cartesian_tensor<std::complex<double>>, quantity_tensor_order::tensor>);

// quantity types must never themselves be a representation (disable_representation guard)
static_assert(!RepresentationOf<quantity<si::metre>, quantity_field::real>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_field::complex>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::vector>);
static_assert(!RepresentationOf<quantity<si::metre>, quantity_tensor_order::tensor>);

// cartesian_tensor whose element is a quantity must not be a representation
static_assert(!RepresentationOf<cartesian_tensor<quantity<si::metre>>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_tensor<quantity<si::metre>>, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::vector>);
static_assert(!RepresentationOf<cartesian_tensor<quantity<si::metre>>, quantity_tensor_order::tensor>);

// a tensor-character ISQ quantity accepts a second-order tensor representation
static_assert(Quantity<quantity<isq::stress[si::pascal], cartesian_tensor<double>>>);

// `VectorRepresentation` subsumes `TensorRepresentation` (the latter is defined as the former plus
// the order-2 case), so a VectorRepresentation-constrained overload is preferred over a
// TensorRepresentation-constrained one for vector representations, while a genuine tensor matches
// only the TensorRepresentation overload. This locks the subsumption ordering used to rank overloads.
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
static_assert(grade<cartesian_tensor<double>>() == 2);  // only TensorRepresentation matches
static_assert(grade<cartesian_vector<double>>() == 1);  // VectorRepresentation wins (subsumes Tensor)
static_assert(grade<double>() == 0);                    // ScalarRepresentation wins (subsumes Vector)
static_assert(grade<std::complex<double>>() == 0);      // a complex scalar is order 0 too
}  // namespace subsumption_check

// cartesian_vector whose element is a quantity must not be a representation
static_assert(!RepresentationOf<cartesian_vector<quantity<si::metre>>, quantity_tensor_order::vector>);

static_assert(!RepresentationOf<std::chrono::seconds, quantity_tensor_order::scalar>);
static_assert(!RepresentationOf<std::string, quantity_tensor_order::scalar>);

// `tensor_order` is detected from a type's structure: one-index access is a vector (order 1),
// two-index access a tensor (order 2), otherwise a scalar (order 0). A type with two-index access
// that is conceptually a vector (an Eigen N×1 column matrix) therefore defaults to order 2 and must
// declare order 1 by specializing `tensor_order` (the Eigen integration does exactly that).
namespace order_detection {
struct scalar_shaped {};
struct vector_shaped {
  double operator[](std::size_t) const;
};
struct matrix_shaped {
  double operator()(std::size_t, std::size_t) const;
};
// GCC 12 prematurely defines `__cpp_multidimensional_subscript` without implementing `t[i, j]`, so
// the library skips that probe there (see `detect_tensor_order`); keep this test in step.
#if __cpp_multidimensional_subscript && MP_UNITS_COMP_GCC != 12
struct multidim_subscript_shaped {
  double operator[](std::size_t, std::size_t) const;
};
#endif
}  // namespace order_detection
static_assert(tensor_order<order_detection::scalar_shaped> == 0);
static_assert(tensor_order<order_detection::vector_shaped> == 1);
static_assert(tensor_order<order_detection::matrix_shaped> == 2);
#if __cpp_multidimensional_subscript && MP_UNITS_COMP_GCC != 12
static_assert(tensor_order<order_detection::multidim_subscript_shaped> == 2);  // C++23 t[i, j]
#endif

// The legacy flat spelling still selects the right (order, field): `vector` -> (vector, real),
// `complex_scalar` -> (scalar, complex), etc. (converted at a function argument in `order_of` /
// `field_of`, never an NTTP boundary).
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(RepresentationOf<double, quantity_character::real_scalar>);
static_assert(RepresentationOf<std::complex<double>, quantity_character::complex_scalar>);
static_assert(!RepresentationOf<double, quantity_character::complex_scalar>);
static_assert(RepresentationOf<cartesian_vector<double>, quantity_character::vector>);
static_assert(RepresentationOf<cartesian_tensor<double>, quantity_character::tensor>);
MP_UNITS_DIAGNOSTIC_POP

// Field is exact (disjoint) and detected from the values, not the `real()`/`imag()` API: a genuinely
// complex representation does not satisfy a real character, and a real one does not satisfy a complex
// character. The detection recurses through wrapped elements.
static_assert(!RepresentationOf<std::complex<double>, quantity_field::real>);
static_assert(!RepresentationOf<cartesian_vector<std::complex<double>>, quantity_field::real>);
static_assert(!RepresentationOf<double, quantity_field::complex>);
static_assert(!RepresentationOf<cartesian_vector<double>, quantity_field::complex>);
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
static_assert(detail::Real<cartesian_vector<double>> && !detail::Complex<cartesian_vector<double>>);
static_assert(!detail::Scalar<cartesian_vector<double>> && detail::Vector<cartesian_vector<double>> &&
              detail::Tensor<cartesian_vector<double>>);
static_assert(!detail::ScalarRepresentation<cartesian_vector<double>> &&
              detail::VectorRepresentation<cartesian_vector<double>> &&
              detail::TensorRepresentation<cartesian_vector<double>>);

// real tensor -> real field; only a tensor
static_assert(detail::Real<cartesian_tensor<double>> && !detail::Complex<cartesian_tensor<double>>);
static_assert(!detail::Scalar<cartesian_tensor<double>> && !detail::Vector<cartesian_tensor<double>> &&
              detail::Tensor<cartesian_tensor<double>>);
static_assert(!detail::ScalarRepresentation<cartesian_tensor<double>> &&
              !detail::VectorRepresentation<cartesian_tensor<double>> &&
              detail::TensorRepresentation<cartesian_tensor<double>>);

// complex scalar -> complex field; scalar, and (degenerate) vector and tensor
static_assert(!detail::Real<std::complex<double>> && detail::Complex<std::complex<double>>);
static_assert(detail::Scalar<std::complex<double>> && detail::Vector<std::complex<double>> &&
              detail::Tensor<std::complex<double>>);
static_assert(detail::ScalarRepresentation<std::complex<double>> &&
              detail::VectorRepresentation<std::complex<double>> && detail::TensorRepresentation<std::complex<double>>);

// complex vector / complex tensor -> complex field; order unchanged from their real counterparts
static_assert(!detail::Real<cartesian_vector<std::complex<double>>> &&
              detail::Complex<cartesian_vector<std::complex<double>>>);
static_assert(!detail::Scalar<cartesian_vector<std::complex<double>>> &&
              detail::Vector<cartesian_vector<std::complex<double>>> &&
              detail::Tensor<cartesian_vector<std::complex<double>>>);
static_assert(!detail::Real<cartesian_tensor<std::complex<double>>> &&
              detail::Complex<cartesian_tensor<std::complex<double>>>);
static_assert(!detail::Vector<cartesian_tensor<std::complex<double>>> &&
              detail::Tensor<cartesian_tensor<std::complex<double>>>);

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

static_assert(!detail::Scalar<quantity<one, cartesian_vector<double>>>);
static_assert(!detail::Scalar<quantity<si::metre, cartesian_vector<double>>>);
static_assert(!detail::Scalar<quantity<isq::velocity[si::metre / si::second], cartesian_vector<double>>>);
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
// static_assert(detail::Vector<quantity<one>, cartesian_vector<double>>);
// static_assert(detail::Vector<quantity<one, cartesian_vector<int>>>);
// static_assert(detail::Vector<quantity<si::metre>, cartesian_vector<double>>);
// static_assert(detail::Vector<quantity<isq::velocity[si::metre / si::second], cartesian_vector<int>>>);

// Scaling concept classification
// double: floating-point → UsesFloatingPointScaling only
static_assert(detail::UsesFloatingPointScaling<double>);
static_assert(!detail::UsesIntegerScaling<double>);
// int: integral scalar → UsesIntegerScaling only
static_assert(!detail::UsesFloatingPointScaling<int>);
static_assert(detail::UsesIntegerScaling<int>);
// cartesian_vector<double>: FP element type → UsesFloatingPointScaling
static_assert(detail::UsesFloatingPointScaling<cartesian_vector<double>>);
static_assert(!detail::UsesIntegerScaling<cartesian_vector<double>>);
// cartesian_vector<int>: integral element → UsesIntegerScaling
static_assert(!detail::UsesFloatingPointScaling<cartesian_vector<int>>);
static_assert(detail::UsesIntegerScaling<cartesian_vector<int>>);

#endif

}  // namespace
