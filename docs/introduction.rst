Introduction
============

**mp-units** is a compile-time enabled Modern C++ library that provides compile-time
dimensional analysis and unit/quantity manipulation. The basic idea and design
heavily bases on `std::chrono::duration <https://en.cppreference.com/w/cpp/chrono/duration>`_
and extends it to work properly with many dimensions.

Thanks to compile-time operations no runtime execution cost is introduced,
facilitating the use of this library to provide dimension checking in
performance-critical code. Support for quantities and units for arbitrary unit
system models and arbitrary value types is provided, as is a fine-grained
general facility for unit conversions.

The library architecture has been designed with flexibility and extensibility
in mind. The demonstrations of the ease of adding new dimensions, their units,
and unit conversions are provided in the :ref:`examples:Examples`.

Open Source
-----------

**mp-units** is Free and Open Source, with a permissive
`MIT license <https://github.com/mpusz/units/blob/master/LICENSE.md>`_. Check
out the source code and issue tracking (for questions and support, reporting
bugs and suggesting feature requests and improvements) at https://github.com/mpusz/units.


Approach
--------

1. Safety and performance

  - strong types
  - compile-time safety
  - ``constexpr`` all the things
  - as fast or even faster than when working with fundamental types

2. The best possible user experience

  - compiler errors
  - debugging

3. No macros in the user interface
4. Easy extensibility
5. No external dependencies
6. Possibility to be standardized as a freestanding part of the C++ Standard
   Library


With the User's Experience in Mind
----------------------------------

Most of the important design decisions in the library are dictated by the requirement of
providing the best user experience as possible. Other C++ physical units libraries are
"famous" for their huge error messages (one line of the error log often do not fit on one
slide). The ultimate goal of **mp-units** is to improve this and make compile-time errors
and debugging as easy and user-friendly as possible.

To achieve this goal several techniques are applied:

- usage of C++20 concepts,
- using strong types for framework entities (instead of type aliases),
- limiting the number of template arguments to the bare minimum,
- :ref:`design/downcasting:The Downcasting Facility`.

.. important::

    In many generic C++ libraries compile-time errors do not happen often. It is hard to
    break ``std::string`` or ``std::vector`` in a way it won't compile with a huge error
    log. Physical Units libraries are different. **Generation of compile-time errors
    is the main reason to create such a library.**
