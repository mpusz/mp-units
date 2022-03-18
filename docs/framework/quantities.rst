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


Quantity Construction Helpers
-----------------------------

.. important::

    Currently the library provides multiple experimental helpers to instantiate
    quantities of different dimensions and units. Users are encourages to try them
    out, vote, and share feedback in this
    `discussion on GitHub <https://github.com/mpusz/units/discussions/274>`_.

    Most probably only one of the options will be included in the final product so
    please do not hesitate to vote on the one that suits you the best.

Dimension-Specific Aliases (Experimental)
+++++++++++++++++++++++++++++++++++++++++

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

Unit-Specific Aliases (Experimental)
++++++++++++++++++++++++++++++++++++

Additionally to the dimension-specific aliases there are also ones provided for
each and every :term:`unit` in the library::

    #ifndef UNITS_NO_ALIASES

    namespace units::aliases::isq::si::inline length {

    template<Representation Rep = double> using m = units::isq::si::length<units::isq::si::metre, Rep>;
    template<Representation Rep = double> using km = units::isq::si::length<units::isq::si::kilometre, Rep>;

    }

    namespace units::aliases::isq::si::inline speed {

    template<Representation Rep = double> using m_per_s = units::isq::si::speed<units::isq::si::metre_per_second, Rep>;
    template<Representation Rep = double> using km_per_h = units::isq::si::speed<units::isq::si::kilometre_per_hour, Rep>;

    }

    #endif // UNITS_NO_ALIASES

Using the above our code can look as follows::

    using namespace units::aliases::isq;
    si::length::km<> d(123);
    si::speed::km_per_h<int> v(70);

Please note that with the C++20 :abbr:`CTAD (Class Template Argument Deduction)` support
for alias templates the above can be rewritten as::

    using namespace units::aliases::isq;
    si::length::km d(123.);
    si::speed::km_per_h v(70);

which will deduce the representation type automatically from the initializer provided
by the user.

Also, this feature allows to be more terse if desired::

    using namespace units::aliases::isq::si;
    auto d = km(123.);
    auto v = km_per_h(70);

Quantity References (Experimental)
++++++++++++++++++++++++++++++++++

Quantity References provide an alternative and simplified way to create quantities.
They are defined using the `reference` class template::

    #ifndef UNITS_NO_REFERENCES

    namespace length_references {

    inline constexpr auto km = reference<dim_length, kilometre>{};

    }  // namespace length_references

    namespace time_references {

    inline constexpr auto h = reference<dim_time, hour>{};

    }  // namespace time_references

    namespace references {

    using namespace length_references;
    using namespace time_references;

    }  // namespace references

    #endif // UNITS_NO_REFERENCES

With the above our code can look as follows::

    using namespace units::isq::si::references;
    auto d = 123. * km;       // si::length<si::kilometre, double>
    auto v = 70 * (km / h);   // si::speed<si::kilometre_per_hour, int>

.. important::

    The following syntaxes are not allowed:
    ``2 / s``, ``km * 3``, ``s / 4``, ``70 * km / h``.

It is also possible to easily define custom quantity references from existing ones::

    inline constexpr auto Nm = N * m;
    inline constexpr auto km_per_h = km / h;
    inline constexpr auto mph = mi / h;


User Defined Literals (Experimental)
++++++++++++++++++++++++++++++++++++

Alternatively, to construct quantities with compile-time known values the library provides
:abbr:`UDL (User Defined Literal)` s for each :term:`unit` of every :term:`dimension`::

    #ifndef UNITS_NO_LITERALS

    inline namespace literals {

    constexpr auto operator"" _q_km(unsigned long long l) { return length<kilometre, std::int64_t>(l); }
    constexpr auto operator"" _q_km(long double l) { return length<kilometre, long double>(l); }

    constexpr auto operator"" _q_km_per_h(unsigned long long l) { return speed<kilometre_per_hour, std::int64_t>(l); }
    constexpr auto operator"" _q_km_per_h(long double l) { return speed<kilometre_per_hour, long double>(l); }

    }

    #endif // UNITS_NO_LITERALS

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

     No possibility to obtain any other representation type. Additionally this gets contagious
     as the result of every arithmetic expression on quantities is always expanded to the common
     type of its arguments. For example `si::length<si::metre, int>(1) + 1_q_m` results in a
     `si::length<si::metre, int64_t>` type.

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
     - unnamed derived units are constructed from base references so no explicit
       definition is required (i.e. ``km / h``)

5. Typical UDL definition for quantities when compiled with a ``-Wsign-conversion``
   flag results in a compilation warning. This warning could be silenced with a
   ``static_cast<std::int64_t>(value)`` in every UDL, but in a such case other safety
   and security issues could be silently introduced.
   Quantity References, on the opposite, always use the exact representation type provided
   by the user so there is no chance for a truncating conversion on a quantity construction.

