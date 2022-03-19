Quick Start
===========

Here is a small example of possible operations::

    #include <units/isq/si/area.h>
    #include <units/isq/si/frequency.h>
    #include <units/isq/si/length.h>
    #include <units/isq/si/speed.h>
    #include <units/isq/si/time.h>

    using namespace units::isq::si::references;

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

.. admonition:: Try it on Compiler Explorer

    `Example #1 <https://godbolt.org/z/5dvY8Woh1>`_

This library requires some C++20 features (concepts, classes as
:abbr:`NTTP (Non-Type Template Parameter)`, ...). Thanks to them the user gets a powerful
but still easy to use interface where all unit conversions and dimensional analysis can be
performed without sacrificing on accuracy. Please see the below example for a quick preview
of basic library features::

    #include <units/format.h>
    #include <units/isq/si/international/length.h>
    #include <units/isq/si/international/speed.h>
    #include <units/isq/si/length.h>
    #include <units/isq/si/speed.h>
    #include <units/isq/si/time.h>
    #include <units/quantity_io.h>
    #include <iostream>

    using namespace units::isq;

    constexpr Speed auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

    int main()
    {
      using namespace units::isq::si::literals;
      using namespace units::isq::si::references;
      using namespace units::aliases::isq::si::international;

      constexpr Speed auto v1 = 110 * (km / h);
      constexpr Speed auto v2 = mi_per_h(70.);
      constexpr Speed auto v3 = avg_speed(220_q_km, 2_q_h);
      constexpr Speed auto v4 = avg_speed(si::length<si::international::mile>(140), si::time<si::hour>(2));
      constexpr Speed auto v5 = quantity_cast<si::speed<si::metre_per_second>>(v3);
      constexpr Speed auto v6 = quantity_cast<si::metre_per_second>(v4);
      constexpr Speed auto v7 = quantity_cast<int>(v6);

      std::cout << v1 << '\n';                                  // 110 km/h
      std::cout << v2 << '\n';                                  // 70 mi/h
      std::cout << std::format("{}", v3) << '\n';               // 110 km/h
      std::cout << std::format("{:*^14}", v4) << '\n';          // ***70 mi/h****
      std::cout << std::format("{:%Q in %q}", v5) << '\n';      // 30.5556 in m/s
      std::cout << std::format("{0:%Q} in {0:%q}", v6) << '\n'; // 31.2928 in m/s
      std::cout << std::format("{:%Q}", v7) << '\n';            // 31
    }

.. admonition:: Try it on Compiler Explorer

    `Example #2 <https://godbolt.org/z/bcb87Kvea>`_

.. seealso::

    You can find more code examples in the :ref:`examples:Examples` chapter.
