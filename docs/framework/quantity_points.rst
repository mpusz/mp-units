.. namespace:: units

Quantity Points
===============

A quantity point is an absolute quantity with respect to an abstract origin
and is represented in the library with a
`quantity_point` class template.

The library supports working with multiple distinct origins, which may or may not be related to each other.
The most well-known example is the case of temperature points, where the implied origin when working with
degree Celisus (°C), degree Fahrenheit (°F) and Kelvin are all different, but at a fixed temperature offset from each other.

If that offset is known and fixed, `quantity_point_cast` is able to transform a `quantity_point` with respect
to one origin to a `quantity_point` specified to another origin. For an example with temperatures,
see the :ref:`examples/temperature:temperature.cpp` example.

If there is no known and fixed offset between two origins, then `quantity_point_cast` will refuse the conversion.

Construction
------------

Construction by constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To create the quantity point object from a `quantity` we just have to pass
the value to the `quantity_point` class template explicit constructor::

    quantity_point<si::dim_length, si::kilometre, double> d(123_q_km);

In this case, a default abstract origin will be created, which is the same for all `quantity_point`s with default
constructed origin of the same dimension, and unrelated to all other origins in the library.

.. note::
   For temperatures, the created default origin *will be the same* for all temperature units and thus not allow
   the "usual" conversion between temperatures. In those cases

Alternatively, a custom origin may be specified::

    quantity_point<si::dim_length, si::kilometre, double, mean_sea_level> flight_level(11_q_km);


.. note::

    As the constructor is explicit, the quantity object can be created from
    a quantity only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity_point<si::dim_length, si::kilometre, double> d = 123_q_km;  // ERROR

Construction by origin constant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library provides origin constants for the three most common temperature references:

 - `units::physical::si::zp_kelvin`: The point of 0 K.
 - `units::physical::si::zp_deg_celsius`: The point of 0 °C.
 - `units::physical::si::imperial::zp_deg_fahrenheit`: The point of 0 °F.

Those can be added to instances of :class:`quantity` to create a :class:`quantity_point`::

    using namespace units::physical::si;

    celsius_temperature_point hot_chocolate = zp_deg_celsius + 55 * deg_C;

For a custom origin point, the corresponding point constant can be defined as follows::

    inline constexp auto zp_mean_sea_level = quantity_point<si::dim_length, si::kilometre, zero_rep, mean_sea_level>{};

Construction by UDL
^^^^^^^^^^^^^^^^^^^

Only for temperatures, the library provides the following UDL:
 - `_qp_K`
 - `_qp_deg_C`
 - `_qp_deg_F`

Differences To Quantity
-----------------------

Unlike `quantity` and with the exception of temperatures, the library provides:

- no helper aliases for quantity points, such as ``length_point``,
- no UDLs for quantity points,
- no origin constants,
- no dimension-specific concepts, such as ``LengthPoint``
  (there's the dimension-agnostic `QuantityPoint`),
- a more limited set of operations on quantity points
  (see the :ref:`framework/dimensions:Quantity Points` chapter)

