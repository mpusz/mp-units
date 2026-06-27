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
#include <mp-units/framework/representation_concepts.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#include <mp-units/utility/constrained.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#endif

namespace {

using namespace mp_units;
using namespace mp_units::utility;
using namespace mp_units::detail;

// ============================================================================
// A test error policy
// ============================================================================

struct test_policy {
  static constexpr void on_constraint_violation(std::string_view) noexcept {}
};

// ============================================================================
// Type traits and value_type
// ============================================================================

static_assert(std::is_same_v<constrained<int, test_policy>::value_type, int>);
static_assert(std::is_same_v<constrained<double, test_policy>::value_type, double>);
static_assert(std::is_same_v<constrained<long, test_policy>::value_type, long>);

// ============================================================================
// error_policy alias
// ============================================================================

static_assert(std::is_same_v<constrained<int, test_policy>::error_policy, test_policy>);

// ============================================================================
// Representation concepts (int)
// ============================================================================

static_assert(std::is_convertible_v<constrained<int, test_policy>, int>);
static_assert(std::is_convertible_v<int, constrained<int, test_policy>>);
static_assert(std::copyable<constrained<int, test_policy>>);
static_assert(std::equality_comparable<constrained<int, test_policy>>);
static_assert(std::totally_ordered<constrained<int, test_policy>>);
static_assert(RepresentationOf<constrained<int, test_policy>, quantity_tensor_order::scalar>);

// ============================================================================
// Representation concepts (double)
// ============================================================================

static_assert(std::is_convertible_v<constrained<double, test_policy>, double>);
static_assert(std::is_convertible_v<double, constrained<double, test_policy>>);
static_assert(std::copyable<constrained<double, test_policy>>);
static_assert(std::equality_comparable<constrained<double, test_policy>>);
static_assert(std::totally_ordered<constrained<double, test_policy>>);
static_assert(RepresentationOf<constrained<double, test_policy>, quantity_tensor_order::scalar>);

// ============================================================================
// std::numeric_limits
// ============================================================================

static_assert(std::numeric_limits<constrained<int, test_policy>>::is_specialized);
static_assert(std::numeric_limits<constrained<double, test_policy>>::is_specialized);

static_assert(std::numeric_limits<constrained<int, test_policy>>::min() ==
              constrained<int, test_policy>{std::numeric_limits<int>::min()});
static_assert(std::numeric_limits<constrained<int, test_policy>>::max() ==
              constrained<int, test_policy>{std::numeric_limits<int>::max()});

// ============================================================================
// Construction and conversion
// ============================================================================

static_assert(constrained<int, test_policy>{42}.value() == 42);
static_assert(static_cast<int>(constrained<int, test_policy>{-7}) == -7);
static_assert(constrained<int, test_policy>{0} == constrained<int, test_policy>{0});
static_assert(constrained<int, test_policy>{1} != constrained<int, test_policy>{2});

static_assert(constrained<double, test_policy>{3.14}.value() == 3.14);
static_assert(static_cast<double>(constrained<double, test_policy>{2.71828}) == 2.71828);

// ============================================================================
// Comparison operators
// ============================================================================

static_assert(constrained<int, test_policy>{1} < constrained<int, test_policy>{2});
static_assert(constrained<int, test_policy>{2} > constrained<int, test_policy>{1});
static_assert(constrained<int, test_policy>{1} <= constrained<int, test_policy>{1});
static_assert(constrained<int, test_policy>{1} >= constrained<int, test_policy>{1});
static_assert((constrained<int, test_policy>{1} <=> constrained<int, test_policy>{2}) < 0);

static_assert(constrained<double, test_policy>{1.0} < constrained<double, test_policy>{2.0});

// ============================================================================
// Unary operators
// ============================================================================

static_assert(+constrained<int, test_policy>{5} == constrained<int, test_policy>{5});
static_assert(-constrained<int, test_policy>{5} == constrained<int, test_policy>{-5});
static_assert(-constrained<int, test_policy>{-3} == constrained<int, test_policy>{3});

static_assert(-constrained<double, test_policy>{1.5} == constrained<double, test_policy>{-1.5});

// ============================================================================
// Arithmetic (int) — forwarded transparently
// ============================================================================

static_assert(constrained<int, test_policy>{3} + constrained<int, test_policy>{4} == constrained<int, test_policy>{7});
static_assert(constrained<int, test_policy>{10} - constrained<int, test_policy>{3} == constrained<int, test_policy>{7});
static_assert(constrained<int, test_policy>{3} * constrained<int, test_policy>{4} == constrained<int, test_policy>{12});
static_assert(constrained<int, test_policy>{12} / constrained<int, test_policy>{4} == constrained<int, test_policy>{3});
static_assert(constrained<int, test_policy>{10} % constrained<int, test_policy>{3} == constrained<int, test_policy>{1});

// ============================================================================
// Arithmetic (double)
// ============================================================================

static_assert(constrained<double, test_policy>{3.0} + constrained<double, test_policy>{4.0} ==
              constrained<double, test_policy>{7.0});
static_assert(constrained<double, test_policy>{10.0} - constrained<double, test_policy>{3.0} ==
              constrained<double, test_policy>{7.0});
static_assert(constrained<double, test_policy>{3.0} * constrained<double, test_policy>{4.0} ==
              constrained<double, test_policy>{12.0});
static_assert(constrained<double, test_policy>{12.0} / constrained<double, test_policy>{4.0} ==
              constrained<double, test_policy>{3.0});

// ============================================================================
// Increment / decrement
// ============================================================================

static_assert([] {
  constrained<int, test_policy> x{5};
  ++x;
  return x == constrained<int, test_policy>{6};
}());

static_assert([] {
  constrained<int, test_policy> x{5};
  auto old = x++;
  return old == constrained<int, test_policy>{5} && x == constrained<int, test_policy>{6};
}());

static_assert([] {
  constrained<int, test_policy> x{5};
  --x;
  return x == constrained<int, test_policy>{4};
}());

static_assert([] {
  constrained<int, test_policy> x{5};
  auto old = x--;
  return old == constrained<int, test_policy>{5} && x == constrained<int, test_policy>{4};
}());

// Postfix ++ / -- for double — exercises the trailing-return-type
// decltype(std::declval<T&>()++) path for floating-point T.
static_assert([] {
  constrained<double, test_policy> x{1.5};
  auto old = x++;
  return old == constrained<double, test_policy>{1.5} && x == constrained<double, test_policy>{2.5};
}());

static_assert([] {
  constrained<double, test_policy> x{2.5};
  auto old = x--;
  return old == constrained<double, test_policy>{2.5} && x == constrained<double, test_policy>{1.5};
}());

// ============================================================================
// constraint_violation_handler specialization
// ============================================================================

// constrained<T, P> has a specialization
static_assert(HasConstraintViolationHandler<constrained<int, test_policy>>);
static_assert(HasConstraintViolationHandler<constrained<double, test_policy>>);

// plain types do not
static_assert(!HasConstraintViolationHandler<int>);
static_assert(!HasConstraintViolationHandler<double>);

// ============================================================================
// Mixed arithmetic with raw types
// ============================================================================

// constrained + raw → should still be constrained (via implicit conversions)
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{3} + constrained<int, test_policy>{4}),
                             constrained<int, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{3} * constrained<int, test_policy>{4}),
                             constrained<int, test_policy>>);
