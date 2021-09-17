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
#include <units/bits/external/downcasting.h>

namespace units {

namespace detail {

template<typename Orig, Unit U>
struct _origin_base : downcast_base<_origin_base<Orig, U>> {
  using base_origin = Orig;
  using reference_unit = U;
};

}  // namespace detail

template<PointOrigin Orig, Unit U>
  requires PointOrigin<downcast<detail::_origin_base<typename Orig::base_origin, U>>>
using downcast_origin = downcast<detail::_origin_base<typename Orig::base_origin, U>>;

template<typename Orig, Unit U>
struct point_origin : downcast_dispatch<Orig, detail::_origin_base<Orig, U>> {};

/**
 * @brief A "default" point origin for scales associated with the base unit of a dimension
 *
 * This is mostly used as a fall-back so code that does not explicitly specify an origin still works.
 *
 * @tparam U the base unit for measurements with respect to this origin.
 */
template <Unit U>
struct default_point_origin : point_origin<default_point_origin<U>, U> {};

}  // namespace units
