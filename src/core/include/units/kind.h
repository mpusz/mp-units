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
#include <units/bits/external/downcasting.h>

namespace units {

template<Dimension D>
struct dynamic_origin;

namespace detail {

template<typename K, Dimension D>
struct _kind_base : downcast_base<_kind_base<K, D>> {
  using base_kind = K;
  using dimension = D;
};

template<Kind K, PointOrigin O>
struct _point_kind_base : downcast_base<_point_kind_base<K, O>> {
  using base_kind = K;
  using dimension = typename K::dimension;
  using origin = O;
};

}  // namespace detail

template<Kind K, Dimension D>
  requires Kind<downcast<detail::_kind_base<typename K::base_kind, D>>>
using downcast_kind = downcast<detail::_kind_base<typename K::base_kind, D>>;

template<Kind K, PointOrigin O = dynamic_origin<typename K::dimension>>
  requires PointKind<downcast<detail::_point_kind_base<K, O>>>
using downcast_point_kind = downcast<detail::_point_kind_base<K, O>>;

template<typename K, Dimension D>
struct kind : downcast_dispatch<K, detail::_kind_base<K, D>> {};

template<typename DK, Dimension D, Kind BK>
  requires std::same_as<BK, typename BK::base_kind>
struct derived_kind : downcast_dispatch<DK, detail::_kind_base<BK, D>> {};

template<typename DPK, Kind BK, PointOrigin O = dynamic_origin<typename BK::dimension>>
struct point_kind : downcast_dispatch<DPK, detail::_point_kind_base<BK, O>> {};

}  // namespace units
