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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <mp-units/compat_macros.h>  // QUANTITY_SPEC - a macro, so needed even in the module build
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cmath>
#include <numbers>
#endif
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/systems/angular.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/polar_vector.h>
#include <mp-units/utility/spherical_vector.h>
#endif

using namespace mp_units;
using namespace mp_units::utility;
using namespace mp_units::si::unit_symbols;  // rad = si::radian, deg = si::degree
using namespace Catch::Matchers;

namespace {

constexpr double kpi = std::numbers::pi;
constexpr double tol = 1e-12;

}  // namespace

// A user-defined, from-scratch strong angle system (its own base dimension - not `angular`, not SI)
// used to prove the `radian_of` customization point makes the facades work with any angle system.
// This is the whole reason `radian_of` is exported, so it must work in the module build too - the
// `QUANTITY_SPEC` macro comes from <mp-units/compat_macros.h> and every entity from `import mp_units`.
namespace test_angle {

inline constexpr struct dim_angle final : base_dimension<"A"> {
} dim_angle;
QUANTITY_SPEC(angle, dim_angle);
inline constexpr struct radian final : named_unit<"trad", kind_of<angle>> {
} radian;
inline constexpr struct degree final : named_unit<"tdeg", mag_ratio<1, 180> * mp_units::π * radian> {
} degree;

}  // namespace test_angle

// the one-line opt-in a custom-system author writes:
template<>
constexpr auto mp_units::utility::radian_of<get_quantity_spec(test_angle::radian)> = test_angle::radian;

// The radius must be a radial magnitude, never an angle: neither coordinate may be swapped for the
// other, and an angle-valued radius is a category error rejected at compile time.
template<auto R, auto A>
concept constructible_polar = requires(quantity<R, double> qr, quantity<A, double> qa) { polar_vector{qr, qa}; };
static_assert(constructible_polar<si::metre, si::degree>);    // (length, angle) - the intended shape
static_assert(!constructible_polar<si::degree, si::metre>);   // swapped: angle radius, non-angle "angle"
static_assert(!constructible_polar<si::degree, si::degree>);  // angle-valued radius is a category error
// the radius must be a scalar magnitude: a scalar quantity_spec is fine, a vector one is rejected
static_assert(constructible_polar<isq::speed[m / s], si::degree>);
static_assert(!constructible_polar<isq::velocity[m / s], si::degree>);

