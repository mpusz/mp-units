.. namespace:: units

Interoperability with Other Units Libraries
===========================================

Other units libraries can be easily integrated with **mp-units** via specialization of a
`quantity_like_traits` for external quantity-like type. The type trait specialization should
provide the following:

- type aliases:
    - ``dimension`` mapping the source with target dimension type
    - ``unit`` mapping the source with target unit type
    - ``rep`` mapping the source with target representation type
- a static member function ``number(T)`` that returns the raw value/magnitude of the quantity.

For example, to provide support for the ``std::chrono::duration`` it is enough to define::

    #include <units/customization_points.h>
    #include <units/isq/si/time.h>
    #include <chrono>

    namespace units {

    template<typename Rep, typename Period>
    struct quantity_like_traits<std::chrono::duration<Rep, Period>> {
      using dimension = isq::si::dim_time;
      using unit = downcast_unit<dimension, ratio(Period::num, Period::den)>;
      using rep = Rep;
      [[nodiscard]] static constexpr rep number(const std::chrono::duration<Rep, Period>& q) { return q.count(); }
    };

    } // namespace units

Providing the above allows the external quantity-like type to satisfy the `QuantityLike` concept
which in turn enables the explicit `quantity` constructor that provides the type conversion.
A Class Template Argument Deduction (CTAD) guide is provided to improve the user experience of
such an explicit conversion::

    using namespace std::chrono_literals;

    static_assert(quantity{1s} + 1 * s == 2 * s);
    static_assert(quantity{1s} + 1 * min == 61 * s);
    static_assert(10 * m / quantity{2s} == 5 * (m / s));

.. note::

    As the constructor is explicit, the quantity object can be created from
    quantity-like type only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        using namespace std::chrono_literals;
        quantity q = 1s;  // ERROR

For external quantity point-like types, `quantity_point_like_traits` is also provided.
It works just like `quantity_like_traits`, except that
``number(T)`` is replaced with ``relative(T)`` that returns the `QuantityLike` value
and ``dimension`` is replaced with ``origin``.

Similar to `quantity` and `quantity_kind`, `quantity_point` and `quantity_point_kind`
provide a deduction guide from `QuantityPointLike`::

    using namespace std::chrono_literals;

    static_assert((quantity_point{std::chrono::sys_seconds{1s}} + 1 * s).relative() == 2s);


Interoperability with the C++ Standard Library
----------------------------------------------

The above ``std::chrono``-specific customization points are already predefined in the ``mp-units``
library for both ``std::chrono::duration`` as well as ``std::chrono::time_point``.
They are provided in a dedicated header file::

    #include <units/chrono.h>

The same header file provides additional conversion helpers from ``mp-units`` to
the C++ Standard Library types:

- an alias that provides a conversion from `ratio` to ``std::ratio``::

    template<ratio R>
    using to_std_ratio = /* ... */

- an explicit conversion of `quantity` of `isq::si::dim_time` to ``std::chrono::duration``::

    template<typename U, typename Rep>
    [[nodiscard]] constexpr auto to_std_duration(const quantity<isq::si::dim_time, U, Rep>& q)

- an explicit conversion of `quantity_point` that previously originated from ``std::chrono``
  to ``std::chrono::time_point``::

    template<typename C, typename U, typename Rep>
    [[nodiscard]] constexpr auto to_std_time_point(const quantity_point<clock_origin<C>, U, Rep>& qp)

.. note::

    Only `quantity_point` that was created from a ``std::chrono::time_point`` of a specific
    ``std::chrono`` clock can be converted back to ``std::chrono`` domain::

        const auto qp1 = quantity_point{sys_seconds{1s}};
        const auto tp1 = to_std_time_point(qp1);  // OK

        const auto qp2 = quantity_point{1 * s};
        const auto tp2 = to_std_time_point(qp2);  // Compile-time Error

Here is an example of how interoperatibility described in this chapter can be used in practice::

    using namespace units::aliases::isq::si;
    using timestamp = quantity_point<clock_origin<std::chrono::system_clock>, si::second>;

    const auto start_time = timestamp(std::chrono::system_clock::now());  // std::chrono -> units
    const auto velocity = speed::km_per_h<>(825);
    const auto distance = length::km<>(8111);
    const auto flight_time = distance / velocity;
    const auto exp_end_time = start_time + flight_time;

    const auto tp = to_std_time_point(exp_end_time);                      // units -> std::chrono
    const auto tp_sec = std::chrono::floor<std::chrono::seconds>(tp);
    const auto tp_days = std::chrono::floor<std::chrono::days>(tp_sec);
    const auto ymd = std::chrono::year_month_day(tp_days);
    const auto tod = tp_sec - tp_days;
    const auto hms = std::chrono::hh_mm_ss(tod);
