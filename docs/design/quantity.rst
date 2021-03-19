.. namespace:: units

quantity
========

Interface
---------

`quantity` class template provides a similar interface to
`std::chrono::duration <https://en.cppreference.com/w/cpp/chrono/duration>`_.
The difference is that it uses ``double`` as a default representation and has
a few additional member types and functions::

    template<Dimension D, UnitOf<D> U, Representation Rep = double>
    class quantity {
    public:
      using dimension = D;
      using unit = U;
      using rep = Rep;

      [[nodiscard]] static constexpr quantity one() noexcept;
      // ...
    };

    template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
      requires detail::basic_arithmetic<Rep1, Rep2> && (!equivalent<D1, dim_invert<D2>>)
    [[nodiscard]] constexpr Quantity auto operator*(const quantity<D1, U1, Rep1>& lhs,
                                                    const quantity<D2, U2, Rep2>& rhs);

    template<Representation Value, typename D, typename U, typename Rep>
      requires std::magma<std::ranges::divided_by, Value, Rep>
    [[nodiscard]] constexpr Quantity auto operator/(const Value& v,
                                                    const quantity<D, U, Rep>& q);

    template<typename D1, typename U1, typename Rep1, typename D2, typename U2, typename Rep2>
      requires detail::basic_arithmetic<Rep1, Rep2> && (!equivalent<D1, D2>)
    [[nodiscard]] constexpr Quantity auto operator/(const quantity<D1, U1, Rep1>& lhs,
                                                    const quantity<D2, U2, Rep2>& rhs);

Additional functions provide the support for operations that result in a
different dimension type than those of their arguments. ``equivalent``
constraint requires two dimensions to be either the same or have convertible
units of base dimension (with the same reference unit).

Beside adding new elements a few other changes where applied compared to the
`std::chrono::duration <https://en.cppreference.com/w/cpp/chrono/duration>`_
class template:

1. The ``std::chrono::duration`` is using ``std::common_type_t<Rep1, Rep2>`` to find a common
   representation for a calculation result. Such a design was reported as problematic
   by SG6 (numerics study group) members as sometimes we want to provide a different
   type in case of multiplication and different in case of division. ``std::common_type``
   lacks that additional information. That is why `quantity` uses the resulting
   type of a concrete operator operation.
2. ``quantity::operator%()`` is constrained with `treat_as_floating_point` type trait to limit the
   types to integral representations only. Also ``quantity::operator%(Rep)`` takes ``Rep`` as a
   template argument to limit implicit conversions.
