.. namespace:: units

Extending the Library
=====================

The library was designed with a simple extensibility in mind. It is easy to add new units,
dimensions, and prefixes. The systems of units are not closed (classes) but open (namespaces)
and can be easily extended, or its content can be partially/fully imported to other systems.


Custom Units
------------

It might happen that the user would like to use a unit that is not predefined by the library
or is predefined but the user would like to name it differently, assign a different symbol
to existing unit, or make it a base unit for prefixes.


Defining a New Unit
^^^^^^^^^^^^^^^^^^^

My working desk is of ``180 cm x 60 cm`` which gives an area of ``0.3 m²``. I would like to
make it a unit of area for my project::

    struct desk : named_scaled_unit<desk, "desk", no_prefix, ratio(3, 10), si::square_metre> {};

With the above I can define a quantity with the area of ``2 desks``::

    auto d1 = si::area<desk>(2);

In case I feel it is too verbose to type the above every time I can easily create a custom
alias or an :abbr:`UDL (User Defined Literal)`::

    // alias with fixed integral representation
    using desks = si::area<desk, std::int64_t>;

    // UDLs
    constexpr auto operator"" _d(unsigned long long l) { return si::area<desk, std::int64_t>(l); }
    constexpr auto operator"" _d(long double l)        { return si::area<desk, long double>(l); }

Right now I am fully set up for my project and can start my work of tracking the area taken
by my desks::

    auto d1 = si::area<desk>(2);
    auto d2 = desks(3);
    auto d3 = 1_d;
    auto sum = d1 + d2 + d3;
    std::cout << "Area: " << sum << '\n';  // prints 'Area: 6 desk'

In case I would like to check how much area ``6 desks`` take in SI units::

    auto sum_si = quantity_cast<si::square_metre>(sum);
    std::cout << "Area (SI): " << sum_si << '\n';  // prints 'Area (SI): 1.8 m²'


Enabling a Unit for Prefixing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case I decide it is reasonable to express my desks with SI prefixes the only thing I have
to change in the above code is to replace `no_prefix` with `isq::si::prefix`::

    struct desk : named_scaled_unit<desk, "desk", si::prefix, ratio(3, 10), si::square_metre> {};

Now I can define a new unit named ``kilodesk``::

    struct kilodesk : prefixed_unit<kilodesk, si::kilo, desk> {};
    static_assert(3_d * 1000 == si::area<kilodesk>(3));

But maybe SI prefixes are not good for me. Maybe I always pack ``6`` desks into one package
for shipment and ``40`` such packages fit into my lorry. To express this with prefixes a new
prefix family and prefixes are needed::

    struct shipping_prefix : prefix_family {};

    struct package : prefix<package, shipping_prefix, "pkg", ratio(6)> {};
    struct lorry : prefix<lorry, shipping_prefix, "lorry", ratio(6 * 40)> {};

Now we can use it for our unit::

    struct desk : named_scaled_unit<desk, "desk", shipping_prefix, ratio(3, 10), si::square_metre> {};
    struct packagedesk : prefixed_unit<packagedesk, package, desk> {};
    struct lorrydesk : prefixed_unit<lorrydesk, lorry, desk> {};

With the above::

    static_assert(6_d == si::area<packagedesk>(1));
    static_assert(240_d == si::area<lorrydesk>(1));
    std::cout << "Area: " << quantity_cast<packagedesk>(sum) << '\n'; // prints 'Area: 1 pkgdesk'

It is important to notice that with the definition of a custom prefix I did not loose SI
units compatibility. If I want to calculate how much area I can cover with desks delivered
by ``3 lorries`` I can do the following::

    auto area = quantity_cast<si::square_metre>(si::area<lorrydesk>(3));
    std::cout << "Area: " << area << '\n';  // prints 'Area: 216 m²'


Custom Dimensions
-----------------

There are cases were a custom unit is not enough and the user would like to define custom
dimensions. The most common case is to define a new derived dimension from other dimensions
already predefined in various systems. But in **mp-units** library it is also really easy to
define a new base dimension for a custom units system.

Custom Derived Dimensions
^^^^^^^^^^^^^^^^^^^^^^^^^

