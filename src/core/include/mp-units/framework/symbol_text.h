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

// NOLINTBEGIN(*-avoid-c-arrays)
#pragma once

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/hacks.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/fixed_string.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#include <mp-units/ext/contracts.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <bit>
#include <compare>  // IWYU pragma: export
#include <cstddef>
#include <cstdint>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

// NOLINTNEXTLINE(readability-enum-initial-value)
MP_UNITS_EXPORT enum class character_set : std::int8_t {
  utf8,  // µs; m³;  L²MT⁻³
  unicode [[deprecated("2.4.0: Use `utf8` instead")]] = utf8,
  portable,  // us; m^3; L^2MT^-3
  ascii [[deprecated("2.4.0: Use `portable` instead")]] = portable,
  default_character_set = utf8,
  default_encoding [[deprecated("2.5.0: Use `default_character_set` instead")]] = default_character_set
};

using text_encoding [[deprecated("2.5.0: Use `character_set` instead")]] = character_set;

MP_UNITS_EXPORT template<std::size_t N, std::size_t M>
class symbol_text;

namespace detail {

constexpr bool is_basic_literal_character_set_char(char ch)
{
  // https://en.cppreference.com/w/cpp/language/charset
  return ch == 0x00 || (0x07 <= ch && ch <= 0x0D) || (0x20 <= ch && ch <= 0x7E);
}

template<typename InputIt>
constexpr bool is_basic_literal_character_set(InputIt begin, InputIt end) noexcept
{
  return all_of(begin, end, is_basic_literal_character_set_char);
}

template<std::size_t N>
constexpr bool is_basic_literal_character_set(const char (&txt)[N]) noexcept
{
  return is_basic_literal_character_set(std::begin(txt), std::end(txt));
}

template<std::size_t N>
constexpr fixed_u8string<N> to_u8string(fixed_string<N> txt)
{
  MP_UNITS_PRECONDITION(is_basic_literal_character_set(txt.begin(), txt.end()));
  // Compiler defect (gcc < 14, clang < 19): during constant evaluation `bit_cast` leaves the empty
  // base subobject (`detail::fixed_string_iface`) uninitialized, so the result is unusable in a
  // constant expression and every compile-time symbol becomes ill-formed. The element-wise copy is
  // a workaround; `bit_cast` is the intended semantics.
#if (defined MP_UNITS_COMP_GCC && MP_UNITS_COMP_GCC < 14) || (defined MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 19)
  char8_t buffer[N + 1] = {};
  for (std::size_t i = 0; i != N; ++i) buffer[i] = static_cast<char8_t>(txt[i]);
  return fixed_u8string<N>(buffer, buffer + N);
#else
  using result_type = fixed_u8string<N>;
  return std::bit_cast<result_type>(txt);
#endif
}

// Hidden-friend interface for `symbol_text` - see `detail::fixed_string_iface` for the rationale.
// Every unit and prefix symbol in a system header mints a `symbol_text<N, M>` specialization, and
// concatenation widens `N`/`M`, so a derived-unit symbol walks a chain of one-off specializations.
// Hosting the operators here declares them once rather than once per link in that chain.
struct symbol_text_iface {
  template<std::size_t N, std::size_t M, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend symbol_text<N + N2, M + M2> operator+(const symbol_text<N, M>& lhs,
                                                                       const symbol_text<N2, M2>& rhs)
  {
    return symbol_text<N + N2, M + M2>(lhs.utf8() + rhs.utf8(), lhs.portable() + rhs.portable());
  }

  template<std::size_t N, std::size_t M, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const symbol_text<N, M>& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    MP_UNITS_DIAGNOSTIC_PUSH
    MP_UNITS_DIAGNOSTIC_IGNORE_ZERO_AS_NULLPOINTER_CONSTANT
    if (const auto cmp = lhs.utf8() <=> rhs.utf8(); cmp != 0) return cmp;
    MP_UNITS_DIAGNOSTIC_POP
    return lhs.portable() <=> rhs.portable();
  }

