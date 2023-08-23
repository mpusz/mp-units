# The Affine Space

The affine space has two types of entities:

- **_point_** - a position specified with coordinate values (i.e. location, address, etc.)
- **_vector_** - the difference between two points (i.e. shift, offset, displacement, duration, etc.)


!!! note

    The _vector_ described here is specific to the affine space theory and is not the same thing
    as the quantity of a vector character that we discussed in the
    ["Scalars, vectors, and tensors" chapter](character_of_a_quantity.md#scalars-vectors-and-tensors)
    (although, in some cases, those terms may overlap).


## Operations in the affine space

Here are the primary operations one can do in the affine space:

- _vector_ + _vector_ -> _vector_
- _vector_ - _vector_ -> _vector_
- -_vector_ -> _vector_
- _vector_ * scalar -> _vector_
- scalar * _vector_ -> _vector_
- _vector_ / scalar -> _vector_
- _point_ - _point_ -> _vector_
- _point_ + _vector_ -> _point_
- _vector_ + _point_ -> _point_
- _point_ - _vector_ -> _point_

!!! note

    It is not possible to:

    - add two _points_,
    - subtract a _point_ from a _vector_,
    - multiply nor divide _points_ with anything else.


## _Vector_ is modeled by `quantity`

Up until now, each time when we used a `quantity` in our code, we were modeling some kind of a
difference between two things:

- the distance between two points
- duration between two time points
- the difference in speed (even if relative to `0`)

As we already know, a `quantity` type provides all operations required for _vector_ type in
the affine space.


## _Point_ is modeled by `PointOrigin` and `quantity_point`

In the **mp-units** library the _point_ abstraction is modelled by:

- [`PointOrigin` concept](basic_concepts.md#PointOrigin) that specifies measurement origin,
- `quantity_point` class template that specifies a _point_ relative to a specific predefined origin.

### Absolute _point_ origin

The **absolute point origin** specifies where the "zero" of our measurement's scale is. User can
specify such an origin by deriving from the `absolute_point_origin` class template:

```cpp
constexpr struct mean_sea_level : absolute_point_origin<isq::altitude> {} mean_sea_level;
```

### `quantity_point`

The `quantity_point` class template specifies an absolute quantity with respect to an origin:

```cpp
template<Reference auto R,
         PointOriginFor<get_quantity_spec(R)> auto PO,
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity_point;
```

As we can see above, the `quantity_point` class template exposes one additional parameter compared
to `quantity`. The `PO` parameter satisfies a [`PointOriginFor` concept](basic_concepts.md#PointOriginFor)
and specifies the origin of our measurement scale.

As a _point_ can be represented with a _vector_ from the origin, a `quantity_point` class
template can be created with the following operations:

```cpp
quantity_point qp1 = mean_sea_level + 42 * m;
quantity_point qp2 = 42 * m + mean_sea_level;
quantity_point qp3 = mean_sea_level - 42 * m;
```

!!! note

    [It is not allowed to subtract a _point_ from a _vector_](#operations-in-the-affine-space)
    thus `42 * m - mean_sea_level` is an invalid operation.

Similarly to [creation of a quantity](../../getting_started/quick_start.md#creating-a-quantity),
if someone does not like the operator-based syntax to create a `quantity_point`, the same results
can be achieved with `make_quantity_point` factory function:

```cpp
quantity_point qp4 = make_quantity_point<mean_sea_level>(42 * m);
quantity_point qp5 = make_quantity_point<mean_sea_level>(-42 * m);
```

The provided `quantity` representing an offset from the origin is stored inside the `quantity_point`
class template and can be obtained with a `quantity_from_origin()` member function:

```cpp
constexpr quantity_point everest_base_camp_alt = mean_sea_level + isq::altitude(5364 * m);
static_assert(everest_base_camp_alt.quantity_from_origin() == 5364 * m);
```


### Relative _point_ origin

We often do not have only one ultimate "zero" point when we measure things.

Continuing the Mount Everest trip example above, measuring all daily hikes from the `mean_sea_level`
might not be efficient. Maybe we know that we are not good climbers, so all our climbs can be
represented with an 8-bit integer type allowing us to save memory in our database of climbs?
Why not use `everest_base_camp_alt` as our reference point?

For this purpose, we can define a `relative_point_origin` in the following way:

```cpp
constexpr struct everest_base_camp : relative_point_origin<everest_base_camp_alt> {} everest_base_camp;
```

The above can be used as an origin for subsequent _points_:

```cpp
constexpr quantity_point first_climb_alt = everest_base_camp + isq::altitude(std::uint8_t{42} * m);
static_assert(first_climb_alt.quantity_from_origin() == 42 * m);
```

As we can see above, the `quantity_from_origin()` member function returns a relative distance from
the current point origin. In case we would like to know the absolute altitude that we reached on this climb,
we can subtract the absolute point origin from the current _point_:

```cpp
static_assert(first_climb_alt - mean_sea_level == 5406 * m);
static_assert(first_climb_alt - first_climb_alt.absolute_point_origin == 5406 * m);
```


### Converting between different representations of the same _point_

As we might represent the same _point_ with _vectors_ from various origins, the **mp-units** library
provides facilities to convert the _point_ to the `quantity_point` class templates expressed in terms
of different origins.

For this purpose, we can either use:

- a converting constructor:

    ```cpp
    constexpr quantity_point<isq::altitude[m], mean_sea_level, int> qp = first_climb_alt;
    static_assert(qp.quantity_from_origin() == 5406 * m);
    ```

- a dedicated conversion interface:

    ```cpp
    constexpr quantity_point qp = first_climb_alt.point_for(mean_sea_level);
    static_assert(qp.quantity_from_origin() == 5406 * m);
    ```

!!! note

    It is only allowed to convert between various origins defined in terms of the same
    `absolute_point_origin`. Even if it is possible to express the same _point_ as a _vector_
    from another `absolute_point_origin`, the library will not provide such a conversion.
    A custom user-defined conversion function will be needed to add this functionality.

    Said otherwise, in the **mp-units** library, there is no way to spell how two distinct
    `absolute_point_origin` types relate to each other.


### _Point_ arithmetics

Let's assume we will attend the CppCon conference hosted in Aurora, CO, and we want to estimate
the distance we will travel. We have to take a taxi to a local airport, fly to DEN airport with
a stopover in FRA, and, in the end, get a cab to the Gaylord Rockies Resort & Convention Center:

```cpp
constexpr struct home : absolute_point_origin<isq::distance> {} home;

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
    (i.e. `mean_sea_level - mean_sea_level`) as those do not contain information about the unit
    so we are not able to determine a resulting `quantity` type.


### Temperature support

Another important example of [relative point origins](#relative-point-origins) is support
of temperature quantity points in units different than kelvin [`K`].

The [SI](../../appendix/glossary.md#si) system definition in the **mp-units** library provides
two predefined point origins:

```cpp
namespace mp_units::si {

inline constexpr struct absolute_zero : absolute_point_origin<isq::thermodynamic_temperature> {} absolute_zero;
inline constexpr struct ice_point : relative_point_origin<absolute_zero + 273.15 * kelvin> {} ice_point;

}
```

With the above, we can be explicit what is the origin of our temperature point. For example, if
we want to implement the degree Celsius scale we can do it as follows:

```cpp
using Celsius_point = quantity_point<isq::Celsius_temperature[deg_C], si::ice_point>;
```

!!! note

    Notice that while stacking point origins, we can use not only different representation types
    but also different units for an origin and a _point_. In the above example, the relative
    point origin is defined in terms of `si::kelvin`, while the quantity point uses
    `si::degree_Celsius`.

To play a bit w temperatures we can implement a simple room's AC temperature controller in
the following way:

```cpp
constexpr struct room_reference_temp : relative_point_origin<si::ice_point + 21 * deg_C> {} room_reference_temp;
using room_temp = quantity_point<isq::Celsius_temperature[deg_C], room_reference_temp>;

constexpr auto step_delta = isq::Celsius_temperature(0.5 * deg_C);
constexpr int number_of_steps = 6;

room_temp room_low = room_reference_temp - number_of_steps * step_delta;
room_temp room_high = room_reference_temp + number_of_steps * step_delta;

std::println("| {:<14} | {:^18} | {:^18} | {:^18} |", "Temperature", "Room reference", "Ice point", "Absolute zero");
std::println("|{0:=^16}|{0:=^20}|{0:=^20}|{0:=^20}|", "");

auto print = [&](std::string_view label, auto v){
  std::println("| {:<14} | {:^18} | {:^18} | {:^18} |",
               label, v - room_reference_temp, v - si::ice_point, v - si::absolute_zero);
};

print("Lowest", room_low);
print("Default", room_reference_temp);
print("Highest", room_high);
```

The above prints:

```text
| Temperature    |   Room reference   |     Ice point      |   Absolute zero    |
|================|====================|====================|====================|
| Lowest         |       -3 °C        |       18 °C        |     291.15 °C      |
| Default        |        0 °C        |       21 °C        |     294.15 °C      |
| Highest        |        3 °C        |       24 °C        |     297.15 °C      |
```


### No text output for _points_

The library does not provide a text output for quantity points, as printing just a number and a unit
is not enough to adequately describe a quantity point. Often, an additional postfix is required.

For example, the text output of `42 m` may mean many things and can also be confused with an output
of a regular quantity. On the other hand, printing `42 m AMSL` for altitudes above mean sea level is
a much better solution, but the library does not have enough information to print it that way by itself.


## The affine space is about type-safety

The following operations are not allowed in the affine space:

- **adding** two `quantity_point` objects
    - It is physically impossible to add positions of home and Denver airports.
- **subtracting** a `quantity_point` from a `quantity`
    - What would it mean to subtract DEN airport location from the distance to it?
- **multiplying/dividing** a `quantity_point` with a scalar
    - What is the position of `2 *` DEN airport location?
- **multiplying/dividing** a `quantity_point` with a quantity
    - What would multiplying the distance with the DEN airport location mean?
- **multiplying/dividing** two `quantity_point` objects
    - What would multiplying home and DEN airport location mean?
- **mixing** `quantity_points` of different quantity kinds
    - It is physically impossible to subtract time from length.
- **mixing** `quantity_points` of inconvertible quantities
    - What does it mean to subtract a distance point to DEN airport from the Mount Everest base camp
      altitude?
- **mixing** `quantity_points` of convertible quantities but with unrelated origins
    - How to subtract a point on our trip to CppCon measured relatively to our home location from
      a point measured relative to the center of the Solar System?

!!! note

    The usage of `quantity_point` and affine space types in general, improves expressiveness and
    type-safety of the code we write.
