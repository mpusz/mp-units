.. namespace:: units

Library Directories Structure
=============================

.. code-block:: text

    units
    ├── bits
    │   └── external
    ├── data
    └── physical
        ├── cgs
        ├── iau
        ├── imperial
        ├── international
        ├── natural
        ├── si
        ├── typographic
        └── us

- *./units*

  - The main directory of the library.
  - Contains headers files that define a public interface of the library framework

- *./units/bits*

  - Contains header files with implementation details for the library. Interface of
    the tools provided here should not be standardized.

- *./units/bits/external*

  - Contains header files of general purpose utilities that are not necessary
    **mp-units** library specific. They are either implementation details of the
    library or should be (or already are) the subject of separate standardization
    proposals not related to a Physical Units library proposal.

- *./units/physical*

  - Contains the definition of physical units dimensions
  - Its subfolders provide the definitions of various
    :term:`systems of units <system of units>` with :term:`SI` being the most popular
    one.

- *./units/data*

  - Provides data information dimensions (i.e. `data::bitrate`).
