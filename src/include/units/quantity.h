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

#include <units/unit.h>
#include <units/bits/concepts.h>
#include <limits>
#include <gsl/gsl-lite.hpp>

namespace units {

  // is_quantity

  template<Dimension D, Unit U, Number Rep>
      requires std::experimental::ranges::Same<D, typename U::dimension>
  class quantity;

  namespace detail {

    template<typename T>
    inline constexpr bool is_quantity = false;

    template<Dimension D, Unit U, Number Rep>
    inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

  }  // namespace detail

  template<typename T>
  concept bool Quantity = detail::is_quantity<T>;

  // treat_as_floating_point

  template<typename Rep> // todo Conceptify that
  inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

  // quantity_cast

  namespace detail {

    template<Quantity To, Ratio CR, Number CRep, bool NumIsOne = false, bool DenIsOne = false>
    struct quantity_cast_impl {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num) /
                                                static_cast<CRep>(CR::den)));
      }
    };

    template<Quantity To, Ratio CR, Number CRep>
    struct quantity_cast_impl<To, CR, CRep, true, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<typename To::rep>(q.count()));
      }
    };

    template<Quantity To, Ratio CR, Number CRep>
    struct quantity_cast_impl<To, CR, CRep, true, false> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CR::den)));
      }
    };

    template<Quantity To, Ratio CR, Number CRep>
    struct quantity_cast_impl<To, CR, CRep, false, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<typename To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num)));
      }
    };

  }  // namespace detail

  template<Quantity To, Dimension D, Unit U, Number Rep>
      requires std::experimental::ranges::Same<typename To::dimension, D>
  constexpr To quantity_cast(const quantity<D, U, Rep>& q)
  {
    using c_ratio = std::ratio_divide<typename U::ratio, typename To::unit::ratio>;
    using c_rep = std::common_type_t<typename To::rep, Rep, intmax_t>;
    using cast = detail::quantity_cast_impl<To, c_ratio, c_rep, c_ratio::num == 1, c_ratio::den == 1>;
    return cast::cast(q);
  }

  // quantity_values

  template<Number Rep>
  struct quantity_values {
    static constexpr Rep zero() noexcept { return Rep(0); }
    static constexpr Rep one() noexcept { return Rep(1); }
    static constexpr Rep max() noexcept { return std::numeric_limits<Rep>::max(); }
    static constexpr Rep min() noexcept { return std::numeric_limits<Rep>::lowest(); }
  };

  // quantity

  template<Dimension D, Unit U, Number Rep>
      requires std::experimental::ranges::Same<D, typename U::dimension>
  class quantity {
    Rep value_;

  public:
    using dimension = D;
    using unit = U;
    using rep = Rep;

    static_assert(!Quantity<Rep>, "rep cannot be a quantity");

    quantity() = default;
    quantity(const quantity&) = default;

    template<std::experimental::ranges::ConvertibleTo<rep> Rep2>
        requires treat_as_floating_point<rep> || (!treat_as_floating_point<Rep2>)
    constexpr explicit quantity(const Rep2& r) : value_{static_cast<rep>(r)}
    {
    }

    template<Quantity Q2>
        requires std::experimental::ranges::Same<dimension, typename Q2::dimension> &&
                 std::experimental::ranges::ConvertibleTo<typename Q2::rep, rep> &&
                 (treat_as_floating_point<rep> ||
                   (std::ratio_divide<typename Q2::unit::ratio, typename unit::ratio>::den == 1 &&
                   !treat_as_floating_point<typename Q2::rep>))
    constexpr quantity(const Q2& q) : value_{quantity_cast<quantity>(q).count()}
    {
    }

    quantity& operator=(const quantity& other) = default;

    [[nodiscard]] constexpr rep count() const noexcept { return value_; }

    [[nodiscard]] static constexpr quantity zero() noexcept { return quantity(quantity_values<Rep>::zero()); }
    [[nodiscard]] static constexpr quantity one() noexcept { return quantity(quantity_values<Rep>::one()); }
    [[nodiscard]] static constexpr quantity min() noexcept { return quantity(quantity_values<Rep>::min()); }
    [[nodiscard]] static constexpr quantity max() noexcept { return quantity(quantity_values<Rep>::max()); }

    [[nodiscard]] constexpr quantity operator-() const { return quantity(-count()); }
    [[nodiscard]] constexpr quantity operator+() const { return quantity(*this); }

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
  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr Quantity operator+(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr Quantity operator-(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<Dimension D, Unit U, Number Rep1, Number Rep2>
  [[nodiscard]] quantity<D, U, std::common_type_t<Rep1, Rep2>>
  constexpr operator*(const quantity<D, U, Rep1>& q,
                      const Rep2& v)
  {
    using ret = quantity<D, U, std::common_type_t<Rep1, Rep2>>;
    return ret(ret(q).count() * v);
  }

  template<Number Rep1, Dimension D, Unit U, Number Rep2>
  [[nodiscard]] quantity<D, U, std::common_type_t<Rep1, Rep2>>
  constexpr operator*(const Rep1& v,
                      const quantity<D, U, Rep2>& q)
  {
    return q * v;
  }

  template<Dimension D1, Unit U1, Number Rep1, Dimension D2, Unit U2, Number Rep2>
      requires treat_as_floating_point<std::common_type_t<Rep1, Rep2>> || (std::ratio_multiply<typename U1::ratio, typename U2::ratio>::den == 1)
  /* [[nodiscard]] */ constexpr Quantity operator*(const quantity<D1, U1, Rep1>& lhs,
                                                   const quantity<D2, U2, Rep2>& rhs)
  {
    using dim = dimension_multiply_t<D1, D2>;
    using ret = quantity<dim, upcasting_traits_t<unit<dim, std::ratio_multiply<typename U1::ratio, typename U2::ratio>>>, std::common_type_t<Rep1, Rep2>>;
    return ret(lhs.count() * rhs.count());
  }

  template<Number Rep1, Dimension D, Unit U, Number Rep2>
  [[nodiscard]] quantity<dim_invert_t<D>, upcasting_traits_t<unit<dim_invert_t<D>, std::ratio<U::ratio::den, U::ratio::num>>>, std::common_type_t<Rep1, Rep2>>
  constexpr operator/(const Rep1& v,
                      const quantity<D, U, Rep2>& q)
  {
    Expects(q != std::decay_t<decltype(q)>(0));

    using dim = dim_invert_t<D>;
    using ret = quantity<dim, upcasting_traits_t<unit<dim, std::ratio<U::ratio::den, U::ratio::num>>>, std::common_type_t<Rep1, Rep2>>;
    using den = quantity<D, U, std::common_type_t<Rep1, Rep2>>;
    return ret(v / den(q).count());
  }

  template<Dimension D, Unit U, Number Rep1, Number Rep2>
  [[nodiscard]] quantity<D, U, std::common_type_t<Rep1, Rep2>>
  constexpr operator/(const quantity<D, U, Rep1>& q,
                      const Rep2& v)
  {
    Expects(v != Rep2{0});

    using ret = quantity<D, U, std::common_type_t<Rep1, Rep2>>;
    return ret(ret(q).count() / v);
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr Number operator/(const quantity<D, U1, Rep1>& lhs,
                                           const quantity<D, U2, Rep2>& rhs)
  {
    Expects(rhs != std::decay_t<decltype(rhs)>(0));

    using cq = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return cq(lhs).count() / cq(rhs).count();
  }

  template<Dimension D1, Unit U1, Number Rep1, Dimension D2, Unit U2, Number Rep2>
      requires treat_as_floating_point<std::common_type_t<Rep1, Rep2>> || (std::ratio_divide<typename U1::ratio, typename U2::ratio>::den == 1)
  /* [[nodiscard]] */ constexpr Quantity operator/(const quantity<D1, U1, Rep1>& lhs,
                                                   const quantity<D2, U2, Rep2>& rhs)
  {
    Expects(rhs != std::decay_t<decltype(rhs)>(0));

    using dim = dimension_divide_t<D1, D2>;
    using ret = quantity<dim, upcasting_traits_t<unit<dim, std::ratio_divide<typename U1::ratio, typename U2::ratio>>>, std::common_type_t<Rep1, Rep2>>;
    return ret(lhs.count() / rhs.count());
  }

  template<Dimension D, Unit U, Number Rep1, Number Rep2>
  [[nodiscard]] constexpr Quantity operator%(const quantity<D, U, Rep1>& q,
                                             const Rep2& v)
  {
    using ret = quantity<D, U, std::common_type_t<Rep1, Rep2>>;
    return ret(ret(q).count() % v);
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr Quantity operator%(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using ret = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  // clang-format on

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator==(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    using ct = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ct(lhs).count() == ct(rhs).count();
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator!=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(lhs == rhs);
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator<(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    using ct = std::common_type_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ct(lhs).count() < ct(rhs).count();
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator<=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(rhs < lhs);
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator>(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return rhs < lhs;
  }

  template<Dimension D, Unit U1, Number Rep1, Unit U2, Number Rep2>
  [[nodiscard]] constexpr bool operator>=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(lhs < rhs);
  }

}  // namespace units

namespace std {

  template<units::Dimension D, units::Unit U, units::Number Rep1, units::Number Rep2>
  struct common_type<units::quantity<D, U, Rep1>, units::quantity<D, U, Rep2>> {
    using type = units::quantity<D, U, std::common_type_t<Rep1, Rep2>>;
  };

  // todo: simplified
  template<units::Dimension D, units::Unit U1, units::Number Rep1, units::Unit U2, units::Number Rep2>
  struct common_type<units::quantity<D, U1, Rep1>, units::quantity<D, U2, Rep2>> {
    using type = units::quantity<D, units::upcasting_traits_t<units::unit<D, units::common_ratio_t<typename U1::ratio, typename U2::ratio>>>,
                                 std::common_type_t<Rep1, Rep2>>;
  };

}  // namespace std
