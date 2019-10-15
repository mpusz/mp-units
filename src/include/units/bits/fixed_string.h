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

#include <cstdlib>

namespace units {

//   TODO gcc:92101
//   Gated by the following gcc bug
//   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92101
//
//   template<typename CharT, std::size_t N>
//   struct basic_fixed_string {
//     CharT data_[N+1] = {};

//     constexpr basic_fixed_string(const CharT (&txt)[N+1]) noexcept
//     {
//       for(std::size_t i = 0; i <= N; ++i)
//         data_[i] = txt[i];
//     }
//     // auto operator==(const basic_fixed_string &) = default;
//     [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data_; }
//   };

//   template<typename CharT, std::size_t N>
//   basic_fixed_string(const CharT (&str)[N]) -> basic_fixed_string<CharT, N-1>;

//   template<std::size_t N>
//   using fixed_string = basic_fixed_string<char, N>;

  template<typename CharT, CharT... Chars>
  struct basic_fixed_string {
    static constexpr CharT txt[] = { Chars..., '\0' };

    static constexpr const CharT* c_str() noexcept
    {
      return txt;
    }
  };

  inline namespace hacks {

    template<typename T, T... chars>
    constexpr basic_fixed_string<T, chars...> operator""_fs() { return {}; }

  }

}
