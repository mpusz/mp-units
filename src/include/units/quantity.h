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

#include <units/bits/fmt.h>
#include <units/bits/concepts.h>
#include <units/unit.h>
#include <units/format.h>
#include <limits>
#include <ostream>
#include <unicode/measunit.h>


namespace units {

  // is_quantity
  namespace detail {

    template<typename T>
    inline constexpr bool is_quantity = false;

    // partial specialization below after the first quantity forward declaration

  }  // namespace detail

  template<typename T>
  concept Quantity = detail::is_quantity<T>;

  template<typename T, typename D>
  concept QuantityOf = Quantity<T> && Dimension<D> && same_dim<typename T::dimension, D>;

  // Scalar

  template<typename T>
  concept Scalar = (!Quantity<T>) && Number<T>;

  template<Unit U, Scalar Rep>
  class quantity;

  namespace detail {

    template<typename U, typename Rep>
    inline constexpr bool is_quantity<quantity<U, Rep>> = true;

  }  // namespace detail

  // common_quantity
  namespace detail {

    template<Quantity Q1, Quantity Q2, Scalar Rep>
    struct common_quantity_impl;

    template<typename U, typename Rep1, typename Rep2, typename Rep>
    struct common_quantity_impl<quantity<U, Rep1>, quantity<U, Rep2>, Rep> {
      using type = quantity<U, Rep>;
    };

    template<typename U1, typename Rep1, typename U2, typename Rep2, typename Rep>
        requires same_dim<typename U1::dimension, typename U2::dimension>
    struct common_quantity_impl<quantity<U1, Rep1>, quantity<U2, Rep2>, Rep> {
      using type =
          quantity<downcast_target<unit<typename U1::dimension, common_ratio<typename U1::ratio, typename U2::ratio>>>,
                  Rep>;
    };

  }  // namespace detail

  template<Quantity Q1, Quantity Q2, Scalar Rep = std::common_type_t<typename Q1::rep, typename Q2::rep>>
  using common_quantity = detail::common_quantity_impl<Q1, Q2, Rep>::type;

  // treat_as_floating_point

  template<typename Rep>  // TODO Conceptify that
  inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

  // quantity_cast

  namespace detail {

