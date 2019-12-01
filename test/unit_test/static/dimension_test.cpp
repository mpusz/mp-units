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

#include "units/derived_dimension.h"
#include "units/unit.h"
#include <utility>

using namespace units;

namespace {

struct u0 : unit<u0> {};
struct d0 : base_dimension<"d0", u0> {};
struct u1 : unit<u1> {};
struct d1 : base_dimension<"d1", u1> {};
struct u2 : unit<u2> {};
struct d2 : base_dimension<"d2", u2> {};
struct u3 : unit<u3> {};
struct d3 : base_dimension<"d3", u3> {};

// exp_invert

static_assert(std::is_same_v<exp_invert<exp<d0, 2>>, exp<d0, -2>>);
static_assert(std::is_same_v<exp_invert<exp<d1, -2>>, exp<d1, 2>>);

// extract

template<typename T>
struct typeinfo;

template<typename... Ts>
using extract = detail::extract<Ts...>::type;

static_assert(std::is_same_v<extract<>, derived_dimension<>>);
static_assert(std::is_same_v<extract<exp<d0, 1>>, derived_dimension<exp<d0, 1>>>);
static_assert(std::is_same_v<extract<exp<d0, 1>, exp<d1, 2>>, derived_dimension<exp<d0, 1>, exp<d1, 2>>>);
using dim0 = derived_dimension<>;
using dim1 = derived_dimension<exp<d0, 1>>;
using dim2 = derived_dimension<exp<d0, 1>, exp<d1, 2>>;
static_assert(std::is_same_v<extract<exp<dim0, 2>, exp<d0, 1>>, derived_dimension<exp<d0, 1>>>);
static_assert(std::is_same_v<extract<exp<dim1, 2>, exp<d0, 1>>, derived_dimension<exp<d0, 2>, exp<d0, 1>>>);
static_assert(std::is_same_v<extract<exp<dim2, -2>, exp<d0, 1>, exp<d1, 2>>,
                             derived_dimension<exp<d0, -2>, exp<d1, -4>, exp<d0, 1>, exp<d1, 2>>>);

// dim_invert
static_assert(std::is_same_v<dim_invert<derived_dimension<exp<d0, -1>>>, d0>);
static_assert(std::is_same_v<dim_invert<derived_dimension<exp<d0, -2>>>, derived_dimension<exp<d0, 2>>>);
static_assert(
    std::is_same_v<dim_invert<derived_dimension<exp<d0, 2>, exp<d1, -1>>>, derived_dimension<exp<d0, -2>, exp<d1, 1>>>);

// make_dimension

template<typename... Ts>
using make_dimension = detail::make_dimension<Ts...>;

static_assert(std::is_same_v<make_dimension<exp<d0, 1>>, derived_dimension<exp<d0, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d1, 1>>, derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 1>, exp<d0, 1>>, derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 1>, exp<d1, 1>>, derived_dimension<exp<d1, 2>>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 1>, exp<d1, -1>>, derived_dimension<>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 1>, exp<d1, 1, 2>>, derived_dimension<exp<d1, 3, 2>>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 1, 2>, exp<d1, 1, 2>>, derived_dimension<exp<d1, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d1, 2>, exp<d1, 1, 2>>, derived_dimension<exp<d1, 5, 2>>>);

static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d1, 1>, exp<d0, 1>, exp<d1, 1>>,
                             derived_dimension<exp<d0, 2>, exp<d1, 2>>>);
static_assert(std::is_same_v<make_dimension<exp<d0, -1>, exp<d1, -1>, exp<d0, -1>, exp<d1, -1>>,
                             derived_dimension<exp<d0, -2>, exp<d1, -2>>>);

static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d1, 1>, exp<d1, -1>>, derived_dimension<exp<d0, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d0, -1>, exp<d1, 1>>, derived_dimension<exp<d1, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d1, 1>, exp<d0, -1>>, derived_dimension<exp<d1, 1>>>);
static_assert(std::is_same_v<make_dimension<exp<d0, 1>, exp<d1, 1>, exp<d0, -1>, exp<d1, -1>>, derived_dimension<>>);

// dimension_multiply

static_assert(std::is_same_v<dimension_multiply<derived_dimension<exp<d0, 1>>, derived_dimension<exp<d1, 1>>>,
                             derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(
    std::is_same_v<dimension_multiply<derived_dimension<exp<d0, 1>>, d1>, derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(
    std::is_same_v<dimension_multiply<d0, derived_dimension<exp<d1, 1>>>, derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(std::is_same_v<dimension_multiply<d0, d1>, derived_dimension<exp<d0, 1>, exp<d1, 1>>>);
static_assert(std::is_same_v<
              dimension_multiply<derived_dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, derived_dimension<exp<d3, 1>>>,
              derived_dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>, exp<d3, 1>>>);
static_assert(std::is_same_v<
              dimension_multiply<derived_dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, derived_dimension<exp<d1, 1>>>,
              derived_dimension<exp<d0, 1>, exp<d1, 2>, exp<d2, 1>>>);
static_assert(std::is_same_v<
              dimension_multiply<derived_dimension<exp<d0, 1>, exp<d1, 1>, exp<d2, 1>>, derived_dimension<exp<d1, -1>>>,
              derived_dimension<exp<d0, 1>, exp<d2, 1>>>);
static_assert(std::is_same_v<dimension_multiply<derived_dimension<exp<d0, 1>>, derived_dimension<exp<d0, -1>>>,
                             derived_dimension<>>);
static_assert(std::is_same_v<dimension_multiply<derived_dimension<exp<d0, 2>>, derived_dimension<exp<d0, -1>>>, d0>);

// dimension_divide

static_assert(std::is_same_v<dimension_divide<derived_dimension<exp<d0, 1>>, derived_dimension<exp<d1, 1>>>,
                             derived_dimension<exp<d0, 1>, exp<d1, -1>>>);
static_assert(std::is_same_v<dimension_divide<derived_dimension<exp<d0, 1>>, derived_dimension<exp<d0, 1>>>,
                             derived_dimension<>>);
static_assert(std::is_same_v<dimension_divide<derived_dimension<exp<d0, 1>>, derived_dimension<exp<d0, 1>>>,
                             derived_dimension<>>);
static_assert(std::is_same_v<dimension_divide<derived_dimension<exp<d0, 2>>, derived_dimension<exp<d0, 1>>>, d0>);

}  // namespace
