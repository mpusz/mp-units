.. namespace:: units

Dimensions
==========

In the previous chapter we briefly introduced the notion of a physical
:term:`dimension`. Now it is time to learn much more about this subject.
Length, time, speed, area, energy are only a few examples of physical
dimensions.

.. raw:: html

    <object data="../_images/dimensions.svg" type="image/svg+xml" class="align-center" style="max-width: 100%;"></object>

..
    https://www.planttext.com


Base Dimensions
---------------

The quantities of base dimensions are called
:term:`base quantities <base quantity>` which are the atomic building blocks
of a :term:`system of quantities`. For example the The International System
of Units (:term:`SI`) defines 7 of them: length, mass, time, electric
current, thermodynamic temperature, substance, and luminous intensity.

To define a new base dimension the `base_dimension` class template is
provided. For example the SI base dimension of length can be defined as::

    namespace si {

    struct dim_length : base_dimension<"L", metre> {};

    }

In the above code sample ``"L"`` is an base dimension's unique identifier
and `isq::si::metre` is a :term:`base unit` of this base dimension. We can
obtain those back easily with::

    static_assert(si::dim_length::symbol == "L");
    static_assert(is_same_v<si::dim_length::base_unit, si::metre>);


Derived Dimensions
------------------

The quantities of derived dimensions are called
:term:`derived quantities <derived quantity>` and are derived from base
quantities. This means that they are created by multiplying or dividing
quantities of other dimensions.

Looking at the previous code snippets the area, speed, or frequency are
the examples of such quantities. Each derived quantity can be represented
as a unique list of exponents of base quantities. For example:

- an area is a length base quantity raised to the exponent ``2``
- a speed is formed from the length base quantity with exponent ``1``
  and time base quantity with exponent ``-1``.

The above dimensions can be defined in the library with the
`derived_dimension` class template as follows::

    namespace si {

    struct dim_area  : derived_dimension<dim_area, square_metre,
                                         exponent<dim_length, 2>> {};
    struct dim_speed : derived_dimension<dim_speed, metre_per_second,
                                         exponent<dim_length, 1>, exponent<dim_time, -1>> {};

    }

In the above code sample `isq::si::square_metre` and
`isq::si::metre_per_second` are the
:term:`coherent derived units <coherent derived unit>` of those derived dimensions.

Coherent unit argument is followed by the list of exponents that form this
derived dimension. This list is called a :term:`recipe` of this derived
dimension and may contain both base and derived dimensions. In the latter
case the dimension is being extracted to base dimensions by the framework
itself. The order and types of dimensions used in the recipe determine how
an dimension's unnamed unit symbol is being printed in the text output.

.. seealso::

    More information on how the :term:`recipe` affect the printed symbol
    of unnamed unit can be found in the :ref:`framework/units:Derived Unnamed Units`
    chapter.

It is important to mention here that beside text output the order and
the number of elements in the `derived_dimension` definition does not
matter. Even if we define the above as:

.. code-block::
    :emphasize-lines: 4, 6

    namespace si {

    struct dim_area  : derived_dimension<dim_area, square_metre,
                                         exponent<dim_length, 1>, exponent<dim_length, 1>> {};
    struct dim_speed : derived_dimension<dim_speed, metre_per_second,
                                         exponent<dim_time, -1>, exponent<dim_length, 1>> {};

    }

the library will do its magic and will end up with the same
:term:`normalized derived dimension` which will allow the dimensional
analysis in the library to work as expected.

.. note::

    The first template argument of `derived_dimension` is the type of the
    child class inherited from the instantiation of this `derived_dimension`
    class template. This is called a
    :abbr:`CRTP (Curiously Recurring Template Parameter)` Idiom and is used
    in many places in this library to provide
    :ref:`design/downcasting:The Downcasting Facility`.


Obtaining a Unit of the Dimension
---------------------------------

In order to obtain the base/coherent unit of any dimension type a
`dimension_unit` helper was introduced::

    static_assert(is_same_v<dimension_unit<si::dim_length>, si::metre>);
    static_assert(is_same_v<dimension_unit<si::dim_speed>, si::metre_per_second>);