static_assert(std::is_same_v<decltype(-constrained<int, test_policy>{5}), constrained<int, test_policy>>);

// constrained<double> * double and double * constrained<double> must be unambiguous
// (required by UsesFloatingPointScaling which is checked by RepresentationOf)
static_assert(requires(constrained<double, test_policy> v, double f) {
  { v * f } -> std::common_with<constrained<double, test_policy>>;
  { f * v } -> std::common_with<constrained<double, test_policy>>;
  { v / f } -> std::common_with<constrained<double, test_policy>>;
});

// ============================================================================
// UnitMagnitudeScalable / RepresentationOf — must hold for quantity_point use
// ============================================================================

// These are the exact concept prerequisites checked when constrained<double> is
// used as the Rep in quantity<> or quantity_point<>.  If any fails, the example
// hello_units.cpp will fail to compile.
static_assert(treat_as_floating_point<constrained<double, test_policy>>);
static_assert(UsesFloatingPointScaling<constrained<double, test_policy>>);
static_assert(UnitMagnitudeScalable<constrained<double, test_policy>>);
static_assert(RealScalar<constrained<double, test_policy>>);
static_assert(ScalarRepresentation<constrained<double, test_policy>>);
static_assert(RepresentationOf<constrained<double, test_policy>, isq::angular_measure>);

// ============================================================================
// representation_values compatibility
// ============================================================================

