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

// Very simplified implementation of std::inplace_vector
// To be replaced with:
// P0843: std::inplace_vector

#include <mp-units/bits/hacks.h>  // IWYU pragma: keep

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstddef>
#include <ranges>
#endif
#endif

// NOLINTBEGIN(*-avoid-c-arrays)
#pragma once

namespace mp_units::detail {
template<class T, std::size_t N>
class inplace_vector {
public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr inplace_vector() noexcept = default;

  // iterators
  constexpr iterator begin() noexcept { return data(); }
  constexpr const_iterator begin() const noexcept { return data(); }
  constexpr iterator end() noexcept { return begin() + size(); }
  constexpr const_iterator end() const noexcept { return begin() + size(); }
  constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  constexpr const_iterator cbegin() const noexcept { return data(); }
  constexpr const_iterator cend() const noexcept { return cbegin() + size(); }
  constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
  constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; };
  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }
  [[nodiscard]] static constexpr size_type max_size() noexcept { return N; }
  [[nodiscard]] static constexpr size_type capacity() noexcept { return N; }

  // element access
  constexpr reference operator[](size_type n) { return data_[n]; }
  constexpr const_reference operator[](size_type n) const { return data_[n]; }
  constexpr reference front() { return *data(); }
  constexpr const_reference front() const { return *data(); }
  constexpr reference back() { return *(data() + size() - 1); }
  constexpr const_reference back() const { return *(data() + size() - 1); }

  // data access
  constexpr T* data() noexcept { return data_; }
  constexpr const T* data() const noexcept { return data_; }

  constexpr reference push_back(const T& v)
    requires std::constructible_from<T, const T&>
  {
    return emplace_back(v);
  }

  constexpr reference push_back(T&& v)
    requires std::constructible_from<T, T&&>
  {
    return emplace_back(std::forward<T&&>(v));
  }

  template<typename... Args>
    requires(std::constructible_from<T, Args...>)
  constexpr T& unchecked_emplace_back(Args&&... args)
  {
    std::construct_at(end(), std::forward<Args>(args)...);
    ++size_;
    return back();
  }

  template<typename... Args>
    requires(std::constructible_from<T, Args...>)
  constexpr T* try_emplace_back(Args&&... args)
  {
    if (size() == capacity()) return nullptr;
    return &unchecked_emplace_back(std::forward<Args>(args)...);
  }

  template<typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr reference emplace_back(Args&&... args)
  {
    MP_UNITS_EXPECTS(size() < capacity());
    auto ptr = try_emplace_back(std::forward<Args>(args)...);
#if MP_UNITS_HOSTED
    if (!ptr) throw std::runtime_error("not enough capacity");
#endif
    return *ptr;
  }

private:
  T data_[N] = {};
  size_type size_ = 0;
};

}  // namespace mp_units::detail

// NOLINTEND(*-avoid-c-arrays)
