#pragma once

#include <units/bits/external/fixed_string.h>
#include <units/bits/external/hacks.h>

namespace units {

namespace detail {

constexpr void validate_ascii_char([[maybe_unused]] char c) noexcept { Expects((c & 0x80) == 0); }

template<std::size_t P>
constexpr void validate_ascii_string([[maybe_unused]] const char (&s)[P + 1]) noexcept
{
#ifndef NDEBUG
  if constexpr (P != 0)
    for (size_t i = 0; i < P; ++i)
      validate_ascii_char(s[i]);
#endif
}

}  // namespace detail


/**
 * @brief A symbol text representation
 * 
 * This class template is responsible for definition and handling of a symbol text
 * representation. In the libary it is used to define symbols of units and prefixes.
 * Each symbol can have two versions: Unicode and ASCI-only.
 * 
 * @tparam StandardCharT Character type to be used for a Unicode representation
 * @tparam N The size of a Unicode symbol
 * @tparam M The size of the ASCII-only symbol
 */
template<typename StandardCharT, std::size_t N, std::size_t M>
struct basic_symbol_text {
  basic_fixed_string<StandardCharT, N> standard_;
  basic_fixed_string<char, M> ascii_;

  constexpr basic_symbol_text(StandardCharT s) noexcept: standard_(s), ascii_(s) { detail::validate_ascii_char(s); }
  constexpr basic_symbol_text(StandardCharT s, char a) noexcept: standard_(s), ascii_(a) { detail::validate_ascii_char(a); }
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1]) noexcept: standard_(s), ascii_(s) { detail::validate_ascii_string<N>(s); }
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s) noexcept: standard_(s), ascii_(s) { detail::validate_ascii_string<N>(s.data_); }
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1], const char (&a)[M + 1]) noexcept: standard_(s), ascii_(a) { detail::validate_ascii_string<M>(a); }
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s, const basic_fixed_string<char, M>& a) noexcept: standard_(s), ascii_(a) { detail::validate_ascii_string<M>(a.data_); }

  [[nodiscard]] constexpr auto& standard() { return standard_; }
  [[nodiscard]] constexpr const auto& standard() const { return standard_; }
  [[nodiscard]] constexpr auto& ascii() { return ascii_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + N2, M + M2> operator+(
      const basic_symbol_text& lhs, const basic_symbol_text<StandardCharT, N2, M2>& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, N + N2, M + M2>(
        lhs.standard_ + rhs.standard_, lhs.ascii_ + rhs.ascii_);
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + N2, M + N2> operator+(
      const basic_symbol_text& lhs, const basic_fixed_string<StandardCharT, N2>& rhs) noexcept
  {
    return lhs + basic_symbol_text<StandardCharT, N2, N2>(rhs);
  }
  
  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + N2, M + N2> operator+(
      const basic_fixed_string<StandardCharT, N2>& lhs, const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, N2, N2>(lhs) + rhs;
  }

  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + N2 - 1, M + N2 - 1> operator+(
      const basic_symbol_text& lhs, const StandardCharT (&rhs)[N2]) noexcept
  {
    return lhs + basic_symbol_text<StandardCharT, N2 - 1, N2 - 1>(rhs);
  }
  
  template<std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + N2 - 1, M + N2 - 1> operator+(
      const StandardCharT (&lhs)[N2], const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, N2 - 1, N2 - 1>(lhs) + rhs;
  }

  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + 1, M + 1> operator+(
      const basic_symbol_text& lhs, StandardCharT rhs) noexcept
  {
    return lhs + basic_symbol_text<StandardCharT, 1, 1>(rhs);
  }
  
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, N + 1, M + 1> operator+(
      StandardCharT lhs, const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, 1, 1>(lhs) + rhs;
  }

#if __GNUC__ >= 10

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr auto operator<=>(const basic_symbol_text& lhs,
                                                  const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    if (const auto cmp = lhs.standard_ <=> rhs.standard_; cmp != 0) return cmp;
    return lhs.ascii_ <=> rhs.ascii_;
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] friend constexpr bool operator==(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return lhs.standard_ == rhs.standard_ && lhs.ascii_ == rhs.ascii_;
  }

#else

  // I did not update the below operators with comparing ASCII as this code is going to be deleted soon anyway...

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return lhs.standard_ == rhs.standard_;
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT, N>& rhs) noexcept
  {
    return lhs.standard_ == rhs;
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT, N>& rhs) noexcept
  {
    return !(lhs == rhs);
  }
  
  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 const basic_fixed_string<StandardCharT2, N2>&) noexcept
  {
    return false;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 const basic_fixed_string<StandardCharT2, N2>&) noexcept
  {
    return true;
  }
  
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const StandardCharT (&rhs)[N + 1]) noexcept
  {
    return lhs.standard_ == rhs;
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 const StandardCharT (&rhs)[N + 1]) noexcept
  {
    return !(lhs == rhs);
  }
  
  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 const StandardCharT2 (&)[N2 + 1]) noexcept
  {
    return false;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 const StandardCharT2 (&)[N2 + 1]) noexcept
  {
    return true;
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 StandardCharT rhs) noexcept
  {
    return lhs.standard_ == rhs;
  }

  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text& lhs,
                                                 StandardCharT rhs) noexcept
  {
    return !(lhs == rhs);
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text&,
                                                 StandardCharT2) noexcept
  {
    return false;
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator!=(const basic_symbol_text&,
                                                 StandardCharT2) noexcept
  {
    return true;
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
     return lhs.standard_ < rhs.standard_;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return lhs.standard_ < rhs;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return lhs.standard_ < basic_fixed_string(rhs);
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
                                                StandardCharT2 rhs) noexcept
  {
    return lhs.standard_ < basic_fixed_string(rhs);
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
                                                const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return rhs < lhs;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
        const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return rhs < lhs;
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
        const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return rhs < lhs;
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator>(const basic_symbol_text& lhs,
                                                StandardCharT2 rhs) noexcept
  {
    return rhs < lhs;
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return !(rhs < lhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return !(rhs < lhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return !(rhs < lhs);
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator<=(const basic_symbol_text& lhs,
                                                 StandardCharT2 rhs) noexcept
  {
    return !(rhs < lhs);
  }

  template<typename StandardCharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const basic_symbol_text<StandardCharT2, N2, M2>& rhs) noexcept
  {
    return !(lhs < rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const basic_fixed_string<StandardCharT2, N2>& rhs) noexcept
  {
    return !(lhs < rhs);
  }

  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 const StandardCharT2 (&rhs)[N2]) noexcept
  {
    return !(lhs < rhs);
  }

  template<typename StandardCharT2>
  [[nodiscard]] constexpr friend bool operator>=(const basic_symbol_text& lhs,
                                                 StandardCharT2 rhs) noexcept
  {
    return !(lhs < rhs);
  }

#endif

};

basic_symbol_text(char) -> basic_symbol_text<char, 1, 1>;

template<typename StandardCharT>
basic_symbol_text(StandardCharT, char) -> basic_symbol_text<StandardCharT, 1, 1>;

template<std::size_t N>
basic_symbol_text(const char (&)[N]) -> basic_symbol_text<char, N - 1, N - 1>;

template<std::size_t N>
basic_symbol_text(const basic_fixed_string<char, N>&) -> basic_symbol_text<char, N, N>;

template<typename StandardCharT, std::size_t N, std::size_t M>
basic_symbol_text(const StandardCharT (&)[N], const char (&)[M]) -> basic_symbol_text<StandardCharT, N - 1, M - 1>;

template<typename StandardCharT, std::size_t N, std::size_t M>
basic_symbol_text(const basic_fixed_string<StandardCharT, N>&,
                  const basic_fixed_string<char, M>&)
-> basic_symbol_text<StandardCharT, N, M>;

}  // namespace units
