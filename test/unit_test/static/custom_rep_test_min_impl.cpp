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
  constexpr min_impl(T v) noexcept : value_(v) {}
  template<typename U>
  constexpr min_impl(min_impl<U> i) noexcept : value_(static_cast<T>(i.value_)) {}
  constexpr operator T() const noexcept { return value_; }
};

}

template<typename T, typename U>
struct std::common_type<min_impl<T>, min_impl<U>> : std::common_type<T, U> {};
template<typename T, typename U>
struct std::common_type<min_impl<T>, U> : std::common_type<T, U> {};
template<typename U, typename T>
struct std::common_type<U, min_impl<T>> : std::common_type<T, U> {};

namespace {

using namespace units;
using namespace units::isq::si;

static_assert(Representation<min_impl<int>>);
static_assert(Representation<min_impl<double>>);

// construction from a value
static_assert(std::constructible_from<length<metre, min_impl<int>>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, min_impl<int>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, length<metre, min_impl<double>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, min_impl<double>>>);

static_assert(!std::constructible_from<length<metre, min_impl<int>>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, length<metre, min_impl<int>>>);

// construction from an underlying type
static_assert(std::constructible_from<length<metre, min_impl<int>>, int>);
static_assert(!std::convertible_to<int, length<metre, min_impl<int>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, double>);
static_assert(!std::convertible_to<double, length<metre, min_impl<double>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, int>);
static_assert(!std::convertible_to<int, length<metre, min_impl<double>>>);

static_assert(!std::constructible_from<length<metre, min_impl<int>>, double>);  // narrowing conversion
static_assert(!std::convertible_to<double, length<metre, min_impl<int>>>);

// dimensionless quantity is convertible from a value
static_assert(std::constructible_from<dimensionless<one, min_impl<int>>, min_impl<int>>);
static_assert(std::convertible_to<min_impl<int>, dimensionless<one, min_impl<int>>>);

static_assert(std::constructible_from<dimensionless<one, min_impl<double>>, min_impl<double>>);
static_assert(std::convertible_to<min_impl<double>, dimensionless<one, min_impl<double>>>);

static_assert(std::constructible_from<dimensionless<one, min_impl<double>>, min_impl<int>>);
static_assert(std::convertible_to<min_impl<int>, dimensionless<one, min_impl<double>>>);

static_assert(!std::constructible_from<dimensionless<one, min_impl<int>>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, dimensionless<one, min_impl<int>>>);

// and underlying type
static_assert(std::constructible_from<dimensionless<one, min_impl<int>>, int>);
static_assert(std::convertible_to<int, dimensionless<one, min_impl<int>>>);

static_assert(std::constructible_from<dimensionless<one, min_impl<double>>, double>);
static_assert(std::convertible_to<double, dimensionless<one, min_impl<double>>>);

static_assert(std::constructible_from<dimensionless<one, min_impl<double>>, int>);
static_assert(std::convertible_to<int, dimensionless<one, min_impl<double>>>);

static_assert(!std::constructible_from<dimensionless<one, min_impl<int>>, double>);  // narrowing conversion
static_assert(!std::convertible_to<double, dimensionless<one, min_impl<int>>>);

// but only for ratio(1), otherwise not convertible
static_assert(std::constructible_from<dimensionless<percent, min_impl<int>>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, dimensionless<percent, min_impl<int>>>);

static_assert(std::constructible_from<dimensionless<percent, min_impl<double>>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, dimensionless<percent, min_impl<double>>>);

static_assert(std::constructible_from<dimensionless<percent, min_impl<double>>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, dimensionless<percent, min_impl<double>>>);

static_assert(!std::constructible_from<dimensionless<percent, min_impl<int>>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, dimensionless<percent, min_impl<int>>>);

// implicit conversion tests
static_assert(std::constructible_from<length<metre, int>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, int>>);

static_assert(std::constructible_from<length<metre, double>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, length<metre, double>>);

static_assert(std::constructible_from<length<metre, double>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, double>>);

static_assert(!std::constructible_from<length<metre, int>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, length<metre, int>>);

// construction from an underlying type
static_assert(std::constructible_from<length<metre, int>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, int>>);

static_assert(std::constructible_from<length<metre, double>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, length<metre, double>>);

static_assert(std::constructible_from<length<metre, double>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, length<metre, double>>);

static_assert(!std::constructible_from<length<metre, int>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, length<metre, int>>);

// dimensionless quantity is convertible from a value
static_assert(std::constructible_from<dimensionless<one, int>, min_impl<int>>);
static_assert(std::convertible_to<min_impl<int>, dimensionless<one, int>>);

static_assert(std::constructible_from<dimensionless<one, double>, min_impl<double>>);
static_assert(std::convertible_to<min_impl<double>, dimensionless<one, double>>);

static_assert(std::constructible_from<dimensionless<one, double>, min_impl<int>>);
static_assert(std::convertible_to<min_impl<int>, dimensionless<one, double>>);

static_assert(!std::constructible_from<dimensionless<one, int>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, dimensionless<one, int>>);

// but only for ratio(1), otherwise not convertible
static_assert(std::constructible_from<dimensionless<percent, int>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, dimensionless<percent, int>>);

static_assert(std::constructible_from<dimensionless<percent, double>, min_impl<double>>);
static_assert(!std::convertible_to<min_impl<double>, dimensionless<percent, double>>);

static_assert(std::constructible_from<dimensionless<percent, double>, min_impl<int>>);
static_assert(!std::convertible_to<min_impl<int>, dimensionless<percent, double>>);

static_assert(!std::constructible_from<dimensionless<percent, int>, min_impl<double>>);  // narrowing conversion
static_assert(!std::convertible_to<min_impl<double>, dimensionless<percent, int>>);

// construction from a quantity
// min_impl<T> -> min_impl<T>
static_assert(std::constructible_from<length<metre, min_impl<int>>, length<metre, min_impl<int>>>);
static_assert(std::convertible_to<length<metre, min_impl<int>>, length<metre, min_impl<int>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, length<metre, min_impl<double>>>);
static_assert(std::convertible_to<length<metre, min_impl<double>>, length<metre, min_impl<double>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, length<metre, min_impl<int>>>);
static_assert(std::convertible_to<length<metre, min_impl<int>>, length<metre, min_impl<double>>>);

static_assert(!std::constructible_from<length<metre, min_impl<int>>, length<metre, min_impl<double>>>); // narrowing conversion
static_assert(!std::convertible_to<length<metre, min_impl<double>>, length<metre, min_impl<int>>>);

// T -> min_impl<T>
static_assert(std::constructible_from<length<metre, min_impl<int>>, length<metre, int>>);
static_assert(std::convertible_to<length<metre, int>, length<metre, min_impl<int>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, length<metre, double>>);
static_assert(std::convertible_to<length<metre, double>, length<metre, min_impl<double>>>);

static_assert(std::constructible_from<length<metre, min_impl<double>>, length<metre, int>>);
static_assert(std::convertible_to<length<metre, int>, length<metre, min_impl<double>>>);

static_assert(!std::constructible_from<length<metre, min_impl<int>>, length<metre, double>>); // narrowing conversion
static_assert(!std::convertible_to<length<metre, double>, length<metre, min_impl<int>>>);

// min_impl<T> -> T
static_assert(std::constructible_from<length<metre, int>, length<metre, min_impl<int>>>);
static_assert(std::convertible_to<length<metre, min_impl<int>>, length<metre, int>>);

static_assert(std::constructible_from<length<metre, double>, length<metre, min_impl<double>>>);
static_assert(std::convertible_to<length<metre, min_impl<double>>, length<metre, double>>);

static_assert(std::constructible_from<length<metre, double>, length<metre, min_impl<int>>>);
static_assert(std::convertible_to<length<metre, min_impl<int>>, length<metre, double>>);

static_assert(!std::constructible_from<length<metre, int>, length<metre, min_impl<double>>>); // narrowing conversion
static_assert(!std::convertible_to<length<metre, min_impl<double>>, length<metre, int>>);


// arithmetic operators
static_assert(length<metre, min_impl<int>>(1) + length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(2));
static_assert(length<metre, min_impl<int>>(1) + length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(2.5));
static_assert(length<metre, int>(1) + length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(2));
static_assert(length<metre, int>(1) + length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(2.5));
static_assert(length<metre, min_impl<int>>(1) + length<metre, int>(1) == length<metre, min_impl<int>>(2));
static_assert(length<metre, min_impl<int>>(1) + length<metre, double>(1.5) == length<metre, min_impl<double>>(2.5));
static_assert(length<metre, min_impl<int>>(1) + length<metre, min_impl<int>>(1) == length<metre, int>(2));
static_assert(length<metre, min_impl<int>>(1) + length<metre, min_impl<double>>(1.5) == length<metre, double>(2.5));

static_assert(length<kilometre, min_impl<int>>(1) + length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<kilometre, min_impl<int>>(1) + length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1001.5));
static_assert(length<kilometre, int>(1) + length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<kilometre, int>(1) + length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1001.5));
static_assert(length<kilometre, min_impl<int>>(1) + length<metre, int>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<kilometre, min_impl<int>>(1) + length<metre, double>(1.5) == length<metre, min_impl<double>>(1001.5));
static_assert(length<kilometre, min_impl<int>>(1) + length<metre, min_impl<int>>(1) == length<metre, int>(1001));
static_assert(length<kilometre, min_impl<int>>(1) + length<metre, min_impl<double>>(1.5) == length<metre, double>(1001.5));

