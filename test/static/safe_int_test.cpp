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

#include <mp-units/bits/hacks.h>
#include <mp-units/framework/customization_points.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/constrained.h>
#include <mp-units/utility/safe_int.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#if MP_UNITS_HOSTED
#include <string>
#endif
#endif

namespace {

using namespace mp_units;
using namespace mp_units::utility;
using namespace mp_units::detail;
using namespace mp_units::utility::detail;  // safe_int's own helpers (add_overflows, is_value_preserving_v, ...)

// ============================================================================
// A test error policy (mirrors constrained_test.cpp for cross-wrapper tests)
// ============================================================================

struct test_policy {
  static constexpr void on_constraint_violation(std::string_view) noexcept {}
};

// ============================================================================
// Type traits and value_type
// ============================================================================

static_assert(std::is_same_v<safe_int<int>::value_type, int>);
static_assert(std::is_same_v<safe_int<long>::value_type, long>);
static_assert(std::is_same_v<safe_int<std::uint8_t>::value_type, std::uint8_t>);

static_assert(std::is_same_v<value_type_t<safe_int<int>>, int>);
static_assert(std::is_same_v<value_type_t<safe_int<long>>, long>);
static_assert(std::is_same_v<value_type_t<safe_int<unsigned>>, unsigned>);

// ============================================================================
// is_value_preserving_int_v — range-based check using numeric_limits
// ============================================================================

// Same type: always value-preserving
static_assert(is_value_preserving_int_v<int, int>);
static_assert(is_value_preserving_int_v<unsigned, unsigned>);
static_assert(is_value_preserving_int_v<short, short>);

// Widening same signedness
static_assert(is_value_preserving_int_v<short, int>);
static_assert(is_value_preserving_int_v<std::int8_t, int>);
static_assert(is_value_preserving_int_v<std::uint8_t, unsigned>);
static_assert(is_value_preserving_int_v<std::uint8_t, std::uint64_t>);

// Narrowing: not value-preserving
static_assert(!is_value_preserving_int_v<int, short>);
static_assert(!is_value_preserving_int_v<int, std::int8_t>);
static_assert(!is_value_preserving_int_v<unsigned, std::uint8_t>);
static_assert(!is_value_preserving_int_v<long long, int>);

// Signed → unsigned same size: not value-preserving (negative values don't fit)
static_assert(!is_value_preserving_int_v<int, unsigned>);
static_assert(!is_value_preserving_int_v<short, unsigned short>);
static_assert(!is_value_preserving_int_v<std::int8_t, std::uint8_t>);

// Unsigned → signed same size: not value-preserving (large unsigned doesn't fit)
static_assert(!is_value_preserving_int_v<unsigned, int>);
static_assert(!is_value_preserving_int_v<unsigned short, short>);

// Unsigned → signed wider: value-preserving (uint8_t 0..255 fits in int)
static_assert(is_value_preserving_int_v<std::uint8_t, int>);
static_assert(is_value_preserving_int_v<std::uint8_t, long long>);

// Signed → unsigned wider: NOT value-preserving (negative values never fit unsigned)
static_assert(!is_value_preserving_int_v<std::int8_t, unsigned>);
static_assert(!is_value_preserving_int_v<int, std::uint64_t>);

// Cross-wrapper: safe_int<T> → raw U (safe_int has numeric_limits with same range as T)
static_assert(is_value_preserving_int_v<safe_int<short>, int>);
static_assert(!is_value_preserving_int_v<safe_int<int>, short>);
static_assert(!is_value_preserving_int_v<safe_int<int>, unsigned>);

// Cross-wrapper: raw T → safe_int<U>
static_assert(is_value_preserving_int_v<short, safe_int<int>>);
static_assert(!is_value_preserving_int_v<int, safe_int<short>>);
static_assert(is_value_preserving_int_v<int, safe_int<int>>);

// Cross-wrapper: safe_int<T> → safe_int<U>
static_assert(is_value_preserving_int_v<safe_int<short>, safe_int<int>>);
static_assert(!is_value_preserving_int_v<safe_int<int>, safe_int<short>>);
static_assert(!is_value_preserving_int_v<safe_int<int>, safe_int<unsigned>>);
static_assert(!is_value_preserving_int_v<safe_int<unsigned>, safe_int<int>>);
static_assert(is_value_preserving_int_v<safe_int<std::uint8_t>, safe_int<unsigned>>);

// Cross-wrapper: constrained<T> → raw U and vice versa
static_assert(is_value_preserving_int_v<constrained<short, test_policy>, int>);
static_assert(!is_value_preserving_int_v<constrained<int, test_policy>, short>);
static_assert(is_value_preserving_int_v<int, constrained<int, test_policy>>);

// Cross-wrapper: safe_int → constrained and vice versa
static_assert(is_value_preserving_int_v<safe_int<short>, constrained<int, test_policy>>);
static_assert(!is_value_preserving_int_v<safe_int<int>, constrained<short, test_policy>>);
static_assert(is_value_preserving_int_v<constrained<short, test_policy>, safe_int<int>>);
static_assert(!is_value_preserving_int_v<constrained<int, test_policy>, safe_int<short>>);

// Types without numeric_limits: always false
#if MP_UNITS_HOSTED
static_assert(!is_value_preserving_int_v<std::string, int>);
static_assert(!is_value_preserving_int_v<int, std::string>);
#endif

// ============================================================================
// is_value_preserving_v — generalized: delegates to is_value_preserving_int_v
// when both have numeric_limits, otherwise falls back to is_convertible_v
// ============================================================================

// Integral pairs: delegates to is_value_preserving_int_v
static_assert(is_value_preserving_v<short, int>);
static_assert(!is_value_preserving_v<int, short>);
static_assert(!is_value_preserving_v<int, unsigned>);
static_assert(is_value_preserving_v<std::uint8_t, int>);

// Wrapper pairs with numeric_limits: delegates to is_value_preserving_int_v
static_assert(is_value_preserving_v<safe_int<short>, safe_int<int>>);
static_assert(!is_value_preserving_v<safe_int<int>, safe_int<short>>);

// Non-numeric_limits types: falls back to is_convertible_v
#if MP_UNITS_HOSTED
static_assert(!is_value_preserving_v<std::string, int>);  // not convertible
static_assert(!is_value_preserving_v<int, std::string>);  // not convertible
#endif

// ============================================================================
// treat_as_floating_point: must be false
// ============================================================================

static_assert(!treat_as_floating_point<safe_int<int>>);
static_assert(!treat_as_floating_point<safe_int<long>>);
static_assert(!treat_as_floating_point<safe_int<std::uint64_t>>);

// ============================================================================
// Representation concepts
// ============================================================================

// UsesIntegerScaling: value_type is std::integral and the type supports * / with integers.
// The scaling engine uses the type's own operators so that safe_int's overflow checks and
// widening promotions are preserved throughout unit conversion.
static_assert(std::integral<value_type_t<safe_int<int>>>);
static_assert(UsesIntegerScaling<safe_int<int>>);
static_assert(UsesIntegerScaling<safe_int<std::int8_t>>);
static_assert(UsesIntegerScaling<safe_int<unsigned long>>);

// WeaklyRegular
static_assert(std::copyable<safe_int<int>>);
static_assert(std::equality_comparable<safe_int<int>>);

// BaseScalar
static_assert(BaseScalar<safe_int<int>>);
static_assert(BaseScalar<safe_int<long long>>);

// totally_ordered
static_assert(std::totally_ordered<safe_int<int>>);
static_assert(std::totally_ordered<safe_int<unsigned>>);

// RealScalar
static_assert(RealScalar<safe_int<int>>);
static_assert(RealScalar<safe_int<std::int8_t>>);
static_assert(RealScalar<safe_int<std::uint64_t>>);

// UnitMagnitudeScalable
static_assert(UnitMagnitudeScalable<safe_int<int>>);
static_assert(UnitMagnitudeScalable<safe_int<unsigned>>);

// ScalarRepresentation (real-field coverage is the RealScalar checks above)
static_assert(ScalarRepresentation<safe_int<int>>);
static_assert(ScalarRepresentation<safe_int<long>>);
static_assert(ScalarRepresentation<safe_int<unsigned short>>);

// floating-point types must NOT be accepted
static_assert(!requires { requires(!treat_as_floating_point<double>) && mp_units::detail::RealScalar<double>; });
static_assert(!requires { requires(!treat_as_floating_point<float>) && mp_units::detail::RealScalar<float>; });

// ============================================================================
// std::numeric_limits
// ============================================================================

static_assert(std::numeric_limits<safe_int<int>>::is_specialized);
static_assert(std::numeric_limits<safe_int<unsigned>>::is_specialized);

static_assert(std::numeric_limits<safe_int<int>>::min() == safe_int<int>{std::numeric_limits<int>::min()});
static_assert(std::numeric_limits<safe_int<int>>::max() == safe_int<int>{std::numeric_limits<int>::max()});
static_assert(std::numeric_limits<safe_int<int>>::lowest() == safe_int<int>{std::numeric_limits<int>::lowest()});

static_assert(std::numeric_limits<safe_int<unsigned>>::min() == safe_int<unsigned>{0u});

// ============================================================================
// representation_values
// ============================================================================

static_assert(std::constructible_from<safe_int<int>, int>);
static_assert(representation_values<safe_int<int>>::zero() == safe_int<int>{0});
static_assert(representation_values<safe_int<int>>::one() == safe_int<int>{1});
static_assert(representation_values<safe_int<int>>::min() == safe_int<int>{std::numeric_limits<int>::min()});
static_assert(representation_values<safe_int<int>>::max() == safe_int<int>{std::numeric_limits<int>::max()});

// ============================================================================
// Construction and conversion
// ============================================================================

static_assert(safe_int<int>{42}.value() == 42);
static_assert(static_cast<int>(safe_int<int>{-7}) == -7);
static_assert(safe_int<int>{0} == safe_int<int>{0});
static_assert(safe_int<int>{1} != safe_int<int>{2});

// operator T() is explicit: safe_int does not silently decay to the raw integer type.
// Use .value() or static_cast<T>() to extract the underlying value intentionally.
static_assert(!std::is_convertible_v<safe_int<int>, int>);
static_assert(std::is_constructible_v<int, safe_int<int>>);  // explicit cast still works

// ============================================================================
// Converting constructors — raw integer types
// ============================================================================

// Widening: implicit (short → int, is_convertible_v<short, int> is true)
static_assert(std::is_convertible_v<short, safe_int<int>>);
static_assert(std::is_constructible_v<safe_int<int>, short>);
static_assert(safe_int<int>{short{42}}.value() == 42);

// Narrowing: explicit (int range doesn't fit in short)
static_assert(!std::is_convertible_v<int, safe_int<short>>);
static_assert(std::is_constructible_v<safe_int<short>, int>);
static_assert(safe_int<short>{int{100}}.value() == 100);

// Signed↔unsigned same size: explicit (ranges don't fully overlap)
static_assert(!std::is_convertible_v<unsigned, safe_int<int>>);
static_assert(std::is_constructible_v<safe_int<int>, unsigned>);
static_assert(safe_int<int>{42u}.value() == 42);

static_assert(!std::is_convertible_v<int, safe_int<unsigned>>);
static_assert(std::is_constructible_v<safe_int<unsigned>, int>);
static_assert(safe_int<unsigned>{42}.value() == 42u);

// Widening unsigned→unsigned: implicit
static_assert(std::is_convertible_v<std::uint8_t, safe_int<unsigned>>);
static_assert(safe_int<unsigned>{std::uint8_t{200}}.value() == 200u);

// Widening signed→signed: implicit
static_assert(std::is_convertible_v<std::int8_t, safe_int<int>>);
static_assert(safe_int<int>{std::int8_t{42}}.value() == 42);

// ============================================================================
// Converting constructors — safe_int<U> → safe_int<T>
// (uses is_value_preserving_int_v for explicit/implicit decision)
// ============================================================================

// Widening: implicit (safe_int<short> → safe_int<int>)
static_assert(std::is_convertible_v<safe_int<short>, safe_int<int>>);
static_assert(std::is_constructible_v<safe_int<int>, safe_int<short>>);
static_assert(safe_int<int>{safe_int<short>{42}}.value() == 42);

// Narrowing: explicit (safe_int<int> → safe_int<short>)
static_assert(!std::is_convertible_v<safe_int<int>, safe_int<short>>);
static_assert(std::is_constructible_v<safe_int<short>, safe_int<int>>);
static_assert(safe_int<short>{safe_int<int>{100}}.value() == 100);

// Signed→unsigned: explicit (negative values don't fit)
static_assert(!std::is_convertible_v<safe_int<int>, safe_int<unsigned>>);
static_assert(std::is_constructible_v<safe_int<unsigned>, safe_int<int>>);
static_assert(safe_int<unsigned>{safe_int<int>{42}}.value() == 42u);

// Unsigned→signed same size: explicit (large unsigned doesn't fit)
static_assert(!std::is_convertible_v<safe_int<unsigned>, safe_int<int>>);
static_assert(std::is_constructible_v<safe_int<int>, safe_int<unsigned>>);
static_assert(safe_int<int>{safe_int<unsigned>{42u}}.value() == 42);

// Widening unsigned→unsigned: implicit
static_assert(std::is_convertible_v<safe_int<std::uint8_t>, safe_int<unsigned>>);
static_assert(safe_int<unsigned>{safe_int<std::uint8_t>{200}}.value() == 200u);

// ============================================================================
// CTAD (class template argument deduction)
// ============================================================================

static_assert(std::is_same_v<decltype(safe_int{42}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int{short{1}}), safe_int<short>>);
static_assert(std::is_same_v<decltype(safe_int{42u}), safe_int<unsigned>>);
static_assert(std::is_same_v<decltype(safe_int{42LL}), safe_int<long long>>);
static_assert(std::is_same_v<decltype(safe_int{std::int8_t{1}}), safe_int<std::int8_t>>);
static_assert(std::is_same_v<decltype(safe_int{std::uint64_t{1}}), safe_int<std::uint64_t>>);
static_assert(safe_int{100}.value() == 100);
static_assert(safe_int{short{42}}.value() == 42);

// ============================================================================
// common_type deduction
//
// Because operator T() is explicit, the only implicit conversion paths are the
// value-preserving constructors (widening).  The ternary used by std::common_type
// therefore has at most one viable implicit direction, so ternary resolution is
// unambiguous and the wrapper is never silently dropped.
//
// Helper: uses a dependent requires-expression so that "no viable conversion"
// (neither direction is implicit) becomes a clean SFINAE failure rather than a
// hard error — the dependent T/U defer the check to substitution time.
// ============================================================================

template<typename T, typename U>
inline constexpr bool has_common_type = requires(T a, U b) { false ? a : b; };

// Widening between same-sign wrappers: one direction is implicit → resolves correctly.
static_assert(
  std::is_same_v<std::common_type_t<safe_int<short>, safe_int<int>>, safe_int<int, safe_int<short>::error_policy>>);
static_assert(std::is_same_v<std::common_type_t<safe_int<std::int8_t>, safe_int<int>>,
                             safe_int<int, safe_int<std::int8_t>::error_policy>>);
static_assert(std::is_same_v<std::common_type_t<safe_int<std::uint8_t>, safe_int<unsigned>>,
                             safe_int<unsigned, safe_int<std::uint8_t>::error_policy>>);

// safe_int<int> vs raw int: with explicit operator T(), only int→safe_int<int> is
// implicit (value-preserving ctor).  The wrapper is preserved, not dropped.
static_assert(std::is_same_v<std::common_type_t<safe_int<int>, int>, safe_int<int>>);
static_assert(std::is_same_v<std::common_type_t<int, safe_int<int>>, safe_int<int>>);

// safe_int<short> vs raw int: int→safe_int<short> is explicit (narrowing), and
// safe_int<short>→int requires explicit operator T().  Neither direction is implicit
// → no common_type.  This prevents silently mixing safe and unsafe integer types.
static_assert(!has_common_type<safe_int<short>, int>);
static_assert(!has_common_type<int, safe_int<short>>);

// Widening unsigned→signed: uint8_t fits in int, so converting ctor is implicit.
static_assert(std::is_same_v<std::common_type_t<safe_int<std::uint8_t>, safe_int<int>>,
                             safe_int<int, safe_int<std::uint8_t>::error_policy>>);

// Signed↔unsigned same size: both constructors are explicit (ranges don't overlap)
// and operator T() is explicit → no common_type.  Mixed-signedness arithmetic
// must be resolved explicitly by the user.
static_assert(!has_common_type<safe_int<int>, safe_int<unsigned>>);
static_assert(!has_common_type<safe_int<unsigned>, safe_int<int>>);

// Cross-wrapper: safe_int<T> has an implicit constructor from constrained<T, P>
// (value-preserving), while constrained<T> only constructs from T directly (would
// need two UDCs via safe_int's operator T() — not allowed).  So only one direction
// is implicit → common_type resolves to safe_int.
static_assert(std::is_same_v<std::common_type_t<safe_int<int>, constrained<int, test_policy>>, safe_int<int>>);
static_assert(std::is_same_v<std::common_type_t<constrained<int, test_policy>, safe_int<int>>, safe_int<int>>);

// Cross-wrapper with widening: constrained<short> → safe_int<int> is implicit (range fits).
static_assert(std::is_same_v<std::common_type_t<constrained<short, test_policy>, safe_int<int>>, safe_int<int>>);

// Cross-wrapper where safe_int<short>→constrained<int> is not implicit (would need
// operator T() to go short→int then int→constrained<int>: two UDCs), and the
// reverse is explicit (narrowing) → no common_type.
static_assert(!has_common_type<safe_int<short>, constrained<int, test_policy>>);

// ============================================================================
// Comparison operators
// ============================================================================

static_assert(safe_int<int>{1} < safe_int<int>{2});
static_assert(safe_int<int>{2} > safe_int<int>{1});
static_assert(safe_int<int>{1} <= safe_int<int>{1});
static_assert(safe_int<int>{1} >= safe_int<int>{1});
static_assert((safe_int<int>{1} <=> safe_int<int>{2}) < 0);

// ============================================================================
// Unary operators
// ============================================================================

static_assert(+safe_int<int>{5} == safe_int<int>{5});
static_assert(-safe_int<int>{5} == safe_int<int>{-5});
static_assert(-safe_int<int>{-3} == safe_int<int>{3});

// Unary +/- model integral promotion: sub-int types promote to int
static_assert(std::is_same_v<decltype(+safe_int<short>{}), safe_int<int, safe_int<short>::error_policy>>);
static_assert(std::is_same_v<decltype(-safe_int<short>{}), safe_int<int, safe_int<short>::error_policy>>);
static_assert(
  std::is_same_v<decltype(+safe_int<unsigned short>{}), safe_int<int, safe_int<unsigned short>::error_policy>>);
static_assert(
  std::is_same_v<decltype(-safe_int<unsigned short>{}), safe_int<int, safe_int<unsigned short>::error_policy>>);
// int and wider types are unaffected by promotion
static_assert(std::is_same_v<decltype(+safe_int<int>{}), safe_int<int, safe_int<int>::error_policy>>);
static_assert(std::is_same_v<decltype(-safe_int<int>{}), safe_int<int, safe_int<int>::error_policy>>);
static_assert(std::is_same_v<decltype(+safe_int<long>{}), safe_int<long, safe_int<long>::error_policy>>);
static_assert(std::is_same_v<decltype(-safe_int<long>{}), safe_int<long, safe_int<long>::error_policy>>);

// safe_int<short>: -INT16_MIN promotes to int first, so int(32768) is in range — no overflow
static_assert(-safe_int<short>{std::numeric_limits<short>::min()} == safe_int<int>{32768});

// ============================================================================
// Arithmetic (non-overflow paths)
// ============================================================================

static_assert(safe_int<int>{3} + safe_int<int>{4} == safe_int<int>{7});
static_assert(safe_int<int>{10} - safe_int<int>{3} == safe_int<int>{7});
static_assert(safe_int<int>{3} * safe_int<int>{4} == safe_int<int>{12});
static_assert(safe_int<int>{12} / safe_int<int>{4} == safe_int<int>{3});
static_assert(safe_int<int>{10} % safe_int<int>{3} == safe_int<int>{1});

// unsigned
static_assert(safe_int<unsigned>{10u} + safe_int<unsigned>{5u} == safe_int<unsigned>{15u});
static_assert(safe_int<unsigned>{10u} - safe_int<unsigned>{5u} == safe_int<unsigned>{5u});
static_assert(safe_int<unsigned>{3u} * safe_int<unsigned>{4u} == safe_int<unsigned>{12u});
static_assert(safe_int<unsigned>{12u} / safe_int<unsigned>{4u} == safe_int<unsigned>{3u});

// ============================================================================
// Increment / decrement
// ============================================================================

static_assert([] {
  safe_int<int> x{5};
  ++x;
  return x == safe_int<int>{6};
}());

static_assert([] {
  safe_int<int> x{5};
  x++;
  return x == safe_int<int>{6};
}());

static_assert([] {
  safe_int<int> x{5};
  --x;
  return x == safe_int<int>{4};
}());

static_assert([] {
  safe_int<int> x{5};
  x--;
  return x == safe_int<int>{4};
}());

// post-increment / post-decrement return the old value
static_assert([] {
  safe_int<int> x{5};
  auto old = x++;
  return old == safe_int<int>{5} && x == safe_int<int>{6};
}());

static_assert([] {
  safe_int<int> x{5};
  auto old = x--;
  return old == safe_int<int>{5} && x == safe_int<int>{4};
}());

// ============================================================================
// Compound assignment
// ============================================================================

static_assert([] {
  safe_int<int> x{10};
  x += safe_int<int>{3};
  return x == safe_int<int>{13};
}());

static_assert([] {
  safe_int<int> x{10};
  x -= safe_int<int>{3};
  return x == safe_int<int>{7};
}());

static_assert([] {
  safe_int<int> x{10};
  x *= safe_int<int>{3};
  return x == safe_int<int>{30};
}());

static_assert([] {
  safe_int<int> x{10};
  x /= safe_int<int>{2};
  return x == safe_int<int>{5};
}());

static_assert([] {
  safe_int<int> x{10};
  x %= safe_int<int>{3};
  return x == safe_int<int>{1};
}());

// ============================================================================
// Overflow detection helpers
// ============================================================================

// add_overflows
static_assert(!utility::detail::add_overflows<int>(1, 2));
static_assert(!utility::detail::add_overflows<int>(std::numeric_limits<int>::max() - 1, 1));
static_assert(utility::detail::add_overflows<int>(std::numeric_limits<int>::max(), 1));
static_assert(utility::detail::add_overflows<int>(std::numeric_limits<int>::min(), -1));
static_assert(!utility::detail::add_overflows<unsigned>(0u, 0u));
static_assert(utility::detail::add_overflows<unsigned>(std::numeric_limits<unsigned>::max(), 1u));

// sub_overflows
static_assert(!utility::detail::sub_overflows<int>(5, 3));
static_assert(utility::detail::sub_overflows<int>(std::numeric_limits<int>::min(), 1));
static_assert(utility::detail::sub_overflows<int>(std::numeric_limits<int>::max(), -1));
static_assert(!utility::detail::sub_overflows<unsigned>(5u, 3u));
static_assert(utility::detail::sub_overflows<unsigned>(0u, 1u));

// mul_overflows
static_assert(!utility::detail::mul_overflows<int>(2, 3));
static_assert(!utility::detail::mul_overflows<int>(std::numeric_limits<int>::max(), 1));
static_assert(utility::detail::mul_overflows<int>(std::numeric_limits<int>::max(), 2));
static_assert(utility::detail::mul_overflows<int>(std::numeric_limits<int>::min(), -1));
static_assert(!utility::detail::mul_overflows<unsigned>(0u, std::numeric_limits<unsigned>::max()));
static_assert(utility::detail::mul_overflows<unsigned>(std::numeric_limits<unsigned>::max(), 2u));

// div_overflows
static_assert(!utility::detail::div_overflows<int>(10, 2));
static_assert(utility::detail::div_overflows<int>(10, 0));
static_assert(utility::detail::div_overflows<int>(std::numeric_limits<int>::min(), -1));
static_assert(!utility::detail::div_overflows<int>(std::numeric_limits<int>::min(), 2));
static_assert(utility::detail::div_overflows<unsigned>(5u, 0u));

// neg_overflows
static_assert(!utility::detail::neg_overflows<int>(5));
static_assert(!utility::detail::neg_overflows<int>(-5));
static_assert(!utility::detail::neg_overflows<int>(0));
static_assert(utility::detail::neg_overflows<int>(std::numeric_limits<int>::min()));
static_assert(!utility::detail::neg_overflows<unsigned>(0u));
static_assert(utility::detail::neg_overflows<unsigned>(1u));
static_assert(utility::detail::neg_overflows<unsigned>(std::numeric_limits<unsigned>::max()));

// ============================================================================
// Error policy types exist and have on_overflow
// ============================================================================

static_assert(requires { safe_int_terminate_policy::on_overflow(""); });
#if MP_UNITS_HOSTED
static_assert(requires { safe_int_throw_policy::on_overflow(""); });
#endif

// ============================================================================
// safe_int<T, ErrorPolicy> default policy selection
// ============================================================================

#if MP_UNITS_HOSTED
static_assert(std::is_same_v<safe_int<int>, safe_int<int, safe_int_throw_policy>>);
#else
static_assert(std::is_same_v<safe_int<int>, safe_int<int, safe_int_terminate_policy>>);
#endif

// ============================================================================
// Mixed-type arithmetic — same value_type (overflow-checked, keeps safe_int<T>)
// ============================================================================

// safe_int<T> * T / T  →  safe_int<T>
static_assert(std::is_same_v<decltype(safe_int<int>{} * 2), safe_int<int>>);
static_assert(std::is_same_v<decltype(2 * safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / 2), safe_int<int>>);

static_assert(safe_int<int>{6} * 2 == safe_int<int>{12});
static_assert(2 * safe_int<int>{6} == safe_int<int>{12});
static_assert(safe_int<int>{12} / 4 == safe_int<int>{3});

// ============================================================================
// Scalar addition, subtraction, modulo — integral (overflow-checked, keeps safe_int<T>)
// ============================================================================

// same-type: result stays safe_int<int>
static_assert(std::is_same_v<decltype(safe_int<int>{} + 2), safe_int<int>>);
static_assert(std::is_same_v<decltype(2 + safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - 2), safe_int<int>>);
static_assert(std::is_same_v<decltype(2 - safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} % 3), safe_int<int>>);
static_assert(std::is_same_v<decltype(10 % safe_int<int>{}), safe_int<int>>);

