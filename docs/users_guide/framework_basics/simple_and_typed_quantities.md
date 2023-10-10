# Simple and Typed Quantities

ISO specifies a quantity as:

!!! quote

    property of a phenomenon, body, or substance, where the property has a magnitude that can be expressed as a number and a reference

After that, it says:

!!! quote

    A reference can be a measurement unit, a measurement procedure, a reference material, or a combination of such.

## `quantity` class template

In the **mp-units** library, a quantity is represented with the following class template:

```cpp
template<Reference auto R,
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity;
```

The concept `Reference` is satisfied by either:

- a unit with an associated quantity type (e.g. `si::metre`)
- a reference type explicitly specifying the quantity type and its unit.

!!! important

    All units in the SI system have an associated quantity type.

A reference type is implicitly created as a result of the following expression:

```cpp
constexpr auto ref = isq::length[m];
```

The above example resulted in the following type `reference<isq::length(), si::metre()>` being instantiated.

Based on this property, the **mp-units** library provides two modes of dealing with quantities.


## Simple quantities

The **simple mode** might be preferred by many developers. It is all about units. Quantities using this mode
have shorter type identifiers, resulting in easier-to-understand error messages and better debugging experience.

Here is a simple example showing how to deal with such quantities:

```cpp
#include <mp-units/ostream.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

using namespace mp_units;

constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> d,
                                                     quantity<si::second> t)
{
  return d / t;
}

int main()
{
  using namespace mp_units::si::unit_symbols;

  const quantity distance = 110 * km;
  const quantity duration = 2 * h;
  const quantity speed = avg_speed(distance, duration);

  std::cout << "A car driving " << distance << " in " << duration
            << " has an average speed of " << speed
            << " (" << speed.in(km / h) << ")\n";
}
```

The code above prints:

```text
A car driving 110 km in 2 h has an average speed of 15.2778 m/s (55 km/h)
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/zWe8ecf93)"


### Easy to understand compilation error messages

In case a user makes an error in a quantity equation and the result of the calculation
will not match the function return type, the compiler will detect such an issue at
compile-time.

For example, in case we will make the following error:

```cpp hl_lines="4"
constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> d,
                                                     quantity<si::second> t)
{
  return d * t;  // (1)!
}
```

1. Quantities multiplied (instead of divided) by accident.

the following compilation error message will be provided:

```text
In function 'constexpr mp_units::quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::per<mp_units::si::second> >()> avg_speed(mp_units::quantity<mp_units::si::metre()>, mp_units::quantity<mp_units::si::second()>)':
error: could not convert 'mp_units::operator*<si::metre(), double, si::second(), double>(d, t)' from 'quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::si::second>(),[...]>' to 'quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::per<mp_units::si::second> >(),[...]>'
   11 |   return d * t;
      |          ~~^~~
      |            |
      |            quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::si::second>(),[...]>
```

## Typed quantities

Simple mode is all about and just about units. In case you care about a specific quantity type,
**typed quantities** should be preferred. With this mode, for example, you can specify if you
deal with `width`, `height`, or `radius` and ensure you will not assign one to another by
accident.

The previous example can be re-typed using typed quantities in the following way:

```cpp
#include <mp-units/ostream.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

constexpr quantity<isq::speed[m / s]> avg_speed(quantity<isq::length[m]> d,
                                                quantity<isq::time[s]> t)
{
  return d / t;
}

int main()
{
  const quantity distance = isq::distance(110 * km);
  const quantity duration = isq::time(2 * h);
  const quantity speed = avg_speed(distance, duration);

  std::cout << "A car driving " << distance << " in " << duration
            << " has an average speed of " << speed
            << " (" << speed.in(km / h) << ")\n";
}
```

```text
A car driving 110 km in 2 h has an average speed of 15.2778 m/s (55 km/h)
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/q3PzMzqsh)"

In case we will accidentally make the same calculation error as before, this time, we will
get a bit longer error message also containing information about the quantity type:

```log
In function 'constexpr mp_units::quantity<mp_units::reference<mp_units::isq::speed(), mp_units::derived_unit<mp_units::si::metre, mp_units::per<mp_units::si::second> >()>()> avg_speed(mp_units::quantity<mp_units::reference<mp_units::isq::length(), mp_units::si::metre()>()>, mp_units::quantity<mp_units::reference<mp_units::isq::time(), mp_units::si::second()>()>)':
error: could not convert 'mp_units::operator*<reference<isq::length(), si::metre()>(), double, reference<isq::time(), si::second()>(), double>(d, t)' from 'quantity<mp_units::reference<mp_units::derived_quantity_spec<mp_units::isq::length, mp_units::isq::time>(), mp_units::derived_unit<mp_units::si::metre, mp_units::si::second>()>(),[...]>' to 'quantity<mp_units::reference<mp_units::isq::speed(), mp_units::derived_unit<mp_units::si::metre, mp_units::per<mp_units::si::second> >()>(),[...]>'
   12 |   return d * t;
      |          ~~^~~
      |            |
      |            quantity<mp_units::reference<mp_units::derived_quantity_spec<mp_units::isq::length, mp_units::isq::time>(), mp_units::derived_unit<mp_units::si::metre, mp_units::si::second>()>(),[...]>
```

