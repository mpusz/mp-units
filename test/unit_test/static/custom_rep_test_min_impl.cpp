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

#include <mp-units/systems/si/units.h>
#include <type_traits>

namespace {

/**
 * @brief Implicitly constructible and convertible representation type
 *
 * A wrapper type that is implicitly convertible from and to the contained type.
 *
 * @tparam T element type
 */
template<typename T>
class min_impl {
  T value_;
public:
  using value_type = T;

  min_impl() = default;
  constexpr explicit(false) min_impl(T v) noexcept : value_(v) {}
  template<typename U>
  constexpr explicit(false) min_impl(min_impl<U> i) noexcept : value_(static_cast<T>(static_cast<U>(i)))
  {
  }
  constexpr explicit(false) operator T() const noexcept { return value_; }
};

}  // namespace

template<typename T>
inline constexpr bool mp_units::is_scalar<min_impl<T>> = true;

template<typename T, typename U>
struct std::common_type<min_impl<T>, min_impl<U>> : std::type_identity<min_impl<std::common_type_t<T, U>>> {};
template<typename T, typename U>
struct std::common_type<min_impl<T>, U> : std::type_identity<min_impl<std::common_type_t<T, U>>> {};
template<typename U, typename T>
struct std::common_type<U, min_impl<T>> : std::type_identity<min_impl<std::common_type_t<T, U>>> {};

