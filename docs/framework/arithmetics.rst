.. namespace:: units

Arithmetics
===========

Quantities
----------

Quantities of the same dimension can be easily added or subtracted with
each other and the result will always be a quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3-4

    Length auto dist1 = 2 * m;
    Length auto dist2 = 1 * m;
    Length auto res1 = dist1 + dist2;
    Length auto res2 = dist1 - dist2;

Additionally, we can always multiply or divide a quantity by a
:term:`scalable number` and in such a case the quantity's dimension will also
not change:

.. code-block::
    :emphasize-lines: 2-4

    Length auto dist = 2 * m;
    Length auto res1 = dist * 2;   // 4 m
    Length auto res2 = 3 * res1;   // 12 m
    Length auto res3 = res2 / 2;   // 6 m

However, if we try to multiply or divide quantities of the same or
different dimensions, or we will divide a scalable number by a quantity, we most
probably will always end up in a quantity of a yet another dimension:

.. code-block::
    :emphasize-lines: 4-6

    Length auto dist1 = 2 * m;
    Length auto dist2 = 3 * m;
    Time auto dur1 = 2 * s;
    Area auto res1 = dist1 * dist2;     // 6 m²
    Speed auto res2 = dist1 / dur1;     // 1 m/s
    Frequency auto res3 = 10 / dur1;    // 5 Hz

However, please note that there is an exception from the above rule.
In case we divide the same dimensions, or multiply by the inverted
dimension, than we will end up with just a dimensionless quantity:

.. code-block::
    :emphasize-lines: 4-5

    Time auto dur1 = 10 * s;
    Time auto dur2 = 2 * s;
    Frequency auto fr1 = 5 * Hz;
    Dimensionless auto v1 = dur1 / dur2;    // quantity(5)
    Dimensionless auto v2 = dur1 * fr1;     // quantity(50)

Quantity Kinds
--------------

Quantity kinds behave the same as quantities for all operations,
except that the quantity types in the operators' declarations
are quantity kind types instead.
Additionally, for the dimensional analysis operators,
you can use a quantity argument instead of a quantity kind.

.. code-block::
    :emphasize-lines: 8-9

    struct height_kind : kind<height_kind, dim_length> {};
    struct rate_of_climb_kind : derived_kind<rate_of_climb_kind, dim_speed, height_kind> {};

    template <Unit U, Representation Rep = double> using height = quantity_kind<height_kind, U, Rep>;
    template <Unit U, Representation Rep = double> using rate_of_climb = quantity_kind<rate_of_climb_kind, U, Rep>;

    height h{100 * m};
    rate_of_climb rate = h / (25 * s);
      // quantity_kind<rate_of_climb_kind, si::metre_per_second, int>(4 * m / s)

.. code-block::
    :emphasize-lines: 8-12

    struct width_kind : kind<width_kind, dim_length> {};
    struct horizontal_area_kind : derived_kind<horizontal_area_kind, dim_area, width_kind> {};

    template <Unit U, Representation Rep = double> using width = quantity_kind<width_kind, U, Rep>;
    template <Unit U, Representation Rep = double> using horizontal_area = quantity_kind<horizontal_area_kind, U, Rep>;

    width w{5 * m};
    horizontal_area area1 = w * w;
      // quantity_kind<horizontal_area_kind, si::metre_per_second, int>(25 * m * m)
    width w2 = area1 / w; // quantity_kind<width_kind, si::metre, int>(5 * m)
    auto q1 = w / w; // Dimensionless quantity kinds related to width
    auto q2 = w / (5 * m); // with .common() equal to quantity{1}

Quantity Points
---------------

Quantity points have a more restricted set of operations.
Quantity can be added to or subtracted
from a quantity point of the same origin.
The result will always be a quantity point of the same origin:

.. code-block::
    :emphasize-lines: 3-5

    Length auto dist1 = 2 * m;
    Length auto dist2 = 1 * m;
    QuantityPoint auto res1 = quantity_point{dist1} + dist2;
    QuantityPoint auto res2 = dist1 + quantity_point{dist2};
    QuantityPoint auto res3 = quantity_point{dist1} - dist2;

We can also subtract two quantity points.
The result is a relative quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3

    Length auto dist1 = 2 * m;
    Length auto dist2 = 1 * m;
    Length auto res1 = quantity_point{dist1} - quantity_point{dist2};

.. note::

    It is not allowed to:

    - add quantity points,
    - subtract a quantity point from a quantity,
    - multiply nor divide quantity points with anything else, and
    - mix quantity points with different origins:

    .. code-block::
        :emphasize-lines: 3-5

        Length auto dist1 = 2 * m;
        Length auto dist2 = 1 * m;
        auto res1 = quantity_point{dist1} + quantity_point{dist2};  // ERROR
        auto res2 = dist1 - quantity_point{dist2};                  // ERROR
        auto res3 = quantity_point{dist1} / (2 * s);                // ERROR
        auto res4 = quantity_point{std::chrono::utc_second{1s}} +
                    quantity_point{std::chrono::sys_second{1s}};    // ERROR

Quantity Point Kinds
--------------------

The same restrictions of a quantity point with respect to its quantity
apply to a quantity point kind with respect to its quantity kind.
