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

#include <units/derived_dimension.h>
#include <units/isq/iec80000/storage_capacity.h>
#include <units/isq/si/time.h>
#include <units/unit.h>

// IWYU pragma: begin_exports
#include <units/quantity.h>
#include <units/symbol_text.h>
// IWYU pragma: end_exports

namespace units::isq::iec80000 {

struct byte_per_second : derived_unit<byte_per_second> {};
struct dim_transfer_rate :
    derived_dimension<dim_transfer_rate, byte_per_second, exponent<dim_storage_capacity, 1>,
                      exponent<si::dim_time, -1>> {};

struct kilobyte_per_second : derived_scaled_unit<kilobyte_per_second, dim_transfer_rate, kilobyte, si::second> {};
struct megabyte_per_second : derived_scaled_unit<megabyte_per_second, dim_transfer_rate, megabyte, si::second> {};
struct gigabyte_per_second : derived_scaled_unit<gigabyte_per_second, dim_transfer_rate, gigabyte, si::second> {};
struct terabyte_per_second : derived_scaled_unit<terabyte_per_second, dim_transfer_rate, terabyte, si::second> {};
struct petabyte_per_second : derived_scaled_unit<petabyte_per_second, dim_transfer_rate, petabyte, si::second> {};
struct exabyte_per_second : derived_scaled_unit<exabyte_per_second, dim_transfer_rate, exabyte, si::second> {};
struct zettabyte_per_second : derived_scaled_unit<zettabyte_per_second, dim_transfer_rate, zettabyte, si::second> {};
struct yottabyte_per_second : derived_scaled_unit<yottabyte_per_second, dim_transfer_rate, yottabyte, si::second> {};

template<typename T>
concept TransferRate = QuantityOf<T, dim_transfer_rate>;

template<UnitOf<dim_transfer_rate> U, Representation Rep = double>
using transfer_rate = quantity<dim_transfer_rate, U, Rep>;

#ifndef UNITS_NO_LITERALS

inline namespace literals {

constexpr auto operator"" _q_B_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<byte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_kB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<kilobyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_MB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<megabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_GB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<gigabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_TB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<terabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_PB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<petabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_EB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<exabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_ZB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<zettabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}
constexpr auto operator"" _q_YB_per_s(unsigned long long l)
{
  gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
  return transfer_rate<yottabyte_per_second, std::int64_t>(static_cast<std::int64_t>(l));
}

}  // namespace literals

#endif  // UNITS_NO_LITERALS

}  // namespace units::isq::iec80000

#ifndef UNITS_NO_ALIASES

namespace units::aliases::isq::iec80000::inline transfer_rate {

template<Representation Rep = double>
using B_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::byte_per_second, Rep>;
template<Representation Rep = double>
using kB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::kilobyte_per_second, Rep>;
template<Representation Rep = double>
using MB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::megabyte_per_second, Rep>;
template<Representation Rep = double>
using GB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::gigabyte_per_second, Rep>;
template<Representation Rep = double>
using TB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::terabyte_per_second, Rep>;
template<Representation Rep = double>
using PB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::petabyte_per_second, Rep>;
template<Representation Rep = double>
using EB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::exabyte_per_second, Rep>;
template<Representation Rep = double>
using ZB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::zettabyte_per_second, Rep>;
template<Representation Rep = double>
using YB_per_s = units::isq::iec80000::transfer_rate<units::isq::iec80000::yottabyte_per_second, Rep>;

}  // namespace units::aliases::isq::iec80000::inline transfer_rate

#endif  // UNITS_NO_ALIASES
