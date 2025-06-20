# Simple and Typed Quantities

ISO defines a quantity as:

!!! quote

    property of a phenomenon, body, or substance, where the property has a magnitude
    that can be expressed as a number and a reference

After that, it says:

!!! quote

    A reference can be a measurement unit, a measurement procedure, a reference material,
    or a combination of such.

## `quantity` class template

In the **mp-units** library, a quantity is represented with the following class template:

```cpp
template<Reference auto R,
         RepresentationOf<get_quantity_spec(R)> Rep = double>
class quantity;
```

The concept `Reference` is satisfied by a type that provides all the domain-specific metadata describing
a quantity (besides the representation type and its value). Such a type can be either:

- a unit with an associated quantity type (e.g., `si::metre`, `m / s`),
- a reference type explicitly specifying the quantity type and its unit.

!!! important

    All units in the SI system have an associated quantity type.

A reference type is implicitly created as a result of the following expression:

```cpp
constexpr auto ref = isq::length[m];
```

The above example results in the following type `reference<isq::length(), si::metre()>` being instantiated.

As we have two alternative options that satisfy the `Reference` concept in the **mp-units** library,
we also have two modes of dealing with quantities.


## Simple quantities

The **simple mode** might be preferred by many developers. It is all about units. Quantities using this mode
have shorter type identifiers, resulting in easier-to-understand error messages and better debugging experience.

Here is a simple example showing how to deal with such quantities:

=== "C++ modules"

    ```cpp
    #include <print>
    import mp_units;

    using namespace mp_units;

    constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> dist,
                                                         quantity<si::second> time)
    {
      return dist / time;
    }

    int main()
    {
      using namespace mp_units::si::unit_symbols;

      const quantity distance = 110 * km;
      const quantity duration = 2 * h;
      const quantity speed = avg_speed(distance, duration);

      std::println("A car driving {} in {} has an average speed of {::N[.4]} ({::N[.4]})",
                   distance, duration, speed, speed.in(km / h));
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <print>

    using namespace mp_units;

    constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> dist,
                                                         quantity<si::second> time)
    {
      return dist / time;
    }

    int main()
    {
      using namespace mp_units::si::unit_symbols;

      const quantity distance = 110 * km;
      const quantity duration = 2 * h;
      const quantity speed = avg_speed(distance, duration);

      std::println("A car driving {} in {} has an average speed of {::N[.4]} ({::N[.4]})",
                   distance, duration, speed, speed.in(km / h));
    }
    ```

The code above prints:

```text
A car driving 110 km in 2 h has an average speed of 15.28 m/s (55 km/h)
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/8EPTh8YrE)"


### User-provided unit wrappers

Sometimes it might be awkward to type some derived units:

```cpp
quantity speed = 60 * km / h;
```

In case such a unit is used a lot in the project, a user can easily provide a nicely named
wrapper for it with:

```cpp
constexpr auto kmph = km / h;
quantity speed = 60 * kmph;
```


### Easy-to-understand compilation error messages

In case a user makes an error in a quantity equation and the result of the calculation
will not match the function return type, the compiler will detect such an issue at
compile-time.

For example, in case we will make the following error:

```cpp hl_lines="4"
constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> dist,
                                                     quantity<si::second> time)
{
  return dist * time;  // (1)!
}
```

1. Quantities multiplied (instead of divided) by accident.

the following compilation error message will be provided:

```text
error: no viable conversion from returned value of type
       'quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::si::second>{{{}}}, [...]>'
       to function return type
       'quantity<mp_units::derived_unit<mp_units::si::metre, mp_units::per<mp_units::si::second>>{{{}}}, [...]>'
   10 |   return dist * time;
      |          ^~~~~~~~~~~
