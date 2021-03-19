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

    static_assert(quantity{1s} + 1_q_s == 2_q_s);
    static_assert(quantity{1s} + 1_q_min == 61_q_s);
    static_assert(10_q_m / quantity{2s} == 5_q_m_per_s);

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
``number(T)`` is replaced with ``relative(T)`` that returns the `QuantityLike` value.

Similar to `quantity` and `quantity_kind`, `quantity_point` and `quantity_kind_point`
provide a deduction guide from `QuantityPointLike`::

    using namespace std::chrono_literals;

    static_assert(quantity_point{std::chrono::sys_seconds{1s}} + 1_q_s == quantity_point{2s});
