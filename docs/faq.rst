FAQ
===

.. contents:: Questions:
    :local:


General
-------

Why dimensions depend on units and not vice versa?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most of the libraries define units in terms of dimensions and this was also an
initial approach for this library. However, it turns out that for such a design
it is hard to provide support for all the required scenarios.

The first of them is to support multiple unit systems (like SI, CGS, ...) where
each of can have a different base unit for the same dimension. Base quantity of
dimension length in SI should use ``m`` to print the unit symbol to the text
output, while the same dimension for CGS should use ``cm``. Also, it helps in
conversions among those systems.

The second one is to support natural units where more than one dimension can be
measured with the same unit (i.e. ``GeV``). Also if someone will decide to
implement a systems where SI quantities of the same kind are expressed as
different dimensions (i.e. height, width, and depth) all of them will just be
measured in meters.


Why do we spell ``metre`` instead of ``meter``?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Well, this is how [ISO-80000]_ defines it (British English spelling by default).


User Defined Literals (UDLs)
----------------------------

Why all UDLs are prefixed with ``q_`` instead of just using unit symbol?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Usage of only unit symbols in UDLs would be a preferred approach (less to type,
easier to understand and maintain). However, while increasing the coverage for
the library we learned that there are a lot unit symbols that conflict with
built-in types or numeric extensions. A few of those are: ``F`` (farad),
``J`` (joule), ``W`` (watt), ``K`` (kelvin), ``d`` (day),
``l`` or ``L`` (litre), ``erg``, ``ergps``. For a while for those we used ``_``
prefix to make the library work at all, but at some point we had to unify the
naming and we came up with ``q_`` prefix which results in a creation of
quantity of a provided unit.


Why do we use UDLs reserved for the C++ standard (no ``_`` prefix)?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This library is meant to become a part of the C++ Standard Library at some point
in the future. We decided to work with the target interface to get implementation
and user experience. Only thanks to this we were able to detect multiple collisions
with literals already existing in the language in our initial approach
(`Why all UDLs are prefixed with q_ instead of just using unit symbol?
<#why-unicode-quantity-symbols-are-used-by-default-instead-of-ascii-only-characters>`_).
This approach has some side effects though. We had to disable some compiler warnings
to make it work:

- ``/wd4455`` on MSVC,
- ``-Wno-literal-suffix`` on other compilers.


Text formatting
---------------

Why Unicode quantity symbols are used by default instead of ASCII-only characters?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Both C++ and :term:`SI` ([ISO-80000]_) are standardized by the
:abbr:`ISO (International Organization for Standardization)`. :term:`SI` standard
specifies Unicode symbols as the official unit names for some quantities (i.e. ``Ω``
symbol for the resistance quantity). As **mp-units** library
is being proposed for standardization as a part of the C++ Standard Library we have
to obey the rules and be consistent with ISO specifications.

.. seealso::

    We do understand engineering reality and constraints and that is why the library
    has the option of :ref:`ASCII-only quantity symbols`.


Compilation Errors
------------------

error: reference to ‘time’ is ambiguous
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unfortunately, if `using-directives <https://en.cppreference.com/w/cpp/language/namespace#Using-directives>`_
(i.e. ``using namespace units::physical::si``) are being used, `units::physical::si::time` will
collide with C `time <https://en.cppreference.com/w/c/chrono/time>`_ function. In such a case the library's
`time` function needs to be prefixed with at least one (or all) namespace names.








.. rubric:: Footnotes:

.. [ISO-80000] ISO 80000 or IEC 80000 is an international standard promulgated jointly
    by the :abbr:`ISO (International Organization for Standardization)` and the
    :abbr:`IEC (International Electrotechnical Commission)`. The standard introduces the
    :term:`International System of Quantities`. It is a style guide for the use of
    physical quantities and units of measurement, formulas involving them, and their
    corresponding units, in scientific and educational documents for worldwide use.
    Read more on `Wikipedia <ISO/IEC 80000 https://en.wikipedia.org/wiki/ISO/IEC_80000>`_.
