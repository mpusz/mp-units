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

  #include "units/ratio.h"

  namespace {

  using namespace units;

  template<Ratio R1, Ratio R2>
  inline constexpr bool same = R1::num == R2::num && R1::den == R2::den && R1::exp == R2::exp;

  static_assert(same<ratio<2, 4>, ratio<1, 2>>);

  // basic exponents tests
  // note use of ::type is required because template params are changed while stamping out template
  static_assert(std::is_same_v<ratio<2, 40, 1>::type, ratio<1, 20, 1>::type>);
  static_assert(std::is_same_v<ratio<20, 4, -1>::type, ratio<10, 2, -1>::type>);
  static_assert(std::is_same_v<ratio<200, 5>::type, ratio<20'000, 50, -1>::type>);

  static_assert(std::is_same_v<ratio_multiply<ratio<1>, ratio<3, 8>>, ratio<3, 8>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<3, 8>, ratio<1>>, ratio<3, 8>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<4>, ratio<1, 8>>, ratio<1, 2>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<4>, ratio<1, 2>>, ratio<2>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 8>, ratio<2>>, ratio<1, 4>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 2>, ratio<8>>, ratio<4>>);

  // multiply with exponents
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 8, 2>, ratio<2, 1, 4>>, ratio<1, 4, 6>>);
  static_assert(std::is_same_v<ratio_multiply<ratio<1, 2, -4>, ratio<8, 1, 3>>, ratio<4, 1, -1>>);

  static_assert(std::is_same_v<ratio_divide<ratio<4>, ratio<2>>, ratio<2>>);
  static_assert(std::is_same_v<ratio_divide<ratio<2>, ratio<8>>, ratio<1, 4>>);
  static_assert(std::is_same_v<ratio_divide<ratio<1, 8>, ratio<2>>, ratio<1, 16>>);
  static_assert(std::is_same_v<ratio_divide<ratio<6>, ratio<3>>, ratio<2>>);

  // divide with exponents
  static_assert(std::is_same_v<ratio_divide<ratio<1, 8, -6>, ratio<2, 1, -8>>, ratio<1, 16, 2>>);
  static_assert(std::is_same_v<ratio_divide<ratio<6, 1, 4>, ratio<3>>, ratio<2, 1, 4>>);

  static_assert(std::is_same_v<ratio_pow<ratio<2>, 0>, ratio<1>>);
  static_assert(std::is_same_v<ratio_pow<ratio<2>, 1>, ratio<2>>);
  static_assert(std::is_same_v<ratio_pow<ratio<2>, 2>, ratio<4>>);
  static_assert(std::is_same_v<ratio_pow<ratio<2>, 3>, ratio<8>>);
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2>, 0>, ratio<1>>);
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2>, 1>, ratio<1, 2>>);
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2>, 2>, ratio<1, 4>>);
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2>, 3>, ratio<1, 8>>);

  // pow with exponents
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2, 3>, 2>, ratio<1, 4, 6>>);
  static_assert(std::is_same_v<ratio_pow<ratio<1, 2, -6>, 3>, ratio<1, 8, -18>>);

  static_assert(std::is_same_v<ratio_sqrt<ratio<9>>, ratio<3>>);
  static_assert(std::is_same_v<ratio_sqrt<ratio<4>>, ratio<2>>);
  static_assert(std::is_same_v<ratio_sqrt<ratio<1>>, ratio<1>>);
  static_assert(std::is_same_v<ratio_sqrt<ratio<0>>, ratio<0>>);
  static_assert(std::is_same_v<ratio_sqrt<ratio<1, 4>>, ratio<1, 2>>);

  // sqrt with exponents
  static_assert(std::is_same_v<ratio_sqrt<ratio<9, 1, 2>>, ratio<3, 1, 1>>);
  static_assert(std::is_same_v<ratio_sqrt<ratio<4>>, ratio<2>>);

  // unused
  // static_assert(std::is_same_v<units::ratio_add<units::ratio<1, 2, 0>, units::ratio<1, 3, 0>>, units::ratio<5, 6, 0>>);
  // static_assert(std::is_same_v<units::ratio_add<units::ratio<1, 2, 1>, units::ratio<1, 3, 1>>, units::ratio<5, 6, 1>>);
  // static_assert(std::is_same_v<units::ratio_add<units::ratio<3, 8, 2>, units::ratio<2, 7, 2>>, units::ratio<37, 56, 2>>);

  // static_assert(std::is_same_v<units::ratio_add<units::ratio<3, 8, 2>, units::ratio<2, 7, 1>>, units::ratio<226, 56, 1>>);
  // static_assert(std::is_same_v<units::ratio_add<units::ratio<2, 7, 1>, units::ratio<3, 8, 2>>, units::ratio<226, 56, 1>>);

  // static_assert(std::is_same_v<units::ratio_add<units::ratio<3, 8, -2>, units::ratio<2, 7, -1>>, units::ratio<181, 56, -2>>);
  // static_assert(std::is_same_v<units::ratio_add<units::ratio<2, 7, -1>, units::ratio<3, 8, -2>>, units::ratio<181, 56, -2>>);


  // common_ratio
  // note use of ::type is required because template params are changed while stamping out template
  static_assert(std::is_same_v<common_ratio<ratio<1>::type, ratio<1000>>, ratio<1>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<1000>, ratio<1>>::type, ratio<1>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<1>, ratio<1, 1000>>::type, ratio<1, 1000>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<1, 1000>, ratio<1>>::type, ratio<1, 1000>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<100, 1>, ratio<10, 1>>::type, ratio<10, 1>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<100, 1>, ratio<1, 10>>::type, ratio<1, 10>::type>);

  // common ratio with exponents
  static_assert(std::is_same_v<common_ratio<ratio<1>, ratio<1, 1, 3>>::type, ratio<1>::type>);
  static_assert(std::is_same_v<common_ratio<ratio<10, 1, -1>, ratio<1, 1, -3>>::type, ratio<1, 1, -3>::type>);



  }  // namespace
