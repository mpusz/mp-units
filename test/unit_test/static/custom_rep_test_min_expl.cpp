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

#include <units/generic/dimensionless.h>
#include <units/isq/si/length.h>
#include <units/isq/si/prefixes.h>
#include <ostream>
#include <type_traits>

namespace {

/**
 * @brief Representation type meeting minimum requirements
 *
 * This type with a default Mode = 0 provides the minimum set of requirements to
 * satisfy @c Representation concept which is used for quantity's representation type.
 *
 * In case of Mode != 0 only one of mandatory operation is removed which should
 * result in @c Representation concept not being satisfied.
 *
 * @tparam Mode a flag to disable specific type's operations
 */
template<int Mode = 0>
class min_expl {
  std::intmax_t value_;
public:
  // default construction
  min_expl()
    requires(Mode != 1)
  = default;

  // construction from std::int64_t
  constexpr explicit min_expl(std::intmax_t v) noexcept
    requires(Mode != 2)
      : value_(v)
  {
  }

  // copy construction
  min_expl(const min_expl&)
    requires(Mode != 3)
  = default;

  // move construction
  min_expl(min_expl&&)
    requires(Mode != 4)
  = default;
  min_expl(min_expl&&)
    requires(Mode == 4)
  = delete;

  // copy assignment
  min_expl& operator=(const min_expl&)
    requires(Mode != 5)
  = default;

  // move assignment
  min_expl& operator=(min_expl&&)
    requires(Mode != 6)
  = default;
  min_expl& operator=(min_expl&&)
    requires(Mode == 6)
  = delete;

  // equality
  [[nodiscard]] bool operator==(const min_expl&) const
    requires(Mode != 7)
  = default;

  // scalability - multiplication
  [[nodiscard]] friend constexpr min_expl operator*(const min_expl& lhs, const min_expl& rhs)
    requires(Mode != 8)
  {
    return min_expl(lhs.value_ * rhs.value_);
  }

  // scalability - division
  [[nodiscard]] friend constexpr min_expl operator/(const min_expl& lhs, const min_expl& rhs)
    requires(Mode != 9)
  {
    return min_expl(lhs.value_ / rhs.value_);
  }
};

}  // namespace

template<int Mode>
struct std::common_type<std::intmax_t, min_expl<Mode>> : std::type_identity<min_expl<Mode>> {};
template<int Mode>
struct std::common_type<min_expl<Mode>, std::intmax_t> : std::type_identity<min_expl<Mode>> {};

