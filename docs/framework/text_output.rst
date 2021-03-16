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

    using namespace units::isq::si::literals;
    using namespace units::isq::si::international::literals;
    constexpr Speed auto v1 = avg_speed(220._q_km, 2_q_h);
    constexpr Speed auto v2 = avg_speed(140._q_mi, 2_q_h);
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

    os << "|" << std::setw(10) << 123_q_m << "|";                       // |     123 m|
    os << "|" << std::setw(10) << std::left << 123_q_m << "|";          // |123 m     |
    os << "|" << std::setw(10) << std::setfill('*') << 123_q_m << "|";  // |*****123 m|


fmt::format
-----------

.. tip::

    The text formatting support is provided via the ``<units/format.h>`` header file.

The **mp-units** via ``fmt::format`` provides a fine-grained control over what
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

    std::cout << "Distance: " << 123_q_km << "\n";
    fmt::print("Distance: {}\n", 123_q_km);
    fmt::print("Distance: {:%Q %q}\n", 123_q_km);


Quantity Value, Symbol, or Both?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The user can easily decide to either print a whole quantity (value and symbol) or
only its parts. Also a different quantity formatting might be applied::

    fmt::print("{:%Q}", 123_q_km);    // 123
    fmt::print("{:%q}", 123_q_km);    // km
    fmt::print("{:%Q%q}", 123_q_km);  // 123km


Controlling Width, Fill, and Alignment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To control width, fill, and alignment the C++ standard grammar tokens ``fill-and-align``
and ``width`` are being used and they treat a quantity value and symbol as a contiguous
text::

    fmt::print("|{:0}|", 123_q_m);     // |123 m|
    fmt::print("|{:10}|", 123_q_m);    // |     123 m|
    fmt::print("|{:<10}|", 123_q_m);   // |123 m     |
    fmt::print("|{:>10}|", 123_q_m);   // |     123 m|
    fmt::print("|{:^10}|", 123_q_m);   // |  123 m   |
    fmt::print("|{:*<10}|", 123_q_m);  // |123 m*****|
    fmt::print("|{:*>10}|", 123_q_m);  // |*****123 m|
    fmt::print("|{:*^10}|", 123_q_m);  // |**123 m***|


ASCII-only Quantity Symbols
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unit symbols of some quantities are specified to use Unicode signs by the :term:`SI`
standard (i.e. ``Ω`` symbol for the resistance quantity). **mp-units** library follows
this by default. From the engineering point of view sometimes Unicode text  might
not be a solution as terminals of many (especially embedded) devices are ASCII-only.
In such a case the unit symbol can be forced to be printed using ASCII-only characters::

    fmt::print("{}", 10_q_R);                 // 10 Ω
    fmt::print("{:%Q %Aq}", 10_q_R);          // 10 ohm
    fmt::print("{}", 125_q_us);               // 125 µs
    fmt::print("{:%Q %Aq}", 125_q_us);        // 125 us
    fmt::print("{}", 9.8_q_m_per_s2);         // 9.8 m/s²
    fmt::print("{:%Q %Aq}", 9.8_q_m_per_s2);  // 9.8 m/s^2


Controlling on How the Quantity Value Is Being Printed
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``sign`` token allows us to specify on how the value's sign is being printed::

    fmt::print("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", 1_q_m);   // 1 m,+1 m,1 m, 1 m
    fmt::print("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", -1_q_m);  // -1 m,-1 m,-1 m,-1 m

where:

- ``+`` indicates that a sign should be used for both non-negative and negative numbers,
- ``-`` indicates that a sign should be used for negative numbers and negative zero only
  (this is the default behavior),
- ``<space>`` indicates that a leading space should be used for non-negative numbers other
  than negative zero, and a minus sign for negative numbers and negative zero.

``precision`` token is allowed only for floating-point representation types::

    fmt::print("{:%.0Q %q}", 1.2345_q_m);  // 1 m
    fmt::print("{:%.1Q %q}", 1.2345_q_m);  // 1.2 m
    fmt::print("{:%.2Q %q}", 1.2345_q_m);  // 1.23 m


:token:`units-rep-type` specifies how a value of the representation type is being
printed. For integral types::

    fmt::print("{:%bQ %q}", 42_q_m);    // 101010 m
    fmt::print("{:%BQ %q}", 42_q_m);    // 101010 m
    fmt::print("{:%dQ %q}", 42_q_m);    // 42 m
    fmt::print("{:%oQ %q}", 42_q_m);    // 52 m
    fmt::print("{:%xQ %q}", 42_q_m);    // 2a m
    fmt::print("{:%XQ %q}", 42_q_m);    // 2A m

The above can be printed in an alternate version thanks to the ``#`` token::

    fmt::print("{:%#bQ %q}", 42_q_m);   // 0b101010 m
    fmt::print("{:%#BQ %q}", 42_q_m);   // 0B101010 m
    fmt::print("{:%#oQ %q}", 42_q_m);   // 052 m
    fmt::print("{:%#xQ %q}", 42_q_m);   // 0x2a m
    fmt::print("{:%#XQ %q}", 42_q_m);   // 0X2A m

For floating-point values the :token:`units-rep-type` token works as follows::

    fmt::print("{:%aQ %q}",   1.2345678_q_m);      // 0x9.e065152d8eae841p-3 m
    fmt::print("{:%.3aQ %q}", 1.2345678_q_m);      // 0x9.e06p-3 m
    fmt::print("{:%AQ %q}",   1.2345678_q_m);      // 0X9.E065152D8EAE841P-3 m
    fmt::print("{:%.3AQ %q}", 1.2345678_q_m);      // 0X9.E06P-3 m
    fmt::print("{:%eQ %q}",   1.2345678_q_m);      // 1.234568e+00 m
    fmt::print("{:%.3eQ %q}", 1.2345678_q_m);      // 1.235e+00 m
    fmt::print("{:%EQ %q}",   1.2345678_q_m);      // 1.234568E+00 m
    fmt::print("{:%.3EQ %q}", 1.2345678_q_m);      // 1.235E+00 m
    fmt::print("{:%gQ %q}",   1.2345678_q_m);      // 1.23457 m
    fmt::print("{:%gQ %q}",   1.2345678e8_q_m);    // 1.23457e+08 m
    fmt::print("{:%.3gQ %q}", 1.2345678_q_m);      // 1.23 m
    fmt::print("{:%.3gQ %q}", 1.2345678e8_q_m);    // 1.23e+08 m
    fmt::print("{:%GQ %q}",   1.2345678_q_m);      // 1.23457 m
    fmt::print("{:%GQ %q}",   1.2345678e8_q_m);    // 1.23457E+08 m
    fmt::print("{:%.3GQ %q}", 1.2345678_q_m);      // 1.23 m
    fmt::print("{:%.3GQ %q}", 1.2345678e8_q_m);    // 1.23E+08 m


Special Signs
^^^^^^^^^^^^^

Beside adding any list of regular characters as a separator between the value and the
symbol, it is possible to type a few special signs there too::

    fmt::print("{:%Q_%q}", 123_q_km);    // 123_km
    fmt::print("{:%Q%t%q}", 123_q_km);   // 123\tkm  <tab>
    fmt::print("{:%Q%n%q}", 123_q_km);   // 123\nkm  <new line>
    fmt::print("{:%Q%% %q}", 123_q_km);  // 123% km
