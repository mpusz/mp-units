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

#include <units/bits/external/hacks.h>
#include <type_traits>

namespace units {

template<typename BaseType>
struct downcast_base {
  using downcast_base_type = BaseType;
  friend auto downcast_guide(downcast_base);
};

template<typename T>
concept Downcastable =
  requires {
    typename T::downcast_base_type;
  } &&
  std::derived_from<T, downcast_base<typename T::downcast_base_type>>;

template<typename Target, Downcastable T>
struct downcast_child : T {
  friend auto downcast_guide(typename downcast_child::downcast_base /* base */) { return Target(); }
};

namespace detail {

template<typename T>
concept has_downcast =
  requires {
    downcast_guide(std::declval<downcast_base<T>>());
  };

template<typename T>
constexpr auto downcast_impl()
{
  if constexpr (has_downcast<T>)
    return decltype(downcast_guide(std::declval<downcast_base<T>>()))();
  else
    return T();
}

}  // namespace detail

template<Downcastable T>
using downcast = decltype(detail::downcast_impl<T>());

template<Downcastable T>
using downcast_base_t = T::downcast_base_type;

}  // namespace units
