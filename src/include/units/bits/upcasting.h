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
#include <type_traits>

namespace units {

  template<typename BaseType>
  struct upcast_base {
    using base_type = BaseType;
  };

  template<typename T>
  concept bool Upcastable =
      requires {
        typename T::base_type;
      } &&
      std::DerivedFrom<T, upcast_base<typename T::base_type>>;

  template<Upcastable T>
  using upcast_from = T::base_type;

  template<Upcastable T>
  using upcast_to = std::type_identity<T>;

  template<Upcastable T>
  struct upcasting_traits : upcast_to<T> {};

  template<Upcastable T>
  using upcasting_traits_t = upcasting_traits<T>::type;

}  // namespace units
