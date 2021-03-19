.. namespace:: units

Working with Unknown Dimensions and Their Units
===============================================

From time to time the user of this library will face an `unknown_dimension` and
`unknown_coherent_unit` types. This chapters describes their purpose and usage in
detail.

What is an unknown dimension?
-----------------------------

As we learned in the :ref:`framework/dimensions:Dimensions` chapter, in most cases the result
of multiplying or dividing two quantities of specific dimensions is a quantity of yet another
dimension.

If such a resulting dimension is predefined by the user (and a proper header file with its
definition is included in the current translation unit)
:ref:`design/downcasting:The Downcasting Facility` will determine its type.
The same applies to the resulting unit. For example:

.. code-block::
    :emphasize-lines: 1,7,9

    #include <units/isq/si/si.h>

    using namespace units::isq::si;

    constexpr auto result = 144_q_km / 2_q_h;
    static_assert(std::is_same_v<decltype(result)::dimension,
                                 dim_speed>);
    static_assert(std::is_same_v<decltype(result)::unit,
                                 kilometre_per_hour>);

However, if the resulting dimension is not predefined by the user, the library framework
will create an instance of an `unknown_dimension`. The coherent unit of such an unknown
dimension is an `unknown_coherent_unit`. Let's see what happens with our example when
instead including the header with all :term:`SI` definitions we will just provide base
dimensions used in the division operation:

.. code-block::
    :emphasize-lines: 1-2,8,10

    #include <units/isq/si/length.h>
    #include <units/isq/si/time.h>

    using namespace units::isq::si;

    constexpr auto result = 144_q_km / 2_q_h;
    static_assert(std::is_same_v<decltype(result)::dimension,
                                 unknown_dimension<exponent<dim_length, 1>, exponent<dim_time, -1>>>);
    static_assert(std::is_same_v<decltype(result)::unit,
                                 scaled_unit<ratio(1, 36, 1), unknown_coherent_unit>>);

.. important::

    To limit the possibility of an ODR violation please always include a header file
    with all the definitions for the current system (e.g. *units/isq/si/si.h*).


Operations On Unknown Dimensions And Their Units
------------------------------------------------

For some cases we can eliminate the need to predefine a specific dimension and just use
the `unknown_dimension` instead. Let's play with the previous example a bit::

    static_assert(result.number() == 72);

As we can see the value stored in this quantity can be easily obtained and contains a
correct result. However, if we try to print its value to the text output we will get::

    std::cout << "Speed: " << result << '\n';  // prints 'Speed: 72 [1/36 × 10¹] m/s'

The output from above program should not be a surprise. It is an unknown dimensions with
a scaled unknown coherent unit. The library can't know what is the symbol of such unit
so it does its best and prints the unit in terms of units of base dimensions that formed
this particular unknown derived dimension.

In case we would like to print the result in terms of base units we can simply do the
following::

    auto s = quantity_cast<unknown_coherent_unit>(result);
    std::cout << "Speed: " << s << '\n';  // prints 'Speed: 20 m/s'

.. seealso::

    Another good example of unknown dimension usage can be found in the
    :ref:`examples/box_example:box_example`::

        std::cout << "float rise rate = " << box.fill_level(measured_mass) / fill_time << '\n';


Temporary Results
-----------------

In many cases there is nothing inherently wrong with having unknown dimensions and units
in your program. A typical example here are temporary results of a long calculation:

.. code-block::
    :emphasize-lines: 5,7

    auto some_long_calculation(Length auto d, Time auto t)
    {
      Speed auto s1 = avg_speed(d, t);

      auto temp1 = s1 * 200_q_km;      // intermediate unknown dimension

      Speed auto s2 = temp1 / 50_q_km; // back to known dimensions again
      Length auto d2 = s2 * 4_q_h;

      // ...
    }

If a programmer wants to break the calculation to several lines/variables he/she does not
have to ensure that the intermediate results are of predefined dimensions or just a clear
science fiction :-) The final result will always be correct.
