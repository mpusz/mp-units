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

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#endif

namespace mp_units::utility {

/// @brief Sentinel default for a specializable variable-template customization point with no usable
/// default.
///
/// A variable template cannot be `= delete`d the way a function can, so a customization point that
/// must be specialized before use defaults to `unspecified`, and a specialization replaces it with a
/// real value. `specified` reports whether that has happened. This is public authoring vocabulary
/// shared by the library's own customization points and by higher-level tools, hence it lives in the
/// `mp_units::utility` namespace.
MP_UNITS_EXPORT struct unspecified_t {};
MP_UNITS_EXPORT inline constexpr unspecified_t unspecified{};

/// @brief Satisfied when a customization point has been given a value (i.e. is not `unspecified`).
MP_UNITS_EXPORT template<typename T>
concept specified = !std::same_as<std::remove_cvref_t<T>, unspecified_t>;

}  // namespace mp_units::utility