In case I want to track how many desks I can produce over time or what is the consumption
rate of wood during production I need to define a new derived dimension together with its
coherent unit::

    // coherent unit must apply to the system rules (in this case SI)
    struct square_metre_per_second : unit<square_metre_per_second> {};

    // new derived dimensions
    struct dim_desk_rate : derived_dimension<dim_desk_rate, square_metre_per_second,
                                             exponent<si::dim_area, 1>, exponent<si::dim_time, -1>> {};

    // our unit of interest for a new derived dimension
    struct desk_per_hour : deduced_unit<desk_per_hour, dim_desk_rate, desk, si::hour> {};

    // a quantity of our dimension
    template<UnitOf<dim_desk_rate> U, Representation Rep = double>
    using desk_rate = quantity<dim_desk_rate, U, Rep>;

    // a concept matching the above quantity
    template<typename T>
    concept DeskRate = QuantityOf<T, dim_desk_rate>;

With the above we can now check what is the production rate::

    DeskRate auto rate = quantity_cast<desk_per_hour>(3._d / 20_q_min);
    std::cout << "Desk rate: " << rate << '\n';  // prints 'Desk rate: 9 desk/h'

and how much wood is being consumed over a unit of time::

    auto wood_rate = quantity_cast<square_metre_per_second>(rate);
    std::cout << "Wood rate: " << wood_rate << '\n';  // prints 'Wood rate: 0.00075 m²/s'


Custom Base Dimensions
^^^^^^^^^^^^^^^^^^^^^^

In case I want to monitor what is the average number of people sitting by one desk in
a customer's office I would need a unit called ``person_per_desk`` of a new derived
dimension. However, our library does not know what a ``person`` is. For this I need to
define a new base dimension, its units, quantity helper, concept, and UDLs::

    struct person : named_unit<person, "person", no_prefix> {};
    struct dim_people : base_dimension<"people", person> {};

    template<UnitOf<dim_people> U, Representation Rep = double>
    using people = quantity<dim_people, U, Rep>;

    template<typename T>
    concept People = QuantityOf<T, dim_people>;

    constexpr auto operator"" _p(unsigned long long l) { return people<person, std::int64_t>(l); }
    constexpr auto operator"" _p(long double l) { return people<person, long double>(l); }


With the above we can now define a new derived dimension::

    struct person_per_square_metre : unit<person_per_square_metre> {};
    struct dim_occupancy_rate : derived_dimension<dim_occupancy_rate, person_per_square_metre,
                                                  exponent<dim_people, 1>,
                                                  exponent<si::dim_area, -1>> {};

    struct person_per_desk : deduced_unit<person_per_desk, dim_occupancy_rate, person, desk> {};

    template<UnitOf<dim_occupancy_rate> U, Representation Rep = double>
    using occupancy_rate = quantity<dim_occupancy_rate, U, Rep>;

    template<typename T>
    concept OccupancyRate = QuantityOf<T, dim_occupancy_rate>;

Now we can play with our new feature::

    People auto employees = 1450._p;
    auto office_desks = 967_d;
    OccupancyRate auto occupancy = employees / office_desks;

    std::cout << "Occupancy: " << occupancy << '\n';  // prints 'Occupancy: 1.49948 person/desk'


Custom Systems
--------------

Being able to extend predefined systems is a mandatory feature of any physical
units library. Fortunately, for **mp-units** there is nothing special to do here.

A system is defined in terms of its base dimensions. If you are using only SI
base dimensions then you are in the boundaries of the SI system. If you are
adding new base dimensions, like we did in the `Custom Base Dimensions`_
chapter, you are defining a new system.

In **mp-units** library a custom system can either be constructed from
unique/new custom base dimensions or reuse dimensions of other systems. This
allows extending, mixing, reuse, and interoperation between different systems.

Systems can be defined as standalone or provide interoperability and conversions
with other systems. It is up to the user to decide which one fits better to the
requirements.

A standalone system is the one that has unique :term:`base units <base unit>`.
Such units do not share their references with base units of other systems:

