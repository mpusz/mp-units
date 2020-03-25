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

#include <units/concepts.h>
#include <units/bits/dimension_op.h>
#include <cassert>

namespace units {

constexpr std::intmax_t ipow10(std::intmax_t exp)
{
  assert(exp >= 0);
  if (exp == 0) return 1;
  std::intmax_t result = 1;
  while (exp > 0) {
    result *= 10;
    --exp;
  }
  return result;
}


constexpr long double fpow10(std::intmax_t exp)
{
  if (exp == 0) return 1.0L;
  long double result = 1.0L;
  if (exp < 0) {
    while (exp < 0) {
      result /= 10.0L;
      ++exp;
    }
  } else {
    while (exp > 0) {
      result *= 10.0L;
      --exp;
    }
  }
  return result;
}


// QuantityOf
template<typename T, typename Dim>
concept QuantityOf = Quantity<T> && Dimension<Dim> && equivalent_dim<typename T::dimension, Dim>;

// quantity_cast
namespace detail {

template<typename To, typename CRatio, typename CRep, bool NumIsOne, bool DenIsOne, bool ExpIsZero>
struct quantity_cast_impl;

template<typename To, typename CRatio, typename CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count()));
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(fpow10(CRatio::exp))));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(ipow10(CRatio::exp))));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(ipow10(-CRatio::exp))));
      }
    }
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, true> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                     (static_cast<CRep>(CRatio::num) /
                                      static_cast<CRep>(CRatio::den))));
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, false> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                     static_cast<CRep>(fpow10(CRatio::exp)) *
                                     (static_cast<CRep>(CRatio::num) /
                                      static_cast<CRep>(CRatio::den))));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                      static_cast<CRep>(CRatio::num) *
                                      static_cast<CRep>(ipow10(CRatio::exp)) /
                                      static_cast<CRep>(CRatio::den)));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) *
                                      static_cast<CRep>(CRatio::num) /
                                      (static_cast<CRep>(CRatio::den) *
                                       static_cast<CRep>(ipow10(-CRatio::exp)))));
      }
    }
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CRatio::den)));
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(fpow10(CRatio::exp)) * (CRep{1} / static_cast<CRep>(CRatio::den))));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(ipow10(CRatio::exp)) / static_cast<CRep>(CRatio::den)));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / (static_cast<CRep>(ipow10(-CRatio::exp)) * static_cast<CRep>(CRatio::den))));
      }
    }
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num)));
  }
};

template<typename To, typename CRatio, constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num) * static_cast<CRep>(fpow10(CRatio::exp))));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num) * static_cast<CRep>(ipow10(CRatio::exp))));
      }
      else {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CRatio::num) / static_cast<CRep>(ipow10(-CRatio::exp))));
      }
    }
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10(CRatio::exp)));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(q.count() * ipow10(CRatio::exp)));
      }
      else {
        return To(static_cast<To::rep>(q.count() / ipow10(-CRatio::exp)));
      }
    }
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, true> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() * (CRatio::num / CRatio::den)));
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, false, false> {
  template<typename Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10(CRatio::exp) * (CRatio::num / CRatio::den)));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(q.count() * CRatio::num * ipow10(CRatio::exp) / CRatio::den));
      }
      else {
        return To(static_cast<To::rep>(q.count()) * CRatio::num / (CRatio::den * ipow10(-CRatio::exp)));
      }
    }
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() / CRatio::den));
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, true, false, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * fpow10(CRatio::exp) / CRatio::den));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(q.count() * ipow10(CRatio::exp) / CRatio::den));
      }
      else {
        return To(static_cast<To::rep>(q.count() / (ipow10(-CRatio::exp) * CRatio::den)));
      }
    }
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, true> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    return To(static_cast<To::rep>(q.count() * CRatio::num));
  }
};

template<typename To, typename CRatio, not_constructible_from_integral CRep>
struct quantity_cast_impl<To, CRatio, CRep, false, true, false> {
  template<Quantity Q>
  static constexpr To cast(const Q& q)
  {
    if constexpr (treat_as_floating_point<CRep>) {
      return To(static_cast<To::rep>(q.count() * CRatio::num * fpow10(CRatio::exp)));
    } else {
      if constexpr (CRatio::exp > 0) {
        return To(static_cast<To::rep>(q.count() * CRatio::num * ipow10(CRatio::exp)));
      }
      else {
        return To(static_cast<To::rep>(q.count() * CRatio::num / ipow10(-CRatio::exp)));
      }
    }
  }
};

template<Dimension FromD, Unit FromU, Dimension ToD, Unit ToU>
struct cast_ratio;

template<BaseDimension FromD, Unit FromU, BaseDimension ToD, Unit ToU>
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using type = ratio_divide<typename FromU::ratio, typename ToU::ratio>;
};

template<DerivedDimension FromD, Unit FromU, DerivedDimension ToD, Unit ToU>
  requires same_unit_reference<FromU, ToU>::value
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using type = ratio_divide<typename FromU::ratio, typename ToU::ratio>;
};

template<DerivedDimension FromD, Unit FromU, DerivedDimension ToD, Unit ToU>
struct cast_ratio<FromD, FromU, ToD, ToU> {
  using from_ratio = ratio_multiply<typename FromD::base_units_ratio, typename FromU::ratio>;
  using to_ratio = ratio_multiply<typename ToD::base_units_ratio, typename ToU::ratio>;
  using type = ratio_divide<from_ratio, to_ratio>;
};

}  // namespace detail

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets the target quantity type to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::si::time<units::si::second>>(1q_ms);
 *
 * @tparam To a target quantity type to cast to
 */
template<Quantity To, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires QuantityOf<To, D>
{
  using c_ratio = detail::cast_ratio<D, U, typename To::dimension, typename To::unit>::type;
  using c_rep = std::common_type_t<typename To::rep, Rep>;
  using ret_unit = downcast_unit<typename To::dimension, typename To::unit::ratio>;
  using ret = quantity<typename To::dimension, ret_unit, typename To::rep>;
  using cast = detail::quantity_cast_impl<ret, c_ratio, c_rep, c_ratio::num == 1, c_ratio::den == 1, c_ratio::exp == 0>;
  return cast::cast(q);
}

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target dimension to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::si::acceleration>(200q_Gal);
 *
 * @tparam ToD a dimension type to use for a target quantity
 */
template<Dimension ToD, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires equivalent_dim<ToD, D>
{
  return quantity_cast<quantity<ToD, dimension_unit<ToD>, Rep>>(q);
}

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only the target unit to cast to. For example:
 *
 * auto q1 = units::quantity_cast<units::si::second>(1q_ms);
 *
 * @tparam ToU a unit type to use for a target quantity
 */
template<Unit ToU, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
  requires UnitOf<ToU, D>
{
  return quantity_cast<quantity<D, ToU, Rep>>(q);
}

/**
 * @brief Explcit cast of a quantity
 *
 * Implicit conversions between quantities of different types are allowed only for "safe"
 * (i.e. non-truncating) conversion. In such cases an explicit cast have to be used.
 *
 * This cast gets only representation to cast to. For example:
 *
 * auto q1 = units::quantity_cast<int>(1q_ms);
 *
 * @tparam ToRep a representation type to use for a target quantity
 */
template<Scalar ToRep, typename D, typename U, typename Rep>
[[nodiscard]] constexpr auto quantity_cast(const quantity<D, U, Rep>& q)
{
  return quantity_cast<quantity<D, U, ToRep>>(q);
}

}  // namespace units
