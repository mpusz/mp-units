# Interoperability with Other Libraries

**mp-units** makes it easy to cooperate with similar entities of other libraries.
No matter if we want to provide interoperability with a simple home-grown strongly typed
wrapper type (e.g., `Meter`, `Timestamp`, ...) or with a feature-rich quantities and units
library, we have to provide specializations of:

- a `quantity_like_traits` for external `quantity`-like type,
- a `quantity_point_like_traits` for external `quantity_point`-like type.


## Specifying a conversion kind

Before we delve into the template specialization details, let's first decide if we want the
conversions to happen implicitly or if explicit ones would be a better choice. Or maybe
the conversion should be implicit in one direction only (e.g., into **mp-units** abstractions)
while the explicit conversions in the other direction should be preferred?

There is no one unified answer to the above questions. Everything depends on the use case.

Typically, the implicit conversions are allowed in cases where:

- both abstractions mean exactly the same, and interchanging them in the code should not change
  its logic,
- there is no significant runtime overhead introduced by such a conversion (e.g., no need for dynamic
  allocation or copying of huge internal buffers),
- the target type of the conversion provides the same or better safety to the users,
- we prefer the simplicity of implicit conversions over safety during the (hopefully short)
  transition period of refactoring our code base from the usage of one library to the other.

In all other scenarios, we should probably enforce explicit conversions.

The kinds of inter-library conversions can be easily configured in partial specializations
of conversion traits in the **mp-units** library. To require an explicit conversion, the return
type of the conversion function should be wrapped in `convert_explicitly<T>`. Otherwise,
`convert_implicitly<T>` should be used.


## Quantities conversions

For example, let's assume that some company has its own `Meter` strong-type wrapper:

```cpp
struct Meter {
  int value;
};
```

As every usage of `Meter` is at least as good and safe as the usage of `quantity<si::metre, int>`,
and as there is no significant runtime performance penalty, we would like to allow the conversion
to `mp_units::quantity` to happen implicitly.

On the other hand, the `quantity` type is much safer than the `Meter`, and that is why we would prefer
to see the opposite conversions stated explicitly in our code.

To enable such interoperability, we must define a partial specialization of
the `quantity_like_traits<T>` type trait. Such specialization should provide:

- static data member `reference` that provides the quantity reference (e.g., unit),
- `rep` type that specifies the underlying storage type,
- `to_numerical_value(T)` static member function returning a quantity's raw value of `rep` type
  packed in either `convert_explicitly` or `convert_implicitly` wrapper.
- `from_numerical_value(rep)` static member function returning `T` packed in either `convert_explicitly`
  or `convert_implicitly` wrapper.

For example, for our `Meter` type, we could provide the following:

```cpp
template<>
struct mp_units::quantity_like_traits<Meter> {
  static constexpr auto reference = si::metre;
  using rep = decltype(Meter::value);
  static constexpr convert_implicitly<rep> to_numerical_value(Meter m) { return m.value; }
  static constexpr convert_explicitly<Meter> from_numerical_value(rep v) { return Meter{v}; }
};
```

