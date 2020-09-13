.. namespace:: units

Concepts
========

.. note::

    All names defined in this chapter reside in the :any:`units` namespace unless
    specified otherwise.

.. concept:: template<typename T> PrefixFamily

    A concept matching a prefix family. Satisfied by all types derived from :class:`prefix_family`.

.. concept:: template<typename T> Prefix

    A concept matching a symbol prefix. Satisfied by all instantiations of :class:`prefix`.

.. concept:: template<ratio R> UnitRatio

    Satisfied by all ratio values for which :expr:`R.num > 0` and :expr:`R.den > 0`.

.. concept:: template<typename T> BaseDimension

    A concept matching all base dimensions in the library. Satisfied by all dimension types
    derived from the instantiation of :class:`base_dimension`.

.. concept:: template<typename T> Exponent

    A concept matching dimension's exponents. Satisfied by all instantiations of :class:`exponent`.

.. concept:: template<typename T> DerivedDimension

    A concept matching all derived dimensions in the library. Satisfied by all dimension
    types derived from the instantiation of :class:`detail::derived_dimension_base`.

.. concept:: template<typename T> Dimension

    A concept matching all dimensions in the library. Satisfied by all dimension types for
    which either :expr:`BaseDimension<T>` or :expr:`DerivedDimension<T>` is ``true``.

.. concept:: template<typename T> Unit

    A concept matching all unit types in the library. Satisfied by all unit types derived
    from the instantiation of :class:`scaled_unit`.

.. concept:: template<typename U, typename D> UnitOf

    A concept matching only units of a specified dimension. Satisfied by all unit types that
    satisfy :expr:`Unit<U>`, :expr:`Dimension<D>`, and for which :expr:`U::reference` and
    :expr:`dimension_unit<D>::reference` denote the same unit type.

    :tparam U: Type to verify against concept constraints.
    :tparam D: Dimension type to use for verification.

.. concept:: template<typename T> Quantity

    A concept matching all quantities in the library. Satisfied by all instantiations of :class:`quantity`.

.. concept:: template<typename T> WrappedQuantity

    A concept matching types that wrap quantity objects. Satisfied by all wrapper types that
    satisfy :expr:`Quantity<typename T::value_type>` recursively
    (i.e. :expr:`std::optional<si::length<si::metre>>`).

.. concept:: template<typename T> ScalableNumber

    A concept matching non-Quantity types. Satisfied by types that match
    :expr:`(!Quantity<T>) && (!WrappedQuantity<T>) && std::regular<T>` and satisfy one of the
    following:

    - if type :expr:`T` is constructible from ``std::int64_t`` (which is the type that stores
      the elements of `ratio`), :expr:`T * T` and :expr:`T / T` must be valid,
    - otherwise, :expr:`T * std::int64_t`, :expr:`std::int64_t * T`, and :expr:`T / std::int64_t`
      must be valid.

.. concept:: template<typename T> QuantityPoint

    A concept matching all quantity points in the library. Satisfied by all instantiations of
    :class:`quantity_point`.

.. concept:: template<typename Dim, template<typename...> typename DimTemplate> concept DimensionOfT

    A concept matching all dimensions being the instantiations derived from the provided dimension
    class template.

.. concept:: template<typename Q, template<typename...> typename DimTemplate> concept QuantityOfT

    A concept matching all quantities with a dimension being the instantiation derived from
    the provided dimension class template.

.. concept:: template<typename T, typename Dim> concept QuantityOf

    A concept matching all quantities with a dimension being the instantiation derived from
    the provided dimension type.
