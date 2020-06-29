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

#include "units/bits/dimension_op.h"
#include "units/unit.h"
#include <utility>

using namespace units;

namespace {

struct u0 : named_unit<u0, "u0", no_prefix> {};
struct d0 : base_dimension<"d0", u0> {};
struct u1 : named_unit<u1, "u1", no_prefix> {};
struct d1 : base_dimension<"d1", u1> {};
struct u2 : named_unit<u2, "u2", no_prefix> {};
struct d2 : base_dimension<"d2", u2> {};
struct u3 : named_unit<u3, "u3", no_prefix> {};
struct d3 : base_dimension<"d3", u3> {};

// exp_invert

static_assert(is_same_v<exp_invert<units::exp<d0, 2>>, units::exp<d0, -2>>);
static_assert(is_same_v<exp_invert<units::exp<d1, -2>>, units::exp<d1, 2>>);

// dim_unpack

template<typename T>
struct typeinfo;

template<typename... Ts>
using dim_unpack = detail::dim_unpack<Ts...>::type;

template<Exponent... Es>
using derived_dim = detail::derived_dimension_base<Es...>;

static_assert(is_same_v<dim_unpack<>, exp_list<>>);
static_assert(is_same_v<dim_unpack<units::exp<d0, 1>>, exp_list<units::exp<d0, 1>>>);
static_assert(is_same_v<dim_unpack<units::exp<d0, 1>, units::exp<d1, 2>>, exp_list<units::exp<d0, 1>, units::exp<d1, 2>>>);
using dim1 = derived_dim<units::exp<d0, 1>>;
using dim2 = derived_dim<units::exp<d0, 1>, units::exp<d1, 2>>;
static_assert(is_same_v<dim_unpack<units::exp<dim1, 2>, units::exp<d0, 1>>, exp_list<units::exp<d0, 2>, units::exp<d0, 1>>>);
static_assert(is_same_v<dim_unpack<units::exp<dim2, -2>, units::exp<d0, 1>, units::exp<d1, 2>>,
                             exp_list<units::exp<d0, -2>, units::exp<d1, -4>, units::exp<d0, 1>, units::exp<d1, 2>>>);

// dim_invert
static_assert(is_same_v<dim_invert<derived_dim<units::exp<d0, -1>>>, d0>);
static_assert(is_same_v<dim_invert<derived_dim<units::exp<d0, -2>>>, unknown_dimension<units::exp<d0, 2>>>);
static_assert(
    is_same_v<dim_invert<derived_dim<units::exp<d0, 2>, units::exp<d1, -1>>>, unknown_dimension<units::exp<d0, -2>, units::exp<d1, 1>>>);

// make_dimension

template<typename... Ts>
using make_dimension = detail::make_dimension<Ts...>;

static_assert(is_same_v<make_dimension<units::exp<d0, 1>>, derived_dim<units::exp<d0, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d0, 1>, units::exp<d1, 1>>, derived_dim<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d1, 1>, units::exp<d0, 1>>, derived_dim<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d1, 1>, units::exp<d1, 1>>, derived_dim<units::exp<d1, 2>>>);
static_assert(is_same_v<make_dimension<units::exp<d1, 1>, units::exp<d1, 1, 2>>, derived_dim<units::exp<d1, 3, 2>>>);
static_assert(is_same_v<make_dimension<units::exp<d1, 1, 2>, units::exp<d1, 1, 2>>, derived_dim<units::exp<d1, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d1, 2>, units::exp<d1, 1, 2>>, derived_dim<units::exp<d1, 5, 2>>>);

static_assert(is_same_v<make_dimension<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d0, 1>, units::exp<d1, 1>>,
                             derived_dim<units::exp<d0, 2>, units::exp<d1, 2>>>);
static_assert(is_same_v<make_dimension<units::exp<d0, -1>, units::exp<d1, -1>, units::exp<d0, -1>, units::exp<d1, -1>>,
                             derived_dim<units::exp<d0, -2>, units::exp<d1, -2>>>);

static_assert(is_same_v<make_dimension<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d1, -1>>, derived_dim<units::exp<d0, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d0, 1>, units::exp<d0, -1>, units::exp<d1, 1>>, derived_dim<units::exp<d1, 1>>>);
static_assert(is_same_v<make_dimension<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d0, -1>>, derived_dim<units::exp<d1, 1>>>);

// dimension_multiply

static_assert(is_same_v<dimension_multiply<derived_dim<units::exp<d0, 1>>, derived_dim<units::exp<d1, 1>>>,
                             unknown_dimension<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(
    is_same_v<dimension_multiply<derived_dim<units::exp<d0, 1>>, d1>, unknown_dimension<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(
    is_same_v<dimension_multiply<d0, derived_dim<units::exp<d1, 1>>>, unknown_dimension<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(is_same_v<dimension_multiply<d0, d1>, unknown_dimension<units::exp<d0, 1>, units::exp<d1, 1>>>);
static_assert(is_same_v<
              dimension_multiply<derived_dim<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d2, 1>>, derived_dim<units::exp<d3, 1>>>,
              unknown_dimension<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d2, 1>, units::exp<d3, 1>>>);
static_assert(is_same_v<
              dimension_multiply<derived_dim<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d2, 1>>, derived_dim<units::exp<d1, 1>>>,
              unknown_dimension<units::exp<d0, 1>, units::exp<d1, 2>, units::exp<d2, 1>>>);
static_assert(is_same_v<
              dimension_multiply<derived_dim<units::exp<d0, 1>, units::exp<d1, 1>, units::exp<d2, 1>>, derived_dim<units::exp<d1, -1>>>,
              unknown_dimension<units::exp<d0, 1>, units::exp<d2, 1>>>);
static_assert(is_same_v<dimension_multiply<derived_dim<units::exp<d0, 2>>, derived_dim<units::exp<d0, -1>>>, d0>);

// dimension_divide

static_assert(is_same_v<dimension_divide<derived_dim<units::exp<d0, 1>>, derived_dim<units::exp<d1, 1>>>,
                             unknown_dimension<units::exp<d0, 1>, units::exp<d1, -1>>>);
static_assert(is_same_v<dimension_divide<derived_dim<units::exp<d0, 2>>, unknown_dimension<units::exp<d0, 1>>>, d0>);

}  // namespace
