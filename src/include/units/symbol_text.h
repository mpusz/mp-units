#pragma once

#include <units/bits/external/fixed_string.h>

namespace units {

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
struct basic_symbol_text {
  basic_fixed_string<StandardCharT, N> standard_;
  basic_fixed_string<ASCIICharT, M> ascii_;

  constexpr basic_symbol_text(StandardCharT s) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(StandardCharT s, ASCIICharT a) noexcept: standard_(s), ascii_(a) {}
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1]) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s) noexcept: standard_(s), ascii_(s) {}
  constexpr basic_symbol_text(const StandardCharT (&s)[N + 1], const ASCIICharT (&a)[M + 1]) noexcept: standard_(s), ascii_(a) {}
  constexpr basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s, const basic_fixed_string<ASCIICharT, M>& a) noexcept: standard_(s), ascii_(a) {}

  [[nodiscard]] constexpr auto& standard() { return standard_; }
  [[nodiscard]] constexpr const auto& standard() const { return standard_; }
  [[nodiscard]] constexpr auto& ascii() { return ascii_; }
  [[nodiscard]] constexpr const auto& ascii() const { return ascii_; }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return standard_.size(); }
  [[nodiscard]] constexpr const StandardCharT* c_str() const noexcept { return standard_.c_str(); }

  template<std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + M2> operator+(
      const basic_symbol_text& lhs, const basic_symbol_text<StandardCharT, ASCIICharT, N2, M2>& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + M2>(
        lhs.standard_ + rhs.standard_, lhs.ascii_ + rhs.ascii_);
  }

  template<typename CharT, std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2> operator+(
      const basic_symbol_text& lhs, const basic_fixed_string<CharT, N2>& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2>(
        lhs.standard_ + rhs, lhs.ascii_ + rhs);
  }
  
  template<typename CharT, std::size_t N2>
  [[nodiscard]] constexpr friend basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2> operator+(
      const basic_fixed_string<CharT, N2>& lhs, const basic_symbol_text& rhs) noexcept
  {
    return basic_symbol_text<StandardCharT, ASCIICharT, N + N2, M + N2>(
        lhs + rhs.standard_, lhs + rhs.ascii_);
  }

  template<typename StandardCharT2, typename ASCIICharT2, std::size_t N2, std::size_t M2>
  [[nodiscard]] constexpr friend bool operator<(const basic_symbol_text& lhs,
        const basic_symbol_text<StandardCharT2, ASCIICharT2, N2, M2>& rhs) noexcept
  {
    return (lhs.standard_ < rhs.standard_);
  }

  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const StandardCharT (&rhs)[N + 1]) noexcept
  {
    return (lhs.standard_ == basic_fixed_string(rhs));
  }
  
  template<typename StandardCharT2, std::size_t N2>
  [[nodiscard]] constexpr friend bool operator==(const basic_symbol_text& lhs,
                                                 const StandardCharT2 (&rhs)[N2 + 1]) noexcept
  {
    return false;
  }

  template<class Traits>
  friend std::basic_ostream<StandardCharT, Traits>& operator<<(std::basic_ostream<StandardCharT, Traits>& os,
                                                               const basic_symbol_text& symbol)
  {
    return os << symbol.standard_.c_str();
  }
};

template<typename StandardCharT>
basic_symbol_text(StandardCharT) -> basic_symbol_text<StandardCharT, StandardCharT, 1, 1>;

template<typename StandardCharT, typename ASCIICharT>
basic_symbol_text(StandardCharT, ASCIICharT) -> basic_symbol_text<StandardCharT, ASCIICharT, 1, 1>;

template<typename StandardCharT, std::size_t N>
basic_symbol_text(const StandardCharT (&)[N]) -> basic_symbol_text<StandardCharT, StandardCharT, N - 1, N - 1>;

template<typename StandardCharT, std::size_t N>
basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s) -> basic_symbol_text<StandardCharT, StandardCharT, N - 1, N - 1>;

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
basic_symbol_text(const StandardCharT (&)[N], const ASCIICharT (&)[M]) -> basic_symbol_text<StandardCharT, ASCIICharT, N - 1, M - 1>;

template<typename StandardCharT, typename ASCIICharT, std::size_t N, std::size_t M>
basic_symbol_text(const basic_fixed_string<StandardCharT, N>& s,
                  const basic_fixed_string<ASCIICharT, M>& a)
-> basic_symbol_text<StandardCharT, ASCIICharT, N - 1, M - 1>;

}  // namespace units
