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

    struct desk : named_scaled_unit<desk, "desk", no_prefix, ratio<3, 10>, si::square_metre> {};

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
to change in the above code is to replace `no_prefix` with `si_prefix`::

    struct desk : named_scaled_unit<desk, "desk", si::prefix, ratio<3, 10>, si::square_metre> {};

Now I can define a new unit named ``kilodesk``::

    struct kilodesk : prefixed_unit<kilodesk, si::kilo, desk> {};
    static_assert(3_d * 1000 == si::area<kilodesk>(3));

But maybe SI prefixes are not good for me. Maybe I always pack ``6`` desks into one package
for shipment and ``40`` such packages fit into my lorry. To express this with prefixes a new
prefix family and prefixes are needed::

    struct shipping_prefix : prefix_family {};

    struct package : prefix<package, shipping_prefix, "pkg", ratio<6>> {};
    struct lorry : prefix<lorry, shipping_prefix, "lorry", ratio<6 * 40>> {};

Now we can use it for our unit::

    struct desk : named_scaled_unit<desk, "desk", shipping_prefix, ratio<3, 10>, si::square_metre> {};
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
                                             exp<si::dim_area, 1>, exp<si::dim_time, -1>> {};

    // our unit of interest for a new derived dimension
    struct desk_per_hour : deduced_unit<desk_per_hour, dim_desk_rate, desk, si::hour> {};

    // a quantity of our dimension
    template<Unit U, Scalar Rep = double>
    using desk_rate = quantity<dim_desk_rate, U, Rep>;

    // a concept matching the above quantity
    template<typename T>
    concept DeskRate = QuantityOf<T, dim_desk_rate>;

With the above we can now check what is the production rate::

    DeskRate auto rate = quantity_cast<desk_per_hour>(3._d / 20q_min);
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

    template<Unit U, Scalar Rep = double>
    using people = quantity<dim_people, U, Rep>;

    template<typename T>
    concept People = QuantityOf<T, dim_people>;

    constexpr auto operator"" _p(unsigned long long l) { return people<person, std::int64_t>(l); }
    constexpr auto operator"" _p(long double l) { return people<person, long double>(l); }


With the above we can now define a new derived dimension::

    struct person_per_square_metre : unit<person_per_square_metre> {};
    struct dim_occupancy_rate : derived_dimension<dim_occupancy_rate, person_per_square_metre,
                                                  exp<dim_people, 1>, exp<si::dim_area, -1>> {};

    struct person_per_desk : deduced_unit<person_per_desk, dim_occupancy_rate, person, desk> {};

    template<Unit U, Scalar Rep = double>
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


.. seealso::

    More information on extending the library can be found in the
    :ref:`Using Custom Representation Types` chapter.