    template<typename To, typename CR, typename CRep, bool NumIsOne = false, bool DenIsOne = false>
    struct quantity_cast_impl {
      template<typename Q>
      static constexpr To cast(const Q& q)
      {
        if constexpr(treat_as_floating_point<CRep>) {
          return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                        (static_cast<CRep>(CR::num) / static_cast<CRep>(CR::den))));
        }
        else {
          return To(
              static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num) / static_cast<CRep>(CR::den)));
        }
      }
    };

    template<typename To, typename CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, true, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(q.count()));
      }
    };

    template<typename To, typename CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, true, false> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        if constexpr(treat_as_floating_point<CRep>) {
          return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * (CRep{1} / static_cast<CRep>(CR::den))));
        }
        else {
          return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CR::den)));
        }
      }
    };

    template<typename To, typename CR, typename CRep>
    struct quantity_cast_impl<To, CR, CRep, false, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR::num)));
      }
    };

  }  // namespace detail

  template<Quantity To, typename U, typename Rep>
  [[nodiscard]] constexpr To quantity_cast(const quantity<U, Rep>& q)
      requires same_dim<typename To::dimension, typename U::dimension>
  {
    using c_ratio = ratio_divide<typename U::ratio, typename To::unit::ratio>;
    using c_rep = std::common_type_t<typename To::rep, Rep, intmax_t>;
    using cast = detail::quantity_cast_impl<To, c_ratio, c_rep, c_ratio::num == 1, c_ratio::den == 1>;
    return cast::cast(q);
  }

  template<Unit ToU, Scalar ToRep, typename U, typename Rep>
  [[nodiscard]] constexpr quantity<ToU, ToRep> quantity_cast(const quantity<U, Rep>& q)
  {
    return quantity_cast<quantity<ToU, ToRep>>(q);
  }

  template<Unit ToU, typename U, typename Rep>
  [[nodiscard]] constexpr quantity<ToU, Rep> quantity_cast(const quantity<U, Rep>& q)
  {
    return quantity_cast<quantity<ToU, Rep>>(q);
  }

  template<Scalar ToRep, typename U, typename Rep>
  [[nodiscard]] constexpr quantity<U, ToRep> quantity_cast(const quantity<U, Rep>& q)
  {
    return quantity_cast<quantity<U, ToRep>>(q);
  }

  // quantity_values

  template<Scalar Rep>
  struct quantity_values {
    static constexpr Rep zero() noexcept { return Rep(0); }
    static constexpr Rep one() noexcept { return Rep(1); }
    static constexpr Rep max() noexcept { return std::numeric_limits<Rep>::max(); }
    static constexpr Rep min() noexcept { return std::numeric_limits<Rep>::lowest(); }
  };

  // quantity

  template<Unit U, Scalar Rep = double>
  class quantity {
    Rep value_;

  public:
    using unit = U;
    using rep = Rep;
    using dimension = U::dimension;

    quantity() = default;
    quantity(const quantity&) = default;

    template<Scalar Rep2>
        requires std::convertible_to<Rep2, rep> &&
                (treat_as_floating_point<rep> || (!treat_as_floating_point<Rep2>))
    constexpr explicit quantity(const Rep2& r): value_{static_cast<rep>(r)}
    {
    }

    template<Quantity Q2>
        requires same_dim<dimension, typename Q2::dimension> &&
                std::convertible_to<typename Q2::rep, rep> &&
                (treat_as_floating_point<rep> ||
                  (std::ratio_divide<typename Q2::unit::ratio, typename unit::ratio>::den == 1 &&
                  !treat_as_floating_point<typename Q2::rep>))
    constexpr quantity(const Q2& q): value_{quantity_cast<quantity>(q).count()}
    {
    }

    quantity& operator=(const quantity& other) = default;

    [[nodiscard]] constexpr rep count() const noexcept { return value_; }

    [[nodiscard]] static constexpr quantity zero() noexcept { return quantity(quantity_values<Rep>::zero()); }
    [[nodiscard]] static constexpr quantity one() noexcept { return quantity(quantity_values<Rep>::one()); }
    [[nodiscard]] static constexpr quantity min() noexcept { return quantity(quantity_values<Rep>::min()); }
    [[nodiscard]] static constexpr quantity max() noexcept { return quantity(quantity_values<Rep>::max()); }

    [[nodiscard]] constexpr quantity operator+() const { return quantity(*this); }
    [[nodiscard]] constexpr quantity operator-() const { return quantity(-count()); }

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

    template<Scalar Rep2>
    constexpr quantity& operator%=(const Rep2& rhs)
        requires (!treat_as_floating_point<rep> && !treat_as_floating_point<Rep2>)
    {
      value_ %= rhs;
      return *this;
    }

    constexpr quantity& operator%=(const quantity& q)
        requires (!treat_as_floating_point<rep>)
    {
      value_ %= q.count();
      return *this;
    }

    template<class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const quantity& q)
    {
      os << q.count() << " ";
      if constexpr(!detail::is_unit<quantity::unit>) {
        // print user-defined unit
        os << unit::symbol;
      }
      else {
        using ratio = quantity::unit::ratio;
        using dim = quantity::unit::dimension;
        if constexpr(!detail::is_dimension<dim>) {
          // print as a prefix or ratio of a coherent unit symbol defined by the user
          using coherent_unit = downcast_target<units::unit<dim, units::ratio<1>>>;
          detail::print_prefix_or_ratio<ratio, typename coherent_unit::prefix_type>(os);
          os << coherent_unit::symbol;
        }
        else {
          // print as a ratio of a coherent unit
          detail::print_ratio<ratio>(os);

          // print coherent unit dimensions and their exponents
          detail::print_dimensions(os, dim{});
        }
      }
      return os;
    }
  };

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator+(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    using common_rep = decltype(lhs.count() + rhs.count());
    using ret = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator-(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    using common_rep = decltype(lhs.count() - rhs.count());
    using ret = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

  template<typename U, typename Rep1, Scalar Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator*(const quantity<U, Rep1>& q, const Rep2& v)
      requires (!Quantity<Rep2>)
  {
    using common_rep = decltype(q.count() * v);
    using ret = quantity<U, common_rep>;
    return ret(ret(q).count() * v);
  }

  template<Scalar Rep1, typename U, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator*(const Rep1& v, const quantity<U, Rep2>& q)
      requires (!Quantity<Rep1>)
  {
    return q * v;
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Scalar AUTO operator*(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, dim_invert<typename U2::dimension>>
  {
    using common_rep = decltype(lhs.count() * rhs.count());
    using ratio = ratio_multiply<typename U1::ratio, typename U2::ratio>;
    return common_rep(lhs.count()) * common_rep(rhs.count()) * common_rep(ratio::num) / common_rep(ratio::den);
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator*(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires (!same_dim<typename U1::dimension, dim_invert<typename U2::dimension>>) &&
              (treat_as_floating_point<decltype(lhs.count() * rhs.count())> ||
                (std::ratio_multiply<typename U1::ratio, typename U2::ratio>::den == 1))
  {
    using dim = dimension_multiply<typename U1::dimension, typename U2::dimension>;
    using common_rep = decltype(lhs.count() * rhs.count());
    using ret = quantity<downcast_target<unit<dim, ratio_multiply<typename U1::ratio, typename U2::ratio>>>, common_rep>;
    return ret(lhs.count() * rhs.count());
  }

  template<Scalar Rep1, typename U, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator/(const Rep1& v, const quantity<U, Rep2>& q)
      requires (!Quantity<Rep1>)
  {
    Expects(q != std::remove_cvref_t<decltype(q)>(0));

    using dim = dim_invert<typename U::dimension>;
    using common_rep = decltype(v / q.count());
    using ret = quantity<downcast_target<unit<dim, ratio<U::ratio::den, U::ratio::num>>>, common_rep>;
    using den = quantity<U, common_rep>;
    return ret(v / den(q).count());
  }

  template<typename U, typename Rep1, Scalar Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator/(const quantity<U, Rep1>& q, const Rep2& v)
      requires (!Quantity<Rep2>)
  {
    Expects(v != Rep2{0});

    using common_rep = decltype(q.count() / v);
    using ret = quantity<U, common_rep>;
    return ret(ret(q).count() / v);
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Scalar AUTO operator/(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    Expects(rhs != std::remove_cvref_t<decltype(rhs)>(0));

    using common_rep = decltype(lhs.count() / rhs.count());
    using cq = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>, common_rep>;
    return cq(lhs).count() / cq(rhs).count();
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator/(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires (!same_dim<typename U1::dimension, typename U2::dimension>) &&
              (treat_as_floating_point<decltype(lhs.count() / rhs.count())> ||
                (ratio_divide<typename U1::ratio, typename U2::ratio>::den == 1))
  {
    Expects(rhs != std::remove_cvref_t<decltype(rhs)>(0));

    using common_rep = decltype(lhs.count() / rhs.count());
    using dim = dimension_divide<typename U1::dimension, typename U2::dimension>;
    using ret = quantity<downcast_target<unit<dim, ratio_divide<typename U1::ratio, typename U2::ratio>>>, common_rep>;
    return ret(lhs.count() / rhs.count());
  }

  template<typename U, typename Rep1, Scalar Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator%(const quantity<U, Rep1>& q, const Rep2& v)
  {
    using common_rep = decltype(q.count() % v);
    using ret = quantity<U, common_rep>;
    return ret(ret(q).count() % v);
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr Quantity AUTO operator%(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() % rhs.count());
    using ret = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator==(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    using ct = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>>;
    return ct(lhs).count() == ct(rhs).count();
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator!=(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    return !(lhs == rhs);
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator<(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    using ct = common_quantity<quantity<U1, Rep1>, quantity<U2, Rep2>>;
    return ct(lhs).count() < ct(rhs).count();
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator<=(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    return !(rhs < lhs);
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator>(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    return rhs < lhs;
  }

  template<typename U1, typename Rep1, typename U2, typename Rep2>
  [[nodiscard]] constexpr bool operator>=(const quantity<U1, Rep1>& lhs, const quantity<U2, Rep2>& rhs)
      requires same_dim<typename U1::dimension, typename U2::dimension>
  {
    return !(lhs < rhs);
  }

}  // namespace units

namespace fmt {
  template<typename Qty, typename T>
  struct formatter<units::quantity<Qty, T>> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const units::quantity<Qty, T>& sm, FormatContext& ctx)
    {
      auto locale = ctx.locale().template get<std::locale>();

      UErrorCode uc;
      icu::MeasureUnit* unit = units::details::create_icu_unit<Qty>(uc);
      icu::Measure m(sm.count(), unit, uc);
      return units::details::format_localized_unit(m, ctx);
    }
  };
}  // namespace fmt