As we can see above, the compilation error is longer but still relatively easy to understand.


### Additional type safety with typed quantities

Based on the previous example, it might seem that typed quantities are not that useful,
more to type and provide harder-to-understand error messages. It might be true in some cases,
but there are cases where they provide an additional level of safety.

Let's see another example:

=== "Simple"

    ```cpp hl_lines="42"
    #include <mp-units/math.h>
    #include <mp-units/systems/si/si.h>
    #include <numbers>

    using namespace mp_units;

    class StorageTank {
      quantity<square(si::metre)> base_;
      quantity<si::metre> height_;
    public:
      constexpr StorageTank(const quantity<square(si::metre)>& base,
                            const quantity<si::metre>& height) :
        base_(base), height_(height)
      {
      }

      // ...
    };

    class CylindricalStorageTank : public StorageTank {
    public:
      constexpr CylindricalStorageTank(const quantity<si::metre>& radius,
                                       const quantity<si::metre>& height) :
        StorageTank(std::numbers::pi * pow<2>(radius), height)
      {
      }
    };

    class RectangularStorageTank : public StorageTank {
    public:
      constexpr RectangularStorageTank(const quantity<si::metre>& length,
                                       const quantity<si::metre>& width,
                                       const quantity<si::metre>& height) :
        StorageTank(length * width, height)
      {
      }
    };

    int main()
    {
      using namespace mp_units::si::unit_symbols;
      auto tank = RectangularStorageTank(1'000 * mm, 500 * mm, 200 * mm);
      // ...
    }
    ```

=== "Typed"

    ```cpp hl_lines="53 54 55"
    #include <mp-units/math.h>
    #include <mp-units/systems/isq/space_and_time.h>
    #include <mp-units/systems/si/si.h>
    #include <numbers>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    // add a custom quantity type of kind isq::length
    inline constexpr struct horizontal_length
        : quantity_spec<isq::length> {} horizontal_length;

    // add a custom derived quantity type of kind isq::area
    // with a constrained quantity equation
    inline constexpr struct horizontal_area
        : quantity_spec<isq::area, horizontal_length * isq::width> {} horizontal_area;

    class StorageTank {
      quantity<horizontal_area[m2]> base_;
      quantity<isq::height[m]> height_;
    public:
      constexpr StorageTank(const quantity<horizontal_area[m2]>& base,
                            const quantity<isq::height[m]>& height) :
        base_(base), height_(height)
      {
      }

      // ...
    };

    class CylindricalStorageTank : public StorageTank {
    public:
      constexpr CylindricalStorageTank(const quantity<isq::radius[m]>& radius,
                                       const quantity<isq::height[m]>& height) :
        StorageTank(quantity_cast<horizontal_area>(std::numbers::pi * pow<2>(radius)),
                    height)
      {
      }
    };

    class RectangularStorageTank : public StorageTank {
    public:
      constexpr RectangularStorageTank(const quantity<horizontal_length[m]>& length,
                                       const quantity<isq::width[m]>& width,
                                       const quantity<isq::height[m]>& height) :
        StorageTank(length * width, height)
      {
      }
    };

    int main()
    {
      auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
                                         isq::width(500 * mm),
                                         isq::height(200 * mm));
      // ...
    }
    ```

In the above example, the highlighted call doesn't look that safe anymore in the case
of simple quantities, right? Suppose someone, either by mistake or due to some refactoring,
will call the function with invalid order of arguments. In that case, the program will compile
fine but not work as expected.

Let's see what will happen if we reorder the arguments in the case of typed quantities:

```cpp hl_lines="2 3"
auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
                                   isq::height(200 * mm),
                                   isq::width(500 * mm));
```

This time a compiler provides the following compilation error:

```text
In function 'int main()':
error: no matching function for call to 'RectangularStorageTank::RectangularStorageTank(mp_units::quantity<mp_units::reference<horizontal_length(), mp_units::si::milli_<mp_units::si::metre()>()>(), int>, mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::milli_<mp_units::si::metre()>()>(), int>, mp_units::quantity<mp_units::reference<mp_units::isq::width(), mp_units::si::milli_<mp_units::si::metre()>()>(), int>)'
   47 |                                      isq::width(500 * mm));
      |                                                          ^
note: candidate: 'constexpr RectangularStorageTank::RectangularStorageTank(const mp_units::quantity<mp_units::reference<horizontal_length(), mp_units::si::metre()>()>&, const mp_units::quantity<mp_units::reference<mp_units::isq::width(), mp_units::si::metre()>()>&, const mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::metre()>()>&)'
   35 |   constexpr RectangularStorageTank(const quantity<horizontal_length[m]>& length,
      |             ^~~~~~~~~~~~~~~~~~~~~~
note:   no known conversion for argument 2 from 'mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::milli_<mp_units::si::metre()>()>(), int>' to 'const mp_units::quantity<mp_units::reference<mp_units::isq::width(), mp_units::si::metre()>()>&'
   36 |                                    const quantity<isq::width[m]>& width,
      |                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~
```