.. code-block::
    :emphasize-lines: 3, 6

    namespace fps {

    struct foot : named_unit<foot, "ft", no_prefix> {};
    struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio(3), foot> {};

    struct dim_length : base_dimension<"L", foot> {};

    template<UnitOf<dim_length> U, Representation Rep = double>
    using length = quantity<dim_length, U, Rep>;

    }  // namespace fps

If the base unit of one system has the same reference as the base unit of
another system, and both systems share the same base dimension symbol, the
library will enable conversions between units of those dimensions in
different systems:

.. code-block::
    :emphasize-lines: 3, 6, 13, 16

    namespace si {

    struct metre : named_unit<metre, "m", units::isq::si::prefix> {};
    struct kilometre : prefixed_unit<kilometre, units::isq::si::kilo, metre> {};

    struct dim_length : base_dimension<"L", metre> {};

    template<UnitOf<dim_length> U, Representation Rep = double>
    using length = quantity<dim_length, U, Rep>;

    namespace fps {

    struct foot : named_scaled_unit<foot, "ft", no_prefix, ratio(3'048, 1'000, -1), metre> {};
    struct yard : named_scaled_unit<yard, "yd", no_prefix, ratio(3), foot> {};

    struct dim_length : base_dimension<"L", foot> {};

    template<UnitOf<dim_length> U, Representation Rep = double>
    using length = quantity<dim_length, U, Rep>;

    }  // namespace fps
    }  // namespace si

Having the above two systems we can try the following code::

    constexpr auto fps_yard = fps::length<fps::yard>(1.);
    std::cout << quantity_cast<si::kilometre>(fps_yard) << "\n";     // compile-time error

    constexpr auto si_fps_yard = si::fps::length<si::fps::yard>(1.);
    std::cout << quantity_cast<si::kilometre>(si_fps_yard) << "\n";  // prints "0.0009144 km"

Even though the base dimension of ``si::fps`` is defined in terms of
``si::metre`` foot is preserved as the base unit of length in both systems::

    constexpr auto fps_yard = fps::length<fps::yard>(1.);
    constexpr auto fps_area = quantity_cast<unknown_coherent_unit>(fps_yard * fps_yard);
    std::cout << fps_yard << "\n";     // 1 yd
    std::cout << fps_area << "\n";     // 9 ft²

    constexpr auto si_fps_yard = si::fps::length<si::fps::yard>(1.);
    constexpr auto si_fps_area = quantity_cast<unknown_coherent_unit>(si_fps_yard * si_fps_yard);
    std::cout << si_fps_yard << "\n";  // 1 yd
    std::cout << si_fps_area << "\n";  // 9 ft²

In most cases we want conversions between systems and that is why nearly all
systems provided with this library are implemented in terms on the :term:`SI`
system. However, such an approach has also its problems. Let's see the
following simple application using the above defined systems::

    std::ostream& operator<<(std::ostream& os, const ratio& r)
    {
      return os << "ratio{" << r.num << ", " << r.den << ", " << r.exp << "}";
    }

    std::ostream& operator<<(std::ostream& os, const Unit auto& u)
    {
      using unit_type = std::remove_cvref_t<decltype(u)>;
      return os << unit_type::ratio << " x " << unit_type::reference::symbol.standard();
    }

    int main()
    {
      std::cout << "fps:     " << fps::yard() << "\n";      // fps:     ratio{3, 1, 0} x ft
      std::cout << "si::fps: " << si::fps::yard() << "\n";  // si::fps: ratio{1143, 125, -1} x m
    }

As we can see, even though ``si::fps::yard`` is defined as ``3`` feet,
the library always keeps the ratio relative to the primary reference unit
which in this case is ``si::metre``. This results in much bigger ratios
and in case of some units may result with a problem of limited resolution
of ``std::int64_t`` used to store numerator, denominator, and exponent
values of ratio. For example the ``si::fps::qubic_foot`` already has the
ratio of ``ratio{55306341, 1953125, -3}``. In case of more complicated
conversion ratio we can overflow `ratio` and get a compile-time error.
In such a situation the standalone system may be a better choice here.


.. seealso::

    More information on extending the library can be found in the
    :ref:`use_cases/custom_representation_types:Using Custom Representation Types`
    chapter.
