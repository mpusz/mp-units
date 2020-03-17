.. namespace:: units

Quantity Constants
==================

In the **mp-units** library quantity constants are implemented as variable
templates of `quantity` types parametrized by the quantity's representation
type.

Additionally, in case of the :term:`SI` its physical constants are put in
separate namespaces named after the official release of the :term:`SI`
specification. This is needed to be able to provide backward and forward
compatibility as exact values of physical constants tend to change with time
(in most cases they are getting more and more accurate with every
specification release).

For example the speed of light constant in :term:`SI` is defined as::

    namespace si::si2019 {

    template<Scalar Rep = double>
    inline constexpr auto speed_of_light = velocity<metre_per_second, Rep>(299792458);

    }

The same constant defined for natural units may be provided as::

    namespace natural {

    template<Scalar Rep = double>
    inline constexpr auto speed_of_light = velocity<unitless, Rep>(1);

    }