After that, we can check that the [`QuantityLike`](../framework_basics/concepts.md#QuantityLike)
concept is satisfied:

```cpp
static_assert(mp_units::QuantityLike<Meter>);
```

and we can write the following:

```cpp
void print(Meter m) { std::cout << m.value << " m\n"; }

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  Meter height{42};

  // implicit conversions
  quantity h1 = height;
  quantity<isq::height[m], int> h2 = height;

  std::cout << h1 << "\n";
  std::cout << h2 << "\n";

  // explicit conversions
  print(Meter(h1));
  print(Meter(h2));
}
```

!!! note

    No matter if we decide to use implicit or explicit conversions, the **mp-units** will not
    allow unsafe operations to happen.

    If we extend the above example with unsafe conversions, the code will not compile, and we will
    have to fix the issues first before the conversion may be performed:

    === "Unsafe"

        ```cpp
        quantity<isq::height[m]> h3 = height;
        quantity<isq::height[mm], int> h4 = height;
        quantity<isq::height[km], int> h5 = height;  // Compile-time error (1)

        std::cout << h3 << "\n";
        std::cout << h4 << "\n";
        std::cout << h5 << "\n";

        print(Meter(h3));                            // Compile-time error (2)
        print(Meter(h4));                            // Compile-time error (3)
        print(Meter(h5));
        ```

        1. Truncation of value while converting from meters to kilometers.
        2. Conversion of `double` to `int` is not value-preserving.
        3. Truncation of value while converting from millimeters to meters.

    === "Fixed"

        ```cpp
        quantity<isq::height[m]> h3 = height;
        quantity<isq::height[mm], int> h4 = height;
        quantity<isq::height[km], int> h5 = quantity{height}.force_in(km);

        std::cout << h3 << "\n";
        std::cout << h4 << "\n";
        std::cout << h5 << "\n";

        print(Meter(value_cast<int>(h3)));
        print(Meter(h4.force_in(m)));
        print(Meter(h5));
        ```


## Quantity points conversions

To play with quantity point conversions, let's assume that we have a `Timestamp` strong type in our
codebase, and we would like to start using **mp-units** to work with this abstraction.

```cpp
struct Timestamp {
  int seconds;
};
```

As we described in [The Affine Space](../framework_basics/the_affine_space.md) chapter, timestamps
should be modeled as quantity points rather than regular quantities.

To allow the conversion between our custom `Timestamp` type and the `quantity_point` class template
we need to provide the following in the partial specialization of the `quantity_point_like_traits<T>`
type trait:

- static data member `reference` that provides the quantity point reference (e.g., unit),
- static data member `point_origin` that specifies the absolute point, which is the beginning of
  our measurement scale for our points,
- `rep` type that specifies the underlying storage type,
- `to_quantity(T)` static member function returning the `quantity` being the offset of the point
  from the origin packed in either `convert_explicitly` or `convert_implicitly` wrapper,
- `from_quantity(quantity<reference, rep>)` static member function returning `T` packed in either
  `convert_explicitly` or `convert_implicitly` wrapper.

For example, for our `Timestamp` type, we could provide the following:

```cpp
template<>
struct mp_units::quantity_point_like_traits<Timestamp> {
  static constexpr auto reference = si::second;
  static constexpr auto point_origin = default_point_origin(reference);
  using rep = decltype(Timestamp::seconds);

  static constexpr convert_implicitly<quantity<reference, rep>> to_quantity(Timestamp ts)
  {
    return ts.seconds * si::second;
  }

  static constexpr convert_explicitly<Timestamp> from_quantity(quantity<reference, rep> q)
  {
    return Timestamp(q.numerical_value_ref_in(si::second));
  }
};
```

After that, we can check that the [`QuantityPointLike`](../framework_basics/concepts.md#QuantityPointLike)
concept is satisfied:

```cpp
static_assert(mp_units::QuantityPointLike<Timestamp>);
```

and we can write the following:


```cpp
void print(Timestamp ts) { std::cout << ts.seconds << " s\n"; }

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  Timestamp ts{42};

  // implicit conversion
  quantity_point qp = ts;

  std::cout << qp.quantity_from_zero() << "\n";

  // explicit conversion
  print(Timestamp(qp));
}
```


## Interoperability with the C++ Standard Library

In the C++ standard library, we have two types that handle quantities and model the affine space.
Those are:

- [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration) - specifies
  quantities of time,
- [`std::chrono::time_point`](https://en.cppreference.com/w/cpp/chrono/time_point) - specifies
  quantity points of time.

The **mp-units** library comes with built-in interoperability with those types. It is enough to
include the _mp-units/systems/si/chrono.h_ file to benefit from it. This file provides:

- partial specializations of `quantity_like_traits` and `quantity_point_like_traits` that provide
  support for implicit conversions between `std` and `mp_units` types in both directions,
- `chrono_point_origin<Clock>` point origin for `std` clocks,
- `to_chrono_duration` and `to_chrono_time_point` dedicated conversion functions that result
  in types exactly representing **mp-units** abstractions.

!!! important

    Only a `quantity_point` that uses `chrono_point_origin<Clock>` as its origin can be converted
    to the `std::chrono` abstractions:

    ```cpp
    inline constexpr struct ts_origin final : relative_point_origin<chrono_point_origin<system_clock> + 1 * h> {} ts_origin;
    inline constexpr struct my_origin final : absolute_point_origin<isq::time> {} my_origin;

    quantity_point qp1 = sys_seconds{1s};
    auto tp1 = to_chrono_time_point(qp1);  // OK

    quantity_point qp2 = chrono_point_origin<system_clock> + 1 * s;
    auto tp2 = to_chrono_time_point(qp2);  // OK

    quantity_point qp3 = ts_origin + 1 * s;
    auto tp3 = to_chrono_time_point(qp3);  // OK

    quantity_point qp4 = my_origin + 1 * s;
    auto tp4 = to_chrono_time_point(qp4);  // Compile-time Error (1)

    quantity_point qp5{1 * s};
    auto tp5 = to_chrono_time_point(qp5);  // Compile-time Error (2)
    ```

    1. `my_origin` is not defined in terms of `chrono_point_origin<Clock>`.
    2. `zeroth_point_origin` is not defined in terms of `chrono_point_origin<Clock>`.

Here is an example of how interoperability described in this chapter can be used in practice:

```cpp
using namespace std::chrono;

sys_seconds ts_now = floor<seconds>(system_clock::now());

quantity_point start_time = ts_now;
quantity speed = 925. * km / h;
quantity distance = 8111. * km;
quantity flight_time = distance / speed;
quantity_point exp_end_time = start_time + flight_time;

sys_seconds ts_end = value_cast<int>(exp_end_time.in(s));

auto curr_time = zoned_time(current_zone(), ts_now);
auto mst_time = zoned_time("America/Denver", ts_end);

std::cout << "Takeoff: " << curr_time << "\n";
std::cout << "Landing: " << mst_time << "\n";
```

The above may print the following output:

```cpp
Takeoff: 2023-11-18 13:20:54 UTC
Landing: 2023-11-18 15:07:01 MST
```
