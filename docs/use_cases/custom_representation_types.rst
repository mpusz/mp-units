.. namespace:: units

Using Custom Representation Types
=================================

A custom representation type can be provided as the last `quantity` class template parameter.
With this a user is able to change how a quantity's value is being represented and provide
its own custom logic for it (i.e. use a complex number or a measurement class that will handle
not only a value but also a measurement error).


A `Representation` concept
--------------------------

To support a minimum set of `quantity` operations all custom representation types have to
satisfy at least the `Representation` concept. Which means that they:

- cannot be quantities by themselves,
- cannot be wrappers over the `quantity` type (i.e. ``std::optional<si::length<si::metre>>``),
- have to be regular types (e.g. they have to provide equality operators)
- should either have common type with ``std::intmax_t`` which provides multiplication and
  division operators
- or :expr:`T::value_type` must be valid, and common type of ``T::value_type`` and ``std::intmax_t``
  must at least provide multiplication and division operators with itself and ``T``.

With the above we will be able to construct quantities, convert between the units of the same
dimension, and compare them for equality.


The Simplest Custom Representation Type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The simplest representation type that fulfills the above requirements can look as follows::

    class my_rep {
      std::intmax_t value_;
    public:
      my_rep() = default;
      constexpr explicit my_rep(std::intmax_t v) noexcept : value_(v) {}

      [[nodiscard]] bool operator==(my_rep) const = default;

      [[nodiscard]] friend constexpr my_rep operator*(my_rep lhs, my_rep rhs)
      {
        return my_rep(lhs.value_ * rhs.value_);
      }
      [[nodiscard]] friend constexpr my_rep operator/(my_rep lhs, my_rep rhs)
      {
        return my_rep(lhs.value_ / rhs.value_);
      }
    };

