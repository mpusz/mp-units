.. namespace:: units

Dimensions
==========

In the previous chapter we briefly introduced the notion of a physical
:term:`dimension`. Now it is time to learn much more about this subject.
Length, time, speed, area, energy are only a few examples of physical
dimensions.

Operations
----------

Quantities of the same dimension can be easily added or subtracted with
each other and the result will always be a quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3-4

    Length auto dist1 = 2q_m;
    Length auto dist2 = 1q_m;
    Length auto res1 = dist1 + dist2;
    Length auto res2 = dist1 - dist2;

Additionally, we can always multiply or divide a quantity by a
:term:`scalar` and in such a case the quantity's dimension will also
not change:

.. code-block::
    :emphasize-lines: 2-4

    Length auto dist = 2q_m;
    Length auto res1 = dist * 2;   // 4 m
    Length auto res2 = 3 * res1;   // 12 m
    Length auto res3 = res2 / 2;   // 6 m

However, if we try to multiply or divide quantities of the same or
different dimensions, or we will divide a scalar by a quantity, we most
probably will always end up in a quantity of a yet another dimension:

.. code-block::
    :emphasize-lines: 4-6

    Length auto dist1 = 2q_m;
    Length auto dist2 = 3q_m;
    Time auto dur1 = 2q_s;
    Area auto res1 = dist1 * dist2;     // 6 m²
    Speed auto res2 = dist1 / dur1;     // 1 m/s
    Frequency auto res3 = 10 / dur1;    // 5 Hz

However, please note that there is an exception from the above rule.
In case we divide the same dimensions, or multiply by the inverted
dimension, than we will end up with just a scalar type:

.. code-block::
    :emphasize-lines: 4-5

    Time auto dur1 = 10q_s;
    Time auto dur2 = 2q_s;
    Frequency auto fr1 = 5q_Hz;
    Scalar auto v1 = dur1 / dur2;    // 5
    Scalar auto v2 = dur1 * fr1;     // 50

Quantity points have a more restricted set of operations.
Quantity points can't be added together,
but can be added to or subtracted from quantities.
The result will always be a quantity point of the same dimension:

.. code-block::
    :emphasize-lines: 3-4

    Length auto dist1 = 2q_m;
    Length auto dist2 = 1q_m;
    QuantityPoint auto res1 = quantity_point{dist1} + dist2;
    QuantityPoint auto res2 = dist1 + quantity_point{dist2};
    QuantityPoint auto res3 = quantity_point{dist1} - dist2;

.. note::

    You can't subtract a quantity from a quantity point:

    .. code-block::
        Length auto dist1 = 2q_m;
        Length auto dist2 = 1q_m;
        auto res1 = dist1 - quantity_point{dist2};  // ERROR

We can also substract two quantity points.
The result is a relative quantity of the same dimension:

    Length auto dist1 = 2q_m;
    Length auto dist2 = 1q_m;
    Length auto res1 = quantity_point{dist1} - quantity_point{dist2};

That's it! You can't multiply nor divide quantity points with anything else.

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
and `si::metre` is a :term:`base unit` of this base dimension. We can
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
                                         exp<dim_length, 2>> {};
    struct dim_speed : derived_dimension<dim_speed, metre_per_second,
                                         exp<dim_length, 1>, exp<dim_time, -1>> {};

    }

In the above code sample `si::square_metre` and `si::metre_per_second`
are the :term:`coherent derived units <coherent derived unit>` of those
derived dimensions.

Coherent unit argument is followed by the list of exponents that form this
derived dimension. This list is called a :term:`recipe` of this derived
dimension and may contain both base and derived dimensions. In the latter
case the dimension is being extracted to base dimensions by the framework
itself. The order and types of dimensions used in the recipe determine how
an dimension's unnamed unit symbol is being printed in the text output.

.. seealso::

    More information on how the :term:`recipe` affect the printed symbol
    of unnamed unit can be found in the :ref:`Derived Unnamed Units` chapter.

It is important to mention here that beside text output the order and
the number of elements in the `derived_dimension` definition does not
matter. Even if we define the above as:

.. code-block::
    :emphasize-lines: 4, 6

    namespace si {

    struct dim_area  : derived_dimension<dim_area, square_metre,
                                         exp<dim_length, 1>, exp<dim_length, 1>> {};
    struct dim_speed : derived_dimension<dim_speed, metre_per_second,
                                         exp<dim_time, -1>, exp<dim_length, 1>> {};

    }

the library will do its magic and will end up with the same
:term:`normalized derived dimension` which will allow the dimensional
analysis in the library to work as expected.

.. note::

    The first template argument of `derived_dimension` is the type of the
    child class inherited from the instantiation of this `derived_dimension`
    class template. This is called a
    :abbr:`CRTP (Curiously Recurring Template Parameter)` Idiom and is used
    in many places in this library to provide :ref:`The Downcasting Facility`.
    Hopefully if [P0847]_ will land in C++23 the additional CRTP-related
    template parameter will be removed from this definition.


Obtaining a Unit of the Dimension
---------------------------------

In order to obtain the base/coherent unit of any dimension type a
`dimension_unit` helper was introduced::

    static_assert(is_same_v<dimension_unit<si::dim_length>, si::metre>);
    static_assert(is_same_v<dimension_unit<si::dim_speed>, si::metre_per_second>);


.. rubric:: Citations:

.. [P0847] `"Deducing this" <https://wg21.link/P0847>`_, Programming Language C++ proposal
