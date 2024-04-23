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

// Formatting library for C++ - the core API for char/UTF-8
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#pragma once

#include <mp-units/compat_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <gsl/gsl-lite.hpp>
#include <concepts>
#include <limits>
#include <string_view>
#endif

// most of the below code is based on/copied from fmtlib

namespace mp_units::detail {

enum class fmt_align { none, left, right, center, numeric };
enum class fmt_arg_id_kind {
  none,
#if MP_UNITS_USE_FMTLIB
  name,
#endif
  index
};

template<typename Char>
struct fmt_arg_ref {
  fmt_arg_id_kind kind = fmt_arg_id_kind::none;
  union value {
    int index = 0;
#if MP_UNITS_USE_FMTLIB
    std::basic_string_view<Char> name;
#endif

    constexpr value() {}
    constexpr value(int idx) : index(idx) {}
#if MP_UNITS_USE_FMTLIB
    constexpr value(std::basic_string_view<Char> n) : name(n) {}
#endif
  } val{};

  fmt_arg_ref() = default;
  constexpr explicit fmt_arg_ref(int index) : kind(fmt_arg_id_kind::index), val(index) {}
#if MP_UNITS_USE_FMTLIB
  constexpr explicit fmt_arg_ref(std::basic_string_view<Char> name) : kind(fmt_arg_id_kind::name), val(name) {}
#endif

  [[nodiscard]] constexpr fmt_arg_ref& operator=(int idx)
  {
    kind = fmt_arg_id_kind::index;
    val.index = idx;
    return *this;
  }
};

template<typename Char>
struct fill_t {
private:
  static constexpr size_t max_size = 4 / sizeof(Char);
  // At most one codepoint (so one char32_t or four utf-8 char8_t)
  Char data_[max_size] = {Char{' '}};
  unsigned char size_ = 1;

public:
  constexpr fill_t& operator=(std::basic_string_view<Char> str)
  {
    auto size = str.size();
    if (size > max_size) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid fill"));
    for (size_t i = 0; i < size && i < max_size; ++i) data_[i] = str[i];
    size_ = static_cast<unsigned char>(size);
    return *this;
  }

  [[nodiscard]] constexpr size_t size() const { return size_; }
  [[nodiscard]] constexpr const Char* data() const { return data_; }

  [[nodiscard]] constexpr Char& operator[](size_t index) { return data_[index]; }
  [[nodiscard]] constexpr const Char& operator[](size_t index) const { return data_[index]; }
};

template<typename T>
inline constexpr bool is_integer = std::is_integral<T>::value && !std::is_same<T, bool>::value &&
                                   !std::is_same<T, char>::value && !std::is_same<T, wchar_t>::value;

// Converts a character to ASCII. Returns a number > 127 on conversion failure.
template<std::integral Char>
[[nodiscard]] constexpr Char to_ascii(Char value)
{
  return value;
}

template<typename Char>
  requires std::is_enum_v<Char>
[[nodiscard]] constexpr std::underlying_type_t<Char> to_ascii(Char value)
{
  return value;
}

// Casts a nonnegative integer to unsigned.
template<typename Int>
[[nodiscard]] constexpr std::make_unsigned_t<Int> to_unsigned(Int value)
{
  gsl_Expects(std::is_unsigned_v<Int> || value >= 0);
  return static_cast<std::make_unsigned_t<Int>>(value);
}

struct width_checker {
  template<typename T>
  [[nodiscard]] constexpr unsigned long long operator()(T value) const
  {
    if constexpr (is_integer<T>) {
      if constexpr (std::numeric_limits<T>::is_signed)
        if (value < 0) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("negative width"));
      return static_cast<unsigned long long>(value);
    }
    MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("width is not integer"));
    return 0;
  }
};

template<class Handler, typename FormatArg>
[[nodiscard]] constexpr int get_dynamic_spec(FormatArg arg)
{
  const unsigned long long value = MP_UNITS_STD_FMT::visit_format_arg(Handler{}, arg);
  if (value > ::mp_units::detail::to_unsigned(std::numeric_limits<int>::max())) {
    MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("number is too big"));
  }
  return static_cast<int>(value);
}

template<typename Context, typename ID>
[[nodiscard]] constexpr auto get_arg(Context& ctx, ID id) -> decltype(ctx.arg(id))
{
  auto arg = ctx.arg(MP_UNITS_FMT_TO_ARG_ID(id));
  if (!arg) MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("argument not found"));
  return arg;
}

template<class Handler, typename Context>
constexpr void handle_dynamic_spec(int& value, fmt_arg_ref<typename Context::char_type> ref, Context& ctx)
{
  switch (ref.kind) {
    case fmt_arg_id_kind::none:
      break;
    case fmt_arg_id_kind::index:
      value = ::mp_units::detail::get_dynamic_spec<Handler>(get_arg(ctx, ref.val.index));
      break;
#if MP_UNITS_USE_FMTLIB
    case fmt_arg_id_kind::name:
      value = ::mp_units::detail::get_dynamic_spec<Handler>(get_arg(ctx, ref.val.name));
      break;
#endif
  }
}

// Parses the range [begin, end) as an unsigned integer. This function assumes
// that the range is non-empty and the first character is a digit.
template<typename Char>
[[nodiscard]] constexpr int parse_nonnegative_int(const Char*& begin, const Char* end, int error_value)
{
  gsl_Expects(begin != end && '0' <= *begin && *begin <= '9');
  unsigned value = 0, prev = 0;
  auto p = begin;
  do {
    prev = value;
    value = value * 10 + unsigned(*p - '0');
    ++p;
  } while (p != end && '0' <= *p && *p <= '9');
  auto num_digits = p - begin;
  begin = p;
  if (num_digits <= std::numeric_limits<int>::digits10) return static_cast<int>(value);
  // Check for overflow.
  const unsigned max = ::mp_units::detail::to_unsigned((std::numeric_limits<int>::max)());
  return num_digits == std::numeric_limits<int>::digits10 + 1 && prev * 10ull + unsigned(p[-1] - '0') <= max
           ? static_cast<int>(value)
           : error_value;
}

