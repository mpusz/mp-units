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
#include <units/unit.h>

namespace units {

namespace detail {

template<typename T, typename U>
struct equivalent_impl : std::false_type {
};

// units

template<Unit U1, Unit U2>
struct equivalent_impl<U1, U2> : std::disjunction<std::is_same<U1, U2>, std::is_base_of<U1, U2>, std::is_base_of<U2, U1>> {};


// dimensions

template<BaseDimension D1, BaseDimension D2>
struct equivalent_base_dim :
    std::conjunction<std::bool_constant<D1::symbol == D2::symbol>,
                     same_unit_reference<typename D1::base_unit, typename D2::base_unit>> {
};

template<BaseDimension D1, BaseDimension D2>
struct equivalent_impl<D1, D2> : std::disjunction<std::is_same<D1, D2>, equivalent_base_dim<D1, D2>> {
};

template<Exponent E1, Exponent E2>
struct equivalent_exp : std::false_type {
};

template<BaseDimension Dim1, std::intmax_t Num, std::intmax_t Den, BaseDimension Dim2>
struct equivalent_exp<exp<Dim1, Num, Den>, exp<Dim2, Num, Den>> : equivalent_impl<Dim1, Dim2> {
};

template<DerivedDimension D1, DerivedDimension D2>
struct equivalent_derived_dim : std::false_type {
};

template<typename... Es1, typename... Es2>
  requires(sizeof...(Es1) == sizeof...(Es2))
struct equivalent_derived_dim<derived_dimension_base<Es1...>, derived_dimension_base<Es2...>> :
    std::conjunction<equivalent_exp<Es1, Es2>...> {
};

template<DerivedDimension D1, DerivedDimension D2>
struct equivalent_impl<D1, D2> :
    std::disjunction<std::is_same<D1, D2>, std::is_base_of<D1, D2>, std::is_base_of<D2, D1>,
                     equivalent_derived_dim<downcast_base_t<D1>, downcast_base_t<D2>>> {
};


// additionally accounts for unknown dimensions
template<Unit U1, Dimension D1, Unit U2, Dimension D2>
struct equivalent_unit : std::disjunction<equivalent_impl<U1, U2>,
                                          std::bool_constant<U1::ratio / dimension_unit<D1>::ratio == U2::ratio / dimension_unit<D2>::ratio>> {};

// quantities and quantity points

template<typename Q1, typename Q2>
  requires (Quantity<Q1> && Quantity<Q2>) || (QuantityPoint<Q1> && QuantityPoint<Q2>)
struct equivalent_impl<Q1, Q2> : std::disjunction<std::is_same<Q1, Q2>,
                                                  std::conjunction<equivalent_impl<typename Q1::dimension, typename Q2::dimension>,
                                                                   equivalent_unit<typename Q1::unit, typename Q1::dimension,
                                                                                   typename Q2::unit, typename Q2::dimension>>> {};

}  // namespace detail

template<typename T, typename U>
inline constexpr bool equivalent = detail::equivalent_impl<T, U>::value;

}  // namespace units
