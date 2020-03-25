.. namespace:: units

Using Custom Representation Types
=================================

A custom representation type can be provided as the last `quantity` class template parameter.
With this a user is able to change how a quantity's value is being represented and provide
its own custom logic for it (i.e. use a complex number or a measurement class that will handle
not only a value but also a measurement error).


A `Scalar` concept
------------------

To support a minimum set of `quantity` operations all custom representation types have to
satisfy at least the `Scalar` concept. Which means that they:

- cannot be quantities or be wrappers over the `quantity` type
  (i.e. ``std::optional<si::length<si::metre>>``),
- have to be regular types (e.g. they have to provide equality operators)
- must be constructible from a fundamental integral type (to 

With the above we will be able to construct quantities, convert between the units of the same
dimension and compare them for equality. To provide additional `quantity` operations the
custom representation type have to satisfy more requirements.


Additional requirements
-----------------------

.. important::

    The requirements described in the chapter are optional in a meaning that if someone does
    not plan to use a specific quantity's operation his/her custom representation type can
    ignore (not implement/satisfy) the requirements for it.


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

In case we want to mix quantities of our Custom Representation Type with the quantities using
fundamental arithmetic types as their representation we have to provide conversion operators.

Again let's assume two types but this time let's scope on conversion operators rather
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


Tricky cases
------------



Customization points
--------------------

treat_as_floating_point

quantity_value



.. seealso::

    For more examples of custom representation types usage please refer to :ref:`measurement`
    example.