static_assert(length<metre, min_impl<int>>(1) + length<kilometre, min_impl<int>>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<metre, min_impl<int>>(1) + length<kilometre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1501));
static_assert(length<metre, int>(1) + length<kilometre, min_impl<int>>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<metre, int>(1) + length<kilometre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1501));
static_assert(length<metre, min_impl<int>>(1) + length<kilometre, int>(1) == length<metre, min_impl<int>>(1001));
static_assert(length<metre, min_impl<int>>(1) + length<kilometre, double>(1.5) == length<metre, min_impl<double>>(1501));
static_assert(length<metre, min_impl<int>>(1) + length<kilometre, min_impl<int>>(1) == length<metre, int>(1001));
static_assert(length<metre, min_impl<int>>(1) + length<kilometre, min_impl<double>>(1.5) == length<metre, double>(1501));

static_assert(length<metre, min_impl<int>>(2) - length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1));
static_assert(length<metre, min_impl<int>>(2) - length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(0.5));
static_assert(length<metre, int>(2) - length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1));
static_assert(length<metre, int>(2) - length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(0.5));
static_assert(length<metre, min_impl<int>>(2) - length<metre, int>(1) == length<metre, min_impl<int>>(1));
static_assert(length<metre, min_impl<int>>(2) - length<metre, double>(1.5) == length<metre, min_impl<double>>(0.5));
static_assert(length<metre, min_impl<int>>(2) - length<metre, min_impl<int>>(1) == length<metre, int>(1));
static_assert(length<metre, min_impl<int>>(2) - length<metre, min_impl<double>>(1.5) == length<metre, double>(0.5));