  template<std::size_t N, std::size_t M, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const symbol_text<N, M>& lhs, const symbol_text<N2, M2>& rhs) noexcept
  {
    return lhs.utf8() == rhs.utf8() && lhs.portable() == rhs.portable();
  }

  // A friend declared inside the class template takes the enclosing specialization by a NON-deduced
  // `const symbol_text&`, so the other operand could reach it through `symbol_text`'s implicit
  // converting constructors (`sym == 'b'`, `unit._symbol_ == "km"`). Deducing both sides - which is
  // what hosting the operator in a non-template base requires - takes that away, so the raw-operand
  // spellings are restored explicitly here. This is the one behavioural difference the interface-base
  // pattern is not free of; comparison is the only place mp-units relies on it.
  // `symbol_text<N, M>` (not CTAD) is deliberate on both counts: it reproduces the original
  // conversion target exactly - the ENCLOSING specialization, so a size mismatch stays ill-formed -
  // and it keeps the expression dependent, which it must be, since the class template is still
  // incomplete here and the deduction guides are not declared until after it.
  template<std::size_t N, std::size_t M>
  [[nodiscard]] friend constexpr bool operator==(const symbol_text<N, M>& lhs, char rhs) noexcept
    requires requires { symbol_text<N, M>(rhs); }
  {
    return lhs == symbol_text<N, M>(rhs);
  }

  template<std::size_t N, std::size_t M>
  [[nodiscard]] friend constexpr auto operator<=>(const symbol_text<N, M>& lhs, char rhs) noexcept
    requires requires { symbol_text<N, M>(rhs); }
  {
    return lhs <=> symbol_text<N, M>(rhs);
  }

  template<std::size_t N, std::size_t M, std::size_t N2>
  [[nodiscard]] friend consteval bool operator==(const symbol_text<N, M>& lhs, const char (&rhs)[N2])
    requires requires { symbol_text<N, M>(rhs); }
  {
    return lhs == symbol_text<N, M>(rhs);
  }

  template<std::size_t N, std::size_t M, std::size_t N2>
  [[nodiscard]] friend consteval auto operator<=>(const symbol_text<N, M>& lhs, const char (&rhs)[N2])
    requires requires { symbol_text<N, M>(rhs); }
  {
    return lhs <=> symbol_text<N, M>(rhs);
  }
};

}  // namespace detail


/**
 * @brief A symbol text representation
 *
 * This class template is responsible for definition and handling of a symbol text
 * representation. In the libary it is used to define symbols of units and prefixes.
 * Each symbol can have two versions: UTF-8 and portable.
 *
 * @tparam N The size of a UTF-8 symbol
 * @tparam M The size of the portable symbol
 */
MP_UNITS_EXPORT template<std::size_t N, std::size_t M>
class symbol_text : public detail::symbol_text_iface {
public:
  fixed_u8string<N> utf8_;
  fixed_string<M> portable_;

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(false) symbol_text(char ch) : utf8_(static_cast<char8_t>(ch)), portable_(ch)
  {
    MP_UNITS_PRECONDITION(detail::is_basic_literal_character_set_char(ch));
  }

  // NOLINTNEXTLINE(*-avoid-c-arrays, google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] consteval explicit(false) symbol_text(const char (&txt)[N + 1]) :
      utf8_(detail::to_u8string(basic_fixed_string{txt})), portable_(txt)
  {
    MP_UNITS_PRECONDITION(txt[N] == char{});
    MP_UNITS_PRECONDITION(detail::is_basic_literal_character_set(txt));
  }

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  [[nodiscard]] constexpr explicit(false) symbol_text(const fixed_string<N>& txt) :
      utf8_(detail::to_u8string(txt)), portable_(txt)
  {
    MP_UNITS_PRECONDITION(detail::is_basic_literal_character_set(txt.data_));
  }

  // NOLINTNEXTLINE(*-avoid-c-arrays)
  [[nodiscard]] consteval symbol_text(const char8_t (&u)[N + 1], const char (&a)[M + 1]) : utf8_(u), portable_(a)
  {
    MP_UNITS_PRECONDITION(u[N] == char8_t{});
    MP_UNITS_PRECONDITION(a[M] == char{});
    MP_UNITS_PRECONDITION(detail::is_basic_literal_character_set(a));
  }

  [[nodiscard]] constexpr symbol_text(const fixed_u8string<N>& utf8, const fixed_string<M>& portable) :
      utf8_(utf8), portable_(portable)
  {
    MP_UNITS_PRECONDITION(detail::is_basic_literal_character_set(portable.data_));
  }

  [[nodiscard]] constexpr const auto& utf8() const { return utf8_; }
  [[nodiscard]] constexpr const auto& portable() const { return portable_; }
  [[deprecated("2.4.0: Use `utf8()` instead")]] constexpr const auto& unicode() const { return utf8(); }
  [[deprecated("2.4.0: Use `portable()` instead")]] constexpr const auto& ascii() const { return portable(); }

  [[nodiscard]] constexpr bool empty() const
  {
    MP_UNITS_ASSERT_DEBUG(utf8().empty() == portable().empty());
    return utf8().empty();
  }
};

symbol_text(char) -> symbol_text<1, 1>;

template<std::size_t N>
symbol_text(const char (&)[N]) -> symbol_text<N - 1, N - 1>;

template<std::size_t N>
symbol_text(const fixed_string<N>&) -> symbol_text<N, N>;

template<std::size_t N, std::size_t M>
symbol_text(const char8_t (&)[N], const char (&)[M]) -> symbol_text<N - 1, M - 1>;

template<std::size_t N, std::size_t M>
symbol_text(const fixed_u8string<N>&, const fixed_string<M>&) -> symbol_text<N, M>;

}  // namespace mp_units
// NOLINTEND(*-avoid-c-arrays)
