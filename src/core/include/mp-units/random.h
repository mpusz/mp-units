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

#include <mp-units/bits/module_macros.h>
#include <mp-units/quantity.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <functional>
#include <random>
#endif

namespace mp_units {

namespace detail {

template<Quantity Q, typename InputIt>
std::vector<typename Q::rep> i_qty_to_rep(InputIt first, InputIt last)
{
  std::vector<typename Q::rep> intervals_rep;
  intervals_rep.reserve(static_cast<size_t>(std::distance(first, last)));
  for (auto itr = first; itr != last; ++itr) {
    intervals_rep.push_back(itr->numerical_value_ref_in(Q::unit));
  }
  return intervals_rep;
}

template<Quantity Q>
std::vector<typename Q::rep> bl_qty_to_rep(std::initializer_list<Q>& bl)
{
  std::vector<typename Q::rep> bl_rep;
  bl_rep.reserve(bl.size());
  for (const Q& qty : bl) {
    bl_rep.push_back(qty.numerical_value_ref_in(Q::unit));
  }
  return bl_rep;
}

template<Quantity Q, typename UnaryOperation>
std::vector<typename Q::rep> fw_bl_pwc(std::initializer_list<Q>& bl, UnaryOperation fw)
{
  using rep = MP_UNITS_TYPENAME Q::rep;
  std::vector<rep> w_bl;
  w_bl.reserve(bl.size());
  for (const Q& qty : bl) {
    w_bl.push_back(fw(qty));
  }
  std::vector<rep> weights;
  weights.reserve(bl.size());
  for (size_t i = 0; i < bl.size() - 1; ++i) {
    weights.push_back(w_bl[i] + w_bl[i + 1]);
  }
  weights.push_back(0);
  return weights;
}

template<Quantity Q, typename UnaryOperation>
std::vector<typename Q::rep> fw_bl_pwl(std::initializer_list<Q>& bl, UnaryOperation fw)
{
  std::vector<typename Q::rep> weights;
  weights.reserve(bl.size());
  for (const Q& qty : bl) {
    weights.push_back(fw(qty));
  }
  return weights;
}
}  // namespace detail

MP_UNITS_EXPORT_BEGIN

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct uniform_int_distribution : public std::uniform_int_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::uniform_int_distribution<rep>;

