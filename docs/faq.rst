.. namespace:: units

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


Why other systems are defined in the `isq::si` namespace?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All systems defined in the `isq::si` namespace are defined in terms of
base units that are convertible to the :term:`SI` units. This enables conversions
of units of the same physical dimension between different systems.

.. seealso::

    More details on this subject can be found in the
    :ref:`use_cases/extensions:Custom Systems` chapter.


Why a dimensionless quantity is not just an fundamental arithmetic type?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the initial design of this library the resulting type of the division of
two quantities was their common representation type::

    static_assert(std::is_same_v<decltype(10_q_km / 5_q_km), std::int64_t>);

The reasoning behind it was to not provide a false impression of a strong `quantity` type
for something that looks and feels like a regular number. Also all of the mathematic
and trigonometric functions were working fine out of the box with such representation
types, so we did not have to rewrite ``sin()``, ``cos()``, ``exp()``, and others.

However, the feedback we got from the production usage was that such an approach
is really bad for generic programming. It is really hard to handle the result of
division (or multiplication) of two quantities as it might be either a `quantity`
or a fundamental type. If we want to raise such a result to some power we have to
either use ``units::pow`` or ``std::pow`` depending on the resulting type. Those
are only a few from many similar issues related to such an approach.

This is why it was decided to go with the current approach.

.. seealso::

    More information on the current design can be found in
    the :ref:`framework/quantities:Dimensionless Quantities` chapter.


Why do we spell ``metre`` instead of ``meter``?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Well, this is how [ISO80000]_ defines it (British English spelling by default).


User Defined Literals (UDLs)
----------------------------

Why all UDLs are prefixed with ``_q_`` instead of just using unit symbol?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::

    Every ``_q_*`` UDL will be replaced by the ``q_*`` literal when/if **mp-units**
    will become a part of the C++ Standard Library.

Usage of only unit symbols in UDLs would be a preferred approach (less to type,
easier to understand and maintain). However, while increasing the coverage for
the library we learned that there are a lot unit symbols that conflict with
built-in types or numeric extensions. A few of those are: ``F`` (farad),
``J`` (joule), ``W`` (watt), ``K`` (kelvin), ``d`` (day),
``l`` or ``L`` (litre), ``erg``, ``ergps``. For a while for those we used ``_``
prefix to make the library work at all, but at some point we had to unify the
naming and we came up with ``_q_`` prefix which results in a creation of
quantity of a provided unit.


Text formatting
---------------

Why Unicode quantity symbols are used by default instead of ASCII-only characters?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Both C++ and :term:`SI` ([ISO80000]_) are standardized by the
:abbr:`ISO (International Organization for Standardization)`. :term:`SI` standard
specifies Unicode symbols as the official unit names for some quantities (i.e. ``Ω``
symbol for the resistance quantity). As **mp-units** library
is being proposed for standardization as a part of the C++ Standard Library we have
to obey the rules and be consistent with ISO specifications.

.. seealso::

    We do understand engineering reality and constraints and that is why the library
    has the option of :ref:`framework/text_output:ASCII-only quantity symbols`.


Compilation Errors
------------------

error: reference to ‘time’ is ambiguous
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unfortunately, if `using-directives <https://en.cppreference.com/w/cpp/language/namespace#Using-directives>`_
(i.e. ``using namespace units::isq::si``) are being used, `units::isq::si::time`
will collide with C `time <https://en.cppreference.com/w/c/chrono/time>`_ function. In
such a case the library's ``time`` function needs to be prefixed with at least one (or all)
namespace names.
