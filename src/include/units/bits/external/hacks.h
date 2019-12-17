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

#include <functional>

#ifdef NDEBUG
#define Expects(cond) (void)(cond);
#else
#include <cassert>
#define Expects(cond) assert(cond);
#endif

#if __GNUC__ < 10

#include <concepts/concepts.hpp>
#define AUTO
#define SAME_AS(T) T

#else

#include <concepts>
#define AUTO auto
#define SAME_AS(T) std::same_as<T>

#endif

namespace std {

#if __GNUC__ < 10

  // concepts
  using concepts::common_reference_with;
  using concepts::common_with;
  using concepts::constructible_from;
  using concepts::convertible_to;
  using concepts::default_constructible;
  using concepts::derived_from;
  using concepts::equality_comparable_with;
  using concepts::regular;
  using concepts::same_as;
  using concepts::totally_ordered;
  using concepts::totally_ordered_with;

  template<class F, class... Args>
  concept invocable =
  requires(F&& f, Args&&... args) {
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  };

  template<class F, class... Args>
  concept regular_invocable = invocable<F, Args...>;

#else

template<class T>
concept default_constructible = constructible_from<T>;

#endif

} // namespace std