// cross-type: int op long long → long long
static_assert(std::is_same_v<decltype(safe_int<int>{} + 1LL), safe_int<long long>>);
static_assert(std::is_same_v<decltype(1LL + safe_int<int>{}), safe_int<long long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - 1LL), safe_int<long long>>);
static_assert(std::is_same_v<decltype(1LL - safe_int<int>{}), safe_int<long long>>);

// values
static_assert(safe_int<int>{3} + 4 == safe_int<int>{7});
static_assert(4 + safe_int<int>{3} == safe_int<int>{7});
static_assert(safe_int<int>{10} - 3 == safe_int<int>{7});
static_assert(10 - safe_int<int>{3} == safe_int<int>{7});
static_assert(safe_int<int>{10} % 3 == safe_int<int>{1});
static_assert(10 % safe_int<int>{3} == safe_int<int>{1});

// ============================================================================
// Cross-type integral (keeps wrapper, promotes to safe_int<R>, overflow-checked)
// R = decltype(T * U), e.g. int * long long → long long
// ============================================================================

static_assert(std::is_same_v<decltype(safe_int<int>{} * 1LL), safe_int<long long>>);
static_assert(std::is_same_v<decltype(1LL * safe_int<int>{}), safe_int<long long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / 1LL), safe_int<long long>>);

