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

#include <units/bits/expression_template.h>
#include <units/bits/external/fixed_string.h>
#include <units/bits/external/type_name.h>
#include <units/bits/external/type_traits.h>

namespace units {

/**
 * @brief A dimension of a base quantity
 *
 * Base quantity is a quantity in a conventionally chosen subset of a given system of quantities, where no quantity
 * in the subset can be expressed in terms of the other quantities within that subset. They are referred to as
 * being mutually independent since a base quantity cannot be expressed as a product of powers of the other base
 * quantities.
 *
 * Symbol template parameters is an unique identifier of the base dimension. The same identifiers can be multiplied
 * and divided which will result with an adjustment of its factor in an Exponent of a DerivedDimension
 * (in case of zero the dimension will be simplified and removed from further analysis of current expresion).
 *
 * @tparam Symbol an unique identifier of the base dimension used to provide dimensional analysis support
 */
template<basic_fixed_string Symbol>
struct base_dimension {
  static constexpr auto symbol = Symbol;  ///< Unique base dimension identifier
};

namespace detail {

template<basic_fixed_string Symbol>
void to_base_base_dimension(const volatile base_dimension<Symbol>*);

}  // namespace detail

/**
 * @brief A concept matching all base dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `base_dimension`.
 */
template<typename T>
concept BaseDimension = requires(T* t) { detail::to_base_base_dimension(t); };

template<BaseDimension D1, BaseDimension D2>
struct base_dimension_less : std::bool_constant<(D1::symbol < D2::symbol)> {};

// TODO Can we provide a smarter implementation?
std::false_type is_derived_dimension(...);

/**
 * @brief A concept matching all derived dimensions in the library.
 *
 * Satisfied by all dimension types derived from an specialization of `derived_dimension`.
 */
template<typename T>
concept DerivedDimension = decltype(is_derived_dimension(std::declval<T*>()))::value;

/**
 * @brief A concept matching all dimensions in the library.
 *
 * Satisfied by all dimension types for which either `BaseDimension<T>` or `DerivedDimension<T>` is `true`.
 */
template<typename T>
concept Dimension = BaseDimension<T> || DerivedDimension<T>;

namespace detail {

/**
 * @brief Unpacks the list of potentially derived dimensions to a list containing only base dimensions
 *
 * @tparam Es Exponents of potentially derived dimensions
 */
template<typename NumList, typename DenList>
struct dim_extract;

template<>
struct dim_extract<type_list<>, type_list<>> {
  using num = type_list<>;
  using den = type_list<>;
};

template<typename T, typename... NRest, typename... Dens>
  requires BaseDimension<T> || BaseDimension<typename T::factor>
struct dim_extract<type_list<T, NRest...>, type_list<Dens...>> {
  using impl = dim_extract<type_list<NRest...>, type_list<Dens...>>;
  using num = type_list_push_front<typename impl::num, T>;
  using den = TYPENAME impl::den;
};

template<typename T, typename... DRest>
  requires BaseDimension<T> || BaseDimension<typename T::factor>
struct dim_extract<type_list<>, type_list<T, DRest...>> {
  using impl = dim_extract<type_list<>, type_list<DRest...>>;
  using num = TYPENAME impl::num;
  using den = type_list_push_front<typename impl::den, T>;
};

template<DerivedDimension T, typename... NRest, typename... Dens>
struct dim_extract<type_list<T, NRest...>, type_list<Dens...>> :
    dim_extract<type_list_push_back<typename T::normalized_num, NRest...>,
                type_list_push_back<typename T::normalized_den, Dens...>> {};

template<DerivedDimension T, int... Ints, typename... NRest, typename... Dens>
struct dim_extract<type_list<power<T, Ints...>, NRest...>, type_list<Dens...>> :
    dim_extract<type_list_push_back<
                  typename expr_power<typename T::normalized_num, power<T, Ints...>::num, power<T, Ints...>::den>::type,
                  NRest...>,
                type_list_push_back<
                  typename expr_power<typename T::normalized_den, power<T, Ints...>::num, power<T, Ints...>::den>::type,
                  Dens...>> {};


template<DerivedDimension T, typename... DRest>
struct dim_extract<type_list<>, type_list<T, DRest...>> :
    dim_extract<typename T::normalized_den, type_list_push_back<typename T::normalized_num, DRest...>> {};

template<DerivedDimension T, int... Ints, typename... DRest>
struct dim_extract<type_list<>, type_list<power<T, Ints...>, DRest...>> :
    dim_extract<typename expr_power<typename T::normalized_den, power<T, Ints...>::num, power<T, Ints...>::den>::type,
                type_list_push_back<
                  typename expr_power<typename T::normalized_num, power<T, Ints...>::num, power<T, Ints...>::den>::type,
                  DRest...>> {};

template<typename T1, typename T2>
using type_list_of_base_dimension_less = expr_less<T1, T2, base_dimension_less>;

/**
 * @brief Converts user provided derived dimension specification into a valid units::normalized_dimension definition
 *
 * User provided definition of a derived dimension may contain the same base dimension repeated more than once on the
 * list possibly hidden in other derived units provided by the user. The process here should:
 * 1. Extract derived dimensions into exponents of base dimensions.
 * 2. Sort the exponents so the same dimensions are placed next to each other.
 * 3. Consolidate contiguous range of exponents of the same base dimensions to a one (or possibly zero) exponent for
 *    this base dimension.
 */
template<typename OneTypeBase, typename... Ds>
struct normalized_dimension : detail::expr_fractions<OneTypeBase, Ds...> {
private:
  using base = detail::expr_fractions<OneTypeBase, Ds...>;
  using extracted = dim_extract<typename base::num, typename base::den>;
  using num_list = expr_consolidate<type_list_sort<typename extracted::num, type_list_of_base_dimension_less>>;
  using den_list = expr_consolidate<type_list_sort<typename extracted::den, type_list_of_base_dimension_less>>;
  using simple = expr_simplify<num_list, den_list, type_list_of_base_dimension_less>;
public:
  using normalized_num = TYPENAME simple::num;
  using normalized_den = TYPENAME simple::den;
};

}  // namespace detail

// TODO add checking for `per` and power elements as well
template<typename T>
concept DimensionSpec = Dimension<T> || is_specialization_of<T, per> || detail::is_specialization_of_power<T>;

// User should not instantiate this type!!!
template<DimensionSpec... Ds>
struct derived_dimension : detail::normalized_dimension<derived_dimension<>, Ds...> {};

// TODO move type_list_fractions out of

template<typename... Args>
std::true_type is_derived_dimension(const volatile derived_dimension<Args...>*);

/**
 * @brief Dimension one
 *
 * Dimension for which all the exponents of the factors corresponding to the base
 * dimensions are zero. Also commonly named as "dimensionless".
 */
inline constexpr struct dim_one : derived_dimension<> {
} dim_one;

namespace detail {

template<Dimension D1, Dimension D2>
struct dimension_less : std::bool_constant<type_name<D1>() < type_name<D2>()> {};

template<typename T1, typename T2>
using type_list_of_dimension_less = expr_less<T1, T2, dimension_less>;

}  // namespace detail

template<Dimension D1, Dimension D2>
constexpr Dimension auto operator*(D1, D2)
{
  return detail::expr_multiply<D1, D2, struct dim_one, detail::type_list_of_dimension_less, derived_dimension>();
}

template<Dimension D1, Dimension D2>
constexpr Dimension auto operator/(D1, D2)
{
  return detail::expr_divide<D1, D2, struct dim_one, detail::type_list_of_dimension_less, derived_dimension>();
}

template<Dimension D>
constexpr Dimension auto operator/(int value, D)
{
  gsl_Assert(value == 1);
  return detail::expr_invert<D, struct dim_one, derived_dimension>();
}

template<Dimension D1, Dimension D2>
constexpr bool operator==(D1, D2)
{
  return false;
}

template<BaseDimension D1, BaseDimension D2>
constexpr bool operator==(D1, D2)
{
  return D1::symbol == D2::symbol;
}

template<BaseDimension D1, Dimension D2>
  requires(type_list_size<typename D2::normalized_den> == 0) && (type_list_size<typename D2::normalized_num> == 1) &&
          BaseDimension<type_list_front<typename D2::normalized_num>>
constexpr bool operator==(D1, D2)
{
  return D1::symbol == type_list_front<typename D2::normalized_num>::symbol;
}

template<Dimension D1, BaseDimension D2>
  requires(type_list_size<typename D1::normalized_den> == 0) && (type_list_size<typename D1::normalized_num> == 1) &&
          BaseDimension<type_list_front<typename D1::normalized_num>>
constexpr bool operator==(D1, D2)
{
  return type_list_front<typename D1::normalized_num>::symbol == D2::symbol;
}

template<DerivedDimension D1, DerivedDimension D2>
constexpr bool operator==(D1, D2)
{
  return is_same_v<typename D1::normalized_num, typename D2::normalized_num> &&
         is_same_v<typename D1::normalized_den, typename D2::normalized_den>;
}

}  // namespace units