namespace {

using namespace units;
using namespace units::isq::si;

// quantity explicitly constructible (not convertible) from the representation type
static_assert(std::constructible_from<length<metre, min_expl<>>, min_expl<>>);
static_assert(!std::convertible_to<min_expl<>, length<metre, min_expl<>>>);

// not constructible from an underlying type
static_assert(!std::constructible_from<length<metre, min_expl<>>, int>);
static_assert(!std::convertible_to<int, length<metre, min_expl<>>>);

// dimensionless quantity implicitly convertible from the representation type
static_assert(std::constructible_from<dimensionless<one, min_expl<>>, min_expl<>>);
static_assert(std::convertible_to<min_expl<>, dimensionless<one, min_expl<>>>);

// but not from an underlying type
static_assert(!std::constructible_from<dimensionless<one, min_expl<>>, int>);
static_assert(!std::convertible_to<int, dimensionless<one, min_expl<>>>);

// or for ratio != 1
static_assert(std::constructible_from<dimensionless<percent, min_expl<>>, min_expl<>>);
static_assert(!std::convertible_to<min_expl<>, dimensionless<percent, min_expl<>>>);

// quantity convertible from itself
static_assert(std::constructible_from<length<metre, min_expl<>>, length<metre, min_expl<>>>);
static_assert(std::convertible_to<length<metre, min_expl<>>, length<metre, min_expl<>>>);

// not convertible from an underlying type
static_assert(!std::constructible_from<length<metre, min_expl<>>, length<metre, int>>);
static_assert(!std::convertible_to<length<metre, int>, length<metre, min_expl<>>>);

// quantity convertible from another non truncating unit
static_assert(std::constructible_from<length<metre, min_expl<>>, length<kilometre, min_expl<>>>);
static_assert(std::convertible_to<length<kilometre, min_expl<>>, length<metre, min_expl<>>>);

// quantity not convertible from another truncating unit
static_assert(!std::constructible_from<length<kilometre, min_expl<>>, length<metre, min_expl<>>>);
static_assert(!std::convertible_to<length<metre, min_expl<>>, length<kilometre, min_expl<>>>);

// rep type with explicit constructor - implicit construction of rep not allowed
static_assert(!std::constructible_from<length<metre, min_expl<>>, int>);
static_assert(!std::convertible_to<int, length<metre, min_expl<>>>);

static_assert(!std::constructible_from<length<metre, min_expl<>>, length<metre, int>>);
static_assert(!std::convertible_to<length<metre, int>, length<metre, min_expl<>>>);

static_assert(!std::constructible_from<length<metre, int>, min_expl<>>);
static_assert(!std::convertible_to<min_expl<>, length<metre, int>>);

static_assert(!std::constructible_from<length<metre, int>, length<metre, min_expl<>>>);
static_assert(!std::convertible_to<length<metre, min_expl<>>, length<metre, int>>);

static_assert(!std::constructible_from<length<metre, min_expl<>>, dimensionless<one>>);
static_assert(!std::convertible_to<dimensionless<one>, length<metre, min_expl<>>>);

static_assert(!std::constructible_from<length<metre, int>, dimensionless<one, min_expl<>>>);
static_assert(!std::convertible_to<dimensionless<one, min_expl<>>, length<metre, int>>);

// all operations needed to satisfy concept
static_assert(Representation<min_expl<>>);
static_assert(!Representation<min_expl<1>>);
static_assert(!Representation<min_expl<2>>);
static_assert(!Representation<min_expl<3>>);
#if !defined(UNITS_COMP_GCC) || UNITS_COMP_GCC > 10 || UNITS_COMP_GCC_MINOR > 1
static_assert(!Representation<min_expl<4>>);
#endif
static_assert(!Representation<min_expl<5>>);
#if !defined(UNITS_COMP_GCC) || UNITS_COMP_GCC > 10 || UNITS_COMP_GCC_MINOR > 1
static_assert(!Representation<min_expl<6>>);
#endif
static_assert(!Representation<min_expl<7>>);
static_assert(!Representation<min_expl<8>>);
static_assert(!Representation<min_expl<9>>);

// quantity's operators should mirror the representation type capabilities
template<typename Rep>
concept invalid_member_operations = requires(length<metre, Rep> lhs) {
  requires !requires { +lhs; };
  requires !requires { -lhs; };
  requires !requires { ++lhs; };
  requires !requires { lhs++; };
  requires !requires { --lhs; };
  requires !requires { lhs--; };

  requires !requires(length<metre, Rep> rhs) { lhs += rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs -= rhs; };
  requires !requires(Rep rhs) { lhs *= rhs; };
  requires !requires(Rep rhs) { lhs /= rhs; };
  requires !requires(Rep rhs) { lhs %= rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs %= rhs; };

  requires !requires(length<metre, Rep> rhs) { lhs + rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs - rhs; };
  requires !requires(Rep rhs) { lhs % rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs % rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs < rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs > rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs <= rhs; };
  requires !requires(length<metre, Rep> rhs) { lhs >= rhs; };

  requires !requires(length<metre, int> rhs) { lhs + rhs; };
  requires !requires(length<metre, int> rhs) { lhs - rhs; };
  requires !requires(int rhs) { lhs % rhs; };
  requires !requires(length<metre, int> rhs) { lhs % rhs; };
  requires !requires(length<metre, int> rhs) { lhs == rhs; };
  requires !requires(length<metre, int> rhs) { lhs != rhs; };
  requires !requires(length<metre, int> rhs) { lhs < rhs; };
  requires !requires(length<metre, int> rhs) { lhs > rhs; };
  requires !requires(length<metre, int> rhs) { lhs <= rhs; };
  requires !requires(length<metre, int> rhs) { lhs >= rhs; };

  requires !requires(std::ostream os) { os << lhs; };
};
static_assert(invalid_member_operations<min_expl<>>);

// equality
static_assert(length<kilometre, min_expl<>>(min_expl(2)) == length<metre, min_expl<>>(min_expl(2000)));
static_assert(length<metre, min_expl<>>(min_expl(123)) * min_expl(2) == length<metre, min_expl<>>(min_expl(246)));
static_assert(length<metre, min_expl<>>(min_expl(123)) * quantity{min_expl(2)} ==
              length<metre, min_expl<>>(min_expl(246)));
static_assert(min_expl(2) * length<metre, min_expl<>>(min_expl(123)) == length<metre, min_expl<>>(min_expl(246)));
static_assert(quantity{min_expl(2)} * length<metre, min_expl<>>(min_expl(123)) ==
              length<metre, min_expl<>>(min_expl(246)));
static_assert(length<metre, min_expl<>>(min_expl(246)) / min_expl(2) == length<metre, min_expl<>>(min_expl(123)));
static_assert(length<metre, min_expl<>>(min_expl(246)) / quantity{min_expl(2)} ==
              length<metre, min_expl<>>(min_expl(123)));
static_assert(length<metre, min_expl<>>(min_expl(246)) / length<metre, min_expl<>>(min_expl(2)) ==
              quantity{min_expl(123)});
static_assert(length<metre, min_expl<>>(min_expl(246)) / length<metre, min_expl<>>(min_expl(2)) == min_expl(123));

}  // namespace