What about derived quantities? In the above example, you probably noticed that we also defined
a custom `horizontal_area` quantity of kind `isq::area`. This quantity has the special property
of being implicitly constructible only from the result of the multiplication of quantities of
`horizontal_area` and `isq::width` or the ones that implicitly convert to them.

Based on the above error message, we already know that a quantity of `isq::height` is not implicitly
constructible to the quantity of `isq::width`. This property is transitively passed to derived
quantities using them. If by accident, we will try to create a `StorageTank` base class
in the following way:

```cpp hl_lines="6"
class RectangularStorageTank : public StorageTank {
public:
  constexpr RectangularStorageTank(const quantity<horizontal_length[m]>& length,
                                   const quantity<isq::width[m]>& width,
                                   const quantity<isq::height[m]>& height) :
    StorageTank(length * height, height)
  {
  }
};
```

we will again get a compilation error message like this one:

```text
In constructor 'constexpr RectangularStorageTank::RectangularStorageTank(const mp_units::quantity<mp_units::reference<horizontal_length(), mp_units::si::metre()>()>&, const mp_units::quantity<mp_units::reference<mp_units::isq::width(), mp_units::si::metre()>()>&, const mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::metre()>()>&)':
error: no matching function for call to 'StorageTank::StorageTank(mp_units::quantity<mp_units::reference<mp_units::derived_quantity_spec<horizontal_length, mp_units::isq::height>(), mp_units::derived_unit<mp_units::power<mp_units::si::metre, 2> >()>(), double>, const mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::metre()>()>&)'
   39 |       StorageTank(length * height, height)
      |                                          ^
note: candidate: 'constexpr StorageTank::StorageTank(const mp_units::quantity<mp_units::reference<horizontal_area(), mp_units::derived_unit<mp_units::power<mp_units::si::metre, 2> >()>()>&, const mp_units::quantity<mp_units::reference<mp_units::isq::height(), mp_units::si::metre()>()>&)'
   16 |   constexpr StorageTank(const quantity<horizontal_area[m2]>& base,
      |             ^~~~~~~~~~~
<source>:16:62: note:   no known conversion for argument 1 from 'mp_units::quantity<mp_units::reference<mp_units::derived_quantity_spec<horizontal_length, mp_units::isq::height>(), mp_units::derived_unit<mp_units::power<mp_units::si::metre, 2> >()>(), double>' to 'const mp_units::quantity<mp_units::reference<horizontal_area(), mp_units::derived_unit<mp_units::power<mp_units::si::metre, 2> >()>()>&'
   16 |   constexpr StorageTank(const quantity<horizontal_area[m2]>& base,
      |                         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~
```

!!! tip

    If you need to use various quantities of the same kind, consider using typed quantities
    to bring an additional level of safety to your project.


### `quantity_cast()` to force unsafe conversions

Did you notice the `quantity_cast()` usage in the other child class?

```cpp hl_lines="5"
class CylindricalStorageTank : public StorageTank {
public:
  constexpr CylindricalStorageTank(const quantity<isq::radius[m]>& radius,
                                   const quantity<isq::height[m]>& height) :
    StorageTank(quantity_cast<horizontal_area>(std::numbers::pi * pow<2>(radius)),
                height)
  {
  }
};
```

As `isq::radius` is not convertible to either a `horizontal_length` or `isq::width`,
the derived quantity of `pow<2>(radius)` can't be converted to `horizontal_area` as well.
It would be unsafe to allow such a conversion as not all of the circles lie flat on the
ground, right?

In such a case, the user has to explicitly force such an unsafe conversion with the
help of a `quantity_cast()`. This function name is easy to spot in code reviews or while
searching the project for problems if something goes sideways. In case of unexpected issues
related to quantities, this should be the first function to look for.

!!! tip

    Do not overuse `quantity_cast()`. Use it only when necessary and ensure that the
    requested conversion is exactly what you need in this case.


## Which mode to use in my project?

In case you wonder which mode you should choose for your project, we have good news for you.
Simple and typed quantity modes can be freely mixed with each other. When you use different
quantities of the same kind (e.g. radius, wavelength, altitude, ...), you should probably
reach for typed quantities to bring additional safety for those cases. Otherwise, just use
simple mode for the remaining quantities. The **mp-units** library will do its best to protect
your project based on the information provided.

!!! tip

    You can easily mix simple and typed quantities in your project.
