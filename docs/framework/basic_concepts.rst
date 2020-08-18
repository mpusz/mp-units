.. namespace:: units

Basic Concepts
==============

The most important concepts in the library are `Unit`, `Dimension`,
`Quantity` and `QuantityPoint`:

.. image:: /_static/img/concepts.png
    :align: center

..
    http://www.nomnoml.com

    [<abstract>Dimension|
    [base_dimension<Symbol, Unit>]<-[exp<Dimension, Num, Den>]
    [derived_dimension<Child, Unit, Exponent...>]<-[exp<Dimension, Num, Den>]
    [exp<Dimension, Num, Den>]<-[derived_dimension<Child, Unit, Exponent...>]
    ]

    [<abstract>Quantity|
    [quantity<Dimension, Unit, Rep>]
    ]

    [<abstract>Unit]<-[Dimension]
    [Dimension]<-[Quantity]
    [Unit]<-[Quantity]

`Unit` is a basic building block of the library. Every dimension works with
a concrete hierarchy of units. Such hierarchy defines a reference unit and
often a few scaled versions of it.

`Dimension` concept matches a dimension of either a base or derived quantity.
`base_dimension` is instantiated with a unique symbol identifier and a base
unit. `derived_dimension` is a list of exponents of either base or other
derived dimensions.

`Quantity` is a concrete amount of a unit for a specified dimension with a
specific representation.

`QuantityPoint` is an absolute `Quantity` with respect to some origin.
