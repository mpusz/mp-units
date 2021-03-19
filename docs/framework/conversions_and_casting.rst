.. namespace:: units

Conversions and Casting
=======================

  Programmers who are not full-time programming language geeks **hate lack of
  implicit conversions** that they consider reasonable. Violate their expectations
  for reasons they consider hypothetical problems and you get disuse. In type
  terms, I usually **translate "reasonable" to "not narrowing" aka "not loosing
  information."** That can reasonably easily be explained to non-language-experts.

  -- *Bjarne Stroustrup*

The library tries to follow the above principle and at the same time is also consistent
with conversions of ``std::chrono::duration``.


No Conversions
--------------

No conversions (either implicit or explicit) are available across quantities of
different dimensions::

    si::length<si::metre> d1 = 1_q_s;            // Compile-time error
    si::length<si::metre> d2(1_q_s);             // Compile-time error
    auto d3 = quantity_cast<si::metre>(1_q_s);   // Compile-time error


Implicit
--------

Implicit conversions are allowed only across quantities of the same dimension:

- for integral types with ratios that guarantee no precision loss::

    si::length<si::metre, int>        d1 = 1_q_km + 1_q_m;  // OK
    si::length<si::millimetre, int>   d2 = 1_q_km + 1_q_m;  // OK
    si::length<si::kilometre, int>    d3 = 1_q_km + 1_q_m;  // Compile-time error
    si::length<si::kilometre, int>    d4(1_q_km + 1_q_m);   // Compile-time error
    si::length<si::metre, int>        d5 = 1_q_m + 1_q_ft;  // Compile-time error
    si::length<si::metre, int>        d6(1_q_m + 1_q_ft);   // Compile-time error

- from an integral to a floating-point representation even in case of a truncating
  ratio::

    si::length<si::kilometre, double> d7 = 1_q_km + 1_q_m;  // OK
    si::length<si::metre, double>     d8 = 1_q_m + 1_q_ft;  // OK

- when both sides use a floating-point representation::

    si::length<si::metre, int>        d9 = 1.23_q_m;        // Compile-time error
    si::length<si::metre, double>    d10 = 1.23_q_m;        // OK


Explicit
--------

Explicit conversions are available with the `quantity_cast`, `quantity_point_cast`,
`quantity_kind_cast`, and `quantity_point_kind_cast` function templates.
They are especially useful to force a truncating conversion across quantities of the same
dimension for integral representation types and ratios that may cause precision loss::

    si::length<si::kilometre, int> d1 = quantity_cast<kilometre>(1km + 1m);    // OK
    si::length<si::kilometre, int> d2 = quantity_cast<kilometre>(1s);          // Error

.. seealso::

    Explicit casts are also really useful when working with legacy interfaces.
    More information on this subject can be found in the
    :ref:`use_cases/legacy_interfaces:Working with Legacy Interfaces` chapter.

Quantity Cast Overloads
^^^^^^^^^^^^^^^^^^^^^^^

`quantity_cast` comes with several overloads:

.. code-block::
    :linenos:

    std::cout << "Distance: " << quantity_cast<si::length<si::metre, int>>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<si::dim_length>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<si::metre>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<int>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<si::dim_length, si::metre>(d) << '\n';

`quantity_cast` in line #1 takes a specific target `quantity` type to which an explicit
cast should be performed. This option will change multiple quantity properties at once
(unit, representation, etc). However, it is also possible to force only some properties at
once and leave the rest intact:

- line #2 forces only a specific destination dimension type,
- line #3 forces only a specific destination unit type,
- line #4 sets only a representation type to the type provided by the user,
- line #5 forces both a specific dimension and a unit while preserving the original
  representation type.

`quantity_point_cast` takes anything that works for `quantity_point`
or a specific target `quantity_point`::

    std::cout << "Point: " << quantity_point_cast<decltype(quantity_point{0_q_m})>(d) << '\n';

For a single explicit template argument,
`quantity_point_kind_cast` is a superset of `quantity_kind_cast`,
which is also a superset of `quantity_cast`.
For the (dimension, unit) pair of explicit arguments case of `quantity_cast`,
`quantity_point_kind_cast` and `quantity_kind_cast`
accept a `PointKind` and `Kind` instead of a `Dimension`, respectively.

.. seealso::

    For more information on conversion and casting and on how to extend the above
    "integral" vs "floating-point" logic please refer to the
    :ref:`use_cases/custom_representation_types:Using Custom Representation Types` chapter.


Implicit conversions of dimensionless quantities
------------------------------------------------

As noted in the :ref:`framework/quantities:Dimensionless Quantities` chapter,
:term:`quantity of dimension one` is somehow special but still obey most of the rules defined
for quantities. However, as they represent numbers it would be highly uncomfortable to every
time type::

    const auto d1 = 10_q_km;
    const auto d2 = 3_q_km;
    if(d1 / d2 > dimensionless<one>(2)) {
      // ...
    }

or::

    const auto fill_time_left = (box.height / box.fill_level(measured_mass) -
                                 dimensionless<one>(1)) * fill_time;

This is why it was decided to allow the ``dimensionless<one>`` quantity of any
representation type to be implicitly constructible from this representation type.
With that the above examples can be rewritten as follows::

    const auto d1 = 10_q_km;
    const auto d2 = 3_q_km;
    if(d1 / d2 > 2) {
      // ...
    }

and::

    const auto fill_time_left = (box.height / box.fill_level(measured_mass) - 1) * fill_time;

The above is true only for dimensionless quantities of `one` unit. If our quantity have a unit with
ratio different than ``1`` the implicit conversion will not happen. This is to prevent cases were the code
could be ambiguous. For example::

    Dimensionless auto foo(Length auto d1, Length auto d2)
    {
      return d1 / d2 + 1;
    }

As long as we can reason about what such code means for ``foo(10_q_km, 2_q_km)`` it is not that obvious
at all in the case of ``foo(10_q_cm, 2_q_ft)``. To make such code to compile for every case we have to
either change the type of the resulting unit to the one having ``ratio(1)`` (:term:`coherent derived unit`)::

    Dimensionless auto foo(Length auto d1, Length auto d2)
    {
      return quantity_cast<one>(d1 / d2) + 1;
    }

or to explicitly state what is the unit of our dimensionless value, e.g. `one`, `percent`, etc::

    Dimensionless auto foo(Length auto d1, Length auto d2)
    {
      return d1 / d2 + dimensionless<one>(1);
    }

There is one more important point to note here. As the the dimensionless quantity is more than just
a number, it is never implicitly converted back to the representation type. This means that the following
code will not compile::

    auto v = std::exp(10_q_m / 5_q_m);

To make it compile fine we have to either explicitly get the value stored in the quantity::

    auto v = std::exp(quantity_cast<one>(10_q_m / 5_q_m).number());

or use a mathematical wrapper function from `units` namespace::

    auto v = units::exp(10_q_m / 5_q_m);

.. important::

    Always remember to explicitly cast the quantity to the destination unit with `quantity_cast` before
    calling `quantity::number()`!
