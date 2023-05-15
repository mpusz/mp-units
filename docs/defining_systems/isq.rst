.. namespace:: units

International System of Quantities (ISQ)
========================================

According to [ISO80000]_:

    The system of quantities, including the relations among them the quantities used as the basis of the units of
    the SI, is named the **International System of Quantities**, denoted "ISQ", in all languages.


Base dimensions and their symbols
---------------------------------

According to the [SIBrochure]_:

    Physical quantities can be organized in a system of dimensions, where the system used is
    decided by convention. Each of the seven base quantities used in the SI is regarded as
    having its own dimension. The symbols used for the base quantities and the symbols used
    to denote their dimension are shown in Table 3.

Following that the library provides the following definitions::

    namespace units::isq {

    template<Unit U> struct dim_time : base_dimension<"T", U> {};
    template<Unit U> struct dim_length : base_dimension<"L", U> {};
    template<Unit U> struct dim_mass : base_dimension<"M", U> {};
    template<Unit U> struct dim_electric_current : base_dimension<"I", U> {};
    template<Unit U> struct dim_thermodynamic_temperature : base_dimension<"Θ", U> {};
    template<Unit U> struct dim_amount_of_substance : base_dimension<"N", U> {};
    template<Unit U> struct dim_luminous_intensity : base_dimension<"J", U> {};

    }

Base dimension symbols provided above are consistently defined by both [SIBrochure]_ and [ISO80000]_.


Derived dimensions
------------------

[SIBrochure]_ states:

    Since the number of quantities is without limit, it is not possible to
    provide a complete list of derived quantities and derived units.

The authors of this library decided to limit the set of defined quantities to all
quantities defined in the [ISO80000]_ series of documents.
