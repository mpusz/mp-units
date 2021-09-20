.. namespace:: units

Text Output
===========

Beside providing dimensional analysis and units conversions, the library
also tries really hard to print any quantity in the most user friendly way.

.. note::

    The library provides no text output for
    quantity points or quantity (point) kinds.

Output Streams
--------------

.. tip::

    The streaming support is provided via the ``<units/quantity_io.h>`` header file.

The easiest way to print a quantity is to provide its object to the output
stream::

    using namespace units::isq::si::references;
    using namespace units::isq::si::international::references;
    constexpr Speed auto v1 = avg_speed(220. * km, 2 * h);
    constexpr Speed auto v2 = avg_speed(140. * mi, 2 * h);
    std::cout << v1 << '\n';  // 110 km/h
    std::cout << v2 << '\n';  // 70 mi/h

The text output will always print the :term:`value of a quantity` followed
by a space and then the symbol of a :term:`unit` associated with this quantity.

.. important::

    Remember that it is a good practice to always `quantity_cast()` a quantity
    of an unknown ``auto`` type before passing it to the text output::

        std::cout << quantity_cast<si::kilometre_per_hour>(v1) << '\n';  // 110 km/h
        std::cout << quantity_cast<si::metre_per_second>(v1) << '\n';    // 30.5556 m/s


Stream Output Formatting
^^^^^^^^^^^^^^^^^^^^^^^^

Only a basic formatting can be applied for output streams. It includes control
over width, fill, and alignment::

    os << "|" << std::setw(10) << 123 * m << "|";                       // |     123 m|
    os << "|" << std::setw(10) << std::left << 123 * m << "|";          // |123 m     |
    os << "|" << std::setw(10) << std::setfill('*') << 123 * m << "|";  // |*****123 m|


std::format
-----------

.. tip::

    The text formatting support is provided via the ``<units/format.h>`` header file.

The **mp-units** via ``std::format`` provides a fine-grained control over what
and how is being printed on the text output.


Grammar
^^^^^^^

