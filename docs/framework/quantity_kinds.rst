.. namespace:: units

Quantity Kinds
==============

A quantity kind is a quantity of more specific usage.
It is represented in the library with a `quantity_kind` class template.


Kind Creation
-------------

We need a `kind` to represent the more specific usage of a quantity::

    struct radius : kind<radius, si::dim_length> {};

Quantities of kind ``radius`` represent a radius.
They are clearly distinct from more generic usages of length quantities.


Construction
------------

To create the quantity kind object from a `quantity` we just have to pass
the value to the `quantity_kind` class template constructor::

    quantity_kind<radius, si::kilometre, double> d(123 * km);

.. note::

    As the constructor without the kind argument is explicit,
    the quantity object can be created from a quantity only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity_kind<radius, si::kilometre, double> d = 123 * km;  // ERROR


Differences To Quantity
-----------------------

The library provides:

- no kinds, such as ``radius`` or ``width``, therefore

    * no UDLs or quantity references,
    * no kind-specific concepts, such as ``Radius``,
      (there's the generic `QuantityKind` and kind-specifiable `QuantityKindOf`),

- a slightly larger set of operations on quantity kinds
  (see the :ref:`framework/dimensions:Quantity Kinds` chapter).


Quantity Point Kinds
--------------------

A quantity point kind is the analogous of a quantity point for quantity kinds
(see the :ref:`framework/quantity_points:Quantity Points` chapter).

They are represented in the library with a `quantity_point_kind` class template.

First, we need a `point_kind` for a `kind`::

    struct width : kind<width, si::dim_length> {};
    struct x_coordinate : point_kind<x_coordinate, width> {};

Now ``x`` coordinates can be constructed::

    quantity_point_kind<x_coordinate, si::metre, int> auto x_pos(123 * m); // QuantityPointKindOf<x_coordinate>
    auto x = x_pos.relative();   // quantity_kind<width, si::metre, int>(123 * m)

.. seealso::

    Please refer to :ref:`examples/glide_computer:glide_computer` example for more
    information on the quantity kinds usage.
