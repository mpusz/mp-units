.. namespace:: units

Angular Units
=============

Some background information
---------------------------

As per today's SI, both radian and steradian are dimensionless. This forces the convention to set the angle ``1 radian``
equal to the number ``1`` within equations (similar to what natural units system does for ``c`` constant).

Following `Wikipedia <https://en.wikipedia.org/wiki/Radian#Dimensional_analysis>`_:

    Giacomo Prando says "the current state of affairs leads inevitably to ghostly appearances and disappearances of the radian
    in the dimensional analysis of physical equations." For example, a mass hanging by a string from a pulley will rise or drop
    by :math:`y=rθ`` centimeters, where :math:`r` is the radius of the pulley in centimeters and :math:`θ` is the angle
    the pulley turns in radians. When multiplying :math:`r` by :math:`θ` the unit of radians disappears from the result.
    Similarly in the formula for the angular velocity of a rolling wheel, :math:`ω=v/r`, radians appear in the units of
    :math:`ω` but not on the right hand side.
    Anthony French calls this phenomenon "a perennial problem in the teaching of mechanics". Oberhofer says that the typical
    advice of ignoring radians during dimensional analysis and adding or removing radians in units according to convention
    and contextual knowledge is "pedagogically unsatisfying".

At least a dozen scientists have made proposals to treat the radian as a base unit of measure defining its own dimension of "angle",
as early as 1936 and as recently as 2022. This would bring the advantages of a physics-based, consistent, and logically-robust
unit system, with unambiguous units for all physical quantities. At the same time the only notable changes for typical
end-users would be: improved units for the quantities **torque**, **angular momentum** and **moment of inertia**.

Paul Quincey in his proposal *"Angles in the SI: a detailed proposal for solving the problem"* [Quincey]_ states:

    The familiar units assigned to some angular quantities are based on equations that have adopted the radian convention,
    and so are missing `rad`s that would be present if the complete equation is used. The physically-correct units are
    those with the `rad`s reinstated. Numerical values would not change, and the physical meanings of all quantities would
    also be unaffected.

He proposes the following changes:

- The **radian** would become either a new base unit or a 'complementary' unit
- The **steradian** would become a derived unit equal to :math:`1\:rad^2`
- The SI units for

  - **Torque** would change from :math:`N\:m` (= J) to :math:`J\:rad^{-1}`
  - **Angular momentum** would change from :math:`J\:s` to :math:`J\:s\:rad^{-1}` (i.e. :math:`J/(rad/s)`)
  - **Moment of inertia** would change from :math:`kg\:m^2` to :math:`kg\:m^2\:rad^{-2}` (i.e. :math:`J/(rad/s)^2`)

- The option to omit the radian from the SI units for **angle**, **angular velocity**, **angular frequency**,
  **angular acceleration**, and **angular wavenumber** would be removed, the only correct SI units being
  :math:`rad`, :math:`rad/s`, :math:`rad/s`, :math:`rad/s^2` and :math:`rad/m` respectively.

Paul Quincey summarizes that with the above in action:

    However, the physical clarity this would build into the SI should be recognised very quickly. The units would tell us that
    :math:`torque \times angle = energy`, and :math:`angular\:momentum \times angle = action`, for example, in the same way that they do for
    :math:`force \times distance = energy`, :math:`linear\:momentum \times distance = action`, and
    :math:`radiant\:intensity \times solid\:angle = radiant\:flux`.
    Dimensional analysis could be used to its full extent. Software involving angular quantities would be rationalised.
    Arguments about the correct units for frequency and angular frequency, and the meaning of the unit Hz, could be left behind.
    The explanation of these changes would be considerably easier and more rewarding than explaining how a kilogram-sized mass
    can be measured in terms of the Planck constant.


Angular quantities in the library
---------------------------------

The mp-units library strives to define physically-correct quantities and their units to provide maximum help
to its users. As treating angle as a dimensional quantity can lead to many "trivial" mistakes in dimensional
analysis and calculation, it was decided to follow the above approach and treat angle as a base quantity
with a base unit of radian and solid angle as its derived quantity.

As those the above (at least for now) not a part of :term:`SI`, the plain angle and solid
angle definitions can be found in the *units/generic* subdirectory. However, in the sake of correctness,
they are consistently used in the dimensional recipes of all angle-based :term:`SI` quantities like
torque or angular velocity::

    #include <units/generic/angle.h>
    #include <units/isq/si/torque.h>
    #include <units/math.h>
    #include <units/quantity_io.h>
    #include <iostream>

    int main()
    {
      using namespace units;
      using namespace units::isq;
      using namespace units::aliases;
      using namespace units::aliases::isq::si;

      const Length auto lever = cm<>(20);
      const Force auto force = N<>(500);
      const Angle auto angle = deg<>(90);
      const Torque auto torque = lever * force * sin(angle) / cotes_angle<>;

      std::cout << "Applying a perpendicular force of " << force << " to a "
                << lever << " long lever results in "
                << quantity_cast<si::newton_metre_per_radian>(torque) << " of torque.\n";
    }

The above program prints:

.. code-block::

    Applying a perpendicular force of 500 N to a 20 cm long lever results in 100 N⋅m/rad of torque.

.. note::

    `cotes_angle` is a constant which represents an angle with the value of exactly ``1 radian``.
    You can find more information about this constant in [Quincey]_.


Radians and degrees support
---------------------------

Thanks to :ref:`framework/magnitudes:Magnitudes` usage the library provides efficient strong types for all
angular types. This means that with the built-in support for magnitudes of :math:`\pi` we can provide
accurate conversions between radians and degrees. The library also provides common trigonometric functions
for angular quantities::

    using namespace units::aliases::isq::si;

    auto velocity = km_per_h<>(110);
    auto rate_of_climb = m_per_s<>(-0.63657);
    auto glide_ratio = velocity / -rate_of_climb;
    auto glide_angle = asin(1 / glide_ratio);

    std::cout << std::format("Glide ratio: {:%.1Q %q}\n", quantity_cast<units::one>(glide_ratio));
    std::cout << "Glide angle:\n";
    std::cout << std::format(" - {:%.4Q %q}\n", glide_angle);
    std::cout << std::format(" - {:%.2Q %q}\n", quantity_cast<units::degree>(glide_angle));
    std::cout << std::format(" - {:%.2Q %q}\n", quantity_cast<units::gradian>(glide_angle));

The above program prints:

.. code-block::

    Glide ratio: 48.0
    Glide angle:
     - 0.0208 rad
     - 1.19 °
     - 1.33 ᵍ

.. note::

    The above ``velocity`` and ``rate_of_climb`` in the production code should be modelled as
    :ref:`framework/quantity_kinds:Quantity Kinds`.
    A better but longer code can be found in the :ref:`examples/basics/glide_computer:glide_computer` example.
