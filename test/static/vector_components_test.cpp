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
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/cartesian_vector.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <complex>
#include <type_traits>
#include <vector>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

namespace {

using vec3 = utility::cartesian_vector<double>;
using vec3i = utility::cartesian_vector<int>;

// the aviation hierarchy: a 3D whole plus 1D-vector component axes (each its own kind)
QUANTITY_SPEC(flight_velocity, isq::velocity);
QUANTITY_SPEC(forward_velocity, isq::velocity, is_kind);
QUANTITY_SPEC(wind_drift, isq::velocity, is_kind);
QUANTITY_SPEC(sink_rate, isq::velocity, is_kind);

}  // namespace

template<>
struct mp_units::vector_components<flight_velocity> : mp_units::vector_axes<forward_velocity, wind_drift, sink_rate> {};

namespace {

using Q = quantity<flight_velocity[km / h], vec3>;
inline constexpr Q v = flight_velocity(vec3{30.0, -4.0, -2.5} * km / h);

// ---- the whole is decomposable ----
static_assert(detail::Decomposable<flight_velocity, vec3>);

// ---- get<Idx> and get<QS> return 1D-vector quantities of the right axis spec ----
static_assert(QuantityOf<decltype(get<0>(v)), forward_velocity>);
static_assert(QuantityOf<decltype(get<2>(v)), sink_rate>);
// a component keeps the vector character of the whole
static_assert(QuantityOf<decltype(get<0>(v)), quantity_tensor_order::vector>);
static_assert(detail::admits_only(detail::admitted_character(decltype(get<0>(v))::quantity_spec).order,
                                  quantity_tensor_order::vector));
static_assert(std::is_same_v<decltype(get<0>(v))::rep, double>);  // component rep is the vector's value_type
static_assert(std::is_same_v<decltype(get<2>(v)), decltype(get<sink_rate>(v))>);

// ---- values ----
static_assert(get<0>(v).numerical_value_in(km / h) == 30.0);
static_assert(get<1>(v).numerical_value_in(km / h) == -4.0);
static_assert(get<sink_rate>(v).numerical_value_in(km / h) == -2.5);

// ---- tuple-like protocol (structured bindings) ----
static_assert(std::tuple_size_v<Q> == 3);
static_assert(std::is_same_v<std::tuple_element_t<2, Q>, decltype(get<2>(v))>);

// ---- arithmetic: collinear (same axis) allowed, orthogonal (cross axis) rejected ----
template<typename A, typename B>
concept addable = requires(A a, B b) { a + b; };
static_assert(addable<decltype(get<0>(v)), decltype(get<0>(v))>);
static_assert(!addable<decltype(get<0>(v)), decltype(get<1>(v))>);

// ---- get is constrained: index in range, spec must be one of the declared axes ----
static_assert(detail::DecomposableIndex<flight_velocity, vec3, 2>);                  // in range
static_assert(!detail::DecomposableIndex<flight_velocity, vec3, 3>);                 // index past the last axis
static_assert(detail::DecomposableAxis<flight_velocity, vec3, sink_rate>);           // a declared axis
static_assert(!detail::DecomposableAxis<flight_velocity, vec3, isq::acceleration>);  // not an axis

// ---- negative cases ----
QUANTITY_SPEC(w_foreign, isq::velocity);
QUANTITY_SPEC(w_too_many, isq::velocity);
QUANTITY_SPEC(w_axis_eq_whole, isq::velocity);
QUANTITY_SPEC(w_no_base, isq::velocity);
QUANTITY_SPEC(px, isq::velocity);  // plain children (not is_kind) -> share the velocity kind
QUANTITY_SPEC(py, isq::velocity);
QUANTITY_SPEC(pz, isq::velocity);
QUANTITY_SPEC(thrust, isq::force, is_kind);  // forces: a different hierarchy root than velocity
QUANTITY_SPEC(drag, isq::force, is_kind);
QUANTITY_SPEC(lift, isq::force, is_kind);
QUANTITY_SPEC(a1, isq::velocity, is_kind);
QUANTITY_SPEC(a2, isq::velocity, is_kind);
QUANTITY_SPEC(a3, isq::velocity, is_kind);
QUANTITY_SPEC(a4, isq::velocity, is_kind);

// axis-intrinsic violations are rejected by `vector_axes` itself (such a specialization is
// ill-formed at the point of definition); tested here through the validity concept
static_assert(detail::ValidVectorAxes<forward_velocity, wind_drift, sink_rate>);   // the valid set
static_assert(!detail::ValidVectorAxes<forward_velocity>);                         // fewer than two axes
static_assert(!detail::ValidVectorAxes<forward_velocity, isq::speed, sink_rate>);  // a scalar axis
static_assert(!detail::ValidVectorAxes<forward_velocity, thrust, sink_rate>);      // mixed hierarchy roots
static_assert(!detail::ValidVectorAxes<px, py, pz>);                               // not distinct kinds

}  // namespace

