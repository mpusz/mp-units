.. namespace:: units

Quantity Points
===============

A quantity point is an absolute quantity with respect to an origin
and is represented in the library with a `quantity_point` class template.

Point Origins
-------------

We need a `point_origin` to represent the origin of a quantity point::

    struct mean_sea_level : base_point_origin<si::dim_length> {};

Quantities points with this origin represent a point from the mean sea level.

The library supports working with multiple distinct origins, which may or may not be related to each other.
The most well-known example is the case of temperature points, where the implied origin when working with
degree Celisus (°C), degree Fahrenheit (°F) and Kelvin are all different, but at a fixed temperature offset from each other.

If that offset is known and fixed, `quantity_point_cast` is able to transform a `quantity_point` with respect
to one origin to a `quantity_point` specified to another origin. For an example with temperatures,
see the :ref:`examples/temperature:temperature.cpp` example.

If there is no known and fixed offset between two origins, then `quantity_point_cast` will refuse the conversion
and the compilation will fail.

The library offers a `unspecified_origin<Dimension>`
for quantity points whose origin is not specified in the type system.

It also offers a `customary_origin_for_unit<Unit>` which selects the origin that is usually implied when referring to
to quantity points using the given unit. The compilation fails if the library is not aware of that origin.
Currently, such customary origins are only implemented for the temperature units.


Construction
------------

Construction using `absolute`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

TODO.

Construction by constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To create the quantity point object from a `quantity` we just have to pass
the value to the `quantity_point` class template explicit constructor::

    quantity_point<mean_sea_level, si::kilometre, double> d(123 * km);

With all template parameters being explicit, it is clear which origin is implied.

Alternatively, a "default" origin can be selected using CTAD::

    QuantityPoint d(32 * deg_F);

In this case, if the library is aware of a specific customary origin that is implied by the unit, it will select
the corresponding `customary_origin_for_unit<Unit>`. Otherwise it will select the `unspecified_origin<Dimension>`.

.. warning::
   Be very careful with that machinery in generic code! Normally, generic code does not need to care about the units
   of it's arguments. Arithmetic summation for example will produce the same physically correct result (up to rounding),
   no matter what units each of the arguments is supplied in, as the library takes care of that.

   Here on the other hand, the machinery may select a physically different origin depending on the supplied unit,
   so the physically irrelevant unit suddenly becomes relevant.

.. note::

    As the constructor is explicit, the quantity object can be created from
    a quantity only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity_point<mean_sea_level, si::kilometre, double> d = 123 * km;  // ERROR

Construction by origin constant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: Origin constants were removed in v0.7.0 - we might add them again at a later point.

The library provides origin constants for the three most common temperature references:

 - `units::isq::si::zeroth_K`: The point of 0 K.
 - `units::isq::si::zeroth_deg_C`: The point of 0 °C.
 - `units::isq::si::imperial::zeroth_deg_F`: The point of 0 °F.

Those can be added to instances of :class:`quantity` to create a :class:`quantity_point`::

    using namespace units::isq::si;

    celsius_temperature_point hot_chocolate = zeroth_deg_C + 55 * deg_C;

For a custom origin point, the corresponding point constant can be defined as follows::

    inline constexp auto zeroth_mean_sea_level = quantity_point<si::dim_length, si::kilometre, zero_rep, mean_sea_level>{};

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
- the set of operations on quantity points is more limited than on quantities on purpose,
  excluding physically ill-defined operations
  (see the :ref:`framework/dimensions:Quantity Points` chapter)
