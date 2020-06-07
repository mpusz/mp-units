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

#include <units/bits/dimension_op.h>

namespace units {

template<in_dimension D, UnitOf<D> U, in_numeric_value Rep>
class quantity;

namespace detail {

template<typename Q1, typename Q2, typename Rep>
struct common_quantity_impl;

template<typename D, typename U, typename Rep1, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U, Rep1>, quantity<D, U, Rep2>, Rep> {
  using type = quantity<D, U, Rep>;
};

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, Rep> {
  using type = quantity<D, downcast_unit<D, common_ratio<typename U1::ratio, typename U2::ratio>>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
  requires same_unit_reference<dimension_unit<D1>, dimension_unit<D2>>::value
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using type = quantity<D1, downcast_unit<D1, common_ratio<typename U1::ratio, typename U2::ratio>>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using ratio1 = ratio_multiply<typename D1::base_units_ratio, typename U1::ratio>;
  using ratio2 = ratio_multiply<typename D2::base_units_ratio, typename U2::ratio>;
  using type = quantity<D1, downcast_unit<D1, common_ratio<ratio1, ratio2>>, Rep>;
};

}  // namespace detail

template<in_quantity Q1, in_quantity Q2, in_numeric_value Rep = std::common_type_t<typename Q1::rep, typename Q2::rep>>
  requires equivalent_dim<typename Q1::dimension, typename Q2::dimension>
using common_quantity = detail::common_quantity_impl<Q1, Q2, Rep>::type;

}  // namespace units

#if COMP_GCC >= 10

namespace std {

#else

namespace concepts {

#endif

template<units::in_quantity Q1, units::in_quantity Q2>
  requires units::equivalent_dim<typename Q1::dimension, typename Q2::dimension>
struct common_type<Q1, Q2> {
  using type = units::common_quantity<Q1, Q2>;
};

}
