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
#include <mp-units/compat_macros.h>
#include <mp-units/framework.h>
#include <mp-units/overflow_policies.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/constrained.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <limits>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::utility;
using namespace mp_units::si::unit_symbols;

// ============================================================================
// Test quantity specifications and origins with bounds
// ============================================================================

QUANTITY_SPEC(test_angle_clamp, isq::angular_measure);
QUANTITY_SPEC(test_angle_wrap, isq::angular_measure);
QUANTITY_SPEC(test_angle_reflect, isq::angular_measure);
QUANTITY_SPEC(test_angle_wrap_constrained, isq::angular_measure);
QUANTITY_SPEC(test_angle_reflect_constrained, isq::angular_measure);
QUANTITY_SPEC(test_angle_unordered_args, isq::angular_measure);

struct test_origin_tag final {};

inline constexpr struct clamp_origin final :
    absolute_point_origin<test_angle_clamp, clamp_to_range{-90 * deg, 90 * deg}> {
} clamp_origin;
inline constexpr struct wrap_origin final :
    absolute_point_origin<test_angle_wrap, wrap_to_range{-180 * deg, 180 * deg}> {
} wrap_origin;
inline constexpr struct reflect_origin final :
    absolute_point_origin<test_angle_reflect, reflect_in_range{-90 * deg, 90 * deg}> {
} reflect_origin;
inline constexpr struct wrap_constrained_origin final :
    absolute_point_origin<test_angle_wrap_constrained, wrap_to_range{-180 * deg, 180 * deg}> {
} wrap_constrained_origin;
inline constexpr struct reflect_constrained_origin final :
    absolute_point_origin<test_angle_reflect_constrained, reflect_in_range{-90 * deg, 90 * deg}> {
} reflect_constrained_origin;

inline constexpr struct unordered_args_origin final :
    absolute_point_origin<test_angle_unordered_args, test_origin_tag{}, clamp_to_range{-45 * deg, 45 * deg}> {
} unordered_args_origin;

// Separate origins for unit conversion tests (need double bounds)
QUANTITY_SPEC(test_angle_clamp_convert, isq::angular_measure);
QUANTITY_SPEC(test_angle_wrap_convert, isq::angular_measure);

inline constexpr struct clamp_convert_origin final :
    absolute_point_origin<test_angle_clamp_convert, clamp_to_range{-90.0 * deg, 90.0 * deg}> {
} clamp_convert_origin;
inline constexpr struct wrap_convert_origin final :
    absolute_point_origin<test_angle_wrap_convert, wrap_to_range{-180.0 * deg, 180.0 * deg}> {
} wrap_convert_origin;

// Origins for testing point_for() with bounds
// MSL and AGL are independent absolute origins — the translation between them
// depends on terrain elevation, which is a runtime value and cannot be fixed
// at compile time as a relative_point_origin offset.
QUANTITY_SPEC(bounded_altitude, isq::height);

inline constexpr struct altitude_msl final :
    absolute_point_origin<bounded_altitude, clamp_to_range{-500 * m, 12000 * m}> {
} altitude_msl;  // Mean Sea Level: physical bounds [-500m, 12000m]

inline constexpr struct altitude_agl final : absolute_point_origin<bounded_altitude, clamp_to_range{0 * m, 500 * m}> {
} altitude_agl;  // Above Ground Level: operational bounds [0m, 500m] (drone)

// Longitude with absolute origin (prime meridian) and a relative Warsaw origin.
QUANTITY_SPEC(geo_longitude, isq::angular_measure);

inline constexpr struct prime_meridian final :
    absolute_point_origin<geo_longitude, clamp_to_range{-180.0 * deg, 180.0 * deg}> {
} prime_meridian;  // bounds: [-180°, 180°]

// Body and room temperature for point_for() cross-origin bounds tests.
QUANTITY_SPEC(clinical_temperature, isq::thermodynamic_temperature);

inline constexpr struct body_temp_origin final :
    absolute_point_origin<clinical_temperature, clamp_to_range{delta<clinical_temperature[si::degree_Celsius]>(35.0),
                                                               delta<clinical_temperature[si::degree_Celsius]>(42.0)}> {
} body_temp_origin;  // bounds: [35°C, 42°C] (clinical range)

inline constexpr struct room_temp_origin final :
    absolute_point_origin<clinical_temperature, clamp_to_range{delta<clinical_temperature[si::degree_Celsius]>(15.0),
                                                               delta<clinical_temperature[si::degree_Celsius]>(30.0)}> {
} room_temp_origin;  // bounds: [15°C, 30°C]

// Warsaw meridian: ~21°E of prime meridian.
// A quantity_point measured from warsaw_meridian represents "degrees east/west of Warsaw".
// No user-specific bounds are added here; only the absolute origin's physical bounds apply.
inline constexpr struct warsaw_meridian final :
    mp_units::relative_point_origin<quantity_point<geo_longitude[deg], prime_meridian, double>{21.0 * deg,
                                                                                               prime_meridian}> {
} warsaw_meridian;

// A second relative origin further east (e.g., Kyiv ~30°E) for chained-offset tests.
inline constexpr struct kyiv_meridian final :
    mp_units::relative_point_origin<quantity_point<geo_longitude[deg], prime_meridian, double>{30.0 * deg,
                                                                                               prime_meridian}> {
} kyiv_meridian;

using qp_clamp = quantity_point<test_angle_clamp[deg], clamp_origin, double>;
using qp_wrap = quantity_point<test_angle_wrap[deg], wrap_origin, double>;
using qp_reflect = quantity_point<test_angle_reflect[deg], reflect_origin, double>;
using qp_unordered_args = quantity_point<test_angle_unordered_args[deg], unordered_args_origin, double>;
using qp_clamp_int = quantity_point<test_angle_clamp[deg], clamp_origin, int>;

// constrained rep: bounds stored as int, incoming value type is constrained<double>
// (uses default policy: throw_policy in hosted, terminate_policy in freestanding)
using safe_double = constrained<double>;
using qp_wrap_constrained = quantity_point<test_angle_wrap_constrained[deg], wrap_constrained_origin, safe_double>;
using qp_reflect_constrained =
  quantity_point<test_angle_reflect_constrained[deg], reflect_constrained_origin, safe_double>;

// Type aliases for unit conversion tests (need double bounds)
using qp_clamp_convert = quantity_point<test_angle_clamp_convert[deg], clamp_convert_origin, double>;
using qp_wrap_convert = quantity_point<test_angle_wrap_convert[deg], wrap_convert_origin, double>;

// ============================================================================
// FILE OVERVIEW
//
// Part I: Basic policy mechanics
//   Clamp, wrap, reflect policies; constrained-rep cross-type arithmetic;
//   mutating operators (+=, -=, ++, --); unit conversion; temperature-unit
//   semantics (Δ°C = ΔK, °C-style bounds, ice-point via relative origin).
//
// Part II: Origin-inheritance scenarios
//   How bounds propagate through the origin chain.  Scenarios are classified
//   by which origin(s) carry bounds:
//     Scenario 1  abs[bounds]                              (direct or inherited by rel)
//     Scenario 2  abs[none]  ← rel[bounds]                 (rel-only bounds)
//     Scenario 3  abs[bounds] ← rel[bounds]                (both; rel is tighter)
//     Scenario 4  abs[none]  ← rel1[bounds] ← rel2[none]   (bounds inherited through chain)
//     Scenario 5  abs[none]  ← rel1[bounds] ← rel2[bounds] (two levels of own bounds)
//
// Part III: Additional domain and edge-case tests
//   Time-of-day (wrap_to_range on isq::duration), min()/max()/numeric_limits.
// ============================================================================

// ============================================================================
// Clamp policy
// ============================================================================

static_assert(qp_clamp(45.0 * deg, clamp_origin).quantity_from(clamp_origin) == 45.0 * deg);
static_assert(qp_clamp(100.0 * deg, clamp_origin).quantity_from(clamp_origin) == 90.0 * deg);
static_assert(qp_clamp(-120.0 * deg, clamp_origin).quantity_from(clamp_origin) == -90.0 * deg);
static_assert(qp_clamp(90.0 * deg, clamp_origin).quantity_from(clamp_origin) == 90.0 * deg);
static_assert(qp_clamp(-90.0 * deg, clamp_origin).quantity_from(clamp_origin) == -90.0 * deg);

// ============================================================================
// Wrap policy
// ============================================================================

static_assert(qp_wrap(90.0 * deg, wrap_origin).quantity_from(wrap_origin) == 90.0 * deg);
static_assert(qp_wrap(200.0 * deg, wrap_origin).quantity_from(wrap_origin) == -160.0 * deg);
static_assert(qp_wrap(-200.0 * deg, wrap_origin).quantity_from(wrap_origin) == 160.0 * deg);
static_assert(qp_wrap(180.0 * deg, wrap_origin).quantity_from(wrap_origin) == -180.0 * deg);

// ============================================================================
// Reflect policy
// ============================================================================

static_assert(qp_reflect(45.0 * deg, reflect_origin).quantity_from(reflect_origin) == 45.0 * deg);
static_assert(qp_reflect(91.0 * deg, reflect_origin).quantity_from(reflect_origin) == 89.0 * deg);

// ============================================================================
// Bounds argument does not require first position in NTTP pack
// ============================================================================

static_assert(qp_unordered_args(50.0 * deg, unordered_args_origin).quantity_from(unordered_args_origin) == 45.0 * deg);
static_assert(qp_unordered_args(-50.0 * deg, unordered_args_origin).quantity_from(unordered_args_origin) ==
              -45.0 * deg);
static_assert(qp_reflect(-91.0 * deg, reflect_origin).quantity_from(reflect_origin) == -89.0 * deg);
static_assert(qp_reflect(90.0 * deg, reflect_origin).quantity_from(reflect_origin) == 90.0 * deg);
static_assert(qp_reflect(-90.0 * deg, reflect_origin).quantity_from(reflect_origin) == -90.0 * deg);

// ============================================================================
// wrap/reflect with constrained rep (bounds stored as int, incoming rep is constrained<double,...>)
// This exercises the cross-type arithmetic in wrap_to_range::operator() and
// reflect_in_range::operator().
// ============================================================================

static_assert(qp_wrap_constrained(safe_double{90.0} * deg, wrap_constrained_origin)
                .quantity_from(wrap_constrained_origin) == safe_double{90.0} * deg);
static_assert(qp_wrap_constrained(safe_double{200.0} * deg, wrap_constrained_origin)
                .quantity_from(wrap_constrained_origin) == safe_double{-160.0} * deg);
static_assert(qp_wrap_constrained(safe_double{-200.0} * deg, wrap_constrained_origin)
                .quantity_from(wrap_constrained_origin) == safe_double{160.0} * deg);

