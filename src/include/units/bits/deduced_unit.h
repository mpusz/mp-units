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

#include <units/derived_dimension.h>

namespace units::detail {

// same_scaled_units
template<typename ExpList, in_unit... Us>
inline constexpr bool same_scaled_units = false;

template<typename... Es, in_unit... Us>
inline constexpr bool same_scaled_units<exp_list<Es...>, Us...> = (in_unit_of<Us, typename Es::dimension> && ...);

// deduced_unit
template<typename Result, int UnitExpNum, int UnitExpDen, typename in_unit_ratio>
struct ratio_op;

template<typename Result, int UnitExpDen, typename in_unit_ratio>
struct ratio_op<Result, 0, UnitExpDen, in_unit_ratio> {
  using ratio = Result;
};

template<typename Result, int UnitExpNum, int UnitExpDen, typename in_unit_ratio>
struct ratio_op {
  using calc_ratio =
      conditional<(UnitExpNum * UnitExpDen > 0), ratio_multiply<Result, in_unit_ratio>, ratio_divide<Result, in_unit_ratio>>;
  static constexpr int value = (UnitExpNum * UnitExpDen > 0) ? (UnitExpNum - UnitExpDen) : (UnitExpNum + UnitExpDen);
  using ratio = ratio_op<calc_ratio, value, UnitExpDen, in_unit_ratio>::ratio;
};

template<typename ExpList, in_unit... Us>
struct derived_ratio;

template<in_unit... Us>
struct derived_ratio<exp_list<>, Us...> {
  using ratio = ::units::ratio<1>;
};

template<typename E, typename... ERest, in_unit U, in_unit... URest>
struct derived_ratio<exp_list<E, ERest...>, U, URest...> {
  using rest_ratio = derived_ratio<exp_list<ERest...>, URest...>::ratio;
  using unit_ratio = ratio_op<rest_ratio, E::num, E::den, typename U::ratio>::ratio;
  using ratio = ratio_divide<unit_ratio, typename dimension_unit<typename E::dimension>::ratio>;
};

template<in_derived_dimension D, in_unit... Us>
using deduced_unit =
    scaled_unit<typename detail::derived_ratio<typename D::recipe, Us...>::ratio, typename D::coherent_unit::reference>;

}  // namespace units::detail