static_assert(safe_int<int>{6} * 2LL == safe_int<long long>{12});
static_assert(2LL * safe_int<int>{6} == safe_int<long long>{12});
static_assert(safe_int<int>{12} / 4LL == safe_int<long long>{3});

// ============================================================================
// Cross-type integral — additional convertibility coverage
//
// Exercises the `safe_int<T> op integral_U` overloads for combinations not covered above:
//   - narrower raw RHS (promotes to T's rank, stays safe_int<T>)
//   - `long` (whose width differs across platforms — 32-bit on Windows, 64-bit on POSIX),
//     verifying that the safe_int op integral overload is chosen over implicit conversion
//     of the raw integer to safe_int<T> via the converting ctor.
// ============================================================================

// Narrowing raw RHS: result keeps the wider wrapper type.
// short → int by C++ promotion rules, so safe_int<int> + short → safe_int<int>.
static_assert(std::is_same_v<decltype(safe_int<int>{} + short{1}), safe_int<int>>);
static_assert(std::is_same_v<decltype(short{1} + safe_int<int>{}), safe_int<int>>);
static_assert(safe_int<int>{3} + short{4} == safe_int<int>{7});
static_assert(short{4} + safe_int<int>{3} == safe_int<int>{7});

// safe_int<short> + int → safe_int<int> (raw RHS has higher rank).
static_assert(std::is_same_v<decltype(safe_int<short>{} + 1), safe_int<int>>);
static_assert(std::is_same_v<decltype(1 + safe_int<short>{}), safe_int<int>>);
static_assert(safe_int<short>{3} + 4 == safe_int<int>{7});