TEST_CASE("polar_vector", "[polar]")
{
  SECTION("native construction and accessors (radius is a quantity)")
  {
    const polar_vector p{2.0 * m, (kpi / 2) * rad};
    CHECK_THAT(p.radius().numerical_value_in(m), WithinAbs(2.0, tol));
    CHECK_THAT(p.magnitude().numerical_value_in(m), WithinAbs(2.0, tol));
    CHECK_THAT(p.theta().numerical_value_in(rad), WithinAbs(kpi / 2, tol));
  }

  SECTION("to_cartesian yields a vector quantity")
  {
    const polar_vector p{2.0 * m, (kpi / 2) * rad};
    const quantity c = p.to_cartesian();  // quantity<m, cartesian_vector<double, 2>>
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(0.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[1], WithinAbs(2.0, tol));
  }

  SECTION("from a vector quantity and round-trip (default angle: si::radian)")
  {
    const quantity disp = utility::cartesian_vector{3.0, 4.0} * m;
    const polar_vector p{disp};
    static_assert(p.angle_unit == si::radian);
    CHECK_THAT(p.radius().numerical_value_in(m), WithinAbs(5.0, tol));
    CHECK_THAT(p.theta().numerical_value_in(rad), WithinAbs(std::atan2(4.0, 3.0), tol));
    const quantity c = p.to_cartesian();
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(3.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[1], WithinAbs(4.0, tol));
  }

  SECTION("SI degree is converted, not mistaken for radians")
  {
    const polar_vector p{2.0 * m, 90.0 * deg};  // CTAD: si::degree
    static_assert(p.angle_unit == si::degree);
    const quantity c = p.to_cartesian();  // 90 deg -> (0, 2), not (cos 90rad, ...)
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(0.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[1], WithinAbs(2.0, tol));
    // an si::radian value reads back in si::degree (same kind)
    CHECK_THAT((polar_vector{2.0 * m, (kpi / 2) * rad}).theta().numerical_value_in(deg), WithinAbs(90.0, tol));
  }

  SECTION("radius reference may be a quantity_spec[unit], not only a bare unit")
  {
    // CTAD keeps the full reference (isq::radius[m]), so the radius is that specific quantity
    const polar_vector p{2.0 * isq::radius[m], 30.0 * deg};
    static_assert(p.radius_reference == isq::radius[m]);
    static_assert(p.radius_unit == m);
    static_assert(std::same_as<decltype(p)::radius_type, quantity<isq::radius[m], double>>);
    CHECK_THAT(p.radius().numerical_value_in(m), WithinAbs(2.0, tol));
    // to_cartesian drops the scalar radius spec: a vector is not "a radius", so it is a bare-unit vector
    const quantity c = p.to_cartesian();
    static_assert(c.unit == m);
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(2.0 * std::cos(kpi / 6), tol));
  }

  SECTION("opt-in: the strong angular system also works")
  {
    const polar_vector p{2.0 * m, 90.0 * angular::degree};
    static_assert(p.angle_unit == angular::degree);
    const quantity c = p.to_cartesian();
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(0.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[1], WithinAbs(2.0, tol));
  }

  SECTION("radial scaling and rotation")
  {
    const polar_vector p{2.0 * m, 30.0 * deg};
    CHECK_THAT((2.0 * p).radius().numerical_value_in(m), WithinAbs(4.0, tol));
    CHECK_THAT((p / 2.0).radius().numerical_value_in(m), WithinAbs(1.0, tol));
    CHECK_THAT((-1.0 * p).radius().numerical_value_in(m), WithinAbs(-2.0, tol));  // negative allowed
    CHECK_THAT(p.rotated_by(60.0 * deg).theta().numerical_value_in(deg), WithinAbs(90.0, tol));

    // dimensioned factor changes the radius unit (velocity * time -> length), angle preserved
    const polar_vector vel{20.0 * (m / s), 30.0 * deg};
    const auto disp = vel * (3.0 * s);
    static_assert(disp.radius_unit == m);
    CHECK_THAT(disp.radius().numerical_value_in(m), WithinAbs(60.0, tol));
    CHECK_THAT(disp.theta().numerical_value_in(deg), WithinAbs(30.0, tol));
  }

  SECTION("azimuth is canonicalized; equality is geometric")
  {
    // construction normalizes the azimuth into the canonical range
    CHECK_THAT((polar_vector{2.0 * m, 400.0 * deg}).theta().numerical_value_in(deg), WithinAbs(40.0, tol));
    CHECK_THAT((polar_vector{2.0 * m, 190.0 * deg}).theta().numerical_value_in(deg), WithinAbs(-170.0, tol));

    const polar_vector p{1.0 * m, 30.0 * deg};
    CHECK(p == p.rotated_by(360.0 * deg));  // a full turn is the identity
    CHECK(p == polar_vector{1.0 * m, 390.0 * deg});
    CHECK(p != polar_vector{1.0 * m, 60.0 * deg});
  }

  SECTION("rotation by more than a full turn wraps (multi-turn)")
  {
    const polar_vector p{1.0 * m, 30.0 * deg};
    CHECK_THAT(p.rotated_by(450.0 * deg).theta().numerical_value_in(deg), WithinAbs(120.0, tol));  // 30+450=480->120
    CHECK_THAT(p.rotated_by(810.0 * deg).theta().numerical_value_in(deg), WithinAbs(120.0, tol));  // +2 turns +90
    CHECK(p == p.rotated_by(720.0 * deg));                                                         // two full turns
    CHECK(p == p.rotated_by(-360.0 * deg));                                                        // negative full turn
    CHECK_THAT(p.rotated_by(-330.0 * deg).theta().numerical_value_in(deg), WithinAbs(60.0, tol));  // 30-330=-300 -> 60
    // construction with a large out-of-range azimuth also wraps
    CHECK_THAT((polar_vector{1.0 * m, 30.0 * deg + 3.0 * 360.0 * deg}).theta().numerical_value_in(deg),
               WithinAbs(30.0, tol));
  }
}

