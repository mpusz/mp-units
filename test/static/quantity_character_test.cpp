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
#include <mp-units/framework.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/cartesian_tensor.h>
#include <mp-units/utility/cartesian_vector.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <complex>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

namespace {

using vec3 = utility::cartesian_vector<double>;
using tens3 = utility::cartesian_tensor<double>;

// ---- what a specification admits ----
// A concrete specification admits exactly one character per axis, and comparing what it admits
// against a public value is only possible through the named questions: the admitted character is a
// set, so it has no `==` against a `quantity_tensor_order`.
static_assert(detail::admits_only(detail::admitted_character(isq::speed).order, quantity_tensor_order::scalar));
static_assert(detail::admits_only(detail::admitted_character(isq::velocity).order, quantity_tensor_order::vector));
static_assert(detail::admits_only(detail::admitted_character(isq::stress).order, quantity_tensor_order::tensor));
static_assert(detail::admits_only(detail::admitted_character(isq::speed).field, quantity_field::real));
static_assert(detail::is_definite(detail::admitted_character(isq::velocity).order));

// A quantity kind constrains neither axis. Nothing in the hierarchy could say which orders it holds:
// a simple quantity is of a kind as well (its specification is the `kind_of<...>` its unit yields), so
// `cartesian_tensor{} * m / s` is a perfectly ordinary quantity of the speed kind holding a tensor.
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::scalar));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::vector));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::tensor));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).field, quantity_field::real));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).field, quantity_field::complex));
static_assert(!detail::is_definite(detail::admitted_character(get_kind(isq::velocity)).order));

// `admits` and `admits_only` differ exactly there: a kind may be a vector, but is not definitely one.
static_assert(!detail::admits_only(detail::admitted_character(get_kind(isq::velocity)).order,
                                   quantity_tensor_order::vector));

// ---- combining the characters of a derived quantity's factors ----
// The highest rank among the factors wins ...
static_assert(detail::admits_only(detail::admitted_character(isq::velocity / isq::duration).order,
                                  quantity_tensor_order::vector));
static_assert(detail::admits_only(detail::admitted_character(isq::length / isq::duration).order,
                                  quantity_tensor_order::scalar));
// ... except that a factor constraining nothing makes the result constrain nothing
static_assert(!detail::is_definite(detail::admitted_character(get_kind(isq::length) / get_kind(isq::time)).order));

// ---- the non_negative precondition, which needs a definite real scalar ----
static_assert(detail::is_real_scalar(detail::admitted_character(isq::speed)));
static_assert(!detail::is_real_scalar(detail::admitted_character(isq::velocity)));
static_assert(!detail::is_real_scalar(detail::admitted_character(get_kind(isq::speed))));

// ---- a mask converts back to a public character only when it is definite ----
// The kind case is a hard error carrying an explanatory message rather than a detectable failure, so
// it cannot be asserted here; its only caller is the deprecated `character` member.
static_assert(detail::definite_character<detail::admitted_character(isq::velocity)>().order ==
              quantity_tensor_order::vector);
static_assert(detail::definite_character<detail::admitted_character(isq::stress)>().order ==
              quantity_tensor_order::tensor);

// ---- matching a representation against a specification ----
// The slots a representation can fill are the rank closure of its own order: a scalar fills every
// slot, a vector fills vector and tensor, a tensor only tensor (ISO 80000-2:2019, 18).
static_assert(RepresentationOf<double, isq::speed>);
static_assert(RepresentationOf<double, isq::velocity>);
static_assert(RepresentationOf<double, isq::stress>);
static_assert(RepresentationOf<vec3, isq::velocity>);
static_assert(RepresentationOf<vec3, isq::stress>);
static_assert(RepresentationOf<tens3, isq::stress>);
static_assert(!RepresentationOf<vec3, isq::speed>);
static_assert(!RepresentationOf<tens3, isq::speed>);
static_assert(!RepresentationOf<tens3, isq::velocity>);

// A kind takes any of them, and needs no special case in the concept to do so.
static_assert(RepresentationOf<double, get_kind(isq::speed)>);
static_assert(RepresentationOf<vec3, get_kind(isq::speed)>);
static_assert(RepresentationOf<tens3, get_kind(isq::speed)>);

// An argument that is not a character at all answers `false` rather than hard-erroring on a
// substitution into the wrong axis type.
template<typename = void>
concept representation_of_is_answerable = requires {
  requires !RepresentationOf<int*, isq::velocity>;
  requires !RepresentationOf<vec3, isq::speed>;
};
static_assert(representation_of_is_answerable<>);

// ---- asking a quantity for its character ----
// `QuantityOf` with a character asks both halves of the question: what the specification admits, and
// what the representation can fill. Neither half suffices alone.
inline constexpr auto scalar_ = quantity_tensor_order::scalar;
inline constexpr auto vector_ = quantity_tensor_order::vector;
inline constexpr auto tensor_ = quantity_tensor_order::tensor;

static_assert(QuantityOf<quantity<isq::speed[m / s], double>, scalar_>);
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, vector_>);  // a speed is never a vector
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, tensor_>);

static_assert(QuantityOf<quantity<isq::velocity[m / s], double>, vector_>);  // a 1D velocity in a double
static_assert(QuantityOf<quantity<isq::velocity[m / s], vec3>, vector_>);
static_assert(!QuantityOf<quantity<isq::velocity[m / s], vec3>, scalar_>);
static_assert(!QuantityOf<quantity<isq::velocity[m / s], vec3>, tensor_>);

static_assert(QuantityOf<quantity<isq::stress[si::pascal], tens3>, tensor_>);
static_assert(QuantityOf<quantity<isq::stress[si::pascal], vec3>, tensor_>);  // a degenerate tensor
static_assert(!QuantityOf<quantity<isq::stress[si::pascal], tens3>, vector_>);

