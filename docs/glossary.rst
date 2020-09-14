.. default-role:: term

Glossary
========

ISO 80000 [1]_ definitions
--------------------------

.. glossary::

    quantity
      - Property of a phenomenon, body, or substance, where the property has a magnitude that can
        be expressed by means of a number and a reference.
      - A reference can be a measurement unit, a measurement procedure, a reference material, or
        a combination of such.
      - A quantity as defined here is a scalar. However, a vector or a tensor, the components of
        which are quantities, is also considered to be a quantity.
      - The concept ’quantity’ may be generically divided into, e.g. ‘physical quantity’,
        ‘chemical quantity’, and ‘biological quantity’, or ‘base quantity’ and ‘derived quantity’.
      - Examples of quantities are: mass, length, density, magnetic field strength, etc.

    kind of quantity
    kind
      - Aspect common to mutually comparable `quantities <quantity>`.
      - The division of the concept ‘quantity’ into several kinds is to some extent arbitrary

        - i.e. the quantities diameter, circumference, and wavelength are generally considered
          to be quantities of the same kind, namely, of the kind of quantity called length.

      - Quantities of the same kind within a given `system of quantities` have the same quantity
        `dimension`. However, `quantities <quantity>` of the same `dimension` are not necessarily
        of the same kind.

        - For example, the absorbed dose and the dose equivalent have the same `dimension`.
          However, the former measures the absolute amount of radiation one receives whereas
          the latter is a weighted measurement taking into account the kind of radiation
          on was exposed to.

    system of quantities
    system
      - Set of `quantities <quantity>` together with a set of non-contradictory equations
        relating those `quantities <quantity>`.
      - Examples of systems of quantities are: the International System of Quantities,
        the Imperial System, etc.

    base quantity
      - `Quantity <quantity>` in a conventionally chosen subset of a given `system of quantities`,
        where no `quantity` in the subset can be expressed in terms of the other
        `quantities <quantity>` within that subset.
      - Base quantities are referred to as being mutually independent since a base quantity
        cannot be expressed as a product of powers of the other base quantities.

    derived quantity
      - `Quantity <quantity>`, in a `system of quantities`, defined in terms of the base
        quantities of that system.

    International System of Quantities
    ISQ
      - `System of quantities <system of quantities>` based on the seven
        `base quantities <base quantity>`: length, mass, time, electric current, thermodynamic
        temperature, amount of substance, and luminous intensity.
      - The International System of Units (SI) is based on the ISQ.

    dimension of a quantity
    quantity dimension
    dimension
      - Expression of the dependence of a `quantity` on the `base quantities <base quantity>`
        of a `system of quantities` as a product of powers of factors corresponding to the
        `base quantities <base quantity>`, omitting any numerical factors.
      - A power of a factor is the factor raised to an exponent. Each factor is the dimension
        of a `base quantity`.
      - In deriving the dimension of a quantity, no account is taken of its scalar, vector, or
        tensor character.
      - In a given `system of quantities`:

        - `quantities <quantity>` of the same `kind` have the same quantity dimension,
        - `quantities <quantity>` of different quantity dimensions are always of different `kinds <kind>`,
        - `quantities <quantity>` having the same quantity dimension are not necessarily of the same `kind`.

    quantity of dimension one
    dimensionless quantity
      - `quantity` for which all the exponents of the factors corresponding to the
        `base quantities <base quantity>` in its `quantity dimension` are zero.
      - The term “dimensionless quantity” is commonly used and is kept here for historical
        reasons. It stems from the fact that all exponents are zero in the symbolic
        representation of the `dimension` for such `quantities <quantity>`. The term “quantity
        of dimension one” reflects the convention in which the symbolic representation of the
        `dimension` for such `quantities <quantity>` is the symbol ``1``. This `dimension` is
        not a number, but the neutral element for multiplication of `dimensions <dimension>`.
      - The `measurement units <measurement unit>` and values of quantities of dimension one
        are numbers, but such `quantities <quantity>` convey more information than a number.
      - Some quantities of dimension one are defined as the ratios of two
        `quantities of the same kind <kind>`. The `coherent derived unit` is the number one,
        symbol ``1``.
      - Numbers of entities are quantities of dimension one.

    unit of measurement
    measurement unit
    unit
      - Real scalar `quantity`, defined and adopted by convention, with which any other
        `quantity of the same kind <kind>` can be compared to express the ratio of the
        second `quantity` to the first one as a number.
      - Measurement units are designated by conventionally assigned names and symbols.
      - Measurement units of `quantities <quantity>` of the same `quantity dimension` may
        be designated by the same name and symbol even when the `quantities <quantity>` are
        not of the same `kind`.

        For example, joule per kelvin and J/K are respectively the name and symbol of both a
        measurement unit of heat capacity and a measurement unit of entropy, which are generally
        not considered to be `quantities of the same kind <kind>`. However, in some cases special
        measurement unit names are restricted to be used with `quantities <quantity>` of specific
        `kind` only. For example, the measurement unit ‘second to the power minus one’ (``1/s``) is
        called hertz (``Hz``) when used for frequencies and becquerel (``Bq``) when used for
        activities of radionuclides. As another example, the joule (``J``) is used as a unit of
        energy, but never as a unit of moment of force, i.e. the newton metre (``N · m``).
      - Measurement units of `quantities of dimension one <quantity of dimension one>` are
        numbers. In some cases, these measurement units are given special names, e.g. radian,
        steradian, and decibel, or are expressed by quotients such as millimole per mole equal
        to :math:`10^{−3}` and microgram per kilogram equal to :math:`10^{−9}`.

    base unit
      - Measurement unit that is adopted by convention for a `base quantity`.
      - In each `coherent system of units`, there is only one base unit for each `base quantity`.
      - A base unit may also serve for a `derived quantity` of the same `quantity dimension`.
      - For example, the `ISQ` has the base units of: metre, kilogram, second, Ampere, Kelvin, mole,
        and candela.

    derived unit
      - Measurement unit for a `derived quantity`.
      - For example, in the `ISQ` Newton, Pascal, and katal are derived units.

    coherent derived unit
      - `Derived unit <derived unit>` that, for a given `system of quantities` and for a chosen
        set of `base units <base unit>`, is a product of powers of `base units <base unit>` with
        no other proportionality factor than one.
      - A power of a `base unit` is the `base unit` raised to an exponent.
      - Coherence can be determined only with respect to a particular `system of quantities`
        and a given set of `base units <base unit>`. That is, if the metre and the second are
        base units, the metre per second is the coherent derived unit of speed.

    system of units
      - Set of `base units <base unit>` and `derived units <derived unit>`, together with
        their multiples and submultiples, defined in accordance with given rules, for a given
        `system of quantities`.

    coherent system of units

      - `System of units <system of units>`, based on a given `system of quantities`, in which
        the measurement unit for each `derived quantity` is a `coherent derived unit`.
      - A `system of units` can be coherent only with respect to a `system of quantities` and
        the adopted `base units <base unit>`.

    off-system measurement unit
    off-system unit
      - `Measurement unit <measurement unit>` that does not belong to a given `system of units`.
        For example, the electronvolt (:math:`≈ 1,602 18 × 10^{–19} J`) is an off-system measurement
        unit of energy with respect to the `SI` or day, hour, minute are off-system measurement
        units of time with respect to the `SI`.

    International System of Units
    SI
      - `System of units <system of units>`, based on the `International System of Quantities`,
        their names and symbols, including a series of prefixes and their names and symbols,
        together with rules for their use, adopted by the General Conference on Weights and
        Measures (CGPM)

    quantity value
    value of a quantity
    value
      - Number and reference together expressing magnitude of a `quantity`.
      - A quantity value can be presented in more than one way.


