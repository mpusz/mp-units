.. namespace:: units

The International System of Units (SI)
======================================

The :term:`SI` system is defined in the [SIBrochure]_ and standardizes units for quantities provided in
the :term:`ISQ` system:

    The SI is a consistent system of units for use in all aspects of life, including international
    trade, manufacturing, security, health and safety, protection of the environment, and in the
    basic science that underpins all of these. The system of quantities underlying the SI and the
    equations relating them are based on the present description of nature and are familiar to all
    scientists, technologists and engineers.

As the :term:`SI` is defined on top of the :term:`ISQ` the C++ namespace for all of its definitions
is ``units::isq::si``.

Base units
----------

Even though from 2019 the :term:`SI` system is defined in terms of the `Defining constants`_,
base units still are really important. As the [SIBrochure]_ states:

    Prior to the definitions adopted in 2018, the SI was defined through seven base units from
    which the derived units were constructed as products of powers of the base units. Defining
    the SI by fixing the numerical values of seven defining constants has the effect that this
    distinction is, in principle, not needed, since all units, base as well as derived units, may be
    constructed directly from the defining constants. Nevertheless, the concept of base and
    derived units is maintained because it is useful and historically well established, noting
    also that the ISO/IEC 80000 series of Standards specify base and derived quantities which
    necessarily correspond to the SI base and derived units defined here.

The base units and quantities of the :term:`SI` system are defined as follows::

    namespace units::isq::si {

    struct second : named_unit<second, "s"> {};
    struct dim_time : isq::dim_time<second> {};
    template<UnitOf<dim_time> U, Representation Rep = double>
    using time = quantity<dim_time, U, Rep>;

    struct metre : named_unit<metre, "m"> {};
    struct dim_length : isq::dim_length<metre> {};
    template<UnitOf<dim_length> U, Representation Rep = double>
    using length = quantity<dim_length, U, Rep>;

    struct gram : named_unit<gram, "g"> {};
    struct kilogram : prefixed_unit<kilogram, kilo, gram> {};
    struct dim_mass : isq::dim_mass<kilogram> {};
    template<UnitOf<dim_mass> U, Representation Rep = double>
    using mass = quantity<dim_mass, U, Rep>;

    struct ampere : named_unit<ampere, "A"> {};
    struct dim_electric_current : isq::dim_electric_current<ampere> {};
    template<UnitOf<dim_electric_current> U, Representation Rep = double>
    using electric_current = quantity<dim_electric_current, U, Rep>;

    struct kelvin : named_unit<kelvin, "K"> {};
    struct dim_thermodynamic_temperature : isq::dim_thermodynamic_temperature<kelvin> {};
    template<UnitOf<dim_thermodynamic_temperature> U, Representation Rep = double>
    using thermodynamic_temperature = quantity<dim_thermodynamic_temperature, U, Rep>;

    struct mole : named_unit<mole, "mol"> {};
    struct dim_amount_of_substance : isq::dim_amount_of_substance<mole> {};
    template<UnitOf<dim_amount_of_substance> U, Representation Rep = double>
    using amount_of_substance = quantity<dim_amount_of_substance, U, Rep>;

    struct candela : named_unit<candela, "cd"> {};
    struct dim_luminous_intensity : isq::dim_luminous_intensity<candela> {};
    template<UnitOf<dim_luminous_intensity> U, Representation Rep = double>
    using luminous_intensity = quantity<dim_luminous_intensity, U, Rep>;

    }


Derived units
-------------

The [SIBrochure]_ states:

    Derived units are defined as products of powers of the base units. When the numerical
    factor of this product is one, the derived units are called coherent derived units. The base
    and coherent derived units of the SI form a coherent set, designated the set of coherent SI
    units. The word “coherent” here means that equations between the numerical values of
    quantities take exactly the same form as the equations between the quantities themselves.
    Some of the coherent derived units in the SI are given special names. ... Together with the
    seven base units they form the core of the set of SI units. All other SI units are combinations
    of some of these 29 units.


Unit prefixes
-------------

According to [SIBrochure]_:

    Prefixes may be used with any of the 29 SI units with special names with
    the exception of the base unit kilogram.

