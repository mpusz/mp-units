.. namespace:: units

The Downcasting Facility
========================

Problem statement
-----------------

Most of the C++ libraries in the world use template aliases to provide a friendly name for a
developer. Unfortunately, such aliases are quickly lost in a compilation process and as a
result the potential error log contains a huge source type rather than a short alias for it.
The same can be observed during debugging of a source code that use template aliases.

Let's assume that we want to provide a user friendly name for a derived dimension of capacitance
quantity. Other libraries will do it in the following way::

    using dim_capacitance = detail::derived_dimension_base<exponent<si::dim_electric_current, 2>,
                                                           exponent<si::dim_length, -2>,
                                                           exponent<si::dim_mass, -1>,
                                                           exponent<si::dim_time, 4>>;

The above solution does provide a good developer's experience but a really poor one for the end
user. If we will get a compilation error message containing ``dim_capacitance`` in most cases
the compiler will print the following type instead of the alias::

    units::detail::derived_dimension_base<units::exponent<units::isq::si::dim_electric_current, 2, 1>,
    units::exponent<units::isq::si::dim_length, -2, 1>, units::exponent<units::isq::si::dim_mass,
    -1, 1>, units::exponent<units::isq::si::dim_time, 4, 1> >

You can notice that in case of **mp-units** even this long syntax was carefully selected to
provide quite good user experience (some other units libraries produce a type that cannot easily
fit on one slide) but it is not questionable less readable than just ``dim_capacitance``.

.. note::

    To better understand how the framework works and not clutter the text and graphs with
    long types in the following examples we will switch from ``dim_capacitance`` to ``dim_area``.
    The latter one has much shorter definition but the end result for both will be exactly the same -
    user-friendly, short name printed by the compiler and the debugger.


As we lack opaque/strong typedefs in the C++ language the only way to improve our case is
to use inheritance:

.. image:: /_static/img/downcast_1.png
    :align: center

.. 
    http://www.nomnoml.com

    [derived_dimension_base<exponent<si::dim_length, 2>>]<:-[dim_area]

This gives us a nice looking strong type when directly used by the user. However, we just got
ourselves into problems. The library's framework does not know how to switch from a long
instantiation of a ``derived_dimension_base`` class template that was generated as a result
of dimensional calculation to a nicely named child class assigned by the user for this
instantiation.


How it works?
-------------

To support this **mp-units** library introduces a new downcasting facility implemented fully
as a library feature. It creates 1-to-1 link between a long class template instantiation and a
strong type provided by the user. This provides automatic type substitution mechanism in the
framework.

.. important::

    The above 1-1 correspondence means that only one child class can be provided for a specific
    base class template instantiation. If a user will try to assign another child class to
    already used base class template instantiation the program will not compile.

The downcasting facility is provided by injecting two classes into our hierarchy:

.. image:: /_static/img/downcast_2.png
    :align: center

.. 
    http://www.nomnoml.com

    [downcast_base<detail::derived_dimension_base<exponent<si::dim_length, 2>>>]<:-[detail::derived_dimension_base<exponent<si::dim_length, 2>>]
    [detail::derived_dimension_base<exponent<si::dim_length, 2>>]<:-[downcast_child<dim_area, detail::derived_dimension_base<exponent<si::dim_length, 2>>>]
    [downcast_child<dim_area, detail::derived_dimension_base<exponent<si::dim_length, 2>>>]<:-[dim_area]

In the above example:

- ``dim_area`` is a downcasting target (child class)

- ``detail::derived_dimension_base`` class template instantiation is a downcasting source (base class)

- `downcast_base` is a class that implements :abbr:`CRTP (Curiously Recurring Template Pattern)`
  idiom, stores the base of a downcasting operation in a ``downcast_base_type`` member type,
  and provides only a Hidden Friend non-member function declaration of ``downcast_guide`` which is an
  :abbr:`ADL (Argument Dependent Lookup)` entry  point for the downcasting operation::
  
      template<typename BaseType>
      struct downcast_base {
        using downcast_base_type = BaseType;
        friend auto downcast_guide(downcast_base);  // declaration only (no implementation)
      };

  .. important::

      An important design point here is that this friend function does not return any specific type
      in its declaration and no definition is provided at this point.

- `downcast_child` is another :abbr:`CRTP (Curiously Recurring Template Pattern)` class template
  that defines the implementation of a non-member friend function of the `downcast_base` class
  template::

      template<typename Target, Downcastable T>
      struct downcast_child : T {
        friend auto downcast_guide(typename T::downcast_base) { return Target(); }
      };

  This is the place where the actual return type of the ``downcast_guide`` function is provided
  which serves as a target type of the downcasting operation.

  In the above class template definition `Downcastable` is a concepts that verifies if a type
  implements and can be used in a downcasting facility::

      template<typename T>
      concept Downcastable =
          requires {
            typename T::downcast_base_type;
          } &&
          std::derived_from<T, downcast_base<typename T::downcast_base_type>>;


With such :abbr:`CRTP (Curiously Recurring Template Pattern)` types the only thing the user
has to do in order to register a new type in the downcasting facility is to publicly derive
from `downcast_child` and pass this type as the first template argument of the `downcast_child`
class template.

Until now we scoped on how we define the base and target of a downcasting operation. To
perform the actual downcasting operation a dedicated alias template is provided::

    template<Downcastable T>
    using downcast = decltype(detail::downcast_impl<T>());

`downcast` is used to obtain the target type of the downcasting operation registered for a
given instantiation in a base type. ``detail::downcast_impl`` checks if a downcasting
target is registered for the specific base class. If yes, it returns the registered type,
otherwise it works like a regular identity type trait returning a provided base class::

    namespace detail {

      template<typename T>
      concept has_downcast_guide = requires(T t) { downcast_guide(t); };

      template<typename T>
      constexpr auto downcast_impl()
      {
        if constexpr(has_downcast_guide<T>)
          return decltype(downcast_guide(std::declval<downcast_base<T>>()))();
        else
          return T();
      }

    }
