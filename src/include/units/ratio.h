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

#include <units/bits/hacks.h>
#include <type_traits>
#include <numeric>
#include <cstdint>

namespace std::experimental::units {

  namespace detail {

    template<typename T>
    [[nodiscard]] constexpr T abs(T v) noexcept { return v < 0 ? -v : v; }

  }

  template<std::intmax_t Num, std::intmax_t Den = 1>
  struct ratio {
    static_assert(Den != 0, "zero denominator");
    static_assert(-INTMAX_MAX <= Num, "numerator too negative");
    static_assert(-INTMAX_MAX <= Den, "denominator too negative");

    static constexpr std::intmax_t num = Num * (Den < 0 ? -1 : 1) / std::gcd(Num, Den);
    static constexpr std::intmax_t den = detail::abs(Den) / std::gcd(Num, Den);

    using type = ratio<num, den>;
  };

  // is_ratio

  namespace detail {

    template<typename T>
    inline constexpr bool is_ratio = false;

    template<intmax_t Num, intmax_t Den>
    inline constexpr bool is_ratio<ratio<Num, Den>> = true;

  }  // namespace detail

  template<typename T>
  concept bool Ratio = detail::is_ratio<T>;

  // ratio_multiply

  namespace detail {

    static constexpr std::intmax_t safe_multiply(std::intmax_t lhs, std::intmax_t rhs)
    {
      constexpr std::uintmax_t c = std::uintmax_t(1) << (sizeof(std::intmax_t) * 4);

      const std::uintmax_t a0 = detail::abs(lhs) % c;
      const std::uintmax_t a1 = detail::abs(lhs) / c;
      const std::uintmax_t b0 = detail::abs(rhs) % c;
      const std::uintmax_t b1 = detail::abs(rhs) / c;

      Expects(a1 == 0 || b1 == 0); //  overflow in multiplication
      Expects(a0 * b1 + b0 * a1 < (c >> 1)); // overflow in multiplication
      Expects(b0 * a0 <= INTMAX_MAX); // overflow in multiplication
      Expects((a0 * b1 + b0 * a1) * c <= INTMAX_MAX -  b0 * a0); // overflow in multiplication

      return lhs * rhs;
    }

    template<typename R1, typename R2>
    struct ratio_multiply_impl {
    private:
      static constexpr std::intmax_t gcd1 = std::gcd(R1::num, R2::den);
      static constexpr std::intmax_t gcd2 = std::gcd(R2::num, R1::den);

    public:
      using type = ratio<safe_multiply(R1::num / gcd1, R2::num / gcd2), safe_multiply(R1::den / gcd2, R2::den / gcd1)>;
      static constexpr std::intmax_t num = type::num;
      static constexpr std::intmax_t den = type::den;
    };

  }

  template<Ratio R1, Ratio R2>
  using ratio_multiply = typename detail::ratio_multiply_impl<R1, R2>::type;

  // ratio_divide

  namespace detail {

    template<typename R1, typename R2>
    struct ratio_divide_impl {
      static_assert(R2::num != 0, "division by 0");
      using type = ratio_multiply<R1, ratio<R2::den, R2::num>>;
      static constexpr std::intmax_t num = type::num;
      static constexpr std::intmax_t den = type::den;
    };

  }

  template<Ratio R1, Ratio R2>
  using ratio_divide = typename detail::ratio_divide_impl<R1, R2>::type;

  // common_ratio

  namespace detail {

    // todo: simplified
    template<typename R1, typename R2>
    struct common_ratio_impl {
      static constexpr std::intmax_t gcd_num = std::gcd(R1::num, R2::num);
      static constexpr std::intmax_t gcd_den = std::gcd(R1::den, R2::den);
      using type = ratio<gcd_num, (R1::den / gcd_den) * R2::den>;
    };

  }

  template<Ratio R1, Ratio R2>
  using common_ratio = typename detail::common_ratio_impl<R1, R2>::type;

}  // namespace std::experimental::units
