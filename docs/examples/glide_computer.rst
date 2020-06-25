glide_computer
==============

This example presents the usage of:

- different units for length, time, and velocity,
- quantities text output formatting,
- `quantity_point` to mark "absolute" values like ``altitude`` (as opposed to ``height``),
- a simple ``vector`` class to show how to handle 3D space for aviation needs and how to build abstractions
  on top of a `Quantity` concept.

``vector`` class template provides a strong type that divides quantities and quantity points to the ones
on a horizontal (X, Y) plane and vertical (Z) axis. Its purpose is to make different kinds of quantity
(i.e. length) separated strong types (i.e. distance, height). Additionally, it wraps both a `quantity`
and `quantity_point` types which means that we can define a ``height`` and ``altitude`` respectively
as different strong types. Some of its function are defined only for quantities (additional operators)
and some for quantity points (additional constructor).

.. literalinclude:: ../../example/glide_computer.cpp
  :caption: glide_computer.cpp
  :start-at: #include
  :end-before: // custom types
  :linenos:
  :lineno-match:

The next part of the example defines strong types used by the glide calculator engine. For example
we have 3 :term:`kinds of quantities <kind>` for a quantity of length:

- ``distance`` - a relative horizontal distance between 2 points on Earth
- ``height`` - a relative altitude difference between 2 points in the air
- ``altitude`` - an absolute altitude value measured form the mean sea level (AMSL).

For the last case a custom text output is provided both for C++ output streams and the text formatting
facility.

.. literalinclude:: ../../example/glide_computer.cpp
  :caption: glide_computer.cpp
  :start-at: // custom types
  :end-before: // An example of a really
  :linenos:
  :lineno-match:

The engine of this simple glide computer works on quantities of the SI units with a floating-point
representation. The user can pass any unit valid for a given quantity and the library will automatically
convert it to the one required by the engine interface.

The glide calculator estimates the number of flight phases (towing, circling, gliding, final glide) needed
to finish a task and the duration needed to finish it while flying a selected glider in the specific weather
conditions.

.. literalinclude:: ../../example/glide_computer.cpp
  :caption: glide_computer.cpp
  :start-at: // An example of a really simplified tactical glide computer
  :linenos:
  :lineno-match:
