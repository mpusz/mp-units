.. namespace:: units

Quantities
==========

A :term:`quantity` is a concrete amount of a unit for a specified dimension
with a specific representation and is represented in the library with a
`quantity` class template.


Quantity Construction
---------------------

To create the quantity object from a :term:`scalar` we just have to pass
the value to the `quantity` class template explicit constructor::

    quantity<si::dim_length, si::kilometre, double> d(123);

.. note::

    As the constructor is explicit, the quantity object can be created from
    an "unsafe" fundamental type only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity<si::dim_length, si::kilometre, double> d = 123;  // ERROR

To simplify `quantity` objects creation the library provides helper aliases for
quantities of each :term:`dimension` which additionally set the representation
type to ``double`` by default::

    namespace si {

    template<Unit U, Scalar Rep = double>
    using length = quantity<dim_length, U, Rep>;

    }

Thanks to that, the above example can be rewritten as follows::

    si::length<si::kilometre> d(123);

To further simplify construction of quantities with compile-time known
values the library provides :abbr:`UDL (User Defined Literal)` s for each
:term:`unit` of every :term:`dimension`. Thanks to them the same code can
be as simple as::

    using namespace units::physical::si::literals;
    constexpr auto d1 = 123q_km;    // si::length<si::kilometre, std::int64_t>
    constexpr auto d2 = 123.q_km;   // si::length<si::kilometre, long double>

``123q_km`` should be read as a quantity of length in kilometers. Initially the
library did not use the ``q_`` prefix for UDLs but it turned out that there are
a few unit symbols that collide with literals already existing in C and C++
language (i.e. ``F`` (farad), ``J`` (joule), ``W`` (watt), ``K`` (kelvin),
``d`` (day), ``l`` or ``L`` (litre), ``erg``, ``ergps``). This is why the
``q_`` prefix was consistently applied to all the UDLs.


Dimension-specific Concepts
---------------------------

In case the user does not care about the specific unit and representation but
requires quantity of a concrete dimension than dimension-specific concepts can
be used::

    using namespace units::physical::si::literals;
    constexpr Length auto d = 123q_km;  // si::length<si::kilometre, std::int64_t>

.. note::

    All instances of `quantity` class always match the `Quantity` concept.
    All other regular types that are not quantities are called
    :term:`scalars <scalar>` by the library and match the `Scalar` concept.

However, the above is not the most important usage of those concepts. Let's
assume that the user wants to implement an ``avg_speed`` function that will
be calculating the average speed based on provided distance and duration
quantities. The usage of such a function can look as follows::

    using namespace units::physical::si::literals;
    using namespace units::physical::international::literals;
    constexpr Speed auto v1 = avg_speed(220q_km, 2q_h);
    constexpr Speed auto v2 = avg_speed(140q_mi, 2q_h);

In this and all other physical units libraries such a function can be
implemented as::

    constexpr si::speed<si::metre_per_second> avg_speed(si::length<si::metre> d,
                                                        si::time<si::second> t)
    {
      return d / t;
    }

While being correct, this function performs unnecessary intermediate
conversions (from kilometers to meters, from hours to seconds,
and from meters per second to kilometers per hour) which can affect
runtime performance and the precision of the final result. To eliminate
all that overhead we have to write a template function::

    template<typename U1, typename R1, typename U2, typename R2>
    constexpr auto avg_speed(si::length<U1, R1> d, si::time<U2, R2> t)
    {
      return d / t;
    }

This function will work for every SI unit and representation without any
unnecessary overhead. It is also simple enough to prove its implementation
being correct just by a simple inspection. However, it might not always be
the case. For more complicated calculations we would like to ensure that we
are returning a physical quantity of a correct dimension. For this
dimension-specific concepts come handy again and with usage of C++20 generic
functions our function can look as simple as::

    constexpr Speed auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

Now we are sure that the dimension of returned quantity is correct. Also
please note that with the above code we implemented a truly generic function
that works efficiently not only with SI units but also with other systems of
units like CGS.

.. seealso::

    Please refer to :ref:`avg_speed` example for more information on different
    kinds of interfaces supported by the library.


Working With Constrained Deduced Quantity Types
-----------------------------------------------

It is important to note that when we assign a result from the function to an
automatically deduced type, even if it is constrained by a dimension-specific
concept, we still do not know what is the exact unit and representation type
of such a quantity. In many cases it might be exactly what we want to get,
but often we would like to know a specific type too. We have two options here:

- query the actual dimension, unit, and representation types::

    constexpr Speed auto v = avg_speed(220q_km, 2q_h);
    using quantity_type = decltype(v);
    using dimension_type = quantity_type::dimension;
    using unit_type = quantity_type::unit;
    using rep_type = quantity_type::rep;

- convert or cast to a desired quantity type::

    constexpr Speed auto v1 = avg_speed(220.q_km, 2q_h);
    constexpr si::speed<si::metre_per_second> v2 = v1;
    constexpr Speed auto v3 = quantity_cast<si::speed<si::metre_per_second>(v1);

.. seealso::

    More information on this subject can be found in :ref:`Conversions and Casting`
    chapter.


Dimensionless Quantities
------------------------

Whenever we divide two quantities of the same dimension we end up with a
:term:`dimensionless quantity` otherwise known as :term:`quantity of dimension one`::

    static_assert(10q_km / 5q_km == 2);
    static_assert(std::is_same_v<decltype(10q_km / 5q_km), quantity<dim_one, unitless, std::int64_t>>);

According to the official ISO definition `dim_one` is a dimension "for which all the
exponents of the factors corresponding to the base quantities in its quantity dimension
are zero".

.. seealso::

    Reasoning for the above design is provided in
    :ref:`Why a dimensionless quantity is not just an fundamental arithmetic type?`

To simplify the usage of the dimensionless quantity a following concept and alias template
are provided::

    template<typename T>
    concept Dimensionless = QuantityOf<T, dim_one>;

    template<Unit U, Scalar Rep = double>
    using dimensionless = quantity<dim_one, U, Rep>;

There are two special units provided for usage with such a quantity:

- `unitless` which is the :ref:`coherent unit` of dimensionless quantity and does not
  provide any textual symbol (according to the ISO definition "the measurement units and
  values of quantities of dimension one are numbers"),
- `percent` which has the symbol ``%`` and ``ratio(1, 100)`` of the `unitless` unit.

For example the following code::

    std::cout << quantity_cast<percent>(50.q_m / 100.q_m) << '\n';

will print ``50 %`` to the console output.

Again, according to the ISO definition "such quantities convey more information than a
number". This is exactly what we observe in the above example. The value stored inside
the quantity, the text output, and the value returned by the `quantity::count()` member
function is ``50`` rather than ``0.5``. It means that dimensionless quantities behave
like all other quantities and store the value in terms of a ratio of a coherent unit.
This allows us to not loose precision when we divide quantities of the same dimensions
but with units having vastly different ratios, e.g.
`Dimensionless Hubble parameter <https://en.wikipedia.org/wiki/Hubble%27s_law#Dimensionless_Hubble_parameter>`_
is expressed as a ratio of kilometers and megaparsecs.

.. seealso::

    More information on dimensionless quantities can be found in
    :ref:`Implicit conversions of dimensionless quantities`.