  uniform_int_distribution() : base() {}
  uniform_int_distribution(const Q& a, const Q& b) :
      base(a.numerical_value_ref_in(Q::unit), b.numerical_value_ref_in(Q::unit))
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q a() const { return base::a() * Q::reference; }
  Q b() const { return base::b() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct uniform_real_distribution : public std::uniform_real_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::uniform_real_distribution<rep>;

  uniform_real_distribution() : base() {}
  uniform_real_distribution(const Q& a, const Q& b) :
      base(a.numerical_value_ref_in(Q::unit), b.numerical_value_ref_in(Q::unit))
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q a() const { return base::a() * Q::reference; }
  Q b() const { return base::b() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct binomial_distribution : public std::binomial_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::binomial_distribution<rep>;

  binomial_distribution() : base() {}
  binomial_distribution(const Q& t, double p) : base(t.numerical_value_ref_in(Q::unit), p) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q t() const { return base::t() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct negative_binomial_distribution : public std::negative_binomial_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::negative_binomial_distribution<rep>;

  negative_binomial_distribution() : base() {}
  negative_binomial_distribution(const Q& k, double p) : base(k.numerical_value_ref_in(Q::unit), p) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q k() const { return base::k() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct geometric_distribution : public std::geometric_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::geometric_distribution<rep>;

  geometric_distribution() : base() {}
  geometric_distribution(double p) : base(p) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct poisson_distribution : public std::poisson_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::poisson_distribution<rep>;

  poisson_distribution() : base() {}
  explicit poisson_distribution(double p) : base(p) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct exponential_distribution : public std::exponential_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::exponential_distribution<rep>;

  exponential_distribution() : base() {}
  explicit exponential_distribution(const rep& lambda) : base(lambda) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct gamma_distribution : public std::gamma_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::gamma_distribution<rep>;

  gamma_distribution() : base() {}
  gamma_distribution(const rep& alpha, const rep& beta) : base(alpha, beta) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct weibull_distribution : public std::weibull_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::weibull_distribution<rep>;

  weibull_distribution() : base() {}
  weibull_distribution(const rep& a, const rep& b) : base(a, b) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct extreme_value_distribution : public std::extreme_value_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::extreme_value_distribution<rep>;

  extreme_value_distribution() : base() {}
  extreme_value_distribution(const Q& a, const rep& b) : base(a.numerical_value_ref_in(Q::unit), b) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return Q(base::operator()(g));
  }

  Q a() const { return base::a() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct normal_distribution : public std::normal_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::normal_distribution<rep>;

  normal_distribution() : base() {}
  normal_distribution(const Q& mean, const Q& stddev) :
      base(mean.numerical_value_ref_in(Q::unit), stddev.numerical_value_ref_in(Q::unit))
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return Q(base::operator()(g));
  }

  Q mean() const { return base::mean() * Q::reference; }
  Q stddev() const { return base::stddev() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct lognormal_distribution : public std::lognormal_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::lognormal_distribution<rep>;

  lognormal_distribution() : base() {}
  lognormal_distribution(const Q& m, const Q& s) :
      base(m.numerical_value_ref_in(Q::unit), s.numerical_value_ref_in(Q::unit))
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q m() const { return base::m() * Q::reference; }
  Q s() const { return base::s() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct chi_squared_distribution : public std::chi_squared_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::chi_squared_distribution<rep>;

  chi_squared_distribution() : base() {}
  explicit chi_squared_distribution(const rep& n) : base(n) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct cauchy_distribution : public std::cauchy_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::cauchy_distribution<rep>;

  cauchy_distribution() : base() {}
  cauchy_distribution(const Q& a, const Q& b) :
      base(a.numerical_value_ref_in(Q::unit), b.numerical_value_ref_in(Q::unit))
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q a() const { return base::a() * Q::reference; }
  Q b() const { return base::b() * Q::reference; }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct fisher_f_distribution : public std::fisher_f_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::fisher_f_distribution<rep>;

  fisher_f_distribution() : base() {}
  fisher_f_distribution(const rep& m, const rep& n) : base(m, n) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
struct student_t_distribution : public std::student_t_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::student_t_distribution<rep>;

  student_t_distribution() : base() {}
  explicit student_t_distribution(const rep& n) : base(n) {}

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::integral<typename Q::rep>
struct discrete_distribution : public std::discrete_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::discrete_distribution<rep>;

  discrete_distribution() : base() {}

  template<typename InputIt>
  discrete_distribution(InputIt first, InputIt last) : base(first, last)
  {
  }

  discrete_distribution(std::initializer_list<double> weights) : base(weights) {}

  template<typename UnaryOperation>
  discrete_distribution(std::size_t count, double xmin, double xmax, UnaryOperation unary_op) :
      base(count, xmin, xmax, unary_op)
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
class piecewise_constant_distribution : public std::piecewise_constant_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::piecewise_constant_distribution<rep>;

  template<typename InputIt>
  piecewise_constant_distribution(const std::vector<rep>& i, InputIt first_w) : base(i.cbegin(), i.cend(), first_w)
  {
  }

  piecewise_constant_distribution(const std::vector<rep>& bl, const std::vector<rep>& weights) :
      base(bl.cbegin(), bl.cend(), weights.cbegin())
  {
  }

public:
  piecewise_constant_distribution() : base() {}

  template<typename InputIt1, typename InputIt2>
  piecewise_constant_distribution(InputIt1 first_i, InputIt1 last_i, InputIt2 first_w) :
      piecewise_constant_distribution(detail::i_qty_to_rep<Q>(first_i, last_i), first_w)
  {
  }

  template<typename UnaryOperation>
  piecewise_constant_distribution(std::initializer_list<Q> bl, UnaryOperation fw) :
      piecewise_constant_distribution(detail::bl_qty_to_rep(bl), detail::fw_bl_pwc(bl, fw))
  {
  }

  template<typename UnaryOperation>
  piecewise_constant_distribution(std::size_t nw, const Q& xmin, const Q& xmax, UnaryOperation fw) :
      base(nw, xmin.numerical_value_ref_in(Q::unit), xmax.numerical_value_ref_in(Q::unit),
           [fw](rep val) { return fw(val * Q::reference); })
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  std::vector<Q> intervals() const
  {
    std::vector<rep> intervals_rep = base::intervals();
    std::vector<Q> intervals_qty;
    intervals_qty.reserve(intervals_rep.size());
    for (const rep& val : intervals_rep) {
      intervals_qty.push_back(val * Q::reference);
    }
    return intervals_qty;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

template<Quantity Q>
  requires std::floating_point<typename Q::rep>
class piecewise_linear_distribution : public std::piecewise_linear_distribution<typename Q::rep> {
  using rep = MP_UNITS_TYPENAME Q::rep;
  using base = MP_UNITS_TYPENAME std::piecewise_linear_distribution<rep>;

  template<typename InputIt>
  piecewise_linear_distribution(const std::vector<rep>& i, InputIt first_w) : base(i.cbegin(), i.cend(), first_w)
  {
  }

  piecewise_linear_distribution(const std::vector<rep>& bl, const std::vector<rep>& weights) :
      base(bl.cbegin(), bl.cend(), weights.cbegin())
  {
  }

public:
  piecewise_linear_distribution() : base() {}

  template<typename InputIt1, typename InputIt2>
  piecewise_linear_distribution(InputIt1 first_i, InputIt1 last_i, InputIt2 first_w) :
      piecewise_linear_distribution(detail::i_qty_to_rep<Q>(first_i, last_i), first_w)
  {
  }

  template<typename UnaryOperation>
  piecewise_linear_distribution(std::initializer_list<Q> bl, UnaryOperation fw) :
      piecewise_linear_distribution(detail::bl_qty_to_rep(bl), detail::fw_bl_pwl(bl, fw))
  {
  }

  template<typename UnaryOperation>
  piecewise_linear_distribution(std::size_t nw, const Q& xmin, const Q& xmax, UnaryOperation fw) :
      base(nw, xmin.numerical_value_ref_in(Q::unit), xmax.numerical_value_ref_in(Q::unit),
           [fw](rep val) { return fw(val * Q::reference); })
  {
  }

  template<typename Generator>
  Q operator()(Generator& g)
  {
    return base::operator()(g) * Q::reference;
  }

  std::vector<Q> intervals() const
  {
    std::vector<rep> intervals_rep = base::intervals();
    std::vector<Q> intervals_qty;
    intervals_qty.reserve(intervals_rep.size());
    for (const rep& val : intervals_rep) {
      intervals_qty.push_back(val * Q::reference);
    }
    return intervals_qty;
  }

  Q min() const { return base::min() * Q::reference; }
  Q max() const { return base::max() * Q::reference; }
};

MP_UNITS_EXPORT_END

}  // namespace mp_units