namespace {

using namespace mp_units;

static_assert(Representation<min_impl<int>>);
static_assert(Representation<min_impl<double>>);

// construction from a value is not allowed
static_assert(!std::constructible_from<quantity<si::metre, min_impl<int>>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, quantity<si::metre, min_impl<int>>>);

static_assert(!std::constructible_from<quantity<si::metre, min_impl<double>>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, quantity<si::metre, min_impl<double>>>);

// multiply syntax should work
template<typename T, auto U>
concept creates_quantity = Unit<std::remove_cvref_t<decltype(U)>> && requires { T{} * U; };

static_assert(creates_quantity<min_impl<int>, si::metre>);
static_assert(creates_quantity<min_impl<double>, si::metre>);

// multiply syntax
static_assert(creates_quantity<min_impl<int>, one>);
static_assert(creates_quantity<min_impl<double>, one>);
static_assert(creates_quantity<min_impl<int>, percent>);
static_assert(creates_quantity<min_impl<double>, percent>);

// construction from a quantity
// min_impl<T> -> min_impl<T>
static_assert(std::constructible_from<quantity<si::metre, min_impl<int>>, quantity<si::metre, min_impl<int>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<int>>, quantity<si::metre, min_impl<int>>>);

static_assert(std::constructible_from<quantity<si::metre, min_impl<double>>, quantity<si::metre, min_impl<double>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<double>>, quantity<si::metre, min_impl<double>>>);

static_assert(std::constructible_from<quantity<si::metre, min_impl<double>>, quantity<si::metre, min_impl<int>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<int>>, quantity<si::metre, min_impl<double>>>);

static_assert(!std::constructible_from<quantity<si::metre, min_impl<int>>,
                                       quantity<si::metre, min_impl<double>>>);  // narrowing conversion
static_assert(!std::convertible_to<quantity<si::metre, min_impl<double>>, quantity<si::metre, min_impl<int>>>);

// T -> min_impl<T>
static_assert(std::constructible_from<quantity<si::metre, min_impl<int>>, quantity<si::metre, int>>);
static_assert(std::convertible_to<quantity<si::metre, int>, quantity<si::metre, min_impl<int>>>);

static_assert(std::constructible_from<quantity<si::metre, min_impl<double>>, quantity<si::metre, double>>);
static_assert(std::convertible_to<quantity<si::metre, double>, quantity<si::metre, min_impl<double>>>);

static_assert(std::constructible_from<quantity<si::metre, min_impl<double>>, quantity<si::metre, int>>);
static_assert(std::convertible_to<quantity<si::metre, int>, quantity<si::metre, min_impl<double>>>);

static_assert(
  !std::constructible_from<quantity<si::metre, min_impl<int>>, quantity<si::metre, double>>);  // narrowing conversion
static_assert(!std::convertible_to<quantity<si::metre, double>, quantity<si::metre, min_impl<int>>>);

// min_impl<T> -> T
static_assert(std::constructible_from<quantity<si::metre, int>, quantity<si::metre, min_impl<int>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<int>>, quantity<si::metre, int>>);

static_assert(std::constructible_from<quantity<si::metre, double>, quantity<si::metre, min_impl<double>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<double>>, quantity<si::metre, double>>);

static_assert(std::constructible_from<quantity<si::metre, double>, quantity<si::metre, min_impl<int>>>);
static_assert(std::convertible_to<quantity<si::metre, min_impl<int>>, quantity<si::metre, double>>);

static_assert(
  !std::constructible_from<quantity<si::metre, int>, quantity<si::metre, min_impl<double>>>);  // narrowing conversion
static_assert(!std::convertible_to<quantity<si::metre, min_impl<double>>, quantity<si::metre, int>>);


// arithmetic operators
static_assert(min_impl<int>{1} * si::metre + min_impl<int>{1} * si::metre == min_impl<int>{2} * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<double>{1.5} * si::metre == min_impl<double>{2.5} * si::metre);
static_assert(1 * si::metre + min_impl<int>{1} * si::metre == min_impl<int>{2} * si::metre);
static_assert(1 * si::metre + min_impl<double>{1.5} * si::metre == min_impl<double>{2.5} * si::metre);
static_assert(min_impl<int>{1} * si::metre + 1 * si::metre == min_impl<int>{2} * si::metre);
static_assert(min_impl<int>{1} * si::metre + double{1.5} * si::metre == min_impl<double>{2.5} * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<int>{1} * si::metre == 2 * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<double>{1.5} * si::metre == double{2.5} * si::metre);

static_assert(min_impl<int>{1} * si::kilo<si::metre> + min_impl<int>{1} * si::metre ==
              min_impl<int>{1'001} * si::metre);
static_assert(min_impl<int>{1} * si::kilo<si::metre> + min_impl<double>{1.5} * si::metre ==
              min_impl<double>{1001.5} * si::metre);
static_assert(1 * si::kilo<si::metre> + min_impl<int>{1} * si::metre == min_impl<int>{1'001} * si::metre);
static_assert(1 * si::kilo<si::metre> + min_impl<double>{1.5} * si::metre == min_impl<double>{1001.5} * si::metre);
static_assert(min_impl<int>{1} * si::kilo<si::metre> + 1 * si::metre == min_impl<int>{1'001} * si::metre);
static_assert(min_impl<int>{1} * si::kilo<si::metre> + double{1.5} * si::metre == min_impl<double>{1001.5} * si::metre);
static_assert(min_impl<int>{1} * si::kilo<si::metre> + min_impl<int>{1} * si::metre == 1'001 * si::metre);
static_assert(min_impl<int>{1} * si::kilo<si::metre> + min_impl<double>{1.5} * si::metre == double{1001.5} * si::metre);

static_assert(min_impl<int>{1} * si::metre + min_impl<int>{1} * si::kilo<si::metre> ==
              min_impl<int>{1'001} * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<double>{1.5} * si::kilo<si::metre> ==
              min_impl<double>{1'501} * si::metre);
static_assert(1 * si::metre + min_impl<int>{1} * si::kilo<si::metre> == min_impl<int>{1'001} * si::metre);
static_assert(1 * si::metre + min_impl<double>{1.5} * si::kilo<si::metre> == min_impl<double>{1'501} * si::metre);
static_assert(min_impl<int>{1} * si::metre + 1 * si::kilo<si::metre> == min_impl<int>{1'001} * si::metre);
static_assert(min_impl<int>{1} * si::metre + double{1.5} * si::kilo<si::metre> == min_impl<double>{1'501} * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<int>{1} * si::kilo<si::metre> == 1'001 * si::metre);
static_assert(min_impl<int>{1} * si::metre + min_impl<double>{1.5} * si::kilo<si::metre> == double{1'501} * si::metre);

static_assert(min_impl<int>{2} * si::metre - min_impl<int>{1} * si::metre == min_impl<int>{1} * si::metre);
static_assert(min_impl<int>{2} * si::metre - min_impl<double>{1.5} * si::metre == min_impl<double>{0.5} * si::metre);
static_assert(2 * si::metre - min_impl<int>{1} * si::metre == min_impl<int>{1} * si::metre);
static_assert(2 * si::metre - min_impl<double>{1.5} * si::metre == min_impl<double>{0.5} * si::metre);
static_assert(min_impl<int>{2} * si::metre - 1 * si::metre == min_impl<int>{1} * si::metre);
static_assert(min_impl<int>{2} * si::metre - double{1.5} * si::metre == min_impl<double>{0.5} * si::metre);
static_assert(min_impl<int>{2} * si::metre - min_impl<int>{1} * si::metre == 1 * si::metre);
static_assert(min_impl<int>{2} * si::metre - min_impl<double>{1.5} * si::metre == double{0.5} * si::metre);

static_assert(min_impl<int>{2} * si::kilo<si::metre> - min_impl<int>{1} * si::metre ==
              min_impl<int>{1'999} * si::metre);
static_assert(min_impl<int>{2} * si::kilo<si::metre> - min_impl<double>{1.5} * si::metre ==
              min_impl<double>{1998.5} * si::metre);
static_assert(2 * si::kilo<si::metre> - min_impl<int>{1} * si::metre == min_impl<int>{1'999} * si::metre);
static_assert(2 * si::kilo<si::metre> - min_impl<double>{1.5} * si::metre == min_impl<double>{1998.5} * si::metre);
static_assert(min_impl<int>{2} * si::kilo<si::metre> - 1 * si::metre == min_impl<int>{1'999} * si::metre);
static_assert(min_impl<int>{2} * si::kilo<si::metre> - double{1.5} * si::metre == min_impl<double>{1998.5} * si::metre);
static_assert(min_impl<int>{2} * si::kilo<si::metre> - min_impl<int>{1} * si::metre == 1'999 * si::metre);
static_assert(min_impl<int>{2} * si::kilo<si::metre> - min_impl<double>{1.5} * si::metre == double{1998.5} * si::metre);

static_assert(min_impl<int>{2'000} * si::metre - min_impl<int>{1} * si::kilo<si::metre> ==
              min_impl<int>{1'000} * si::metre);
static_assert(min_impl<int>{2'000} * si::metre - min_impl<double>{1.5} * si::kilo<si::metre> ==
              min_impl<double>{500} * si::metre);
static_assert(2'000 * si::metre - min_impl<int>{1} * si::kilo<si::metre> == min_impl<int>{1'000} * si::metre);
static_assert(2'000 * si::metre - min_impl<double>{1.5} * si::kilo<si::metre> == min_impl<double>{500} * si::metre);
static_assert(min_impl<int>{2'000} * si::metre - 1 * si::kilo<si::metre> == min_impl<int>{1'000} * si::metre);
static_assert(min_impl<int>{2'000} * si::metre - double{1.5} * si::kilo<si::metre> ==
              min_impl<double>{500} * si::metre);
static_assert(min_impl<int>{2'000} * si::metre - min_impl<int>{1} * si::kilo<si::metre> == 1'000 * si::metre);
static_assert(min_impl<int>{2'000} * si::metre - min_impl<double>{1.5} * si::kilo<si::metre> ==
              double{500} * si::metre);

static_assert(min_impl<int>{123} * si::metre * min_impl<double>(1.5) == min_impl<double>{184.5} * si::metre);
static_assert(min_impl<int>{123} * si::metre * 1.5 == min_impl<double>{184.5} * si::metre);
static_assert(123 * si::metre * min_impl<double>(1.5) == min_impl<double>{184.5} * si::metre);

static_assert(min_impl<int>{123} * si::metre * (min_impl<double>(1.5) * one) == min_impl<double>{184.5} * si::metre);
static_assert(min_impl<int>{123} * si::metre * (1.5 * one) == min_impl<double>{184.5} * si::metre);
static_assert(123 * si::metre * (min_impl<double>(1.5) * one) == min_impl<double>{184.5} * si::metre);

static_assert(min_impl<double>(1.5) * min_impl<int>{123} * si::metre == min_impl<double>{184.5} * si::metre);
static_assert(min_impl<double>(1.5) * 123 * si::metre == min_impl<double>{184.5} * si::metre);
static_assert(1.5 * min_impl<int>{123} * si::metre == min_impl<double>{184.5} * si::metre);

static_assert(min_impl<double>(1.5) * one * (min_impl<int>{123} * si::metre) == min_impl<double>{184.5} * si::metre);
static_assert(min_impl<double>(1.5) * one * (123 * si::metre) == min_impl<double>{184.5} * si::metre);
static_assert(1.5 * one * (min_impl<int>{123} * si::metre) == min_impl<double>{184.5} * si::metre);

#ifndef MP_UNITS_COMP_CLANG
static_assert(min_impl<int>{123} * si::metre / min_impl<double>(2.) == min_impl<double>{61.5} * si::metre);
static_assert(min_impl<int>{123} * si::metre / 2. == min_impl<double>{61.5} * si::metre);
static_assert(123 * si::metre / min_impl<double>(2.) == min_impl<double>{61.5} * si::metre);

static_assert(min_impl<int>{123} * si::metre / (min_impl<double>(2.) * one) == min_impl<double>{61.5} * si::metre);
static_assert(min_impl<int>{123} * si::metre / (2. * one) == min_impl<double>{61.5} * si::metre);
static_assert(123 * si::metre / (min_impl<double>(2.) * one) == min_impl<double>{61.5} * si::metre);

static_assert(min_impl<int>{123} * si::metre / (min_impl<double>{2.} * si::metre) == 61.5 * one);
static_assert(min_impl<int>{123} * si::metre / (double{2.} * si::metre) == 61.5 * one);
static_assert(123 * si::metre / (min_impl<double>{2.} * si::metre) == 61.5 * one);
#endif

static_assert(min_impl<int>{123} * si::metre % (min_impl<int>(100) * si::metre) == 23 * si::metre);
static_assert(min_impl<int>{123} * si::metre % (100 * si::metre) == 23 * si::metre);
static_assert(123 * si::metre % (min_impl<int>(100) * si::metre) == 23 * si::metre);

}  // namespace
