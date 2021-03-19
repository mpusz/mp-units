.. namespace:: units

Quantities
==========

A :term:`quantity` is a concrete amount of a unit for a specified dimension
with a specific representation and is represented in the library with a
`quantity` class template.


Construction
------------

To create the quantity object from a :term:`scalable number` we just have to pass
the value to the `quantity` class template explicit constructor::

    quantity<si::dim_length, si::kilometre, double> d(123);
    quantity<si::dim_speed, si::kilometre_per_hour, int> v(70);

.. note::

    As the constructor is explicit, the quantity object can be created from
    an "unsafe" fundamental type only via
    `direct initialization <https://en.cppreference.com/w/cpp/language/direct_initialization>`_.
    This is why the code below using
    `copy initialization <https://en.cppreference.com/w/cpp/language/copy_initialization>`_
    **does not compile**::

        quantity<si::dim_length, si::kilometre, double> d = 123;  // ERROR

Dimension-Specific Aliases
++++++++++++++++++++++++++

To simplify `quantity` objects creation the library provides helper aliases for
quantities of each :term:`dimension` which additionally set the representation
type to ``double`` by default::

    namespace si {

    template<Unit U, Representation Rep = double>
    using length = quantity<dim_length, U, Rep>;

    template<Unit U, Representation Rep = double>
    using speed = quantity<dim_speed, U, Rep>;

    }

Thanks to that, the above example can be rewritten as follows::

    si::length<si::kilometre> d(123);
    si::speed<si::kilometre_per_hour, int> v(70);

User Defined Literals
+++++++++++++++++++++

To further simplify construction of quantities with compile-time known
values the library provides :abbr:`UDL (User Defined Literal)` s for each
:term:`unit` of every :term:`dimension`::

    inline namespace literals {

    constexpr auto operator"" _q_km(unsigned long long l) { return length<kilometre, std::int64_t>(l); }
    constexpr auto operator"" _q_km(long double l) { return length<kilometre, long double>(l); }

    constexpr auto operator"" _q_km_per_h(unsigned long long l) { return speed<kilometre_per_hour, std::int64_t>(l); }
    constexpr auto operator"" _q_km_per_h(long double l) { return speed<kilometre_per_hour, long double>(l); }

    }

Thanks to them the same code can be as simple as::

    using namespace units::isq::si::literals;
    auto d = 123._q_km;     // si::length<si::kilometre, long double>
    auto v = 70_q_km_per_h; // si::speed<si::kilometre_per_hour, std::int64_t>

.. note::

    ``123._q_km`` should be read as a quantity of length in kilometers. Initially the
    library did not use the ``_q_`` prefix for UDLs but it turned out that there are
    a few unit symbols that collide with literals already existing in C and C++
    language (i.e. ``F`` (farad), ``J`` (joule), ``W`` (watt), ``K`` (kelvin),
    ``d`` (day), ``l`` or ``L`` (litre), ``erg``, ``ergps``). This is why the
    ``_q_`` prefix was consistently applied to all the UDLs.


Quantity References
+++++++++++++++++++

Quantity References provide an alternative way to simplify quantities creation.
They are defined using the `reference` class template::

    namespace references {

    inline constexpr auto km = reference<dim_length, kilometre>{};
    inline constexpr auto h = reference<dim_time, hour>{};

    }

With the above our code can look as follows::

    using namespace units::isq::si::references;
    auto d = 123. * km;     // si::length<si::kilometre, double>
    auto v = 70 * (km / h);   // si::speed<si::kilometre_per_hour, int>

.. important::

    The following syntaxes are not allowed:
    ``2 / s``, ``km * 3``, ``s / 4``, ``70 * km / h``.

It is also allowed to easily define custom quantity references from existing ones::

    inline constexpr auto Nm = N * m;
    inline constexpr auto km_per_h = km / h;
    inline constexpr auto mph = mi / h;

UDLs vs Quantity References
+++++++++++++++++++++++++++

UDLs are helpful but they also have some disadvantages compared to Quantity References:

1. UDLs are only for compile-time known values and do not work for runtime variables

   - UDLs::

       using namespace units::isq::si::literals;
       auto v1 = 120_q_km / 2_q_h;
       auto v2 = length<kilometre>(distance) / time<hour>(duration);

   - Quantity References::

       using namespace units::isq::si::references;
       auto v1 = 120 * km / (2 * h);
       auto v2 = distance * (1 * km) / (duration * (1 * h));

   References treat both cases in a unified way. It is also worth to notice that we work
   mostly with runtime variables and compile-time known values mostly appear only in physical
   constants and unit tests.

2. UDLs cannot be disambiguated with a namespace name

   - UDLs::

       using namespace units::isq::si::literals;
       using namespace units::isq::si::cgs::literals;
       auto d = 1_q_cm;   // FAILS TO COMPILE

   - Quantity References::

       inline constexpr auto si_cm = units::isq::si::references::cm;
       inline constexpr auto cgs_cm = units::isq::si::cgs::references::cm;

       auto d1 = 1. * si_cm;   // si::length<si::centimetre>
       auto d2 = 1. * cgs_cm;  // si::cgs::length<si::centimetre>

