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

#include <units/isq/si/cgs/length.h>
#include <units/isq/si/cgs/speed.h> // IWYU pragma: keep
#include <units/isq/si/fps/length.h>
#include <units/isq/si/fps/speed.h> // IWYU pragma: keep
#include <units/isq/si/length.h> // IWYU pragma: keep
#include <units/isq/si/prefixes.h>
#include <units/isq/si/speed.h>
#include <units/quantity_point_kind.h>
#include <units/chrono.h>
#include <chrono>
#include <complex>
#include <mutex>
#include <optional>
#include <ratio>
#include <string>
#include <utility>

namespace {

using namespace units;
using namespace units::isq;

// Prefix family

static_assert(PrefixFamily<si::prefix>);
static_assert(!PrefixFamily<si::kilo>);

// Prefix

static_assert(Prefix<si::kilo>);
static_assert(!Prefix<si::prefix>);
static_assert(!Prefix<std::kilo>);

// UnitRatio

static_assert(UnitRatio<ratio(1000)>);
static_assert(!UnitRatio<ratio(0)>);
// static_assert(UnitRatio<ratio(1000, 0)>);  // static_assert in ratio
static_assert(UnitRatio<ratio(-1000, -1)>);
static_assert(!UnitRatio<ratio(-1000, 1)>);
static_assert(!UnitRatio<ratio(1, -1000)>);

// BaseDimension

static_assert(BaseDimension<si::dim_length>);
static_assert(!BaseDimension<si::dim_speed>);
static_assert(!BaseDimension<int>);

// DerivedDimension

static_assert(DerivedDimension<si::dim_speed>);
static_assert(!DerivedDimension<si::dim_length>);
static_assert(!DerivedDimension<int>);

// Dimension

static_assert(Dimension<si::dim_length>);
static_assert(Dimension<si::dim_speed>);
static_assert(!Dimension<si::metre>);
static_assert(!Dimension<int>);
static_assert(!Dimension<std::chrono::seconds>);

// Unit

static_assert(Unit<si::metre>);
static_assert(Unit<si::kilometre>);
static_assert(Unit<si::fps::mile>);
static_assert(Unit<si::metre_per_second>);
static_assert(!Unit<si::dim_length>);
static_assert(!Unit<int>);
static_assert(!Unit<std::chrono::seconds>);

// UnitOf

static_assert(UnitOf<si::metre, si::dim_length>);
static_assert(UnitOf<si::kilometre, si::dim_length>);
static_assert(UnitOf<si::fps::mile, si::dim_length>);
static_assert(!UnitOf<si::second, si::dim_length>);

// Representation

static_assert(Representation<int>);
static_assert(Representation<std::complex<double>>);
static_assert(!Representation<si::length<si::metre>>);
static_assert(!Representation<std::optional<si::length<si::metre>>>);
static_assert(!Representation<std::mutex>);
static_assert(!Representation<std::string>);

// Quantity

static_assert(Quantity<si::length<si::metre>>);
static_assert(!Quantity<std::chrono::seconds>);
static_assert(!Quantity<quantity_point<unspecified_origin<si::dim_length>, si::metre>>);

// QuantityPoint

static_assert(QuantityPoint<quantity_point<unspecified_origin<si::dim_length>, si::metre>>);
static_assert(!QuantityPoint<si::length<si::metre>>);
static_assert(!QuantityPoint<std::chrono::seconds>);

// QuantityLike

static_assert(QuantityLike<std::chrono::seconds>);
static_assert(QuantityLike<std::chrono::hours>);
static_assert(!QuantityLike<si::time<si::second>>);
static_assert(!QuantityLike<int>);

// WrappedQuantity

static_assert(wrapped_quantity_<std::optional<si::length<si::metre>>>);
static_assert(!wrapped_quantity_<std::pair<si::length<si::metre>, si::length<si::metre>>>);

// QuantityOf

static_assert(QuantityOf<si::length<si::metre>, si::dim_length>);
// TODO it seems `QuantityOf` is a bad name if `si::cgs::length<si::cgs::centimetre>` matches `si::fps::dim_length`
static_assert(QuantityOf<si::cgs::length<si::cgs::centimetre>, si::dim_length>);
static_assert(QuantityOf<si::cgs::length<si::metre>, si::dim_length>);
static_assert(QuantityOf<si::cgs::length<si::cgs::centimetre>, si::fps::dim_length>);
static_assert(!QuantityOf<si::cgs::length<si::cgs::centimetre>, si::dim_time>);

static_assert(QuantityPointOf<quantity_point<unspecified_origin<si::dim_time>, si::second, int>, unspecified_origin<si::dim_time>>);
static_assert(QuantityPointOf<quantity_point<clock_origin<std::chrono::system_clock>, si::second, int>, clock_origin<std::chrono::system_clock>>);
static_assert(!QuantityPointOf<quantity_point<unspecified_origin<si::dim_time>, si::second, int>, clock_origin<std::chrono::system_clock>>);
static_assert(!QuantityPointOf<quantity_point<clock_origin<std::chrono::system_clock>, si::second, int>, unspecified_origin<si::dim_time>>);

}  // namespace