// these axis lists ARE intrinsically valid, so the specializations compile; they fail relationally
template<>
struct mp_units::vector_components<w_foreign> : mp_units::vector_axes<thrust, drag, lift> {};
template<>
struct mp_units::vector_components<w_too_many> : mp_units::vector_axes<a1, a2, a3, a4> {};
template<>
struct mp_units::vector_components<w_axis_eq_whole> : mp_units::vector_axes<forward_velocity, wind_drift, pz> {};
template<>
struct mp_units::vector_components<w_no_base> {  // intentionally does NOT inherit from vector_axes
  static constexpr std::size_t size = 3;
};

namespace {

// negative tests as a custom concept asserted via static_assert (the `invalid_types` idiom)
template<typename = void>
concept invalid_decompositions = requires {
  requires !detail::Decomposable<w_foreign, vec3>;          // axes valid, but foreign to the whole's root
  requires !detail::Decomposable<w_axis_eq_whole, vec3>;    // an axis shares the whole's kind
  requires !detail::Decomposable<w_too_many, vec3>;         // more axes than the representation holds
  requires !detail::Decomposable<w_no_base, vec3>;          // vector_components does not derive vector_axes
  requires !detail::Decomposable<isq::speed, double>;       // a scalar whole and a scalar representation
  requires !detail::Decomposable<flight_velocity, double>;  // a non-indexable (scalar) representation
};
static_assert(invalid_decompositions<>);

}  // namespace

// ---- complex-field vector decomposition (the field pin was removed: any field, order must stay vector) ----
namespace {

QUANTITY_SPEC(cplx_flight_velocity, isq::velocity, quantity_field::complex);
QUANTITY_SPEC(cplx_forward, isq::velocity, is_kind, quantity_field::complex);
QUANTITY_SPEC(cplx_drift, isq::velocity, is_kind, quantity_field::complex);
QUANTITY_SPEC(cplx_sink, isq::velocity, is_kind, quantity_field::complex);
QUANTITY_SPEC(cplx_speed, isq::speed, quantity_field::complex);  // a complex *scalar*

}  // namespace

template<>
struct mp_units::vector_components<cplx_flight_velocity> :
    mp_units::vector_axes<cplx_forward, cplx_drift, cplx_sink> {};

namespace {

using cvec3 = utility::cartesian_vector<std::complex<double>>;
using CQ = quantity<cplx_flight_velocity[km / h], cvec3>;
inline constexpr CQ cv =
  cplx_flight_velocity(cvec3{std::complex{30.0, 1.0}, std::complex{-4.0, 0.0}, std::complex{-2.5, 2.0}} * (km / h));

// a complex vector whole is decomposable, and complex vector axes are valid (field no longer pinned to real)
static_assert(detail::Decomposable<cplx_flight_velocity, cvec3>);
static_assert(detail::ValidVectorAxes<cplx_forward, cplx_drift, cplx_sink>);

// order is still enforced regardless of field: a complex *scalar* axis is rejected
static_assert(!detail::ValidVectorAxes<cplx_forward, cplx_speed, cplx_sink>);

// get<Idx> yields the right complex 1D-vector axis quantity
static_assert(QuantityOf<decltype(get<0>(cv)), cplx_forward>);
static_assert(get<0>(cv).numerical_value_in(km / h) == std::complex{30.0, 1.0});
static_assert(get<2>(cv).numerical_value_in(km / h) == std::complex{-2.5, 2.0});

}  // namespace

// ---- unnamed decomposition: a vector quantity that declares no axes ----
namespace {

using U = quantity<isq::velocity[km / h], vec3>;
inline constexpr U uv = isq::velocity(vec3{30.0, -4.0, -2.5} * km / h);

static_assert(detail::UnnamedDecomposable<isq::velocity, vec3>);
static_assert(!detail::NamedDecomposable<isq::velocity, vec3>);

// a component carries the whole's *kind*, not the whole's own spec: it belongs to that kind without
// being a complete `isq::velocity`. The kind admits every character, so the vector character comes
// from the representation and survives. The rep is the vector's value_type.
static_assert(decltype(get<0>(uv))::quantity_spec == get_kind(isq::velocity));
static_assert(QuantityOf<decltype(get<0>(uv)), isq::velocity>);
static_assert(std::is_same_v<decltype(get<0>(uv))::rep, double>);
static_assert(get<0>(uv).numerical_value_in(km / h) == 30.0);
static_assert(get<2>(uv).numerical_value_in(km / h) == -2.5);

// the component count comes from the representation, which bounds the index and drives the tuple
// protocol (and therefore structured bindings)
static_assert(std::tuple_size_v<U> == 3);
static_assert(std::is_same_v<std::tuple_element_t<2, U>, decltype(get<2>(uv))>);
static_assert(detail::DecomposableIndex<isq::velocity, vec3, 2>);
static_assert(!detail::DecomposableIndex<isq::velocity, vec3, 3>);

// a quantity kind is character-agnostic, so a unit-only vector quantity decomposes as well
inline constexpr quantity kv = vec3{1.0, 2.0, 3.0} * m;
static_assert(std::tuple_size_v<decltype(kv)> == 3);
static_assert(get<2>(kv).numerical_value_in(m) == 3.0);

// unnamed components share a single kind, so cross-component arithmetic is *not* rejected here:
// that safety is exactly what declaring the axes buys
static_assert(addable<decltype(get<0>(uv)), decltype(get<1>(uv))>);

template<typename = void>
concept invalid_unnamed_accesses = requires {
  // lookup by axis spec is a named-mode privilege
  requires !detail::DecomposableAxis<isq::velocity, vec3, isq::velocity>;
  // a representation with no compile-time length cannot bound a compile-time index
  requires !detail::Decomposable<isq::velocity, std::vector<double>>;
};
static_assert(invalid_unnamed_accesses<>);

}  // namespace