// safe_int<int> + 1L: result is safe_int<long>.  On Windows where `long` and `int` have
// equal width this would otherwise be tempted by the converting ctor (raw long → safe_int<int>),
// which would yield safe_int<int>; the safe_int op integral overload wins by exact match.
static_assert(std::is_same_v<decltype(safe_int<int>{} + 1L), safe_int<long>>);
static_assert(std::is_same_v<decltype(1L + safe_int<int>{}), safe_int<long>>);
static_assert(safe_int<int>{3} + 4L == safe_int<long>{7L});
static_assert(4L + safe_int<int>{3} == safe_int<long>{7L});

// Unsigned widening: safe_int<unsigned> + uint8_t → safe_int<unsigned>.
static_assert(std::is_same_v<decltype(safe_int<unsigned>{} + std::uint8_t{1}), safe_int<unsigned>>);
static_assert(std::is_same_v<decltype(std::uint8_t{1} + safe_int<unsigned>{}), safe_int<unsigned>>);
static_assert(safe_int<unsigned>{3u} + std::uint8_t{4} == safe_int<unsigned>{7u});

// int × unsigned scalar arithmetic is intentionally ill-formed — same rationale as
// safe_int<int> + safe_int<unsigned>: sign-mismatch conversions produce counterintuitive
// results (e.g. safe_int<int>{-1} * 2u → UINT_MAX-1 via reinterpretation).
// Comparisons remain allowed because they use std::cmp_* which is correct for mixed-sign.
static_assert(!std::is_invocable_v<std::plus<>, safe_int<int>, unsigned>);
static_assert(!std::is_invocable_v<std::minus<>, safe_int<int>, unsigned>);
static_assert(!std::is_invocable_v<std::multiplies<>, safe_int<int>, unsigned>);
static_assert(!std::is_invocable_v<std::divides<>, safe_int<int>, unsigned>);
static_assert(!std::is_invocable_v<std::modulus<>, safe_int<int>, unsigned>);
static_assert(!std::is_invocable_v<std::plus<>, unsigned, safe_int<int>>);
static_assert(!std::is_invocable_v<std::minus<>, unsigned, safe_int<int>>);
static_assert(!std::is_invocable_v<std::multiplies<>, unsigned, safe_int<int>>);
static_assert(!std::is_invocable_v<std::divides<>, unsigned, safe_int<int>>);
static_assert(!std::is_invocable_v<std::modulus<>, unsigned, safe_int<int>>);

