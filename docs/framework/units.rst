.. namespace:: units

Units
=====

Each quantity has a magnitude (a numerical value). In order to be able to
compare quantities of the same dimension a notion of a :term:`measurement unit`
was introduced. Units are designated by conventionally assigned names and
symbols. Thanks to them it is possible to compare two quantities of the
same dimension and express the ratio of the second quantity to the first
one as a number. For example ``10 s`` is ``10`` times more than ``1 s``.

Base quantities are expressed in terms of :term:`base units <base unit>`
(i.e. ``m`` (meter), ``s`` (second)), while derived quantities are expressed
in terms of :term:`derived units <derived unit>`.


Class Hierarchy
---------------

All of the described here class templates to produce unit types inherit from some instance
of a `scaled_unit` class template:

.. raw:: html

    <object data="../_images/units.svg" type="image/svg+xml" class="align-center" style="max-width: 100%;"></object>

..
    https://www.planttext.com

    @startuml

    skinparam monochrome true
    skinparam shadowing false
    skinparam backgroundColor #fcfcfc

    hide members
    hide circle

    left to right direction

    package Unit <<Frame>> [[../../framework/units.html]] {

    abstract scaled_unit<UnitRatio, Unit>

    abstract prefixed_alias_unit<Unit, Prefix, AliasUnit> [[../../framework/units.html#aliased-units]]
    abstract alias_unit<Unit, Symbol> [[../../framework/units.html#aliased-units]]
    abstract derived_scaled_unit<Dimension, Unit, Unit...> [[../../framework/units.html#derived-scaled-units]]
    abstract derived_unit [[../../framework/units.html#derived-unnamed-units]]
    abstract prefixed_unit<Prefix, Unit> [[../../framework/units.html#prefixed-unit]]
    abstract named_scaled_unit<Symbol, Ratio, Unit> [[../../framework/units.html#named-scaled-units]]
    abstract named_unit<Symbol> [[../../framework/units.html#base-units]]

    scaled_unit <|-- named_unit
    scaled_unit <|-- named_scaled_unit
    scaled_unit <|-- prefixed_unit
    scaled_unit <|-- derived_unit
    scaled_unit <|-- derived_scaled_unit
    scaled_unit <|-- alias_unit
    scaled_unit <|-- prefixed_alias_unit
    }

    @enduml

`scaled_unit` is a class template used exclusively by the library's framework
and user should not instantiate it by him/her-self. However the user can sometimes
observe this type in case an unit/dimension conversion expression will end up with an
unknown/undefined unit type like in the below example::

    using namespace units::isq::si::references;

    Length auto l = 100 * (km / h) * (10 * s);

The type of ``l`` above will be
``si::length<scaled_unit<ratio(1, 36, 1), si::metre>, long double>``. This is caused
by the fact that the library does not define a unit of a length quantity that has the
ratio ``10/36`` of a ``si::metre``. If such a unit was predefined we would see its concrete
type here instead.


Base Units
----------

:term:`Base units <base unit>` are the units of
:term:`base quantities <base quantity>` defined for
:term:`base dimensions <base dimension>`. For example in :term:`SI`
``m`` (meter) is a base unit of length, ``s`` (second) is a base unit of
time. In each :term:`coherent system of units`, there is only one base
unit for each base quantity. This is why a base unit type is required by
the `base_dimension` definition in this library. For example ``si::dim_length``
can be defined in the following way::

    namespace si {

    struct dim_length : base_dimension<"L", metre> {};

    }

where ``si::metre`` is defined as::

    namespace si {

    struct metre : named_unit<metre, "m", prefix> {};

    }

In the above definition ``"m"`` is the unit symbol to be used in the text
output, and ``si::prefix`` specifies that the library should allow
definitions of prefixed units using ``si::metre`` as a reference (i.e.
``si::centimetre``).

.. seealso::

    For more information on prefixes and scaling please refer to
    `Scaled Units`_.

.. note::

    The first template argument of `named_unit` is the type of the
    child class inherited from the instantiation of this `named_unit`
    class template. This is called a
    :abbr:`CRTP (Curiously Recurring Template Parameter)` Idiom and is used
    in many places in this library to provide
    :ref:`design/downcasting:The Downcasting Facility`.


It is important to notice here that :term:`SI` is not the only system used
in the industry and the library has to support other systems too. Also
in some cases conversions between such systems should be allowed. Thanks to
the fact that the `base_dimension` takes the base unit type in its definition
allows us to easily define various systems of units for the same physical
dimension. For example length in the
`CGS <https://en.wikipedia.org/wiki/Centimetre%E2%80%93gram%E2%80%93second_system_of_units>`_
could be defined as::

    namespace si::cgs {

    struct dim_length : base_dimension<"L", si::centimetre> {};

    }

The fact that both base dimensions use the same identifier ``"L"`` tells
the library that both definitions refer to the same physical dimension of
length. The only difference is the measurement unit used to define their
base dimensions. Thanks to using the unit that is defined in terms of the
the same reference unit as the one provided to ``si::dim_length`` definition
(namely ``si::centimetre`` which is ``1/100`` of ``si::metre``) we also enabled
the ability to easily convert between those 2 base dimensions (as the library
knows how to convert ``si::metre`` to ``si::centimetre`` and vice versa).

.. seealso::

    More details on custom systems definitions and conversions between
    units of the same physical dimension can be found in the
    :ref:`use_cases/extensions:Custom Systems` chapter.


Scaled Units
------------

Described above base units (in case of base quantities) and
:term:`coherent units <coherent derived unit>` (in case of derived quantities),
in their system of units, have proportionality factor/ratio equal to one.
However, quantities of such dimensions can also use units of measurement
with other ratios to describe their magnitude (numerical value).


Named Scaled Units
^^^^^^^^^^^^^^^^^^

We are used to use minutes, hours, or days to measure quantities of time.
Those units are the scaled versions of a time dimension's base unit,
namely second. Those can be defined easily in the library using
`named_scaled_unit` class template::

    struct minute : named_scaled_unit<minute, "min", ratio(60), second> {};
    struct hour : named_scaled_unit<hour, "h", ratio(60), minute> {};
    struct day : named_scaled_unit<day, "d", ratio(24), hour> {};

The `ratio` type used in the definition is really similar to ``std::ratio`` but it takes
an additional ``Exponent`` template parameter that defines the exponent of the ratio.
Another important difference is the fact that the objects of that class are used
as class NTTPs rather then a type template parameter kind.

Thanks to it we can address nearly infinite scaling factors between units
and define units like::

    struct electronvolt : named_scaled_unit<electronvolt, "eV", prefix,
                                            ratio(1'602'176'634, 1'000'000'000, -19), joule> {};

Finally, the last of the `named_scaled_unit` class template parameters
provide a reference unit for scaling. Please note that it can be a dimension's
base/coherent unit (like ``si::second``) or any other unit (i.e. ``si::minute``,
``si::hour``) that is a scaled version of the dimension's base/coherent unit.


Prefixed Unit
^^^^^^^^^^^^^

Prefixed units are just scaled units with a standardized ratio. For example
:term:`SI` defines prefixes based on the exponent of ``10``. Here is a
complete list of all the :term:`SI` prefixes supported by the library::

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

Alternative hierarchy of prefixes is the one used in data information
domain::

    namespace iec80000 {

    struct kibi : prefix<kibi, "Ki", mag_power<2, 10>()> {};
    struct mebi : prefix<mebi, "Mi", mag_power<2, 20>()> {};
    struct gibi : prefix<gibi, "Gi", mag_power<2, 30>()> {};
    struct tebi : prefix<tebi, "Ti", mag_power<2, 40>()> {};
    struct pebi : prefix<pebi, "Pi", mag_power<2, 50>()> {};
    struct exbi : prefix<exbi, "Ei", mag_power<2, 60>()> {};
    struct zebi : prefix<zebi, "Zi", mag_power<2, 70>()> {};
    struct yobi : prefix<yobi, "Yi", mag_power<2, 80>()> {};

    }

With the definitions like above we can easily define prefixed unit. For
example we can define ``si::kilometre`` as::

    namespace si {

    struct kilometre : prefixed_unit<kilometre, kilo, metre> {};

    }

.. important::

    Prefixed units have to use named units as a reference. For unnamed
    units we could end up with some strange, misleading, and sometimes
    wrong definitions ("kilo square metre" seams strange and spelled
    as ``km²`` would be invalid).


Derived Units
-------------

:term:`Derived units <derived unit>` are the units used to measure
:term:`derived quantities <derived quantity>`. They can either have their own unique
names (i.e. ``N`` (newton)) or can be composed from the names of units of quantities
used to define thier derived quantity (i.e. ``km/h``).


Derived Named Units
^^^^^^^^^^^^^^^^^^^

Derived named units have a unique symbol (i.e. ``N`` (newton) or ``Pa``
(pascal)) and they are defined in the same way as base units (which
always have to be a named unit)::

    namespace si {

    struct newton : named_unit<newton, "N", prefix> {};

    }


Derived Unnamed Units
^^^^^^^^^^^^^^^^^^^^^

Derived unnamed units are the units where the symbol is derived from the
base quantities symbols and the expression of the dependence of the derived
quantity on the base quantities (i.e. ``m/s`` (metre per second), ``m²``
(square metre)). To support such use cases a library introduced a notion of
:term:`derived dimension recipe` which stores the information about the
order, exponents, and types of dimensions used to define this particular
derived dimension. For example each of the below ``momentum`` definitions
will result in a different unnamed unit symbol:

.. code-block::
    :emphasize-lines: 2-4, 6-8, 10-12

    struct dim_momentum : derived_dimension<dim_momentum, kilogram_metre_per_second,
                                            exponent<si::dim_mass, 1>,
                                            exponent<si::dim_length, 1>,
                                            exponent<si::dim_time, -1>> {};    // kg⋅m/s
    struct dim_momentum : derived_dimension<dim_momentum, kilogram_metre_per_second,
                                            exponent<si::dim_length, 1>,
                                            exponent<si::dim_mass, 1>,
                                            exponent<si::dim_time, -1>> {};    // m⋅kg/s
    struct dim_momentum : derived_dimension<dim_momentum, kilogram_metre_per_second,
                                            exponent<si::dim_time, -1>,
                                            exponent<si::dim_length, 1>,
                                            exponent<si::dim_mass, 1>> {};     // 1/s⋅m⋅kg

where ``kilogram_metre_per_second`` is defined as::

    struct kilogram_metre_per_second : derived_unit<kilogram_metre_per_second> {};

However, the easiest way to define momentum is just to use the
``si::dim_speed`` derived dimension in the recipe:

.. code-block::
    :emphasize-lines: 3

    struct dim_momentum : derived_dimension<dim_momentum, kilogram_metre_per_second,
                                            exponent<si::dim_mass, 1>,
                                            exponent<si::dim_speed, 1>> {}; // kg⋅m/s

In such a case the library will do its magic and will automatically
unpack a provided derived dimension to its base dimensions in order to
end up with a :term:`normalized derived dimension` for a parent entity.

The need to support a derived dimension in the recipe is not just a
syntactic sugar that allows us to do less typing. It is worth to notice
here that some of the derived unnamed units are defined in terms of other
derived named units (i.e. surface tension quantity is measured in terms
of ``N/m``):

.. code-block::
    :emphasize-lines: 2

    struct dim_surface_tension : derived_dimension<dim_surface_tension, newton_per_metre,
                                                   exponent<si::dim_force, 1>,
                                                   exponent<si::dim_length, -1>> {}; // N/m

If we defined the above in terms of base units we would end up with
a ``kg/s²`` derived unit symbol.


Derived Scaled Units
^^^^^^^^^^^^^^^^^^^^

For some units determining of a correct scaling ratio may not be trivial,
and even if done correctly, may be a pain to maintain. For a simple example
let's take a "kilometre per hour" unit. What is the easiest to maintain
ratio in reference to the "metre per second":

- ``1000/3600``
- ``10/36``
- ``5/18``

Whichever, we choose there will always be someone not happy with our choice.

Thanks to a `derived_scaled_unit` class template provided by the library this problem
does not exist at all. With it ``si::kilometre_per_hour`` can be defined as::

    namespace si {

    struct kilometre_per_hour : derived_scaled_unit<kilometre_per_hour, dim_speed, kilometre, hour> {};

    }

In case the scaled derived unit should serve as a named one we can use
a `named_derived_unit` where the user is able to provide a symbol for the unit
by him/her-self::

    namespace si::fps {

    struct knot : named_derived_unit<knot, dim_speed, "knot", nautical_mile, hour> {};

    }

Please note that the dervided scaled units are the only unit-related class templates
that take a dimension as its parameter. This derived dimension provides a :term:`recipe`
used for its definition. Based on the information stored in the recipe
(order, type, and exponents of composite dimensions) and the ratios of units
provided in the template parameter list after the derived dimension parameter,
the library calculates the final ratio for this unit.


Aliased Units
-------------

In order to make our life easier people tend to assign alternative/aliased names
to some popular units. As an example we often use "tonne" instead of "megagram",
"litre" instead of "cubic decimetre", or "hectare" instead of "square hectometre".

This library provides facilities to define aliased names to already defined units
with `alias_unit` class template::

    namespace si {

    struct litre : alias_unit<cubic_decimetre, "l", prefix> {};

    }

Also, it is possible to add prefixes to such aliased units with `prefixed_alias_unit`
class template::

    namespace si {

    struct millilitre : prefixed_alias_unit<cubic_centimetre, milli, litre> {};

    }


.. seealso::

    To learn more about unknown units please refer to the
    :ref:`use_cases/unknown_dimensions:Working with Unknown Dimensions and Their Units` chapter.