static_assert(representation_values<constrained<int, test_policy>>::zero() == constrained<int, test_policy>{0});
static_assert(representation_values<constrained<int, test_policy>>::one() == constrained<int, test_policy>{1});
static_assert(representation_values<constrained<int, test_policy>>::min() ==
              constrained<int, test_policy>{std::numeric_limits<int>::lowest()});
static_assert(representation_values<constrained<int, test_policy>>::max() ==
              constrained<int, test_policy>{std::numeric_limits<int>::max()});

// ============================================================================
// Mixed-type arithmetic — same value_type (returns constrained<T>)
// ============================================================================

static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * 2), constrained<int, test_policy>>);
static_assert(std::is_same_v<decltype(2 * constrained<int, test_policy>{}), constrained<int, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / 2), constrained<int, test_policy>>);

static_assert(constrained<int, test_policy>{6} * 2 == constrained<int, test_policy>{12});
static_assert(2 * constrained<int, test_policy>{6} == constrained<int, test_policy>{12});
static_assert(constrained<int, test_policy>{12} / 4 == constrained<int, test_policy>{3});

// ============================================================================
// Cross-type arithmetic (returns constrained<decltype(T op U), P>)
// Uses decltype of the actual underlying operation, not std::common_type_t.
// ============================================================================

// constrained<int> * double  →  constrained<double>  (decltype(int*double) == double)
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * 1.0), constrained<double, test_policy>>);
static_assert(std::is_same_v<decltype(1.0 * constrained<int, test_policy>{}), constrained<double, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / 1.0), constrained<double, test_policy>>);

// constrained<int> * float  →  constrained<float>
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * 1.0f), constrained<float, test_policy>>);
static_assert(std::is_same_v<decltype(1.0f * constrained<int, test_policy>{}), constrained<float, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / 1.0f), constrained<float, test_policy>>);

// constrained<int> * long long  →  constrained<long long>  (decltype(int*ll) == long long)
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * 1LL), constrained<long long, test_policy>>);
static_assert(std::is_same_v<decltype(1LL * constrained<int, test_policy>{}), constrained<long long, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / 1LL), constrained<long long, test_policy>>);

// constrained<double> * int  →  constrained<double>  (decltype(double*int) == double)
static_assert(std::is_same_v<decltype(constrained<double, test_policy>{} * 1), constrained<double, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<double, test_policy>{} / 1), constrained<double, test_policy>>);

// values
static_assert(constrained<int, test_policy>{3} * 2.5 == constrained<double, test_policy>{7.5});
static_assert(2.5 * constrained<int, test_policy>{3} == constrained<double, test_policy>{7.5});
static_assert(constrained<int, test_policy>{10} / 4.0 == constrained<double, test_policy>{2.5});

// ============================================================================
// Heterogeneous wrapper×wrapper arithmetic (constrained<T,EP> op constrained<U,EP>, T≠U)
// Result type is constrained<decltype(T op U), EP> — same EP, promoted value_type.
// ============================================================================

// int op long  →  long  (int widens to long on 64-bit platforms)
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} + constrained<long, test_policy>{}),
                             constrained<long, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} - constrained<long, test_policy>{}),
                             constrained<long, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} * constrained<long, test_policy>{}),
                             constrained<long, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} / constrained<long, test_policy>{}),
                             constrained<long, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} % constrained<long, test_policy>{}),
                             constrained<long, test_policy>>);

// short op int  →  int  (short promotes to int)
static_assert(std::is_same_v<decltype(constrained<short, test_policy>{} + constrained<int, test_policy>{}),
                             constrained<int, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} + constrained<short, test_policy>{}),
                             constrained<int, test_policy>>);

// Value correctness
static_assert(constrained<int, test_policy>{3} + constrained<long, test_policy>{4L} ==
              constrained<long, test_policy>{7L});
static_assert(constrained<int, test_policy>{10} - constrained<long, test_policy>{4L} ==
              constrained<long, test_policy>{6L});
static_assert(constrained<int, test_policy>{3} * constrained<long, test_policy>{4L} ==
              constrained<long, test_policy>{12L});
static_assert(constrained<int, test_policy>{12} / constrained<long, test_policy>{4L} ==
              constrained<long, test_policy>{3L});
static_assert(constrained<int, test_policy>{10} % constrained<long, test_policy>{3L} ==
              constrained<long, test_policy>{1L});