Now we can put ``my_rep`` as the last parameter of the `quantity` class template and the following
code will work just fine::

    static_assert(si::length<si::metre, my_rep>(my_rep(2'000)) == si::length<si::kilometre, my_rep>(my_rep(2)));


Construction of Quantities with Custom Representation Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Assuming that our custom representation type is constructible from a fundamental integral type,
let's assume two types:

.. code-block::
    :emphasize-lines: 5, 13

    class impl {
      int value_{};
    public:
      impl() = default;
      constexpr impl(int v): value_(v) {}
      // the rest of the representation type implementation
    };

    class expl {
      int value_{};
    public:
      expl() = default;
      constexpr explicit expl(int v): value_(v) {}
      // the rest of the representation type implementation
    };

The difference between the above types is that ``impl`` class is implicitly constructible
from values of type ``int`` while ``expl`` is not. To create quantities using those as their
representation types we have to obey similar rules::

    si::length<si::metre, impl> d1(123);         // OK
    si::length<si::metre, expl> d2(123);         // Compile-time error
    si::length<si::metre, expl> d3(expl(123));   // OK

This also applies when we want to create a quantity with a custom representation type
from a regular quantity value::

    Length auto d = 123_q_m;
    si::length<si::metre, impl> d1(d);                      // OK
    si::length<si::metre, expl> d2(d);                      // Compile-time error
    si::length<si::metre, expl> d3(quantity_cast<expl>(d)); // OK

The only difference here is that in this case we have to explicitly cast the `quantity` with
`quantity_cast` overload that scopes only on changing the representation type.

Additional Requirements
-----------------------

As noted in the previous chapter, the `Representation` concept guarantees us the possibility
to construct quantities, convert between the units of the same dimension, and compare them
for equality. To provide additional `quantity` operations the custom representation type
have to satisfy more requirements.

.. important::

    The requirements described in this chapter are optional in a meaning that if someone does
    not plan to use a specific quantity's operation his/her custom representation type can
    ignore (not implement/satisfy) the requirements for it.


Relational Quantity Operators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case we want to to compare the values of `quantity` type not only for equality but
also for ordering, we have to provide a corresponding operators to our ``my_rep`` class.
With C++20 it is really easy to do::

    class my_rep {
    public:
      [[nodiscard]] constexpr auto operator<=>(my_rep other) const = default;

      // ...
    };

With the above the following code will compile fine::

    static_assert(si::length<si::metre, my_rep>(2'000) < si::length<si::kilometre, my_rep>(3));


Arithmetic Quantity Operators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case we plan to perform arithmetic operations on our `quantity` type we have to provide
at least the following::

    class my_rep {
    public:
      [[nodiscard]] friend constexpr my_rep operator+(my_rep lhs, my_rep rhs)
      {
        return my_rep(lhs.value_ + rhs.value_);
      }
      [[nodiscard]] friend constexpr my_rep operator-(my_rep lhs, my_rep rhs)
      {
        return my_rep(lhs.value_ - rhs.value_);
      }

      // ...
    };

Thanks to it the following code will run as expected::

    static_assert(si::length<si::metre, my_rep>(2'000) + si::length<si::kilometre, my_rep>(1) ==
                  si::length<si::kilometre, my_rep>(3));

Of course, the above operators are the smallest possible set to provide support for basic
arithmetic operations. In case the user wants to use faster or more sophisticated operators
the following ones can be provided::

    class my_rep {
    public:
      [[nodiscard]] constexpr my_rep operator+() const;
      [[nodiscard]] constexpr my_rep operator-() const;

      constexpr my_rep& operator++();
      constexpr my_rep operator++(int);
      constexpr my_rep& operator--();
      constexpr my_rep operator--(int);
      constexpr my_rep& operator+=(my_rep q);
      constexpr my_rep& operator-=(my_rep q);
      constexpr my_rep& operator*=(my_rep rhs);
      constexpr my_rep& operator/=(my_rep rhs);
      constexpr my_rep& operator%=(my_rep rhs);

      [[nodiscard]] friend constexpr my_rep operator%(my_rep lhs, my_rep rhs);

      // ...
    };

Each of the above operators will enable a respective operator in the `quantity`
type.


Customization Points
--------------------

Up to now we were enabling new functionalities by adding new operations to the custom representation
type. However, we can also enable more operations and customize the engine behavior through a few
customization points.

`quantity_value`
^^^^^^^^^^^^^^^^

The `quantity` class template has a few static member functions: `quantity::zero`, `quantity::one`,
`quantity::min`, and `quantity::max`. Those return the respective quantity values for a specific
representation type. The default implementation is provided through the `quantity_values` class
template::

    template<Representation Rep>
    struct quantity_values {
      static constexpr Rep zero() noexcept { return Rep(0); }
      static constexpr Rep one() noexcept { return Rep(1); }
      static constexpr Rep min() noexcept { return std::numeric_limits<Rep>::lowest(); }
      static constexpr Rep max() noexcept { return std::numeric_limits<Rep>::max(); }
    };

The user can provide an explicit/partial class template specialization for his/her custom
representation type and provide values that should be returned by the respective `quantity`
operations.

`treat_as_floating_point`
^^^^^^^^^^^^^^^^^^^^^^^^^

In the :ref:`framework/conversions_and_casting:Conversions and Casting` chapter we learned
that the conversions provided by the library's framework treat floating-point representation
types differently than the integral ones. This behavior can also be extended to the custom
representation types with `treat_as_floating_point` customization point which default
definition is::

    template<Representation Rep>
    inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

If our representation type should have a floating-point semantics or if it is a class
template, in which case we may not know exactly what is the final representation type,
we can specialize this variable template as follows::

    namespace custom {

    template<typename T>
    class my_rep {
      T value_{};
    public:
      // ...
    };

    }  // namespace custom

    namespace units {

    template<typename T>
    inline constexpr bool treat_as_floating_point<custom::my_rep<T>> = std::is_floating_point_v<T>;

    }  // namespace units

.. important::

    Please remember that by the C++ language rules all template specializations have to be put
    always in the same namespace as the primary template definition.


Conversions of Quantities with Custom Representation Types
----------------------------------------------------------

In case we want to mix quantities of our Custom Representation Type with the quantities using
fundamental arithmetic types as their representation we have to provide conversion operators
in our representation type.

Again let's assume two types, but this time let's scope on conversion operators rather
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

Similarly, when we have quantities of the above types we can create quantities of other
representation types with::

    si::length<si::metre, impl<int>> d_impl(1);
    si::length<si::metre, impl<int>> d_expl(1);
    si::length<si::metre, int> d1(d_impl);                      // OK
    si::length<si::metre, int> d2(d_expl);                      // Compile-time error
    si::length<si::metre, int> d3(quantity_cast<int>(d_expl));  // OK


.. seealso::

    For more examples of custom representation types usage please refer to the
    :ref:`use_cases/linear_algebra:Linear Algebra vs. Quantities` chapter and the
    :ref:`examples/measurement:measurement` example.
