.. namespace:: units

Linear Algebra vs. Quantities
=============================

Even though **mp-units** library does not implement any Linear Algebra types it is generic
enough to be used with other Linear Algebra libraries existing on the market.

.. note::

    All of the examples provided in this chapter refer to the official proposal of the
    Linear Algebra Library for the C++23 defined in `P1385 <https://wg21.link/P1385>`_
    and its latest implementation from `GitHub <https://github.com/BobSteagall/wg21/tree/master/include>`_
    or `Conan <https://conan.io/center/wg21-linear_algebra>`_.
    Also, to simplify the examples all of them assume::

        using namespace std::math;


Linear Algebra of Quantities
----------------------------

The official :term:`quantity` definition states:

    *A quantity as defined here is a scalar. However, a vector or a tensor, the components of
    which are quantities, is also considered to be a quantity.*

So the most common use case would be to create a vector or matrix of quantities::

    fs_vector<si::length<si::metre>, 3> v = { 1 * m, 2 * m, 3 * m };
    fs_vector<si::length<si::metre>, 3> u = { 3 * m, 2 * m, 1 * m };
    fs_vector<si::length<si::kilometre>, 3> t = { 3 * km, 2 * km, 1 * km };

Having such definitions we can perform full dimensional analysis operations for the operations
allowed by the Linear Algebra rules. For example::

    std::cout << "v + u     = " << v + u << "\n";
    std::cout << "v + t     = " << v + t << "\n";
    std::cout << "t[m]      = " << fs_vector<si::length<si::metre>, 3>(t) << "\n";
    std::cout << "v * u     = " << v * u << "\n";
    std::cout << "2 * m * v = " << 2 * m * v << "\n";

The above code works as expected and produces the following output:

.. code-block:: text

    v + u     = |       4 m       4 m       4 m |
    v + t     = |    3001 m    2002 m    1003 m |
    t[m]      = |    3000 m    2000 m    1000 m |
    v * u     = 10 m²
    2 * m * v = |     2 m²     4 m²     6 m² |


Quantities of Linear Algebra Types
----------------------------------

The previous chapter should address most of the Linear Algebra related requirements.
However, it is also possible to use Linear Algebra entities as custom representation
types provided to a `quantity` class template.

.. seealso::

    More information on providing custom representation types for `quantity` can be
    found in the :ref:`use_cases/custom_representation_types:Using Custom Representation Types`
    chapter.

With this the above vector definitions can be rewritten as follows::

    si::length<si::metre, fs_vector<int, 3>> v(fs_vector<int, 3>{ 1, 2, 3 });
    si::length<si::metre, fs_vector<int, 3>> u(fs_vector<int, 3>{ 3, 2, 1 });
    si::length<si::kilometre, fs_vector<int, 3>> t(fs_vector<int, 3>{ 3, 2, 1 });

Now the same code doing basic Linear Algebra operations will provide the following
output:

.. code-block:: text

    v + u     = |         4         4         4 | m
    v + t     = |      3001      2002      1003 | m
    t[m]      = |      3000      2000      1000 | m
    v * u     = 10 m²
    2 * m * v = |         2         4         6 | m²


.. seealso::

    For more examples of Linear Algebra definition and operations please refer to
    the :ref:`examples/custom_representation/linear_algebra:linear_algebra` example.
