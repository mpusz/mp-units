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

QUANTITY_SPEC(kind_equation_speed, kind_of<isq::length> / kind_of<isq::time>);

// ---- what a specification admits ----
// a concrete specification admits exactly one character per axis
static_assert(detail::admits_only(detail::admitted_character(isq::speed).order, quantity_tensor_order::scalar));
static_assert(detail::admits_only(detail::admitted_character(isq::velocity).order, quantity_tensor_order::vector));
static_assert(detail::admits_only(detail::admitted_character(isq::stress).order, quantity_tensor_order::tensor));
static_assert(detail::admits_only(detail::admitted_character(isq::speed).field, quantity_field::real));
static_assert(detail::is_definite(detail::admitted_character(isq::velocity).order));

// a quantity kind constrains neither axis (`cartesian_tensor{} * m / s` is a speed-kind quantity holding a tensor)
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::scalar));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::vector));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).order, quantity_tensor_order::tensor));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).field, quantity_field::real));
static_assert(detail::admits(detail::admitted_character(get_kind(isq::velocity)).field, quantity_field::complex));
static_assert(!detail::is_definite(detail::admitted_character(get_kind(isq::velocity)).order));

// `admits` and `admits_only` differ for a quantity kind, for example: it may be a vector but isn't definitely one
static_assert(!detail::admits_only(detail::admitted_character(get_kind(isq::velocity)).order,
                                   quantity_tensor_order::vector));

// ---- derived quantities ----
// the highest order among the factors wins
static_assert(detail::admits_only(detail::admitted_character(isq::velocity / isq::duration).order,
                                  quantity_tensor_order::vector));
static_assert(detail::admits_only(detail::admitted_character(isq::length / isq::duration).order,
                                  quantity_tensor_order::scalar));
// a product or quotient of kinds is itself a kind (`clone_kind_of`), so it constrains nothing
static_assert(!detail::is_definite(detail::admitted_character(get_kind(isq::length) / get_kind(isq::time)).order));
// a kind factor of a mixed equation constrains nothing
static_assert(!detail::is_definite(detail::admitted_character(isq::speed * get_kind(isq::time)).order));
// a specification defined by a kind equation constrains nothing, and neither does a product with it
static_assert(!detail::is_definite(detail::admitted_character(kind_equation_speed).order));
static_assert(!detail::is_definite(detail::admitted_character(kind_equation_speed * isq::time).order));
static_assert(!detail::is_definite(detail::admitted_character(kind_equation_speed * isq::time).field));
static_assert(RepresentationOf<double, kind_equation_speed * isq::time>);

// ---- non_negative precondition ----
static_assert(detail::is_real_scalar(detail::admitted_character(isq::speed)));
static_assert(!detail::is_real_scalar(detail::admitted_character(isq::velocity)));
static_assert(!detail::is_real_scalar(detail::admitted_character(get_kind(isq::speed))));

// ---- definite_character ----
// for a kind it is a hard error with a message, which can't be asserted here (callers are the deprecated
// `character` members)
static_assert(detail::definite_character<detail::admitted_character(isq::velocity)>().order ==
              quantity_tensor_order::vector);
static_assert(detail::definite_character<detail::admitted_character(isq::stress)>().order ==
              quantity_tensor_order::tensor);

// ---- RepresentationOf ----
// a scalar fills every order, a vector fills vector and tensor, a tensor only tensor (a library rule
// generalizing clause 18 of ISO 80000-2:2019)
static_assert(RepresentationOf<double, isq::speed>);
static_assert(RepresentationOf<double, isq::velocity>);
static_assert(RepresentationOf<double, isq::stress>);
static_assert(RepresentationOf<vec3, isq::velocity>);
static_assert(RepresentationOf<vec3, isq::stress>);
static_assert(RepresentationOf<tens3, isq::stress>);
static_assert(!RepresentationOf<vec3, isq::speed>);
static_assert(!RepresentationOf<tens3, isq::speed>);
static_assert(!RepresentationOf<tens3, isq::velocity>);

// a kind takes any of them
static_assert(RepresentationOf<double, get_kind(isq::speed)>);
static_assert(RepresentationOf<vec3, get_kind(isq::speed)>);
static_assert(RepresentationOf<tens3, get_kind(isq::speed)>);

// an argument that isn't a character at all answers `false`
static_assert(!RepresentationOf<double, 42>);
static_assert(!RepresentationOf<double, si::metre>);

// a mismatch answers `false` inside a requires-expression too
template<typename = void>
concept representation_of_is_answerable = requires {
  requires !RepresentationOf<int*, isq::velocity>;
  requires !RepresentationOf<vec3, isq::speed>;
};
static_assert(representation_of_is_answerable<>);

// ---- QuantityOf with a character ----
// both the specification and the representation have to admit the character
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

