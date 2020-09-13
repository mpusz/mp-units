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
inline constexpr bool same_exponents_of<unknown_dimension<Es...>, DimTemplate> = requires { typename DimTemplate<unknown_dimension<Es...>, unknown_coherent_unit, typename Es::dimension...>; } && std::same_as<exponent_list<Es...>, typename DimTemplate<unknown_dimension<Es...>, unknown_coherent_unit, typename Es::dimension...>::recipe>;

} // namespace detail

template<typename Dim, template<typename...> typename DimTemplate>
concept EquivalentUnknownDimensionOfT = Dimension<Dim> && is_derived_from_specialization_of<Dim, unknown_dimension> && detail::same_exponents_of<Dim, DimTemplate>;

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
template<typename T, typename Dim>
concept QuantityOf = Quantity<T> && Dimension<Dim> && equivalent<typename T::dimension, Dim>;

}  // namespace units
