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

#include <units/bits/hacks.h>

// P1813 - A Concept Design for the Numeric Algorithms

namespace std {

  template<class BOp, class T, class U = T>
  struct left_identity {};

  template<class BOp, class T, class U = T>
  using left_identity_t = decltype(declval<left_identity<BOp, T, U>>()());

  template<class BOp, class T, class U = T>
  struct right_identity {};

  template<class BOp, class T, class U = T>
  using right_identity_t = decltype(declval<right_identity<BOp, T, U>>()());

  namespace detail {

    template<class BOp, class T, class U>
    concept has_two_sided_identity = // exposition only
      requires(BOp bop, const T& t, const U& u) {
        typename left_identity_t<BOp, T, U>;
        typename left_identity_t<BOp, U, T>;
        typename right_identity_t<BOp, T, U>;
        typename right_identity_t<BOp, U, T>;
        requires same_as<left_identity_t<BOp, T, U>, left_identity_t<BOp, U, T>>;
        requires same_as<right_identity_t<BOp, T, U>, right_identity_t<BOp, U, T>>;
        requires same_as<left_identity_t<BOp, T, U>, right_identity_t<BOp, T, U>>;
      };

  }

  template<class BOp, class T, class U = T>
    requires detail::has_two_sided_identity<BOp, T, U>
  struct two_sided_identity {
    constexpr common_type_t<T, U> operator()() const;
  };

  template<class BOp, class T, class U>
  using two_sided_identity_t = decltype(two_sided_identity<BOp&, T, U>());

  template<class BOp, class T, class U = T>
  struct left_zero {};

  template<class BOp, class T, class U = T>
  using left_zero_t = decltype(declval<left_zero<BOp, T, U>>()());

  template<class BOp, class T, class U = T>
  struct right_zero {};

  template<class BOp, class T, class U = T>
  using right_zero_t = decltype(declval<right_zero<BOp, T, U>>()());

  namespace detail {

    template<class BOp, class T, class U>
    concept has_two_sided_zero = // exposition only
      requires(BOp bop, const T& t, const U& u) {
        typename left_zero_t<BOp, T, U>;
        typename left_zero_t<BOp, U, T>;
        typename right_zero_t<BOp, T, U>;
        typename right_zero_t<BOp, U, T>;
        requires same_as<left_zero_t<BOp, T, U>, left_zero_t<BOp, U, T>>;
        requires same_as<right_zero_t<BOp, T, U>, right_zero_t<BOp, U, T>>;
        requires same_as<left_zero_t<BOp, T, U>, right_zero_t<BOp, T, U>>;
      };

  }

  template<class BOp, class T, class U>
    requires detail::has_two_sided_zero<BOp, T, U>
  struct two_sided_zero {
    constexpr common_type_t<T, U> operator()(BOp bop, T&& t, U&& u) const;
  };

  template<class BOp, class T, class U = T>
  using two_sided_zero_t = decltype(two_sided_zero<BOp&, T, U>());

  template<class BOp>
  struct inverse_traits {};

  template<class BOp>
  using inverse_operation_t = typename inverse_traits<BOp>::type;

  template<class BOp, class T, class U>
  concept commutative_operation =
    regular_invocable<BOp, T, U> &&
    regular_invocable<BOp, U, T> &&
    common_with<T, U> &&
    equality_comparable_with<T, U>;

  template<class BOp, class T, class U>
  concept magma =
    common_with<T, U> &&
    regular_invocable<BOp, T, T> &&
    regular_invocable<BOp, U, U> &&
    regular_invocable<BOp, T, U> &&
    regular_invocable<BOp, U, T> &&
    common_with<invoke_result_t<BOp&, T, U>, T> &&
    common_with<invoke_result_t<BOp&, T, U>, U> &&
    same_as<invoke_result_t<BOp&, T, U>, invoke_result_t<BOp&, U, T>>;

  template<class BOp, class T, class U>
  concept semigroup = magma<BOp, T, U>;

  template<class BOp, class T, class U = T>
  concept monoid = semigroup<BOp, T, U> &&
    requires {
      typename two_sided_identity_t<BOp, remove_cvref_t<T>, remove_cvref_t<U>>;
    };

  template<class BOp, class T, class U>
  concept quasigroup = magma<BOp, T, U> &&
    requires {
      typename inverse_operation_t<BOp>;
      typename inverse_operation_t<inverse_operation_t<BOp>>;
      requires same_as<BOp, inverse_operation_t<inverse_operation_t<BOp>>>;
    };

