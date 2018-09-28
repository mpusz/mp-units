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

  template<Dimension D, typename Rep, mp::Ratio R>
  class quantity;

  namespace detail {

    template<typename T>
    struct is_quantity : std::false_type {
    };

    template<Dimension D, typename Rep, mp::Ratio R>
    struct is_quantity<quantity<D, Rep, R>> : std::true_type {
    };

  }

  template<typename T>
  concept bool Quantity = detail::is_quantity<T>::value;

  // treat_as_floating_point

  template<class Rep>
  struct treat_as_floating_point : std::is_floating_point<Rep> {};

  template<class Rep>
  inline constexpr bool treat_as_floating_point_v = treat_as_floating_point<Rep>::value;

  // quantity_cast

  namespace detail {

    template<Quantity To, mp::Ratio CR, typename CRep, bool NumIsOne = false, bool DenIsOne = false>
    struct quantity_cast_impl {
      template<Dimension D, typename Rep, mp::Ratio R>
      static constexpr To cast(const quantity<D, Rep, R>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num) /
                                                static_cast<CRep>(CR::den)));
      }
    };

    template<Quantity To, mp::Ratio CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, true, true> {
      template<Dimension D, typename Rep, mp::Ratio R>
      static constexpr To cast(const quantity<D, Rep, R>& q)
      {
        return To(static_cast<typename To::rep>(q.count()));
      }
    };

    template<Quantity To, mp::Ratio CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, true, false> {
      template<Dimension D, typename Rep, mp::Ratio R>
      static constexpr To cast(const quantity<D, Rep, R>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CR::den)));
      }
    };

    template<Quantity To, mp::Ratio CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, false, true> {
      template<Dimension D, typename Rep, mp::Ratio R>
      static constexpr To cast(const quantity<D, Rep, R>& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num)));
      }
    };

  }  // namespace detail

  template<Quantity To, Dimension D, typename Rep, mp::Ratio R>
    requires mp::Same<typename To::dimension, D>
  constexpr To quantity_cast(const quantity<D, Rep, R>& q)
  {
    using c_ratio = std::ratio_divide<R, typename To::ratio>;
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

  template<Dimension D, typename Rep, mp::Ratio R = std::ratio<1>>
  class quantity {
    Rep value_;
  public:
    using dimension = D;
    using rep = Rep;
    using ratio = R;

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
  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>
  constexpr operator+(const quantity<D, Rep1, R1>& lhs,
                      const quantity<D, Rep2, R2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>
  constexpr operator-(const quantity<D, Rep1, R1>& lhs,
                      const quantity<D, Rep2, R2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<Dimension D, typename Rep1, mp::Ratio R, typename Rep2>
  quantity<D, std::common_type_t<Rep1, Rep2>, R>
  constexpr operator*(const quantity<D, Rep1, R>& q,
                      const Rep2& v)
  {
    using ret = quantity<D, std::common_type_t<Rep1, Rep2>, R>;
    return ret(ret(q).count() * v);
  }

  template<typename Rep1, Dimension D, typename Rep2, mp::Ratio R>
  quantity<D, std::common_type_t<Rep1, Rep2>, R>
  constexpr operator*(const Rep1& v,
                      const quantity<D, Rep2, R>& q)
  {
    return q * v;
  }

  template<Dimension D1, typename Rep1, mp::Ratio R1, Dimension D2, typename Rep2, mp::Ratio R2>
      requires treat_as_floating_point_v<std::common_type_t<Rep1, Rep2>> || std::ratio_multiply<R1, R2>::den == 1
  quantity<dimension_multiply_t<D1, D2>, std::common_type_t<Rep1, Rep2>, std::ratio_multiply<R1, R2>>
  constexpr operator*(const quantity<D1, Rep1, R1>& lhs,
                      const quantity<D2, Rep2, R2>& rhs)
  {
    using ret = quantity<dimension_multiply_t<D1, D2>, std::common_type_t<Rep1, Rep2>, std::ratio_multiply<R1, R2>>;
    return ret(lhs.count() * rhs.count());
  }

  template<typename Rep1, Exponent E, mp::Ratio R, typename Rep2>
  quantity<dimension<exp_invert_t<E>>, std::common_type_t<Rep1, Rep2>, R>
  constexpr operator/(const Rep1& v,
                      const quantity<dimension<E>, Rep2, R>& q)
  {
    using ret = quantity<dimension<exp_invert_t<E>>, std::common_type_t<Rep1, Rep2>, R>;
    using den = quantity<dimension<E>, std::common_type_t<Rep1, Rep2>, R>;
    return ret(v / den(q).count());
  }

  template<Dimension D, typename Rep1, mp::Ratio R, typename Rep2>
  quantity<D, std::common_type_t<Rep1, Rep2>, R>
  constexpr operator/(const quantity<D, Rep1, R>& q,
                      const Rep2& v)
  {
    using ret = quantity<D, std::common_type_t<Rep1, Rep2>, R>;
    return ret(ret(q).count() / v);
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  std::common_type_t<Rep1, Rep2>
  constexpr operator/(const quantity<D, Rep1, R1>& lhs,
                      const quantity<D, Rep2, R2>& rhs)
  {
    using cq = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return cq(lhs).count() / cq(rhs).count();
  }

  template<Dimension D1, typename Rep1, mp::Ratio R1, Dimension D2, typename Rep2, mp::Ratio R2>
      requires treat_as_floating_point_v<std::common_type_t<Rep1, Rep2>> || std::ratio_divide<R1, R2>::den == 1
  quantity<dimension_divide_t<D1, D2>, std::common_type_t<Rep1, Rep2>, std::ratio_divide<R1, R2>>
  constexpr operator/(const quantity<D1, Rep1, R1>& lhs,
                      const quantity<D2, Rep2, R2>& rhs)
  {
    using ret = quantity<dimension_divide_t<D1, D2>, std::common_type_t<Rep1, Rep2>, std::ratio_divide<R1, R2>>;
    return ret(lhs.count() / rhs.count());
  }

  template<Dimension D, typename Rep1, mp::Ratio R, typename Rep2>
  quantity<D, std::common_type_t<Rep1, Rep2>, R>
  constexpr operator%(const quantity<D, Rep1, R>& q,
                      const Rep2& v)
  {
    using ret = quantity<D, std::common_type_t<Rep1, Rep2>, R>;
    return ret(ret(q).count() % v);
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>
  constexpr operator%(const quantity<D, Rep1, R1>& lhs,
                      const quantity<D, Rep2, R2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  // clang-format on

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator==(const quantity<D, Rep1, R1>& lhs,
                            const quantity<D, Rep2, R2>& rhs)
  {
    using ct = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return ct(lhs).count() == ct(rhs).count();
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator!=(const quantity<D, Rep1, R1>& lhs,
                            const quantity<D, Rep2, R2>& rhs)
  {
    return !(lhs == rhs);
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator<(const quantity<D, Rep1, R1>& lhs,
                           const quantity<D, Rep2, R2>& rhs)
  {
    using ct = std::common_type_t<quantity<D, Rep1, R1>, quantity<D, Rep2, R2>>;
    return ct(lhs).count() < ct(rhs).count();
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator<=(const quantity<D, Rep1, R1>& lhs,
                            const quantity<D, Rep2, R2>& rhs)
  {
    return !(rhs < lhs);
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator>(const quantity<D, Rep1, R1>& lhs,
                           const quantity<D, Rep2, R2>& rhs)
  {
    return rhs < lhs;
  }

  template<Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  constexpr bool operator>=(const quantity<D, Rep1, R1>& lhs,
                            const quantity<D, Rep2, R2>& rhs)
  {
    return !(lhs < rhs);
  }

}  // namespace units

namespace std {

  // todo: simplified
  template<units::Dimension D, typename Rep1, mp::Ratio R1, typename Rep2, mp::Ratio R2>
  struct common_type<units::quantity<D, Rep1, R1>, units::quantity<D, Rep2, R2>> {
    using type =
        units::quantity<D, std::common_type_t<Rep1, Rep2>, mp::common_ratio_t<R1, R2>>;
  };

}  // namespace std
