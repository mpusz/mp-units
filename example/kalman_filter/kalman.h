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

#include <units/bits/fmt_hacks.h>
#include <units/format.h>
#include <units/generic/dimensionless.h>
#include <units/isq/dimensions/time.h>
#include <units/math.h>
#include <units/quantity.h>
#include <units/quantity_point.h>
#include <tuple>

namespace kalman {

template<typename T>
concept QuantityOrQuantityPoint =
  units::Quantity<T> || units::QuantityPoint<T>;  // TODO Should it also account for `kinds`?

template<typename... Qs>
inline constexpr bool are_derivatives = false;

template<typename Q>
inline constexpr bool are_derivatives<Q> = true;

template<typename Q1, typename Q2, typename... Qs>
inline constexpr bool are_derivatives<Q1, Q2, Qs...> =
  units::DimensionOfT<typename decltype(Q1::reference / Q2::reference)::dimension,
                      units::isq::dim_time> &&  // TODO Think on how to simplify this
  are_derivatives<Q2, Qs...>;

// state
template<QuantityOrQuantityPoint... QQPs>
  requires(sizeof...(QQPs) > 0) && (sizeof...(QQPs) <= 3) && are_derivatives<QQPs...>
struct state {
  std::tuple<QQPs...> variables_;
  constexpr state(QQPs... qqps) : variables_(std::move(qqps)...) {}
};

template<typename T>
concept State = units::is_specialization_of<T, state>;

template<std::size_t Idx, typename... Qs>
constexpr auto& get(state<Qs...>& s)
{
  return get<Idx>(s.variables_);
}

template<std::size_t Idx, typename... Qs>
constexpr const auto& get(const state<Qs...>& s)
{
  return get<Idx>(s.variables_);
}

// estimation
template<QuantityOrQuantityPoint QQP, QuantityOrQuantityPoint... QQPs>
struct estimation {
private:
  using uncertainty_ref = decltype(QQP::reference * QQP::reference);
  using uncertainty_type =
    units::quantity<typename uncertainty_ref::dimension, typename uncertainty_ref::unit, typename QQP::rep>;
public:
  kalman::state<QQP, QQPs...> state;  // TODO extend kalman functions to work with this variadic patermater list
  uncertainty_type uncertainty;
};

#if UNITS_COMP_CLANG <= 14

template<QuantityOrQuantityPoint QQP, units::Quantity U>
estimation(state<QQP>, U) -> estimation<QQP>;

#endif

// kalman gain
template<units::Quantity Q>
constexpr units::dimensionless<units::one> kalman_gain(Q estimate_uncertainty, Q measurement_uncertainty)
{
  return estimate_uncertainty / (estimate_uncertainty + measurement_uncertainty);
}

// state update
template<typename Q, QuantityOrQuantityPoint QM, units::Dimensionless K>
  requires units::equivalent<typename Q::dimension, typename QM::dimension>
constexpr state<Q> state_update(const state<Q>& predicted, QM measured, K gain)
{
  return {get<0>(predicted) + gain * (measured - get<0>(predicted))};
}

template<typename Q1, typename Q2, QuantityOrQuantityPoint QM, units::Dimensionless K, units::isq::Time T>
  requires units::equivalent<typename Q1::dimension, typename QM::dimension>
constexpr state<Q1, Q2> state_update(const state<Q1, Q2>& predicted, QM measured, std::array<K, 2> gain, T interval)
{
  const auto q1 = get<0>(predicted) + get<0>(gain) * (measured - get<0>(predicted));
  const auto q2 = get<1>(predicted) + get<1>(gain) * (measured - get<0>(predicted)) / interval;
  return {q1, q2};
}

template<typename Q1, typename Q2, typename Q3, QuantityOrQuantityPoint QM, units::Dimensionless K, units::isq::Time T>
  requires units::equivalent<typename Q1::dimension, typename QM::dimension>
constexpr state<Q1, Q2, Q3> state_update(const state<Q1, Q2, Q3>& predicted, QM measured, std::array<K, 3> gain,
                                         T interval)
{
  const auto q1 = get<0>(predicted) + get<0>(gain) * (measured - get<0>(predicted));
  const auto q2 = get<1>(predicted) + get<1>(gain) * (measured - get<0>(predicted)) / interval;
  const auto q3 = get<2>(predicted) + get<2>(gain) * (measured - get<0>(predicted)) / (interval * interval / 2);
  return {q1, q2, q3};
}

// covariance update
template<units::Quantity Q, units::Dimensionless K>
constexpr Q covariance_update(Q uncertainty, K gain)
{
  return (1 - gain) * uncertainty;
}

// state extrapolation
template<typename Q1, typename Q2, units::isq::Time T>
constexpr state<Q1, Q2> state_extrapolation(const state<Q1, Q2>& estimated, T interval)
{
  const auto q1 = get<0>(estimated) + get<1>(estimated) * interval;
  const auto q2 = get<1>(estimated);
  return {q1, q2};
}

template<typename Q1, typename Q2, typename Q3, units::isq::Time T>
constexpr state<Q1, Q2, Q3> state_extrapolation(const state<Q1, Q2, Q3>& estimated, T interval)
{
  const auto q1 = get<0>(estimated) + get<1>(estimated) * interval + get<2>(estimated) * pow<2>(interval) / 2;
  const auto q2 = get<1>(estimated) + get<2>(estimated) * interval;
  const auto q3 = get<2>(estimated);
  return {q1, q2, q3};
}

// covariance extrapolation
template<units::Quantity Q>
constexpr Q covariance_extrapolation(Q uncertainty, Q process_noise_variance)
{
  return uncertainty + process_noise_variance;
}

}  // namespace kalman

