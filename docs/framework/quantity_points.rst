.. namespace:: units

Quantity Points
===============

A quantity point is an absolute quantity with respect to an origin
and is represented in the library with a `quantity_point` class template.

Point Origins
-------------

We need a `point_origin` to represent the origin of a quantity point::

    struct mean_sea_level : point_origin<si::dim_length> {};

Quantities points with this origin represent a point from the mean sea level.

The library offers a `dynamic_origin<Dimension>`
for quantity points whose origin is not specified in the type system.


Construction
------------

To create the quantity point object from a `quantity` we just have to pass
the value to the `quantity_point` class template explicit constructor::

    quantity_point<dynamic_origin<si::dim_length>, si::kilometre, double> d(123 * km);

.. note::

    As the constructor is explicit, the quantity object can be created from
    a quantity only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity_point<dynamic_origin<si::dim_length>, si::kilometre, double> d = 123 * km;  // ERROR


Differences To Quantity
-----------------------

Unlike `quantity`, the library provides:

- no helper aliases for quantity points, such as ``length_point``,
- no UDLs for quantity points,
- no dimension-specific concepts, such as ``LengthPoint``
  (there's the dimension-agnostic `QuantityPoint`),
- a more limited set of operations on quantity points
  (see the :ref:`framework/arithmetics:Quantity Points` chapter)
