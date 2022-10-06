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
#include <units/dimension.h>
#include <units/unit.h>
// #include <units/bits/equivalent.h>
// #include <units/quantity_cast.h>

namespace units {

// template<PointOrigin O, UnitOf<typename O::dimension> U, Representation Rep>
// class quantity_point;

// template<Kind K, UnitOf<typename K::dimension> U, Representation Rep>
// class quantity_kind;

// template<PointKind PK, UnitOf<typename PK::dimension> U, Representation Rep>
// class quantity_point_kind;

// TODO common_unit should use common_magnitude(U1::mag, U2::mag)

template<Dimension D1, Dimension D2>
  requires(equivalent(D1{}, D2{}))
using common_dimension = conditional<std::is_same_v<D1, D2>, D1, detail::dim_type<D1>>;

template<Unit U1, Unit U2>
// requires(equivalent<U1, U2>)
using common_unit = U1;

namespace detail {

template<typename R1, typename R2>
struct common_quantity_reference_impl;

template<typename R>
struct common_quantity_reference_impl<R, R> {
  using type = R;
};

template<typename D, typename U1, typename U2>
struct common_quantity_reference_impl<reference<D, U1>, reference<D, U2>> {
  using type = reference<D, common_unit<U1, U2>>;
};

template<typename R1, typename R2>
  requires(equivalent(R1::dimension, R2::dimension))
struct common_quantity_reference_impl<R1, R2> {
  using type = reference<common_dimension<decltype(R1::dimension), decltype(R2::dimension)>,
                         common_unit<decltype(R1::unit), decltype(R2::unit)>>;
};

template<Quantity Q1, quantity_equivalent_to<Q1> Q2>
using common_quantity_reference =
  TYPENAME detail::common_quantity_reference_impl<std::remove_const_t<decltype(Q1::reference)>,
                                                  std::remove_const_t<decltype(Q2::reference)>>::type;

}  // namespace detail
}  // namespace units

namespace std {

template<units::Quantity Q1, units::quantity_equivalent_to<Q1> Q2>
  requires requires { typename common_type_t<typename Q1::rep, typename Q2::rep>; }
struct common_type<Q1, Q2> {
private:
  using ref = units::detail::common_quantity_reference<Q1, Q2>;
public:
  using type = units::quantity<ref{}, common_type_t<typename Q1::rep, typename Q2::rep>>;
};

// template<units::QuantityPoint QP1, units::QuantityPointEquivalentTo<QP1> QP2>
//   requires requires { typename common_type_t<typename QP1::rep, typename QP2::rep>; }
// struct common_type<QP1, QP2> {
//   using type =
//     units::quantity_point<units::rebind_point_origin_dimension<
//                             typename QP1::origin, typename common_type_t<typename QP1::quantity_type,
//                                                                          typename QP2::quantity_type>::dimension>,
//                           typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::unit,
//                           typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::rep>;
// };

// template<units::QuantityKind QK1, units::QuantityKindEquivalentTo<QK1> QK2>
//   requires requires { typename common_type_t<typename QK1::rep, typename QK2::rep>; }
// struct common_type<QK1, QK2> {
//   using type =
//     units::quantity_kind<typename QK1::kind_type,
//                          typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::unit,
//                          typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::rep>;
// };

// template<units::QuantityPointKind QPK1, units::QuantityPointKindEquivalentTo<QPK1> QPK2>
//   requires requires { typename common_type_t<typename QPK1::rep, typename QPK2::rep>; }
// struct common_type<QPK1, QPK2> {
//   using type = units::quantity_point_kind<
//     typename QPK1::point_kind_type,
//     typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::unit,
//     typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::rep>;
// };

}  // namespace std
