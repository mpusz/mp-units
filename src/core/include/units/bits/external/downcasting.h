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

#ifdef UNITS_DOWNCAST_MODE
#if UNITS_DOWNCAST_MODE < 0 || UNITS_DOWNCAST_MODE > 2
#error "Invalid UNITS_DOWNCAST_MODE value"
#endif
#else
#define UNITS_DOWNCAST_MODE 1
#endif

namespace units {

template<typename BaseType>
struct downcast_base {
  using downcast_base_type = BaseType;
  UNITS_DIAGNOSTIC_PUSH
  UNITS_DIAGNOSTIC_IGNORE_NON_TEMPLATE_FRIEND
  friend auto downcast_guide(downcast_base);
  friend auto downcast_poison_pill(downcast_base);
  UNITS_DIAGNOSTIC_POP
};

template<typename T>
concept Downcastable =
  requires { typename T::downcast_base_type; } && std::derived_from<T, downcast_base<typename T::downcast_base_type>>;

template<typename T>
concept has_downcast_guide = requires(T t) { downcast_guide(t); };

template<typename T>
concept has_downcast_poison_pill = requires(T t) { downcast_poison_pill(t); };

template<typename Target, Downcastable T>
struct downcast_child : T {
  friend auto downcast_guide(typename T::downcast_base) { return std::type_identity<Target>(); }
};

template<Downcastable T>
struct downcast_poison : T {
  friend auto downcast_poison_pill(typename T::downcast_base) { return true; }
};

enum class downcast_mode {
  off = 0,       // no downcasting at all
  on = 1,        // downcasting always forced -> compile-time errors in case of duplicated definitions
  automatic = 2  // downcasting automatically enabled if no collisions are present
};


template<typename Target, Downcastable T, downcast_mode mode = static_cast<downcast_mode>(UNITS_DOWNCAST_MODE)>
struct downcast_dispatch :
    std::conditional_t<mode == downcast_mode::off, T,
#ifdef UNITS_COMP_MSVC
                       downcast_child<Target, T>> {
};
#else
                       std::conditional_t<mode == downcast_mode::automatic && has_downcast_guide<T>, downcast_poison<T>,
                                          downcast_child<Target, T>>> {
};
#endif

namespace detail {

template<typename T>
constexpr auto downcast_impl()
{
  if constexpr (has_downcast_guide<downcast_base<T>> && !has_downcast_poison_pill<downcast_base<T>>)
    return decltype(downcast_guide(std::declval<downcast_base<T>>()))();
  else
    return std::type_identity<T>();
}

}  // namespace detail

template<Downcastable T>
using downcast = TYPENAME decltype(detail::downcast_impl<T>())::type;

template<Downcastable T>
using downcast_base_t = TYPENAME T::downcast_base_type;

}  // namespace units
