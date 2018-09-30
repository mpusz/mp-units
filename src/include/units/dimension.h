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

#include "bits/tools.h"
#include "bits/type_list.h"

namespace units {

  // dim_id

  template<int UniqueValue>
  using dim_id = std::integral_constant<int, UniqueValue>;

  // dim_id_less

  template<typename D1, typename D2>
      struct dim_id_less : std::bool_constant < D1::value<D2::value> {
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
    struct is_exp : std::false_type {
    };

    template<typename BaseDim, int Value>
    struct is_exp<exp<BaseDim, Value>> : std::true_type {
    };
  }  // namespace detail

  template<typename T>
  concept bool Exponent = detail::is_exp<T>::value;

  // exp_less

  template<Exponent E1, Exponent E2>
  struct exp_less : dim_id_less<typename E1::dimension, typename E2::dimension> {
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
  struct dimension;

  // is_dimension
  namespace detail {
    template<typename T>
    struct is_dimension : std::false_type {
    };

    template<Exponent... Es>
    struct is_dimension<dimension<Es...>> : std::bool_constant<(is_exp<Es>::value && ...)> {
    };
  }  // namespace detail

  template<typename T>
  concept bool Dimension = detail::is_dimension<T>::value;

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
    using type = detail::dim_consolidate_t<mp::type_list_sort_t<dimension<Es...>, exp_less>>;
  };

  template<Exponent... Es>
  using make_dimension_t = typename make_dimension<Es...>::type;

  // dimension_multiply

  template<Dimension D1, Dimension D2>
  struct dimension_multiply;

  template<Exponent... E1, Exponent... E2>
  struct dimension_multiply<dimension<E1...>, dimension<E2...>> {
    using type = make_dimension_t<E1..., E2...>;
  };

  template<Dimension D1, Dimension D2>
  using dimension_multiply_t = typename dimension_multiply<D1, D2>::type;

  // dimension_divide

  template<Dimension D1, Dimension D2>
  struct dimension_divide;

  template<Exponent... E1, Exponent... E2>
  struct dimension_divide<dimension<E1...>, dimension<E2...>>
      : dimension_multiply<dimension<E1...>, dimension<exp_invert_t<E2>...>> {
  };

  template<Dimension D1, Dimension D2>
  using dimension_divide_t = typename dimension_divide<D1, D2>::type;

}  // namespace units
