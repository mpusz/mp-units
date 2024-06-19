# Strong Angular System

## Some background information

As per today's SI, both radian and steradian are dimensionless. This forces the convention to set the angle `1 radian`
equal to the number `1` within equations (similar to what natural units system does for `c` constant).

Following [Wikipedia](https://en.wikipedia.org/wiki/Radian#Dimensional_analysis):

!!! quote "Wikipedia: Radian - Dimensional analysis"

    Giacomo Prando says "the current state of affairs leads inevitably to ghostly appearances and disappearances of the radian
    in the dimensional analysis of physical equations." For example, a mass hanging by a string from a pulley will rise or drop
    by $y=rθ$ centimeters, where $r$ is the radius of the pulley in centimeters and $θ$ is the angle the pulley turns in radians.
    When multiplying $r$ by $θ$ the unit of radians disappears from the result.
    Similarly in the formula for the angular velocity of a rolling wheel, $ω=v/r$, radians appear in the units of
    $ω$ but not on the right hand side.
    Anthony French calls this phenomenon "a perennial problem in the teaching of mechanics". Oberhofer says that the typical
    advice of ignoring radians during dimensional analysis and adding or removing radians in units according to convention
    and contextual knowledge is "pedagogically unsatisfying".

At least a dozen scientists have made proposals to treat the radian as a base unit of measure defining its own dimension of "angle",
as early as 1936 and as recently as 2022. This would bring the advantages of a physics-based, consistent, and logically-robust
unit system, with unambiguous units for all physical quantities. At the same time the only notable changes for typical
end-users would be: improved units for the quantities _torque_, _angular momentum_, and _moment of inertia_.

Paul Quincey in his proposal [_"Angles in the SI: a detailed proposal for solving the problem"_](../../appendix/references.md#Quincey) states:

!!! quote "Paul Quincey: Angles in the SI: a detailed proposal for solving the problem"

    The familiar units assigned to some angular quantities are based on equations that have adopted the radian convention,
    and so are missing `rad`s that would be present if the complete equation is used. The physically-correct units are
    those with the `rad`s reinstated. Numerical values would not change, and the physical meanings of all quantities would
    also be unaffected.

He proposes the following changes:

- The **radian** would become either a new base unit or a 'complementary' unit
- The **steradian** would become a derived unit equal to $1\:rad^2$
- The SI units for

    - _Torque_ would change from $N\:m$ ($=J$) to $J\:rad^{-1}$
    - _Angular momentum_ would change from $J\:s$ to $J\:s\:rad^{-1}$ (i.e. $J/(rad/s)$)
    - _Moment of inertia_ would change from $kg\:m^2$ to $kg\:m^2\:rad^{-2}$ (i.e. $J/(rad/s)^2$)

- The option to omit the radian from the SI units for _angle_, _angular velocity_, _angular frequency_,
  _angular acceleration_, and _angular wavenumber_ would be removed, the only correct SI units being
  $rad$, $rad/s$, $rad/s$, $rad/s^2$ and $rad/m$ respectively.

Paul Quincey summarizes that with the above in action:

!!! quote "Paul Quincey: Angles in the SI: a detailed proposal for solving the problem"

    However, the physical clarity this would build into the SI should be recognised very quickly. The units would tell us that
    $torque \times angle = energy$, and $angular\:momentum \times angle = action$, for example, in the same way that they do for
    $force \times distance = energy$, $linear\:momentum \times distance = action$, and
    $radiant\:intensity \times solid\:angle = radiant\:flux$.
    Dimensional analysis could be used to its full extent. Software involving angular quantities would be rationalised.
    Arguments about the correct units for frequency and angular frequency, and the meaning of the unit $Hz$, could be left behind.
    The explanation of these changes would be considerably easier and more rewarding than explaining how a kilogram-sized mass
    can be measured in terms of the Planck constant.


## Angular quantities in the SI

Even though the SI somehow ignores the dimensionality of angle:

!!! quote "SI Brochure"

    Plane and solid angles, when expressed in radians and steradians respectively, are in effect
    also treated within the SI as quantities with the unit one. The symbols $rad$ and $sr$
    are written explicitly where appropriate, in order to emphasize that, for radians or
    steradians, the quantity being considered is, or involves the plane angle or solid angle
    respectively. For steradians it emphasizes the distinction between units of flux and intensity
    in radiometry and photometry for example. However, it is a long-established practice in
    mathematics and across all areas of science to make use of $rad = 1$ and $sr = 1$.
    For historical reasons the radian and steradian are treated as derived units.

It also explicitly states:

!!! quote "SI Brochure"

    The SI unit of frequency is hertz, the SI unit of angular velocity and angular frequency is
    radian per second, and the SI unit of activity is becquerel, implying counts per second.
    Although it is formally correct to write all three of these units as the reciprocal second, the
    use of the different names emphasizes the different nature of the quantities concerned. It is
    especially important to carefully distinguish frequencies from angular frequencies, because
    by definition their numerical values differ by a factor of $2\pi$. Ignoring this fact may cause
    an error of $2\pi$. Note that in some countries, frequency values are conventionally expressed
    using “cycle/s” or “cps” instead of the SI unit $Hz$, although “cycle” and “cps” are not units
    in the SI. Note also that it is common, although not recommended, to use the term
    frequency for quantities expressed in $rad/s$. Because of this, it is recommended that
    quantities called “frequency”, “angular frequency”, and “angular velocity” always be given
    explicit units of $Hz$ or $rad/s$ and not $s^{-1}$.


## Strong Angular extensions in the library

The mp-units library strives to define physically-correct quantities and their units to provide maximum help
to its users. As treating angle as a dimensional quantity can lead to many "trivial" mistakes in dimensional
analysis and calculation, it was decided to provide additional experimental systems of quantities and units
that follow the above approach and treat _angle_ as a base quantity with a base unit of radian and
_solid angle_ as its derived quantity.

As those (at least for now) are not a part of SI, the _plain angle_ and _solid angle_ definitions can be found
in a dedicated `angular` system. Those definitions are also used in the `isq_angle` system of quantities
to make the recipes for angle-based quantities like _torque_ or _angular velocity_ physically correct:

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using mp_units::angular::unit_symbols::deg;
using mp_units::angular::unit_symbols::rad;

const quantity lever = isq_angle::position_vector(20 * cm);
const quantity force = isq_angle::force(500 * N);
const quantity angle = isq_angle::angular_measure(90. * deg);
const quantity torque = isq_angle::torque(lever * force * angular::sin(angle) / (1 * isq_angle::cotes_angle));

std::cout << "Applying a perpendicular force of " << force << " to a " << lever << " long lever results in "
          << torque.in(N * m / rad) << " of torque.\n";
```

The above program prints:

```text
Applying a perpendicular force of 500 N to a 20 cm long lever results in 100 N m/rad of torque.
```

!!! note

    `cotes_angle` is a constant which represents an angle with the value of exactly `1 radian`.
    You can find more information about this constant in [Quincey](../../appendix/references.md#Quincey).

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/qfxosv9YM)"
