Quick Start
===========

Here is a small example of possible operations::

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

    `Example #1 <https://godbolt.org/z/XPmjPz>`_

This library requires some C++20 features (concepts, classes as
:abbr:`NTTP (Non-Type Template Parameter)`, ...). Thanks to them the user gets a powerful
but still easy to use interface where all unit conversions and dimensional analysis can be
performed without sacrificing on accuracy. Please see the below example for a quick preview
of basic library features::

    #include <units/physical/si/speed.h>
    #include <units/physical/international/speed.h>
    #include <units/format.h>
    #include <iostream>

    using namespace units::physical;

    constexpr Speed auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

    int main()
    {
      using namespace units::physical::si::literals;
      Speed auto v1 = avg_speed(220_q_km, 2_q_h);
      Speed auto v2 = avg_speed(si::length<international::mile>(140), si::time<si::hour>(2));
      Speed auto v3 = quantity_cast<si::metre_per_second>(v2);
      Speed auto v4 = quantity_cast<int>(v3);

      std::cout << v1 << '\n';                             // 110 km/h
      std::cout << fmt::format("{}", v2) << '\n';          // 70 mi/h
      std::cout << fmt::format("{:%Q in %q}", v3) << '\n'; // 31.2928 in m/s
      std::cout << fmt::format("{:%Q}", v4) << '\n';       // 31
    }

.. admonition:: Try it on Compiler Explorer

    `Example #2 <https://godbolt.org/z/xE91TY>`_

.. seealso::

    You can find more code examples in the :ref:`Examples` chapter.