// Heterogeneous comparison (operator== and operator<=>)
static_assert(constrained<int, test_policy>{3} == constrained<long, test_policy>{3L});
static_assert(constrained<int, test_policy>{2} != constrained<long, test_policy>{3L});
static_assert((constrained<int, test_policy>{2} <=> constrained<long, test_policy>{3L}) < 0);
static_assert((constrained<int, test_policy>{3} <=> constrained<long, test_policy>{3L}) == 0);
static_assert((constrained<int, test_policy>{4} <=> constrained<long, test_policy>{3L}) > 0);

// EP is preserved: result carries the same error policy as the operands
static_assert(std::is_same_v<decltype(constrained<int, test_policy>{} + constrained<long, test_policy>{})::error_policy,
                             test_policy>);

// ============================================================================
// Integer and floating-point promotion rules
// The wrapper uses decltype of the underlying operation, so it must follow
// C++ usual-arithmetic-conversion and integer-promotion rules exactly.
// ============================================================================

// Homogeneous short×short: both operands promote to int before the op;
// result is constrained<decltype(short{}+short{})> (== constrained<int> on all
// common platforms, but written portably here).
static_assert(std::is_same_v<decltype(constrained<short, test_policy>{} + constrained<short, test_policy>{}),
                             constrained<decltype(short{} + short{}), test_policy>>);
static_assert(std::is_same_v<decltype(constrained<short, test_policy>{} * constrained<short, test_policy>{}),
                             constrained<decltype(short{} * short{}), test_policy>>);

// Unary + also triggers integer promotion: +short → int.
static_assert(
  std::is_same_v<decltype(+constrained<short, test_policy>{}), constrained<decltype(+short{}), test_policy>>);

// Scalar path inherits promotion: constrained<short> * int-literal.
static_assert(
  std::is_same_v<decltype(constrained<short, test_policy>{} * 2), constrained<decltype(short{} * 2), test_policy>>);

// Floating-point: float × float stays float (no promotion).
static_assert(std::is_same_v<decltype(constrained<float, test_policy>{} + constrained<float, test_policy>{}),
                             constrained<float, test_policy>>);

// Usual arithmetic conversion: float op double → double.
static_assert(std::is_same_v<decltype(constrained<float, test_policy>{} + constrained<double, test_policy>{}),
                             constrained<double, test_policy>>);
static_assert(std::is_same_v<decltype(constrained<double, test_policy>{} + constrained<float, test_policy>{}),
                             constrained<double, test_policy>>);

// FP scalar path: constrained<float> * double-literal → constrained<double>.
static_assert(std::is_same_v<decltype(constrained<float, test_policy>{} * 1.0), constrained<double, test_policy>>);

// ============================================================================
// common_type deduction
// SFINAE-friendly helper (GCC emits hard errors from `requires { typename common_type_t<...>; }`
// when the ternary inside common_type is ambiguous, so we use void_t instead).
template<typename, typename, typename = void>
struct has_common_type : std::false_type {};

template<typename T, typename U>
struct has_common_type<T, U, std::void_t<std::common_type_t<T, U>>> : std::true_type {};

// ============================================================================
// common_type deduction
//
// constrained<T,EP> has no converting constructor from constrained<U,EP>.
// When both wrappers have the same underlying type, common_type fails
// (ambiguous: both constrained→T and T→constrained are one-step implicit).
// When they have different underlying types, the wrapper drops off and
// common_type resolves to the underlying common_type.
// ============================================================================

// Same type: trivially exists
static_assert(std::is_same_v<std::common_type_t<constrained<int, test_policy>, constrained<int, test_policy>>,
                             constrained<int, test_policy>>);

// Different underlying types: wrapper drops off, resolves to underlying common_type
static_assert(std::is_same_v<std::common_type_t<constrained<short, test_policy>, constrained<int, test_policy>>, int>);
static_assert(std::is_same_v<std::common_type_t<constrained<int, test_policy>, constrained<long, test_policy>>, long>);
static_assert(
  std::is_same_v<std::common_type_t<constrained<int, test_policy>, constrained<unsigned, test_policy>>, unsigned>);

// constrained vs raw same type: ambiguous (both wrapper→T and T→wrapper are one-step implicit)
static_assert(!has_common_type<constrained<int, test_policy>, int>::value);
static_assert(!has_common_type<int, constrained<int, test_policy>>::value);
static_assert(!has_common_type<constrained<double, test_policy>, double>::value);

// Different policies: wrapper drops off, resolves to underlying type
struct other_policy {
  static constexpr void on_constraint_violation(std::string_view) noexcept {}
};
static_assert(std::is_same_v<std::common_type_t<constrained<int, test_policy>, constrained<int, other_policy>>, int>);

}  // namespace
