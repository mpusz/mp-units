.. namespace:: units

Basic Concepts
==============

The most important concepts in the library are `Unit`, `Dimension`,
`Quantity`, `QuantityPoint`, `QuantityKind`, and `QuantityPointKind`:


.. raw:: html

    <object data="../_images/concepts.svg" type="image/svg+xml" class="align-center" style="max-width: 100%;"></object>

..
    https://www.planttext.com

    @startuml

    skinparam monochrome true
    skinparam shadowing false
    skinparam backgroundColor #fcfcfc

    hide circle
    hide members
    show class methods

    package Unit <<Frame>> [[../../framework/units.html]] {
    }

    package Dimension <<Frame>> [[../../framework/dimensions.html]] {
    }

    package Kind <<Frame>> [[../../framework/quantity_kinds.html#kind-creation]] {
    abstract kind<Dimension> [[../../framework/quantity_kinds.html#kind-creation]]
    }

    package PointKind <<Frame>> [[../../framework/quantity_kinds.html#quantity-point-kinds]] {
    abstract point_kind<Kind, PointOrigin> [[../../framework/quantity_kinds.html#quantity-point-kinds]]
    }

    package PointOrigin <<Frame>> [[../../framework/quantity_points.html#point-origins]] {
    abstract point_origin<Dimension> [[../../framework/quantity_points.html#point-origins]]
    }

    package Quantity <<Frame>> [[../../framework/quantities.html]] {
    class quantity<Dimension, Unit, Rep> [[../../framework/quantities.html#construction]] {
    rep number()
    }
    }

    package QuantityPoint <<Frame>> [[../../framework/quantity_points.html]] {
    class quantity_point<PointOrigin, Unit, Rep> [[../../framework/quantity_points.html#construction]] {
    quantity relative()
    }
    }

    package QuantityKind <<Frame>> [[../../framework/quantity_kinds.html]] {
    class quantity_kind<Kind, Unit, Rep> [[../../framework/quantity_kinds.html#construction]] {
    quantity common()
    }
    }

    package QuantityPointKind <<Frame>> [[../../framework/quantity_kinds.html#quantity-point-kinds]] {
    class quantity_point_kind<PointKind, Unit, Rep> [[../../framework/quantity_kinds.html#quantity-point-kinds]] {
    quantity_kind relative()
    }
    }


    Unit <.. Dimension

    Dimension <... Quantity
    Unit <... Quantity

    Dimension <... Kind
    Dimension <... PointOrigin
    PointOrigin <... PointKind
    Kind <... PointKind

    Unit <... QuantityPoint
    PointOrigin <... QuantityPoint
    quantity --* quantity_point

    Unit <... QuantityKind
    Kind <... QuantityKind
    quantity --* quantity_kind

    Unit <... QuantityPointKind
    PointKind <... QuantityPointKind
    quantity_kind --* quantity_point_kind

    @enduml

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

`QuantityPoint` is an absolute `Quantity` with respect to an origin.
Examples: timestamp (as opposed to duration), absolute temperature
(as opposed to temperature difference).

`QuantityKind` is a `Quantity` with more specific usage. Examples:
distance (``horizonal_kind``) and height (``vertical_kind``) are different kinds
of a length quantity.

`QuantityPointKind` is an absolute `QuantityKind` with respect to an origin.
Examples: altitude is a quantity point of ``vertical_kind`` (as opposed to
height).
