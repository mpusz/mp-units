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

namespace units {

// DimensionOfT
#if UNITS_DOWNCAST_MODE == 0

namespace detail {

template<typename Dim, template<typename...> typename DimTemplate>
inline constexpr bool same_exponents_of = false;

template<Exponent... Es, template<typename...> typename DimTemplate>
inline constexpr bool same_exponents_of<unknown_dimension<Es...>, DimTemplate> =
  requires {
    typename DimTemplate<unknown_dimension<Es...>, unknown_coherent_unit<Es...>, typename Es::dimension...>;
  } && std::same_as<exponent_list<Es...>, typename DimTemplate<unknown_dimension<Es...>, unknown_coherent_unit<Es...>,
                                                               typename Es::dimension...>::recipe>;

}  // namespace detail

template<typename Dim, template<typename...> typename DimTemplate>
concept EquivalentUnknownDimensionOfT = Dimension<Dim> && is_derived_from_specialization_of<Dim, unknown_dimension> &&
                                        detail::same_exponents_of<Dim, DimTemplate>;

#endif

/**
 * @brief A concept matching all dimensions matching provided dimension class template
 *
 * Satisfied by all dimensions being the instantiations derived from the provided dimension
 * class template.
 */
template<typename Dim, template<typename...> typename DimTemplate>
concept DimensionOfT = Dimension<Dim> && (is_derived_from_specialization_of<Dim, DimTemplate>
#if UNITS_DOWNCAST_MODE == 0
                                          || EquivalentUnknownDimensionOfT<Dim, DimTemplate>
#endif
                                         );

/**
 * @brief A concept matching all quantities with provided dimension class template
 *
 * Satisfied by all quantities with a dimension being the instantiation derived from
 * the provided dimension class template.
 */
template<typename Q, template<typename...> typename DimTemplate>
concept QuantityOfT = Quantity<Q> && DimensionOfT<typename Q::dimension, DimTemplate>;

/**
 * @brief A concept matching all quantities with provided dimension
 *
 * Satisfied by all quantities with a dimension being the instantiation derived from
 * the provided dimension type.
 */
template<typename Q, typename Dim>
concept QuantityOf = Quantity<Q> && Dimension<Dim> && equivalent<typename Q::dimension, Dim>;

/**
 * @brief A concept matching two equivalent quantities
 *
 * Satisfied by quantities having equivalent dimensions.
 */
template<typename Q1, typename Q2>
concept QuantityEquivalentTo = Quantity<Q1> && QuantityOf<Q2, typename Q1::dimension>;

/**
 * @brief A concept matching all quantity points of the provided origin
 *
 * Satisfied by all quantity points with an origin equivalent to the provided one.
 */
template<typename QP, typename Orig>
concept QuantityPointOf = QuantityPoint<QP> && PointOrigin<Orig> && equivalent<typename QP::origin, Orig>;

/**
 * @brief A concept matching two equivalent quantity points
 *
 * Satisfied by quantity points having equivalent origins.
 */
template<typename QP1, typename QP2>
concept QuantityPointEquivalentTo = QuantityPoint<QP1> && QuantityPointOf<QP2, typename QP1::origin>;

/**
 * @brief A concept matching only quantity kinds of a specific kind.
 *
 * @tparam QK Quantity kind to verify.
 * @tparam K Kind type to use for verification.
 */
template<typename QK, typename K>
concept QuantityKindOf = QuantityKind<QK> && Kind<K> && equivalent<typename QK::kind_type, K>;

/**
 * @brief A concept matching two equivalent quantity kinds
 *
 * Satisfied by quantity kinds having equivalent kinds.
 */
template<typename QK1, typename QK2>
concept QuantityKindEquivalentTo = QuantityKind<QK1> && QuantityKindOf<QK2, typename QK1::kind_type>;

/**
 * @brief A concept matching only quantity point kinds of a specific point kind.
 *
 * @tparam QPK Quantity point kind to verify.
 * @tparam PK Point kind type to use for verification.
 */
template<typename QPK, typename PK>
concept QuantityPointKindOf = QuantityPointKind<QPK> && PointKind<PK> && equivalent<typename QPK::point_kind_type, PK>;

/**
 * @brief A concept matching two equivalent quantity point kinds
 *
 * Satisfied by quantity point kinds having equivalent kinds.
 */
template<typename QPK1, typename QPK2>
concept QuantityPointKindEquivalentTo =
  QuantityPointKind<QPK1> && QuantityPointKindOf<QPK2, typename QPK1::point_kind_type>;

}  // namespace units