.. productionlist::
    units-format-spec: [fill-and-align] [width] [units-specs]
    units-specs: conversion-spec
               : units-specs conversion-spec
               : units-specs literal-char
    literal-char: any character other than '{' or '}'
    conversion-spec: '%' units-type
    units-type: [units-rep-modifier] 'Q'
              : [units-unit-modifier] 'q'
              : one of "nt%"
    units-rep-modifier: [sign] [#] [precision] [L] [units-rep-type]
    units-rep-type: one of "aAbBdeEfFgGoxX"
    units-unit-modifier: 'A'

In the above grammar:

- ``fill-and-align``, ``width``, ``sign``, ``#``, ``precision`` and ``L`` tokens and
  individual tokens of :token:`units-rep-type` are defined in the
  `format.string.std <https://wg21.link/format.string.std>`_ chapter of the C++
  standard specification,
- tokens ``Q``, ``q``, ``n``, ``t``, and ``%`` of :token:`units-type` are described
  in the `time.format <https://wg21.link/time.format>`_ chapter of the C++ standard
  specification,
- ``A`` token of :token:`units-unit-modifier` forces ASCII-only output (instead of the
  default Unicode symbols defined by the :term:`SI` specification).


Default formatting
^^^^^^^^^^^^^^^^^^

To format `quantity` values the formatting facility uses :token:`units-format-spec`.
In case it is left empty the default formatting of ``{:%Q %q}`` is applied. The same
default formatting is also applied to the output streams. This is why the following
code lines produce the same output::

    std::cout << "Distance: " << 123 * km << "\n";
    std::cout << std::format("Distance: {}\n", 123 * km);
    std::cout << std::format("Distance: {:%Q %q}\n", 123 * km);


Quantity Value, Symbol, or Both?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The user can easily decide to either print a whole quantity (value and symbol) or
only its parts. Also a different quantity formatting might be applied::

    std::cout << std::format("{:%Q}", 123 * km);    // 123
    std::cout << std::format("{:%q}", 123 * km);    // km
    std::cout << std::format("{:%Q%q}", 123 * km);  // 123km


Controlling Width, Fill, and Alignment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To control width, fill, and alignment the C++ standard grammar tokens ``fill-and-align``
and ``width`` are being used and they treat a quantity value and symbol as a contiguous
text::

    std::cout << std::format("|{:0}|", 123 * m);     // |123 m|
    std::cout << std::format("|{:10}|", 123 * m);    // |     123 m|
    std::cout << std::format("|{:<10}|", 123 * m);   // |123 m     |
    std::cout << std::format("|{:>10}|", 123 * m);   // |     123 m|
    std::cout << std::format("|{:^10}|", 123 * m);   // |  123 m   |
    std::cout << std::format("|{:*<10}|", 123 * m);  // |123 m*****|
    std::cout << std::format("|{:*>10}|", 123 * m);  // |*****123 m|
    std::cout << std::format("|{:*^10}|", 123 * m);  // |**123 m***|


ASCII-only Quantity Symbols
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unit symbols of some quantities are specified to use Unicode signs by the :term:`SI`
standard (i.e. ``Ω`` symbol for the resistance quantity). **mp-units** library follows
this by default. From the engineering point of view sometimes Unicode text  might
not be a solution as terminals of many (especially embedded) devices are ASCII-only.
In such a case the unit symbol can be forced to be printed using ASCII-only characters::

    std::cout << std::format("{}", 10 * R);                   // 10 Ω
    std::cout << std::format("{:%Q %Aq}", 10 * R);            // 10 ohm
    std::cout << std::format("{}", 125 * us);                 // 125 µs
    std::cout << std::format("{:%Q %Aq}", 125 * us);          // 125 us
    inline constexpr auto s2 = s * s;
    std::cout << std::format("{}", 9.8 * (m / s2));           // 9.8 m/s²
    std::cout << std::format("{:%Q %Aq}", 9.8 * (m / s2));    // 9.8 m/s^2


Controlling on How the Quantity Value Is Being Printed
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``sign`` token allows us to specify on how the value's sign is being printed::

    std::cout << std::format("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", 1 * m);   // 1 m,+1 m,1 m, 1 m
    std::cout << std::format("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", -1 * m);  // -1 m,-1 m,-1 m,-1 m

where:

- ``+`` indicates that a sign should be used for both non-negative and negative numbers,
- ``-`` indicates that a sign should be used for negative numbers and negative zero only
  (this is the default behavior),
- ``<space>`` indicates that a leading space should be used for non-negative numbers other
  than negative zero, and a minus sign for negative numbers and negative zero.

``precision`` token is allowed only for floating-point representation types::

    std::cout << std::format("{:%.0Q %q}", 1.2345 * m);  // 1 m
    std::cout << std::format("{:%.1Q %q}", 1.2345 * m);  // 1.2 m
    std::cout << std::format("{:%.2Q %q}", 1.2345 * m);  // 1.23 m


:token:`units-rep-type` specifies how a value of the representation type is being
printed. For integral types::

    std::cout << std::format("{:%bQ %q}", 42 * m);    // 101010 m
    std::cout << std::format("{:%BQ %q}", 42 * m);    // 101010 m
    std::cout << std::format("{:%dQ %q}", 42 * m);    // 42 m
    std::cout << std::format("{:%oQ %q}", 42 * m);    // 52 m
    std::cout << std::format("{:%xQ %q}", 42 * m);    // 2a m
    std::cout << std::format("{:%XQ %q}", 42 * m);    // 2A m

The above can be printed in an alternate version thanks to the ``#`` token::

    std::cout << std::format("{:%#bQ %q}", 42 * m);   // 0b101010 m
    std::cout << std::format("{:%#BQ %q}", 42 * m);   // 0B101010 m
    std::cout << std::format("{:%#oQ %q}", 42 * m);   // 052 m
    std::cout << std::format("{:%#xQ %q}", 42 * m);   // 0x2a m
    std::cout << std::format("{:%#XQ %q}", 42 * m);   // 0X2A m

For floating-point values the :token:`units-rep-type` token works as follows::

    std::cout << std::format("{:%aQ %q}",   1.2345678 * m);      // 0x9.e065152d8eae841p-3 m
    std::cout << std::format("{:%.3aQ %q}", 1.2345678 * m);      // 0x9.e06p-3 m
    std::cout << std::format("{:%AQ %q}",   1.2345678 * m);      // 0X9.E065152D8EAE841P-3 m
    std::cout << std::format("{:%.3AQ %q}", 1.2345678 * m);      // 0X9.E06P-3 m
    std::cout << std::format("{:%eQ %q}",   1.2345678 * m);      // 1.234568e+00 m
    std::cout << std::format("{:%.3eQ %q}", 1.2345678 * m);      // 1.235e+00 m
    std::cout << std::format("{:%EQ %q}",   1.2345678 * m);      // 1.234568E+00 m
    std::cout << std::format("{:%.3EQ %q}", 1.2345678 * m);      // 1.235E+00 m
    std::cout << std::format("{:%gQ %q}",   1.2345678 * m);      // 1.23457 m
    std::cout << std::format("{:%gQ %q}",   1.2345678e8 * m);    // 1.23457e+08 m
    std::cout << std::format("{:%.3gQ %q}", 1.2345678 * m);      // 1.23 m
    std::cout << std::format("{:%.3gQ %q}", 1.2345678e8 * m);    // 1.23e+08 m
    std::cout << std::format("{:%GQ %q}",   1.2345678 * m);      // 1.23457 m
    std::cout << std::format("{:%GQ %q}",   1.2345678e8 * m);    // 1.23457E+08 m
    std::cout << std::format("{:%.3GQ %q}", 1.2345678 * m);      // 1.23 m
    std::cout << std::format("{:%.3GQ %q}", 1.2345678e8 * m);    // 1.23E+08 m


Special Signs
^^^^^^^^^^^^^

Beside adding any list of regular characters as a separator between the value and the
symbol, it is possible to type a few special signs there too::

    std::cout << std::format("{:%Q_%q}", 123 * km);    // 123_km
    std::cout << std::format("{:%Q%t%q}", 123 * km);   // 123\tkm  <tab>
    std::cout << std::format("{:%Q%n%q}", 123 * km);   // 123\nkm  <new line>
    std::cout << std::format("{:%Q%% %q}", 123 * km);  // 123% km
