# The Affine Space

The affine space has two types of entities:

- **_Point_** - a position specified with coordinate values (e.g., location, address, etc.)
- **_Vector_** - the difference between two points (e.g., shift, offset, displacement, duration, etc.)


!!! note

    The _Vector_ described here is specific to the affine space theory and is not the same thing
    as the quantity of a vector character that we discussed in the
    ["Scalars, vectors, and tensors" chapter](character_of_a_quantity.md#scalars-vectors-and-tensors)
    (although, in some cases, those terms may overlap).


## Operations in the affine space

Here are the primary operations one can do in the affine space:

- _Vector_ + _Vector_ -> _Vector_
- _Vector_ - _Vector_ -> _Vector_
- -_Vector_ -> _Vector_
- _Vector_ * Scalar -> _Vector_
- Scalar * _Vector_ -> _Vector_
- _Vector_ / Scalar -> _Vector_
- _Point_ - _Point_ -> _Vector_
- _Point_ + _Vector_ -> _Point_
- _Vector_ + _Point_ -> _Point_
- _Point_ - _Vector_ -> _Point_

!!! important

    It is not possible to:

    - add two _Points_,
    - subtract a _Point_ from a _Vector_,
    - multiply nor divide _Points_ with anything else.


## _Points_ are more common than most of us imagine

_Point_ abstractions should be used more often in the C++ software.
They are not only about _temperature_ or _time_. _Points_ are everywhere around us and should become
more popular in the products we implement. They can be used to implement:

- _temperature_ points,
- timestamps,
- daily _mass_ readouts from the scale,
- _altitudes_ of mountain peaks on a map,
- current _speed_ displayed on a car's speed-o-meter,
- today's _price_ of instruments on the market,
- and many more.

Improving the affine space's _Points_ intuition will allow us to write better and safer software.


## _Vector_ is modeled by `quantity`

Up until now, each time we used a `quantity` in our code, we were modeling some kind of a
difference between two things:

- the _distance_ between two points,
- _duration_ between two time points,
- the difference in _speed_ (even if relative to zero).

As we already know, a `quantity` type provides all operations required for a _Vector_ type in
the affine space.


## _Point_ is modeled by `quantity_point` and `PointOrigin`

In the **mp-units** library the _Point_ abstraction is modelled by:

- [`PointOrigin` concept](concepts.md#PointOrigin) that specifies measurement origin,
- `quantity_point` class template that specifies a _Point_ relative to a specific predefined origin.


### `quantity_point`

The `quantity_point` class template specifies an absolute quantity measured from a predefined
origin:

```cpp
template<Reference auto R,
         PointOriginFor<get_quantity_spec(R)> auto PO = default_point_origin(R),
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity_point;
```

As we can see above, the `quantity_point` class template exposes one additional parameter compared
to `quantity`. The `PO` parameter satisfies a [`PointOriginFor` concept](concepts.md#PointOriginFor)
and specifies the origin of our measurement scale. By default, it is initialized with a quantity's
zeroth point using the following rules:

- if the measurement unit of a quantity specifies its point origin in its definition
  (e.g., degree Celsius), then this point is being used,
- otherwise, an instantiation of `zeroth_point_origin<QuantitySpec>` is being used which
  provides a zeroth point for a specific quantity type.

!!! tip

    The `quantity_point` definition can be found in the `mp-units/quantity_point.h` header file.


### Implicit point origin

Let's assume that Alice goes for a trip driving a car. She likes taking notes about interesting
places that she visits on the road. For every such item, she writes down:

- its name,
- a readout from the car's odometer at the location,
- a current timestamp.

We can implement this in the following way:

```cpp
using std::chrono::system_clock;

struct trip_log_item {
  std::string name;
  quantity_point<isq::distance[km]> odometer;
  quantity_point<si::second> timestamp;
};
using trip_log = std::vector<trip_log_item>;
```

```cpp
trip_log log;

quantity_point timestamp_1{quantity{system_clock::now().time_since_epoch()}};
log.emplace_back("home", quantity_point{1356 * km}, timestamp_1);

// some time passes

quantity_point timestamp_2{quantity{system_clock::now().time_since_epoch()}};
log.emplace_back("castle", quantity_point{1401 * km}, timestamp_2);
```

This is an excellent example of where points are helpful. There is no doubt about the correctness
of their usage in this scenario:

- adding two odometer readouts or two timestamps have no physical sense, and that is why we will
  expect a compile-time error when we try to perform such operations accidentally,
- subtracting two odometer readouts or timestamps is perfectly valid and results in a quantity
  storing the interval value between the two points.

Having such a database, we can print the trip log in the following way:

```cpp
for (const auto& item : log) {
  std::cout << "POI: " << item.name << "\n";
  std::cout << "- Distance from home: " << item.odometer - log.front().odometer;
  std::cout << "- Trip duration from start: " << (item.timestamp - log.front().timestamp).in(non_si::minute);
}
```

Moreover, if Alice had reset the car's trip odometer before leaving home, we could have rewritten
one of the previous lines like that:

```cpp
std::cout << "Distance from home: " << item.odometer.quantity_from_zero();
```

The above always returns a quantity measured from the "ultimate" zeroth point of a scale used for
this specific quantity type.

!!! tip

    Storing _Points_ is the most efficient representation we can choose in this scenario:

    - to store a value, we read it directly from the instrument, and no additional transformation
      is needed,
    - to print the absolute value (e.g., odometer), we have the value available right away,
    - to get any relative quantity (e.g., distance from the start, distance from the previous point,
      etc.), we have to perform a single subtraction operation.

    If we stored _Vectors_ in our database instead, we would have to pay at runtime for additional
    operations:

    - to store a quantity, we would have to perform the subtraction right away to get the interval
      between the current value and some reference point,
    - to print the absolute value, we would have to add the quantity to the reference point that
      we need to store somewhere in the database as well,
    - to get a relative quantity, only the currently stored one is immediate; all other values
      will require at least one quantity addition operation.

Now, let's assume that Bob, a friend of Alice, also keeps a log of his trips but he, of
course, measures distances from his own home with the odometer in his car. Everything is fine as
long as we deal with one trip at a time, but if we start to work with both at once, we may
accidentally subtract points from different trips. The library will not prevent
us from doing so.

The points from Alice's and Bob's trips should be considered separate, and to enforce it at
compilation time, we need to introduce explicit origins.


### Absolute _Point_ origin

The **absolute point origin** specifies the "zero" of our measurement's scale. User can
specify such an origin by deriving from the `absolute_point_origin` class template:

```cpp
enum class actor { alice, bob };

template<actor A>
struct zeroth_odometer_t : absolute_point_origin<zeroth_odometer_t<A>, isq::distance> {};

template<actor A>
inline constexpr zeroth_odometer_t<A> zeroth_odometer;
```

!!! info

    The `absolute_point_origin` class template uses the CRTP idiom to enforce the uniqueness of
    such a type. You should pass the type of a derived class as the first argument of the template
    instantiation.

*[CRTP]: Curiously Recurring Template Parameter

!!! note

    Unfortunately, due to inconsistencies in C++ language rules:

    - we can't define the above in one line of code,
    - provide the same identifier for a class and variable template.

Odometer is not the only one that can get an explicit point origin in our case. As timestamps are
provided by the `std::chrono::system_clock`, their values are always relative to the epoch of this
clock.

!!! note

    The **mp-units** library provides means to specify
    [interoperability with other units libraries](../use_cases/interoperability_with_other_libraries.md).
    It also has built-in compatibility with `std::chrono` types, so users do not have to define
    interoperability traits or point origins for such types by themselves. Those are already
    provided in the `mp-units/chrono.h` header file.


Now, we can refactor our database to benefit from the explicit points:

```cpp
template<actor A>
struct trip_log_item {
  std::string point_name;
  quantity_point<si::kilo<si::metre>, zeroth_odometer<A>> odometer;
  quantity_point<si::second, chrono_point_origin<system_clock>> timestamp;
};

template<actor A>
using trip_log = std::vector<trip_log_item<A>>;
```

We also need to update the initialization part in our code. In the case of implicit zeroth origins,
we could construct `quantity_point` directly from the value of a `quantity`. This is no longer
the case.
As a _Point_ can be represented with a _Vector_ from the origin, to improve the safety of the code
we write, a `quantity_point` class template must be created with one of the following operations:

```cpp
quantity_point qp1 = zeroth_odometer<actor::alice> + 1356 * km;
quantity_point qp2 = 1356 * km + zeroth_odometer<actor::alice>;
quantity_point qp3 = zeroth_odometer<actor::alice> - 1356 * km;
```

Although, the `qp3` above does not have a physical sense in this specific scenario.

!!! note

    [It is not allowed to subtract a _Point_ from a _Vector_](#operations-in-the-affine-space)
    thus `1356 * km - zeroth_odometer<actor::alice>` is an invalid operation.

!!! info

    A rationale for this longer construction syntax can be found in the
    [Why can't I create a quantity by passing a number to a constructor?](../../getting_started/faq.md#why-cant-i-create-a-quantity-by-passing-a-number-to-a-constructor)
    chapter.

Similarly to [creation of a quantity](../../getting_started/quick_start.md#creating-a-quantity),
if someone does not like the operator-based syntax to create a `quantity_point`, the same results
can be achieved with a two-parameter constructor:

```cpp
quantity_point qp4{1356 * km, zeroth_odometer<actor::alice>};
```

Also, as now our timestamps have a proper point origin provided in a type, we can simplify the
previous code by directly converting `std::chrono::time_point` to our `quantity_point` type.

With all the above, we can refactor our initialization part to the following:

```cpp
trip_log<actor::alice> alice_log;

alice_log.emplace_back("home", zeroth_odometer<actor::alice> + 1356 * km, system_clock::now());

// some time passes

alice_log.emplace_back("castle", zeroth_odometer<actor::alice> + 1401 * km, system_clock::now());
```


### _Point_ arithmetics

As another example, let's assume we will attend the CppCon conference hosted in Aurora, CO,
and we want to estimate the distance we will travel. We have to take a taxi to a local airport,
fly to DEN airport with a stopover in FRA, and, in the end, get a cab to the Gaylord Rockies
Resort & Convention Center:

```cpp
constexpr struct home : absolute_point_origin<home, isq::distance> {} home;

quantity_point<isq::distance[km], home> home_airport = home + 15 * km;
quantity_point<isq::distance[km], home> fra_airport = home_airport + 829 * km;
quantity_point<isq::distance[km], home> den_airport = fra_airport + 8115 * km;
quantity_point<isq::distance[km], home> cppcon_venue = den_airport + 10.1 * mi;
```

As we can see above, we can easily get a new point by adding a quantity to an origin or another
quantity point.

If we want to find out the distance traveled between two points, we simply subtract them:

```cpp
quantity<isq::distance[km]> total = cppcon_venue - home;
quantity<isq::distance[km]> flight = den_airport - home_airport;
```

If we would like to find out the total distance traveled by taxi as well, we have to do a bit
more calculations:

```cpp
quantity<isq::distance[km]> taxi1 = home_airport - home;
quantity<isq::distance[km]> taxi2 = cppcon_venue - den_airport;
quantity<isq::distance[km]> taxi = taxi1 + taxi2;
```

Now, if we print the results:

```cpp
std::cout << "Total distance:  " << total << "\n";
std::cout << "Flight distance: " << flight << "\n";
std::cout << "Taxi distance:   " << taxi << "\n";
```

we will see the following output:

```text
Total distance:  8975.25 km
Flight distance: 8944 km
Taxi distance:   31.2544 km
```

!!! note

    It is not allowed to subtract two point origins defined in terms of `absolute_point_origin`
    (e.g., `home - home`) as those do not contain information about the unit, so we are not able
    to determine a resulting `quantity` type.


### Relative _Point_ origin

We often do not have only one ultimate "zero" point when we measure things.

For example, let's assume that we have the following absolute point origin:

```cpp
constexpr struct mean_sea_level : absolute_point_origin<mean_sea_level, isq::altitude> {} mean_sea_level;
```

If we want to model a trip to Mount Everest, measuring all daily hikes from the `mean_sea_level`
might not be efficient. We may know that we are not good climbers, so all our climbs can be
represented with an 8-bit integer type, allowing us to save memory in our database of climbs.

For this purpose, we can define a `relative_point_origin` in the following way:

```cpp
constexpr struct everest_base_camp : relative_point_origin<mean_sea_level + 5364 * m> {} everest_base_camp;
```

The above can be used as an origin for subsequent _Points_:

```cpp
constexpr quantity_point first_climb_alt = everest_base_camp + isq::altitude(std::uint8_t{42} * m);
static_assert(first_climb_alt.quantity_from(everest_base_camp) == 42 * m);
static_assert(first_climb_alt.quantity_from(mean_sea_level) == 5406 * m);
static_assert(first_climb_alt.quantity_from_zero() == 5406 * m);
```

As we can see above, the `quantity_from()` member function returns a relative distance from the
provided point origin while the `quantity_from_zero()` returns the distance from the absolute point
origin.


### Converting between different representations of the same _Point_

As we might represent the same _Point_ with _Vectors_ from various origins, the **mp-units** library
provides facilities to convert the _Point_ to `quantity_point` class templates expressed in
terms of origins relative to each other in the type system.

For this purpose, we can use:

- a converting constructor:

    ```cpp
    constexpr quantity_point<isq::altitude[m], mean_sea_level, int> qp = first_climb_alt;
    static_assert(qp.quantity_ref_from(qp.point_origin) == 5406 * m);
    ```

- a dedicated conversion interface:

    ```cpp
    constexpr quantity_point qp = first_climb_alt.point_for(mean_sea_level);
    static_assert(qp.quantity_ref_from(qp.point_origin) == 5406 * m);
    ```

!!! note

    It is only allowed to convert between various origins defined in terms of the same
    `absolute_point_origin`. Even if it is theoretically possible to express the same _Point_ as
    a _Vector_ from another `absolute_point_origin`, the library will not allow such a conversion.
    A custom user-defined conversion function will be needed to add this functionality.

    Said otherwise, in the **mp-units** library, there is no way to spell how two distinct
    `absolute_point_origin` types relate to each other.


### Temperature support

Another important example of [relative point origins](#relative-point-origins) is the support
of temperature quantity points. The **mp-units** library provides a few predefined point origins
for this purpose:

```cpp
namespace si {

inline constexpr struct absolute_zero : absolute_point_origin<absolute_zero, isq::thermodynamic_temperature> {} absolute_zero;
inline constexpr struct zeroth_kelvin : decltype(absolute_zero) {} zeroth_kelvin;

inline constexpr struct ice_point : relative_point_origin<quantity_point{273'150 * milli<kelvin>}> {} ice_point;
inline constexpr struct zeroth_degree_Celsius : decltype(ice_point) {} zeroth_degree_Celsius;

}

namespace usc {

inline constexpr struct zeroth_degree_Fahrenheit :
  relative_point_origin<si::zeroth_degree_Celsius - 32 * (mag<ratio{5, 9}> * si::degree_Celsius)> {} zeroth_degree_Fahrenheit;

}
```

The above is a great example of how point origins can be stacked on top of each other:

- `usc::zeroth_degree_Fahrenheit` is defined relative to `si::zeroth_degree_Celsius`
- `si::zeroth_degree_Celsius` is defined relative to `si::zeroth_kelvin`.

!!! note

    Notice that while stacking point origins, we can use not only different representation types
    but also different units for origins and a _Point_. In the above example, the relative
    point origin for degree Celsius is defined in terms of `si::kelvin`, while the quantity point
    for it will use `si::degree_Celsius` as a unit.

The temperature point origins defined above are provided explicitly in the respective units'
definitions:

```cpp
namespace si {

inline constexpr struct kelvin :
    named_unit<"K", kind_of<isq::thermodynamic_temperature>, zeroth_kelvin> {} kelvin;
inline constexpr struct degree_Celsius :
    named_unit<basic_symbol_text{"°C", "`C"}, kelvin, zeroth_degree_Celsius> {} degree_Celsius;

}

namespace usc {

inline constexpr struct degree_Fahrenheit :
    named_unit<basic_symbol_text{"°F", "`F"}, mag<ratio{5, 9}> * si::degree_Celsius,
               zeroth_degree_Fahrenheit> {} degree_Fahrenheit;

}
```

Now let's see how we can benefit from the above definitions. We have quite a few alternatives to
choose from here. Depending on our needs or taste we can:

- be explicit about the unit and origin:

    ```cpp
    quantity_point<si::degree_Celsius, si::zeroth_degree_Celsius> q1 = si::zeroth_degree_Celsius + 20.5 * deg_C;
    quantity_point<si::degree_Celsius, si::zeroth_degree_Celsius> q2 = {20.5 * deg_C, si::zeroth_degree_Celsius};
    quantity_point<si::degree_Celsius, si::zeroth_degree_Celsius> q3{20.5 * deg_C};
    ```

- specify a unit and use its zeroth point origin implicitly:

    ```cpp
    quantity_point<si::degree_Celsius> q4 = si::zeroth_degree_Celsius + 20.5 * deg_C;
    quantity_point<si::degree_Celsius> q5 = {20.5 * deg_C, si::zeroth_degree_Celsius};
    quantity_point<si::degree_Celsius> q6{20.5 * deg_C};
    ```

- benefit from CTAD:

    ```cpp
    quantity_point q7 = si::zeroth_degree_Celsius + 20.5 * deg_C;
    quantity_point q8 = {20.5 * deg_C, si::zeroth_degree_Celsius};
    quantity_point q9{20.5 * deg_C};
    ```

*[CTAD]: Class Template Argument Deduction

In all of the above cases, we end up with the `quantity_point` of the same type and value.

To play a bit more with temperatures, we can implement a simple room AC temperature controller in
the following way:

```cpp
constexpr struct room_reference_temp : relative_point_origin<quantity_point{21 * deg_C}> {} room_reference_temp;
using room_temp = quantity_point<isq::Celsius_temperature[deg_C], room_reference_temp>;

constexpr auto step_delta = isq::Celsius_temperature(0.5 * deg_C);
constexpr int number_of_steps = 6;

room_temp room_ref{};
room_temp room_low = room_ref - number_of_steps * step_delta;
room_temp room_high = room_ref + number_of_steps * step_delta;

std::println("Room reference temperature: {} ({}, {})\n",
             room_ref.quantity_from_zero(),
             room_ref.in(usc::degree_Fahrenheit).quantity_from_zero(),
             room_ref.in(si::kelvin).quantity_from_zero());

std::println("| {:<14} | {:^18} | {:^18} | {:^18} |",
             "Temperature", "Room reference", "Ice point", "Absolute zero");
std::println("|{0:=^16}|{0:=^20}|{0:=^20}|{0:=^20}|", "");

auto print = [&](std::string_view label, auto v) {
  fmt::println("| {:<14} | {:^18} | {:^18} | {:^18} |", label,
               v - room_reference_temp, v - si::ice_point, v - si::absolute_zero);
};

print("Lowest", room_low);
print("Default", room_ref);
print("Highest", room_high);
```

The above prints:

```text
Room reference temperature: 21 °C (69.8 °F, 294.15 K)

| Temperature    |   Room reference   |     Ice point      |   Absolute zero    |
|================|====================|====================|====================|
| Lowest         |       -3 °C        |       18 °C        |     291.15 °C      |
| Default        |        0 °C        |       21 °C        |     294.15 °C      |
| Highest        |        3 °C        |       24 °C        |     297.15 °C      |
```


### No text output for _Points_

The library does not provide a text output for quantity points, as printing just a number and a unit
is not enough to adequately describe a quantity point. Often, an additional prefix or postfix is
required.

For example, the text output of `42 m` may mean many things and can also be confused with an output
of a regular quantity. On the other hand, printing `42 m AMSL` for altitudes above mean sea level is
a much better solution, but the library does not have enough information to print it that way by itself.


## The affine space is about type-safety

The following operations are not allowed in the affine space:

- **adding** two `quantity_point` objects
    - It is physically impossible to add positions of home and Denver airports.
- **subtracting** a `quantity_point` from a `quantity`
    - What would it mean to subtract the DEN airport location from the distance to it?
- **multiplying/dividing** a `quantity_point` with a scalar
    - What is the position of `2 *` DEN airport location?
- **multiplying/dividing** a `quantity_point` with a quantity
    - What would multiplying the distance with the DEN airport location mean?
- **multiplying/dividing** two `quantity_point` objects
    - What would multiplying home and DEN airport location mean?
- **mixing** `quantity_points` of different quantity kinds
    - It is physically impossible to subtract time from length.
- **mixing** `quantity_points` of inconvertible quantities
    - What does subtracting a distance point to DEN airport from the Mount Everest base camp
      altitude mean?
- **mixing** `quantity_points` of convertible quantities but with unrelated origins
    - How do we subtract a point on our trip to CppCon measured relatively to our home location from
      a point measured relative to the center of the Solar System?

!!! important "Important: The affine space improves safety"

    The usage of `quantity_point` and affine space types, in general, improves expressiveness and
    type-safety of the code we write.