template<typename Char>
[[nodiscard]] constexpr bool is_name_start(Char c)
{
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

template<typename Char, typename Handler>
[[nodiscard]] constexpr const Char* do_parse_arg_id(const Char* begin, const Char* end, Handler&& handler)
{
  Char c = *begin;
  if (c >= '0' && c <= '9') {
    int index = 0;
    constexpr int max = (std::numeric_limits<int>::max)();
    if (c != '0')
      index = ::mp_units::detail::parse_nonnegative_int(begin, end, max);
    else
      ++begin;
    if (begin == end || (*begin != '}' && *begin != ':'))
      MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid format string"));
    else
      handler.on_index(index);
    return begin;
  }
  if (c == '%') return begin;  // mp-units extension
  if (!::mp_units::detail::is_name_start(c)) {
    MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid format string"));
    return begin;
  }
  auto it = begin;
  do {
    ++it;
  } while (it != end && (::mp_units::detail::is_name_start(*it) || ('0' <= *it && *it <= '9')));
#if MP_UNITS_USE_FMTLIB
  handler.on_name({begin, ::mp_units::detail::to_unsigned(it - begin)});
#else
  MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("named arguments are not supported in the C++ standard facilities"));
#endif

  return it;
}

template<typename Char, typename Handler>
[[nodiscard]] constexpr const Char* parse_arg_id(const Char* begin, const Char* end, Handler&& handler)
{
  gsl_Expects(begin != end);
  Char c = *begin;
  if (c != '}' && c != ':') return ::mp_units::detail::do_parse_arg_id(begin, end, handler);
  handler.on_auto();
  return begin;
}

template<typename Char>
struct dynamic_spec_id_handler {
  MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx;
  fmt_arg_ref<Char>& ref;

  constexpr void on_auto()
  {
    int id = MP_UNITS_FMT_FROM_ARG_ID(ctx.next_arg_id());
    ref = fmt_arg_ref<Char>(id);
#if MP_UNITS_USE_FMTLIB || __cpp_lib_format >= 202305L
    ctx.check_dynamic_spec(id);
#endif
  }
  constexpr void on_index(int id)
  {
    ref = fmt_arg_ref<Char>(id);
    ctx.check_arg_id(MP_UNITS_FMT_TO_ARG_ID(id));
#if MP_UNITS_USE_FMTLIB || __cpp_lib_format >= 202305L
    ctx.check_dynamic_spec(id);
#endif
  }
#if MP_UNITS_USE_FMTLIB
  constexpr void on_name([[maybe_unused]] std::basic_string_view<Char> id)
  {
    ref = fmt_arg_ref<Char>(id);
    ctx.check_arg_id(id);
  }
#endif
};

template<typename Char>
[[nodiscard]] constexpr const Char* parse_dynamic_spec(const Char* begin, const Char* end, int& value,
                                                       fmt_arg_ref<Char>& ref,
                                                       MP_UNITS_STD_FMT::basic_format_parse_context<Char>& ctx)
{
  gsl_Expects(begin != end);
  if ('0' <= *begin && *begin <= '9') {
    int val = ::mp_units::detail::parse_nonnegative_int(begin, end, -1);
    if (val != -1)
      value = val;
    else
      MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("number is too big"));
  } else if (*begin == '{') {
    ++begin;
    if (*begin == '%') return begin - 1;  // mp-units extension
    auto handler = dynamic_spec_id_handler<Char>{ctx, ref};
    if (begin != end) begin = ::mp_units::detail::parse_arg_id(begin, end, handler);
    if (begin != end && *begin == '}') return ++begin;
    MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid format string"));
  }
  return begin;
}

template<std::input_iterator It>
constexpr int code_point_length(It begin)
{
  if constexpr (sizeof(std::iter_value_t<It>) != 1) return 1;
  constexpr char lengths[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0};
  int len = lengths[static_cast<unsigned char>(*begin) >> 3];

  // Compute the pointer to the next character early so that the next
  // iteration can start working on the next character. Neither Clang
  // nor GCC figure out this reordering on their own.
  return len + !len;
}

// Parses fill and alignment.
template<typename Char, typename Specs>
[[nodiscard]] constexpr const Char* parse_align(const Char* begin, const Char* end, Specs& specs,
                                                fmt_align default_align = fmt_align::none)
{
  gsl_Expects(begin != end);
  auto align = fmt_align::none;
  auto p = begin + code_point_length(begin);
  if (end - p <= 0) p = begin;
  for (;;) {
    switch (to_ascii(*p)) {
      case '<':
        align = fmt_align::left;
        break;
      case '>':
        align = fmt_align::right;
        break;
      case '^':
        align = fmt_align::center;
        break;
    }
    if (align != fmt_align::none) {
      if (p != begin) {
        auto c = *begin;
        if (c == '}') return begin;
        if (c == '{') {
          MP_UNITS_THROW(MP_UNITS_STD_FMT::format_error("invalid fill character '{'"));
          return begin;
        }
        specs.fill = {begin, to_unsigned(p - begin)};
        begin = p + 1;
      } else {
        ++begin;
      }
      break;
    } else if (p == begin) {
      break;
    }
    p = begin;
  }
  if (align == fmt_align::none) align = default_align;  // mp-units extension
  specs.align = align;
  return begin;
}

}  // namespace mp_units::detail
