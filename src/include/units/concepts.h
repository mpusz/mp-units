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

#include <units/bits/external/downcasting.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/hacks.h>
#include <units/bits/external/numeric_concepts.h>
#include <units/bits/external/type_traits.h>
#include <units/customization_points.h>

namespace units {

namespace detail {

template<typename T, typename U = T>
concept basic_arithmetic = // exposition only
    std::magma<std::ranges::plus, T, U> &&
    std::magma<std::ranges::minus, T, U> &&
    std::magma<std::ranges::times, T, U> &&
    std::magma<std::ranges::divided_by, T, U>;

} // namespace detail

// PrefixType
struct prefix_type;

template<typename T>
concept PrefixType = std::derived_from<T, prefix_type>;

// Prefix
// TODO gcc:92150
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92150
// namespace detail {

//   template<typename T>
//   inline constexpr bool is_prefix = false;

//   template<typename PrefixType, Ratio R, basic_fixed_string Symbol>
//   inline constexpr bool is_prefix<prefix<PrefixType, R, Symbol>> = true;

// }  // namespace detail

template<typename T>
//  concept Prefix = detail::is_prefix<T>;
concept Prefix = true;


namespace detail {

template<typename T>
inline constexpr bool is_ratio = false;

} // namespace detail

template<typename T>
concept Ratio = detail::is_ratio<T>;


// UnitRatio
template<typename R>
concept UnitRatio = Ratio<R> && (R::num * R::den > 0);

// Unit
template<typename U, UnitRatio R>
struct scaled_unit;

template<typename T>
concept Unit = is_derived_from_instantiation<T, scaled_unit>;

// BaseDimension
template<basic_fixed_string Name, Unit U>
struct base_dimension;

namespace detail {

#if __GNUC__ == 9 && __GNUC_MINOR__ < 2

template<typename T>
inline constexpr bool is_base_dimension = true;

#else

template<typename T>
inline constexpr bool is_base_dimension = false;

template<basic_fixed_string Name, typename... Params>
inline constexpr bool is_base_dimension<base_dimension<Name, Params...>> = true;

#endif

}  // namespace detail

template<typename T>
concept BaseDimension = detail::is_base_dimension<typename T::base_type_workaround>;

// Exponent
namespace detail {

template<typename T>
inline constexpr bool is_exp = false;

}  // namespace detail

template<typename T>
concept Exponent = detail::is_exp<T>;

// DerivedDimension
template<Exponent E, Exponent... ERest>
  requires (BaseDimension<typename E::dimension> && ... && BaseDimension<typename ERest::dimension>)
struct derived_dimension_base;

template<typename T>
concept DerivedDimension = is_instantiation<downcast_base_t<T>, derived_dimension_base>;

// Dimension
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;

// UnitOf
namespace detail {

template<Dimension D>
struct dimension_unit_impl;

template<BaseDimension D>
struct dimension_unit_impl<D> {
  using type = D::base_unit;
};

template<DerivedDimension D>
struct dimension_unit_impl<D> {
  using type = D::coherent_unit;
};

} // namespace detail

template<Dimension D>
using dimension_unit = detail::dimension_unit_impl<D>::type;

template<typename U, typename D>
concept UnitOf =
  Unit<U> &&
  Dimension<D> &&
  std::same_as<typename U::reference, typename dimension_unit<D>::reference>;

// Quantity
namespace detail {

template<typename T>
inline constexpr bool is_quantity = false;

}  // namespace detail

template<typename T>
concept Quantity = detail::is_quantity<T>;

// Scalar
template<typename T>
concept Scalar = (!Quantity<T>) && std::regular<T> && std::totally_ordered<T> && detail::basic_arithmetic<T>;

}  // namespace units
