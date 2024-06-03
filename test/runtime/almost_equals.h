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
#include <mp-units/compat_macros.h>
#include <algorithm>
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/format.h>
#include <mp-units/framework/quantity.h>
#endif

namespace mp_units {

template<Quantity T>
struct AlmostEqualsMatcher : Catch::Matchers::MatcherGenericBase {
  explicit AlmostEqualsMatcher(const T& target) : target_{target} {}

  template<std::convertible_to<T> U>
    requires std::same_as<typename T::rep, typename U::rep>
  bool match(const U& other) const
  {
    using std::abs;
    using rep = typename T::rep;
    using common = conditional<treat_as_floating_point<rep>, std::common_type_t<T, U>, T>;
    const auto x = common(target_).numerical_value_in(common::unit);
    const auto y = common(other).numerical_value_in(common::unit);
    if constexpr (treat_as_floating_point<rep>) {
      const auto maxXYOne = std::max({rep{1}, abs(x), abs(y)});
      return abs(x - y) <= std::numeric_limits<rep>::epsilon() * maxXYOne;
    } else {
      if (x >= 0) {
        return x - 1 <= y && y - 1 <= x;
      } else {
        return x <= y + 1 && y <= x + 1;
      }
    }
  }

  std::string describe() const override
  {
    if constexpr (treat_as_floating_point<typename T::rep>) {
      return "almost equals: " + MP_UNITS_STD_FMT::format("{}", target_);
    } else {
      return "almost equals: " + MP_UNITS_STD_FMT::format("[ {0} ({0:#x}) +/- 1 ] {1}",
                                                          target_.numerical_value_is_an_implementation_detail_,
                                                          target_.unit);
    }
  }

private:
  const T& target_;
};

template<Quantity T>
AlmostEqualsMatcher<T> AlmostEquals(const T& target)
{
  return AlmostEqualsMatcher<T>{target};
}


}  // namespace mp_units