static_assert(qp_reflect_constrained(safe_double{45.0} * deg, reflect_constrained_origin)
                .quantity_from(reflect_constrained_origin) == safe_double{45.0} * deg);
static_assert(qp_reflect_constrained(safe_double{91.0} * deg, reflect_constrained_origin)
                .quantity_from(reflect_constrained_origin) == safe_double{89.0} * deg);
static_assert(qp_reflect_constrained(safe_double{-91.0} * deg, reflect_constrained_origin)
                .quantity_from(reflect_constrained_origin) == safe_double{-89.0} * deg);

// ============================================================================
// Mutating operators enforce bounds
// ============================================================================

consteval bool clamp_plus_assign()
{
  auto pt = qp_clamp(80.0 * deg, clamp_origin);
  pt += 20.0 * deg;
  return pt.quantity_from(clamp_origin) == 90.0 * deg;
}
static_assert(clamp_plus_assign());

consteval bool clamp_minus_assign()
{
  auto pt = qp_clamp(-80.0 * deg, clamp_origin);
  pt -= 20.0 * deg;
  return pt.quantity_from(clamp_origin) == -90.0 * deg;
}
static_assert(clamp_minus_assign());

consteval bool clamp_pre_increment()
{
  auto pt = qp_clamp_int(90 * deg, clamp_origin);
  ++pt;
  return pt.quantity_from(clamp_origin) == 90 * deg;
}
static_assert(clamp_pre_increment());

consteval bool clamp_pre_decrement()
{
  auto pt = qp_clamp_int(-90 * deg, clamp_origin);
  --pt;
  return pt.quantity_from(clamp_origin) == -90 * deg;
}
static_assert(clamp_pre_decrement());

consteval bool clamp_post_increment()
{
  auto pt = qp_clamp_int(89 * deg, clamp_origin);
  auto old_value = pt++;
  // Check old value returned
  if (old_value.quantity_from(clamp_origin) != 89 * deg) return false;
  // Check new value is clamped to max
  return pt.quantity_from(clamp_origin) == 90 * deg;
}
static_assert(clamp_post_increment());

consteval bool clamp_post_decrement()
{
  auto pt = qp_clamp_int(-89 * deg, clamp_origin);
  auto old_value = pt--;
  // Check old value returned
  if (old_value.quantity_from(clamp_origin) != -89 * deg) return false;
  // Check new value is clamped to min
  return pt.quantity_from(clamp_origin) == -90 * deg;
}
static_assert(clamp_post_decrement());

consteval bool wrap_post_increment()
{
  auto pt_wrap_int = quantity_point<test_angle_wrap[deg], wrap_origin, int>(179 * deg, wrap_origin);
  auto old_value = pt_wrap_int++;
  // Check old value returned
  if (old_value.quantity_from(wrap_origin) != 179 * deg) return false;
  // Check new value wraps to -180 (since range is [-180, 180))
  return pt_wrap_int.quantity_from(wrap_origin) == -180 * deg;
}
static_assert(wrap_post_increment());

// ============================================================================
// Unit conversion with bounded quantity_points
// ============================================================================

// Simple scaling: degrees to radians and back
consteval bool angle_unit_conversion()
{
  auto pt = qp_clamp_convert(45.0 * deg, clamp_convert_origin);
  auto in_rad = pt.in(rad);
  // 45° in radians
  constexpr auto expected_rad = (45.0 * deg).numerical_value_in(rad);
  return in_rad.quantity_from(clamp_convert_origin).numerical_value_in(rad) == expected_rad;
}
static_assert(angle_unit_conversion());

consteval bool clamp_with_unit_scaling()
{
  // Create with radians (exceeds bounds in degrees: π rad = 180°), should clamp to 90°
  constexpr auto pi_rad = 1.0 * pi * rad;
  auto pt = qp_clamp_convert(pi_rad, clamp_convert_origin);
  // Expected: clamped to 90°
  constexpr auto expected = 90.0 * deg;
  return pt.quantity_from(clamp_convert_origin) == expected;
}
static_assert(clamp_with_unit_scaling());

// Wrap with unit scaling
consteval bool wrap_with_unit_scaling()
{
  // 400° wraps to 40° (400 - 360 = 40)
  auto pt = qp_wrap_convert(400.0 * deg, wrap_convert_origin);
  const auto in_rad = pt.in(rad);
  const auto back_in_deg = in_rad.quantity_from(wrap_convert_origin);
  // Wrapped value: 400 - 360 = 40, which is in [-180, 180)
  return back_in_deg == 40.0 * deg;
}
static_assert(wrap_with_unit_scaling());

// ============================================================================
// Temperature conversion with offset units
// ============================================================================

// Temperature origin with bounds in kelvin
QUANTITY_SPEC(bounded_temperature, isq::thermodynamic_temperature);

inline constexpr struct temp_origin final :
    absolute_point_origin<bounded_temperature, clamp_to_range{delta<bounded_temperature[si::kelvin]>(200.0),
                                                              delta<bounded_temperature[si::kelvin]>(400.0)}> {
} temp_origin;

// Relative origin at +300K above temp_origin (simulates an ice-point-style offset).
// 300K is within temp_origin's [200K, 400K] range, so the QP constructor does not clamp it.
// Has NO own bounds; inherits enforcement from temp_origin's [200K, 400K] range.
// Local range from temp_offset_origin: [200-300, 400-300] = [-100K, 100K].
inline constexpr struct temp_offset_origin final :
    mp_units::relative_point_origin<mp_units::quantity_point<bounded_temperature[si::kelvin], temp_origin, double>{
      delta<bounded_temperature[si::kelvin]>(300.0), temp_origin}> {
} temp_offset_origin;

using qp_temp = quantity_point<bounded_temperature[si::kelvin], temp_origin, double>;

