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

template<Dimension D, UnitOf<D> U>
struct reference;

template<Dimension D, UnitOf<D> U, Representation Rep>
class quantity;

template<PointOrigin O, UnitOf<typename O::dimension> U, Representation Rep>
class quantity_point;

template<Kind K, UnitOf<typename K::dimension> U, Representation Rep>
class quantity_kind;

template<PointKind PK, UnitOf<typename PK::dimension> U, Representation Rep>
class quantity_point_kind;

namespace detail {

template<typename R1, typename R2>
struct common_quantity_reference_impl;

template<typename D, typename U>
struct common_quantity_reference_impl<reference<D, U>, reference<D, U>> {
  using type = reference<D, U>;
};

template<typename D, typename U1, typename U2>
struct common_quantity_reference_impl<reference<D, U1>, reference<D, U2>> {
  using type = reference<D, downcast_unit<D, common_magnitude(U1::mag, U2::mag)>>;
};

template<typename D1, typename U1, typename D2, typename U2>
  requires(same_unit_reference<dimension_unit<D1>, dimension_unit<D2>>::value)
struct common_quantity_reference_impl<reference<D1, U1>, reference<D2, U2>> {
  using type = reference<D1, downcast_unit<D1, common_magnitude(U1::mag, U2::mag)>>;
};

template<typename D1, typename U1, typename D2, typename U2>
struct common_quantity_reference_impl<reference<D1, U1>, reference<D2, U2>> {
  static constexpr UNITS_CONSTRAINED_AUTO_WORKAROUND(Magnitude) auto mag = common_magnitude(reference<D1, U1>::mag,
                                                                                            reference<D2, U2>::mag);
  using dimension = conditional<is_specialization_of<D1, unknown_dimension>, D2, D1>;
  using unit = downcast_unit<dimension, mag / dimension::mag>;
  using type = reference<dimension, unit>;
};

template<Quantity Q1, QuantityEquivalentTo<Q1> Q2>
using common_quantity_reference =
  TYPENAME detail::common_quantity_reference_impl<std::remove_const_t<decltype(Q1::reference)>,
                                                  std::remove_const_t<decltype(Q2::reference)>>::type;

}  // namespace detail
}  // namespace units

namespace std {

template<units::Quantity Q1, units::QuantityEquivalentTo<Q1> Q2>
  requires requires { typename common_type_t<typename Q1::rep, typename Q2::rep>; }
struct common_type<Q1, Q2> {
private:
  using ref = units::detail::common_quantity_reference<Q1, Q2>;
public:
  using type =
    units::quantity<typename ref::dimension, typename ref::unit, common_type_t<typename Q1::rep, typename Q2::rep>>;
};

template<units::QuantityPoint QP1, units::QuantityPointEquivalentTo<QP1> QP2>
  requires requires { typename common_type_t<typename QP1::rep, typename QP2::rep>; }
struct common_type<QP1, QP2> {
  using type =
    units::quantity_point<units::rebind_point_origin_dimension<
                            typename QP1::origin, typename common_type_t<typename QP1::quantity_type,
                                                                         typename QP2::quantity_type>::dimension>,
                          typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::unit,
                          typename common_type_t<typename QP1::quantity_type, typename QP2::quantity_type>::rep>;
};

template<units::QuantityKind QK1, units::QuantityKindEquivalentTo<QK1> QK2>
  requires requires { typename common_type_t<typename QK1::rep, typename QK2::rep>; }
struct common_type<QK1, QK2> {
  using type =
    units::quantity_kind<typename QK1::kind_type,
                         typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::unit,
                         typename common_type_t<typename QK1::quantity_type, typename QK2::quantity_type>::rep>;
};

template<units::QuantityPointKind QPK1, units::QuantityPointKindEquivalentTo<QPK1> QPK2>
  requires requires { typename common_type_t<typename QPK1::rep, typename QPK2::rep>; }
struct common_type<QPK1, QPK2> {
  using type = units::quantity_point_kind<
    typename QPK1::point_kind_type,
    typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::unit,
    typename common_type_t<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>::rep>;
};

}  // namespace std