// U / safe_int — symmetric with other scalar ops
static_assert(std::is_same_v<decltype(12 / safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(12LL / safe_int<int>{}), safe_int<long long>>);
static_assert(12 / safe_int<int>{4} == safe_int<int>{3});
static_assert(10LL / safe_int<int>{3} == safe_int<long long>{3});

// ============================================================================
// 128-bit integer scalar (mp_units::detail::integral extension for __int128 / unsigned __int128)
//
// On GCC std::integral<__int128> = false, so these operators previously fell
// through to the (now-removed) non-integral fallback and returned the raw type,
// not safe_int.  After the mp_units::detail::integral fix they are handled by the typed
// integral operators and return safe_int<R> with overflow checking.
// ============================================================================

#if defined(__SIZEOF_INT128__)
// mp_units::detail::integral must recognise both 128-bit types
static_assert(mp_units::detail::integral<int128_t>);
static_assert(mp_units::detail::integral<uint128_t>);

// mp_units::detail::is_signed_v must reflect the true signedness even on GCC strict mode
static_assert(mp_units::detail::is_signed_v<int128_t>);
static_assert(!mp_units::detail::is_signed_v<uint128_t>);

// same_sign_v must work for mixed width/128-bit pairings
static_assert(utility::detail::same_sign_v<long, int128_t>);
static_assert(utility::detail::same_sign_v<int128_t, long>);
static_assert(utility::detail::same_sign_v<unsigned long, uint128_t>);
static_assert(utility::detail::same_sign_v<uint128_t, unsigned long>);
static_assert(!utility::detail::same_sign_v<long, uint128_t>);
static_assert(!utility::detail::same_sign_v<unsigned long, int128_t>);

// safe_int<signed> × int128_t → safe_int<int128_t> (wrapper preserved)
static_assert(std::is_same_v<decltype(safe_int<long>{} * int128_t{1}), safe_int<int128_t>>);
static_assert(std::is_same_v<decltype(int128_t{1} * safe_int<long>{}), safe_int<int128_t>>);
static_assert(std::is_same_v<decltype(safe_int<long>{} / int128_t{1}), safe_int<int128_t>>);
static_assert(std::is_same_v<decltype(int128_t{1} / safe_int<long>{}), safe_int<int128_t>>);

// safe_int<unsigned> × uint128_t → safe_int<uint128_t> (wrapper preserved)
static_assert(std::is_same_v<decltype(safe_int<unsigned long>{} * uint128_t{1}), safe_int<uint128_t>>);
static_assert(std::is_same_v<decltype(uint128_t{1} * safe_int<unsigned long>{}), safe_int<uint128_t>>);
static_assert(std::is_same_v<decltype(safe_int<unsigned long>{} / uint128_t{1}), safe_int<uint128_t>>);
static_assert(std::is_same_v<decltype(uint128_t{1} / safe_int<unsigned long>{}), safe_int<uint128_t>>);

// sign mismatch with 128-bit is ill-formed (same policy as other mixed-sign operations)
static_assert(!std::is_invocable_v<std::multiplies<>, safe_int<long>, uint128_t>);
static_assert(!std::is_invocable_v<std::multiplies<>, safe_int<unsigned long>, int128_t>);
static_assert(!std::is_invocable_v<std::divides<>, safe_int<long>, uint128_t>);
static_assert(!std::is_invocable_v<std::divides<>, safe_int<unsigned long>, int128_t>);

// values
static_assert(safe_int<long>{6} * int128_t{2} == safe_int<int128_t>{12});
static_assert(int128_t{2} * safe_int<long>{6} == safe_int<int128_t>{12});
static_assert(safe_int<long>{12} / int128_t{4} == safe_int<int128_t>{3});
static_assert(int128_t{12} / safe_int<long>{4} == safe_int<int128_t>{3});

static_assert(safe_int<unsigned long>{6u} * uint128_t{2} == safe_int<uint128_t>{12u});
static_assert(uint128_t{2} * safe_int<unsigned long>{6u} == safe_int<uint128_t>{12u});
static_assert(safe_int<unsigned long>{12u} / uint128_t{4} == safe_int<uint128_t>{3u});
static_assert(uint128_t{12} / safe_int<unsigned long>{4u} == safe_int<uint128_t>{3u});

#endif

// ============================================================================
// Cross-type floating-point (drops wrapper, returns bare decltype(T * U))
// ============================================================================

// safe_int<int> + double  →  double (bare, no overflow wrapper)
static_assert(std::is_same_v<decltype(safe_int<int>{} + 1.0), double>);
static_assert(std::is_same_v<decltype(1.0 + safe_int<int>{}), double>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - 1.0), double>);
static_assert(std::is_same_v<decltype(1.0 - safe_int<int>{}), double>);

