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

#pragma once

#include "bits/tools.h"
#include "dimension.h"
#include <limits>

namespace units {

  // is_quantity

  template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
  class quantity;

  namespace detail {

    template<typename T>
    struct is_quantity : std::false_type {
    };

    template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
    struct is_quantity<quantity<Dimension, Rep, Ratio>> : std::true_type {
    };

  }

  template<typename T>
  bool concept Quantity = detail::is_quantity<T>::value;

  // treat_as_floating_point

  template<class Rep>
  struct treat_as_floating_point : std::is_floating_point<Rep> {};

  template<class Rep>
  inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<Rep>::value;

  // quantity_cast

  namespace detail {

    template<Quantity To, mp::Ratio CRatio, typename CRep, bool NumIsOne = false, bool DenIsOne = false>
    struct quantity_cast_impl {
      template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
      static constexpr To cast(const quantity<Dimension, Rep, Ratio>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num) /
                                                static_cast<CRep>(CRatio::den)));
      }
    };

    template<Quantity To, mp::Ratio CRatio, typename CRep>
    struct quantity_cast_impl<To, CRatio, CRep, true, true> {
      template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
      static constexpr To cast(const quantity<Dimension, Rep, Ratio>& q)
      {
        return To(static_cast<typename To::rep>(q.count()));
      }
    };

    template<Quantity To, mp::Ratio CRatio, typename CRep>
    struct quantity_cast_impl<To, CRatio, CRep, true, false> {
      template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
      static constexpr To cast(const quantity<Dimension, Rep, Ratio>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CRatio::den)));
      }
    };

    template<Quantity To, mp::Ratio CRatio, typename CRep>
    struct quantity_cast_impl<To, CRatio, CRep, false, true> {
      template<Dimension Dimension, typename Rep, mp::Ratio Ratio>
      static constexpr To cast(const quantity<Dimension, Rep, Ratio>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num)));
      }
    };

  }  // namespace detail

  template<Quantity To, Dimension Dimension, typename Rep, mp::Ratio Ratio>
    requires mp::Same<typename To::dimension, Dimension>
  constexpr To quantity_cast(const quantity<Dimension, Rep, Ratio>& q)
  {
    using c_ratio = std::ratio_divide<Ratio, typename To::ratio>;
    using c_rep = std::common_type_t<typename To::rep, Rep, intmax_t>;
    using cast = detail::quantity_cast_impl<To, c_ratio, c_rep, c_ratio::num == 1, c_ratio::den == 1>;
    return cast::cast(q);
  }

  // quantity_values

  template<typename Rep>
  struct quantity_values {
    static constexpr Rep zero() { return Rep(0); }
    static constexpr Rep max() { return std::numeric_limits<Rep>::max(); }
    static constexpr Rep min() { return std::numeric_limits<Rep>::lowest(); }
  };

  // quantity

  template<Dimension Dimension, typename Rep, mp::Ratio Ratio = std::ratio<1>>
  class quantity {
    Rep value_;
  public:
    using dimension = Dimension;
    using rep = Rep;
    using ratio = Ratio;

    static_assert(!detail::is_quantity<Rep>::value, "rep cannot be a quantity");
    static_assert(ratio::num > 0, "ratio must be positive");

    quantity() = default;
    quantity(const quantity&) = default;

    template<class Rep2>
      requires mp::ConvertibleTo<Rep2, rep> && (treat_as_floating_point_v<rep> || !treat_as_floating_point_v<Rep2>)
    constexpr explicit quantity(const Rep2& r) : value_{static_cast<rep>(r)}
    {
    }

    template<class Rep2, mp::Ratio Ratio2>
      requires mp::ConvertibleTo<Rep2, rep> && (treat_as_floating_point_v<rep> ||
               (std::ratio_divide<Ratio2, ratio>::den == 1 && !treat_as_floating_point_v<Rep2>))
    constexpr quantity(const quantity<Dimension, Rep2, Ratio2>& q) : value_{quantity_cast<quantity>(q).count()}
    {
    }

    quantity& operator=(const quantity& other) = default;

    constexpr rep count() const noexcept { return value_; }

    static constexpr quantity zero() { return quantity(quantity_values<Rep>::zero()); }
    static constexpr quantity min() { return quantity(quantity_values<Rep>::min()); }
    static constexpr quantity max() { return quantity(quantity_values<Rep>::max()); }

    constexpr std::common_type_t<quantity> operator+() const { return quantity(*this); }
    constexpr std::common_type_t<quantity> operator-() const { return quantity(-count()); }

    constexpr quantity& operator++()
    {
      ++value_;
      return *this;
    }
    constexpr quantity operator++(int) { return quantity(value_++); }

    constexpr quantity& operator--()
    {
      --value_;
      return *this;
    }
    constexpr quantity operator--(int) { return quantity(value_--); }

    constexpr quantity& operator+=(const quantity& q)
    {
      value_ += q.count();
      return *this;
    }

    constexpr quantity& operator-=(const quantity& q)
    {
      value_ -= q.count();
      return *this;
    }

    constexpr quantity& operator*=(const rep& rhs)
    {
      value_ *= rhs;
      return *this;
    }

    constexpr quantity& operator/=(const rep& rhs)
    {
      value_ /= rhs;
      return *this;
    }

    constexpr quantity& operator%=(const rep& rhs)
    {
      value_ %= rhs;
      return *this;
    }

    constexpr quantity& operator%=(const quantity& q)
    {
      value_ %= q.count();
      return *this;
    }
  };

  // clang-format off
  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>
  constexpr operator+(const quantity<Dimension, Rep1, Ratio1>& lhs,
                      const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using ret = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>
  constexpr operator-(const quantity<Dimension, Rep1, Ratio1>& lhs,
                      const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using ret = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio, typename Rep2>
  quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>
  constexpr operator*(const quantity<Dimension, Rep1, Ratio>& q,
                      const Rep2& v)
  {
    using ret = quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>;
    return ret(ret(q).count() * v);
  }

  template<typename Rep1, Dimension Dimension, typename Rep2, mp::Ratio Ratio>
  quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>
  constexpr operator*(const Rep1& v,
                      const quantity<Dimension, Rep2, Ratio>& q)
  {
    return q * v;
  }

  template<Dimension Dimension1, typename Rep1, mp::Ratio Ratio1, Dimension Dimension2, typename Rep2, mp::Ratio Ratio2>
      requires treat_as_floating_point_v<std::common_type_t<Rep1, Rep2>> || std::ratio_multiply<Ratio1, Ratio2>::den == 1
  quantity<dimension_multiply_t<Dimension1, Dimension2>, std::common_type_t<Rep1, Rep2>, std::ratio_multiply<Ratio1, Ratio2>>
  constexpr operator*(const quantity<Dimension1, Rep1, Ratio1>& lhs,
                      const quantity<Dimension2, Rep2, Ratio2>& rhs)
  {
    using ret = quantity<dimension_multiply_t<Dimension1, Dimension2>, std::common_type_t<Rep1, Rep2>, std::ratio_multiply<Ratio1, Ratio2>>;
    return ret(lhs.count() * rhs.count());
  }

  template<typename Rep1, Exp Exponent, mp::Ratio Ratio, typename Rep2>
  quantity<dimension<exp_invert_t<Exponent>>, std::common_type_t<Rep1, Rep2>, Ratio>
  constexpr operator/(const Rep1& v,
                      const quantity<dimension<Exponent>, Rep2, Ratio>& q)
  {
    using ret = quantity<dimension<exp_invert_t<Exponent>>, std::common_type_t<Rep1, Rep2>, Ratio>;
    using den = quantity<dimension<Exponent>, std::common_type_t<Rep1, Rep2>, Ratio>;
    return ret(v / den(q).count());
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio, typename Rep2>
  quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>
  constexpr operator/(const quantity<Dimension, Rep1, Ratio>& q,
                      const Rep2& v)
  {
    using ret = quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>;
    return ret(ret(q).count() / v);
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  std::common_type_t<Rep1, Rep2>
  constexpr operator/(const quantity<Dimension, Rep1, Ratio1>& lhs,
                      const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using cq = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return cq(lhs).count() / cq(rhs).count();
  }

  template<Dimension Dimension1, typename Rep1, mp::Ratio Ratio1, Dimension Dimension2, typename Rep2, mp::Ratio Ratio2>
      requires treat_as_floating_point_v<std::common_type_t<Rep1, Rep2>> || std::ratio_divide<Ratio1, Ratio2>::den == 1
  quantity<dimension_divide_t<Dimension1, Dimension2>, std::common_type_t<Rep1, Rep2>, std::ratio_divide<Ratio1, Ratio2>>
  constexpr operator/(const quantity<Dimension1, Rep1, Ratio1>& lhs,
                      const quantity<Dimension2, Rep2, Ratio2>& rhs)
  {
    using ret = quantity<dimension_divide_t<Dimension1, Dimension2>, std::common_type_t<Rep1, Rep2>, std::ratio_divide<Ratio1, Ratio2>>;
    return ret(lhs.count() / rhs.count());
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio, typename Rep2>
  quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>
  constexpr operator%(const quantity<Dimension, Rep1, Ratio>& q,
                      const Rep2& v)
  {
    using ret = quantity<Dimension, std::common_type_t<Rep1, Rep2>, Ratio>;
    return ret(ret(q).count() % v);
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>
  constexpr operator%(const quantity<Dimension, Rep1, Ratio1>& lhs,
                      const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using ret = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  // clang-format on

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator==(const quantity<Dimension, Rep1, Ratio1>& lhs,
                            const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using ct = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return ct(lhs).count() == ct(rhs).count();
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator!=(const quantity<Dimension, Rep1, Ratio1>& lhs,
                            const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    return !(lhs == rhs);
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator<(const quantity<Dimension, Rep1, Ratio1>& lhs,
                           const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    using ct = std::common_type_t<quantity<Dimension, Rep1, Ratio1>, quantity<Dimension, Rep2, Ratio2>>;
    return ct(lhs).count() < ct(rhs).count();
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator<=(const quantity<Dimension, Rep1, Ratio1>& lhs,
                            const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    return !(rhs < lhs);
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator>(const quantity<Dimension, Rep1, Ratio1>& lhs,
                           const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    return rhs < lhs;
  }

  template<Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  constexpr bool operator>=(const quantity<Dimension, Rep1, Ratio1>& lhs,
                            const quantity<Dimension, Rep2, Ratio2>& rhs)
  {
    return !(lhs < rhs);
  }

}  // namespace units

namespace std {

  // todo: simplified
  template<units::Dimension Dimension, typename Rep1, mp::Ratio Ratio1, typename Rep2, mp::Ratio Ratio2>
  struct common_type<units::quantity<Dimension, Rep1, Ratio1>, units::quantity<Dimension, Rep2, Ratio2>> {
    using type =
        units::quantity<Dimension, std::common_type_t<Rep1, Rep2>, mp::common_ratio_t<Ratio1, Ratio2>>;
  };

}  // namespace std