  template<class BOp, class T, class U>
  concept loop = quasigroup<BOp, T, U> &&
    requires {
      typename two_sided_identity_t<BOp, remove_cvref_t<T>, remove_cvref_t<U>>;
    };

  template<class BOp, class T, class U>
  concept group = semigroup<BOp, T, U> && quasigroup<BOp, T, U>;

  template<class BOp, class T, class U>
  concept abelian_group = group<BOp, T, U> && commutative_operation<BOp, T, U>;

  template<class BOp1, class BOp2, class T, class U, class V>
  concept weak_magmaring = magma<BOp2, U, V> && magma<BOp1, T, invoke_result_t<BOp2&, U, V>>;

  template<class BOp1, class BOp2, class T, class U, class V>
  concept near_semiring = weak_magmaring<BOp1, BOp2, T, U, V> &&
    monoid<BOp1, T, invoke_result_t<BOp2&, U, V>> && semigroup<BOp2, U, V> &&
    requires {
      typename two_sided_zero_t<BOp2, remove_cvref_t<U>, remove_cvref_t<V>>;
    };


  namespace ranges {

    namespace detail {

      template<class T, class U>
      concept summable_with = // exposition only
        default_constructible<remove_reference_t<T>> &&
        default_constructible<remove_reference_t<U>> &&
        common_reference_with<T, U> &&
        requires(T&& t, U&& u) {
          { std::forward<T>(t) + std::forward<T>(t) } -> common_with<T>;
          { std::forward<U>(u) + std::forward<U>(u) } -> common_with<U>;
          { std::forward<T>(t) + std::forward<U>(u) } -> common_with<T>;
          { std::forward<U>(u) + std::forward<T>(t) } -> common_with<U>;
          requires same_as<decltype(std::forward<T>(t) + std::forward<U>(u)),
                           decltype(std::forward<U>(u) + std::forward<T>(t))>;
        };

    }

    struct plus {
      template<class T, detail::summable_with<T> U>
      constexpr decltype(auto) operator()(T&& t, U&& u) const {
        return std::forward<T>(t) + std::forward<U>(u);
      }

      using is_transparent = std::true_type;
    };

    namespace detail {

      template<class T>
      concept negatable = // exposition only
        summable_with<T, T> &&
        totally_ordered<T> &&
        requires(T&& t) {
          { -std::forward<T>(t) } -> common_with<T>;
        };

    }

    struct negate {
      template<detail::negatable T>
      constexpr decltype(auto) operator()(T&& t) const {
        return -std::forward<T>(t);
      }
      
      using is_transparent = std::true_type;
    };

    namespace detail {

      template<class T, class U>
      concept differenceable_with = // exposition only
        summable_with<T, U> &&
        negatable<T> &&
        negatable<U> &&
        totally_ordered_with<T, U> &&
        requires(T&& t, U&& u) {
          { std::forward<T>(t) - std::forward<T>(t) } -> common_with<T>;
          { std::forward<U>(u) - std::forward<U>(u) } -> common_with<U>;
          { std::forward<T>(t) - std::forward<U>(u) } -> common_with<T>;
          { std::forward<U>(u) - std::forward<T>(t) } -> common_with<U>;
          requires same_as<decltype(std::forward<T>(t) - std::forward<U>(u)),
                           decltype(std::forward<U>(u) - std::forward<T>(t))>;
        };

    }

    struct minus {
      template<class T, detail::differenceable_with<T> U>
      constexpr decltype(auto) operator()(T&& t, U&& u) const {
        return std::forward<T>(t) - std::forward<U>(u);
      }

      using is_transparent = std::true_type;
    };

    namespace detail {

      template<class T, class U>
      concept multiplicable_with = // exposition only
        detail::summable_with<T, U> &&
        constructible_from<remove_cvref_t<T>, int> && // specifically T{0} and T{1}
        constructible_from<remove_cvref_t<U>, int> && // specifically U{0} and U{1}
        constructible_from<remove_cvref_t<common_type_t<T, U>>, int> &&
        common_reference_with<T, U> &&
        requires(T&& t, U&& u) {
          { std::forward<T>(t) * std::forward<T>(t) } -> common_with<T>;
          { std::forward<U>(u) * std::forward<U>(u) } -> common_with<U>;
          { std::forward<T>(t) * std::forward<U>(u) } -> common_with<T>;
          { std::forward<U>(u) * std::forward<T>(t) } -> common_with<U>;
          requires same_as<decltype(std::forward<T>(t) * std::forward<U>(u)),
                           decltype(std::forward<U>(u) * std::forward<T>(t))>;
        };
  
    }

