.. namespace:: units

Quantity Points
===============

A :term:`quantity point` is an absolute quantity with respect to zero
(which represents some origin) and is represented in the library with a
`quantity_point` class template.


Construction
------------

To create the quantity point object from a `quantity` we just have to pass
the value to the `quantity_point` class template explicit constructor::

    quantity_point<si::dim_length, si::kilometre, double> d(123q_km);

.. note::

    As the constructor is explicit, the quantity object can be created from
    a quantity only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity_point<si::dim_length, si::kilometre, double> d = 123q_km;  // ERROR


Differences to quantity
-----------------------

Unlike `quantity`, the library provides
- no helper aliases for quantity points, such as `length_point`,
- no UDLs for quantity points,
- no dimension-specific concepts, such as `LengthPoint`
  (there's the dimension-agnostic `QuantityPoint`),
