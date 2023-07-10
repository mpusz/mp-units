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

#include <mp-units/bits/fmt_hacks.h>
#include <mp-units/format.h>
#include <mp-units/math.h>
#include <mp-units/quantity.h>
#include <mp-units/quantity_point.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <tuple>

namespace kalman {

template<typename T>
concept QuantityOrQuantityPoint = mp_units::Quantity<T> || mp_units::QuantityPoint<T>;

template<mp_units::Dimension auto... Ds>
inline constexpr bool are_time_derivatives = false;

template<mp_units::Dimension auto D>
inline constexpr bool are_time_derivatives<D> = true;

template<mp_units::Dimension auto D1, mp_units::Dimension auto D2, mp_units::Dimension auto... Ds>
inline constexpr bool are_time_derivatives<D1, D2, Ds...> =
  (D1 / D2 == mp_units::isq::dim_time) && are_time_derivatives<D2, Ds...>;

// state
template<QuantityOrQuantityPoint... QQPs>
  requires(sizeof...(QQPs) > 0) && (sizeof...(QQPs) <= 3) && are_time_derivatives<QQPs::dimension...>
struct state {
  std::tuple<QQPs...> variables_;
  constexpr state(QQPs... qqps) : variables_(std::move(qqps)...) {}
};

template<typename T>
concept State = mp_units::is_specialization_of<T, state>;

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
  static constexpr auto uncertainty_ref = QQP::reference * QQP::reference;
  using uncertainty_type = mp_units::quantity<uncertainty_ref, typename QQP::rep>;
public:
  kalman::state<QQP, QQPs...> state;  // TODO extend kalman functions to work with this variadic parameter list
  uncertainty_type uncertainty;
};

template<QuantityOrQuantityPoint QQP, mp_units::Quantity U>
estimation(state<QQP>, U) -> estimation<QQP>;

// kalman gain
template<mp_units::Quantity Q>
constexpr mp_units::quantity<mp_units::dimensionless[mp_units::one]> kalman_gain(Q estimate_uncertainty,
                                                                                 Q measurement_uncertainty)
{
  return estimate_uncertainty / (estimate_uncertainty + measurement_uncertainty);
}

// state update
template<typename Q, QuantityOrQuantityPoint QM, mp_units::QuantityOf<mp_units::dimensionless> K>
  requires(Q::quantity_spec == QM::quantity_spec)
constexpr state<Q> state_update(const state<Q>& predicted, QM measured, K gain)
{
  return {get<0>(predicted) + gain * (measured - get<0>(predicted))};
}

template<typename Q1, typename Q2, QuantityOrQuantityPoint QM, mp_units::QuantityOf<mp_units::dimensionless> K,
         mp_units::QuantityOf<mp_units::isq::time> T>
  requires(Q1::quantity_spec == QM::quantity_spec)
constexpr state<Q1, Q2> state_update(const state<Q1, Q2>& predicted, QM measured, std::array<K, 2> gain, T interval)
{
  const auto q1 = get<0>(predicted) + get<0>(gain) * (measured - get<0>(predicted));
  const auto q2 = get<1>(predicted) + get<1>(gain) * (measured - get<0>(predicted)) / interval;
  return {q1, q2};
}

template<typename Q1, typename Q2, typename Q3, QuantityOrQuantityPoint QM,
         mp_units::QuantityOf<mp_units::dimensionless> K, mp_units::QuantityOf<mp_units::isq::time> T>
  requires(Q1::quantity_spec == QM::quantity_spec)
constexpr state<Q1, Q2, Q3> state_update(const state<Q1, Q2, Q3>& predicted, QM measured, std::array<K, 3> gain,
                                         T interval)
{
  const auto q1 = get<0>(predicted) + get<0>(gain) * (measured - get<0>(predicted));
  const auto q2 = get<1>(predicted) + get<1>(gain) * (measured - get<0>(predicted)) / interval;
  const auto q3 = get<2>(predicted) + get<2>(gain) * (measured - get<0>(predicted)) / (interval * interval / 2);
  return {q1, q2, q3};
}

// covariance update
template<mp_units::Quantity Q, mp_units::QuantityOf<mp_units::dimensionless> K>
constexpr Q covariance_update(Q uncertainty, K gain)
{
  return (1 * mp_units::one - gain) * uncertainty;
}

// state extrapolation
template<typename Q1, typename Q2, mp_units::QuantityOf<mp_units::isq::time> T>
constexpr state<Q1, Q2> state_extrapolation(const state<Q1, Q2>& estimated, T interval)
{
  const auto q1 = get<0>(estimated) + get<1>(estimated) * interval;
  const auto q2 = get<1>(estimated);
  return {q1, q2};
}

