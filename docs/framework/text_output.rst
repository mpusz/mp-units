.. namespace:: units

Text output
===========

Beside providing dimensional analysis and units conversions, the library
also tries really hard to print any quantity in the most user friendly way.

Output streams
--------------

The easiest way to print a quantity is to provide its object to the output
stream::

    using namespace si::literals;
    using namespace international::literals;
    constexpr Velocity auto v1 = avg_speed(220.q_km, 2q_h);
    constexpr Velocity auto v2 = avg_speed(140.q_mi, 2q_h);
    std::cout << v1 << '\n';  // 110 km/h
    std::cout << v2 << '\n';  // 70 mi/h

The text output will always print the :term:`value of a quantity` followed
by the symbol of a :term:`unit` associated with this quantity. We will learn
more about units in the :ref:`Units` chapter, but for now, it is important
to remember that it is a good practice to always `quantity_cast()` a quantity
of an unknown ``auto`` type before passing it to the text output::

    std::cout << quantity_cast<si::kilometre_per_hour>(v1) << '\n';  // 110 km/h
    std::cout << quantity_cast<si::metre_per_second>(v1) << '\n';    // 30.5556 m/s


Formatting the output
---------------------

Grammar:
^^^^^^^^

.. productionlist::
    units-format-spec: fill-and-align[opt] sign[opt] width[opt] precision[opt] type[opt] units-specs[opt]
    units-specs: conversion-spec
               : units-specs conversion-spec
               : units-specs literal-char
    literal-char: any character other than '{' or '}'
    conversion-spec: '%' modifier[opt] units-type
    modifier: 'A'
    units-type: one of 'n', 'q', 'Q', 't', '%'
