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

// Opt-in integration that lets GLM vectors be used directly as mp-units `quantity`
// representation types.
//
// GLM evaluates eagerly (its operators return concrete `glm::vec`, not expression templates), so
// no `representation_canonical_type` specialization is needed, and `glm::vec` already exposes a
// `value_type` member that `representation_underlying_type` detects automatically. Two things are
// missing. First, the Euclidean magnitude: GLM spells it `glm::length()` rather than `norm()`, so
// this header adds a `magnitude()` overload (found by ADL) that the library's `magnitude()` CPO
// picks up. Second, the tuple protocol: unlike `Eigen` (whose fixed-size vectors get it from the
// mp-units plugin) and `blaze::StaticVector` (which ships its own), a `glm::vec` states its length
// only through the static `length()`, so this header maps it onto `std::tuple_size` and adds the
// matching `std::tuple_element` and `get<I>`. That makes a GLM vector structured-bindings friendly
// and lets a vector quantity backed by one decompose into its components.
//
// The whole header is inert unless GLM is actually available, so it is always safe to include.
//
// The header is dual-mode: included textually for header-mode consumers, and pulled into the
// `mp_units.integrations.glm` module interface unit otherwise. The `magnitude()` overload is
// exported so that the `magnitude()` CPO can find it by ADL across the module boundary; in module
// mode the GLM headers live in the module's global module fragment, so they are not included here.

#if __has_include(<glm/geometric.hpp>)

#include <mp-units/bits/module_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
// The GLM headers come first and the `import std;` last: GLM includes `<cmath>` and friends
// textually, and libstdc++ requires every textual standard-library include to precede the import.
#include <glm/geometric.hpp>  // glm::length
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <tuple>
#include <type_traits>
#endif
#endif

namespace glm {

MP_UNITS_EXPORT template<length_t L, typename T, qualifier Q>
[[nodiscard]] constexpr T magnitude(const vec<L, T, Q>& v)
{
  return length(v);
}

MP_UNITS_EXPORT template<std::size_t Idx, length_t L, typename T, qualifier Q>
  requires(Idx < static_cast<std::size_t>(L))
[[nodiscard]] constexpr decltype(auto) get(vec<L, T, Q>&& v)
{
  return std::move(v)[static_cast<length_t>(Idx)];
}

MP_UNITS_EXPORT template<std::size_t Idx, length_t L, typename T, qualifier Q>
  requires(Idx < static_cast<std::size_t>(L))
[[nodiscard]] constexpr decltype(auto) get(vec<L, T, Q>& v)
{
  return v[static_cast<length_t>(Idx)];
}

MP_UNITS_EXPORT template<std::size_t Idx, length_t L, typename T, qualifier Q>
  requires(Idx < static_cast<std::size_t>(L))
[[nodiscard]] constexpr decltype(auto) get(const vec<L, T, Q>& v)
{
  return v[static_cast<length_t>(Idx)];
}

}  // namespace glm

template<glm::length_t L, typename T, glm::qualifier Q>
struct std::tuple_size<glm::vec<L, T, Q>> : std::integral_constant<std::size_t, static_cast<std::size_t>(L)> {};

template<std::size_t Idx, glm::length_t L, typename T, glm::qualifier Q>
  requires(Idx < static_cast<std::size_t>(L))
struct std::tuple_element<Idx, glm::vec<L, T, Q>> {
  using type = std::remove_cv_t<T>;
};

#endif  // __has_include(<glm/geometric.hpp>)
