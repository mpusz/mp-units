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

#include <mp-units/compat_macros.h>
#include <algorithm>
#include <locale>
#include <tuple>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/ext/algorithm.h>
#include <mp-units/format.h>
#include <mp-units/framework/quantity.h>
#include <mp-units/framework/quantity_point.h>
#include <mp-units/math.h>
#include <mp-units/systems/isq/base_quantities.h>
#endif

namespace kalman {

namespace detail {

template<mp_units::Dimension auto... Ds>
inline constexpr bool are_time_derivatives = false;

template<mp_units::Dimension auto D>
inline constexpr bool are_time_derivatives<D> = true;

template<mp_units::Dimension auto D1, mp_units::Dimension auto D2, mp_units::Dimension auto... Ds>
inline constexpr bool are_time_derivatives<D1, D2, Ds...> =
  (D1 / D2 == mp_units::isq::dim_time) && are_time_derivatives<D2, Ds...>;

}  // namespace detail

// system state
template<mp_units::QuantityPoint... QPs>
  requires(sizeof...(QPs) > 0) && (sizeof...(QPs) <= 3) && detail::are_time_derivatives<QPs::dimension...>
class system_state {
  std::tuple<QPs...> variables_;
public:
  constexpr explicit system_state(QPs... qps) : variables_(std::move(qps)...) {}

  template<std::size_t Idx>
  [[nodiscard]] friend constexpr auto& get(system_state<QPs...>& s)
  {
    return get<Idx>(s.variables_);
  }

  template<std::size_t Idx>
  [[nodiscard]] friend constexpr const auto& get(const system_state<QPs...>& s)
  {
    return get<Idx>(s.variables_);
  }
};

template<typename T>
concept SystemState = mp_units::is_specialization_of<T, system_state>;


// system state estimation
template<mp_units::QuantityPoint QP, mp_units::QuantityPoint... Rest>
  requires requires { typename system_state<QP, Rest...>; }
class system_state_estimate {
public:
  using state_type = system_state<QP, Rest...>;
  using standard_deviation_type = QP::quantity_type;
  using variance_type =
    mp_units::quantity<pow<2>(standard_deviation_type::reference), typename standard_deviation_type::rep>;
private:
  state_type state_;
  variance_type variance_;
public:
  constexpr system_state_estimate(state_type state, standard_deviation_type standard_deviation) :
      state_(state), variance_(pow<2>(standard_deviation))
  {
  }
  constexpr system_state_estimate(state_type state, variance_type variance) : state_(state), variance_(variance) {}
  [[nodiscard]] constexpr const state_type& state() const { return state_; }
  [[nodiscard]] constexpr const variance_type& variance() const { return variance_; }
  [[nodiscard]] constexpr standard_deviation_type standard_deviation() const { return sqrt(variance_); }
};

// kalman gain
template<mp_units::Quantity Q1, mp_units::Quantity Q2>
  requires requires { mp_units::common_reference(Q1::reference, Q2::reference); }
[[nodiscard]] constexpr mp_units::quantity<mp_units::dimensionless[mp_units::one]> kalman_gain(
  Q1 variance_in_estimate, Q2 variance_in_measurement)
{
  return variance_in_estimate / (variance_in_estimate + variance_in_measurement);
}

// state update
template<typename QP, mp_units::QuantityPoint QM, mp_units::QuantityOf<mp_units::dimensionless> K>
  requires(implicitly_convertible(QM::quantity_spec, QP::quantity_spec))
[[nodiscard]] constexpr system_state<QP> state_update(const system_state<QP>& predicted, QM measured, K gain)
{
  return system_state<QP>{get<0>(predicted) + gain * (measured - get<0>(predicted))};
}

template<typename QP1, typename QP2, mp_units::QuantityPoint QM, mp_units::QuantityOf<mp_units::dimensionless> K,
         mp_units::QuantityOf<mp_units::isq::time> T>
  requires(implicitly_convertible(QM::quantity_spec, QP1::quantity_spec))
[[nodiscard]] constexpr system_state<QP1, QP2> state_update(const system_state<QP1, QP2>& predicted, QM measured,
                                                            std::array<K, 2> gain, T interval)
{
  const auto qp1 = fma(get<0>(gain), measured - get<0>(predicted), get<0>(predicted));
  const auto qp2 = fma(get<1>(gain), (measured - get<0>(predicted)) / interval, get<1>(predicted));
  return system_state<QP1, QP2>{qp1, qp2};
}

template<typename QP1, typename QP2, typename QP3, mp_units::QuantityPoint QM,
         mp_units::QuantityOf<mp_units::dimensionless> K, mp_units::QuantityOf<mp_units::isq::time> T>
  requires(implicitly_convertible(QM::quantity_spec, QP1::quantity_spec))
[[nodiscard]] constexpr system_state<QP1, QP2, QP3> state_update(const system_state<QP1, QP2, QP3>& predicted,
                                                                 QM measured, std::array<K, 3> gain, T interval)
{
  const auto qp1 = fma(get<0>(gain), measured - get<0>(predicted), get<0>(predicted));
  const auto qp2 = fma(get<1>(gain), (measured - get<0>(predicted)) / interval, get<1>(predicted));
  const auto qp3 = fma(get<2>(gain), (measured - get<0>(predicted)) / (interval * interval / 2), get<2>(predicted));
  return system_state<QP1, QP2, QP3>{qp1, qp2, qp3};
}

// covariance update
template<mp_units::Quantity Q, mp_units::QuantityOf<mp_units::dimensionless> K>
[[nodiscard]] constexpr Q covariance_update(Q uncertainty, K gain)
{
  return (1 * mp_units::one - gain) * uncertainty;
}

template<mp_units::QuantityPoint... QPs, mp_units::QuantityPoint QP, mp_units::QuantityOf<mp_units::dimensionless> K>
[[nodiscard]] constexpr system_state_estimate<QPs...> state_estimate_update(
  const system_state_estimate<QPs...>& previous, QP measurement, K gain)
{
  return {state_update(previous.state(), measurement, gain), covariance_update(previous.variance(), gain)};
};


// state extrapolation
template<typename QP1, typename QP2, mp_units::QuantityOf<mp_units::isq::time> T>
[[nodiscard]] constexpr system_state<QP1, QP2> state_extrapolation(const system_state<QP1, QP2>& estimated, T interval)
{
  auto to_quantity = [](const auto& qp) { return qp.quantity_ref_from(qp.point_origin); };
  const auto qp1 = fma(to_quantity(get<1>(estimated)), interval, get<0>(estimated));
  const auto qp2 = get<1>(estimated);
  return system_state<QP1, QP2>{qp1, qp2};
}

template<typename QP1, typename QP2, typename QP3, mp_units::QuantityOf<mp_units::isq::time> T>
[[nodiscard]] constexpr system_state<QP1, QP2, QP3> state_extrapolation(const system_state<QP1, QP2, QP3>& estimated,
                                                                        T interval)
{
  auto to_quantity = [](const auto& qp) { return qp.quantity_ref_from(qp.point_origin); };
  const auto qp1 = to_quantity(get<2>(estimated)) * pow<2>(interval) / 2 +
                   fma(to_quantity(get<1>(estimated)), interval, get<0>(estimated));
  const auto qp2 = fma(to_quantity(get<2>(estimated)), interval, get<1>(estimated));
  const auto qp3 = get<2>(estimated);
  return system_state<QP1, QP2, QP3>{qp1, qp2, qp3};
}

// covariance extrapolation
template<mp_units::Quantity Q1, mp_units::Quantity Q2>
  requires requires { mp_units::common_reference(Q1::reference, Q2::reference); }
[[nodiscard]] constexpr mp_units::Quantity auto covariance_extrapolation(Q1 uncertainty, Q2 process_noise_variance)
{
  return uncertainty + process_noise_variance;
}

}  // namespace kalman

