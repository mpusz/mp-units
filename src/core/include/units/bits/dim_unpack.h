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

#include <units/bits/derived_dimension_base.h>
#include <units/bits/external/type_list.h>
#include <units/exponent.h>

namespace units::detail {

/**
 * @brief Unpacks the list of potentially derived dimensions to a list containing only base dimensions
 *
 * @tparam Es Exponents of potentially derived dimensions
 */
template<Exponent... Es>
struct dim_unpack;

template<>
struct dim_unpack<> {
  using type = exponent_list<>;
};

template<BaseDimension Dim, std::intmax_t Num, std::intmax_t Den, Exponent... ERest>
struct dim_unpack<exponent<Dim, Num, Den>, ERest...> {
  using type = type_list_push_front<typename dim_unpack<ERest...>::type, exponent<Dim, Num, Den>>;
};

template<DerivedDimension Dim, std::intmax_t Num, std::intmax_t Den, Exponent... ERest>
struct dim_unpack<exponent<Dim, Num, Den>, ERest...> {
  using type = TYPENAME dim_unpack<exponent<downcast_base_t<Dim>, Num, Den>, ERest...>::type;
};

template<Exponent... Es, std::intmax_t Num, std::intmax_t Den, Exponent... ERest>
struct dim_unpack<exponent<derived_dimension_base<Es...>, Num, Den>, ERest...> {
  using type = type_list_push_front<typename dim_unpack<ERest...>::type, exponent_multiply<Es, Num, Den>...>;
};

}  // namespace units::detail