Here is a complete list of all the :term:`SI` prefixes supported by the library::

    namespace si {

    struct yocto  : prefix<yocto, "y",             mag_power<10, -24>()> {};
    struct zepto  : prefix<zepto, "z",             mag_power<10, -21>()> {};
    struct atto   : prefix<atto,  "a",             mag_power<10, -18>()> {};
    struct femto  : prefix<femto, "f",             mag_power<10, -15>()> {};
    struct pico   : prefix<pico,  "p",             mag_power<10, -12>()> {};
    struct nano   : prefix<nano,  "n",             mag_power<10, -9>()> {};
    struct micro  : prefix<micro, basic_symbol_text{"\u00b5", "u"},
                                                   mag_power<10, -6>()> {};
    struct milli  : prefix<milli, "m",             mag_power<10, -3>()> {};
    struct centi  : prefix<centi, "c",             mag_power<10, -2>()> {};
    struct deci   : prefix<deci,  "d",             mag_power<10, -1>()> {};
    struct deca   : prefix<deca,  "da",            mag_power<10, 1>()> {};
    struct hecto  : prefix<hecto, "h",             mag_power<10, 2>()> {};
    struct kilo   : prefix<kilo,  "k",             mag_power<10, 3>()> {};
    struct mega   : prefix<mega,  "M",             mag_power<10, 6>()> {};
    struct giga   : prefix<giga,  "G",             mag_power<10, 9>()> {};
    struct tera   : prefix<tera,  "T",             mag_power<10, 12>()> {};
    struct peta   : prefix<peta,  "P",             mag_power<10, 15>()> {};
    struct exa    : prefix<exa,   "E",             mag_power<10, 18>()> {};
    struct zetta  : prefix<zetta, "Z",             mag_power<10, 21>()> {};
    struct yotta  : prefix<yotta, "Y",             mag_power<10, 24>()> {};

    }


Other definitions for names units
---------------------------------

For all of the above units the library also provides:

- prefixed versions using SI prefixes::

    namespace units::isq::si {

    struct millisecond : prefixed_unit<millisecond, milli, second> {};

    }

- :ref:`framework/quantities:Quantity References (Experimental)`::

    namespace units::isq::si {

    namespace time_references {

    inline constexpr auto s = reference<dim_time, second>{};

    }

    namespace references {

    using namespace time_references;

    }

- :ref:`framework/quantities:Unit-Specific Aliases (Experimental)`::

    namespace units::aliases::isq::si::inline time {

    template<Representation Rep = double>
    using s = units::isq::si::time<units::isq::si::second, Rep>;

    }

- :ref:`framework/quantities:User Defined Literals (Experimental)`::

    namespace units::isq::si::inline literals {

    constexpr auto operator"" _q_s(unsigned long long l)
    {
      gsl_ExpectsAudit(std::in_range<std::int64_t>(l));
      return time<second, std::int64_t>(static_cast<std::int64_t>(l));
    }
    constexpr auto operator"" _q_s(long double l) { return time<second, long double>(l); }

    }

For some of the units, when accepted by the :term:`SI`, other non-standard scaled versions are also provided::

    namespace units::isq::si {

    struct minute : named_scaled_unit<minute, "min", mag<60>(), second> {};
    struct hour : named_scaled_unit<hour, "h", mag<60>(), minute> {};
    struct day : named_scaled_unit<day, "d", mag<24>(), hour> {};

    }




Defining constants
------------------

[SIBrochure]_ states that:

    The definition of the SI units is established in terms of a set of seven defining constants.
    The complete system of units can be derived from the fixed values of these defining
    constants, expressed in the units of the SI.

Those constants are provided in the *units/isq/si/constants.h* header file as::

    namespace units::isq::si::si2019 {

    template<Representation Rep = double>
    inline constexpr auto hyperfine_structure_transition_frequency = frequency<hertz, Rep>(Rep{9'192'631'770});

    template<Representation Rep = double>
    inline constexpr auto speed_of_light = speed<metre_per_second, Rep>(299'792'458);

    template<Representation Rep = double>
    inline constexpr auto planck_constant = energy<joule, Rep>(6.62607015e-34) * time<second, Rep>(1);

    template<Representation Rep = double>
    inline constexpr auto elementary_charge = electric_charge<coulomb, Rep>(1.602176634e-19);

    template<Representation Rep = double>
    inline constexpr auto boltzmann_constant = energy<joule, Rep>(1.380649e-23) / thermodynamic_temperature<kelvin, Rep>(1);

    template<Representation Rep = double>
    inline constexpr auto avogadro_constant = Rep(6.02214076e23) / amount_of_substance<mole, Rep>(1);

    template<Representation Rep = double>
    inline constexpr auto luminous_efficacy = luminous_flux<lumen, Rep>(683) / power<watt, Rep>(1);

    }

.. note::

    Please note the nested `si2019` namespace. It is introduced in case those constants were changed/updated
    by the :term:`SI` in the future.