// safe_int<int> + float  →  float
static_assert(std::is_same_v<decltype(safe_int<int>{} + 1.0f), float>);
static_assert(std::is_same_v<decltype(1.0f + safe_int<int>{}), float>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - 1.0f), float>);
static_assert(std::is_same_v<decltype(1.0f - safe_int<int>{}), float>);

// safe_int<int> * double  →  double (bare, no overflow wrapper)
static_assert(std::is_same_v<decltype(safe_int<int>{} * 1.0), double>);
static_assert(std::is_same_v<decltype(1.0 * safe_int<int>{}), double>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / 1.0), double>);

// safe_int<int> * float  →  float
static_assert(std::is_same_v<decltype(safe_int<int>{} * 1.0f), float>);
static_assert(std::is_same_v<decltype(1.0f * safe_int<int>{}), float>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / 1.0f), float>);

// safe_int<long long> * double  →  double
static_assert(std::is_same_v<decltype(safe_int<long long>{} * 1.0), double>);
static_assert(std::is_same_v<decltype(safe_int<long long>{} / 1.0), double>);

// FP / safe_int — symmetric with safe_int / FP
static_assert(std::is_same_v<decltype(1.0 / safe_int<int>{}), double>);
static_assert(std::is_same_v<decltype(1.0f / safe_int<int>{}), float>);

// values
static_assert(safe_int<int>{3} + 1.5 == 4.5);
static_assert(1.5 + safe_int<int>{3} == 4.5);
static_assert(safe_int<int>{3} - 1.5 == 1.5);
static_assert(5.0 - safe_int<int>{3} == 2.0);
static_assert(safe_int<int>{3} * 2.5 == 7.5);
static_assert(2.5 * safe_int<int>{3} == 7.5);
static_assert(safe_int<int>{10} / 4.0 == 2.5);
static_assert(10.0 / safe_int<int>{4} == 2.5);
static_assert(10.0f / safe_int<int>{4} == 2.5f);

// ============================================================================
// Heterogeneous wrapper×wrapper arithmetic (safe_int<T,EP> op safe_int<U,EP>, T≠U)
// Result type is safe_int<decltype(T op U), EP> — same EP, promoted value_type.
// ============================================================================

// int op long  →  long  (int widens to long on 64-bit platforms)
static_assert(std::is_same_v<decltype(safe_int<int>{} + safe_int<long>{}), safe_int<long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - safe_int<long>{}), safe_int<long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} * safe_int<long>{}), safe_int<long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / safe_int<long>{}), safe_int<long>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} % safe_int<long>{}), safe_int<long>>);

// short op int  →  int  (short promotes to int)
static_assert(std::is_same_v<decltype(safe_int<short>{} + safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} + safe_int<short>{}), safe_int<int>>);

// Value correctness (non-overflow paths)
static_assert(safe_int<int>{3} + safe_int<long>{4L} == safe_int<long>{7L});
static_assert(safe_int<int>{10} - safe_int<long>{4L} == safe_int<long>{6L});
static_assert(safe_int<int>{3} * safe_int<long>{4L} == safe_int<long>{12L});
static_assert(safe_int<int>{12} / safe_int<long>{4L} == safe_int<long>{3L});
static_assert(safe_int<int>{10} % safe_int<long>{3L} == safe_int<long>{1L});

// Widening prevents overflow: INT_MAX + 1 fits in long long on every platform.
// (`long long` rather than `long`: on Windows `long` is 32-bit and equal to `int`, so the
// RHS `static_cast<long>(INT_MAX) + 1L` would itself be 32-bit signed overflow — UB at
// constexpr — and the assertion would not be portable.)
static_assert(safe_int<int>{std::numeric_limits<int>::max()} + safe_int<long long>{1LL} ==
              safe_int<long long>{static_cast<long long>(std::numeric_limits<int>::max()) + 1LL});

// Heterogeneous comparison (operator== and operator<=>)
static_assert(safe_int<int>{3} == safe_int<long>{3L});
static_assert(safe_int<int>{2} != safe_int<long>{3L});
static_assert((safe_int<int>{2} <=> safe_int<long>{3L}) < 0);
static_assert((safe_int<int>{3} <=> safe_int<long>{3L}) == 0);
static_assert((safe_int<int>{4} <=> safe_int<long>{3L}) > 0);

// Scalar comparison — integral
static_assert(safe_int<int>{3} == 3);
static_assert(3 == safe_int<int>{3});
static_assert(safe_int<int>{2} != 3);
static_assert(2 != safe_int<int>{3});
static_assert(safe_int<int>{2} < 3);
static_assert(2 < safe_int<int>{3});
static_assert((safe_int<int>{2} <=> 3) < 0);
static_assert((2 <=> safe_int<int>{3}) < 0);
static_assert((safe_int<int>{3} <=> 3) == 0);
static_assert((safe_int<int>{4} <=> 3) > 0);

// Scalar comparison — floating-point
static_assert(safe_int<int>{3} == 3.0);
static_assert(3.0 == safe_int<int>{3});
static_assert(safe_int<int>{2} < 2.5);
static_assert(2.5 > safe_int<int>{2});
static_assert((safe_int<int>{2} <=> 2.5) < 0);

// Mixed-signedness safe_int × safe_int comparisons use std::cmp_* — correct and allowed.
static_assert(safe_int<int>{-1} < safe_int<unsigned>{0u});
static_assert(!(safe_int<int>{-1} == safe_int<unsigned>{0u}));
static_assert(safe_int<int>{-1} != safe_int<unsigned>{0u});
static_assert((safe_int<int>{-1} <=> safe_int<unsigned>{0u}) < 0);
static_assert(safe_int<unsigned>{0u} > safe_int<int>{-1});
static_assert((safe_int<unsigned>{0u} <=> safe_int<int>{-1}) > 0);
static_assert(safe_int<int>{42} == safe_int<unsigned>{42u});
static_assert((safe_int<int>{42} <=> safe_int<unsigned>{42u}) == 0);
// Different EP is also fine for cross-sign comparisons:
static_assert(safe_int<int, safe_int_terminate_policy>{-1} < safe_int<unsigned>{0u});
// Same-sign, different EP comparisons are also allowed — bool result needs no policy:
static_assert(safe_int<int, safe_int_terminate_policy>{1} == safe_int<int>{1});
static_assert(safe_int<int, safe_int_terminate_policy>{1} < safe_int<int>{2});
static_assert((safe_int<int, safe_int_terminate_policy>{1} <=> safe_int<int>{2}) < 0);
static_assert(safe_int<int, safe_int_terminate_policy>{-1} < safe_int<long>{0});

// Mixed signedness scalar comparisons
static_assert(safe_int<int>{-1} < 0u);
static_assert(safe_int<unsigned>{0u} > -1);
static_assert((safe_int<int>{-1} <=> 0u) < 0);
static_assert(safe_int<int>{42} == 42u);

// Cross-wrapper comparison — safe_int vs constrained
static_assert(safe_int<int>{3} == constrained<int>{3});
static_assert(constrained<int>{3} == safe_int<int>{3});
static_assert(safe_int<int>{2} < constrained<int>{3});
static_assert(constrained<int>{2} < safe_int<int>{3});
static_assert((safe_int<int>{2} <=> constrained<int>{3}) < 0);
static_assert((constrained<int>{2} <=> safe_int<int>{3}) < 0);
static_assert((safe_int<int>{3} <=> constrained<long>{3L}) == 0);