// ---- component-wise construction, the inverse of decomposition and graded the same way ----
namespace {

inline constexpr Q composed = {30.0 * forward_velocity[km / h], -4.0 * wind_drift[km / h], -2.5 * sink_rate[km / h]};
static_assert(composed == v);

// the constructed type pins the unit, so each argument is converted into it
inline constexpr Q converted = {30.0 * forward_velocity[km / h], 10.0 * wind_drift[m / s], -2.5 * sink_rate[km / h]};
static_assert(get<wind_drift>(converted).numerical_value_in(km / h) == 36.0);

using fwd_q = quantity<forward_velocity[km / h]>;
using drift_q = quantity<wind_drift[km / h]>;
using sink_q = quantity<sink_rate[km / h]>;

template<typename... Qs>
concept composable = requires(Qs... qs) { Q{qs...}; };

template<typename... Qs>
concept composable_undeclared = requires(Qs... qs) { U{qs...}; };

template<typename... Qs>
concept composable_integral = requires(Qs... qs) { quantity<flight_velocity[km / h], vec3i>{qs...}; };

template<typename = void>
concept invalid_compositions = requires {
  requires composable<fwd_q, drift_q, sink_q>;                                // the valid call
  requires !composable<drift_q, fwd_q, sink_q>;                               // two axes swapped
  requires !composable<fwd_q, drift_q>;                                       // fewer arguments than axes
  requires !composable<fwd_q, drift_q, sink_q, sink_q>;                       // more arguments than axes
  requires !composable<fwd_q, drift_q, quantity<isq::velocity[km / h]>>;      // a bare velocity is not an axis
  requires !composable<fwd_q, drift_q, quantity<isq::acceleration[m / s2]>>;  // an unrelated kind
  requires !composable<double, double, double>;                               // plain numbers name no axis
  // a quantity kind converts into every quantity of its hierarchy, so it fits every position and
  // forfeits the order check: reaching for one is the usual way to opt out of kind safety, and this
  // constructor does not second-guess it
  requires composable<quantity<km / h>, quantity<km / h>, quantity<km / h>>;
  requires composable<fwd_q, drift_q, quantity<km / h>>;
  // a unit conversion per argument is fine as long as it is value-preserving
  requires composable<fwd_q, quantity<wind_drift[m / s]>, sink_q>;
  // and the ordinary truncation rules apply to each argument on its own: a floating-point component
  // does not narrow into an integral representation, and m/s does not scale into an integral km/h one
  requires composable_integral<quantity<forward_velocity[km / h], int>, quantity<wind_drift[km / h], int>,
                               quantity<sink_rate[km / h], int>>;
  requires !composable_integral<fwd_q, drift_q, sink_q>;
  requires !composable_integral<quantity<forward_velocity[km / h], int>, quantity<wind_drift[m / s], int>,
                                quantity<sink_rate[km / h], int>>;
};
static_assert(invalid_compositions<>);

// an undeclared whole composes too, from quantities of its own spec. There are no positions to tell
// apart there, so order is unchecked and a quantity kind is accepted, but the count still is and each
// argument is still converted into the unit the type pins.
inline constexpr U undeclared_composed = {30.0 * isq::velocity[km / h], 10.0 * isq::velocity[m / s],
                                          -2.5 * isq::velocity[km / h]};
static_assert(get<1>(undeclared_composed).numerical_value_in(km / h) == 36.0);

// so both round trips close: what `get` takes apart goes back together
static_assert(U{get<0>(uv), get<1>(uv), get<2>(uv)} == uv);
static_assert(Q{get<0>(v), get<1>(v), get<2>(v)} == v);

using vel_q = quantity<isq::velocity[km / h]>;

template<typename = void>
concept undeclared_compositions = requires {
  requires composable_undeclared<vel_q, vel_q, vel_q>;
  requires composable_undeclared<quantity<km / h>, quantity<km / h>, quantity<km / h>>;
  requires !composable_undeclared<vel_q, vel_q>;  // the representation holds three
  requires !composable_undeclared<vel_q, vel_q, vel_q, vel_q>;
  requires !composable_undeclared<fwd_q, drift_q, sink_q>;  // an axis is not a velocity of this kind
};
static_assert(undeclared_compositions<>);

}  // namespace