template<auto R, auto PO, typename Rep, typename Char>
struct MP_UNITS_STD_FMT::formatter<mp_units::quantity_point<R, PO, Rep>, Char> :
    MP_UNITS_STD_FMT::formatter<typename mp_units::quantity_point<R, PO, Rep>::quantity_type> {
  template<typename FormatContext>
  auto format(const mp_units::quantity_point<R, PO, Rep>& qp, FormatContext& ctx) const -> decltype(ctx.out())
  {
    return MP_UNITS_STD_FMT::formatter<typename mp_units::quantity_point<R, PO, Rep>::quantity_type>::format(
      qp.quantity_ref_from(qp.point_origin), ctx);
  }
};

template<typename... QPs, typename Char>
class MP_UNITS_STD_FMT::formatter<kalman::system_state<QPs...>, Char> {
  using format_specs = mp_units::detail::fill_align_width_format_specs<Char>;
  format_specs specs_{};
  std::array<std::basic_string<Char>, sizeof...(QPs)> format_str_;
  std::tuple<MP_UNITS_STD_FMT::formatter<typename QPs::quantity_type, Char>...> formatters_{};

  template<typename Formatter>
  constexpr const Char* parse_default_spec(const Char* begin, const Char* end, Formatter& f, std::string& format_str)
  {
    if (begin == end || *begin++ != '[')
      throw MP_UNITS_STD_FMT::format_error("`default-spec` should contain a `[` character");
    auto it = begin;
    for (int nested_brackets = 0; it != end && !(*it == ']' && nested_brackets == 0); it++) {
      if (*it == '[') ++nested_brackets;
      if (*it == ']') {
        if (nested_brackets == 0) throw MP_UNITS_STD_FMT::format_error("unmatched ']' in format string");
        --nested_brackets;
      }
    }
    format_str = "{:" + std::string(begin, it) + '}';
    if (it == end) throw MP_UNITS_STD_FMT::format_error("unmatched '[' in format string");
    MP_UNITS_STD_FMT::basic_format_parse_context<Char> ctx(std::string_view(begin, it));
    auto ptr = f.parse(ctx);
    if (ptr != it) throw MP_UNITS_STD_FMT::format_error("invalid subentity format '" + std::string(begin, it) + "'");
    return ++it;  // skip `]`
  }

