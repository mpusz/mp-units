.. namespace:: units

Dimensions
==========

In the previous chapter we briefly introduced the notion of a physical
:term:`dimension`. Now it is time to learn much more about this subject.
Length, time, speed, area, energy are only a few examples of physical
dimensions.

Arithmetics
-----------

Quantities
++++++++++

Quantities of the same dimension can be easily added or subtracted with
each other and the result will always be a quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3-4

    Length auto dist1 = 2_q_m;
    Length auto dist2 = 1_q_m;
    Length auto res1 = dist1 + dist2;
    Length auto res2 = dist1 - dist2;

Additionally, we can always multiply or divide a quantity by a
:term:`scalable number` and in such a case the quantity's dimension will also
not change:

.. code-block::
    :emphasize-lines: 2-4

    Length auto dist = 2_q_m;
    Length auto res1 = dist * 2;   // 4 m
    Length auto res2 = 3 * res1;   // 12 m
    Length auto res3 = res2 / 2;   // 6 m

However, if we try to multiply or divide quantities of the same or
different dimensions, or we will divide a scalable number by a quantity, we most
probably will always end up in a quantity of a yet another dimension:

.. code-block::
    :emphasize-lines: 4-6

    Length auto dist1 = 2_q_m;
    Length auto dist2 = 3_q_m;
    Time auto dur1 = 2_q_s;
    Area auto res1 = dist1 * dist2;     // 6 m²
    Speed auto res2 = dist1 / dur1;     // 1 m/s
    Frequency auto res3 = 10 / dur1;    // 5 Hz

However, please note that there is an exception from the above rule.
In case we divide the same dimensions, or multiply by the inverted
dimension, than we will end up with just a dimensionless quantity:

.. code-block::
    :emphasize-lines: 4-5

    Time auto dur1 = 10_q_s;
    Time auto dur2 = 2_q_s;
    Frequency auto fr1 = 5_q_Hz;
    Dimensionless auto v1 = dur1 / dur2;    // quantity(5)
    Dimensionless auto v2 = dur1 * fr1;     // quantity(50)

Quantity Kinds
++++++++++++++

Quantity kinds behave the same as quantities for all operations,
except that the quantity types in the operators' declarations
are quantity kind types instead.
Additionally, for the dimensional analysis operators,
you can use a quantity argument instead of a quantity kind.

.. code-block::
    :emphasize-lines: 8-9

    struct height_kind : kind<height_kind, dim_length> {};
    struct rate_of_climb_kind : derived_kind<rate_of_climb_kind, height_kind, dim_speed> {};

    template <Unit U, Representation Rep = double> using height = quantity_kind<height_kind, U, Rep>;
    template <Unit U, Representation Rep = double> using rate_of_climb = quantity_kind<rate_of_climb_kind, U, Rep>;

    height h{100 * m};
    rate_of_climb rate = h / (25 * s);
      // quantity_kind<rate_of_climb_kind, si::metre_per_second, int>(4 * m / s)

.. code-block::
    :emphasize-lines: 8-12

    struct width_kind : kind<width_kind, dim_length> {};
    struct horizontal_area_kind : derived_kind<horizontal_area_kind, width_kind, dim_area> {};

    template <Unit U, Representation Rep = double> using width = quantity_kind<width_kind, U, Rep>;
    template <Unit U, Representation Rep = double> using horizontal_area = quantity_kind<horizontal_area_kind, U, Rep>;

    width w{5 * m};
    horizontal_area area1 = w * w;
      // quantity_kind<horizontal_area_kind, si::metre_per_second, int>(25 * m * m)
    width w2 = area1 / w; // quantity_kind<width_kind, si::metre, int>(5 * m)
    auto q1 = w / w; // Dimensionless quantity kinds related to width
    auto q2 = w / (5 * m); // with .common() equal to quantity{1}

Quantity Points
+++++++++++++++

Quantity points have a more restricted set of operations.
Quantity can be added to or subtracted from a quantity point.
The result will always be a quantity point of the same dimension:

.. code-block::
    :emphasize-lines: 3-5

    Length auto dist1 = 2_q_m;
    Length auto dist2 = 1_q_m;
    QuantityPoint auto res1 = quantity_point{dist1} + dist2;
    QuantityPoint auto res2 = dist1 + quantity_point{dist2};
    QuantityPoint auto res3 = quantity_point{dist1} - dist2;

We can also subtract two quantity points.
The result is a relative quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3

    Length auto dist1 = 2_q_m;
    Length auto dist2 = 1_q_m;
    Length auto res1 = quantity_point{dist1} - quantity_point{dist2};

.. note::

    It is not allowed to:

    - add quantity points
    - subtract a quantity point from a quantity:
    - multiply nor divide quantity points with anything else.

    .. code-block::
        :emphasize-lines: 3-5

        Length auto dist1 = 2_q_m;
        Length auto dist2 = 1_q_m;
        auto res1 = quantity_point{dist1} + quantity_point{dist2};  // ERROR
        auto res2 = dist1 - quantity_point{dist2};                  // ERROR
        auto res3 = quantity_point{dist1} / 2_q_s;                  // ERROR

Quantity Point Kinds
++++++++++++++++++++

The same restrictions of a quantity point with respect to its quantity
apply to a quantity point kind with respect to its quantity kind.


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
    Hopefully if [P0847]_ will land in C++23 the additional CRTP-related
    template parameter will be removed from this definition.


Obtaining a Unit of the Dimension
---------------------------------

In order to obtain the base/coherent unit of any dimension type a
`dimension_unit` helper was introduced::

    static_assert(is_same_v<dimension_unit<si::dim_length>, si::metre>);
    static_assert(is_same_v<dimension_unit<si::dim_speed>, si::metre_per_second>);
