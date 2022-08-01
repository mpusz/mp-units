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

#include <units/bits/basic_concepts.h>
#include <units/reference.h>
#include <units/unit.h>

namespace units {

namespace detail {

template<typename T, typename U>
struct equivalent_impl : std::false_type {};

template<typename T>
struct equivalent_impl<T, T> : std::true_type {};


// units

template<Unit U1, Unit U2>
struct equivalent_impl<U1, U2> : std::disjunction<std::is_base_of<U1, U2>, std::is_base_of<U2, U1>> {};


// dimensions

template<BaseDimension D1, BaseDimension D2>
struct equivalent_impl<D1, D2> :
    std::conjunction<std::bool_constant<D1::symbol == D2::symbol>,
                     same_unit_reference<typename D1::base_unit, typename D2::base_unit>> {};

template<Exponent E1, Exponent E2>
struct equivalent_exp : std::false_type {};

template<BaseDimension Dim1, std::intmax_t Num, std::intmax_t Den, BaseDimension Dim2>
struct equivalent_exp<exponent<Dim1, Num, Den>, exponent<Dim2, Num, Den>> : equivalent_impl<Dim1, Dim2> {};

template<DerivedDimension D1, DerivedDimension D2>
struct equivalent_derived_dim : std::false_type {};

template<typename... Es1, typename... Es2>
  requires(sizeof...(Es1) == sizeof...(Es2))
struct equivalent_derived_dim<derived_dimension_base<Es1...>, derived_dimension_base<Es2...>> :
    std::conjunction<equivalent_exp<Es1, Es2>...> {};

template<DerivedDimension D1, DerivedDimension D2>
struct equivalent_impl<D1, D2> :
    std::disjunction<std::is_base_of<D1, D2>, std::is_base_of<D2, D1>,
                     equivalent_derived_dim<downcast_base_t<D1>, downcast_base_t<D2>>> {};


// additionally accounts for unknown dimensions
template<Unit U1, Dimension D1, Unit U2, Dimension D2>
struct equivalent_unit :
    std::disjunction<equivalent_impl<U1, U2>, std::bool_constant<reference<D1, U1>::mag == reference<D2, U2>::mag>> {};

// point origins

template<typename T, typename U>
concept EquivalentPointOrigins =
  RebindablePointOriginFor<T, typename U::dimension> && RebindablePointOriginFor<U, typename T::dimension> &&
  std::same_as<T, rebind_point_origin_dimension<U, typename T::dimension>> &&
  std::same_as<U, rebind_point_origin_dimension<T, typename U::dimension>>;

template<PointOrigin T, PointOrigin U>
struct equivalent_impl<T, U> :
    std::bool_constant<EquivalentPointOrigins<T, U> &&
                       equivalent_impl<typename T::dimension, typename U::dimension>::value> {};


// (point) kinds

template<Kind T, Kind U>
struct equivalent_impl<T, U> :
    std::conjunction<std::is_same<typename T::base_kind, typename U::base_kind>,
                     equivalent_impl<typename T::dimension, typename U::dimension>> {};

template<PointKind T, PointKind U>
struct equivalent_impl<T, U> :
    std::conjunction<equivalent_impl<typename T::base_kind, typename U::base_kind>,
                     equivalent_impl<typename T::origin, typename U::origin>> {};


// quantities, quantity points, quantity (point) kinds

template<Quantity Q1, Quantity Q2>
struct equivalent_impl<Q1, Q2> :
    std::conjunction<
      equivalent_impl<typename Q1::dimension, typename Q2::dimension>,
      equivalent_unit<typename Q1::unit, typename Q1::dimension, typename Q2::unit, typename Q2::dimension>> {};

template<QuantityPoint QP1, QuantityPoint QP2>
struct equivalent_impl<QP1, QP2> :
    std::conjunction<equivalent_impl<typename QP1::quantity_type, typename QP2::quantity_type>,
                     equivalent_impl<typename QP1::origin, typename QP2::origin>> {};

template<QuantityKind QK1, QuantityKind QK2>
struct equivalent_impl<QK1, QK2> :
    std::conjunction<equivalent_impl<typename QK1::kind_type, typename QK2::kind_type>,
                     equivalent_impl<typename QK1::quantity_type, typename QK2::quantity_type>> {};

template<QuantityPointKind QPK1, QuantityPointKind QPK2>
struct equivalent_impl<QPK1, QPK2> :
    std::conjunction<equivalent_impl<typename QPK1::quantity_kind_type, typename QPK2::quantity_kind_type>,
                     equivalent_impl<typename QPK1::origin, typename QPK2::origin>> {};

}  // namespace detail

template<typename T, typename U>
inline constexpr bool equivalent = detail::equivalent_impl<T, U>::value;

}  // namespace units