// ============================================================================
// Cross-wrapper arithmetic: constrained op safe_int and safe_int op constrained
//
// Rules (regardless of operand order):
//   - Integral result  → safe_int<R, EP> wins (overflow is the tighter guarantee)
//   - Non-integral result (FP) → constrained<R, CP> wins (preserves constraint)
// ============================================================================

using EP = safe_int<int>::error_policy;  // safe_int_throw_policy on hosted

// --- operator+, operator- ---
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} + safe_int<int>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} + constrained<int, test_policy>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} - safe_int<int>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} - constrained<int, test_policy>{}), safe_int<int, EP>>);

// Non-integral result: constrained wins
static_assert(
  std::is_same_v<decltype(constrained<double, test_policy>{} + safe_int<int>{}), constrained<double, test_policy>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} + constrained<double, test_policy>{}), constrained<double, test_policy>>);
static_assert(
  std::is_same_v<decltype(constrained<double, test_policy>{} - safe_int<int>{}), constrained<double, test_policy>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} - constrained<double, test_policy>{}), constrained<double, test_policy>>);

// --- operator* ---
// Integral result: safe_int wins, same type both orderings
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * safe_int<int>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} * constrained<int, test_policy>{}), safe_int<int, EP>>);

// Cross-type integral: result type follows decltype of underlying op
static_assert(
  std::is_same_v<decltype(constrained<int, test_policy>{} * safe_int<long long>{}), safe_int<long long, EP>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} * constrained<long long, test_policy>{}), safe_int<long long, EP>>);

// Non-integral result: constrained wins, same type both orderings
static_assert(
  std::is_same_v<decltype(constrained<double, test_policy>{} * safe_int<int>{}), constrained<double, test_policy>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} * constrained<double, test_policy>{}), constrained<double, test_policy>>);

// --- operator/ ---
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / safe_int<int>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} / constrained<int, test_policy>{}), safe_int<int, EP>>);

static_assert(
  std::is_same_v<decltype(constrained<int, test_policy>{} / safe_int<long long>{}), safe_int<long long, EP>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} / constrained<long long, test_policy>{}), safe_int<long long, EP>>);

static_assert(
  std::is_same_v<decltype(constrained<double, test_policy>{} / safe_int<int>{}), constrained<double, test_policy>>);
static_assert(
  std::is_same_v<decltype(safe_int<int>{} / constrained<double, test_policy>{}), constrained<double, test_policy>>);

// --- operator% ---
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} % safe_int<int>{}), safe_int<int, EP>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} % constrained<int, test_policy>{}), safe_int<int, EP>>);

// Value tests for all operators (both orderings)
static_assert(constrained<int, test_policy>{3} + safe_int<int>{4} == safe_int<int>{7});
static_assert(safe_int<int>{3} + constrained<int, test_policy>{4} == safe_int<int>{7});
static_assert(constrained<int, test_policy>{10} - safe_int<int>{3} == safe_int<int>{7});
static_assert(safe_int<int>{10} - constrained<int, test_policy>{3} == safe_int<int>{7});
static_assert(constrained<int, test_policy>{3} * safe_int<int>{4} == safe_int<int>{12});
static_assert(safe_int<int>{3} * constrained<int, test_policy>{4} == safe_int<int>{12});
static_assert(constrained<int, test_policy>{12} / safe_int<int>{4} == safe_int<int>{3});
static_assert(safe_int<int>{12} / constrained<int, test_policy>{4} == safe_int<int>{3});
static_assert(constrained<int, test_policy>{10} % safe_int<int>{3} == safe_int<int>{1});
static_assert(safe_int<int>{10} % constrained<int, test_policy>{3} == safe_int<int>{1});

// Non-integral cross-wrapper value tests (constrained wins)
static_assert(constrained<double, test_policy>{3.0} + safe_int<int>{4} == constrained<double, test_policy>{7.0});
static_assert(safe_int<int>{3} + constrained<double, test_policy>{4.0} == constrained<double, test_policy>{7.0});
static_assert(constrained<double, test_policy>{10.0} - safe_int<int>{3} == constrained<double, test_policy>{7.0});
static_assert(safe_int<int>{10} - constrained<double, test_policy>{3.0} == constrained<double, test_policy>{7.0});
static_assert(constrained<double, test_policy>{3.0} * safe_int<int>{4} == constrained<double, test_policy>{12.0});
static_assert(safe_int<int>{3} * constrained<double, test_policy>{4.0} == constrained<double, test_policy>{12.0});
static_assert(constrained<double, test_policy>{12.0} / safe_int<int>{4} == constrained<double, test_policy>{3.0});
static_assert(safe_int<int>{12} / constrained<double, test_policy>{4.0} == constrained<double, test_policy>{3.0});

// ============================================================================
// Integer promotion rules
// safe_int uses decltype of the underlying operation, so homogeneous short×short
// must promote to safe_int<int> just as raw short+short promotes to int.
// ============================================================================

// Homogeneous short×short: both operands promote to int before the op.
static_assert(std::is_same_v<decltype(safe_int<short>{} + safe_int<short>{}), safe_int<decltype(short{} + short{})>>);
static_assert(std::is_same_v<decltype(safe_int<short>{} * safe_int<short>{}), safe_int<decltype(short{} * short{})>>);

// Scalar path inherits promotion: safe_int<short> * int-literal.
static_assert(std::is_same_v<decltype(safe_int<short>{} * 2), safe_int<decltype(short{} * 2)>>);

// Same-signedness widening (safe_int<short> + safe_int<int>) → safe_int<int>.
// The narrower type converts implicitly to safe_int<int> (value-preserving), and
// the homogeneous operator runs.
static_assert(std::is_same_v<decltype(safe_int<short>{} + safe_int<int>{}), safe_int<int>>);
static_assert(std::is_same_v<decltype(safe_int<int>{} + safe_int<short>{}), safe_int<int>>);

// Mixed-signedness safe_int×safe_int arithmetic is intentionally ill-formed.
// (has_common_type is false for int/unsigned pairs — explicit conversion required.)
static_assert(!std::is_invocable_v<std::plus<>, safe_int<int>, safe_int<unsigned>>);
static_assert(!std::is_invocable_v<std::minus<>, safe_int<int>, safe_int<unsigned>>);
static_assert(!std::is_invocable_v<std::multiplies<>, safe_int<int>, safe_int<unsigned>>);
static_assert(!std::is_invocable_v<std::divides<>, safe_int<int>, safe_int<unsigned>>);
static_assert(!std::is_invocable_v<std::plus<>, safe_int<unsigned>, safe_int<int>>);
static_assert(!std::is_invocable_v<std::minus<>, safe_int<unsigned>, safe_int<int>>);
static_assert(!std::is_invocable_v<std::multiplies<>, safe_int<unsigned>, safe_int<int>>);
static_assert(!std::is_invocable_v<std::divides<>, safe_int<unsigned>, safe_int<int>>);

