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

// IWYU pragma: always_keep

#pragma once

#include <mp-units/bits/requires_hosted.h>
//
#include <mp-units/bits/fmt.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>

namespace mp_units::detail {

template<std::forward_iterator It>
[[nodiscard]] constexpr It at_most_one_of(It begin, It end, std::string_view modifiers)
{
  const It it = mp_units::detail::find_first_of(begin, end, modifiers.begin(), modifiers.end());
  if (it != end && mp_units::detail::find_first_of(it + 1, end, modifiers.begin(), modifiers.end()) != end)
    throw MP_UNITS_STD_FMT::format_error("only one of '" + std::string(modifiers) +
                                         "' unit modifiers may be used in the format spec");
  return it;
}

// TODO the below should be exposed by the C++ Standard Library (used in our examples)
MP_UNITS_EXPORT_BEGIN

template<typename Char>
struct fill_align_width_format_specs {
  fill_t<Char> fill;
  fmt_align align : 4 = fmt_align::none;
  int width = 0;
  fmt_arg_ref<Char> width_ref;
};

template<std::forward_iterator It, typename Specs>
[[nodiscard]] constexpr It parse_fill_align_width(
  MP_UNITS_STD_FMT::basic_format_parse_context<std::iter_value_t<It>>& ctx, It begin, It end, Specs& specs,
  fmt_align default_align = fmt_align::none)
{
  auto it = begin;
  if (it == end || *it == '}') return it;

  it = mp_units::detail::parse_align(it, end, specs, default_align);
  if (it == end) return it;

  return mp_units::detail::parse_dynamic_spec(it, end, specs.width, specs.width_ref, ctx);
}

/**
 * @brief Writes a string to an output iterator with fill/align/width padding.
 *
 * Avoids instantiating format_to/vformat_to infrastructure for the common padding use-case
 * inside formatter<Unit>, formatter<Dimension>, and formatter<quantity> specializations.
 */
template<typename Char, std::output_iterator<Char> Out>
constexpr Out write_padded(Out out, std::basic_string_view<Char> s, int width, fmt_align align,
                           const fill_t<Char>& fill)
{
  const int len = static_cast<int>(s.size());
  const int pad = (width > len) ? width - len : 0;
  const int lpad = (align == fmt_align::center) ? pad / 2 : (align == fmt_align::right) ? pad : 0;
  const int rpad = pad - lpad;
  auto write_fill = [&](int n) {
    for (int i = 0; i < n; ++i)
      for (std::size_t j = 0; j < fill.size(); ++j) *out++ = fill[j];
  };
  write_fill(lpad);
  MP_UNITS_DIAGNOSTIC_PUSH
  // Loop variable shadows a `si::unit_symbols` short name when `using namespace si::unit_symbols`
  // is in scope at the call site (MSVC C4459); the local variable here is the documented
  // canonical name for this trivial loop.
  MP_UNITS_DIAGNOSTIC_IGNORE_SHADOW
  for (Char c : s) *out++ = c;
  MP_UNITS_DIAGNOSTIC_POP
  write_fill(rpad);
  return out;
}

MP_UNITS_EXPORT_END

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=120625
template<typename T>
inline constexpr bool GCC_120625_is_complete = requires { sizeof(T) > 0; };

}  // namespace mp_units::detail
