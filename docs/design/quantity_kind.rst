.. namespace:: units

quantity_kind
=============

The ``quantity_kind`` class template provides a similar interface to ``quantity``.

Kinds
-----

The first template parameter of ``quantity_kind`` is a ``Kind``.

``Kind`` s, by themselves:

    * Wrap a ``Dimension``, and
    * opt into the downcasting facility.

If a dimensional analysis operator on a quantity of kind *A*
can result on a quantity of kind *B*,
*A* and *B* are *related*.

The library provides two ``Kind`` bases, ``kind`` and ``derived_kind``.

``kind`` is an entry point, and there's only one per set of related kinds.

``derived_kind`` is used to explicitly name other related kinds.
Unnamed, they look like ``detail::_kind_base<the-entry-kind, some_dimension>``.

Unnamed kinds, like unnamed units and dimensions,
allows (intermediate) results in formulas without having to name them.

Quantity kinds
--------------

``quantity_kind`` wraps a ``quantity`` and layers over its dimensional analysis.
While all properties of the ``quantity`` apply transparently,
results are always ``quantity_kind`` s related to the kind(s) of the argument(s).

One of the arguments to the dimensional analysis operators can also be ``Quantity``.

The way back to ``quantity`` is through the ``.common()`` observer.

Intra-kind operations
---------------------

``quantity_kind`` takes care of operators between related quantity kinds.
Intra-kind operations can be opted into as follows::

    struct width : kind<width, dim_length> {};
    struct height : kind<height, dim_length> {};

    size2d operator+(QuantityKindOf<width> auto, QuantityKindOf<height> auto);
