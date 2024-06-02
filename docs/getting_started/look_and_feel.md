# Look and Feel

Here is a small example of operations possible on scalar quantities:

=== "C++ modules"

    ```cpp
    import mp_units;

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    // simple numeric operations
    static_assert(10 * km / 2 == 5 * km);

    // conversions to common units
    static_assert(1 * h == 3600 * s);
    static_assert(1 * km + 1 * m == 1001 * m);

    // derived quantities
    static_assert(1 * km / (1 * s) == 1000 * m / s);
    static_assert(2 * km / h * (2 * h) == 4 * km);
    static_assert(2 * km / (2 * km / h) == 1 * h);

    static_assert(2 * m * (3 * m) == 6 * m2);

    static_assert(10 * km / (5 * km) == 2 * one);

    static_assert(1000 / (1 * s) == 1 * kHz);
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    // simple numeric operations
    static_assert(10 * km / 2 == 5 * km);

    // conversions to common units
    static_assert(1 * h == 3600 * s);
    static_assert(1 * km + 1 * m == 1001 * m);

    // derived quantities
    static_assert(1 * km / (1 * s) == 1000 * m / s);
    static_assert(2 * km / h * (2 * h) == 4 * km);
    static_assert(2 * km / (2 * km / h) == 1 * h);

    static_assert(2 * m * (3 * m) == 6 * m2);

    static_assert(10 * km / (5 * km) == 2 * one);

    static_assert(1000 / (1 * s) == 1 * kHz);
    ```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/8acPeq743)"


This library requires some C++20 features ([concepts and constraints](https://en.cppreference.com/w/cpp/language/constraints),
[classes as NTTP](https://en.cppreference.com/w/cpp/language/template_parameters), ...). Thanks to them,
a user gets a powerful but still easy-to-use interface where all unit conversions and dimensional analysis can be
performed without sacrificing accuracy. Please see the below example for a quick preview of basic library features:

*[NTTP]: Non-Type Template Parameter

=== "C++ modules"

    ```cpp
    #include <format>
    #include <iomanip>
    #include <iostream>
    #include <print>
    import mp_units;

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

      constexpr quantity v1 = 110 * km / h;
      constexpr quantity v2 = 70 * mph;
      constexpr quantity v3 = avg_speed(220. * isq::distance[km], 2 * h);
      constexpr quantity v4 = avg_speed(isq::distance(140. * mi), 2 * h);
      constexpr quantity v5 = v3.in(m / s);
      constexpr quantity v6 = value_cast<m / s>(v4);
      constexpr quantity v7 = value_cast<int>(v6);

      std::cout << v1 << '\n';                                        // 110 km/h
      std::cout << std::setw(10) << std::setfill('*') << v2 << '\n';  // ***70 mi/h
      std::cout << std::format("{:*^10}\n", v3);                      // *110 km/h*
      std::println("{:%N in %U of %D}", v4);                          // 70 in mi/h of LT⁻¹
      std::println("{::N[.2f]}", v5);                                 // 30.56 m/s
      std::println("{::N[.2f]U[dn]}", v6);                            // 31.29 m⋅s⁻¹
      std::println("{:%N}", v7);                                      // 31
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/format.h>
    #include <mp-units/ostream.h>
    #include <mp-units/systems/international.h>
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <format>
    #include <iomanip>
    #include <iostream>
    #include <print>

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

      constexpr quantity v1 = 110 * km / h;
      constexpr quantity v2 = 70 * mph;
      constexpr quantity v3 = avg_speed(220. * isq::distance[km], 2 * h);
      constexpr quantity v4 = avg_speed(isq::distance(140. * mi), 2 * h);
      constexpr quantity v5 = v3.in(m / s);
      constexpr quantity v6 = value_cast<m / s>(v4);
      constexpr quantity v7 = value_cast<int>(v6);

      std::cout << v1 << '\n';                                        // 110 km/h
      std::cout << std::setw(10) << std::setfill('*') << v2 << '\n';  // ***70 mi/h
      std::cout << std::format("{:*^10}\n", v3);                      // *110 km/h*
      std::println("{:%N in %U of %D}", v4);                          // 70 in mi/h of LT⁻¹
      std::println("{::N[.2f]}", v5);                                 // 30.56 m/s
      std::println("{::N[.2f]U[dn]}", v6);                            // 31.29 m⋅s⁻¹
      std::println("{:%N}", v7);                                      // 31
    }
    ```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/nhqhT8Mzb)"

!!! note

    More code examples can be found in the [Examples](../users_guide/examples/tags_index.md) chapter.