// A kind quantity is whatever its representation makes it, which is the whole point: this is the
// case that used to claim `scalar` for a vector.
static_assert(QuantityOf<quantity<m / s, vec3>, vector_>);
static_assert(QuantityOf<quantity<m / s, tens3>, tensor_>);
static_assert(!QuantityOf<quantity<m / s, tens3>, vector_>);  // a tensor representation is not a vector
static_assert(QuantityOf<quantity<m / s, double>, scalar_>);
static_assert(QuantityOf<quantity<m / s, double>, vector_>);  // "a kind is anything", so a double may be

// A specification argument keeps its quantity-tree meaning and does not become a character question.
// The two readings are guarded by disjoint argument types, so neither widens the other.
static_assert(QuantityOf<quantity<isq::velocity[m / s], vec3>, isq::velocity>);
static_assert(QuantityOf<quantity<isq::velocity[m / s], vec3>, isq::speed>);  // a velocity is a speed
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, isq::velocity>);

#if MP_UNITS_HOSTED
static_assert(QuantityOf<quantity<isq::speed[m / s], double>, quantity_field::real>);
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, quantity_field::complex>);
static_assert(QuantityOf<quantity<m / s, std::complex<double>>, quantity_field::complex>);
static_assert(!QuantityOf<quantity<m / s, std::complex<double>>, quantity_field::real>);
#endif

// ---- a quantity point answers the same question ----
// A point is not a `Quantity`, so `QuantityOf` cannot be asked about one. `QuantityPointOf` is where
// a point user asks, and it agrees with the quantity of the same reference and representation.
using vel_point = quantity_point<isq::velocity[m / s], default_point_origin(isq::velocity[m / s]), double>;
static_assert(QuantityPointOf<vel_point, vector_>);
static_assert(!QuantityPointOf<vel_point, tensor_>);
static_assert(QuantityPointOf<vel_point, quantity_field::real>);

// and it weighs the specification the same way, so a scalar point is never a vector one
using speed_point = quantity_point<isq::speed[m / s], default_point_origin(isq::speed[m / s]), double>;
static_assert(QuantityPointOf<speed_point, scalar_>);
static_assert(!QuantityPointOf<speed_point, vector_>);

// The representation is weighed for a point as well: a kind admits every character, so only the
// representation can reject a complex or a tensor point.
using complex_point = quantity_point<si::metre, default_point_origin(si::metre), std::complex<double>>;
static_assert(QuantityPointOf<complex_point, quantity_field::complex>);
static_assert(!QuantityPointOf<complex_point, quantity_field::real>);
using tensor_point = quantity_point<si::metre, default_point_origin(si::metre), tens3>;
static_assert(QuantityPointOf<tensor_point, tensor_>);
static_assert(!QuantityPointOf<tensor_point, vector_>);
static_assert(!QuantityPointOf<tensor_point, scalar_>);

// ---- what the named interfaces gate ----
template<typename Q>
concept has_magnitude = requires(Q q) { q.magnitude(); };

// A magnitude belongs to a vector or a tensor, never to a definite scalar ...
static_assert(!has_magnitude<quantity<isq::speed[m / s], double>>);
static_assert(has_magnitude<quantity<isq::velocity[m / s], double>>);
static_assert(has_magnitude<quantity<isq::velocity[m / s], vec3>>);
static_assert(has_magnitude<quantity<isq::stress[si::pascal], tens3>>);
// ... and a vector-backed kind quantity has one again, which is what the fabricated `scalar`
// character used to deny
static_assert(has_magnitude<quantity<m / s, vec3>>);
static_assert(has_magnitude<quantity<m / s, tens3>>);
static_assert(has_magnitude<quantity<m / s, double>>);

// ---- the authoring-tier question, asked of a specification alone ----
// For an author who holds a specification and no quantity, `QuantitySpecOf` takes a character too. A
// concrete specification answers about its own single character on each axis.
template<auto QS, auto V>
concept spec_of = QuantitySpecOf<decltype(QS), V>;

static_assert(spec_of<isq::speed, quantity_tensor_order::scalar>);
static_assert(!spec_of<isq::speed, quantity_tensor_order::vector>);
static_assert(spec_of<isq::velocity, quantity_tensor_order::vector>);
static_assert(!spec_of<isq::velocity, quantity_tensor_order::scalar>);
static_assert(spec_of<isq::stress, quantity_tensor_order::tensor>);
static_assert(spec_of<isq::speed, quantity_field::real>);
static_assert(!spec_of<isq::speed, quantity_field::complex>);

// A kind admits every character, because a quantity of that kind may hold any of them.
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::scalar>);
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::vector>);
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::tensor>);
static_assert(spec_of<get_kind(isq::speed), quantity_field::complex>);

// A whole character constrains both axes at once.
static_assert(spec_of<isq::velocity, quantity_character{quantity_tensor_order::vector}>);
static_assert(!spec_of<isq::velocity, quantity_character{quantity_tensor_order::vector, quantity_field::complex}>);

// A specification argument means convertibility, not a shared character. `isq::velocity` and
// `isq::position_vector` are both vectors yet unrelated in the tree, while `isq::speed` is a
// `velocity`'s parent. Reading either the other way round is what the disjoint guards prevent.
static_assert(!spec_of<isq::velocity, isq::position_vector>);
static_assert(spec_of<isq::velocity, isq::speed>);

// ---- the deprecated character member reports only definite characters ----
#if !MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(quantity<isq::velocity[m / s], vec3>::character.order == quantity_tensor_order::vector);
static_assert(quantity<isq::speed[m / s], double>::character.order == quantity_tensor_order::scalar);
MP_UNITS_DIAGNOSTIC_POP
#endif

}  // namespace
