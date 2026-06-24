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

// Exercises a third-party linear algebra library used as an mp-units `quantity` representation
// type. The backend is selected automatically from whichever library is available on the include
// path; the build system compiles this file once per library it finds (one executable each).

// 1. The third-party library headers are always included textually: the libraries are not
//    modularized, and we need their vector type directly. A backend is selected here. mp-units'
//    own `cartesian_vector` is the built-in baseline: it ships with the library, needs no
//    third-party dependency and no integration plugin, and is used when forced
//    (`MP_UNITS_LA_USE_CARTESIAN`) or when no linear algebra library is found.
#if defined(MP_UNITS_LA_USE_CARTESIAN)
#define MP_UNITS_LA_CARTESIAN
#elif __has_include(<Eigen/Core>)
#include <Eigen/Core>
#include <Eigen/Geometry>  // cross()
#define MP_UNITS_LA_EIGEN
#elif __has_include(<glm/vec3.hpp>)
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#define MP_UNITS_LA_GLM
#elif __has_include(<blaze/math/StaticVector.h>)
#include <blaze/math/StaticVector.h>
#define MP_UNITS_LA_BLAZE
#else
#define MP_UNITS_LA_CARTESIAN
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// 2. When the build is configured with `import std;` the mp-units headers below consume the
//    standard library as a module, so this TU must import it too (it is header-mode otherwise).
//    All textual includes above stay before the import, which is the order libstdc++ requires.
#ifdef MP_UNITS_IMPORT_STD
import std;
#endif

// 3. mp-units and the matching integration adapter. The test is header-mode only (module-mode
//    consumption of the integration is demonstrated by the `linear_algebra` example).
#include <mp-units/math.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#if defined(MP_UNITS_LA_EIGEN)
#include <mp-units/integrations/eigen.h>
#elif defined(MP_UNITS_LA_GLM)
#include <mp-units/integrations/glm.h>
#elif defined(MP_UNITS_LA_BLAZE)
#include <mp-units/integrations/blaze.h>
#elif defined(MP_UNITS_LA_CARTESIAN)
#include <mp-units/cartesian_vector.h>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

#if defined(MP_UNITS_LA_EIGEN)
inline constexpr const char* backend = "Eigen";
using vec3 = Eigen::Vector3d;
#elif defined(MP_UNITS_LA_GLM)
inline constexpr const char* backend = "GLM";
using vec3 = glm::dvec3;
#elif defined(MP_UNITS_LA_BLAZE)
inline constexpr const char* backend = "Blaze";
using vec3 = blaze::StaticVector<double, 3>;
#elif defined(MP_UNITS_LA_CARTESIAN)
inline constexpr const char* backend = "cartesian_vector (built-in)";
using vec3 = cartesian_vector<double>;
#endif

[[nodiscard]] vec3 make_vec3(double x, double y, double z) { return {x, y, z}; }

// Only the built-in `cartesian_vector` backend offers `constexpr` arithmetic; the third-party
// cross products are runtime-only (e.g. `blaze::cross` materializes via a non-constexpr ctor), so
// marking their helper `constexpr` would be an ill-formed never-constant-expression function.
#if defined(MP_UNITS_LA_CARTESIAN)
#define MP_UNITS_LA_CONSTEXPR constexpr
#else
#define MP_UNITS_LA_CONSTEXPR
#endif

// Euclidean cross product on the bare vector type. Every backend spells it differently, so the
// difference is confined to this one helper; the quantity-level overload below is backend-agnostic.
[[nodiscard]] MP_UNITS_LA_CONSTEXPR vec3 cross(const vec3& lhs, const vec3& rhs)
{
#if defined(MP_UNITS_LA_EIGEN)
  return lhs.cross(rhs);
#elif defined(MP_UNITS_LA_GLM)
  return glm::cross(lhs, rhs);
#elif defined(MP_UNITS_LA_BLAZE)
  return blaze::cross(lhs, rhs);
#elif defined(MP_UNITS_LA_CARTESIAN)
  return vector_product(lhs, rhs);
#endif
}

// Cross product of two vector quantities: cross the numerical values, then combine the references.
template<Quantity Q1, Quantity Q2>
[[nodiscard]] MP_UNITS_LA_CONSTEXPR QuantityOf<Q1::quantity_spec * Q2::quantity_spec> auto cross(const Q1& q1,
                                                                                                 const Q2& q2)
{
  return cross(q1.numerical_value_in(q1.unit), q2.numerical_value_in(q2.unit)) * (Q1::reference * Q2::reference);
}

// --- compile-time guarantees -------------------------------------------------------------------

// The library vector type is accepted as a vector representation.
static_assert(RepresentationOf<vec3, quantity_tensor_order::vector>);

// A library scalar multiplication may yield a lazy expression template; the representation
// machinery must canonicalize it back to the concrete vector type rather than store the proxy
// (which would dangle). `decltype(vec3{} * 2.0)` is exactly such a proxy for Eigen/Blaze.
static_assert(std::same_as<representation_canonical_type_t<decltype(std::declval<vec3>() * 2.0)>, vec3>);