template<typename Q1, typename Q2, typename Q3, mp_units::QuantityOf<mp_units::isq::time> T>
constexpr state<Q1, Q2, Q3> state_extrapolation(const state<Q1, Q2, Q3>& estimated, T interval)
{
  const auto q1 = get<0>(estimated) + get<1>(estimated) * interval + get<2>(estimated) * pow<2>(interval) / 2;
  const auto q2 = get<1>(estimated) + get<2>(estimated) * interval;
  const auto q3 = get<2>(estimated);
  return {q1, q2, q3};
}

// covariance extrapolation
template<mp_units::Quantity Q>
constexpr Q covariance_extrapolation(Q uncertainty, Q process_noise_variance)
{
  return uncertainty + process_noise_variance;
}

}  // namespace kalman

template<typename... Qs>
struct MP_UNITS_STD_FMT::formatter<kalman::state<Qs...>> {
  constexpr auto parse(format_parse_context& ctx)
  {
    mp_units::detail::dynamic_specs_handler handler(specs, ctx);
    return mp_units::detail::parse_format_specs(ctx.begin(), ctx.end(), handler);
  }

  template<typename FormatContext>
  auto format(const kalman::state<Qs...>& s, FormatContext& ctx)
  {
    std::string value_buffer;
    auto to_value_buffer = std::back_inserter(value_buffer);
    if (specs.precision != -1) {
      if constexpr (sizeof...(Qs) == 1)
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{1:%.{0}Q %q}", specs.precision, kalman::get<0>(s));
      else if constexpr (sizeof...(Qs) == 2)
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{{ {1:%.{0}Q %q}, {2:%.{0}Q %q} }}", specs.precision,
                                    kalman::get<0>(s), kalman::get<1>(s));
      else
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{{ {1:%.{0}Q %q}, {2:%.{0}Q %q}, {3:%.{0}Q %q} }}",
                                    specs.precision, kalman::get<0>(s), kalman::get<1>(s), kalman::get<2>(s));
    } else {
      if constexpr (sizeof...(Qs) == 1)
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{}", kalman::get<0>(s));
      else if constexpr (sizeof...(Qs) == 2)
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{{ {}, {} }}", kalman::get<0>(s), kalman::get<1>(s));
      else
        MP_UNITS_STD_FMT::format_to(to_value_buffer, "{{ {}, {}, {} }}", kalman::get<0>(s), kalman::get<1>(s),
                                    kalman::get<2>(s));
    }

    std::string global_format_buffer;
    mp_units::detail::quantity_global_format_specs<char> global_specs = {specs.fill, specs.align, specs.width};
    mp_units::detail::format_global_buffer(std::back_inserter(global_format_buffer), global_specs);

    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                        MP_UNITS_STD_FMT::make_format_args(value_buffer));
  }
private:
  mp_units::detail::dynamic_format_specs<char> specs;
};

template<typename Q>
struct MP_UNITS_STD_FMT::formatter<kalman::estimation<Q>> {
  constexpr auto parse(format_parse_context& ctx)
  {
    mp_units::detail::dynamic_specs_handler handler(specs, ctx);
    return mp_units::detail::parse_format_specs(ctx.begin(), ctx.end(), handler);
  }

  template<typename FormatContext>
  auto format(kalman::estimation<Q> e, FormatContext& ctx)
  {
    mp_units::Quantity auto q = [](const Q& t) {
      if constexpr (mp_units::Quantity<Q>)
        return t;
      else
        return t.relative();
    }(kalman::get<0>(e.state));

    std::string value_buffer;
    auto to_value_buffer = std::back_inserter(value_buffer);
    if (specs.precision != -1) {
      MP_UNITS_STD_FMT::format_to(to_value_buffer, "{0:%.{2}Q} ± {1:%.{2}Q} {0:%q}", q, sqrt(e.uncertainty),
                                  specs.precision);
    } else {
      MP_UNITS_STD_FMT::format_to(to_value_buffer, "{0:%Q} ± {1:%Q} {0:%q}", q, sqrt(e.uncertainty));
    }

    std::string global_format_buffer;
    mp_units::detail::quantity_global_format_specs<char> global_specs = {specs.fill, specs.align, specs.width};
    mp_units::detail::format_global_buffer(std::back_inserter(global_format_buffer), global_specs);

    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), global_format_buffer,
                                        MP_UNITS_STD_FMT::make_format_args(value_buffer));
  }
private:
  mp_units::detail::dynamic_format_specs<char> specs;
};
