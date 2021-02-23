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
#include <units/bits/equivalent.h>
#include <units/quantity_cast.h>

namespace units {

template<Dimension D, UnitOf<D> U, QuantityValue Rep>
class quantity;

template<Dimension D, UnitOf<D> U, QuantityValue Rep, PointOrigin Orig>
class quantity_point;

template<Kind K, UnitOf<typename K::dimension> U, QuantityValue Rep>
class quantity_kind;

template<PointKind PK, UnitOf<typename PK::dimension> U, QuantityValue Rep, PointOrigin Orig>
class quantity_point_kind;

namespace detail {

template<typename Q1, typename Q2, typename Rep>
struct common_quantity_impl;

template<typename D, typename U, typename Rep1, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U, Rep1>, quantity<D, U, Rep2>, Rep> {
  using type = quantity<D, U, Rep>;
};

template<typename D, typename U1, typename Rep1, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, Rep> {
  using type = quantity<D, downcast_unit<D, common_ratio(U1::ratio, U2::ratio)>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
  requires same_unit_reference<dimension_unit<D1>, dimension_unit<D2>>::value
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using type = quantity<D1, downcast_unit<D1, common_ratio(U1::ratio, U2::ratio)>, Rep>;
};

template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2, typename Rep>
struct common_quantity_impl<quantity<D1, U1, Rep1>, quantity<D2, U2, Rep2>, Rep> {
  using dimension = conditional<is_specialization_of<D1, unknown_dimension>, D2, D1>;
  static constexpr ratio r1 = D1::base_units_ratio * U1::ratio;
  static constexpr ratio r2 = D2::base_units_ratio * U2::ratio;
  static constexpr ratio cr = common_ratio(r1, r2);
  using unit = downcast_unit<dimension, cr / dimension::base_units_ratio>;
  using type = quantity<dimension, unit, Rep>;
};

}  // namespace detail

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2, QuantityValue Rep = std::common_type_t<typename Q1::rep, typename Q2::rep>>
using common_quantity = TYPENAME detail::common_quantity_impl<Q1, Q2, Rep>::type;

template<QuantityPoint QP1, QuantityPointEquivalentTo<QP1> QP2>
using common_quantity_point = std::common_type_t<QP1, QP2>;

template<QuantityKind QK1, QuantityKindEquivalentTo<QK1> QK2>
using common_quantity_kind = std::common_type_t<QK1, QK2>;

template<QuantityPointKind QPK1, QuantityPointKindEquivalentTo<QPK1> QPK2>
using common_quantity_point_kind = std::common_type_t<QPK1, QPK2>;

}  // namespace units

namespace std {

template<units::Quantity Q1, units::QuantityEquivalentTo<Q1> Q2>
  requires requires { typename common_type_t<typename Q1::rep, typename Q2::rep>; }
struct common_type<Q1, Q2> {
  using type = units::common_quantity<Q1, Q2>;
};

template<units::QuantityPoint QP1, units::QuantityPointEquivalentTo<QP1> QP2>
  requires(requires { typename common_type_t<typename QP1::rep, typename QP2::rep>; } &&
           std::is_same_v<typename QP1::origin, typename QP2::origin>)
struct common_type<QP1, QP2> {
  using type = units::quantity_point<typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::dimension,
    typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::unit,
    typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::rep,
    typename QP1::origin>;
};

template<units::QuantityKind QK1, units::QuantityKindEquivalentTo<QK1> QK2>
  requires requires { typename common_type_t<typename QK1::rep, typename QK2::rep>; }
struct common_type<QK1, QK2> {
  using type = units::quantity_kind<typename QK1::kind_type,
    typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::unit,
    typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::rep>;
};

template<units::QuantityPointKind QPK1, units::QuantityPointKindEquivalentTo<QPK1> QPK2>
  requires (requires { typename common_type_t<typename QPK1::rep, typename QPK2::rep>; } &&
            std::is_same_v<typename QPK1::origin, typename QPK2::origin>)
struct common_type<QPK1, QPK2> {
  using type = units::quantity_point_kind<typename QPK1::point_kind_type,
    typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::unit,
    typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::rep,
    typename QPK1::origin>;
};

}  // namespace std
