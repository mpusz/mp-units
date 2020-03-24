.. namespace:: units

Using Custom Representation Types
=================================

Construction of Quantities with Custom Representation Types
-----------------------------------------------------------

Let's assume two types:

.. code-block::
    :emphasize-lines: 6, 15

    template<typename T>
    class impl {
      T value_{};
    public:
      impl() = default;
      constexpr impl(T v): value_(v) {}
      // the rest of the representation type implementation
    };

    template<typename T>
    class expl {
      T value_{};
    public:
      expl() = default;
      constexpr explicit expl(T v): value_(v) {}
      // the rest of the representation type implementation
    };

The difference between the above types is that ``impl`` class is implicitly constructible
from values of type ``T`` while ``expl`` is not. To create quantities using those types as
representation types we have to obey similar rules::

    si::length<si::metre, impl<int>> d1(123);         // OK
    si::length<si::metre, expl<int>> d2(123);         // Compile-time error
    si::length<si::metre, expl<int>> d3(expl(123));   // OK

This also applies when we want to create a quantity with a custom representation type
from a regular quantity value::

    Length auto d = 123q_m;
    si::length<si::metre, impl<int>> d1(d);                           // OK
    si::length<si::metre, expl<int>> d2(d);                           // Compile-time error
    si::length<si::metre, expl<int>> d3(quantity_cast<expl<int>>(d)); // OK


Conversions of Quantities with Custom Representation Types
----------------------------------------------------------

Again let's assume two types but this time let's scope on converting operators rather
than on constructors:

.. code-block::
    :emphasize-lines: 5, 13

    template<typename T>
    class impl {
      T value_{};
    public:
      constexpr operator const T&() const& { return value_; }
      // the rest of the representation type implementation
    };

    template<typename T>
    class expl {
      T value_{};
    public:
      constexpr explicit operator const T&() const& { return value_; }
      // the rest of the representation type implementation
    };

If we have instances of the above types we can construct quantities in the following way::

    impl<int> v_impl(1);
    expl<int> v_expl(1);
    si::length<si::metre, int> d1(v_impl);      // OK
    si::length<si::metre, int> d2(v_expl);      // Compile-time error
    si::length<si::metre, int> d3(int(v_expl);  // OK

Similarly, when we have quantities of above types we can create quantities of other
representation types with::

    si::length<si::metre, impl<int>> d_impl(1);
    si::length<si::metre, impl<int>> d_expl(1);
    si::length<si::metre, int> d1(d_impl);                      // OK
    si::length<si::metre, int> d2(d_expl);                      // Compile-time error
    si::length<si::metre, int> d3(quantity_cast<int>(d_expl));  // OK


Customization points
--------------------





.. seealso::

    For more examples of custom representation types usage please refer to
    :ref:`Linear Algebra of Quantities` chapter and :ref:`measurement` example.