// Consequently, arithmetic on vector quantities stores the concrete vector type, not a proxy.
static_assert(std::same_as<decltype(make_vec3(1, 2, 3) * isq::velocity[m / s] * (2. * isq::duration[s]))::rep, vec3>);

// A vector quantity models the `Vector` concept (so `magnitude(q)` works on it directly) ...
static_assert(detail::Vector<decltype(make_vec3(0, 0, 0) * isq::velocity[m / s])>);
// ... but is NOT a representation type: a quantity can never be nested as another quantity's
// representation (`value_type_t<quantity>` is the quantity itself, which `NotQuantity` rejects).
static_assert(!detail::VectorRepresentation<decltype(make_vec3(0, 0, 0) * isq::velocity[m / s])>);
static_assert(!RepresentationOf<decltype(make_vec3(0, 0, 0) * isq::velocity[m / s]), quantity_tensor_order::vector>);
// `magnitude()` of a vector quantity is a scalar quantity in the same unit.
static_assert(QuantityOf<decltype(magnitude(make_vec3(3, 4, 0) * isq::velocity[m / s])), isq::speed>);

}  // namespace

TEST_CASE("linear algebra type as a quantity representation")
{
  INFO("backend: " << backend);

  SECTION("construction preserves the numerical value")
  {
    const quantity v = make_vec3(1, 2, 3) * isq::displacement[m];
    const auto& nv = v.numerical_value_in(m);
    CHECK(nv[0] == 1);
    CHECK(nv[1] == 2);
    CHECK(nv[2] == 3);
  }

  SECTION("exact unit conversion (km -> m) of a vector quantity")
  {
    const quantity v = make_vec3(3, 2, 1) * isq::displacement[km];
    const auto& nv = v.numerical_value_in(m);
    CHECK(nv[0] == 3000);
    CHECK(nv[1] == 2000);
    CHECK(nv[2] == 1000);
  }

  SECTION("multiplication by a scalar number, on either side")
  {
    const quantity v = make_vec3(1, 2, 3) * isq::displacement[m];
    const auto& left = (2. * v).numerical_value_in(m);
    const auto& right = (v * 2.).numerical_value_in(m);
    CHECK(left[0] == 2);
    CHECK(left[1] == 4);
    CHECK(left[2] == 6);
    CHECK(right[0] == 2);
    CHECK(right[1] == 4);
    CHECK(right[2] == 6);
  }

  SECTION("division by a scalar number")
  {
    const quantity v = make_vec3(2, 4, 6) * isq::displacement[m];
    const auto& nv = (v / 2.).numerical_value_in(m);
    CHECK(nv[0] == 1);
    CHECK(nv[1] == 2);
    CHECK(nv[2] == 3);
  }

  SECTION("unary negation")
  {
    const quantity v = make_vec3(1, -2, 3) * isq::displacement[m];
    const auto& nv = (-v).numerical_value_in(m);
    CHECK(nv[0] == -1);
    CHECK(nv[1] == 2);
    CHECK(nv[2] == -3);
  }

  SECTION("vector quantity times scalar quantity yields a vector quantity")
  {
    const quantity velocity = make_vec3(30, 40, 0) * isq::velocity[km / h];
    const quantity displacement = velocity * (2. * isq::duration[h]);
    const auto& nv = displacement.in(km).numerical_value_in(km);
    CHECK(nv[0] == 60);
    CHECK(nv[1] == 80);
    CHECK(nv[2] == 0);
  }

  SECTION("scalar quantity multiplication is symmetric and combines the quantity specifications")
  {
    const quantity v = make_vec3(1, 2, 3) * isq::velocity[m / s];
    const quantity mass = 2. * isq::mass[kg];

    // the result is a derived quantity (mass * velocity), with the same value on either side
    const auto& lhs = (mass * v).numerical_value_in(kg * m / s);
    const auto& rhs = (v * mass).numerical_value_in(kg * m / s);
    CHECK(lhs[0] == 2);
    CHECK(lhs[1] == 4);
    CHECK(lhs[2] == 6);
    CHECK(rhs[0] == 2);
    CHECK(rhs[1] == 4);
    CHECK(rhs[2] == 6);

    // the derived quantity can be cast to the named quantity it represents
    const quantity<isq::momentum[N * s], vec3> momentum = quantity_cast<isq::momentum>(mass * v);
    const auto& nv = momentum.numerical_value_in(N * s);
    CHECK(nv[0] == 2);
    CHECK(nv[1] == 4);
    CHECK(nv[2] == 6);
  }

  SECTION("vector quantity divided by a scalar quantity")
  {
    const quantity displacement = make_vec3(60, 80, 0) * isq::displacement[km];
    const quantity velocity = displacement / (2. * isq::duration[h]);
    const auto& nv = velocity.numerical_value_in(km / h);
    CHECK(nv[0] == 30);
    CHECK(nv[1] == 40);
    CHECK(nv[2] == 0);

    // the derived quantity (displacement / duration) can be cast to the named velocity quantity
    const quantity<isq::velocity[km / h], vec3> casted = quantity_cast<isq::velocity>(velocity);
    const auto& cnv = casted.numerical_value_in(km / h);
    CHECK(cnv[0] == 30);
    CHECK(cnv[1] == 40);
    CHECK(cnv[2] == 0);
  }

  SECTION("cross product of two vector quantities combines their quantity specifications")
  {
    const quantity position = make_vec3(3, 0, 0) * isq::position_vector[m];
    const quantity force = make_vec3(0, 10, 0) * isq::force[N];
    const quantity moment = cross(position, force);
    const auto& nv = moment.numerical_value_in(N * m);
    CHECK(nv[0] == 0);
    CHECK(nv[1] == 0);
    CHECK(nv[2] == 30);
    // r x F has the quantity specification of a moment of force
    static_assert(implicitly_convertible(decltype(moment)::quantity_spec, isq::moment_of_force));
  }

  SECTION("vector addition with an automatic unit conversion of one operand")
  {
    const quantity lhs = make_vec3(1, 2, 3) * isq::displacement[km];
    const quantity rhs = make_vec3(500, 0, 0) * isq::displacement[m];
    const auto& nv = (lhs + rhs).numerical_value_in(m);
    CHECK(nv[0] == 1500);
    CHECK(nv[1] == 2000);
    CHECK(nv[2] == 3000);
  }

  SECTION("vector subtraction with an automatic unit conversion of one operand")
  {
    const quantity lhs = make_vec3(1, 2, 3) * isq::displacement[km];
    const quantity rhs = make_vec3(500, 0, 0) * isq::displacement[m];
    const auto& nv = (lhs - rhs).numerical_value_in(m);
    CHECK(nv[0] == 500);
    CHECK(nv[1] == 2000);
    CHECK(nv[2] == 3000);
  }

  SECTION("Euclidean magnitude as a scalar quantity (via the norm() the library provides)")
  {
    const quantity velocity = make_vec3(30, 40, 0) * isq::velocity[km / h];
    const quantity speed = magnitude(velocity);
    CHECK_THAT(speed.numerical_value_in(km / h), Catch::Matchers::WithinAbs(50.0, 1e-9));
  }

  SECTION("equality of vector quantities")
  {
    const quantity lhs = make_vec3(1, 2, 3) * isq::displacement[km];
    const quantity rhs = make_vec3(1000, 2000, 3000) * isq::displacement[m];
    CHECK(lhs == rhs);
    CHECK(lhs != make_vec3(1, 2, 4) * isq::displacement[km]);
  }
}

