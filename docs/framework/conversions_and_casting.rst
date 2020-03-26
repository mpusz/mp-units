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

    si::length<si::metre> d1 = 1q_s;            // Compile-time error
    si::length<si::metre> d2(1q_s);             // Compile-time error
    auto d3 = quantity_cast<si::metre>(1q_s);   // Compile-time error


Implicit
--------

Implicit conversions are allowed only across quantities of the same dimension:

- for integral types with ratios that guarantee no precision loss::

    si::length<si::metre, int>        d1 = 1q_km + 1q_m;  // OK
    si::length<si::millimetre, int>   d2 = 1q_km + 1q_m;  // OK
    si::length<si::kilometre, int>    d3 = 1q_km + 1q_m;  // Compile-time error
    si::length<si::kilometre, int>    d4(1q_km + 1q_m);   // Compile-time error
    si::length<si::metre, int>        d5 = 1q_m + 1q_ft;  // Compile-time error
    si::length<si::metre, int>        d6(1q_m + 1q_ft);   // Compile-time error

- from an integral to a floating-point representation even in case of a truncating
  ratio::

    si::length<si::kilometre, double> d7 = 1q_km + 1q_m;  // OK
    si::length<si::metre, double>     d8 = 1q_m + 1q_ft;  // OK

- when both sides use a floating-point representation::

    si::length<si::metre, int>        d9 = 1.23q_m;       // Compile-time error
    si::length<si::metre, double>    d10 = 1.23q_m;       // OK


Explicit
--------

Explicit conversions are available with a `quantity_cast` function template. They
are especially useful to force a truncating conversion across quantities of the same
dimension for integral representation types and ratios that may cause precision loss::

    si::length<si::kilometre, int> d1 = quantity_cast<kilometre>(1km + 1m);    // OK
    si::length<si::kilometre, int> d2 = quantity_cast<kilometre>(1s);          // Error

.. seealso::

    Explicit casts are also really useful when working with legacy interfaces. More
    information on this subject can be found in :ref:`Working with Legacy Interfaces`
    chapter.

Quantity Cast Overloads
^^^^^^^^^^^^^^^^^^^^^^^

`quantity_cast` comes with several overloads:

.. code-block::
    :linenos:

    std::cout << "Distance: " << quantity_cast<si::length<si::metre, int>>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<si::metre>(d) << '\n';
    std::cout << "Distance: " << quantity_cast<int>(d) << '\n';

`quantity_cast` in line #1 takes a specific target `quantity` type to which an explicit
cast should be performed. This option will change multiple quantity properties at once
(unit, representation, etc). However, it is also possible to force only one property at
once and leave the rest intact:

- line #2 forces only a specific destination unit type,
- line #3 sets only a representation type to the type provided by the user.

.. seealso::

    For more information on conversion and casting and on how to extend the above "integral"
    vs "floating-point" logic please refer to the :ref:`Using Custom Representation Types`
    chapter.
