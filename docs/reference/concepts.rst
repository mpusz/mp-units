Concepts
========

.. namespace:: units

.. note::

  All names defined in this chapter reside in the :any:`units` namespace unless specified otherwise.

.. concept:: template<typename T> PrefixType

  Satisfied by all types derived from :class:`prefix_type`.

.. concept:: template<typename T> Prefix

  Satisfied by all instantiations of :class:`prefix`.

.. concept:: template<typename T> Ratio

  Satisfied by all instantiations of :class:`ratio`.

.. concept:: template<typename R> UnitRatio

  Satisfied by all types that satisfy :expr:`Ratio<R>` and for which :expr:`R::num > 0` and :expr:`R::den > 0`.

.. concept:: template<typename T> BaseDimension

  Satisfied by all dimension types derived from instantiation of :class:`base_dimension`.

.. concept:: template<typename T> Exponent

  Satisfied by all instantiations of :class:`exp`.

.. concept:: template<typename T> DerivedDimension

  Satisfied by all dimension types derived from instantiation of :class:`detail::derived_dimension_base`.

.. concept:: template<typename T> Dimension

  Satisfied by all dimension types for which either :expr:`BaseDimension<T>` or :expr:`DerivedDimension<T>` is ``true``.

.. concept:: template<typename T> Unit

  Satisfied by all unit types derived from instantiation of :class:`scaled_unit`.

.. concept:: template<typename U, typename D> UnitOf

  Satisfied by all unit types that satisfy :expr:`Unit<U>`, :expr:`Dimension<D>`, and for which
  :expr:`U::reference` and :expr:`dimension_unit<D>::reference` denote the same unit type.

.. concept:: template<typename T> Quantity

  Satisfied by all instantiations of :class:`quantity`.

.. concept:: template<typename T> WrappedQuantity

  Satisfied by all wrapper types that satisfy :expr:`Quantity<typename T::value_type>` recursively
  (i.e. :expr:`std::optional<si::length<si::metre>>`).

.. concept:: template<typename T> Scalar

  Satisfied by types that satisfy :expr:`(!Quantity<T>) && (!WrappedQuantity<T>) && std::regular<T>`.