TEST_CASE("spherical_vector", "[spherical]")
{
  SECTION("native construction and accessors")
  {
    const spherical_vector sph{3.0 * m, (kpi / 4) * rad, (kpi / 3) * rad};
    CHECK_THAT(sph.radius().numerical_value_in(m), WithinAbs(3.0, tol));
    CHECK_THAT(sph.theta().numerical_value_in(rad), WithinAbs(kpi / 4, tol));
    CHECK_THAT(sph.phi().numerical_value_in(rad), WithinAbs(kpi / 3, tol));
  }

  SECTION("to_cartesian on the +z axis")
  {
    const spherical_vector sph{2.0 * m, 0.0 * rad, 0.0 * rad};
    const quantity c = sph.to_cartesian();
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(0.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[1], WithinAbs(0.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[2], WithinAbs(2.0, tol));
  }

  SECTION("dimensioned radial scaling (velocity * time), no Cartesian round-trip")
  {
    const spherical_vector v{20.0 * (m / s), 50.0 * deg, 30.0 * deg};
    const auto displacement = v * (3.0 * s);  // radius m/s -> m; angles unchanged
    static_assert(displacement.radius_unit == m);
    CHECK_THAT(displacement.radius().numerical_value_in(m), WithinAbs(60.0, tol));
    CHECK_THAT(displacement.theta().numerical_value_in(deg), WithinAbs(50.0, tol));
    CHECK_THAT(displacement.phi().numerical_value_in(deg), WithinAbs(30.0, tol));
  }

  SECTION("from a vector quantity and round-trip")
  {
    const quantity disp = utility::cartesian_vector{1.0, 1.0, 1.0} * m;
    const spherical_vector sph{disp};
    CHECK_THAT(sph.radius().numerical_value_in(m), WithinAbs(std::sqrt(3.0), tol));
    CHECK_THAT(sph.theta().numerical_value_in(rad), WithinAbs(std::acos(1.0 / std::sqrt(3.0)), tol));
    CHECK_THAT(sph.phi().numerical_value_in(rad), WithinAbs(kpi / 4, tol));
    const quantity c = sph.to_cartesian();
    CHECK_THAT(c.numerical_value_in(m)[0], WithinAbs(1.0, tol));
    CHECK_THAT(c.numerical_value_in(m)[2], WithinAbs(1.0, tol));
  }

  SECTION("radial scaling and azimuthal rotation")
  {
    const spherical_vector sph{2.0 * m, 30.0 * deg, 30.0 * deg};
    CHECK_THAT((2.0 * sph).radius().numerical_value_in(m), WithinAbs(4.0, tol));
    CHECK_THAT(sph.rotated_about_z(60.0 * deg).phi().numerical_value_in(deg), WithinAbs(90.0, tol));
    CHECK(sph == sph.rotated_about_z(360.0 * deg));  // full turn is the identity
  }

  SECTION("tilted_by crosses a pole: inclination reflects and azimuth flips by pi")
  {
    const spherical_vector sph{2.0 * m, 170.0 * deg, 30.0 * deg};
    const auto tilted = sph.tilted_by(30.0 * deg);  // 200° -> (160°, azimuth + 180°)
    CHECK_THAT(tilted.theta().numerical_value_in(deg), WithinAbs(160.0, tol));
    CHECK_THAT(tilted.phi().numerical_value_in(deg), WithinAbs(-150.0, tol));
    CHECK(tilted == spherical_vector{2.0 * m, 200.0 * deg, 30.0 * deg});  // same direction, canonical

    // construction canonicalizes out-of-range inclination
    CHECK_THAT((spherical_vector{2.0 * m, 200.0 * deg, 30.0 * deg}).theta().numerical_value_in(deg),
               WithinAbs(160.0, tol));
  }

  SECTION("rotation/tilt by more than a full turn (multi-turn)")
  {
    const spherical_vector sph{2.0 * m, 30.0 * deg, 30.0 * deg};

    // azimuth: multi-turn wraps, whole turns are the identity
    CHECK_THAT(sph.rotated_about_z(450.0 * deg).phi().numerical_value_in(deg), WithinAbs(120.0, tol));  // +90 mod 360
    CHECK(sph == sph.rotated_about_z(720.0 * deg));
    CHECK(sph == sph.rotated_about_z(-360.0 * deg));

    // inclination tilt > 360° that also crosses a pole: 30+560=590 -> 230 (mod 360) -> reflect 130°, phi flips
    const auto tlt = sph.tilted_by(560.0 * deg);
    CHECK_THAT(tlt.theta().numerical_value_in(deg), WithinAbs(130.0, tol));
    CHECK_THAT(tlt.phi().numerical_value_in(deg), WithinAbs(-150.0, tol));
    CHECK(tlt == sph.tilted_by(200.0 * deg));  // 560° and 200° tilt are the same direction
    CHECK(sph == sph.tilted_by(360.0 * deg));  // a full inclination turn is the identity
  }

  SECTION("zero radius leaves the angles at zero")
  {
    const quantity origin = utility::cartesian_vector{0.0, 0.0, 0.0} * m;
    const spherical_vector sph{origin};
    CHECK(sph.radius().numerical_value_in(m) == 0.0);
    CHECK(sph.theta().numerical_value_in(rad) == 0.0);
    CHECK(sph.phi().numerical_value_in(rad) == 0.0);
  }
}

TEST_CASE("converting constructors between compatible facades", "[polar][spherical][conversion]")
{
  SECTION("polar: unit conversion (degree -> radian) preserves the value, implicitly")
  {
    const polar_vector<si::metre, si::degree, double> src{2.0 * m, 90.0 * deg};
    const polar_vector<si::metre, si::radian, double> dst = src;  // same kinds, convertible unit
    CHECK_THAT(dst.radius().numerical_value_in(m), WithinAbs(2.0, tol));
    CHECK_THAT(dst.theta().numerical_value_in(rad), WithinAbs(kpi / 2, tol));
  }

  SECTION("polar: rep conversion follows the cartesian_vector rule (floating-point target implicit)")
  {
    const polar_vector<si::metre, si::radian, float> flt{2.0f * m, 1.0f * rad};
    const polar_vector<si::metre, si::radian, double> wide = flt;  // float -> double: implicit
    CHECK_THAT(wide.theta().numerical_value_in(rad), WithinAbs(1.0, 1e-6));

    // double -> float: a floating-point target, so implicit here (like cartesian_vector), and the
    // rep is static_cast so it stays warning-clean under -Wfloat-conversion
    static_assert(
      std::convertible_to<polar_vector<si::metre, si::radian, double>, polar_vector<si::metre, si::radian, float>>);
    const polar_vector<si::metre, si::radian, float> narrow = wide;
    CHECK_THAT(narrow.theta().numerical_value_in(rad), WithinAbs(1.0, 1e-6));
  }

  SECTION("incompatible facades do not convert")
  {
    // different angle kinds (the strong `angular` system vs SI) never interconvert
    static_assert(!std::constructible_from<polar_vector<si::metre, si::radian, double>,
                                           polar_vector<si::metre, angular::radian, double>>);
    // different radius dimension (length vs time) is rejected
    static_assert(!std::constructible_from<polar_vector<si::metre, si::radian, double>,
                                           polar_vector<si::second, si::radian, double>>);
  }

  SECTION("spherical: unit conversion on both angles")
  {
    const spherical_vector<si::metre, si::degree, double> src{2.0 * m, 60.0 * deg, 45.0 * deg};
    const spherical_vector<si::metre, si::radian, double> dst = src;
    CHECK_THAT(dst.radius().numerical_value_in(m), WithinAbs(2.0, tol));
    CHECK_THAT(dst.theta().numerical_value_in(rad), WithinAbs(kpi / 3, tol));
    CHECK_THAT(dst.phi().numerical_value_in(rad), WithinAbs(kpi / 4, tol));
  }

  SECTION("from a cartesian_vector quantity whose rep differs from the facade's")
  {
    const quantity vf = utility::cartesian_vector{3.0f, 4.0f} * m;  // rep = float
    const polar_vector<si::metre, si::radian, double> p{vf};        // double facade from float vector
    CHECK_THAT(p.radius().numerical_value_in(m), WithinAbs(5.0, 1e-6));

    const quantity vf3 = utility::cartesian_vector{0.0f, 0.0f, 2.0f} * m;
    const spherical_vector<si::metre, si::radian, double> sv{vf3};  // double facade from float vector
    CHECK_THAT(sv.radius().numerical_value_in(m), WithinAbs(2.0, 1e-6));
  }

  SECTION("to a cartesian_vector quantity whose rep differs from the facade's")
  {
    const polar_vector<si::metre, si::radian, double> p{2.0 * m, 0.0 * rad};
    const quantity<si::metre, utility::cartesian_vector<float, 2>> xy = p.to_cartesian();  // double -> float
    CHECK_THAT(xy.numerical_value_in(m)[0], WithinAbs(2.0f, 1e-6f));
    CHECK_THAT(xy.numerical_value_in(m)[1], WithinAbs(0.0f, 1e-6f));
  }

  SECTION("CTAD from a vector-character quantity takes the radius from its magnitude()")
  {
    // isq::velocity has vector character, so it has a magnitude(); the deduced radius is that
    // magnitude's scalar reference (a kind over m/s in V2, upgrading to isq::speed in V3)
    const quantity vel = isq::velocity(utility::cartesian_vector{0.0, 0.0, 2.0} * (m / s));
    const spherical_vector sv{vel};
    static_assert(get_quantity_spec(sv.radius_reference).character.order == quantity_tensor_order::scalar);
    static_assert(sv.radius_unit == m / s);
    CHECK_THAT(sv.radius().numerical_value_in(m / s), WithinAbs(2.0, tol));
    // and it round-trips back into a velocity vector
    const quantity<isq::velocity[m / s], utility::cartesian_vector<double, 3>> back = sv.to_cartesian();
    CHECK_THAT(back.numerical_value_in(m / s)[2], WithinAbs(2.0, tol));
  }
}

TEST_CASE("the facades are not tied to cartesian_vector", "[polar][spherical][generic-rep]")
{
  // The input rep only has to be a tuple-like vector of the right dimension with a magnitude() CPO
  // (dimension read from std::tuple_size, components via get<>). The dimension gating is observable
  // through construction: a 2-D vector builds a polar_vector but not a spherical_vector, and vice versa.
  using v2 = quantity<si::metre, utility::cartesian_vector<double, 2>>;
  using v3 = quantity<si::metre, utility::cartesian_vector<double, 3>>;
  static_assert(std::constructible_from<polar_vector<si::metre, si::radian, double>, v2>);
  static_assert(!std::constructible_from<polar_vector<si::metre, si::radian, double>, v3>);
  static_assert(std::constructible_from<spherical_vector<si::metre, si::radian, double>, v3>);
  static_assert(!std::constructible_from<spherical_vector<si::metre, si::radian, double>, v2>);

  // to_cartesian's destination representation is a template parameter (here a different element type,
  // proving the output is not hard-wired to cartesian_vector<Rep, N>)
  const polar_vector<si::metre, si::radian, double> p{2.0 * m, 0.0 * rad};
  const quantity<si::metre, utility::cartesian_vector<float, 2>> xy =
    p.to_cartesian<utility::cartesian_vector<float, 2>>();
  CHECK_THAT(xy.numerical_value_in(m)[0], WithinAbs(2.0f, 1e-6f));
  CHECK_THAT(xy.numerical_value_in(m)[1], WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("angle unit is an extension point via radian_of", "[polar][spherical][angle_unit]")
{
  using namespace test_angle;

  SECTION("a user-defined angle system works once its radian is registered")
  {
    // 90 custom-degrees = pi/2 custom-radians; to_cartesian must scale through the registered radian
    const polar_vector p{2.0 * m, 90.0 * degree};
    static_assert(p.angle_unit == degree);
    CHECK_THAT(p.theta().numerical_value_in(radian), WithinAbs(kpi / 2, tol));
    const quantity xy = p.to_cartesian();
    CHECK_THAT(xy.numerical_value_in(m)[0], WithinAbs(0.0, tol));  // 2 cos 90°
    CHECK_THAT(xy.numerical_value_in(m)[1], WithinAbs(2.0, tol));  // 2 sin 90°
  }

  SECTION("round-trips and canonicalization use the custom radian too")
  {
    const spherical_vector sph{2.0 * m, 60.0 * degree, 45.0 * degree};
    CHECK_THAT(sph.to_cartesian().numerical_value_in(m)[2], WithinAbs(2.0 * std::cos(kpi / 3), tol));  // z = r cos θ
    // a full custom-turn (360 tdeg) is the identity, proving canonicalization scales through trad
    CHECK(sph == sph.rotated_about_z(360.0 * degree));
    CHECK(sph == sph.tilted_by(360.0 * degree));
  }
}