// Value within bounds
static_assert(qp_temp(delta<si::kelvin>(300.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(300.0));

// Above max clamps
static_assert(qp_temp(delta<si::kelvin>(500.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(400.0));

// Below min clamps
static_assert(qp_temp(delta<si::kelvin>(100.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(200.0));

// Convert to millikelvin (simple scaling)
consteval bool temperature_unit_scaling()
{
  auto pt = qp_temp(delta<si::kelvin>(300.0), temp_origin);
  auto in_mK = pt.in(si::milli<si::kelvin>);
  // 300 K = 300'000 mK
  return in_mK.quantity_from(temp_origin).numerical_value_in(si::milli<si::kelvin>) == 300'000.0;
}
static_assert(temperature_unit_scaling());

// Creating with millikelvin (out of bounds) clamps correctly
consteval bool temperature_clamp_with_millikelvin()
{
  // 500'000 mK = 500 K, which exceeds max of 400 K
  auto pt = qp_temp(delta<si::milli<si::kelvin>>(500'000.0), temp_origin);
  // Should clamp to 400 K = 400'000 mK
  return pt.in(si::milli<si::kelvin>).quantity_from(temp_origin) == delta<si::milli<si::kelvin>>(400'000.0);
}
static_assert(temperature_clamp_with_millikelvin());

// ============================================================================
// Offset-origin bounds: bounds on the parent absolute origin are applied after
// translating by the +300K offset of temp_offset_origin.
// Local allowed range: [200-300, 400-300] = [-100K, 100K] from temp_offset_origin.
// This also verifies that deg_C input (Δ°C == ΔK, same scale) produces the same
// result — confirming that the "offset" in bounds comes from the origin chain, not
// from the unit's own offset.
// ============================================================================

using qp_offset = quantity_point<bounded_temperature[si::kelvin], temp_offset_origin, double>;

// Within absolute bounds: 0K from offset = 300K abs ∈ [200, 400] → no clamp.
static_assert(qp_offset(delta<bounded_temperature[si::kelvin]>(0.0), temp_offset_origin)
                .quantity_from(temp_offset_origin) == delta<bounded_temperature[si::kelvin]>(0.0));

// Below abs min: -200K from offset = 100K abs < 200K → clamp to 200K abs → -100K from offset.
static_assert(qp_offset(delta<bounded_temperature[si::kelvin]>(-200.0), temp_offset_origin)
                .quantity_from(temp_offset_origin) == delta<bounded_temperature[si::kelvin]>(-100.0));

// Above abs max: 200K from offset = 500K abs > 400K → clamp to 400K abs → 100K from offset.
static_assert(qp_offset(delta<bounded_temperature[si::kelvin]>(200.0), temp_offset_origin)
                .quantity_from(temp_offset_origin) == delta<bounded_temperature[si::kelvin]>(100.0));

// Same tests using deg_C as input unit (Δ°C = ΔK, 1:1 scale):
// confirms that specifying the value in deg_C rather than K does not introduce
// a spurious 273.15 shift — the offset lives in the origin, not in the delta unit.
static_assert(qp_offset(delta<bounded_temperature[si::degree_Celsius]>(-200.0), temp_offset_origin)
                .quantity_from(temp_offset_origin) == delta<bounded_temperature[si::kelvin]>(-100.0));
static_assert(qp_offset(delta<bounded_temperature[si::degree_Celsius]>(200.0), temp_offset_origin)
                .quantity_from(temp_offset_origin) == delta<bounded_temperature[si::kelvin]>(100.0));

// ============================================================================
// Bounds in °C, input/output in K: point-temperature shift lives in the origin.
//
// A new spec is given bounds expressed in degrees Celsius [200 Δ°C, 350 Δ°C].
// Because Δ°C == ΔK (same scale), these are numerically identical to [200 ΔK, 350 ΔK].
// So bounds in °C give EXACTLY the same clamp thresholds as bounds in K for any
// input unit — there is no automatic "add-273" treatment.
//
// To get the physical ice-point shift (273 K) one must use a RELATIVE ORIGIN
// placed 273 K above the absolute origin.  The same numerical K value submitted
// to each origin then receives different treatment (accepted vs clamped),
// because it represents a different absolute temperature in each frame.
//
//   celsius_style_abs_origin      bounds [200 Δ°C, 350 Δ°C] ≡ [200 ΔK, 350 ΔK]
//   celsius_style_ice_origin      at +273 K from abs_origin (no own bounds)
//                                 inherited range: [200-273, 350-273] = [-73 K, +77 K]
// ============================================================================

QUANTITY_SPEC(celsius_style_temp, isq::thermodynamic_temperature);

inline constexpr struct celsius_style_abs_origin final :
    absolute_point_origin<celsius_style_temp, clamp_to_range{delta<celsius_style_temp[si::degree_Celsius]>(200.0),
                                                             delta<celsius_style_temp[si::degree_Celsius]>(350.0)}> {
} celsius_style_abs_origin;

// Relative origin at +273 K: simulates the ice-point shift with a round number.
// 273 ∈ [200, 350] so the QP constructor does not clamp it.
// Inherits abs_origin's bounds; local allowed range: [-73 K, +77 K].
inline constexpr struct celsius_style_ice_origin final :
    mp_units::relative_point_origin<
      mp_units::quantity_point<celsius_style_temp[si::kelvin], celsius_style_abs_origin, double>{
        delta<celsius_style_temp[si::kelvin]>(273.0), celsius_style_abs_origin}> {
} celsius_style_ice_origin;

using qp_abs = quantity_point<celsius_style_temp[si::kelvin], celsius_style_abs_origin, double>;
using qp_ice = quantity_point<celsius_style_temp[si::kelvin], celsius_style_ice_origin, double>;

// --- Bounds in °C, input in K: Δ°C == ΔK, no 273 shift from the unit ---

// Within [200 Δ°C, 350 Δ°C] = [200 ΔK, 350 ΔK]: 270 K passes.
static_assert(qp_abs(delta<celsius_style_temp[si::kelvin]>(270.0), celsius_style_abs_origin)
                .quantity_from(celsius_style_abs_origin) == delta<celsius_style_temp[si::kelvin]>(270.0));

// Below 200 Δ°C (= 200 ΔK): 30 K → clamped to 200 K, NOT to 473 K (200+273).
static_assert(qp_abs(delta<celsius_style_temp[si::kelvin]>(30.0), celsius_style_abs_origin)
                .quantity_from(celsius_style_abs_origin) == delta<celsius_style_temp[si::kelvin]>(200.0));

// Same with °C input: identical result confirms Δ°C == ΔK in bound comparisons.
static_assert(qp_abs(delta<celsius_style_temp[si::degree_Celsius]>(30.0), celsius_style_abs_origin)
                .quantity_from(celsius_style_abs_origin) == delta<celsius_style_temp[si::kelvin]>(200.0));

// Above 350 Δ°C (= 350 ΔK): 400 K → clamped to 350 K.
static_assert(qp_abs(delta<celsius_style_temp[si::kelvin]>(400.0), celsius_style_abs_origin)
                .quantity_from(celsius_style_abs_origin) == delta<celsius_style_temp[si::kelvin]>(350.0));

// .in(K) is a unit-label change only: stored value is unchanged, no 273 shift.
consteval bool celsius_style_in_kelvin()
{
  auto pt = qp_abs(delta<celsius_style_temp[si::degree_Celsius]>(270.0), celsius_style_abs_origin);
  auto in_K = pt.in(si::kelvin);
  return in_K.quantity_from(celsius_style_abs_origin).numerical_value_in(si::kelvin) == 270.0;
}
static_assert(celsius_style_in_kelvin());

// --- The 273 K shift lives in the origin, not the unit ---
//
// The SAME numerical value "30 K":
//   • from celsius_style_abs_origin → absolute = 30 K → below 200 → CLAMPED to 200 K
//   • from celsius_style_ice_origin → absolute = 273+30 = 303 K → within [200, 350] → VALID

// 30 K from abs_origin — below min — clamped (shown above; repeated for side-by-side clarity).
static_assert(qp_abs(delta<celsius_style_temp[si::kelvin]>(30.0), celsius_style_abs_origin)
                .quantity_from(celsius_style_abs_origin) == delta<celsius_style_temp[si::kelvin]>(200.0));

// 30 K from ice_origin — absolute 303 K — within [200, 350] — NOT clamped.
static_assert(qp_ice(delta<celsius_style_temp[si::kelvin]>(30.0), celsius_style_ice_origin)
                .quantity_from(celsius_style_ice_origin) == delta<celsius_style_temp[si::kelvin]>(30.0));

// Above max from ice_origin: 100 K → absolute 373 K > 350 → clamp → 350-273 = 77 K.
static_assert(qp_ice(delta<celsius_style_temp[si::kelvin]>(100.0), celsius_style_ice_origin)
                .quantity_from(celsius_style_ice_origin) == delta<celsius_style_temp[si::kelvin]>(77.0));

// Below min from ice_origin: -300 K → absolute -27 K < 200 → clamp → 200-273 = -73 K.
static_assert(qp_ice(delta<celsius_style_temp[si::kelvin]>(-300.0), celsius_style_ice_origin)
                .quantity_from(celsius_style_ice_origin) == delta<celsius_style_temp[si::kelvin]>(-73.0));

// ============================================================================
// Conversions between different origins (with offsets)
// ============================================================================

// Test conversion between absolute_zero and ice_point origins
// ice_point is at 273.15K above absolute_zero
consteval bool convert_between_offset_origins()
{
  // Create a bounded point at temp_origin: 300K
  auto pt_temp = qp_temp(delta<bounded_temperature[si::kelvin]>(300.0), temp_origin);

  // Convert to quantity_point with si::absolute_zero origin
  // This requires going through the quantity: quantity_from gives us the displacement
  const auto q = pt_temp.quantity_from(temp_origin);
  auto pt_abs_zero = si::absolute_zero + q;

  // Now convert to ice_point (which is 273.15K above absolute_zero)
  const auto q_from_ice = pt_abs_zero.quantity_from(si::ice_point);

  // 300K from absolute_zero = (300 - 273.15)K from ice_point = 26.85K
  // Use approximate comparison due to floating point precision
  constexpr auto expected = 300.0 - 273.15;
  const auto actual = q_from_ice.numerical_value_in(si::kelvin);
  return (actual >= expected - 0.0001) && (actual <= expected + 0.0001);
}
static_assert(convert_between_offset_origins());

// ============================================================================
// Scenario 1: Only absolute bounds (absolute origin WITH bounds).
//
// A bounds specialization lives directly on an absolute_point_origin.
// Relative origins that build on top carry no own bounds and simply inherit
// the absolute origin's enforcement (cumulative offset translated away first).
//
// Examples below:
//   altitude_msl / altitude_agl     — two independent absolute origins
//   prime_meridian / warsaw / kyiv  — absolute bounds + inheriting relative origins
//   body_temp_origin / room_temp    — two independent absolute origins (°C)
//   check_origin                    — absolute origin with check_in_range policy
// ============================================================================

// ============================================================================
// Altitude: two independent absolute origins (MSL and AGL).
// The translation between them is terrain-dependent and must be done at runtime
// by the user; the library cannot express it as a relative_point_origin offset.
// Each origin enforces its own physical or operational bounds independently.
// ============================================================================

using qp_altitude_msl = quantity_point<bounded_altitude[m], altitude_msl, double>;
using qp_altitude_agl = quantity_point<bounded_altitude[m], altitude_agl, double>;

// MSL: value within bounds.
static_assert(qp_altitude_msl(1000.0 * m, altitude_msl).quantity_from(altitude_msl) == 1000.0 * m);

// MSL: above max clamps to 12000m.
static_assert(qp_altitude_msl(15000.0 * m, altitude_msl).quantity_from(altitude_msl) == 12000.0 * m);

// AGL: value within bounds.
static_assert(qp_altitude_agl(200.0 * m, altitude_agl).quantity_from(altitude_agl) == 200.0 * m);

// AGL: above max clamps to 500m.
static_assert(qp_altitude_agl(700.0 * m, altitude_agl).quantity_from(altitude_agl) == 500.0 * m);

// AGL: below min (negative AGL is underground) clamps to 0m.
static_assert(qp_altitude_agl(-10.0 * m, altitude_agl).quantity_from(altitude_agl) == 0.0 * m);

// ============================================================================
// Longitude: relative origin (Warsaw) inherits absolute physical bounds.
// No user-specific bounds on warsaw_meridian — the absolute origin's [-180°,180°]
// bounds are always enforced (translated by the +21° offset).
// Max allowed from Warsaw = 180° - 21° = 159°.
// Min allowed from Warsaw = -180° - 21° = -201°.
// ============================================================================

using qp_prime = quantity_point<geo_longitude[deg], prime_meridian, double>;
using qp_warsaw = quantity_point<geo_longitude[deg], warsaw_meridian, double>;
using qp_kyiv = quantity_point<geo_longitude[deg], kyiv_meridian, double>;

// Prime meridian: value within bounds.
static_assert(qp_prime(90.0 * deg, prime_meridian).quantity_from(prime_meridian) == 90.0 * deg);

// Prime meridian: above 180° clamps to 180°.
static_assert(qp_prime(200.0 * deg, prime_meridian).quantity_from(prime_meridian) == 180.0 * deg);

// Warsaw: value within absolute bounds (21° + 100° = 121°, within [-180°, 180°]).
static_assert(qp_warsaw(100.0 * deg, warsaw_meridian).quantity_from(warsaw_meridian) == 100.0 * deg);

// Warsaw: 200° east of Warsaw = 221° from prime meridian → exceeds 180° → clamps.
// After clamp in absolute frame: 180°. Back in Warsaw frame: 180° - 21° = 159°.
static_assert(qp_warsaw(200.0 * deg, warsaw_meridian).quantity_from(warsaw_meridian) == 159.0 * deg);

// Warsaw: -200° west of Warsaw = -179° from prime meridian → within [-180°, 180°] → no clamp.
static_assert(qp_warsaw(-170.0 * deg, warsaw_meridian).quantity_from(warsaw_meridian) == -170.0 * deg);

// Warsaw: -165° west of Warsaw = -144° from prime meridian → no clamp; straightforward.
static_assert(qp_warsaw(-165.0 * deg, warsaw_meridian).quantity_from(warsaw_meridian) == -165.0 * deg);

// Kyiv relative origin: same physical bounds check. 170° east of Kyiv = 200° → clamps.
// 200° from prime → clamps to 180° → 180° - 30° = 150° from Kyiv.
static_assert(qp_kyiv(170.0 * deg, kyiv_meridian).quantity_from(kyiv_meridian) == 150.0 * deg);

// ============================================================================
// point_for() cross-origin bounds: body vs room temperature.
// A body temperature (37°C) is valid for body_temp_origin,
// but out-of-range for room_temp_origin [15°C, 30°C] — should clamp on conversion.
// Since these are independent absolute origins there is no point_for() between them;
// the user must do the conversion explicitly via quantity_from.
// The following tests validate that each absolute origin clamps independently.
// ============================================================================

using qp_body = quantity_point<clinical_temperature[si::degree_Celsius], body_temp_origin, double>;
using qp_room = quantity_point<clinical_temperature[si::degree_Celsius], room_temp_origin, double>;

// Body temperature 37°C: within [35, 42] → unchanged.
static_assert(qp_body(delta<clinical_temperature[si::degree_Celsius]>(37.0), body_temp_origin)
                .quantity_from(body_temp_origin) == delta<clinical_temperature[si::degree_Celsius]>(37.0));

// Body temperature 44°C: above max 42°C → clamps to 42°C.
static_assert(qp_body(delta<clinical_temperature[si::degree_Celsius]>(44.0), body_temp_origin)
                .quantity_from(body_temp_origin) == delta<clinical_temperature[si::degree_Celsius]>(42.0));

// Room temperature 20°C: within [15, 30] → unchanged.
static_assert(qp_room(delta<clinical_temperature[si::degree_Celsius]>(20.0), room_temp_origin)
                .quantity_from(room_temp_origin) == delta<clinical_temperature[si::degree_Celsius]>(20.0));

// Room temperature 37°C (a body temperature entered as room): exceeds [15, 30] → clamps to 30°C.
static_assert(qp_room(delta<clinical_temperature[si::degree_Celsius]>(37.0), room_temp_origin)
                .quantity_from(room_temp_origin) == delta<clinical_temperature[si::degree_Celsius]>(30.0));

// ============================================================================
// Scenario 1 (continued): check_in_range policy.
// Same abs[bounds] topology; behaviour on out-of-range values is a contract
// violation (MP_UNITS_PRECONDITION) rather than a silent clamp/wrap/reflect.
// Only in-range paths can be tested at compile time.
// ============================================================================

// Origin with check_in_range policy
QUANTITY_SPEC(test_angle_check, isq::angular_measure);

inline constexpr struct check_origin final :
    absolute_point_origin<test_angle_check, check_in_range{-90 * deg, 90 * deg}> {
} check_origin;

using qp_check = quantity_point<test_angle_check[deg], check_origin, double>;

// Values within bounds should work fine
static_assert(qp_check(45.0 * deg, check_origin).quantity_from(check_origin) == 45.0 * deg);
static_assert(qp_check(0.0 * deg, check_origin).quantity_from(check_origin) == 0.0 * deg);
static_assert(qp_check(90.0 * deg, check_origin).quantity_from(check_origin) == 90.0 * deg);
static_assert(qp_check(-90.0 * deg, check_origin).quantity_from(check_origin) == -90.0 * deg);

// Arithmetic within bounds
consteval bool check_arithmetic_within_bounds()
{
  auto pt = qp_check(45.0 * deg, check_origin);
  pt += 30.0 * deg;  // Result: 75°, within bounds
  return pt.quantity_from(check_origin) == 75.0 * deg;
}
static_assert(check_arithmetic_within_bounds());

consteval bool check_increment_within_bounds()
{
  using qp_check_int = quantity_point<test_angle_check[deg], check_origin, int>;
  auto pt = qp_check_int(45 * deg, check_origin);
  ++pt;  // Result: 46°, within bounds
  return pt.quantity_from(check_origin) == 46 * deg;
}
static_assert(check_increment_within_bounds());

// Note: Out-of-bounds cases for check_in_range with plain rep cannot be tested at
// compile time since they use MP_UNITS_PRECONDITION (may be disabled in release builds).

// ============================================================================
// Scenario 2: Only relative bounds (parent absolute origin has NO bounds).
// A relative origin defines local operational constraints; the parent origin
// has no universal physical invariant.
// ============================================================================

QUANTITY_SPEC(mission_range, isq::length);

inline constexpr struct home_base final : absolute_point_origin<mission_range> {
} home_base;  // no bounds — the home base is an unconstrained reference

// relative origin: at 100m from home_base, with its own [-20m, 20m] operational bounds.
inline constexpr struct patrol_radius final :
    relative_point_origin<home_base + 100.0 * m, clamp_to_range{-20.0 * m, 20.0 * m}> {
} patrol_radius;

using qp_patrol = quantity_point<mission_range[m], patrol_radius, double>;

// Within patrol bounds: unchanged.
static_assert(qp_patrol(10.0 * m, patrol_radius).quantity_from(patrol_radius) == 10.0 * m);

// Exceeds patrol max (20m): clamps to 20m.
static_assert(qp_patrol(30.0 * m, patrol_radius).quantity_from(patrol_radius) == 20.0 * m);

// Below patrol min (-20m): clamps to -20m.
static_assert(qp_patrol(-25.0 * m, patrol_radius).quantity_from(patrol_radius) == -20.0 * m);

// ============================================================================
// Scenario 3: Absolute bounds AND relative bounds (both enforced, relative is tighter).
// PO = rel[±5m], parent = abs[±10m].  Only the tighter relative bounds fire at runtime.
// A compile-time check verifies relative ⊆ absolute.
// ============================================================================

QUANTITY_SPEC(nested_height, isq::height);

inline constexpr struct ground_abs final :
    absolute_point_origin<nested_height,
                          mp_units::clamp_to_range{-10.0 * mp_units::si::metre, 10.0 * mp_units::si::metre}> {
} ground_abs;  // bounds: [-10m, 10m]

inline constexpr struct platform final :
    mp_units::relative_point_origin<mp_units::quantity_point<nested_height[mp_units::si::metre], ground_abs, double>{
                                      0.0 * mp_units::si::metre, ground_abs},
                                    mp_units::clamp_to_range{-5.0 * mp_units::si::metre, 5.0 * mp_units::si::metre}> {
} platform;

using qp_platform = quantity_point<nested_height[m], platform, double>;

// Within the relative bounds: unchanged.
static_assert(qp_platform(3.0 * m, platform).quantity_from(platform) == 3.0 * m);

// Exceeds relative max (5m) but within absolute (10m): clamps to relative max 5m.
static_assert(qp_platform(7.0 * m, platform).quantity_from(platform) == 5.0 * m);

// ============================================================================
// Scenario 4: abs[bounds] ← rel1[bounds] ← rel2[no-bounds].
// rel2 has no own bounds; it inherits rel1's bounds (translated).
// rel1 is at offset +5m from abs, with bounds [-3m, +3m].
// rel2 is at offset +1m from rel1 (i.e., +6m from abs), with no own bounds.
// Absolute frame: rel2 value v_abs = v_rel2 + 1 (rel2→rel1) + 5 (rel1→abs)
//                               = v_rel2 + 6.
// rel1 bounds in absolute frame: [-3+5, +3+5] = [+2, +8].
// rel1 bounds in rel2's frame: [+2-6, +8-6] = [-4, +2].
// So a value of v_rel2 in [-4, +2] passes; outside is clamped.
// ============================================================================

QUANTITY_SPEC(chain_length, isq::length);

inline constexpr struct chain_abs final : absolute_point_origin<chain_length> {
} chain_abs;  // bounds: not used directly; testing via rel1 bounds

// chain_rel1: at +5m from chain_abs, with bounds [-3m, +3m].
inline constexpr struct chain_rel1 final :
    mp_units::relative_point_origin<mp_units::quantity_point<chain_length[mp_units::si::metre], chain_abs, double>{
                                      5.0 * mp_units::si::metre, chain_abs},
                                    mp_units::clamp_to_range{-3.0 * mp_units::si::metre, 3.0 * mp_units::si::metre}> {
} chain_rel1;

// chain_rel2: at +1m from chain_rel1 (i.e., +6m from chain_abs), with NO bounds.
inline constexpr struct chain_rel2 final :
    mp_units::relative_point_origin<mp_units::quantity_point<chain_length[mp_units::si::metre], chain_rel1, double>{
      1.0 * mp_units::si::metre, chain_rel1}> {
} chain_rel2;

using qp_chain2 = quantity_point<chain_length[m], chain_rel2, double>;

// In rel2 frame: 0m → abs frame: 0+1+5=6m → rel1 frame: 6-5=1m → within [-3,3] → pass.
// No clamp: 0m from rel2 stays 0m.
static_assert(qp_chain2(0.0 * m, chain_rel2).quantity_from(chain_rel2) == 0.0 * m);

// In rel2 frame: -3m → abs frame: -3+6=3m → rel1 frame: 3-5=-2m → within [-3,3] → pass.
static_assert(qp_chain2(-3.0 * m, chain_rel2).quantity_from(chain_rel2) == -3.0 * m);

// In rel2 frame: +3m → abs frame: 3+6=9m → rel1 frame: 9-5=4m → exceeds max 3m → clamp to 3m (rel1).
// Back to rel2 frame: 3 - 1 = 2m (offset is 1m from rel1 to rel2; rel1_frame_val - 0 = 3, rel2 = rel1_val - 1 = 2).
// rel1 clamps to 3m (in rel1 frame). rel2 val = clamped_rel1 - offset = 3 - 1 = 2.
static_assert(qp_chain2(3.0 * m, chain_rel2).quantity_from(chain_rel2) == 2.0 * m);

// In rel2 frame: -5m → abs frame: -5+6=1m → rel1 frame: 1-5=-4m → below min -3m → clamp to -3m (rel1).
// Back to rel2: -3 - 1 = -4m.
static_assert(qp_chain2(-5.0 * m, chain_rel2).quantity_from(chain_rel2) == -4.0 * m);

// ============================================================================
// Scenario 5: abs[no-bounds] ← rel1[bounds] ← rel2[bounds].
// rel1 is at +10m from abs (unbounded), with bounds [-5m, +5m].
// rel2 is at +2m from rel1, with its own tighter bounds [-1m, +1m].
// Since rel2 HAS bounds, they are enforced (the tighter ones).
// A compile-time check verifies rel2 bounds ⊆ rel1 bounds (in rel1's frame).
// rel2 bounds in rel1 frame: [-1+2, +1+2] = [+1, +3] ⊆ [-5, +5] → OK.
// ============================================================================

QUANTITY_SPEC(nested2_length, isq::length);

inline constexpr struct nested2_abs final : absolute_point_origin<nested2_length> {
} nested2_abs;  // no bounds

// nested2_rel1: at +10m from nested2_abs, bounds [-5m, +5m].
inline constexpr struct nested2_rel1 final :
    mp_units::relative_point_origin<mp_units::quantity_point<nested2_length[mp_units::si::metre], nested2_abs, double>{
                                      10.0 * mp_units::si::metre, nested2_abs},
                                    mp_units::clamp_to_range{-5.0 * mp_units::si::metre, 5.0 * mp_units::si::metre}> {
} nested2_rel1;

// nested2_rel2: at +2m from nested2_rel1, bounds [-1m, +1m].
inline constexpr struct nested2_rel2 final :
    mp_units::relative_point_origin<mp_units::quantity_point<nested2_length[mp_units::si::metre], nested2_rel1, double>{
                                      2.0 * mp_units::si::metre, nested2_rel1},
                                    mp_units::clamp_to_range{-1.0 * mp_units::si::metre, 1.0 * mp_units::si::metre}> {
} nested2_rel2;

using qp_nested2 = quantity_point<nested2_length[m], nested2_rel2, double>;

// Within rel2 bounds [-1, +1]: unchanged.
static_assert(qp_nested2(0.5 * m, nested2_rel2).quantity_from(nested2_rel2) == 0.5 * m);

// Exceeds rel2 max (+1m): clamp to +1m (rel2's own bounds fire, not rel1's).
static_assert(qp_nested2(3.0 * m, nested2_rel2).quantity_from(nested2_rel2) == 1.0 * m);

// Below rel2 min (-1m): clamp to -1m.
static_assert(qp_nested2(-2.0 * m, nested2_rel2).quantity_from(nested2_rel2) == -1.0 * m);

// ============================================================================
// Scenario: time-of-day (wrap_to_range with isq::duration, [0 h, 24 h))
// Models a clock/timestamp that wraps cyclically over a 24-hour day.
// This tests wrap_to_range with a duration quantity spec, separate from the
// angular tests above.
// ============================================================================

QUANTITY_SPEC(time_of_day_qs, isq::duration);

inline constexpr struct midnight final : absolute_point_origin<time_of_day_qs, wrap_to_range{0 * h, 24 * h}> {
} midnight;

using time_of_day = quantity_point<time_of_day_qs[s], midnight, double>;

// Within [0, 86400): unchanged.
static_assert(time_of_day(0.0 * s, midnight).quantity_from(midnight) == 0.0 * s);
static_assert(time_of_day(3600.0 * s, midnight).quantity_from(midnight) == 3600.0 * s);    // 01:00
static_assert(time_of_day(43200.0 * s, midnight).quantity_from(midnight) == 43200.0 * s);  // 12:00
static_assert(time_of_day(86399.0 * s, midnight).quantity_from(midnight) == 86399.0 * s);  // 23:59:59

// Overflow past midnight wraps back to early morning.
static_assert(time_of_day(86400.0 * s, midnight).quantity_from(midnight) == 0.0 * s);     // 24:00 -> 00:00
static_assert(time_of_day(90000.0 * s, midnight).quantity_from(midnight) == 3600.0 * s);  // 25:00 -> 01:00

// Negative values (before midnight) wrap to previous day.
static_assert(time_of_day(-3600.0 * s, midnight).quantity_from(midnight) == 82800.0 * s);  // -1 h -> 23:00

// Arithmetic: adding a duration that crosses midnight wraps correctly.
consteval bool time_of_day_wrap_arithmetic()
{
  auto tod = time_of_day(82800.0 * s, midnight);  // 23:00
  tod += 7200.0 * s;                              // +2 h -> 01:00 next day
  return tod.quantity_from(midnight) == 3600.0 * s;
}
static_assert(time_of_day_wrap_arithmetic());

// ---- Same tests using hours as the input unit ------------------------------
// Use time_of_day_qs[h] so the quantity spec matches and unit conversion
// to the stored unit (s) is handled automatically.

// 0 h = 0 s: at lower boundary.
static_assert(time_of_day(0.0 * time_of_day_qs[h], midnight).quantity_from(midnight) == 0.0 * s);

// 1 h = 3600 s: well inside range.
static_assert(time_of_day(1.0 * time_of_day_qs[h], midnight).quantity_from(midnight) == 3600.0 * s);

// 24 h = 86400 s: exactly at the upper (exclusive) boundary → wraps to 0.
static_assert(time_of_day(24.0 * time_of_day_qs[h], midnight).quantity_from(midnight) == 0.0 * s);

// 25 h = 90000 s: one hour past midnight → wraps to 1 h = 3600 s.
static_assert(time_of_day(25.0 * time_of_day_qs[h], midnight).quantity_from(midnight) == 3600.0 * s);

// -1 h = -3600 s: one hour before midnight → wraps to 23 h = 82800 s.
static_assert(time_of_day(-1.0 * time_of_day_qs[h], midnight).quantity_from(midnight) == 82800.0 * s);

// ---- Same tests using minutes as the input unit ----------------------------

// 60 min = 3600 s = 1 h.
static_assert(time_of_day(60.0 * time_of_day_qs[min], midnight).quantity_from(midnight) == 3600.0 * s);

// 1440 min = 86400 s = 24 h → wraps to 0.
static_assert(time_of_day(1440.0 * time_of_day_qs[min], midnight).quantity_from(midnight) == 0.0 * s);

// -60 min = -3600 s → wraps to 82800 s = 23 h.
static_assert(time_of_day(-60.0 * time_of_day_qs[min], midnight).quantity_from(midnight) == 82800.0 * s);

// 90 min = 5400 s = 1 h 30 min: inside range.
static_assert(time_of_day(90.0 * time_of_day_qs[min], midnight).quantity_from(midnight) == 5400.0 * s);

// ---- Same tests using milliseconds as the input unit -----------------------

// 3'600'000 ms = 3600 s = 1 h.
static_assert(time_of_day(3'600'000.0 * time_of_day_qs[ms], midnight).quantity_from(midnight) == 3600.0 * s);

// 86'400'000 ms = 86400 s → wraps to 0.
static_assert(time_of_day(86'400'000.0 * time_of_day_qs[ms], midnight).quantity_from(midnight) == 0.0 * s);

// 90'000'000 ms = 90000 s = 25 h → wraps to 3600 s.
static_assert(time_of_day(90'000'000.0 * time_of_day_qs[ms], midnight).quantity_from(midnight) == 3600.0 * s);

// -3'600'000 ms = -3600 s → wraps to 82800 s.
static_assert(time_of_day(-3'600'000.0 * time_of_day_qs[ms], midnight).quantity_from(midnight) == 82800.0 * s);

// ---- Cross-unit arithmetic: increment in hours, query in seconds -----------

consteval bool time_of_day_hour_increment()
{
  auto tod = time_of_day(23.0 * time_of_day_qs[h], midnight);  // 23:00 = 82800 s
  tod += 2.0 * time_of_day_qs[h];                              // +2 h → 25:00 = 90000 s → wraps to 3600 s
  return tod.quantity_from(midnight) == 3600.0 * s;
}
static_assert(time_of_day_hour_increment());

consteval bool time_of_day_minute_increment()
{
  auto tod = time_of_day(23.0 * time_of_day_qs[h] + 30.0 * time_of_day_qs[min], midnight);  // 23:30 = 84600 s
  tod += 45.0 * time_of_day_qs[min];  // +45 min → 24:15 = 87300 s → wraps to 900 s
  return tod.quantity_from(midnight) == 900.0 * s;
}
static_assert(time_of_day_minute_increment());

}  // namespace

// ============================================================================
// Static member functions: min(), max(), and std::numeric_limits
// Tests cover: full bounds, half-line min-only, half-line max-only.
// ============================================================================

namespace {

QUANTITY_SPEC(test_halfbounded, isq::height);

// Half-line policy: clamp from below only (has .min, no .max).
template<Quantity Q>
struct clamp_min_only {
  Q min;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < V{min}) return V{min};
    return v;
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD
template<Quantity Q>
clamp_min_only(Q) -> clamp_min_only<Q>;
#endif

// Half-line policy: clamp from above only (has .max, no .min).
template<Quantity Q>
struct clamp_max_only {
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v > V{max}) return V{max};
    return v;
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD
template<Quantity Q>
clamp_max_only(Q) -> clamp_max_only<Q>;
#endif

// min-only origin: value >= 0 m, no upper bound.
inline constexpr struct halfbound_min_origin final : absolute_point_origin<test_halfbounded, clamp_min_only{0 * m}> {
} halfbound_min_origin;

// max-only origin: value <= 12000 m, no lower bound.
inline constexpr struct halfbound_max_origin final :
    absolute_point_origin<test_halfbounded, clamp_max_only{12000 * m}> {
} halfbound_max_origin;

// ---- Full bounds (clamp_origin: [-90 deg, +90 deg]) ------------------------

// quantity_point::min() / max() use the bounds directly.
static_assert(qp_clamp::min().quantity_from(clamp_origin) == -90.0 * deg);
static_assert(qp_clamp::max().quantity_from(clamp_origin) == 90.0 * deg);

// std::numeric_limits delegates to the bounded min/max.
static_assert(std::numeric_limits<qp_clamp>::min().quantity_from(clamp_origin) == -90.0 * deg);
static_assert(std::numeric_limits<qp_clamp>::max().quantity_from(clamp_origin) == 90.0 * deg);

// lowest() delegates to min() because .min exists in bounds.
static_assert(std::numeric_limits<qp_clamp>::lowest().quantity_from(clamp_origin) == -90.0 * deg);

// ---- Bounds declared in another unit or representation ---------------------

// The bounds are declared as whole metres while the point counts kilometres in `double`. The bound
// is reported as the point's own quantity type, so it comes out exactly rather than being truncated
// to the whole kilometre below (which would report a range wider than the declared one).
QUANTITY_SPEC(test_height_coarse, isq::height);
inline constexpr struct coarse_bounds_origin final :
    absolute_point_origin<test_height_coarse, clamp_to_range{1500 * m, 8500 * m}> {
} coarse_bounds_origin;

using qp_coarse = quantity_point<test_height_coarse[km], coarse_bounds_origin, double>;
static_assert(qp_coarse::min().quantity_from(coarse_bounds_origin) == 1.5 * km);
static_assert(qp_coarse::max().quantity_from(coarse_bounds_origin) == 8.5 * km);
static_assert(std::numeric_limits<qp_coarse>::min().quantity_from(coarse_bounds_origin) == 1.5 * km);
static_assert(std::numeric_limits<qp_coarse>::max().quantity_from(coarse_bounds_origin) == 8.5 * km);

// the reported range is exactly the declared one, so it neither admits nor rejects too much
static_assert(qp_coarse::min().quantity_from(coarse_bounds_origin) == 1500 * m);
static_assert(qp_coarse::max().quantity_from(coarse_bounds_origin) == 8500 * m);

// ---- Half-line min-only (halfbound_min_origin: value >= 0 m) ---------------

using qp_hmin = quantity_point<test_halfbounded[m], halfbound_min_origin, double>;

// min() returns the bound; max() falls back to the representation maximum.
static_assert(qp_hmin::min().quantity_from(halfbound_min_origin) == 0.0 * m);
static_assert(qp_hmin::max().quantity_from(halfbound_min_origin).numerical_value_in(m) ==
              std::numeric_limits<double>::max());

// numeric_limits mirrors the above.
static_assert(std::numeric_limits<qp_hmin>::min().quantity_from(halfbound_min_origin) == 0.0 * m);
static_assert(std::numeric_limits<qp_hmin>::max().quantity_from(halfbound_min_origin).numerical_value_in(m) ==
              std::numeric_limits<double>::max());

// lowest() delegates to min() because .min exists in bounds.
static_assert(std::numeric_limits<qp_hmin>::lowest().quantity_from(halfbound_min_origin) == 0.0 * m);

// ---- Half-line max-only (halfbound_max_origin: value <= 12000 m) -----------

using qp_hmax = quantity_point<test_halfbounded[m], halfbound_max_origin, double>;

// min() falls back to the representation minimum (lowest); max() returns the bound.
static_assert(qp_hmax::min().quantity_from(halfbound_max_origin).numerical_value_in(m) ==
              std::numeric_limits<double>::lowest());
static_assert(qp_hmax::max().quantity_from(halfbound_max_origin) == 12000.0 * m);

// numeric_limits mirrors the above.
static_assert(std::numeric_limits<qp_hmax>::min().quantity_from(halfbound_max_origin).numerical_value_in(m) ==
              std::numeric_limits<double>::lowest());
static_assert(std::numeric_limits<qp_hmax>::max().quantity_from(halfbound_max_origin) == 12000.0 * m);

// lowest() has no .min in bounds — falls back to the representation lowest.
static_assert(std::numeric_limits<qp_hmax>::lowest().quantity_from(halfbound_max_origin).numerical_value_in(m) ==
              std::numeric_limits<double>::lowest());

// ============================================================================
// Large-delta tests: deltas much larger than the range width.
//
// wrap_to_range and reflect_in_range are total functions — defined for all
// inputs, not just "one boundary crossing".  These tests verify that adding a
// delta that covers several full periods still yields the correct result.
// (clamp_to_range is excluded: it is idempotent past the boundary — any
// value beyond [min, max] produces the same saturated result regardless of
// how far it overshoots, so there is nothing new to test for large deltas.)
// ============================================================================

// ---- wrap_to_range: range [-180°, 180°), period = 360° ---------------------

// 3 full rotations + 45°: same as 45°.
static_assert(qp_wrap(3 * 360.0 * deg + 45.0 * deg, wrap_origin).quantity_from(wrap_origin) == 45.0 * deg);

// 3 full rotations in the negative direction + 45°: same as 45°.
static_assert(qp_wrap(-3 * 360.0 * deg + 45.0 * deg, wrap_origin).quantity_from(wrap_origin) == 45.0 * deg);

// 2.5 rotations (= 900°): same as 180° → at upper exclusive boundary → wraps to -180°.
static_assert(qp_wrap(2.5 * 360.0 * deg, wrap_origin).quantity_from(wrap_origin) == -180.0 * deg);

// operator+=: start at 90°, add 3 full rotations + 30° (= 1110°) → lands at 120°.
consteval bool wrap_large_delta_assign()
{
  auto pt = qp_wrap(90.0 * deg, wrap_origin);
  pt += 3 * 360.0 * deg + 30.0 * deg;
  return pt.quantity_from(wrap_origin) == 120.0 * deg;
}
static_assert(wrap_large_delta_assign());

// ---- reflect_in_range: range [-90°, 90°], period = 360° -------------------
// The period of reflect_in_range{[a,b]} is 2*(b−a) = 2*180° = 360°.
// After an integer number of periods the point returns to the same position.

// Half a period (180°): the "fold-back" point — lands at 0°.
static_assert(qp_reflect(180.0 * deg, reflect_origin).quantity_from(reflect_origin) == 0.0 * deg);

// 3/4 of a period (270°): bounces off max, then off min → lands at -90°.
static_assert(qp_reflect(270.0 * deg, reflect_origin).quantity_from(reflect_origin) == -90.0 * deg);

// 3 full periods + 45° (= 1125°): same as 45°.
static_assert(qp_reflect(3 * 360.0 * deg + 45.0 * deg, reflect_origin).quantity_from(reflect_origin) == 45.0 * deg);

// Negative direction: -(1 full period + 45°) = -405° → same as -45°.
static_assert(qp_reflect(-1 * 360.0 * deg - 45.0 * deg, reflect_origin).quantity_from(reflect_origin) == -45.0 * deg);

// operator+=: start at 0°, add 1.5 periods (= 540°) → same as adding 180° → 0°.
consteval bool reflect_large_delta_assign()
{
  auto pt = qp_reflect(0.0 * deg, reflect_origin);
  pt += 1.5 * 360.0 * deg;
  return pt.quantity_from(reflect_origin) == 0.0 * deg;
}
static_assert(reflect_large_delta_assign());

// ---- wrap_to_range: time-of-day [0 s, 86400 s), period = 86400 s ----------

// 3 full days + 1 hour (= 262800 s): wraps to 3600 s = 01:00.
static_assert(time_of_day(3 * 86400.0 * s + 3600.0 * s, midnight).quantity_from(midnight) == 3600.0 * s);

// Negative: 3 full days before midnight + 1 hour before end = -3*86400+82800 = -176400 s → 82800 s = 23:00.
static_assert(time_of_day(-3 * 86400.0 * s + 82800.0 * s, midnight).quantity_from(midnight) == 82800.0 * s);

// operator+=: start at 23:00 (82800 s), add 3 days + 2 hours (= 266400 s) → lands at 01:00 (3600 s).
consteval bool time_of_day_multiday_assign()
{
  auto tod = time_of_day(82800.0 * s, midnight);  // 23:00
  tod += 3 * 86400.0 * s + 2 * 3600.0 * s;        // +3 days 2 hours
  return tod.quantity_from(midnight) == 3600.0 * s;
}
static_assert(time_of_day_multiday_assign());

// ============================================================================
// Automatic non-negative bounds for natural_point_origin<QS>
// ============================================================================
// Non-negative base quantities automatically receive check_non_negative bounds
// via the natural_origin_base conditional inheritance in quantity_point.h.

// isq::length, isq::mass, isq::duration are tagged non_negative → automatic bounds.
static_assert(mp_units::detail::HasQuantityBounds<mp_units::natural_point_origin_<mp_units::isq::length>>);
static_assert(mp_units::detail::HasQuantityBounds<mp_units::natural_point_origin_<mp_units::isq::mass>>);
static_assert(mp_units::detail::HasQuantityBounds<mp_units::natural_point_origin_<mp_units::isq::duration>>);

// isq::angular_measure has no non_negative tag → no automatic bounds.
static_assert(!mp_units::detail::HasQuantityBounds<mp_units::natural_point_origin_<mp_units::isq::angular_measure>>);

// The automatic policy type is check_non_negative.
static_assert(
  std::is_same_v<std::remove_cvref_t<decltype(mp_units::natural_point_origin_<mp_units::isq::length>::_bounds_)>,
                 mp_units::check_non_negative>);

// A non-negative value passes through the policy unchanged.
static_assert(mp_units::natural_point_origin_<mp_units::isq::length>::_bounds_(5.0 * m));
static_assert(mp_units::natural_point_origin_<mp_units::isq::length>::_bounds_(0.0 * m));
static_assert(!mp_units::natural_point_origin_<mp_units::isq::length>::_bounds_(-1.0 * m));

// ============================================================================
// Non-negative natural_point_origin: bounds inherited by relative origins.
//
// When QS is tagged non_negative, natural_point_origin<QS> receives
// check_non_negative bounds automatically.  A relative_point_origin with no own
// bounds inherits this constraint via enforce_bounds — values may be negative
// relative to the offset origin as long as the absolute value stays ≥ 0.
//
// Example: average_height_origin at +1700 m above the ground floor.
//   rel = -1500 m → abs = 200 m  (≥ 0) → valid, unchanged.
//   rel = -1700 m → abs =   0 m         → valid (boundary).
//   rel = -1701 m → abs =  -1 m  (< 0) → check_non_negative fires.
//
// The violation case cannot be verified at compile time (check_non_negative uses
// MP_UNITS_PRECONDITION, which may be a no-op in release builds).  See the runtime
// test for violation coverage with constrained<double, throw_policy> rep.
// ============================================================================

QUANTITY_SPEC(avg_height_qs, isq::height);

// natural_point_origin gets check_non_negative automatically (isq::height is non_negative).
static_assert(mp_units::detail::HasQuantityBounds<natural_point_origin_<avg_height_qs>>);

// Relative origin at +1700 m — no own bounds, inherits check_non_negative.
inline constexpr struct average_height_origin final :
    relative_point_origin<natural_point_origin<avg_height_qs> + 1700.0 * m> {
} average_height_origin;

// Inherits bounds from ancestor but has none of its own.
static_assert(!mp_units::detail::HasQuantityBounds<decltype(average_height_origin)>);
static_assert(mp_units::detail::any_ancestor_has_bounds(average_height_origin));

using qp_avg_height = quantity_point<avg_height_qs[m], average_height_origin, double>;

// Values that keep absolute height ≥ 0 pass through unchanged.
static_assert(qp_avg_height(0.0 * m, average_height_origin).quantity_from(average_height_origin) == 0.0 * m);
static_assert(qp_avg_height(500.0 * m, average_height_origin).quantity_from(average_height_origin) == 500.0 * m);
static_assert(qp_avg_height(-1500.0 * m, average_height_origin).quantity_from(average_height_origin) == -1500.0 * m);
// Boundary: −1700 m relative = 0 m absolute (natural ground level).
static_assert(qp_avg_height(-1700.0 * m, average_height_origin).quantity_from(average_height_origin) == -1700.0 * m);


// ============================================================================
// Asking the bounds: is_within_bounds() and try_make().
//
// A value read from outside (EEPROM, a sensor, a bus) is validated against the
// origin's own bounds, and a violation comes back as a value instead of reaching
// the representation's error policy, so the bounds are not repeated in user code
// and nothing throws or terminates.
// ============================================================================

constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

// The doubles adjacent to 10 and -10, on the outside.
constexpr double just_above_10 = 10.000000000000002;
constexpr double just_below_minus_10 = -10.000000000000002;
static_assert(just_above_10 > 10.0 && just_below_minus_10 < -10.0);

// ----------------------------------------------------------------------------
// Absolute origin.
// ----------------------------------------------------------------------------

inline constexpr struct trim_zero final :
    absolute_point_origin<isq::angular_measure, check_in_range{-10. * deg, 10. * deg}> {
} trim_zero;

using trim_angle = quantity_point<si::degree, trim_zero, constrained<double, terminate_policy>>;

static_assert(is_within_bounds<trim_angle>(0. * deg, trim_zero));
static_assert(is_within_bounds<trim_angle>(10. * deg, trim_zero));
static_assert(is_within_bounds<trim_angle>(-10. * deg, trim_zero));
static_assert(!is_within_bounds<trim_angle>(just_above_10 * deg, trim_zero));
static_assert(!is_within_bounds<trim_angle>(just_below_minus_10 * deg, trim_zero));
static_assert(!is_within_bounds<trim_angle>(not_a_number * deg, trim_zero));

// The value is converted to the point's quantity type first, as the constructor does.
static_assert(is_within_bounds<trim_angle>(0.17 * rad, trim_zero));   // 9.74°
static_assert(!is_within_bounds<trim_angle>(0.18 * rad, trim_zero));  // 10.31°

// A plain double representation: a rejected value never reaches MP_UNITS_PRECONDITION.
using trim_angle_plain = quantity_point<si::degree, trim_zero, double>;
static_assert(!is_within_bounds<trim_angle_plain>(11. * deg, trim_zero));

// Default origin: the natural origin of a non-negative quantity carries check_non_negative,
// which has a lower bound only.
using bav_height = quantity_point<isq::height[m]>;
static_assert(is_within_bounds<bav_height>(0. * m));
static_assert(is_within_bounds<bav_height>(1e300 * m));
static_assert(!is_within_bounds<bav_height>(-1e-300 * m));
static_assert(!is_within_bounds<bav_height>(not_a_number * m));

// No bounds anywhere: nothing can be violated.
static_assert(is_within_bounds<quantity_point<mission_range[m], home_base>>(1e300 * m, home_base));

// ----------------------------------------------------------------------------
// Offered only where the enforced policy is a checking one.
// ----------------------------------------------------------------------------

template<typename QP, auto PO, typename Q>
concept can_query_bounds = requires(Q q) { is_within_bounds<QP>(q, PO); };

static_assert(can_query_bounds<trim_angle, trim_zero, quantity<deg, double>>);
static_assert(!can_query_bounds<quantity_point<test_angle_clamp[deg], clamp_origin>, clamp_origin,
                                quantity<test_angle_clamp[deg], double>>);
static_assert(!can_query_bounds<quantity_point<test_angle_wrap[deg], wrap_origin>, wrap_origin,
                                quantity<test_angle_wrap[deg], double>>);
static_assert(!can_query_bounds<quantity_point<test_angle_reflect[deg], reflect_origin>, reflect_origin,
                                quantity<test_angle_reflect[deg], double>>);

// A checking origin under an adjusting one: construction applies only the child's policy (the
// nesting check keeps it inside the parent's range), so the query is offered and tests just that.
QUANTITY_SPEC(bav_mixed, isq::length);

inline constexpr struct bav_mixed_abs final : absolute_point_origin<bav_mixed, clamp_to_range{-10. * m, 10. * m}> {
} bav_mixed_abs;

inline constexpr struct bav_mixed_rel final :
    relative_point_origin<quantity_point<bav_mixed[m], bav_mixed_abs, double>{0. * m, bav_mixed_abs},
                          check_in_range{-5. * m, 5. * m}> {
} bav_mixed_rel;

static_assert(can_query_bounds<quantity_point<bav_mixed[m], bav_mixed_rel>, bav_mixed_rel, quantity<m, double>>);
static_assert(is_within_bounds<quantity_point<bav_mixed[m], bav_mixed_rel>>(5. * m, bav_mixed_rel));
static_assert(!is_within_bounds<quantity_point<bav_mixed[m], bav_mixed_rel>>(6. * m, bav_mixed_rel));

// An adjusting origin under a checking one: construction clamps, so the query is not offered.
QUANTITY_SPEC(bav_mixed2, isq::length);

inline constexpr struct bav_mixed2_abs final : absolute_point_origin<bav_mixed2, check_in_range{-10. * m, 10. * m}> {
} bav_mixed2_abs;

inline constexpr struct bav_mixed2_rel final :
    relative_point_origin<quantity_point<bav_mixed2[m], bav_mixed2_abs, double>{0. * m, bav_mixed2_abs},
                          clamp_to_range{-5. * m, 5. * m}> {
} bav_mixed2_rel;

static_assert(!can_query_bounds<quantity_point<bav_mixed2[m], bav_mixed2_rel>, bav_mixed2_rel, quantity<m, double>>);

// ----------------------------------------------------------------------------
// Relative origins: the query tests the bounds construction enforces. The nesting check
// keeps those inside the bounds of every origin above, so they are the only ones that matter.
// ----------------------------------------------------------------------------

// At +4° from trim_zero with [-5°, 5°] of its own, i.e. [-1°, 9°] in the parent's frame.
inline constexpr struct trim_centre final :
    relative_point_origin<trim_angle_plain{4. * deg, trim_zero}, check_in_range{-5. * deg, 5. * deg}> {
} trim_centre;

using trim_offset = quantity_point<si::degree, trim_centre, constrained<double, terminate_policy>>;

static_assert(is_within_bounds<trim_offset>(0. * deg, trim_centre));
static_assert(is_within_bounds<trim_offset>(5. * deg, trim_centre));
static_assert(is_within_bounds<trim_offset>(-5. * deg, trim_centre));
static_assert(!is_within_bounds<trim_offset>(5.000000000000001 * deg, trim_centre));
static_assert(!is_within_bounds<trim_offset>(-5.000000000000001 * deg, trim_centre));
static_assert(!is_within_bounds<trim_offset>(not_a_number * deg, trim_centre));

// An origin without bounds of its own is checked against its nearest bounded ancestor,
// translated exactly as enforce_bounds translates it: here [-6°, 4°] in its own frame.
inline constexpr struct trim_centre_plus_one final : relative_point_origin<trim_centre + 1. * deg> {
} trim_centre_plus_one;

using trim_offset2 = quantity_point<si::degree, trim_centre_plus_one, constrained<double, terminate_policy>>;

static_assert(is_within_bounds<trim_offset2>(4. * deg, trim_centre_plus_one));
static_assert(is_within_bounds<trim_offset2>(-6. * deg, trim_centre_plus_one));
static_assert(!is_within_bounds<trim_offset2>(4.5 * deg, trim_centre_plus_one));
static_assert(!is_within_bounds<trim_offset2>(-6.5 * deg, trim_centre_plus_one));
// NaN through a translated origin: see the runtime test (clang refuses NaN arithmetic in constant evaluation).

// Comparing points of different origins translates by the offset between them, a plain `double` here.
// `constrained` has no arithmetic with a plain scalar, so these two do not compare, which the constraint
// of `==` and `<=>` has to say rather than their bodies. With a plain representation they compare.
template<typename QP1, typename QP2>
concept can_compare_points = requires(const QP1& lhs, const QP2& rhs) {
  lhs == rhs;
  lhs <=> rhs;
};

static_assert(can_compare_points<trim_angle, trim_angle>);
static_assert(!can_compare_points<trim_offset, trim_angle>);
static_assert(!can_compare_points<trim_angle, trim_offset>);
static_assert(can_compare_points<trim_angle_plain, quantity_point<si::degree, trim_centre, double>>);
static_assert(trim_angle_plain{4. * deg, trim_zero} ==
              quantity_point<si::degree, trim_centre, double>{0. * deg, trim_centre});

// A checking policy reached through an ancestor never changes the value: translating it into the
// ancestor's frame and back would turn 0.1 into 0.099999999999909051 here.
static_assert(qp_avg_height(0.1 * m, average_height_origin).quantity_from(average_height_origin) == 0.1 * m);

// A wrap_to_range origin excludes its max, so a child below it must stay strictly under that value:
// [-10°, 349°] at +10° ends at 359°. A child ending at 350° (360°, wrapped to 0°) is rejected.
QUANTITY_SPEC(bav_azimuth, isq::angular_measure);

inline constexpr struct bav_north final : absolute_point_origin<bav_azimuth, wrap_to_range{0. * deg, 360. * deg}> {
} bav_north;

inline constexpr struct bav_sensor final :
    relative_point_origin<quantity_point<bav_azimuth[deg], bav_north, double>{10. * deg, bav_north},
                          check_in_range{-10. * deg, 349. * deg}> {
} bav_sensor;

static_assert(is_within_bounds<quantity_point<bav_azimuth[deg], bav_sensor>>(349. * deg, bav_sensor));

// exclusive_bound excludes an end from the range: a heading in [0°, 360°).
QUANTITY_SPEC(bav_heading, isq::angular_measure);

inline constexpr struct bav_heading_zero final :
    absolute_point_origin<bav_heading, check_in_range{0. * deg, exclusive_bound{360. * deg}}> {
} bav_heading_zero;

using bav_heading_pt = quantity_point<bav_heading[deg], bav_heading_zero>;
static_assert(is_within_bounds<bav_heading_pt>(0. * deg, bav_heading_zero));
static_assert(is_within_bounds<bav_heading_pt>(359.5 * deg, bav_heading_zero));
static_assert(!is_within_bounds<bav_heading_pt>(360. * deg, bav_heading_zero));
static_assert(!is_within_bounds<bav_heading_pt>(not_a_number * deg, bav_heading_zero));

// ... and a strictly positive length in (0 m, 100 m].
QUANTITY_SPEC(bav_gap, isq::length);

inline constexpr struct bav_gap_zero final :
    absolute_point_origin<bav_gap, check_in_range{exclusive_bound{0. * m}, 100. * m}> {
} bav_gap_zero;

using bav_gap_pt = quantity_point<bav_gap[m], bav_gap_zero>;
static_assert(!is_within_bounds<bav_gap_pt>(0. * m, bav_gap_zero));
static_assert(is_within_bounds<bav_gap_pt>(1e-300 * m, bav_gap_zero));
static_assert(is_within_bounds<bav_gap_pt>(100. * m, bav_gap_zero));

// No min() (max()) is offered for an excluded end.
template<typename QP>
concept has_min = requires { QP::min(); };
template<typename QP>
concept has_max = requires { QP::max(); };
template<typename QP>
concept has_limits_max = requires { std::numeric_limits<QP>::max(); };
template<typename QP>
concept has_limits_lowest = requires { std::numeric_limits<QP>::lowest(); };

static_assert(has_min<bav_heading_pt> && !has_max<bav_heading_pt>);
static_assert(bav_heading_pt::min().quantity_from(bav_heading_zero) == 0. * deg);
static_assert(!has_limits_max<bav_heading_pt>);
static_assert(!has_min<bav_gap_pt> && has_max<bav_gap_pt> && !has_limits_lowest<bav_gap_pt>);
// wrap_to_range excludes its max, spelled exclusive_bound or not.
static_assert(!has_max<quantity_point<bav_azimuth[deg], bav_north>>);

inline constexpr struct bav_north_spelled final :
    absolute_point_origin<bav_azimuth, wrap_to_range{0. * deg, exclusive_bound{360. * deg}}> {
} bav_north_spelled;

static_assert(quantity_point<bav_azimuth[deg], bav_north_spelled>(370. * deg, bav_north_spelled)
                .quantity_from(bav_north_spelled) == 10. * deg);
// Spelling the max as exclusive_bound changes nothing: the same values wrap the same way, 360° is
// the same point as 0°, and neither form offers max().
using bav_wrap_plain = quantity_point<bav_azimuth[deg], bav_north>;
using bav_wrap_spelled = quantity_point<bav_azimuth[deg], bav_north_spelled>;
static_assert(!has_max<bav_wrap_spelled> && has_min<bav_wrap_spelled>);
static_assert(bav_wrap_spelled(360. * deg, bav_north_spelled).quantity_from(bav_north_spelled) == 0. * deg);
static_assert(bav_wrap_spelled(-10. * deg, bav_north_spelled).quantity_from(bav_north_spelled) == 350. * deg);
static_assert(bav_wrap_spelled(725. * deg, bav_north_spelled).quantity_from(bav_north_spelled) ==
              bav_wrap_plain(725. * deg, bav_north).quantity_from(bav_north));
static_assert(wrap_to_range{0. * deg, exclusive_bound{360. * deg}}(1e20 * deg) ==
              wrap_to_range{0. * deg, 360. * deg}(1e20 * deg));

// Nesting compares exclusivity: a [0°, 350°) child at +10° under [0°, 360°) ends at an excluded 360°.
inline constexpr struct bav_sector final :
    relative_point_origin<quantity_point<bav_heading[deg], bav_heading_zero, double>{10. * deg, bav_heading_zero},
                          check_in_range{0. * deg, exclusive_bound{350. * deg}}> {
} bav_sector;

static_assert(is_within_bounds<quantity_point<bav_heading[deg], bav_sector>>(349.5 * deg, bav_sector));
static_assert(!is_within_bounds<quantity_point<bav_heading[deg], bav_sector>>(350. * deg, bav_sector));

// wrap_to_range and reflect_in_range reduce any finite value in O(log) steps: subtracting the range
// once per step would never terminate here, as 1e20 - 360 == 1e20 in a double.
// 1e20 = 280 (mod 360), so 1e20° wraps to 280° and -1e20° to 80°.
static_assert(wrap_to_range{0. * deg, 360. * deg}(1e20 * deg) == 280. * deg);
static_assert(wrap_to_range{0. * deg, 360. * deg}(-1e20 * deg) == 80. * deg);
static_assert(wrap_to_range{0 * deg, 360 * deg}(1'000'000'007 * deg) == 287 * deg);
// Reflecting at [0°, 180°] has a period of 360°: 1e20° sits 280° into it and folds back to 80°.
static_assert(reflect_in_range{0. * deg, 180. * deg}(1e20 * deg) == 80. * deg);
static_assert(reflect_in_range{-90. * deg, 90. * deg}(-91. * deg) == -89. * deg);

// An integral representation never forms value - min, which overflows near its limits. In a
// constant expression an overflow would be ill-formed, so these also prove there is none.
static_assert(wrap_to_range{-180 * deg, 180 * deg}(std::numeric_limits<int>::max() * deg) == 127 * deg);
static_assert(wrap_to_range{-180 * deg, 180 * deg}(std::numeric_limits<int>::min() * deg) == -128 * deg);
static_assert(reflect_in_range{-90 * deg, 90 * deg}(std::numeric_limits<int>::max() * deg) == 53 * deg);
static_assert(reflect_in_range{-90 * deg, 90 * deg}(std::numeric_limits<int>::min() * deg) == -52 * deg);
static_assert(wrap_to_range{-180 * deg, 180 * deg}(180 * deg) == -180 * deg);
static_assert(wrap_to_range{-180 * deg, 180 * deg}(-181 * deg) == 179 * deg);
static_assert(reflect_in_range{-90 * deg, 90 * deg}(91 * deg) == 89 * deg);
static_assert(reflect_in_range{-90 * deg, 90 * deg}(-91 * deg) == -89 * deg);

// check_in_range needs both ends: a single argument would leave max value-initialized.
template<auto Lo>
concept can_check_one_sided = requires { check_in_range{Lo}; };
static_assert(!can_check_one_sided<1. * m>);

// exclusive_bound cannot be nested, and only check_in_range and wrap_to_range (at its max) take it.
template<typename T>
concept can_exclude = requires { typename exclusive_bound<T>; };
static_assert(can_exclude<quantity<m, double>>);
static_assert(!can_exclude<exclusive_bound<quantity<m, double>>>);

template<auto Lo, auto Hi>
concept can_clamp = requires { clamp_to_range{Lo, Hi}; };
template<auto Lo, auto Hi>
concept can_reflect = requires { reflect_in_range{Lo, Hi}; };
template<auto Lo, auto Hi>
concept can_wrap = requires { wrap_to_range{Lo, Hi}; };

static_assert(can_clamp<0. * m, 1. * m>);
static_assert(!can_clamp<0. * m, exclusive_bound{1. * m}>);
static_assert(!can_clamp<exclusive_bound{0. * m}, 1. * m>);
static_assert(!can_reflect<0. * m, exclusive_bound{1. * m}>);
static_assert(can_wrap<0. * m, exclusive_bound{1. * m}>);
static_assert(!can_wrap<exclusive_bound{0. * m}, 1. * m>);

// A user-defined checking policy: its operator() returns bool, and the library reports a value it
// rejects. Here a domain that is not an interval: a transmitter may only be tuned within its
// allowed bands, from 2.4 GHz to 2.4835 GHz and from 5.725 GHz to 5.875 GHz. min and max declare
// the outer ends.
template<Quantity Q>
struct check_in_bands {
  Q min;  // first band: [min, first_band_max]
  Q first_band_max;
  Q second_band_min;  // second band: [second_band_min, max]
  Q max;

  template<Quantity V>
  [[nodiscard]] constexpr bool operator()(const V& v) const
  {
    return (v >= V{min} && v <= V{first_band_max}) || (v >= V{second_band_min} && v <= V{max});
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD
template<Quantity Q>
check_in_bands(Q, Q, Q, Q) -> check_in_bands<Q>;
#endif

inline constexpr struct bav_tuning_zero final :
    absolute_point_origin<isq::frequency, check_in_bands{2.4 * GHz, 2.4835 * GHz, 5.725 * GHz, 5.875 * GHz}> {
} bav_tuning_zero;

using bav_tx_frequency = quantity_point<isq::frequency[GHz], bav_tuning_zero>;
static_assert(is_within_bounds<bav_tx_frequency>(2.45 * GHz, bav_tuning_zero));
static_assert(is_within_bounds<bav_tx_frequency>(2.4835 * GHz, bav_tuning_zero));
static_assert(is_within_bounds<bav_tx_frequency>(5.8 * GHz, bav_tuning_zero));
static_assert(!is_within_bounds<bav_tx_frequency>(3. * GHz, bav_tuning_zero));
static_assert(!is_within_bounds<bav_tx_frequency>(6. * GHz, bav_tuning_zero));
static_assert(!is_within_bounds<bav_tx_frequency>(not_a_number * GHz, bav_tuning_zero));
static_assert(bav_tx_frequency::max().quantity_from(bav_tuning_zero) == 5.875 * GHz);

// enforce_bounds keeps an adjusting policy's result on the inherited path: a user-defined clamp
// reached through an ancestor still clamps (and a value it lets through is stored unchanged).
template<Quantity Q>
struct user_clamp {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < V{min}) return V{min};
    if (v > V{max}) return V{max};
    return v;
  }
};

#if !MP_UNITS_HAS_AGGREGATE_CTAD
template<Quantity Q>
user_clamp(Q, Q) -> user_clamp<Q>;
#endif

QUANTITY_SPEC(bav_track, isq::length);

inline constexpr struct bav_track_start final : absolute_point_origin<bav_track, user_clamp{0. * m, 1. * m}> {
} bav_track_start;

inline constexpr struct bav_track_mid final : relative_point_origin<bav_track_start + 0.5 * m> {
} bav_track_mid;

static_assert(quantity_point<bav_track[m], bav_track_mid>(5. * m, bav_track_mid).quantity_from(bav_track_start) ==
              1. * m);

// clamp_non_negative adjusts a negative value, so it offers no query either.
QUANTITY_SPEC(bav_energy_like, isq::length);

inline constexpr struct bav_clamped_zero final : absolute_point_origin<bav_energy_like, clamp_non_negative{}> {
} bav_clamped_zero;

static_assert(
  !can_query_bounds<quantity_point<bav_energy_like[m], bav_clamped_zero>, bav_clamped_zero, quantity<m, double>>);

// Shapes the nesting check now rejects (each is a hard error in has_quantity_bounds_v, so they
// cannot be probed here):
// - a child declaring only a lower bound (check_non_negative) under an origin with an upper bound;
// - a child whose range exceeds a bounded origin two levels up, with an unbounded origin between.

// ----------------------------------------------------------------------------
// try_make: available with <expected> only.
// ----------------------------------------------------------------------------

#if MP_UNITS_HAS_EXPECTED

static_assert(try_make<trim_angle>(0. * deg, trim_zero).has_value());
static_assert(try_make<trim_angle>(10. * deg, trim_zero) == trim_angle{10. * deg, trim_zero});
static_assert(try_make<trim_angle>(-10. * deg, trim_zero) == trim_angle{-10. * deg, trim_zero});
static_assert(try_make<trim_angle>(just_above_10 * deg, trim_zero).error() == bounds_violation{});
static_assert(try_make<trim_angle>(just_below_minus_10 * deg, trim_zero).error() == bounds_violation{});
static_assert(try_make<trim_angle>(not_a_number * deg, trim_zero).error() == bounds_violation{});
static_assert(!try_make<trim_angle_plain>(11. * deg, trim_zero).has_value());

static_assert(try_make<bav_height>(0. * m)->quantity_from_zero() == 0. * m);
static_assert(!try_make<bav_height>(-1. * m).has_value());

static_assert(try_make<trim_offset>(5. * deg, trim_centre) == trim_offset{5. * deg, trim_centre});
static_assert(try_make<quantity_point<si::degree, trim_centre, double>>(5. * deg, trim_centre)
                ->quantity_from(trim_zero) == 9. * deg);
static_assert(!try_make<trim_offset>(6. * deg, trim_centre).has_value());
static_assert(try_make<trim_offset2>(4. * deg, trim_centre_plus_one).has_value());

template<typename QP, auto PO, typename Q>
concept can_try_make = requires(Q q) { try_make<QP>(q, PO); };

static_assert(can_try_make<trim_angle, trim_zero, quantity<deg, double>>);
static_assert(!can_try_make<quantity_point<test_angle_clamp[deg], clamp_origin>, clamp_origin,
                            quantity<test_angle_clamp[deg], double>>);
static_assert(can_try_make<quantity_point<bav_mixed[m], bav_mixed_rel>, bav_mixed_rel, quantity<m, double>>);
static_assert(!can_try_make<quantity_point<bav_mixed2[m], bav_mixed2_rel>, bav_mixed2_rel, quantity<m, double>>);
static_assert(try_make<bav_heading_pt>(359.5 * deg, bav_heading_zero).has_value());
static_assert(try_make<bav_heading_pt>(360. * deg, bav_heading_zero).error() == bounds_violation{});
static_assert(try_make<bav_tx_frequency>(5.8 * GHz, bav_tuning_zero).has_value());
static_assert(!try_make<bav_tx_frequency>(3. * GHz, bav_tuning_zero).has_value());

#endif  // MP_UNITS_HAS_EXPECTED

}  // namespace