6. UDLs take long to compile

   - UDLs:

     Every unit requires two UDLs to be defined which in turns requires two instantiations
     of "heavy" `quantity` class template. Those are then not often used by non-UDL construction
     as most users instantiate `quantity` class template with `int` or `double` which
     again have to be separately instantiated. This has a significant impact on the compile-time
     performance.

   - Quantity References:

     `reference` class template is "cheaper" to instantiate. Additionally, every unit requires
     only one instantiation of a `reference` class template. Such pre-defined reference instance
     is then shared among all the instantiations of `quantity` class template for this specific
     unit (no matter of its representation type). With this approach we end up with much less class
     template instantiations in the application.


Quantity References vs Unit-specific Aliases
++++++++++++++++++++++++++++++++++++++++++++

1. Shadowing issues

   - Quantity References

     References occupy a pool of many short identifiers which sometimes shadow the variables,
     function arguments, or even template parameters provided by the user or other libraries. This
     results in warnings being generated by some compilers. The most restrictive here is MSVC which
     for example emits a warning of shadowing ``N`` template parameter for an array size provided
     in a header file with Newton unit included via namespace declaration in the ``main()`` program
     function (see `experimental_angle <https://github.com/mpusz/units/blob/master/example/references/experimental_angle.cpp>`_).
     In other cases user is forced to rename its local identifiers to not collide with predefined
     references (see `capacitor_time_curve <https://github.com/mpusz/units/blob/master/example/references/capacitor_time_curve.cpp>`_).

   - Unit-specific Aliases

     As aliases are defined in terms of types rather variables no major shadowing issues were found
     so far. In case of identifiers abiguity it was always possible to disambiguate with more
     namespaces prefixed in front of the alias.

2. Adjustable verbosity

   - Quantity References

     References allow creating custom helpers for complex units. Instead of typing::

        static_assert(2 * km / (2 * (km / h)) == 1 * h);

     one can do the following::

        inline constexpr auto kmph = km / h;
        static_assert(2 * km / (2 * kmph) == 1 * h);

   - Unit-specific Aliases

     There is no need to create custom helpers for complex units as most of them are predefined in
     a library already. However, this feature also allows controlling verbosity of the code. For
     example in the below example ``d1``, ``d2``, and ``d3`` will end up being of the same type
     and having the same value::

        auto d1 = m(123.45);
        double a = 123.45;
        auto d2 = m(a);
        auto d3 = length::m(a);

3. Readability

   - Quantity References

     As long as references are easy to understand in the following code::

        auto d = 123 * m;

     it is not that nice when a variable is used instead of a compile time number::

        constexpr Speed auto avg_speed(double d, double t)
        {
          using namespace units::isq::si::length_references;
          using namespace units::isq::si::time_references;
          return d * m / (t * s);
        }

     Notice that if ``using namespace units::isq::si::references;`` was used instead above it could
     cause a clash of ``t`` function parameter with ``si::tonne`` unit symbol if ``si/mass.h`` was
     included.

   - Unit-specific Aliases

     The same using aliases can look as follows::

        constexpr Speed auto avg_speed(double d, double t)
        {
          using namespace units::aliases::isq::si;
          return m(d) / s(t);
        }

     or::

        constexpr Speed auto avg_speed(double d, double t)
        {
          using namespace units::aliases::isq::si;
          return length::m(d) / time::s(t);
        }

4. Operators Precedence

   - Quantity References

     The syntax for references uses ``*`` operator which has some predefined precedence. This operator
     always takes a magnitude or a reference as ``lhs`` and a reference as ``rhs``. All other comibnations
     are not allowed. It means that in order to satisfy the operators precedence sometimes quite a lot
     of parenthesis have to be sprinkled in the code in order for the code to compile::

        static_assert(2 * km / (2 * (km / h)) == 1 * h);

   - Unit-specific Aliases

     Aliases do not use operator syntax thus they are not affected by the precedence issue.

5. Composition for unnamed derived units

   - Quantity References

     References have only to be defined for named units. Also for the user's conveniance references are
     predefined for units raised to a specific power (e.g. ``m2``, ``km3``, etc). All other derived units
     can be constructed using the provided ones already even if they do not correspond to any predefined
     dimension::

        inline constexpr auto kmph = km / h;
        inline constexpr auto kmph3 = kmph * kmph * kmph;

   - Unit-specific Aliases

     Such a feature is not possible with aliases. In order to create a derived unit a full alias template
     has to be explicitly provided::

       template<Representation Rep = double> using km_per_h = units::isq::si::speed<units::isq::si::kilometre_per_hour, Rep>;

6. Explicit control over the representation type

   Both options here allow to preserve user provide representation type but only aliases allow
   to override it if needed.