Other definitions
-----------------

.. glossary::
    :sorted:

    base dimension
      - A `dimension` of a `base quantity`.

    derived dimension
      - A `dimension` of a `derived quantity`.
      - Often implemented as a list of exponents of `base dimensions  <base dimension>`.

    normalized derived dimension
      A `derived dimension` in which:

      - `base dimensions <base dimension>` are not repeated in a list (each base dimension is provided at most once),
      - `base dimensions <base dimension>` are consistently ordered,
      - `base dimensions <base dimension>` having zero exponent are elided.

    derived dimension recipe
    recipe
      - The ordered list of exponents used to define a derived dimension
      - The list may contain both base and derived dimensions (in the latter case
        the dimension is being extracted to base dimensions by the framework)
      - The order and types of dimensions used in the recipe determine how an unnamed
        dimension's unit symbol is being printed in the text output

    scalable number
      - Not a `quantity`
      - Can be passed as a representation type to the :class:`units::quantity` type or be used as a factor
        while multiplying or dividing a `quantity`.

.. rubric:: Footnotes:

.. [1] [ISO80000]_ gives general information and definitions concerning quantities, systems of quantities,
    units, quantity and unit symbols, and coherent unit systems, especially the International System
    of Quantities, ISQ, and the International System of Units, SI. The principles laid down in
    ISO 80000-1:2009 are intended for general use within the various fields of science and technology
    and as an introduction to other parts of the Quantities and units series. Ordinal quantities and
    nominal properties are outside the scope of ISO 80000-1:2009.
