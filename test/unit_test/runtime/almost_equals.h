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

#include <catch2/matchers/catch_matchers_templated.hpp>
#include <units/format.h>
#include <units/quantity.h>

namespace units {

template<Quantity T>
struct AlmostEqualsMatcher : Catch::Matchers::MatcherGenericBase {
  AlmostEqualsMatcher(const T& target) : target_{target} {}

  template<std::convertible_to<T> U>
    requires std::same_as<typename T::rep, typename U::rep> && treat_as_floating_point<typename T::rep> bool
  match(const U& other) const
  {
    using std::abs;
    using common = std::common_type_t<T, U>;
    const auto x = common(target_).number();
    const auto y = common(other).number();
    const auto maxXYOne = std::max({typename T::rep{1}, abs(x), abs(y)});
    return abs(x - y) <= std::numeric_limits<typename T::rep>::epsilon() * maxXYOne;
  }

  std::string describe() const override { return "almost equals: " + STD_FMT::format("{}", target_); }

private:
  const T& target_;
};

template<Quantity T>
AlmostEqualsMatcher<T> AlmostEquals(const T& target)
{
  return {target};
}

}  // namespace units
