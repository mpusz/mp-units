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

#include <gsl/gsl-lite.hpp>
#include <units/bits/external/hacks.h>
#include <units/bits/math_concepts.h>
#include <units/bits/ratio_maths.h>

namespace units::detail {

struct decimal_fp {
  double significant;
  std::intmax_t exponent;
};

[[nodiscard]] constexpr decimal_fp to_decimal(double v) noexcept
{
  UNITS_DIAGNOSTIC_PUSH
  UNITS_DIAGNOSTIC_IGNORE_FLOAT_EQUAL
  if (v == 0) {
    return {.significant = 0.0, .exponent = 0};
  }
  UNITS_DIAGNOSTIC_POP

  double significant = abs(v);
  std::intmax_t exponent = 0;

  while (significant < 1) {
    significant *= 10.0;
    --exponent;
  }

  while (significant >= 10) {
    significant /= 10.0;
    ++exponent;
  }

  if (v < 0) {
    significant = -significant;
  }

  return {.significant = significant, .exponent = exponent};
}

/* approximate natural log as https://math.stackexchange.com/a/977836
 far slower than std::log but works at compile time with similar accuracy
 */
[[nodiscard]] constexpr double constexpr_log(double v) noexcept
{
  gsl_Expects(v > 0);

  // lookup table to speed up convergence for all significant values
  // significant values of 7 and greater benefit mostly as they now converge in 5 terms compared to O(10)-O(100)
  // required without the table
  //
  // using python:
  // >>> import math
  // >>> for i in range(1, 100):
  // ...     print(f"/* log({i:>2d}) = */ {math.log(i):.16f},")
  constexpr std::array<double, 99> log_table{
    /* log( 1) = */ 0.0000000000000000,
    /* log( 2) = */ 0.6931471805599453,
    /* log( 3) = */ 1.0986122886681098,
    /* log( 4) = */ 1.3862943611198906,
    /* log( 5) = */ 1.6094379124341003,
    /* log( 6) = */ 1.7917594692280550,
    /* log( 7) = */ 1.9459101490553132,
    /* log( 8) = */ 2.0794415416798357,
    /* log( 9) = */ 2.1972245773362196,
    /* log(10) = */ 2.3025850929940459,
    /* log(11) = */ 2.3978952727983707,
    /* log(12) = */ 2.4849066497880004,
    /* log(13) = */ 2.5649493574615367,
    /* log(14) = */ 2.6390573296152584,
    /* log(15) = */ 2.7080502011022101,
    /* log(16) = */ 2.7725887222397811,
    /* log(17) = */ 2.8332133440562162,
    /* log(18) = */ 2.8903717578961645,
    /* log(19) = */ 2.9444389791664403,
    /* log(20) = */ 2.9957322735539909,
    /* log(21) = */ 3.0445224377234230,
    /* log(22) = */ 3.0910424533583161,
    /* log(23) = */ 3.1354942159291497,
    /* log(24) = */ 3.1780538303479458,
    /* log(25) = */ 3.2188758248682006,
    /* log(26) = */ 3.2580965380214821,
    /* log(27) = */ 3.2958368660043291,
    /* log(28) = */ 3.3322045101752038,
    /* log(29) = */ 3.3672958299864741,
    /* log(30) = */ 3.4011973816621555,
    /* log(31) = */ 3.4339872044851463,
    /* log(32) = */ 3.4657359027997265,
    /* log(33) = */ 3.4965075614664802,
    /* log(34) = */ 3.5263605246161616,
    /* log(35) = */ 3.5553480614894135,
    /* log(36) = */ 3.5835189384561099,
    /* log(37) = */ 3.6109179126442243,
    /* log(38) = */ 3.6375861597263857,
    /* log(39) = */ 3.6635616461296463,
    /* log(40) = */ 3.6888794541139363,
    /* log(41) = */ 3.7135720667043080,
    /* log(42) = */ 3.7376696182833684,
    /* log(43) = */ 3.7612001156935624,
    /* log(44) = */ 3.7841896339182610,
    /* log(45) = */ 3.8066624897703196,
    /* log(46) = */ 3.8286413964890951,
    /* log(47) = */ 3.8501476017100584,
    /* log(48) = */ 3.8712010109078911,
    /* log(49) = */ 3.8918202981106265,
    /* log(50) = */ 3.9120230054281460,
    /* log(51) = */ 3.9318256327243257,
    /* log(52) = */ 3.9512437185814275,
    /* log(53) = */ 3.9702919135521220,
    /* log(54) = */ 3.9889840465642745,
    /* log(55) = */ 4.0073331852324712,
    /* log(56) = */ 4.0253516907351496,
    /* log(57) = */ 4.0430512678345503,
    /* log(58) = */ 4.0604430105464191,
    /* log(59) = */ 4.0775374439057197,
    /* log(60) = */ 4.0943445622221004,
    /* log(61) = */ 4.1108738641733114,
    /* log(62) = */ 4.1271343850450917,
    /* log(63) = */ 4.1431347263915326,
    /* log(64) = */ 4.1588830833596715,
    /* log(65) = */ 4.1743872698956368,
    /* log(66) = */ 4.1896547420264252,
    /* log(67) = */ 4.2046926193909657,
    /* log(68) = */ 4.2195077051761070,
    /* log(69) = */ 4.2341065045972597,
    /* log(70) = */ 4.2484952420493594,
    /* log(71) = */ 4.2626798770413155,
    /* log(72) = */ 4.2766661190160553,
    /* log(73) = */ 4.2904594411483910,
    /* log(74) = */ 4.3040650932041702,
    /* log(75) = */ 4.3174881135363101,
    /* log(76) = */ 4.3307333402863311,
    /* log(77) = */ 4.3438054218536841,
    /* log(78) = */ 4.3567088266895917,
    /* log(79) = */ 4.3694478524670215,
    /* log(80) = */ 4.3820266346738812,
    /* log(81) = */ 4.3944491546724391,
    /* log(82) = */ 4.4067192472642533,
    /* log(83) = */ 4.4188406077965983,
    /* log(84) = */ 4.4308167988433134,
    /* log(85) = */ 4.4426512564903167,
    /* log(86) = */ 4.4543472962535073,
    /* log(87) = */ 4.4659081186545837,
    /* log(88) = */ 4.4773368144782069,
    /* log(89) = */ 4.4886363697321396,
    /* log(90) = */ 4.4998096703302650,
    /* log(91) = */ 4.5108595065168497,
    /* log(92) = */ 4.5217885770490405,
    /* log(93) = */ 4.5325994931532563,
    /* log(94) = */ 4.5432947822700038,
    /* log(95) = */ 4.5538768916005408,
    /* log(96) = */ 4.5643481914678361,
    /* log(97) = */ 4.5747109785033828,
    /* log(98) = */ 4.5849674786705723,
    /* log(99) = */ 4.5951198501345898,
  };
  decimal_fp x = to_decimal(v);

  // dividing the significant by nearest lower value in [1.0, 1.1, 1.2, ..., 9.9] will greatly improve convergence
  x.significant *= 10;
  const auto isignificant = static_cast<std::size_t>(x.significant);
  x.significant /= static_cast<double>(isignificant);
  const double result = static_cast<double>(x.exponent - 1) * log_table[9] + log_table[isignificant - 1];

  // 1.0 <= significant < 1.1 converges rapidly
  const double y = (x.significant - 1) / (x.significant + 1);
  const double y_squared = y * y;
  double sum = 0;
  // 5 terms are needed for convergence to machine precision in the worst case scenario
  for (int k = 4; k > 0; --k) {
    sum = y_squared * (1 / (2 * static_cast<double>(k) + 1) + sum);
  }
  sum = 2 * y * (1 + sum);  // k = 0 term
  return result + sum;
}

/* approximate e^x as Taylor series e^x = 1 + x/1! + x^2/2! + x^3/3! +... where N is the order of the Taylor series
 use https://math.stackexchange.com/a/1988927 to improve convergence for large values

 larger Factor values improve convergence for all values but reduce the precision
*/
template<std::size_t N, std::intmax_t Factor = 256>
  requires gt_zero<Factor>
[[nodiscard]] constexpr double constexpr_exp(double v) noexcept
{
  if constexpr (N == 0) {
    return 1.0;
  } else {
    constexpr auto coefficients = []() {
      std::array<double, N> coeffs;
      std::size_t factorial = 1;
      for (std::size_t i = 0; i < N; ++i) {
        factorial *= i + 1;
        coeffs[i] = 1.0 / static_cast<double>(factorial);
      }
      return coeffs;
    }();

    const double x = v / static_cast<double>(Factor);
    double result = 0;
    for (auto i = static_cast<std::intmax_t>(N - 1); i >= 0; --i) {
      result = x * (coefficients[static_cast<std::size_t>(i)] + result);
    }

    // for factors of power of 2 this should be replaced by log2(Factor) multiplications by the compiler
    return pow_impl<Factor>(1 + result);
  }
}

// default template arguments provide reasonable precision even for fairly large exponents
// see constexpr_exp for template arguments
template<std::size_t ExpOrder = 10, std::intmax_t Factor = 128>
[[nodiscard]] constexpr double constexpr_pow(double v, double exponent) noexcept
{
  const double x = exponent * constexpr_log(v);
  return constexpr_exp<ExpOrder, Factor>(x);
}

}  // namespace units::detail