// Scalar path with matching sub-int type must also promote.
// std::common_type_t<short, short> = short, but decltype(short{} op short{}) = int.
// Each operator below must return safe_int<int>, not safe_int<short>.
static_assert(std::is_same_v<decltype(safe_int<short>{} + short{}), safe_int<decltype(short{} + short{})>>);
static_assert(std::is_same_v<decltype(short{} + safe_int<short>{}), safe_int<decltype(short{} + short{})>>);
static_assert(std::is_same_v<decltype(safe_int<short>{} - short{}), safe_int<decltype(short{} - short{})>>);
static_assert(std::is_same_v<decltype(short{} - safe_int<short>{}), safe_int<decltype(short{} - short{})>>);
static_assert(std::is_same_v<decltype(safe_int<short>{} * short{}), safe_int<decltype(short{} * short{})>>);
static_assert(std::is_same_v<decltype(short{} * safe_int<short>{}), safe_int<decltype(short{} * short{})>>);
static_assert(std::is_same_v<decltype(safe_int<short>{} / short{}), safe_int<decltype(short{} / short{})>>);
static_assert(std::is_same_v<decltype(short{} / safe_int<short>{}), safe_int<decltype(short{} / short{})>>);
static_assert(std::is_same_v<decltype(safe_int<short>{} % short{}), safe_int<decltype(short{} % short{})>>);
static_assert(std::is_same_v<decltype(short{} % safe_int<short>{}), safe_int<decltype(short{} % short{})>>);

// ============================================================================
// Using safe_int as a quantity representation type
// ============================================================================

using namespace mp_units::si::unit_symbols;

// Basic quantity construction and operations
static_assert([] {
  quantity<isq::length[m], safe_int<int>> dist = safe_int{100} * m;
  return dist.numerical_value_in(m) == 100;
}());

// Unit conversion with safe_int
static_assert([] {
  quantity<isq::length[m], safe_int<int>> distance = safe_int{2} * m;
  quantity<isq::length[mm], safe_int<int>> distance_mm = distance;
  return distance_mm.numerical_value_in(mm) == 2000;
}());

// Arithmetic operations on quantities with safe_int
static_assert([] {
  quantity<isq::length[m], safe_int<int>> d1 = safe_int{100} * m;
  quantity<isq::length[m], safe_int<int>> d2 = safe_int{50} * m;
  auto sum = d1 + d2;
  return sum.numerical_value_in(m) == 150;
}());

// Multiplication of quantities
static_assert([] {
  quantity<isq::length[m], safe_int<int>> length = safe_int{3} * m;
  quantity<isq::length[m], safe_int<int>> width = safe_int{4} * m;
  auto area = length * width;
  return area.numerical_value_in(m2) == 12;
}());

// Division of quantities
static_assert([] {
  quantity<isq::length[m], safe_int<int>> distance = safe_int{100} * m;
  quantity<isq::time[s], safe_int<int>> time = safe_int{10} * s;
  auto speed = distance / time;
  return speed.numerical_value_in(m / s) == 10;
}());

// Scalar multiplication
static_assert([] {
  quantity<isq::length[m], safe_int<int>> dist = safe_int{5} * m;
  auto doubled = dist * 2;
  return doubled.numerical_value_in(m) == 10;
}());

// Comparison of quantities with safe_int
static_assert([] {
  quantity<isq::length[m], safe_int<int>> d1 = safe_int{100} * m;
  quantity<isq::length[m], safe_int<int>> d2 = safe_int{50} * m;
  return d1 > d2;
}());

// Same safe_int rep type requirement for quantity arithmetic
static_assert([] {
  quantity<isq::length[m], safe_int<int>> d1 = safe_int{100} * m;
  quantity<isq::length[m], safe_int<int>> d2 = safe_int{50} * m;
  auto sum = d1 + d2;
  return sum.numerical_value_in(m) == 150;
}());

// verify safe_int propagates through quantity operations
static_assert([] {
  quantity<isq::length[m], safe_int<int>> dist = safe_int{10} * m;
  auto squared = dist * dist;
  // Result should still use safe_int
  using result_rep = decltype(squared)::rep;
  return std::is_same_v<result_rep, safe_int<int>>;
}());

// Convenience aliases work with quantities
static_assert([] {
  quantity<isq::length[m], safe_i32> dist = safe_i32{100} * m;
  return dist.numerical_value_in(m) == 100;
}());

// ============================================================================
// Quantity-level common_type and implicit conversion with safe_int reps
//
// The framework's quantity common_type is:
//   quantity<get_common_reference(R1,R2), common_type_t<Rep1,Rep2>>
// It uses common_type_t<Rep1,Rep2> directly (not the ternary trick on quantities).
// This means bidirectional quantity-level implicit constructors (implicitly_scalable
// is true for same-unit non-FP) do NOT cause ambiguous ternaries — the raw
// safe_int level (where only widening is implicit) resolves common_type correctly.
// ============================================================================

using namespace mp_units::si::unit_symbols;
using qty_m_int = quantity<isq::length[si::metre], int>;
using qty_m_si = quantity<isq::length[si::metre], safe_int<int>>;

// Widening: safe_int<short> → safe_int<int> is implicit at both rep and quantity level.
static_assert(std::is_convertible_v<quantity<isq::length[si::metre], safe_int<short>>,
                                    quantity<isq::length[si::metre], safe_int<int>>>);

// Narrowing: safe_int<int> → safe_int<short> is explicit at both the rep and quantity
// level: the quantity constructor gates on `std::convertible_to<Rep2, rep>`, which is
// false when the rep's own constructor is explicit.
static_assert(!std::is_convertible_v<quantity<isq::length[si::metre], safe_int<int>>,
                                     quantity<isq::length[si::metre], safe_int<short>>>);
static_assert(std::is_constructible_v<quantity<isq::length[si::metre], safe_int<short>>,
                                      quantity<isq::length[si::metre], safe_int<int>>>);

// Common type: quantity<m, safe_int<int>> wins because common_type_t<safe_int<int>, safe_int<short>>
// = safe_int<int> (widening at the raw safe_int level is one-directional: short→int implicit only).
static_assert(std::is_same_v<std::common_type_t<quantity<isq::length[si::metre], safe_int<int>>,
                                                quantity<isq::length[si::metre], safe_int<short>>>,
                             quantity<isq::length[si::metre], safe_int<int>>>);

static_assert(std::is_same_v<std::common_type_t<quantity<isq::length[si::metre], safe_int<short>>,
                                                quantity<isq::length[si::metre], safe_int<int>>>,
                             quantity<isq::length[si::metre], safe_int<int>>>);

// With explicit operator T(), common_type<safe_int<int>, int> = safe_int<int>, so
// mixing quantity<m, safe_int<int>> with quantity<m, int> is now well-formed and
// the result preserves the safety wrapper.
static_assert(std::is_same_v<std::common_type_t<quantity<isq::length[si::metre], safe_int<int>>,
                                                quantity<isq::length[si::metre], int>>,
                             quantity<isq::length[si::metre], safe_int<int>>>);


// unit-conversion implicit: m→mm scales by ×1000 (integral), so implicitly_scalable=true.
static_assert(std::is_convertible_v<quantity<isq::length[si::metre], safe_int<int>>,
                                    quantity<isq::length[si::milli<si::metre>], safe_int<int>>>);

// unit-conversion explicit: mm→m scales by ÷1000 (non-integral), so implicitly_scalable=false.
static_assert(!std::is_convertible_v<quantity<isq::length[si::milli<si::metre>], safe_int<int>>,
                                     quantity<isq::length[si::metre], safe_int<int>>>);

}  // namespace
