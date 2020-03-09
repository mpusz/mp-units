FAQ
===

General
-------

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


Why dimensions depend on units and not vice versa?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most of the libraries define units in terms of dimensions and this was also an
initial approach for this library. However it turns out that for such a design
it is hard to provide support for all the required scenarios.

The first of them is to support multiple unit systems (like SI, CGS, ...) where
each of can have a different base unit for the same dimension. Base quantity of
dimension length in SI has to know that it should use ``m`` to print the unit
symbol to the text output, while the same dimension for CGS should use ``cm``.
Also it helps in conversions among those systems.

The second one is to support natural units where more than one dimension can be
measured with the same unit (i.e. ``GeV``). Also if someone will decide to
implement a systems where SI quantities of the same kind are expressed as
different dimensions (i.e. height, width, and depth) all of them will just be
measured in meters.


Why do we spell ``metre`` instead of ``meter``?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Well, this is how ISO 80000 defines it (British English spelling by default).