```

## Typed quantities

Simple mode is all about and just about units. **Typed quantities** should be preferred if we also
want to be **quantity-safe**. This, for example, allows us to specify if we deal with _width_,
_height_, or _radius_ and ensure we will not assign one to another by accident.

The previous example can be re-typed using typed quantities in the following way:

=== "C++ modules"

    ```cpp
    #include <print>
    import mp_units;

    using namespace mp_units;

    constexpr quantity<isq::speed[si::metre / si::second]> avg_speed(quantity<isq::length[si::metre]> dist,
                                                                     quantity<isq::time[si::second]> time)
    {
      return dist / time;
    }

    int main()
    {
      using namespace mp_units::si::unit_symbols;

      const quantity distance = isq::distance(110 * km);
      const quantity duration = isq::time(2 * h);
      const quantity speed = avg_speed(distance, duration);

      std::println("A car driving {} in {} has an average speed of {::N[.4]} ({::N[.4]})",
                   distance, duration, speed, speed.in(km / h));
    }
    ```

=== "Header files"

    ```cpp
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <print>

    using namespace mp_units;

    constexpr quantity<isq::speed[si::metre / si::second]> avg_speed(quantity<isq::length[si::metre]> dist,
                                                                     quantity<isq::time[si::second]> time)
    {
      return dist / time;
    }

    int main()
    {
      using namespace mp_units::si::unit_symbols;

      const quantity distance = isq::distance(110 * km);
      const quantity duration = isq::time(2 * h);
      const quantity speed = avg_speed(distance, duration);

      std::println("A car driving {} in {} has an average speed of {::N[.4]} ({::N[.4]})",
                   distance, duration, speed, speed.in(km / h));
    }
    ```

```text
A car driving 110 km in 2 h has an average speed of 15.28 m/s (55 km/h)
```

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/joc4Yn9Mz)"

In case we will accidentally make the same calculation error as before, this time, we will
get a bit longer error message, this time also containing information about the quantity type:

```log
error: no viable conversion from returned value of type
       'quantity<reference<get_quantity_spec(metre{}) * struct time{{{}}}, metre{} * second{{}}>{}, [...]>'
       to function return type
       'quantity<reference<speed{}, derived_unit<metre, per<second>>{}>{}, [...]>'
   12 |   return dist * time;
      |          ^~~~~~~~~~~
```

As we can see above, the compilation error is longer but still relatively easy to understand.


### Quantity-safety with typed quantities

Based on the previous example, it might seem that typed quantities are not that useful,
more to type and provide harder-to-understand error messages. It might be true in some cases,
but there are scenarios where they offer additional level of safety.

Let's see another example:

=== "C++ modules"

    === "Simple"

        ```cpp hl_lines="41"
        #include <numbers>
        import mp_units;

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

        ```cpp hl_lines="51 52 53"
        #include <numbers>
        import mp_units;

        using namespace mp_units;

        // add a custom quantity type of kind isq::length
        inline constexpr struct horizontal_length final :
            quantity_spec<isq::length> {} horizontal_length;

        // add a custom derived quantity type of kind isq::area
        // with a constrained quantity equation
        inline constexpr struct horizontal_area final :
            quantity_spec<isq::area, horizontal_length * isq::width> {} horizontal_area;

        class StorageTank {
          quantity<horizontal_area[square(si::metre)]> base_;
          quantity<isq::height[si::metre]> height_;
        public:
          constexpr StorageTank(const quantity<horizontal_area[square(si::metre)]>& base,
                                const quantity<isq::height[si::metre]>& height) :
            base_(base), height_(height)
          {
          }

          // ...
        };

        class CylindricalStorageTank : public StorageTank {
        public:
          constexpr CylindricalStorageTank(const quantity<isq::radius[si::metre]>& radius,
                                           const quantity<isq::height[si::metre]>& height) :
            StorageTank(quantity_cast<horizontal_area>(std::numbers::pi * pow<2>(radius)),
                        height)
          {
          }
        };

        class RectangularStorageTank : public StorageTank {
        public:
          constexpr RectangularStorageTank(const quantity<horizontal_length[si::metre]>& length,
                                           const quantity<isq::width[si::metre]>& width,
                                           const quantity<isq::height[si::metre]>& height) :
            StorageTank(length * width, height)
          {
          }
        };

        int main()
        {
          using namespace mp_units::si::unit_symbols;
          auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
                                             isq::width(500 * mm),
                                             isq::height(200 * mm));
          // ...
        }
        ```