static_assert(length<kilometre, min_impl<int>>(2) - length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1999));
static_assert(length<kilometre, min_impl<int>>(2) - length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1998.5));
static_assert(length<kilometre, int>(2) - length<metre, min_impl<int>>(1) == length<metre, min_impl<int>>(1999));
static_assert(length<kilometre, int>(2) - length<metre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(1998.5));
static_assert(length<kilometre, min_impl<int>>(2) - length<metre, int>(1) == length<metre, min_impl<int>>(1999));
static_assert(length<kilometre, min_impl<int>>(2) - length<metre, double>(1.5) == length<metre, min_impl<double>>(1998.5));
static_assert(length<kilometre, min_impl<int>>(2) - length<metre, min_impl<int>>(1) == length<metre, int>(1999));
static_assert(length<kilometre, min_impl<int>>(2) - length<metre, min_impl<double>>(1.5) == length<metre, double>(1998.5));

static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, min_impl<int>>(1) == length<metre, min_impl<int>>(1000));
static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(500));
static_assert(length<metre, int>(2000) - length<kilometre, min_impl<int>>(1) == length<metre, min_impl<int>>(1000));
static_assert(length<metre, int>(2000) - length<kilometre, min_impl<double>>(1.5) == length<metre, min_impl<double>>(500));
static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, int>(1) == length<metre, min_impl<int>>(1000));
static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, double>(1.5) == length<metre, min_impl<double>>(500));
static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, min_impl<int>>(1) == length<metre, int>(1000));
static_assert(length<metre, min_impl<int>>(2000) - length<kilometre, min_impl<double>>(1.5) == length<metre, double>(500));

