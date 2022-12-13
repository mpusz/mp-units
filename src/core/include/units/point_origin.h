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
#include <units/bits/equivalent.h>


namespace units {

template<Dimension D>
struct point_origin {
  using dimension = D;
};

/**
 * @brief A statically unspecified quantity point origin
 *
 * An origin, unspecified in the type system, from which an absolute quantity is measured from.
 * The type system considers all unspecified_origin<D> with equivalent dimensions to refer to the same abstract origin.
 * (that is, the types `unspecified_origin<D>` and `unspecified_origin<D2>` are equivalent exactly iff `D` is equivalent
 * to `D2`.
 *
 * @tparam D a dimension of the quantity point (can be either a BaseDimension or a DerivedDimension)
 */
template<Dimension D>
struct unspecified_origin : point_origin<D> {
  template<Dimension D2>
    requires equivalent<D, D2>
  using rebind = unspecified_origin<D2>;
};


namespace detail {

template<Unit U>
struct customary_origin_spec_for_unit {};

}  // namespace detail

template<Unit U>
  requires requires { typename detail::customary_origin_spec_for_unit<U>::type; }
struct customary_origin_for_unit : point_origin<typename detail::customary_origin_spec_for_unit<U>::type::dimension> {
  // equivalence of instantiations of customary_unit_origin is determined by equality of canonical_point_origin
  using canonical_point_origin = typename detail::customary_origin_spec_for_unit<U>::type;
  using reference_point_origin = typename canonical_point_origin::reference_point_origin;
  static constexpr auto offset_to_reference = canonical_point_origin::offset_to_reference;
};

namespace detail {

template<Dimension D, UnitOf<D> U>
struct default_point_origin_for_dim_and_unit {
  using type = unspecified_origin<D>;
};

template<Dimension D, UnitOf<D> U>
  requires requires { typename detail::customary_origin_spec_for_unit<U>::type; }
struct default_point_origin_for_dim_and_unit<D, U> {
  using type = customary_origin_for_unit<U>;
};

template<Dimension D, UnitOf<D> U>
using default_point_origin_for_dim_and_unit_t = typename default_point_origin_for_dim_and_unit<D, U>::type;

}  // namespace detail


}  // namespace units