#if defined(MP_UNITS_LA_CARTESIAN)

// The built-in `cartesian_vector` supports integral representations and `constexpr` evaluation,
// which the floating-point, runtime-only third-party backends above cannot exercise. These checks
// are therefore specific to the built-in backend.

namespace {
using vec3i = cartesian_vector<int>;
[[nodiscard]] constexpr vec3i make_vec3i(int x, int y, int z) { return {x, y, z}; }
}  // namespace

TEST_CASE("built-in cartesian_vector with an integral representation")
{
  SECTION("exact (non-truncating) unit conversion preserves all components")
  {
    const quantity v = make_vec3i(3, 2, 1) * isq::displacement[km];
    CHECK(v.numerical_value_in(m) == vec3i{3000, 2000, 1000});
  }

  SECTION("truncating unit conversion requires the forcing interface")
  {
    const quantity v = make_vec3i(1500, 1500, 1500) * isq::displacement[m];
    CHECK(v.force_numerical_value_in(km) == vec3i{1, 1, 1});
  }

  SECTION("integral multiplication and division by a scalar number")
  {
    const quantity v = make_vec3i(1, 2, 3) * isq::displacement[m];
    CHECK((2 * v).numerical_value_in(m) == vec3i{2, 4, 6});
    CHECK((v * 2).numerical_value_in(m) == vec3i{2, 4, 6});
    CHECK((v / 2).numerical_value_in(m) == vec3i{0, 1, 1});
  }

  SECTION("integral addition and subtraction")
  {
    const quantity lhs = make_vec3i(1, 2, 3) * isq::displacement[m];
    const quantity rhs = make_vec3i(3, 2, 1) * isq::displacement[m];
    CHECK((lhs + rhs).numerical_value_in(m) == vec3i{4, 4, 4});
    CHECK((lhs - rhs).numerical_value_in(m) == vec3i{-2, 0, 2});
  }
}

// the whole pipeline (construction, scaling, arithmetic) is usable in a constant expression
static_assert((make_vec3i(1, 2, 3) * isq::displacement[m] + make_vec3i(3, 2, 1) * isq::displacement[m])
                .numerical_value_in(m) == vec3i{4, 4, 4});
static_assert(cross(make_vec3i(3, 0, 0) * isq::position_vector[m], make_vec3i(0, 10, 0) * isq::force[N])
                .numerical_value_in(N * m) == vec3i{0, 0, 30});

#endif  // MP_UNITS_LA_CARTESIAN