static_assert(length<metre, min_impl<int>>(123) * min_impl<double>(1.5) == length<metre, min_impl<double>>(184.5));
static_assert(length<metre, min_impl<int>>(123) * 1.5 == length<metre, min_impl<double>>(184.5));
static_assert(length<metre, int>(123) * min_impl<double>(1.5) == length<metre, min_impl<double>>(184.5));

static_assert(length<metre, min_impl<int>>(123) * quantity{min_impl<double>(1.5)} == length<metre, min_impl<double>>(184.5));
static_assert(length<metre, min_impl<int>>(123) * quantity{1.5} == length<metre, min_impl<double>>(184.5));
static_assert(length<metre, int>(123) * quantity{min_impl<double>(1.5)} == length<metre, min_impl<double>>(184.5));

static_assert(min_impl<double>(1.5) * length<metre, min_impl<int>>(123) == length<metre, min_impl<double>>(184.5));
static_assert(min_impl<double>(1.5) * length<metre, int>(123) == length<metre, min_impl<double>>(184.5));
static_assert(1.5 * length<metre, min_impl<int>>(123) == length<metre, min_impl<double>>(184.5));

static_assert(quantity{min_impl<double>(1.5)} * length<metre, min_impl<int>>(123) == length<metre, min_impl<double>>(184.5));
static_assert(quantity{min_impl<double>(1.5)} * length<metre, int>(123) == length<metre, min_impl<double>>(184.5));
static_assert(quantity{1.5} * length<metre, min_impl<int>>(123) == length<metre, min_impl<double>>(184.5));

static_assert(length<metre, min_impl<int>>(123) / min_impl<double>(2.) == length<metre, min_impl<double>>(61.5));
static_assert(length<metre, min_impl<int>>(123) / 2. == length<metre, min_impl<double>>(61.5));
static_assert(length<metre, int>(123) / min_impl<double>(2.) == length<metre, min_impl<double>>(61.5));

static_assert(length<metre, min_impl<int>>(123) / quantity{min_impl<double>(2.)} == length<metre, min_impl<double>>(61.5));
static_assert(length<metre, min_impl<int>>(123) / quantity{2.} == length<metre, min_impl<double>>(61.5));
static_assert(length<metre, int>(123) / quantity{min_impl<double>(2.)} == length<metre, min_impl<double>>(61.5));

static_assert(length<metre, min_impl<int>>(123) / length<metre, min_impl<double>>(2.) == 61.5);
static_assert(length<metre, min_impl<int>>(123) / length<metre, double>(2.) == 61.5);
static_assert(length<metre, int>(123) / length<metre, min_impl<double>>(2.) == 61.5);

static_assert(length<metre, min_impl<int>>(123) % min_impl<int>(100) == length<metre, int>(23));
static_assert(length<metre, min_impl<int>>(123) % 100 == length<metre, int>(23));
static_assert(length<metre, int>(123) % min_impl<int>(100) == length<metre, int>(23));

static_assert(length<metre, min_impl<int>>(123) % quantity{min_impl<int>(100)} == length<metre, int>(23));
static_assert(length<metre, min_impl<int>>(123) % quantity{100} == length<metre, int>(23));
static_assert(length<metre, int>(123) % quantity{min_impl<int>(100)} == length<metre, int>(23));

}  // namespace
