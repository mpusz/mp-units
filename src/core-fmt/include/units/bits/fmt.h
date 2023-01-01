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

#include <gsl/gsl-lite.hpp>
#include <units/bits/fmt_hacks.h>
#include <concepts>
#include <limits>
#include <string_view>

// most of the below code is based on/copied from libfmt

namespace units::detail {

struct auto_id {};

enum class fmt_align { none, left, right, center };
enum class fmt_sign { none, minus, plus, space };
enum class arg_id_kind { none, index, name };

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
    if (size > max_size) UNITS_THROW(UNITS_STD_FMT::format_error("invalid fill"));
    for (size_t i = 0; i < size; ++i) data_[i] = str[i];
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

template<typename Char>
[[nodiscard]] constexpr bool is_ascii_letter(Char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Converts a character to ASCII. Returns a number > 127 on conversion failure.
template<std::integral Char>
[[nodiscard]] constexpr Char to_ascii(Char value)
{
  return value;
}

template<typename Char>
  requires std::is_enum_v<Char>
[[nodiscard]] constexpr auto to_ascii(Char value) -> std::underlying_type_t<Char>
{
  return value;
}

struct width_checker {
  template<typename T>
  [[nodiscard]] constexpr unsigned long long operator()(T value) const
  {
    if constexpr (is_integer<T>) {
      if constexpr (std::numeric_limits<T>::is_signed) {
        if (value < 0) UNITS_THROW(UNITS_STD_FMT::format_error("negative width"));
      }
      return static_cast<unsigned long long>(value);
    } else {
      UNITS_THROW(UNITS_STD_FMT::format_error("width is not integer"));
    }
  }
};

struct precision_checker {
  template<typename T>
  [[nodiscard]] constexpr unsigned long long operator()(T value) const
  {
    if constexpr (is_integer<T>) {
      if constexpr (std::numeric_limits<T>::is_signed) {
        if (value < 0) UNITS_THROW(UNITS_STD_FMT::format_error("negative precision"));
      }
      return static_cast<unsigned long long>(value);
    } else {
      UNITS_THROW(UNITS_STD_FMT::format_error("precision is not integer"));
    }
  }
};

// Format specifiers for built-in and string types.
template<typename Char>
struct basic_format_specs {
  int width = 0;
  int precision = -1;
  char type = '\0';
  fmt_align align : 4 = fmt_align::none;
  fmt_sign sign : 3 = fmt_sign::none;
  bool alt : 1 = false;  // Alternate form ('#').
  bool localized : 1 = false;
  fill_t<Char> fill;
};

// Format specifiers with width and precision resolved at formatting rather
// than parsing time to allow re-using the same parsed specifiers with
// different sets of arguments (precompilation of format strings).
template<typename Char>
struct dynamic_format_specs : basic_format_specs<Char> {
  int dynamic_width_index = -1;
  int dynamic_precision_index = -1;
};

[[nodiscard]] constexpr int verify_dynamic_arg_index_in_range(size_t idx)
{
  if (idx > static_cast<size_t>(std::numeric_limits<int>::max())) {
    UNITS_THROW(UNITS_STD_FMT::format_error("Dynamic width or precision index too large."));
  }
  return static_cast<int>(idx);
}

template<typename CharT>
[[nodiscard]] constexpr int on_dynamic_arg(size_t arg_id, UNITS_STD_FMT::basic_format_parse_context<CharT>& context)
{
  context.check_arg_id(UNITS_FMT_TO_ARG_ID(arg_id));
  return verify_dynamic_arg_index_in_range(arg_id);
}

template<typename CharT>
[[nodiscard]] constexpr int on_dynamic_arg(auto_id, UNITS_STD_FMT::basic_format_parse_context<CharT>& context)
{
  return verify_dynamic_arg_index_in_range(UNITS_FMT_FROM_ARG_ID(context.next_arg_id()));
}

template<class Handler, typename FormatContext>
[[nodiscard]] constexpr int get_dynamic_spec(int index, FormatContext& ctx)
{
  const unsigned long long value =
    UNITS_STD_FMT::visit_format_arg(Handler{}, ctx.arg(UNITS_FMT_TO_ARG_ID(static_cast<size_t>(index))));
  if (value > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
    UNITS_THROW(UNITS_STD_FMT::format_error("number is too big"));
  }
  return static_cast<int>(value);
}

// Parses the range [begin, end) as an unsigned integer. This function assumes
// that the range is non-empty and the first character is a digit.
template<std::input_iterator It, std::sentinel_for<It> S>
[[nodiscard]] constexpr It parse_nonnegative_int(It begin, S end, size_t& value)
{
  gsl_Expects(begin != end && '0' <= *begin && *begin <= '9');
  constexpr auto max_int = static_cast<unsigned>(std::numeric_limits<int>::max());
  constexpr auto big_int = max_int / 10u;
  value = 0;

  do {
    if (value > big_int) {
      value = max_int + 1;
      break;
    }
    value = value * 10 + static_cast<unsigned int>(*begin - '0');
    ++begin;
  } while (begin != end && '0' <= *begin && *begin <= '9');

  if (value > max_int) UNITS_THROW(UNITS_STD_FMT::format_error("Number is too big"));

  return begin;
}

template<std::input_iterator It, std::sentinel_for<It> S>
[[nodiscard]] constexpr It parse_nonnegative_int(It begin, S end, int& value)
{
  size_t val_unsigned = 0;
  begin = parse_nonnegative_int(begin, end, val_unsigned);
  // Never invalid because parse_nonnegative_integer throws an error for values that don't fit in signed integers
  value = static_cast<int>(val_unsigned);
  return begin;
}

template<std::input_iterator It, std::sentinel_for<It> S, typename IDHandler>
[[nodiscard]] constexpr It do_parse_arg_id(It begin, S end, IDHandler&& handler)
{
  gsl_Expects(begin != end);
  auto c = *begin;
  if (c >= '0' && c <= '9') {
    size_t index = 0;
    if (c != '0')
      begin = parse_nonnegative_int(begin, end, index);
    else
      ++begin;
    if (begin == end || (*begin != '}' && *begin != ':'))
      UNITS_THROW(UNITS_STD_FMT::format_error("invalid format string"));
    else
      handler(index);
    return begin;
  }
  UNITS_THROW(UNITS_STD_FMT::format_error("invalid format string"));
}

template<std::input_iterator It, std::sentinel_for<It> S, typename IDHandler>
[[nodiscard]] constexpr It parse_arg_id(It begin, S end, IDHandler&& handler)
{
  auto c = *begin;
  if (c != '}' && c != ':') return do_parse_arg_id(begin, end, handler);
  handler();
  return begin;
}

template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
[[nodiscard]] constexpr It parse_sign(It begin, S end, Handler&& handler)
{
  gsl_Expects(begin != end);
  switch (to_ascii(*begin)) {
    case '+':
      handler.on_sign(fmt_sign::plus);
      ++begin;
      break;
    case '-':
      handler.on_sign(fmt_sign::minus);
      ++begin;
      break;
    case ' ':
      handler.on_sign(fmt_sign::space);
      ++begin;
      break;
    default:
      break;
  }
  return begin;
}

template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
[[nodiscard]] constexpr It parse_width(It begin, S end, Handler&& handler)
{
  struct width_adapter {
    Handler& handler;
    constexpr void operator()() { handler.on_dynamic_width(auto_id{}); }
    constexpr void operator()(size_t id) { handler.on_dynamic_width(id); }
  };

  gsl_Expects(begin != end);
  if ('0' <= *begin && *begin <= '9') {
    int width = 0;
    begin = parse_nonnegative_int(begin, end, width);
    if (width != -1)
      handler.on_width(width);
    else
      UNITS_THROW(UNITS_STD_FMT::format_error("number is too big"));
  } else if (*begin == '{') {
    ++begin;
    if (begin != end) begin = parse_arg_id(begin, end, width_adapter{handler});
    if (begin == end || *begin != '}') UNITS_THROW(UNITS_STD_FMT::format_error("invalid format string"));
    ++begin;
  }
  return begin;
}

template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
[[nodiscard]] constexpr It parse_precision(It begin, S end, Handler&& handler)
{
  struct precision_adapter {
    Handler& handler;
    constexpr void operator()() { handler.on_dynamic_precision(auto_id{}); }
    constexpr void operator()(size_t id) { handler.on_dynamic_precision(id); }
  };

  ++begin;
  auto c = begin != end ? *begin : std::iter_value_t<It>();
  if ('0' <= c && c <= '9') {
    auto precision = 0;
    begin = parse_nonnegative_int(begin, end, precision);
    if (precision != -1)
      handler.on_precision(precision);
    else
      UNITS_THROW(UNITS_STD_FMT::format_error("number is too big"));
  } else if (c == '{') {
    ++begin;
    if (begin != end) begin = parse_arg_id(begin, end, precision_adapter{handler});
    if (begin == end || *begin++ != '}') UNITS_THROW(UNITS_STD_FMT::format_error("invalid format string"));
  } else {
    UNITS_THROW(UNITS_STD_FMT::format_error("missing precision specifier"));
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
template<std::input_iterator It, std::sentinel_for<It> S, typename Handler>
[[nodiscard]] constexpr It parse_align(It begin, S end, Handler&& handler)
{
  gsl_Expects(begin != end);
  auto align = fmt_align::none;
  auto p = begin + code_point_length(begin);
  if (p >= end) p = begin;
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
      default:
        break;
    }
    if (align != fmt_align::none) {
      if (p != begin) {
        auto c = *begin;
        if (c == '{') UNITS_THROW(UNITS_STD_FMT::format_error("invalid fill character '{'"));
        handler.on_fill(std::basic_string_view<std::iter_value_t<It>>(&*begin, static_cast<size_t>(p - begin)));
        begin = p + 1;
      } else
        ++begin;
      handler.on_align(align);
      break;
    } else if (p == begin) {
      break;
    }
    p = begin;
  }
  return begin;
}

// Parses standard format specifiers and sends notifications about parsed
// components to handler.
template<std::input_iterator It, std::sentinel_for<It> S, typename SpecHandler>
[[nodiscard]] constexpr It parse_format_specs(It begin, S end, SpecHandler&& handler)
{
  if (begin + 1 < end && begin[1] == '}' && is_ascii_letter(*begin) && *begin != 'L') {
    handler.on_type(*begin++);
    return begin;
  }

  if (begin == end) return begin;

  begin = ::units::detail::parse_align(begin, end, handler);
  if (begin == end) return begin;

  // Parse sign.
  begin = ::units::detail::parse_sign(begin, end, handler);
  if (begin == end) return begin;

  if (*begin == '#') {
    handler.on_hash();
    if (++begin == end) return begin;
  }

  // Parse zero flag.
  if (*begin == '0') {
    handler.on_zero();
    if (++begin == end) return begin;
  }

  begin = ::units::detail::parse_width(begin, end, handler);
  if (begin == end) return begin;

  // Parse precision.
  if (*begin == '.') {
    begin = ::units::detail::parse_precision(begin, end, handler);
    if (begin == end) return begin;
  }

  if (*begin == 'L') {
    handler.on_localized();
    ++begin;
  }

  // Parse type.
  if (begin != end && *begin != '}') handler.on_type(*begin++);
  return begin;
}

// A format specifier handler that sets fields in basic_format_specs.
template<typename Char>
class specs_setter {
protected:
  basic_format_specs<Char>& specs_;
public:
  constexpr explicit specs_setter(basic_format_specs<Char>& specs) : specs_(specs) {}
  constexpr void on_align(fmt_align align) { specs_.align = align; }
  constexpr void on_fill(std::basic_string_view<Char> fill) { specs_.fill = fill; }
  constexpr void on_sign(fmt_sign s) { specs_.sign = s; }
  constexpr void on_hash() { specs_.alt = true; }
  constexpr void on_localized() { specs_.localized = true; }
  constexpr void on_zero() { specs_.fill[0] = Char('0'); }
  constexpr void on_width(int width) { specs_.width = width; }
  constexpr void on_precision(int precision) { specs_.precision = precision; }
  constexpr void on_type(Char type) { specs_.type = static_cast<char>(type); }
};

// Format spec handler that saves references to arguments representing dynamic
// width and precision to be resolved at formatting time.
template<typename ParseContext>
class dynamic_specs_handler : public specs_setter<typename ParseContext::char_type> {
public:
  using char_type = TYPENAME ParseContext::char_type;

  constexpr dynamic_specs_handler(dynamic_format_specs<char_type>& specs, ParseContext& ctx) :
      specs_setter<char_type>(specs), specs_(specs), context_(ctx)
  {
  }

  template<typename T>
  constexpr void on_dynamic_width(T t)
  {
    specs_.dynamic_width_index = on_dynamic_arg(t, context_);
  }

  template<typename T>
  constexpr void on_dynamic_precision(T t)
  {
    specs_.dynamic_precision_index = on_dynamic_arg(t, context_);
  }
private:
  dynamic_format_specs<char_type>& specs_;
  ParseContext& context_;
};

}  // namespace units::detail