  template<std::size_t... Is>
  [[nodiscard]] constexpr const Char* parse_default_spec(const Char* begin, const Char* end, size_t idx,
                                                         std::index_sequence<Is...>)
  {
    auto parse = [&](bool flag, auto& f, std::basic_string<Char>& str) {
      return flag ? parse_default_spec(begin, end, f, str) : begin;
    };
    return std::max({parse(idx == Is, std::get<Is>(formatters_), format_str_[Is])...});
  }

  [[nodiscard]] constexpr const Char* parse_defaults_specs(const Char* begin, const Char* end)
  {
    if (begin == end || *begin == '}') return begin;
    if (*begin++ != ':') throw MP_UNITS_STD_FMT::format_error("`defaults-specs` should start with a `:`");
    do {
      auto c = *begin++;
      if (c < '0' || c >= static_cast<char>('0' + sizeof...(QPs)))
        throw MP_UNITS_STD_FMT::format_error(std::string("unknown `subentity-id` token '") + c + "'");
      const size_t idx = static_cast<size_t>(c - '0');
      begin = parse_default_spec(begin, end, idx, std::index_sequence_for<QPs...>{});
    } while (begin != end && *begin != '}');
    return begin;
  }

  template<typename OutputIt, typename FormatContext, std::size_t Idx>
  OutputIt format_system_state(OutputIt out, const kalman::system_state<QPs...>& s, FormatContext& ctx,
                               std::index_sequence<Idx>) const
  {
    const std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    auto f = [&](const std::basic_string<Char>& str, const mp_units::QuantityPoint auto& qp) {
      return MP_UNITS_STD_FMT::vformat_to(out, locale, str, MP_UNITS_STD_FMT::make_format_args(qp));
    };
    return f(get<Idx>(format_str_), get<Idx>(s));
  }

  template<typename OutputIt, typename FormatContext, std::size_t Idx, std::size_t... Rest>
    requires(sizeof...(Rest) > 0)
  OutputIt format_system_state(OutputIt out, const kalman::system_state<QPs...>& s, FormatContext& ctx,
                               std::index_sequence<Idx, Rest...>) const
  {
    const std::locale locale = MP_UNITS_FMT_LOCALE(ctx.locale());
    auto f = [&](const std::basic_string<Char>& str, const mp_units::QuantityPoint auto& qp) {
      return MP_UNITS_STD_FMT::vformat_to(out, locale, str, MP_UNITS_STD_FMT::make_format_args(qp));
    };
    return f(get<Idx>(format_str_), get<Idx>(s)), ((*out++ = ' ', f(get<Rest>(format_str_), get<Rest>(s))), ...);
  }

public:
  constexpr formatter()
  {
    for (auto& str : format_str_) str = "{}";
  }

  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
  {
    auto begin = ctx.begin(), end = ctx.end();

    begin = parse_fill_align_width(ctx, begin, end, specs_, mp_units::detail::fmt_align::right);
    if (begin == end) return begin;

    return parse_defaults_specs(begin, end);
  }

  template<typename FormatContext>
  auto format(const kalman::system_state<QPs...>& s, FormatContext& ctx) const -> decltype(ctx.out())
  {
    auto specs = specs_;
    mp_units::detail::handle_dynamic_spec<mp_units::detail::width_checker>(specs.width, specs.width_ref, ctx);

    if (specs.width == 0) {
      // Avoid extra copying if width is not specified
      format_system_state(ctx.out(), s, ctx, std::index_sequence_for<QPs...>{});
      return ctx.out();
    }
    std::basic_string<Char> quantity_buffer;
    format_system_state(std::back_inserter(quantity_buffer), s, ctx, std::index_sequence_for<QPs...>{});

    std::basic_string<Char> fill_align_width_format_str;
    mp_units::detail::format_global_buffer(std::back_inserter(fill_align_width_format_str), specs);
    return MP_UNITS_STD_FMT::vformat_to(ctx.out(), fill_align_width_format_str,
                                        MP_UNITS_STD_FMT::make_format_args(quantity_buffer));
  }
};
