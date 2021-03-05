.. namespace:: units

Basic Concepts
==============

The most important concepts in the library are `Unit`, `Dimension`,
`Quantity`, `QuantityPoint`, `QuantityKind`, and `QuantityPointKind`:

.. image:: /_static/img/concepts.png
    :align: center

..
    http://www.nomnoml.com

    [<abstract>Dimension|
    [base_dimension<Symbol, Unit>]<-[exponent<Dimension, Num, Den>]
    [derived_dimension<Child, Unit, Exponent...>]<-[exponent<Dimension, Num, Den>]
    [exponent<Dimension, Num, Den>]<-[derived_dimension<Child, Unit, Exponent...>]
    ]

    [<abstract>Quantity|
    [quantity<Dimension, Unit, Rep>]
    ]

    [<abstract>QuantityPoint|
    [quantity_point<Dimension, Unit, Rep>]
    ]

    [<abstract>QuantityKind|
    [quantity_kind<Kind, Unit, Rep>]
    ]

    [<abstract>QuantityPointKind|
    [quantity_point_kind<PointKind, Unit, Rep>]
    ]

    [<abstract>Unit]<-[Dimension]
    [Dimension]<-[Quantity]
    [Unit]<-[Quantity]
    [Quantity]<-[QuantityPoint]

    [<abstract>Kind]<-[QuantityKind]
    [Dimension]<-[Kind]
    [Quantity]<-[QuantityKind]

    [<abstract>PointKind]<-[QuantityPointKind]
    [Kind]<-[PointKind]
    [QuantityKind]<-[QuantityPointKind]

`Unit` is a basic building block of the library. Every dimension works with
a concrete hierarchy of units. Such hierarchy defines a reference unit and
often a few scaled versions of it. Examples: ``second``, ``metre``, ``kilometre_per_hour``.

`Dimension` concept matches a dimension of either a base or derived quantity.
`base_dimension` is instantiated with a unique symbol identifier and a base
unit. `derived_dimension` is a list of exponents of either base or other
derived dimensions. Examples: ``si::dim_time``, ``si::dim_length``, ``si::dim_speed``.

`Quantity` is a concrete amount of a unit for a specified dimension with a
specific representation. Examples: ``quantity<si::dim_time, si::second, int>``,
``si::length<si::metre, int>``, ``si::speed<si::kilometre_per_hour>``.

`QuantityPoint` is an absolute `Quantity` with respect to some origin.
Examples: timestamp (as opposed to duration), absolute temperature
(as opposed to temperature difference).

`QuantityKind` is a `Quantity` with more specific usage. Examples:
distance (``horizonal_kind``) and height (``vertical_kind``) are different kinds
of a length quantity.

`QuantityPointKind` is an absolute `QuantityKind` with respect to some origin.
Examples: altitude is a quantity point of ``vertical_kind`` (as opposed to
height).