7. Simplified quantity casting

   Aliases can easily replace ``quantity_cast<Unit>()`` which is not possible with references::

       constexpr auto meter = 1 * m;
       std::cout << " = " << quantity_cast<si::international::foot>(meter) << '\n';

   The above code for references may look as follows::

       constexpr auto meter = m(1);
       std::cout << " = " << international::ft(meter) << '\n';
       std::cout << " = " << ft(meter) << '\n';

   The above will preserve the representation type of the source type.

8. Compilation performance

   For our experiments it seems that aliases are 2-5x faster to compile than references (declaring an
   alias template is much faster than instantiating a class template).


Summary
+++++++

+-----------------------------------------------+-------------+------------+---------------+
|                    Feature                    |   Aliases   | References |     UDLs      |
+===============================================+=============+============+===============+
| Literals and variables support                | **Yes**     | **Yes**    | Literals only |
+-----------------------------------------------+-------------+------------+---------------+
| Preserves user provided representation type   | **Yes**     | **Yes**    | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Explicit control over the representation type | **Yes**     | No         | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Possibility to resolve ambiguity              | **Yes**     | **Yes**    | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Readability                                   | **Good**    | Medium     | **Good**      |
+-----------------------------------------------+-------------+------------+---------------+
| Hard to resolve shadowing issues              | **No**      | Yes        | **No**        |
+-----------------------------------------------+-------------+------------+---------------+
| Operators precedence issue                    | **No**      | Yes        | **No**        |
+-----------------------------------------------+-------------+------------+---------------+
| Controlled verbosity                          | **Yes**     | No         | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Easy composition for derived units            | No          | **Yes**    | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Simplified quantity casting                   | **Yes**     | No         | No            |
+-----------------------------------------------+-------------+------------+---------------+
| Implementation and standardization effort     | Medium      | **Lowest** | Highest       |
+-----------------------------------------------+-------------+------------+---------------+
| Compile-time performance                      | **Fastest** | Medium     | Slowest       |
+-----------------------------------------------+-------------+------------+---------------+


Don't pay for what you don't use (compile-time performance)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

As noted in the previous chapter, each quantity creation helper has a different impact on the compile-time
performance. Aliases tend to be the fastest to compile but even their definition can be expensive for some
if it is not used in the source code. This is why it is possible to opt-out from each or every quantity
creation helper with the following preprocessor defines::

    #define UNITS_NO_ALIASES
    #define UNITS_NO_REFERENCES
    #define UNITS_NO_LITERALS


Dimension-specific Concepts
---------------------------

In case the user does not care about the specific unit and representation but
requires quantity of a concrete dimension than dimension-specific concepts can
be used::

    using namespace units::isq::si::references;
    constexpr Length auto d = 123 * km;  // si::length<si::kilometre, int>

.. note::

    All instances of `quantity` class always match the `Quantity` concept.
    All other regular types that are not quantities are called
    :term:`scalable numbers <scalable number>` by the library and match the
    `Representation` concept.

However, the above is not the most important usage of those concepts. Let's
assume that the user wants to implement an ``avg_speed`` function that will
be calculating the average speed based on provided distance and duration
quantities. The usage of such a function can look as follows::

    using namespace units::isq::si::references;
    using namespace units::isq::si::international::references;
    constexpr Speed auto v1 = avg_speed(220 * km, 2 * h);
    constexpr Speed auto v2 = avg_speed(140 * mi, 2 * h);

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

    Please refer to :ref:`examples/basics/avg_speed:avg_speed` example for more
    information on different kinds of interfaces supported by the library.


Working With Constrained Deduced Quantity Types
-----------------------------------------------

It is important to note that when we assign a result from the function to an
automatically deduced type, even if it is constrained by a dimension-specific
concept, we still do not know what is the exact unit and representation type
of such a quantity. In many cases it might be exactly what we want to get,
but often we would like to know a specific type too. We have two options here:

- query the actual dimension, unit, and representation types::

    constexpr Speed auto v = avg_speed(220 * km, 2 * h);
    using quantity_type = decltype(v);
    using dimension_type = quantity_type::dimension;
    using unit_type = quantity_type::unit;
    using rep_type = quantity_type::rep;

- convert or cast to a desired quantity type::

    constexpr Speed auto v1 = avg_speed(220. * km, 2 * h);
    constexpr si::speed<si::metre_per_second> v2 = v1;
    constexpr Speed auto v3 = quantity_cast<si::speed<si::metre_per_second>(v1);

.. seealso::

    More information on this subject can be found in the
    :ref:`framework/conversions_and_casting:Conversions and Casting` chapter.


Dimensionless Quantities
------------------------

Whenever we divide two quantities of the same dimension we end up with a
:term:`dimensionless quantity` otherwise known as :term:`quantity of dimension one`::

    static_assert(10 * km / (5 * km) == 2);
    static_assert(std::is_same_v<decltype(10 * km / (5 * km)), quantity<dim_one, one, int>>);

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

    std::cout << quantity_cast<percent>(50. * m / (100. * m)) << '\n';

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
