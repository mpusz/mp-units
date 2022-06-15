.. namespace:: units

Quantity-like Types
===================

.. raw:: html

    <object data="../_images/quantity_like.svg" type="image/svg+xml" class="align-center" style="max-width: 100%;"></object>

..
    https://www.planttext.com

    @startuml

    skinparam monochrome true
    skinparam shadowing false
    skinparam backgroundColor #fcfcfc

    hide members
    hide circle

    class quantity [[../framework/quantities.html]]
    class quantity_point [[../framework/quantity_points.html]]
    class quantity_kind [[../framework/quantity_kinds.html]]
    class quantity_point_kind [[../framework/quantity_kinds.html#quantity-point-kinds]]

    Rep <-- quantity : number()
    quantity <-- quantity_point : relative()
    quantity <-- quantity_kind : common()
    quantity_kind <-- quantity_point_kind : relative()

    @enduml


[ISO80000]_ defines a :term:`quantity` as a:

    Property of a phenomenon, body, or substance, where the property has a magnitude that can be expressed by
    means of a number and a reference.

You can use `quantity::number()` member function to get a concrete amount of a unit expressed with a specific
representation type ``Rep``.

:term:`Kind of quantity <kind of quantity>` is defined as:

    Aspect common to mutually comparable quantities.

We can obtain a `quantity` with a `quantity_kind::common()` member function.

`quantity_point` and `quantity_point_kind` are absolute quantities and quantity kinds relative to some
specific origin. `quantity` and `quantity_point` types can be obtained from them using a ``relative()`` member
function (`quantity_point::relative()`, `quantity_point_kind::relative()`).
