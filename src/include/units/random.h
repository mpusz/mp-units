#pragma once

#include <random>
#include <functional>

namespace units {

template<typename Quantity>
struct uniform_int_distribution : public std::uniform_int_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::uniform_int_distribution<Rep>;
    
    uniform_int_distribution() : Base() {}
    uniform_int_distribution(Quantity a, Quantity b) : Base(a.count(), b.count()) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity a() const { return Quantity(Base::a()); }
    Quantity b() const { return Quantity(Base::b()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct uniform_real_distribution : public std::uniform_real_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::uniform_real_distribution<Rep>;
    
    uniform_real_distribution() : Base() {}
    uniform_real_distribution(Quantity a, Quantity b) : Base(a.count(), b.count()) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity a() const { return Quantity(Base::a()); }
    Quantity b() const { return Quantity(Base::b()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct binomial_distribution : public std::binomial_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::binomial_distribution<Rep>;
    
    binomial_distribution() : Base() {}
    binomial_distribution(Quantity t, double p) : Base(t.count(), p) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity t() const { return Quantity(Base::t()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct negative_binomial_distribution : public std::negative_binomial_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::negative_binomial_distribution<Rep>;
    
    negative_binomial_distribution() : Base() {}
    negative_binomial_distribution(Quantity k, double p) : Base(k.count(), p) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity k() const { return Quantity(Base::k()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct geometric_distribution : public std::geometric_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::geometric_distribution<Rep>;
    
    geometric_distribution() : Base() {}
    geometric_distribution(double p) : Base(p) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }

    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct poisson_distribution : public std::poisson_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::poisson_distribution<Rep>;
    
    poisson_distribution() : Base() {}
    poisson_distribution(double p) : Base(p) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct exponential_distribution : public std::exponential_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::exponential_distribution<Rep>;
    
    exponential_distribution() : Base() {}
    exponential_distribution(Rep lambda) : Base(lambda) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct gamma_distribution : public std::gamma_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::gamma_distribution<Rep>;
    
    gamma_distribution() : Base() {}
    gamma_distribution(Rep alpha, Rep beta) : Base(alpha, beta) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct weibull_distribution : public std::weibull_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::weibull_distribution<Rep>;
    
    weibull_distribution() : Base() {}
    weibull_distribution(Rep a, Rep b) : Base(a, b) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct extreme_value_distribution : public std::extreme_value_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::extreme_value_distribution<Rep>;
    
    extreme_value_distribution() : Base() {}
    extreme_value_distribution(Quantity a, Rep b) : Base(a.count(), b) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }

    Quantity a() const { return Quantity(Base::a()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct normal_distribution : public std::normal_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::normal_distribution<Rep>;
    
    normal_distribution() : Base() {}
    normal_distribution(Quantity mean, Quantity stddev) : Base(mean.count(), stddev.count()) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity mean() const { return Quantity(Base::mean()); }
    Quantity stddev() const { return Quantity(Base::stddev()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct lognormal_distribution : public std::lognormal_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::lognormal_distribution<Rep>;
    
    lognormal_distribution() : Base() {}
    lognormal_distribution(Quantity m, Quantity s) : Base(m.count(), s.count()) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity m() const { return Quantity(Base::m()); }
    Quantity s() const { return Quantity(Base::s()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct chi_squared_distribution : public std::chi_squared_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::chi_squared_distribution<Rep>;
    
    chi_squared_distribution() : Base() {}
    chi_squared_distribution(Rep n) : Base(n) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct cauchy_distribution : public std::cauchy_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::cauchy_distribution<Rep>;
    
    cauchy_distribution() : Base() {}
    cauchy_distribution(Quantity a, Quantity b) : Base(a.count(), b.count()) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity a() const { return Quantity(Base::a()); }
    Quantity b() const { return Quantity(Base::b()); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct fisher_f_distribution : public std::fisher_f_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::fisher_f_distribution<Rep>;
    
    fisher_f_distribution() : Base() {}
    fisher_f_distribution(Rep m, Rep n) : Base(m, n) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct student_t_distribution : public std::student_t_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::student_t_distribution<Rep>;
    
    student_t_distribution() : Base() {}
    student_t_distribution(Rep n) : Base(n) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
struct discrete_distribution : public std::discrete_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::discrete_distribution<Rep>;
    
    discrete_distribution() : Base() {}
    
    template <typename InputIt>
    discrete_distribution(InputIt first, InputIt last) : Base(first, last) {}

    discrete_distribution(std::initializer_list<double> weights) : Base(weights) {}

    template <typename UnaryOperation>
    discrete_distribution(std::size_t count, double xmin, double xmax, UnaryOperation unary_op) :
        Base(count, xmin, xmax, unary_op) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
class piecewise_constant_distribution : public std::piecewise_constant_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::piecewise_constant_distribution<Rep>;

    template <typename InputIt>
    inline static std::vector<Rep> i_qty_to_rep(InputIt first, InputIt last)
    {
        std::vector<Rep> intervals_rep;
        intervals_rep.reserve(static_cast<size_t>(std::distance(first, last)));
        for (auto itr = first; itr != last; ++itr) { intervals_rep.push_back(itr->count()); }
        return intervals_rep;
    }
    
    inline static std::vector<Rep> bl_qty_to_rep(std::initializer_list<Quantity>& bl)
    {
        std::vector<Rep> bl_rep;
        bl_rep.reserve(bl.size());
        for (const Quantity& qty : bl) { bl_rep.push_back(qty.count()); }
        return bl_rep;
    }

    template <typename UnaryOperation>
    inline static std::vector<Rep> fw_bl(std::initializer_list<Quantity>& bl, UnaryOperation fw)
    {
        std::vector<Rep> w_bl;
        w_bl.reserve(bl.size());
        for (const Quantity& qty : bl) { w_bl.push_back(fw(qty)); }   
        std::vector<Rep> weights;
        weights.reserve(bl.size());
        for (size_t i = 0; i < bl.size() - 1; ++i) { weights.push_back(w_bl[i] + w_bl[i + 1]); }
        weights.push_back(0);
        return weights;
    }
    
    template <typename InputIt>
    piecewise_constant_distribution(const std::vector<Rep>& i, InputIt first_w) :
        Base(i.cbegin(), i.cend(), first_w) {}
    
    piecewise_constant_distribution(const std::vector<Rep>& bl, const std::vector<Rep>& weights) :
        Base(bl.cbegin(), bl.cend(), weights.cbegin()) {}

public:
    piecewise_constant_distribution() : Base() {}
    
    template <typename InputIt1, typename InputIt2>
    piecewise_constant_distribution(InputIt1 first_i, InputIt1 last_i, InputIt2 first_w) :
        piecewise_constant_distribution(i_qty_to_rep(first_i, last_i), first_w) {}

    template <typename UnaryOperation>
    piecewise_constant_distribution(std::initializer_list<Quantity> bl, UnaryOperation fw) :
        piecewise_constant_distribution(bl_qty_to_rep(bl), fw_bl(bl, fw)) {}

    template <typename UnaryOperation>
    piecewise_constant_distribution(std::size_t nw, Quantity xmin, Quantity xmax, UnaryOperation fw) :
        Base(nw, xmin.count(), xmax.count(), [fw](Rep val) { return fw(Quantity(val)); }) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }

    std::vector<Quantity> intervals() const
    {
        std::vector<Rep> intervals_rep = Base::intervals();
        std::vector<Quantity> intervals_qty;
        intervals_qty.reserve(intervals_rep.size());
        for (const Rep& val : intervals_rep) { intervals_qty.push_back(Quantity(val)); }
        return intervals_qty;
    }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

template<typename Quantity>
class piecewise_linear_distribution : public std::piecewise_linear_distribution<typename Quantity::rep>
{
    using Rep = Quantity::rep;
    using Base = std::piecewise_linear_distribution<Rep>;

    template <typename InputIt>
    inline static std::vector<Rep> i_qty_to_rep(InputIt first, InputIt last)
    {
        std::vector<Rep> intervals_rep;
        intervals_rep.reserve(static_cast<size_t>(std::distance(first, last)));
        for (auto itr = first; itr != last; ++itr) { intervals_rep.push_back(itr->count()); }
        return intervals_rep;
    }
    
    inline static std::vector<Rep> bl_qty_to_rep(std::initializer_list<Quantity>& bl)
    {
        std::vector<Rep> bl_rep;
        bl_rep.reserve(bl.size());
        for (const Quantity& qty : bl) { bl_rep.push_back(qty.count()); }
        return bl_rep;
    }

    template <typename UnaryOperation>
    inline static std::vector<Rep> fw_bl(std::initializer_list<Quantity>& bl, UnaryOperation fw)
    {
        std::vector<Rep> weights;
        weights.reserve(bl.size());
        for (const Quantity& qty : bl) { weights.push_back(fw(qty)); }   
        return weights;
    }
    
    template <typename InputIt>
    piecewise_linear_distribution(const std::vector<Rep>& i, InputIt first_w) :
        Base(i.cbegin(), i.cend(), first_w) {}
    
    piecewise_linear_distribution(const std::vector<Rep>& bl, const std::vector<Rep>& weights) :
        Base(bl.cbegin(), bl.cend(), weights.cbegin()) {}

public:
    piecewise_linear_distribution() : Base() {}
    
    template <typename InputIt1, typename InputIt2>
    piecewise_linear_distribution(InputIt1 first_i, InputIt1 last_i, InputIt2 first_w) :
        piecewise_linear_distribution(i_qty_to_rep(first_i, last_i), first_w) {}

    template <typename UnaryOperation>
    piecewise_linear_distribution(std::initializer_list<Quantity> bl, UnaryOperation fw) :
        piecewise_linear_distribution(bl_qty_to_rep(bl), fw_bl(bl, fw)) {}

    template <typename UnaryOperation>
    piecewise_linear_distribution(std::size_t nw, Quantity xmin, Quantity xmax, UnaryOperation fw) :
        Base(nw, xmin.count(), xmax.count(), [fw](Rep val) { return fw(Quantity(val)); }) {}
    
    template<typename Generator>
    Quantity operator()(Generator& g) { return Quantity(Base::operator()(g)); }

    std::vector<Quantity> intervals() const
    {
        std::vector<Rep> intervals_rep = Base::intervals();
        std::vector<Quantity> intervals_qty;
        intervals_qty.reserve(intervals_rep.size());
        for (const Rep& val : intervals_rep) { intervals_qty.push_back(Quantity(val)); }
        return intervals_qty;
    }
    
    Quantity min() const { return Quantity(Base::min()); }
    Quantity max() const { return Quantity(Base::max()); }
};

} // namespace units