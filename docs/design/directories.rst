.. namespace:: units

Library Directories Structure
=============================

.. code-block:: text

    units
    ├── bits
    │   └── external
    ├── generic
    └── isq
        ├── dimensions
        ├── iec80000
        ├── natural
        └── si
            ├── cgs
            ├── fps
            ├── iau
            ├── imperial
            ├── international
            ├── typographic
            └── us

- *./units*

  - The main directory of the library.
  - Contains headers files that define a public interface of the library framework.

- *./units/bits*

  - Contains header files with implementation details for the library. Interface of
    the tools provided here should not be standardized.

- *./units/bits/external*

  - Contains header files of general purpose utilities that are not necessary
    **mp-units** library specific. They are either implementation details of the
    library or should be (or already are) the subject of separate standardization
    proposals not related to a Physical Units library proposal.

- *./units/generic*

  - Provides quantity types not related to any :term:`system of quantities`
    (e.g. `dimensionless`, `angle`).

- *./units/isq*

  - Contains the definition of quantity dimensions of International System of Quantities
    (:term:`ISQ`).
  - Its subfolders provide the definitions of various
    :term:`systems of units <system of units>` with International System of Units (:term:`SI`)
    being the most popular one.

.. seealso::

  More information on provided :term:`systems of units <system of units>` can be
  found in :ref:`reference/systems:Systems` chapter.

.. important::

    While working with predefined systems please always include a header file with all
    the definitions for the current system to limit the possibility of an ODR violation
    (e.g. *units/isq/si/si.h*).