// a kind quantity is whatever its representation makes it
static_assert(QuantityOf<quantity<m / s, vec3>, vector_>);
static_assert(QuantityOf<quantity<m / s, tens3>, tensor_>);
static_assert(!QuantityOf<quantity<m / s, tens3>, vector_>);  // a tensor representation is not a vector
static_assert(QuantityOf<quantity<m / s, double>, scalar_>);
static_assert(QuantityOf<quantity<m / s, double>, vector_>);  // a `double` may hold a 1D vector

// a specification argument keeps its quantity-tree meaning
static_assert(QuantityOf<quantity<isq::velocity[m / s], vec3>, isq::velocity>);
static_assert(QuantityOf<quantity<isq::velocity[m / s], vec3>, isq::speed>);  // a velocity is a speed
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, isq::velocity>);

#if MP_UNITS_HOSTED
static_assert(QuantityOf<quantity<isq::speed[m / s], double>, quantity_field::real>);
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, quantity_field::complex>);
static_assert(QuantityOf<quantity<m / s, std::complex<double>>, quantity_field::complex>);
static_assert(!QuantityOf<quantity<m / s, std::complex<double>>, quantity_field::real>);
#endif

// ---- QuantityPointOf with a character ----
// it agrees with `QuantityOf` for a quantity of the same reference and representation
using vel_point = quantity_point<isq::velocity[m / s], default_point_origin(isq::velocity[m / s]), double>;
static_assert(QuantityPointOf<vel_point, vector_>);
static_assert(!QuantityPointOf<vel_point, tensor_>);
static_assert(QuantityPointOf<vel_point, quantity_field::real>);

// a speed point is never a vector one
using speed_point = quantity_point<isq::speed[m / s], default_point_origin(isq::speed[m / s]), double>;
static_assert(QuantityPointOf<speed_point, scalar_>);
static_assert(!QuantityPointOf<speed_point, vector_>);

// only the representation narrows a kind point's character
using complex_point = quantity_point<si::metre, default_point_origin(si::metre), std::complex<double>>;
static_assert(QuantityPointOf<complex_point, quantity_field::complex>);
static_assert(!QuantityPointOf<complex_point, quantity_field::real>);
using tensor_point = quantity_point<si::metre, default_point_origin(si::metre), tens3>;
static_assert(QuantityPointOf<tensor_point, tensor_>);
static_assert(!QuantityPointOf<tensor_point, vector_>);
static_assert(!QuantityPointOf<tensor_point, scalar_>);

// ---- magnitude() ----
template<typename Q>
concept has_magnitude = requires(Q q) { q.magnitude(); };

// a definite scalar has no magnitude
static_assert(!has_magnitude<quantity<isq::speed[m / s], double>>);
static_assert(has_magnitude<quantity<isq::velocity[m / s], double>>);
static_assert(has_magnitude<quantity<isq::velocity[m / s], vec3>>);
static_assert(has_magnitude<quantity<isq::stress[si::pascal], tens3>>);
// a kind quantity has one whatever its representation, if that can produce a norm
static_assert(has_magnitude<quantity<m / s, vec3>>);
static_assert(has_magnitude<quantity<m / s, tens3>>);
static_assert(has_magnitude<quantity<m / s, double>>);

// ---- QuantitySpecOf with a character ----
// a concrete specification has one character on each axis
template<auto QS, auto V>
concept spec_of = QuantitySpecOf<decltype(QS), V>;

static_assert(spec_of<isq::speed, quantity_tensor_order::scalar>);
static_assert(!spec_of<isq::speed, quantity_tensor_order::vector>);
static_assert(spec_of<isq::velocity, quantity_tensor_order::vector>);
static_assert(!spec_of<isq::velocity, quantity_tensor_order::scalar>);
static_assert(spec_of<isq::stress, quantity_tensor_order::tensor>);
static_assert(spec_of<isq::speed, quantity_field::real>);
static_assert(!spec_of<isq::speed, quantity_field::complex>);

// a kind admits every character
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::scalar>);
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::vector>);
static_assert(spec_of<get_kind(isq::speed), quantity_tensor_order::tensor>);
static_assert(spec_of<get_kind(isq::speed), quantity_field::complex>);

// a full `quantity_character` constrains both axes
static_assert(spec_of<isq::velocity, quantity_character{quantity_tensor_order::vector}>);
static_assert(!spec_of<isq::velocity, quantity_character{quantity_tensor_order::vector, quantity_field::complex}>);

// a specification argument asks about the quantity tree: `isq::velocity` and `isq::position_vector`
// are both vectors but unrelated, while `isq::speed` is the parent of `isq::velocity`
static_assert(!spec_of<isq::velocity, isq::position_vector>);
static_assert(spec_of<isq::velocity, isq::speed>);

// ---- deprecated character member ----
#if !MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
static_assert(quantity<isq::velocity[m / s], vec3>::character.order == quantity_tensor_order::vector);
static_assert(quantity<isq::speed[m / s], double>::character.order == quantity_tensor_order::scalar);
MP_UNITS_DIAGNOSTIC_POP
#endif

}  // namespace
