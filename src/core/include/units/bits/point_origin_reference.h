// The MIT License (MIT)
//
// Copyright (c) 2020 Yves Delley
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
#include <units/bits/equivalent.h>

namespace units {

namespace detail {

template<PointOrigin Orig>
struct reference_point_origin {
  using type = typename Orig::reference_point_origin;
};

template<PointOrigin Orig>
using reference_point_origin_t =
  typename reference_point_origin<Orig>::type;

template<PointOrigin Orig>
struct ultimate_reference_origin {
  using type =
    typename std::conditional_t<
      std::is_same_v<reference_point_origin_t<Orig>,Orig>,
      std::type_identity<Orig>,
      ultimate_reference_origin<reference_point_origin_t<Orig>>
    >::type;
};

template<PointOrigin Orig>
using ultimate_reference_origin_t = typename ultimate_reference_origin<Orig>::type;

template <Quantity Q, PointOrigin Orig>
struct offset_to_ultimate_reference_origin {
  static constexpr auto value = [](){
    using RefOrig = reference_point_origin_t<Orig>;
    if constexpr(std::is_same_v<RefOrig,Orig>) {
      return Q::zero();
    } else {
      return offset_to_ultimate_reference_origin<Q,RefOrig>::value + Orig::offset_to_reference;
    }
  }();
};

template <Quantity Q, PointOrigin Orig>
inline constexpr auto offset_to_ultimate_reference_origin_v = offset_to_ultimate_reference_origin<Q, Orig>::value;

}  // namespace detail

template <PointOrigin Orig1, PointOrigin Orig2>
inline constexpr bool fixed_known_offset = equivalent<detail::ultimate_reference_origin_t<Orig1>, detail::ultimate_reference_origin_t<Orig2>>;

template <Quantity Q, PointOrigin To, PointOrigin From> requires fixed_known_offset<To, From>
inline constexpr auto offset_between_origins = detail::offset_to_ultimate_reference_origin_v<Q, To> - detail::offset_to_ultimate_reference_origin_v<Q, From>;

} // namespace units