=== "Header files"

    === "Simple"

        ```cpp hl_lines="42"
        #include <mp-units/math.h>
        #include <mp-units/systems/si.h>
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
        #include <mp-units/systems/isq.h>
        #include <mp-units/systems/si.h>
        #include <numbers>

        using namespace mp_units;

        // add a custom quantity type of kind isq::length
        inline constexpr struct horizontal_length final :
            quantity_spec<isq::length> {} horizontal_length;

        // add a custom derived quantity type of kind isq::area
        // with a constrained quantity equation
        inline constexpr struct horizontal_area final :
            quantity_spec<isq::area, horizontal_length * isq::width> {} horizontal_area;

        class StorageTank {
          quantity<horizontal_area[square(si::metre)]> base_;
          quantity<isq::height[si::metre]> height_;
        public:
          constexpr StorageTank(const quantity<horizontal_area[square(si::metre)]>& base,
                                const quantity<isq::height[si::metre]>& height) :
            base_(base), height_(height)
          {
          }

          // ...
        };

        class CylindricalStorageTank : public StorageTank {
        public:
          constexpr CylindricalStorageTank(const quantity<isq::radius[si::metre]>& radius,
                                           const quantity<isq::height[si::metre]>& height) :
            StorageTank(quantity_cast<horizontal_area>(std::numbers::pi * pow<2>(radius)),
                        height)
          {
          }
        };

        class RectangularStorageTank : public StorageTank {
        public:
          constexpr RectangularStorageTank(const quantity<horizontal_length[si::metre]>& length,
                                           const quantity<isq::width[si::metre]>& width,
                                           const quantity<isq::height[si::metre]>& height) :
            StorageTank(length * width, height)
          {
          }
        };

        int main()
        {
          using namespace mp_units::si::unit_symbols;
          auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
                                             isq::width(500 * mm),
                                             isq::height(200 * mm));
          // ...
        }
        ```

In the above example, the highlighted call doesn't look that safe anymore in the case
of simple quantities, right? Suppose someone, either by mistake or due to some refactoring,
will call the function with an invalid order of arguments. In that case, the program will compile
fine but not work as expected.

Let's see what will happen if we reorder the arguments in the case of typed quantities:

```cpp hl_lines="2 3"
auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
                                   isq::height(200 * mm),
                                   isq::width(500 * mm));
```

This time, a compiler provides the following compilation error:

```text
<source>:53:15: error: no matching constructor for initialization of 'RectangularStorageTank'
   53 |   auto tank = RectangularStorageTank(horizontal_length(1'000 * mm),
      |               ^                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   54 |                                      isq::height(200 * mm),
      |                                      ~~~~~~~~~~~~~~~~~~~~~~
   55 |                                      isq::width(500 * mm));
      |                                      ~~~~~~~~~~~~~~~~~~~~
<source>:43:13: note: candidate constructor not viable: no known conversion from
                'quantity<mp_units::reference<mp_units::isq::height{{{{{}}}}},
                                              mp_units::si::milli_<mp_units::si::metre{{}}>{{{{}}}}>{}, int>' to
                'const quantity<reference<width{}, metre{}>{}, (default) double>' for 2nd argument
   43 |   constexpr RectangularStorageTank(const quantity<horizontal_length[m]>& length,
      |             ^
   44 |                                    const quantity<isq::width[m]>& width,
      |                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```

What about derived quantities? In the above example, you probably noticed that we also defined
a custom `horizontal_area` quantity of kind `isq::area`. This quantity has the unique property
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
error: no matching constructor for initialization of 'StorageTank'
   46 |     StorageTank(length * height, height)
      |     ^           ~~~~~~~~~~~~~~~~~~~~~~~
<source>:22:13: note: candidate constructor not viable: no known conversion from
                'quantity<mp_units::reference<mp_units::derived_quantity_spec<horizontal_length, mp_units::isq::height>{{}, {{}}},
                                              mp_units::derived_unit<mp_units::power<mp_units::si::metre, 2>>{{{}}}>{}, [...]>' to
                'const quantity<reference<horizontal_area{}, derived_unit<power<metre, 2>>{}>{}, [...]>' for 1st argument
   22 |   constexpr StorageTank(const quantity<horizontal_area[m2]>& base,
      |             ^           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

As `isq::radius` is not convertible to `horizontal_length`, the derived quantity of
`pow<2>(radius)` can't be converted to `horizontal_area` as well.
It would be unsafe to allow such a conversion as not all of the circles lie flat on the
ground, right?

In such a case, the user has to explicitly force such an unsafe conversion with the
help of a `quantity_cast()`. This function name is easy to spot in code reviews or while
searching the project for problems if something goes sideways. In case of unexpected
quantities-related issues, this should be the first function to look for.

!!! tip

    Do not overuse `quantity_cast()`. Use it only when necessary and ensure that the
    requested conversion is exactly what you need in this case.


## Which mode should I use in my project?

We have good news for you if you wonder which mode you should choose for your project.
Simple and typed quantity modes can be freely mixed with each other. When you use different
quantities of the same kind (e.g., _radius_, _wavelength_, _altitude_, ...), you should probably
reach for typed quantities to bring additional safety for those cases. Otherwise, just use
simple mode for the remaining quantities. The **mp-units** library will do its best to protect
your project based on the information provided.

!!! tip

    You can easily mix simple and typed quantities in your project.