3. Poor control over the representation types as UDLs return only ``std::int64_t`` or
   ``long double``

   - UDLs::

       using namespace units::isq::si::literals;
       auto d1 = 123._q_km;   // si::length<si::kilometre, long double>
       auto d2 = 123_q_km;    // si::length<si::kilometre, std::int64_t>

     No possibility to obtain any other representation type.

   - Quantity References::

       using namespace units::isq::si::references;
       auto d1 = 123. * km;   // si::length<si::kilometre, double>
       auto d2 = 123 * km;    // si::length<si::kilometre, int>
       auto d3 = 123.f * km;  // si::length<si::kilometre, float>
       auto d4 = 123.L * km;  // si::length<si::kilometre, long double>
       auto d5 = 123ul * km;  // si::length<si::kilometre, unsigned long>
       auto d6 = 123ll * km;  // si::length<si::kilometre, long long>

4. UDLs are verbose to define and standardize

   - UDLs:
     
     - for each unit an integral and a floating-point UDL have to be defined
     - have to be provided for unnamed derived units (i.e. ``_q_km_per_h``)
    
   - Quantity References:
   
     - one reference per unit
     - unnamed derived units constructed from base references (i.e. ``km / h``)

5. Typical UDL definition for quantities when compiled with a ``-Wsign-conversion``
   flag results in a compilation warning. This warning could be silenced with a
   ``static_cast<std::int64_t>(value)`` in every UDL, but in a such case other safety
   and security issues could be silently introduced.
   Quantity References, on the opposite, always use the exact representation type provided
   by the user so there is no chance for a truncating conversion on a quantity construction.


Dimension-specific Concepts
---------------------------

In case the user does not care about the specific unit and representation but
requires quantity of a concrete dimension than dimension-specific concepts can
be used::

    using namespace units::isq::si::literals;
    constexpr Length auto d = 123_q_km;  // si::length<si::kilometre, std::int64_t>

.. note::

    All instances of `quantity` class always match the `Quantity` concept.
    All other regular types that are not quantities are called
    :term:`scalable numbers <scalable number>` by the library and match the
    `Representation` concept.

However, the above is not the most important usage of those concepts. Let's
assume that the user wants to implement an ``avg_speed`` function that will
be calculating the average speed based on provided distance and duration
quantities. The usage of such a function can look as follows::

    using namespace units::isq::si::literals;
    using namespace units::isq::si::international::literals;
    constexpr Speed auto v1 = avg_speed(220_q_km, 2_q_h);
    constexpr Speed auto v2 = avg_speed(140_q_mi, 2_q_h);

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

    Please refer to :ref:`examples/avg_speed:avg_speed` example for more
    information on different kinds of interfaces supported by the library.


Working With Constrained Deduced Quantity Types
-----------------------------------------------

It is important to note that when we assign a result from the function to an
automatically deduced type, even if it is constrained by a dimension-specific
concept, we still do not know what is the exact unit and representation type
of such a quantity. In many cases it might be exactly what we want to get,
but often we would like to know a specific type too. We have two options here:

- query the actual dimension, unit, and representation types::

    constexpr Speed auto v = avg_speed(220_q_km, 2_q_h);
    using quantity_type = decltype(v);
    using dimension_type = quantity_type::dimension;
    using unit_type = quantity_type::unit;
    using rep_type = quantity_type::rep;

- convert or cast to a desired quantity type::

    constexpr Speed auto v1 = avg_speed(220._q_km, 2_q_h);
    constexpr si::speed<si::metre_per_second> v2 = v1;
    constexpr Speed auto v3 = quantity_cast<si::speed<si::metre_per_second>(v1);

.. seealso::

    More information on this subject can be found in the
    :ref:`framework/conversions_and_casting:Conversions and Casting` chapter.


Dimensionless Quantities
------------------------

Whenever we divide two quantities of the same dimension we end up with a
:term:`dimensionless quantity` otherwise known as :term:`quantity of dimension one`::

    static_assert(10_q_km / 5_q_km == 2);
    static_assert(std::is_same_v<decltype(10_q_km / 5_q_km), quantity<dim_one, one, std::int64_t>>);

According to the official ISO definition `dim_one` is a dimension "for which all the
exponents of the factors corresponding to the base quantities in its quantity dimension
are zero".

.. seealso::

    Reasoning for the above design is provided in
    :ref:`faq:Why a dimensionless quantity is not just an fundamental arithmetic type?`

To simplify the usage of the dimensionless quantity a following concept and alias template
are provided::

    template<typename T>
    concept Dimensionless = QuantityOf<T, dim_one>;

    template<Unit U, Representation Rep = double>
    using dimensionless = quantity<dim_one, U, Rep>;

There are two special units provided for usage with such a quantity:

- `one` which is the :term:`coherent derived unit` of dimensionless quantity and does not
  provide any textual symbol (according to the ISO definition "the measurement units and
  values of quantities of dimension one are numbers"),
- `percent` which has the symbol ``%`` and ``ratio(1, 100)`` of the `one` unit.

For example the following code::

    std::cout << quantity_cast<percent>(50._q_m / 100._q_m) << '\n';

will print ``50 %`` to the console output.

Again, according to the ISO definition "such quantities convey more information than a
number". This is exactly what we observe in the above example. The value stored inside
the quantity, the text output, and the value returned by the `quantity::number()` member
function is ``50`` rather than ``0.5``. It means that dimensionless quantities behave
like all other quantities and store the value in terms of a ratio of a coherent unit.
This allows us to not loose precision when we divide quantities of the same dimensions
but with units having vastly different ratios, e.g.
`Dimensionless Hubble parameter <https://en.wikipedia.org/wiki/Hubble%27s_law#Dimensionless_Hubble_parameter>`_
is expressed as a ratio of kilometers and megaparsecs.

.. seealso::

    More information on dimensionless quantities can be found in the
    :ref:`framework/conversions_and_casting:Implicit conversions of dimensionless quantities`
    chapter.
