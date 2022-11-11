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

#include <units/isq/space_and_time.h>
#include <units/quantity_io.h>
#include <units/si/unit_symbols.h>
#include <units/si/units.h>
#include <cmath>
#include <exception>
#include <iostream>

namespace {

template<class T>
class measurement {
public:
  using value_type = T;

  measurement() = default;

  constexpr explicit measurement(const value_type& val, const value_type& err = {}) : value_(val)
  {
    // it sucks that using declaration cannot be provided for a constructor initializer list
    using namespace std;
    uncertainty_ = abs(err);
  }

  [[nodiscard]] constexpr const value_type& value() const { return value_; }
  [[nodiscard]] constexpr const value_type& uncertainty() const { return uncertainty_; }

  [[nodiscard]] constexpr value_type relative_uncertainty() const { return uncertainty() / value(); }
  [[nodiscard]] constexpr value_type lower_bound() const { return value() - uncertainty(); }
  [[nodiscard]] constexpr value_type upper_bound() const { return value() + uncertainty(); }

  [[nodiscard]] constexpr measurement operator-() const { return measurement(-value(), uncertainty()); }

  [[nodiscard]] friend constexpr measurement operator+(const measurement& lhs, const measurement& rhs)
  {
    using namespace std;
    return measurement(lhs.value() + rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  [[nodiscard]] friend constexpr measurement operator-(const measurement& lhs, const measurement& rhs)
  {
    using namespace std;
    return measurement(lhs.value() - rhs.value(), hypot(lhs.uncertainty(), rhs.uncertainty()));
  }

  [[nodiscard]] friend constexpr measurement operator*(const measurement& lhs, const measurement& rhs)
  {
    const auto val = lhs.value() * rhs.value();
    using namespace std;
    return measurement(val, val * hypot(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
  }

  [[nodiscard]] friend constexpr measurement operator*(const measurement& lhs, const value_type& value)
  {
    const auto val = lhs.value() * value;
    return measurement(val, val * lhs.relative_uncertainty());
  }

  [[nodiscard]] friend constexpr measurement operator*(const value_type& value, const measurement& rhs)
  {
    const auto val = rhs.value() * value;
    return measurement(val, val * rhs.relative_uncertainty());
  }

  [[nodiscard]] friend constexpr measurement operator/(const measurement& lhs, const measurement& rhs)
  {
    const auto val = lhs.value() / rhs.value();
    using namespace std;
    return measurement(val, val * hypot(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
  }

  [[nodiscard]] friend constexpr measurement operator/(const measurement& lhs, const value_type& value)
  {
    const auto val = lhs.value() / value;
    return measurement(val, val * lhs.relative_uncertainty());
  }

  [[nodiscard]] friend constexpr measurement operator/(const value_type& value, const measurement& rhs)
  {
    const auto val = value / rhs.value();
    return measurement(val, val * rhs.relative_uncertainty());
  }

  [[nodiscard]] constexpr auto operator<=>(const measurement&) const = default;

  friend std::ostream& operator<<(std::ostream& os, const measurement& v)
  {
    return os << v.value() << " ± " << v.uncertainty();
  }

private:
  value_type value_{};
  value_type uncertainty_{};
};

}  // namespace


namespace {

static_assert(units::Representation<measurement<double>>);

void example()
{
  using namespace units;
  using namespace units::si::unit_symbols;

  const auto a = measurement{9.8, 0.1} * isq::acceleration[m / s2];
  const auto t = measurement{1.2, 0.1} * isq::time[s];

  const weak_quantity_of<isq::speed> auto v = a * t;
  std::cout << a << " * " << t << " = " << v << " = " << v[km / h] << '\n';

  const auto length = measurement{123., 1.} * isq::length[si::metre];
  std::cout << "10 * " << length << " = " << 10 * length << '\n';
}

}  // namespace

int main()
{
  try {
    example();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