    struct times {
      template<class T, detail::multiplicable_with<T> U>
      constexpr decltype(auto) operator()(T&& t, U&& u) const
      { return std::forward<T>(t) * std::forward<U>(u); }

      using is_transparent = std::true_type;
    };

    namespace detail {

      template<class T, class U>
      concept divisible_with = // exposition only
        multiplicable_with<T, U> &&
        differenceable_with<T, U> &&
        requires(T&& t, U&& u) {
          { std::forward<T>(t) / std::forward<T>(t) } -> common_with<T>;
          { std::forward<U>(u) / std::forward<U>(u) } -> common_with<U>;
          { std::forward<T>(t) / std::forward<U>(u) } -> common_with<T>;
          { std::forward<U>(u) / std::forward<T>(t) } -> common_with<U>;
          requires same_as<decltype(std::forward<T>(t) / std::forward<U>(u)),
                           decltype(std::forward<U>(u) / std::forward<T>(t))>;
        };

    }

    struct divided_by {
      template<class T, detail::divisible_with<T> U>
      constexpr decltype(auto) operator()(T&& t, U&& u) const
      { return std::forward<T>(t) / std::forward<U>(u); }
    };

    namespace detail {

      template<class T, class Q>
      concept modulo_with = // exposition only
        divisible_with<T, Q> &&
        requires(T&& t, Q&& q) {
          { std::forward<T>(t) % std::forward<T>(t) } -> common_with<T>;
          { std::forward<Q>(q) % std::forward<Q>(q) } -> common_with<Q>;
          { std::forward<T>(t) % std::forward<Q>(q) } -> common_with<T>;
          { std::forward<Q>(q) % std::forward<T>(t) } -> common_with<Q>;
          requires same_as<decltype(std::forward<T>(t) % std::forward<Q>(q)),
                           decltype(std::forward<Q>(q) % std::forward<T>(t))>;
        };

    }

    struct modulus {
      template<class T, detail::modulo_with<T> U>
      constexpr decltype(auto) operator()(T&& t, U&& u) const
      { return std::forward<T>(t) % std::forward<U>(u); }
    };

  } // namespace ranges

  template<class T, class U>
    requires magma<ranges::plus, T, U>
  struct left_identity<ranges::plus, T, U> {
    constexpr common_type_t<T, U> operator()() const { return T{}; }
  };

  template<class T, class U>
    requires magma<ranges::plus, T, U>
  struct right_identity<ranges::plus, T, U> {
    constexpr common_type_t<T, U> operator()() const { return U{}; }
  };

  template<>
  struct inverse_traits<ranges::plus> {
    using type = ranges::minus;
    constexpr type operator()() const noexcept { return type{}; }
  };

  template<class T, class U>
  struct right_identity<ranges::minus, T, U> : private right_identity<ranges::plus, T, U> {
    using right_identity<ranges::plus, T, U>::operator();
  };

  template<>
  struct inverse_traits<ranges::minus> {
    using type = ranges::plus;
    constexpr type operator()() const noexcept { return type{}; }
  };

  template<class T, class U>
    requires magma<ranges::times, T, U>
  struct left_identity<ranges::times, T, U> {
    constexpr common_type_t<T, U> operator()() const { return T{1}; }
  };

  template<class T, class U>
    requires magma<ranges::times, T, U>
  struct right_identity<ranges::times, T, U> {
    constexpr common_type_t<T, U> operator()() const { return U{1}; }
  };

  template<class T, class U>
    requires magma<ranges::times, T, U>
  struct left_zero<ranges::times, T, U> {
    constexpr common_type_t<T, U> operator()() const { return T{}; }
  };

  template<class T, class U>
    requires magma<ranges::times, T, U>
  struct right_zero<ranges::times, T, U> {
    constexpr common_type_t<T, U> operator()() const { return U{}; }
  };

  template<>
  struct inverse_traits<ranges::times> {
    using type = ranges::divided_by;
    constexpr type operator()() const noexcept { return type{}; }
  };

  template<class T, class U>
    requires magma<ranges::divided_by, T, U>
  struct right_identity<ranges::divided_by, T, U> {
    constexpr common_type_t<T, U> operator()() const { return U{1}; }
  };

  template<>
  struct inverse_traits<ranges::divided_by> {
    using type = ranges::times;
    constexpr type operator()() const noexcept { return type{}; }
  };

  template<class T, class U>
    requires magma<ranges::modulus, T, U>
  struct left_zero<ranges::modulus, T, U> {
    constexpr common_type_t<T, U> operator()() const { return T{}; }
  };

}  // namespace std