template<typename... Qs>
struct STD_FMT::formatter<kalman::state<Qs...>> {
  constexpr auto parse(format_parse_context& ctx)
  {
    units::detail::dynamic_specs_handler handler(specs, ctx);
    return units::detail::parse_format_specs(ctx.begin(), ctx.end(), handler);
  }

  template<typename FormatContext>
  auto format(const kalman::state<Qs...>& s, FormatContext& ctx)
  {
    std::string value_buffer;
    auto to_value_buffer = std::back_inserter(value_buffer);
    if (specs.precision != -1) {
      if constexpr (sizeof...(Qs) == 1)
        STD_FMT::format_to(to_value_buffer, "{1:%.{0}Q %q}", specs.precision, kalman::get<0>(s));
      else if constexpr (sizeof...(Qs) == 2)
        STD_FMT::format_to(to_value_buffer, "{{ {1:%.{0}Q %q}, {2:%.{0}Q %q} }}", specs.precision, kalman::get<0>(s),
                           kalman::get<1>(s));
      else
        STD_FMT::format_to(to_value_buffer, "{{ {1:%.{0}Q %q}, {2:%.{0}Q %q}, {3:%.{0}Q %q} }}", specs.precision,
                           kalman::get<0>(s), kalman::get<1>(s), kalman::get<2>(s));
    } else {
      if constexpr (sizeof...(Qs) == 1)
        STD_FMT::format_to(to_value_buffer, "{}", kalman::get<0>(s));
      else if constexpr (sizeof...(Qs) == 2)
        STD_FMT::format_to(to_value_buffer, "{{ {}, {} }}", kalman::get<0>(s), kalman::get<1>(s));
      else
        STD_FMT::format_to(to_value_buffer, "{{ {}, {}, {} }}", kalman::get<0>(s), kalman::get<1>(s),
                           kalman::get<2>(s));
    }

    std::string global_format_buffer;
    units::detail::quantity_global_format_specs<char> global_specs = {specs.fill, specs.align, specs.width};
    units::detail::format_global_buffer(std::back_inserter(global_format_buffer), global_specs);

    return STD_FMT::vformat_to(ctx.out(), global_format_buffer, STD_FMT::make_format_args(value_buffer));
  }
private:
  units::detail::dynamic_format_specs<char> specs;
};

template<typename Q>
struct STD_FMT::formatter<kalman::estimation<Q>> {
  constexpr auto parse(format_parse_context& ctx)
  {
    units::detail::dynamic_specs_handler handler(specs, ctx);
    return units::detail::parse_format_specs(ctx.begin(), ctx.end(), handler);
  }

  template<typename FormatContext>
  auto format(kalman::estimation<Q> e, FormatContext& ctx)
  {
    units::Quantity auto q = [](const Q& t) {
      if constexpr (units::Quantity<Q>)
        return t;
      else
        return t.relative();
    }(kalman::get<0>(e.state));

    std::string value_buffer;
    auto to_value_buffer = std::back_inserter(value_buffer);
    if (specs.precision != -1) {
      STD_FMT::format_to(to_value_buffer, "{0:%.{2}Q} ± {1:%.{2}Q} {0:%q}", q, sqrt(e.uncertainty), specs.precision);
    } else {
      STD_FMT::format_to(to_value_buffer, "{0:%Q} ± {1:%Q} {0:%q}", q, sqrt(e.uncertainty));
    }

    std::string global_format_buffer;
    units::detail::quantity_global_format_specs<char> global_specs = {specs.fill, specs.align, specs.width};
    units::detail::format_global_buffer(std::back_inserter(global_format_buffer), global_specs);

    return STD_FMT::vformat_to(ctx.out(), global_format_buffer, STD_FMT::make_format_args(value_buffer));
  }
private:
  units::detail::dynamic_format_specs<char> specs;
};
