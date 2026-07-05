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

// Exercises the `polar_vector`/`spherical_vector` conversion facades with a third-party linear
// algebra vector as the Cartesian representation. The facades are not tied to `cartesian_vector`:
// they read a vector representation's dimension via `std::tuple_size` and its components via `get`,
// both of which the integration adapter adds for the backend's fixed-size vector type. The backend
// is selected from whatever library is available (the build compiles this file once per backend).

#if __has_include(<Eigen/Core>)
#include <Eigen/Core>
#define MP_UNITS_PS_EIGEN
#elif __has_include(<blaze/math/StaticVector.h>)
#include <blaze/math/StaticVector.h>
#define MP_UNITS_PS_BLAZE
#endif

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <concepts>
#include <numbers>
#include <tuple>
#include <type_traits>
#endif

#include <mp-units/systems/si.h>
#include <mp-units/utility/polar_vector.h>
#include <mp-units/utility/spherical_vector.h>
#if defined(MP_UNITS_PS_EIGEN)
#include <mp-units/integrations/eigen.h>
#elif defined(MP_UNITS_PS_BLAZE)
#include <mp-units/integrations/blaze.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using Catch::Matchers::WithinAbs;

namespace {

#if defined(MP_UNITS_PS_EIGEN)
inline constexpr const char* backend = "Eigen";
using vec2 = Eigen::Vector2d;
using vec3 = Eigen::Vector3d;
#elif defined(MP_UNITS_PS_BLAZE)
inline constexpr const char* backend = "Blaze";
using vec2 = blaze::StaticVector<double, 2>;
using vec3 = blaze::StaticVector<double, 3>;
#endif

[[nodiscard]] vec2 make_vec2(double x, double y) { return {x, y}; }
[[nodiscard]] vec3 make_vec3(double x, double y, double z) { return {x, y, z}; }

// The adapter makes the backend's fixed-size vector a valid Cartesian source for the facades:
// dimension 2 feeds `polar_vector`, dimension 3 feeds `spherical_vector`, and never the reverse.
static_assert(std::constructible_from<utility::polar_vector<si::metre, si::radian, double>, quantity<si::metre, vec2>>);
static_assert(
  !std::constructible_from<utility::polar_vector<si::metre, si::radian, double>, quantity<si::metre, vec3>>);
static_assert(
  std::constructible_from<utility::spherical_vector<si::metre, si::radian, double>, quantity<si::metre, vec3>>);
static_assert(
  !std::constructible_from<utility::spherical_vector<si::metre, si::radian, double>, quantity<si::metre, vec2>>);

// The tuple protocol the adapter adds is genuine structured-bindings support, not just enough for
// the facades: dimension is visible through `std::tuple_size`, the element type through
// `std::tuple_element`.
static_assert(std::tuple_size_v<vec2> == 2);
static_assert(std::tuple_size_v<vec3> == 3);
static_assert(std::same_as<std::tuple_element_t<0, vec3>, double>);

// `to_cartesian<To>()` names the target rep, which must be a vector representation of the matching
// dimension. A detection concept verifies the mismatch is a clean SFINAE rejection (the member is
// simply not callable), not a hard error.
template<typename To>
concept polar_makes = requires(const utility::polar_vector<si::metre, si::radian, double>& p) { p.to_cartesian<To>(); };
template<typename To>
concept spherical_makes =
  requires(const utility::spherical_vector<si::metre, si::radian, double>& s) { s.to_cartesian<To>(); };
static_assert(polar_makes<vec2>);
static_assert(!polar_makes<vec3>);
static_assert(spherical_makes<vec3>);
static_assert(!spherical_makes<vec2>);

}  // namespace

TEST_CASE("polar_vector round-trips through a backend vector", "[polar][integration]")
{
  INFO("backend: " << backend);

  SECTION("construction from a 2-D backend vector quantity")
  {
    // (3, 4) m -> radius 5 m, heading atan2(4, 3)
    const quantity<si::metre, vec2> xy{make_vec2(3.0, 4.0), si::metre};
    const utility::polar_vector<si::metre, si::radian, double> p{xy};
    CHECK_THAT(p.radius().numerical_value_in(m), WithinAbs(5.0, 1e-12));
    CHECK_THAT(p.theta().numerical_value_in(rad), WithinAbs(std::atan2(4.0, 3.0), 1e-12));
  }

  SECTION("to_cartesian into the backend vector type")
  {
    const utility::polar_vector<si::metre, si::radian, double> p{2.0 * m, 0.0 * rad};
    const quantity xy = p.to_cartesian<vec2>();
    static_assert(std::same_as<decltype(xy)::rep, vec2>);
    CHECK_THAT(xy.numerical_value_in(m)[0], WithinAbs(2.0, 1e-12));
    CHECK_THAT(xy.numerical_value_in(m)[1], WithinAbs(0.0, 1e-12));
  }
}

TEST_CASE("spherical_vector round-trips through a backend vector", "[spherical][integration]")
{
  INFO("backend: " << backend);

  SECTION("construction from a 3-D backend vector quantity")
  {
    // (3, 4, 0) m -> radius 5 m, inclination pi/2 (in the xy-plane), azimuth atan2(4, 3)
    const quantity<si::metre, vec3> xyz{make_vec3(3.0, 4.0, 0.0), si::metre};
    const utility::spherical_vector<si::metre, si::radian, double> sph{xyz};
    CHECK_THAT(sph.radius().numerical_value_in(m), WithinAbs(5.0, 1e-12));
    CHECK_THAT(sph.theta().numerical_value_in(rad), WithinAbs(std::numbers::pi / 2.0, 1e-12));
    CHECK_THAT(sph.phi().numerical_value_in(rad), WithinAbs(std::atan2(4.0, 3.0), 1e-12));
  }

  SECTION("to_cartesian into the backend vector type")
  {
    const utility::spherical_vector<si::metre, si::radian, double> sph{2.0 * m, std::numbers::pi / 2.0 * rad,
                                                                       0.0 * rad};
    const quantity xyz = sph.to_cartesian<vec3>();
    static_assert(std::same_as<decltype(xyz)::rep, vec3>);
    CHECK_THAT(xyz.numerical_value_in(m)[0], WithinAbs(2.0, 1e-12));
    CHECK_THAT(xyz.numerical_value_in(m)[1], WithinAbs(0.0, 1e-12));
    CHECK_THAT(xyz.numerical_value_in(m)[2], WithinAbs(0.0, 1e-12));
  }
}

TEST_CASE("the backend vector is structured-bindings friendly", "[integration][structured-bindings]")
{
  INFO("backend: " << backend);
  const vec3 v = make_vec3(1.0, 2.0, 3.0);
  const auto [x, y, z] = v;
  CHECK_THAT(x, WithinAbs(1.0, 1e-12));
  CHECK_THAT(y, WithinAbs(2.0, 1e-12));
  CHECK_THAT(z, WithinAbs(3.0, 1e-12));
}
