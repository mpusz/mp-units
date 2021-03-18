Quick Start
===========

Here is a small example of possible operations::

    #include <units/isq/si/area.h>
    #include <units/isq/si/frequency.h>
    #include <units/isq/si/speed.h>

    using namespace units::isq::si;

    // simple numeric operations
    static_assert(10_q_km / 2 == 5_q_km);

    // unit conversions
    static_assert(1_q_h == 3600_q_s);
    static_assert(1_q_km + 1_q_m == 1001_q_m);

    // dimension conversions
    static_assert(1_q_km / 1_q_s == 1000_q_m_per_s);
    static_assert(2_q_km_per_h * 2_q_h == 4_q_km);
    static_assert(2_q_km / 2_q_km_per_h == 1_q_h);

    static_assert(2_q_m * 3_q_m == 6_q_m2);

    static_assert(10_q_km / 5_q_km == 2);

    static_assert(1000 / 1_q_s == 1_q_kHz);

.. admonition:: Try it on Compiler Explorer

    `Example #1 <https://godbolt.org/z/shcohY>`_

This library requires some C++20 features (concepts, classes as
:abbr:`NTTP (Non-Type Template Parameter)`, ...). Thanks to them the user gets a powerful
but still easy to use interface where all unit conversions and dimensional analysis can be
performed without sacrificing on accuracy. Please see the below example for a quick preview
of basic library features::

    #include <units/format.h>
    #include <units/isq/si/speed.h>
    #include <units/isq/si/international/speed.h>
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
      using namespace units::isq::si::unit_constants;

      constexpr Speed auto v1 = 110 * (km / h);
      constexpr Speed auto v2 = avg_speed(220_q_km, 2_q_h);
      constexpr Speed auto v3 = avg_speed(si::length<si::international::mile>(140), si::time<si::hour>(2));
      constexpr Speed auto v4 = quantity_cast<si::speed<si::metre_per_second>>(v2);
      constexpr Speed auto v5 = quantity_cast<si::metre_per_second>(v3);
      constexpr Speed auto v6 = quantity_cast<int>(v5);

      std::cout << v1 << '\n';                                  // 110 km/h
      std::cout << fmt::format("{}", v2) << '\n';               // 110 km/h
      std::cout << fmt::format("{:*^14}", v3) << '\n';          // ***70 mi/h****
      std::cout << fmt::format("{:%Q in %q}", v4) << '\n';      // 30.5556 in m/s
      std::cout << fmt::format("{0:%Q} in {0:%q}", v5) << '\n'; // 31.2928 in m/s
      std::cout << fmt::format("{:%Q}", v6) << '\n';            // 31
    }

.. admonition:: Try it on Compiler Explorer

    `Example #2 <https://godbolt.org/z/dY1dEd>`_

.. seealso::

    You can find more code examples in the :ref:`examples:Examples` chapter.
