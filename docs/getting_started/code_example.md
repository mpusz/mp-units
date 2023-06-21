# Code Example

Here is a small example of operations possible on scalar quantities:

```cpp
#include <mp-units/systems/si/si.h>

using namespace mp_units::si::unit_symbols;

// simple numeric operations
static_assert(10 * km / 2 == 5 * km);

// unit conversions
static_assert(1 * h == 3600 * s);
static_assert(1 * km + 1 * m == 1001 * m);

// dimension conversions
inline constexpr auto kmph = km / h;
static_assert(1 * km / (1 * s) == 1000 * (m / s));
static_assert(2 * kmph * (2 * h) == 4 * km);
static_assert(2 * km / (2 * kmph) == 1 * h);

static_assert(2 * m * (3 * m) == 6 * m2);

static_assert(10 * km / (5 * km) == 2);

static_assert(1000 / (1 * s) == 1 * kHz);
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/j8afKnarv)"


This library requires some C++20 features ([concepts and constraints](https://en.cppreference.com/w/cpp/language/constraints),
[classes as NTTP](https://en.cppreference.com/w/cpp/language/template_parameters), ...). Thanks to them,
the user gets a powerful but still easy-to-use interface where all unit conversions and dimensional analysis can be
performed without sacrificing accuracy. Please see the below example for a quick preview of basic library features:

*[NTTP]: Non-Type Template Parameter

```cpp
#include <mp-units/format.h>
#include <mp-units/iostream.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

using namespace mp_units;

constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto d,
                                                QuantityOf<isq::time> auto t)
{
  return d / t;
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::international::unit_symbols;

  constexpr auto v1 = 110 * (km / h);
  constexpr auto v2 = 70 * mph;
  constexpr auto v3 = avg_speed(220. * isq::distance[km], 2 * h);
  constexpr auto v4 = avg_speed(isq::distance(140. * mi), 2 * h);
  constexpr auto v5 = v3[m / s];
  constexpr auto v6 = value_cast<m / s>(v4);
  constexpr auto v7 = value_cast<int>(v6);

  std::cout << v1 << '\n';                                  // 110 km/h
  std::cout << v2 << '\n';                                  // 70 mi/h
  std::cout << std::format("{}", v3) << '\n';               // 110 km/h
  std::cout << std::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
  std::cout << std::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
  std::cout << std::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
  std::cout << std::format("{:%Q}", v7) << '\n';            // 31
}
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/T8bovrqTP)"

!!! note

    You can find more code examples in the Examples chapter.
