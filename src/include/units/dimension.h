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

#include <units/bits/tools.h>
#include <units/bits/type_list.h>

namespace units {

  // dim_id

  template<int UniqueValue>
  using dim_id = std::integral_constant<int, UniqueValue>;

  // dim_id_less

  template<typename D1, typename D2>
  struct dim_id_less : std::bool_constant<D1::value < D2::value> {
  };

  // exp

  template<typename BaseDimension, int Value>  // todo: to be replaced with fixed_string when supported by the compilers
  // template<fixed_string BaseDimension, int Value>
  struct exp {
    using dimension = BaseDimension;
    static constexpr int value = Value;
  };

  // is_exp
  namespace detail {
    template<typename T>
    inline constexpr bool is_exp = false;

    template<typename BaseDim, int Value>
    inline constexpr bool is_exp<exp<BaseDim, Value>> = true;
  }  // namespace detail

  template<typename T>
  concept bool Exponent = detail::is_exp<T>;

  // exp_dim_id_less

  template<Exponent E1, Exponent E2>
  struct exp_dim_id_less : dim_id_less<typename E1::dimension, typename E2::dimension> {
  };

  // exp_dim_id_less

  template<Exponent E1, Exponent E2>
  struct exp_greater_equal : std::bool_constant<(E1::value >= E2::value)> {
  };

  // exp_invert

  template<Exponent E>
  struct exp_invert;

  template<typename BaseDimension, int Value>
  struct exp_invert<exp<BaseDimension, Value>> {
    using type = exp<BaseDimension, -Value>;
  };

  template<Exponent E>
  using exp_invert_t = typename exp_invert<E>::type;

  // dimension

  template<Exponent... Es>
  struct dimension : upcast_base<dimension<Es...>> {};

  // is_dimension
  namespace detail {
    template<typename T>
    inline constexpr bool is_dimension = false;

    template<Exponent... Es>
    inline constexpr bool is_dimension<dimension<Es...>> = true;
  }  // namespace detail

  template<typename T>
  concept bool Dimension =
      std::is_empty_v<T> &&
      detail::is_dimension<upcast_from<T>>;


  // dim_invert

  template<Dimension E>
  struct dim_invert;

  template<Exponent... Es>
  struct dim_invert<dimension<Es...>> : std::type_identity<upcasting_traits_t<dimension<exp_invert_t<Es>...>>> {};

  template<Dimension D>
  using dim_invert_t = typename dim_invert<typename D::base_type>::type;


  // make_dimension

  namespace detail {

    template<Dimension D>
    struct dim_consolidate;

    template<Dimension D>
    using dim_consolidate_t = typename dim_consolidate<D>::type;

    template<>
    struct dim_consolidate<dimension<>> {
      using type = dimension<>;
    };

    template<Exponent E>
    struct dim_consolidate<dimension<E>> {
      using type = dimension<E>;
    };

    template<Exponent E1, Exponent... ERest>
    struct dim_consolidate<dimension<E1, ERest...>> {
      using rest = dim_consolidate_t<dimension<ERest...>>;
      using type =
          std::conditional_t<std::is_same_v<rest, dimension<>>, dimension<E1>, mp::type_list_push_front_t<rest, E1>>;
    };

    template<typename D, int V1, int V2, Exponent... ERest>
    struct dim_consolidate<dimension<exp<D, V1>, exp<D, V2>, ERest...>> {
      using type = std::conditional_t<V1 + V2 == 0, dim_consolidate_t<dimension<ERest...>>,
                                      dim_consolidate_t<dimension<exp<D, V1 + V2>, ERest...>>>;
    };

  }  // namespace detail

  template<Exponent... Es>
  struct make_dimension {
    using type = mp::type_list_sort_t<detail::dim_consolidate_t<mp::type_list_sort_t<dimension<Es...>, exp_dim_id_less>>, exp_greater_equal>;
  };

  template<Exponent... Es>
  using make_dimension_t = typename make_dimension<Es...>::type;

  template<Dimension D1, Dimension D2>
  struct merge_dimension {
    using type = mp::type_list_sort_t<detail::dim_consolidate_t<mp::type_list_merge_sorted_t<D1, D2, exp_dim_id_less>>, exp_greater_equal>;
  };

  template<Dimension D1, Dimension D2>
  using merge_dimension_t = typename merge_dimension<D1, D2>::type;

  // dimension_multiply

  template<Dimension D1, Dimension D2>
  struct dimension_multiply;

  template<Exponent... E1, Exponent... E2>
  struct dimension_multiply<dimension<E1...>, dimension<E2...>> : std::type_identity<upcasting_traits_t<merge_dimension_t<dimension<E1...>, dimension<E2...>>>> {};

  template<Dimension D1, Dimension D2>
  using dimension_multiply_t = typename dimension_multiply<typename D1::base_type, typename D2::base_type>::type;

  // dimension_divide

  template<Dimension D1, Dimension D2>
  struct dimension_divide;

  template<Exponent... E1, Exponent... E2>
  struct dimension_divide<dimension<E1...>, dimension<E2...>>
      : dimension_multiply<dimension<E1...>, dimension<exp_invert_t<E2>...>> {
  };

  template<Dimension D1, Dimension D2>
  using dimension_divide_t = typename dimension_divide<typename D1::base_type, typename D2::base_type>::type;

}  // namespace units
