# Glossary

## ISO definitions

!!! note

    The ISO terms provided below are only a few of many defined in
    the [ISO/IEC Guide 99](https://www.iso.org/obp/ui#iso:std:iso-iec:guide:99:ed-1:v2:en).

[`quantity`](#quantity){ #quantity }

:   - Property of a phenomenon, body, or substance, where the property has a magnitude that can
      be expressed by means of a number and a reference.
    - A reference can be a [measurement unit](#unit), a measurement procedure, a reference material,
      or a combination of such.
    - A quantity as defined here is a scalar. However, a vector or a tensor, the components of
      which are quantities, is also considered to be a quantity.
    - The concept ’quantity’ may be generically divided into, e.g. ‘physical quantity’,
      ‘chemical quantity’, and ‘biological quantity’, or [‘base quantity’](#base-quantity)
      and [‘derived quantity’](#derived-quantity).
    - Examples of quantities are: length, radius, wavelength, energy, electric charge, etc.

[`kind of quantity, kind`](#kind){ #kind }

:   - Aspect common to mutually comparable [quantities](#quantity).
    - The division of the concept ‘quantity’ into several kinds is to some extent arbitrary, for example:
        - the quantities diameter, circumference, and wavelength are generally considered
          to be quantities of the same kind, namely, of the kind of quantity called length,
        - the quantities heat, kinetic energy, and potential energy are generally considered
          to be quantities of the same kind, namely of the kind of quantity called energy.
    - Quantities of the same kind within a given [system of quantities](#system-of-quantities)
      have the same [quantity dimension](#dimension). However, [quantities](#quantity)
      of the same [dimension](#dimension) are not necessarily of the same kind.
        - For example, the quantities moment of force and energy are, by convention, not regarded
          as being of the same kind, although they have the same dimension. Similarly for
          heat capacity and entropy, as well as for number of entities, relative permeability,
          and mass fraction.

[`system of quantities`](#system-of-quantities){ #system-of-quantities }

:   - Set of [quantities](#quantity) together with a set of non-contradictory equations
      relating those [quantities](#quantity).
    - Examples of systems of quantities are: [the International System of Quantities](#isq),
      the Imperial System, etc.

[`base quantity`](#base-quantity){ #base-quantity }

:   - [Quantity](#quantity) in a conventionally chosen subset of a given
      [system of quantities](#system-of-quantities), where no [quantity](#quantity) in the
      subset can be expressed in terms of the others.
    - Base quantities are referred to as being mutually independent since a base quantity
      cannot be expressed as a product of powers of the other base quantities.
    - ‘Number of entities’ can be regarded as a base quantity in any
      [system of quantities](#system-of-quantities).

[`derived quantity`](#derived-quantity){ #derived-quantity }

:   - [Quantity](#quantity), in a [system of quantities](#system-of-quantities), defined in
      terms of the [base quantities](#base-quantity) of that system.

[`International System of Quantities, ISQ`](#isq){ #isq }

:   - [System of quantities](#system-of-quantities) based on the seven [base quantities](#base-quantity):
      length, mass, time, electric current, thermodynamic temperature, amount of substance,
      and luminous intensity.
    - This system of quantities is published in the ISO 80000 and IEC 80000 series _Quantities and units_.
    - [The International System of Units (SI)](#si) is based on the ISQ.

[`quantity dimension, dimension of a quantity, dimension`](#dimension){ #dimension }

:   - Expression of the dependence of a [quantity](#quantity) on the [base quantities](#base-quantity)
      of a [system of quantities](#system-of-quantities) as a product of powers of factors corresponding
      to the [base quantities](#base-quantity), omitting any numerical factor.
        - i.e. in the [ISQ](#isq), the quantity dimension of force is denoted by $\textsf{dim }F = \mathsf{LMT}^{–2}$.
    - A power of a factor is the factor raised to an exponent. Each factor is the dimension
      of a [base quantity](#base-quantity).
    - In deriving the dimension of a quantity, no account is taken of its scalar, vector, or
      tensor character.
    - In a given [system of quantities](#system-of-quantities):
        - [quantities](#quantity) of the same [kind](#kind) have the same quantity dimension,
        - [quantities](#quantity) of different quantity dimensions are always of different [kinds](#kind),
        - [quantities](#quantity) having the same quantity dimension are not necessarily of the same
          [kind](#kind).
    - Symbols representing the dimensions of the [base quantities](#base-quantity) in the [ISQ](#isq) are:

        | Base quantity             | Symbol for dimension |
        |---------------------------|:--------------------:|
        | length                    |     $\mathsf{L}$     |
        | mass                      |     $\mathsf{M}$     |
        | time                      |     $\mathsf{T}$     |
        | electric current          |     $\mathsf{I}$     |
        | thermodynamic temperature |     $\mathsf{Θ}$     |
        | amount of substance       |     $\mathsf{N}$     |
        | luminous intensity        |     $\mathsf{J}$     |

        Thus, the dimension of a quantity $Q$ is denoted by
        $\textsf{dim }Q = \mathsf{L}^α\mathsf{M}^β\mathsf{T}^γ\mathsf{I}^δ\mathsf{Θ}^ε\mathsf{N}^ζ\mathsf{J}^η$
        where the exponents, named dimensional exponents, are positive, negative, or zero.

[`quantity of dimension one, dimensionless quantity`](#dimensionless-quantity){ #dimensionless-quantity }

:     - [quantity](#quantity) for which all the exponents of the factors corresponding to the
        [base quantities](#base-quantity) in its [quantity dimension](#dimension) are zero.
      - The term “dimensionless quantity” is commonly used and is kept here for historical
        reasons. It stems from the fact that all exponents are zero in the symbolic
        representation of the [dimension](#dimension) for such [quantities](#quantity).
        The term “quantity of dimension one” reflects the convention in which the symbolic
        representation of the [dimension](#dimension) for such [quantities](#quantity) is
        the symbol `1`.
      - The [measurement units](#unit) and [values](#quantity-value) of quantities of
        dimension one are numbers, but such quantities convey more information than a number.
      - Some quantities of dimension one are defined as the ratios of two
        [quantities of the same kind](#kind).
      - Numbers of entities are quantities of dimension one.

[`measurement unit, unit of measurement, unit`](#unit){ #unit }

:   - Real scalar [quantity](#quantity), defined and adopted by convention, with which any other
      [quantity of the same kind](#kind) can be compared to express the ratio of the two
      [quantities](#quantity) as a number.
    - Measurement units are designated by conventionally assigned names and symbols.
    - Measurement units of [quantities](#quantity) of the same [quantity dimension](#dimension)
      may be designated by the same name and symbol even when the [quantities](#quantity) are
      not of the same `kind`.
        - For example, joule per kelvin and J/K are respectively the name and symbol of both a
          measurement unit of heat capacity and a measurement unit of entropy, which are generally
          not considered to be [quantities of the same kind](#kind). However, in some cases special
          measurement unit names are restricted to be used with [quantities](#quantity) of specific
          [kind](#kind) only. For example, the measurement unit ‘second to the power minus one’
          (1/s) is called hertz (Hz) when used for frequencies and becquerel (Bq) when used for
          activities of radionuclides. As another example, the joule (J) is used as a unit of
          energy, but never as a unit of moment of force, i.e. the newton metre (N·m).
    - Measurement units of [quantities of dimension one](#dimensionless-quantity) are
      numbers. In some cases, these measurement units are given special names, e.g. radian,
      steradian, and decibel, or are expressed by quotients such as millimole per mole equal
      to $10^{−3}$ and microgram per kilogram equal to $10^{−9}$.

[`base unit`](#base-unit){ #base-unit }

:   - [Measurement unit](#unit) that is adopted by convention for a [base quantity](#base-quantity).
    - In each [coherent system of units](#coherent-system-of-units), there is only one base unit
      for each [base quantity](#base-quantity).
        - i.e. in the [SI](#si), the metre is the base unit of length. In the CGS systems,
          the centimetre is the base unit of length.
    - A base unit may also serve for a [derived quantity](#derived-quantity) of the same
      [quantity dimension](#dimension).
    - For number of entities, the number one, symbol `1`, can be regarded as a base unit in
      any system of units.

[`derived unit`](#derived-unit){ #derived-unit }

:   - [Measurement unit](#unit) for a [derived quantity](#derived-quantity).
    - For example, the metre per second, symbol m/s, and the centimetre per second, symbol cm/s,
      are derived units of speed in the [SI](#si). The kilometre per hour, symbol km/h, is a
      [measurement unit](#unit) of speed outside the [SI](#si) but accepted for use with
      the [SI](#si). The knot, equal to one nautical mile per hour, is a measurement unit of speed
      outside the [SI](#si).

[`coherent derived unit`](#coherent-derived-unit){ #coherent-derived-unit }

:   - [Derived unit](#derived-unit) that, for a given [system of quantities](#system-of-quantities)
      and for a chosen set of [base units](#base-unit), is a product of powers of
      [base units](#base-unit) with no other proportionality factor than one.
    - A power of a [base unit](#base-unit) is the [base unit](#base-unit) raised to an exponent.
    - Coherence can be determined only with respect to a particular
      [system of quantities](#system-of-quantities) and a given set of [base units](#base-unit).
        - For example, if the metre, the second, and the mole are base units, the metre per second is
          the coherent derived unit of velocity when velocity is defined by the
          [quantity equation](#quantity-equation) $v = \mathsf{d}r/\mathsf{d}t$, and the mole per
          cubic metre is the coherent derived unit of amount-of-substance concentration when
          amount-of-substance concentration is defined by the [quantity equation](#quantity-equation)
          $c = n/V$. The kilometre per hour and the knot, given as examples of [derived units](#derived-unit),
          are not coherent derived units in such a [system of quantities](#system-of-quantities).
    - A [derived unit](#derived-unit) can be coherent with respect to one
      [system of quantities](#system-of-quantities) but not to another.
        - For example, the centimetre per second is the coherent derived unit of speed in a CGS system
          of units but is not a coherent derived unit in the [SI](#si).
    - The coherent derived unit for every [derived quantity of dimension one](#dimensionless-quantity)
      in a given [system of units](#system-of-units) is the number one, symbol `1`. The name and
      symbol of the [measurement unit](#unit) one are generally not indicated.

[`system of units`](#system-of-units){ #system-of-units }

:   - Set of [base units](#base-unit) and [derived units](#derived-unit), together with
      their multiples and submultiples, defined in accordance with given rules, for a given
      [system of quantities](#system-of-quantities).

[`coherent system of units`](#coherent-system-of-units){ #coherent-system-of-units }

:   - [System of units](#system-of-units), based on a given [system of quantities](#system-of-quantities),
      in which the [measurement unit](#unit) for each [derived quantity](#derived-quantity) is
      a [coherent derived unit](#coherent-derived-unit).
    - A [system of units](#system-of-units) can be coherent only with respect to a
      [system of quantities](#system-of-quantities) and the adopted [base units](#base-unit).
    - For a coherent system of units, [numerical value equations](#numerical-value-equation) have
      the same form, including numerical factors, as the corresponding
      [quantity equations](#quantity-equation).

[`off-system measurement unit, off-system unit`](#off-system-unit){ #off-system-unit }

:   - [Measurement unit](#unit) that does not belong to a given [system of units](#system-of-units).
    - For example, the electronvolt (about $1.602\;18 × 10^{–19} \mathsf{J}$) is an
      off-system measurement unit of energy with respect to the [SI](#si). Day, hour, minute
      are off-system measurement units of time with respect to the [SI](#si).

[`International System of Units, SI`](#si){ #si }

:   - [System of units](#system-of-units), based on the [International System of Quantities](#isq),
      their names and symbols, including a series of prefixes and their names and symbols,
      together with rules for their use, adopted by the General Conference on Weights and
      Measures (CGPM).

[`quantity value, value of a quantity, value`](#quantity-value){ #quantity-value }

:   - Number and reference together expressing magnitude of a [quantity](#quantity).
    - The number can be complex.
    - A quantity value can be presented in more than one way.
    - In the case of vector or tensor quantities, each component has a quantity value.
        - For example, force acting on a given particle, e.g. in Cartesian components
          $(F_x; F_y; F_z) = (−31.5; 43.2; 17.0) \mathsf{N}$.

[`quantity equation`](#quantity-equation){ #quantity-equation }

:   - Mathematical relation between [quantities](#quantity) in a given [system of quantities](#system-of-quantities),
      independent of [measure­ment units](#unit).
    - For example, $T = (1/2) mv^2$ where $T$ is the kinetic energy and $v$ the speed
      of a specified particle of mass $m$.

[`unit equation`](#unit-equation){ #unit-equation }

:   - Mathematical relation between [base units](#base-unit),
      [coher­ent derived units](#coherent-derived-unit) or other [measurement units](#unit).
    - For example, $\mathsf{J} := \mathsf{kg}\:\mathsf{m}^2/\mathsf{s}^2$, where, $\mathsf{J}$,
      $\mathsf{kg}$, $\mathsf{m}$, and $\mathsf{s}$ are the symbols for the joule, kilogram,
      metre, and second, respectively. (The symbol $:=$ denotes “is by definition equal to”
      as given in the ISO 80000 and IEC 80000 series.). $1\;\mathsf{km/h} = (1/3.6)\;\mathsf{m/s}$.

[`numerical value equation, numerical quantity value equation`](#numerical-value-equation){ #numerical-value-equation }

:   - Mathematical relation between numerical [quantity values](#quantity-value), based on
      a given [quantity equation](#quantity-equation) and specified [measurement units](#unit).
    - For example, in the [quantity equation](#quantity-equation) for kinetic energy of a particle,
      $T = (1/2) mv^2$, if $m = 2 kg$ and $v = 3 m/s$, then ${T} = (1/2) × 2 × 3^2$ is a numerical
      value equation giving the numerical value $9$ of $T$ in joules.

## Other definitions

!!! info

    The below terms are describing the implementation-related part of the **mp-units** library.

[`base dimension`](#base-dimension){ #base-dimension }

:   - A [dimension](#dimension) of a [base quantity](#base-quantity).

[`derived dimension`](#derived-dimension){ #derived-dimension }

:   - A [dimension](#dimension) of a [derived quantity](#derived-quantity).
    - Implemented as an expression template being the result of the
      [dimension equation](#dimension-equation) on [base dimensions](#base-dimension).

[`dimension equation`](#dimension-equation){ #dimension-equation }

:   - Mathematical relation between [dimensions](#dimension) in a given
      [system of quantities](#system-of-quantities), independent of [measure­ment units](#unit).

[`quantity kind hierarchy, quantity hierarchy`](#quantity-hierarchy){ #quantity-hierarchy }

:   - [Quantities of the same kind](#kind) form a hierarchy that determines their:
        - convertibility (i.e. every width is a length, but width should not be
          convertible to height)
        - common quantity type (i.e. width + height -> length)

[`quantity character, character of a quantity, character`](#character){ #character }

:   - Scalars, vectors and tensors are mathematical objects that can be used to denote
      certain [physical quantities](#quantity) and their [values](#quantity-value).
      They are as such independent of the particular choice of a coordinate system,
      whereas each scalar component of a vector or a tensor and each component vector
      and component tensor depend on that choice.
    - A vector is a tensor of the first order and a scalar is a tensor of order zero.
    - For vectors and tensors, the components are [quantities](#quantity) that can be
      expressed as a product of a number and a [unit](#unit).
    - Vectors and tensors can also be expressed as a numerical value vector or tensor,
      respectively, multiplied by a [unit](#unit).
    - [Quantities](#quantity) of different characters support different set of operations.
        - For example, a [quantity](#quantity) can be multiplied by another one only if any
          of them has scalar character. Vectors and tensors can't be multiplied or divided,
          but they support additional operations like dot and cross products, which
          are not available for scalars.
    - The term ’character’ was borrowed from the below quote:

    !!! quote "ISO 80000-1_2009"

        In deriving the dimension of a quantity, no account is taken of its scalar,
        vector, or tensor **character**.

[`quantity specification, quantity_spec`](#quantity_spec){ #quantity_spec }

:   - An entity storing all the information about a specific [quantity](#quantity):
        - location in a [quantity hierarchy](#quantity-hierarchy)
        - [quantity equation](#quantity-equation)
        - [dimension of a quantity](#dimension)
        - [quantity kind](#kind)
        - [quantity character](#character)
        - additional constraints (i.e. non-negative)
    - [Dimension of a quantity](#dimension) is not enough to specify all the properties of
      a [quantity](#quantity).
