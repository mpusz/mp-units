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

#include <units/dimensions/acceleration.h>
#include <iostream>

namespace {

  template<typename T, template<typename> typename Trait>
  concept Satisfies = Trait<T>::value;
  
  // root sum of squares
  template<typename T>
  T rss(const T& v1, const T& v2)
  {
    return std::sqrt(std::pow(v1, 2) + std::pow(v2, 2));
  }

  template<class T>
  class measurement {
  public:
    using value_type = T;

    measurement() = default;

    constexpr /* explicit */ measurement(const value_type& val, const value_type& err = {}):   // cannot be explicit as `magma` concept requires implicit conversions :-(
        value_(val), uncertainty_(std::abs(err))
    {
    }

    constexpr const value_type& value() const { return value_; }
    constexpr const value_type& uncertainty() const { return uncertainty_; }

    constexpr value_type relative_uncertainty() const { return uncertainty() / value(); }
    constexpr value_type lower_bound() const { return value() - uncertainty(); }
    constexpr value_type upper_bound() const { return value() + uncertainty(); }

    [[nodiscard]] constexpr measurement operator-() const { return measurement(-value(), uncertainty()); }

    [[nodiscard]] friend constexpr measurement operator+(const measurement& lhs, const measurement& rhs)
    {
      return measurement(lhs.value() + rhs.value(), rss(lhs.uncertainty(), rhs.uncertainty()));
    }

    [[nodiscard]] friend constexpr measurement operator-(const measurement& lhs, const measurement& rhs)
    {
      return measurement(lhs.value() - rhs.value(), rss(lhs.uncertainty(), rhs.uncertainty()));
    }

    [[nodiscard]] friend constexpr measurement operator*(const measurement& lhs, const measurement& rhs)
    {
      const auto val = lhs.value() * rhs.value();
      return measurement(val, val * rss(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
    }

    [[nodiscard]] friend constexpr measurement operator/(const measurement& lhs, const measurement& rhs)
    {
      const auto val = lhs.value() / rhs.value();
      return measurement(val, val * rss(lhs.relative_uncertainty(), rhs.relative_uncertainty()));
    }

    [[nodiscard]] friend constexpr bool operator==(const measurement& lhs, const measurement& rhs)
    {
      return lhs.value()  == rhs.value() && lhs.uncertainty() == rhs.uncertainty();
    }

    [[nodiscard]] friend constexpr bool operator!=(const measurement& lhs, const measurement& rhs)
    {
      return !(lhs == rhs);
    }

    [[nodiscard]] friend constexpr bool operator<(const measurement& lhs, const measurement& rhs)
    {
      return lhs.value() == rhs.value() ? lhs.uncertainty() < rhs.uncertainty() : lhs.value() < rhs.value();
    }

    [[nodiscard]] friend constexpr bool operator>(const measurement& lhs, const measurement& rhs)
    {
      return rhs < lhs;
    }

    [[nodiscard]] friend constexpr bool operator<=(const measurement& lhs, const measurement& rhs)
    {
      return !(rhs < lhs);
    }

    [[nodiscard]] friend constexpr bool operator>=(const measurement& lhs, const measurement& rhs)
    {
      return !(lhs < rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const measurement& v)
    {
      return os << v.value() << " ± " << v.uncertainty();
    }

  private:
    value_type value_{};
    value_type uncertainty_{};
  };

  template<units::Unit U>
  using m_quantity = units::quantity<U, measurement<double>>;

}  // namespace

template<typename T>
inline constexpr bool units::treat_as_floating_point<measurement<T>> = std::is_floating_point_v<T>;


int main()
{
  const auto a = m_quantity<units::metre_per_second_sq>(measurement(9.8, 0.1));
  const auto t = m_quantity<units::second>(measurement(1.2, 0.1));

  units::Velocity AUTO v1 = a * t;
  m_quantity<units::kilometre_per_hour> v2(v1);
  std::cout << a << " * " << t << " = " << v1 << " = " << v2 << '\n';

  m_quantity<units::metre> length(measurement(123., 1.));
  std::cout << "10 * " << length << " = " << 10 * length << '\n';
}
