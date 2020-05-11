Quick Start
===========

Here is a small example of possible operations::

    // simple numeric operations
    static_assert(10q_km / 2 == 5q_km);

    // unit conversions
    static_assert(1q_h == 3600q_s);
    static_assert(1q_km + 1q_m == 1001q_m);

    // dimension conversions
    static_assert(2q_m * 3q_m == 6q_m2);
    static_assert(10q_km / 5q_km == 2);
    static_assert(1000 / 1q_s == 1q_kHz);
    static_assert(1q_km / 1q_s == 1000q_m_per_s);
    static_assert(2q_km_per_h * 2q_h == 4q_km);
    static_assert(2q_km / 2q_km_per_h == 1q_h);

.. admonition:: Try it on Compiler Explorer

    `Example #1 <https://godbolt.org/z/XPmjPz>`_

This library requires some C++20 features (concepts, classes as
:abbr:`NTTP (Non-Type Template Parameter)`, ...). Thanks to them the user gets a powerful
but still easy to use interface where all unit conversions and dimensional analysis can be
performed without sacrificing on accuracy. Please see the below example for a quick preview
of basic library features::

    #include <units/physical/si/speed.h>
    #include <units/physical/international/speed.h>
    #include <iostream>

    using namespace units::physical;

    constexpr Speed auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

    int main()
    {
      using namespace units::physical::si::literals;
      Speed auto v1 = avg_speed(220q_km, 2q_h);
      Speed auto v2 = avg_speed(si::length<international::mile>(140), si::time<si::hour>(2));
      Speed auto v3 = quantity_cast<si::metre_per_second>(v2);
      Speed auto v4 = quantity_cast<int>(v3);

      std::cout << v1 << '\n';    // 110 km/h
      std::cout << v2 << '\n';    // 70 mi/h
      std::cout << v3 << '\n';    // 31.2928 m/s
      std::cout << v4 << '\n';    // 31 m/s
    }

.. admonition:: Try it on Compiler Explorer

    `Example #2 <https://godbolt.org/z/f-f6y6>`_

.. seealso::

    You can find more code examples in the :ref:`Examples` chapter.
