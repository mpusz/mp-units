.. namespace:: units

glide_computer
==============

This example presents the usage of:

- `quantity_point` to mark "absolute" quantities like ``timestamp``
- `quantity_point_kind` to mark "absolute" kinds of quantities like ``altitude`` (as opposed to ``height``),
- different kinds for length, time, and speed,
- the use of quantity kinds to show how to handle 3D space for aviation needs,
- unit constants to initialize the database
- quantities text output formatting,
- cooperation with `std::chrono`.

.. literalinclude:: ../../example/glide_computer.h
  :caption: glide_computer.h
  :start-at: #include
  :end-before: using namespace units;
  :linenos:
  :lineno-match:

The use of `quantity_kind` and `quantity_point_kind` provides strong typing
that divide quantities and quantity points to the ones on a horizontal (X, Y) plane and vertical (Z) axis.
Their purpose is to make different kinds of quantity
(i.e. length) separate strong types (i.e. distance, height).
Additionally, separating a quantity from its quantity point
means that we can define a ``height`` and ``altitude`` as different strong types.
A quantity point provides a more restricted interface meant for absolute calculations.

The glide calculator engine also take benefits from different :term:`kinds of quantities <kind>`.
For example we have 3 for a quantity of length:

- ``distance`` - a relative horizontal distance between 2 points on Earth
- ``height`` - a relative altitude difference between 2 points in the air
- ``altitude`` - an absolute altitude value measured form the mean sea level (AMSL).

.. literalinclude:: ../../example/glide_computer.h
  :caption: glide_computer.h
  :start-at: using namespace units;
  :end-before: // text output
  :linenos:
  :lineno-match:

Next a custom text output is provided both for C++ output streams and the text formatting facility.

.. literalinclude:: ../../example/glide_computer.h
  :caption: glide_computer.h
  :start-at: // text output
  :end-before: // definition of glide computer databases and utilities
  :linenos:
  :lineno-match:

The engine of this simple glide computer works on quantities of the SI units with a floating-point
representation. The user can pass any unit valid for a given quantity and the library will automatically
convert it to the one required by the engine interface.

The glide calculator takes task created as a list of waypoints, glider performance data, weather conditions,
safety constraints, and a towing height.

.. literalinclude:: ../../example/glide_computer.h
  :caption: glide_computer.h
  :start-at: // definition of glide computer databases and utilities
  :linenos:
  :lineno-match:

.. literalinclude:: ../../example/glide_computer_example.cpp
  :caption: glide_computer_example.cpp
  :start-at: #include
  :linenos:
  :lineno-match:

Having all of that it estimates the number of flight phases (towing, circling, gliding, final glide)
needed to finish a task. As an output it provides the duration needed to finish the task while
flying a selected glider in the specific weather conditions.

.. literalinclude:: ../../example/glide_computer.cpp
  :caption: glide_computer.cpp
  :start-at: #include
  :linenos:
  :lineno-match:
