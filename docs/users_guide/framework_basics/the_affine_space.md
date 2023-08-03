# The Affine Space

The affine space has two types of entities:

- **_point_** - a position specified with coordinate values (i.e. location, address, etc.)
- **_vector_** - the difference between two points (i.e. shift, offset, displacement, duration, etc.)


!!! note

    The _vector_ described here is specific to the affine space theory and is not the same thing
    as the quantity of a vector character that we discussed in the
    ["Scalars, vectors, and tensors" chapter](character_of_a_quantity/#scalars-vectors-and-tensors)
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


## _Point_ is modeled by `quantity_point`

A _point_ is an absolute quantity with respect to an origin and is represented in the library with a
`quantity_point` class template:

```cpp
template<Reference auto R,
         PointOriginFor<get_quantity_spec(R)> auto PO = absolute_point_origin<get_quantity_spec(R)>{},
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity_point;
```

As we can see above, the `quantity_point` class template exposes one additional parameter compared
to `quantity`. The `PO` parameter satisfies a [`PointOriginFor` concept](../basic_concepts/#pointoriginfor)
and specifies the origin of our scale.


### The origin

The **origin** specifies where the "zero" of our measurement's scale is.

Please notice that a _point_ can be represented with a _vector_ from the origin. This is why in
the **mp-units** library, a `quantity_point` gets a `quantity` in its constructor. Such a `quantity`:

- specifies the relative distance of a specific point from the scale origin,
- is the only data member of the `quantity_point` class template,
- can be obtained with the `relative()` member function.

```cpp
constexpr quantity_point<isq::altitude[m]> everest_base_camp_alt{5364 * m};
static_assert(everest_base_camp_alt.relative() == 5364 * m);
```

!!! note

    As the constructor is explicit, the quantity point object can only be created from a quantity via
    direct initialization. This is why the code below that uses copy initialization does not compile:

    ```cpp
    quantity_point<isq::altitude[m]> everest_base_camp_alt = 5364 * m;  // ERROR
    ```

In the **mp-units** library, the origin is either provided implicitly (as above) or can be predefined
by the user and then provided explicitly as the `quantity_point` class template argument:

```cpp
constexpr struct mean_sea_level : absolute_point_origin<isq::altitude> {} mean_sea_level;

constexpr quantity_point<isq::altitude[m], mean_sea_level> everest_base_camp_alt{5364 * m};
static_assert(everest_base_camp_alt.relative() == 5364 * m);
```

!!! note

    The `mean_sea_level` and the default `absolute_point_origin<isq::altitude>` origins are distinct from
    each other, which means that _points_ defined with them are not compatible (can't be subtracted or
    compared).


### Class Template Argument Deduction (CTAD)

Typing the entire `quantity_point` type may sometimes be quite verbose. Also, please note that we
"accidentally" used `double` as a representation type in the above examples, even though we operated
only on integral values. This was done for the convenience of saving typing.

To improve the developer's experience, the `quantity_point` class template comes with the user-defined
class template argument deduction guides. Thanks to them, the above definitions can be rewritten as
follows:

- implicit default origin

    ```cpp
    constexpr quantity_point everest_base_camp_alt{isq::altitude(5364 * m)};
    ```

- explicit origin

    ```cpp
    constexpr quantity_point everest_base_camp_alt{isq::altitude(5364 * m), mean_sea_level};
    ```


### Relative _point_ origins

We often do not have only one ultimate "zero" point when we measure things.

Continuing the Mount Everest trip example above, measuring all daily hikes from the `mean_sea_level`
might not be efficient. Maybe we know that we are not good climbers, so all our climbs can be
represented with an 8-bit integer type which will allow us to save memory in our database of climbs?
Why not use `everest_base_camp_alt` as our reference point?

For this purpose, we can define a `relative_point_origin` in the following way:

```cpp
constexpr struct everest_base_camp : relative_point_origin<everest_base_camp_alt> {} everest_base_camp;
```

The above can be used as an origin for subsequent _points_:

```cpp
constexpr quantity_point<isq::altitude[m], everest_base_camp, std::uint8_t> first_climb_alt{42 * m};
static_assert(first_climb_alt.relative() == 42 * m);
```

As we can see above, the `relative()` member function returns a relative distance from the current
point origin. In case we would like to know the absolute altitude that we reached on this climb,
we can either:

- add the two relative heights from both points

    ```cpp
    static_assert(first_climb_alt.relative() + everest_base_camp_alt.relative() == 5406 * m);
    ```

- call `absolute()` member function

    ```cpp
    static_assert(first_climb_alt.absolute() == 5406 * m);
    ```


### Converting between different representations of the same _point_

As we might represent the same _point_ with _vectors_ from various origins, the **mp-units** library
provides facilities to convert the _point_ to the `quantity_point` class templates expressed in terms
of different origins.

For this purpose, we can either use:

- a converting constructor:

    ```cpp
    static_assert(quantity_point<isq::altitude[m], mean_sea_level>{first_climb_alt}.relative() == 5406 * m);
    ```

- a dedicated conversion interface:

    ```cpp
    constexpr QuantityPoint auto qp = first_climb_alt.point_from(mean_sea_level);
    static_assert(qp.relative() == 5406 * m);
    ```

### _Point_ arithmetics

Let's assume we are going to attend the CppCon conference that is hosted in Aurora, CO, and we
want to estimate the distance we are going to travel. We have to take a taxi to a local airport, fly to
DEN airport with a stopover in FRA, and in the end, get a taxi to the Gaylord Rockies Resort & Convention
Center:

```cpp
constexpr struct home_location : absolute_point_origin<isq::distance> {} home_location;

quantity_point<isq::distance[km], home_location> home{};
quantity_point<isq::distance[km], home_location> home_airport = home + 15 * km;
quantity_point<isq::distance[km], home_location> fra_airport = home_airport + 829 * km;
quantity_point<isq::distance[km], home_location> den_airport = fra_airport + 8115 * km;
quantity_point<isq::distance[km], home_location> cppcon_venue = den_airport + 10.1 * mi;
```

As we can see above, we can easily get a new point by adding a quantity to another quantity point.

If we want to find out the distance traveled between two points, we simply subtract them:

```cpp
quantity<isq::distance[km]> total = cppcon_venue - home;
quantity<isq::distance[km]> flight = den_airport - home_airport;
```

If we would like to find out the total distance traveled by taxi as well, we have to do more
calculations:

```cpp
quantity<isq::distance[km]> taxi1 = home_airport - home;
quantity<isq::distance[km]> taxi2 = cppcon_venue - den_airport;
quantity<isq::distance[km]> taxi = taxi1 + taxi2;
```

Now if we will print the results:

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


### Temperature support

Another important example of [relative point origins](#relative-point-origins) is support
of temperature quantity points in units different than kelvin [`K`].

For example, the degree Celsius scale can be implemented as follows:

```cpp
constexpr struct ice_point : relative_point_origin<quantity_point<isq::thermodynamic_temperature[K]>{273.15 * K}> {} ice_point;
using Celsius_point = quantity_point<isq::thermodynamic_temperature[deg_C], ice_point>;
```

!!! note

    Notice that while stacking point origins we can use not only different representation types but
    also different units for an origin and a _point_.

With the above, for example, if we want to implement a room temperature controller, we can type:

```cpp
constexpr struct room_reference_temperature : relative_point_origin<Celsius_point{21 * deg_C}> {} room_reference_temperature;
using room_temperature = quantity_point<isq::thermodynamic_temperature[deg_C], room_reference_temperature>;

constexpr auto step_delta = isq::thermodynamic_temperature(0.5 * deg_C);
constexpr int number_of_steps = 6;

room_temperature room_default{};
room_temperature room_low = room_default - number_of_steps * step_delta;
room_temperature room_high = room_default + number_of_steps * step_delta;

std::cout << "Lowest temp: " << room_low.relative() << " (" << room_low - Celsius_point::zero() << ")\n";
std::cout << "Highest temp: " << room_high.relative() << " (" << room_high - Celsius_point::zero() << ")\n";
```

The above prints:

```text
Lowest temp: -3 °C (18 °C)
Highest temp: 3 °C (24 °C)
```


## The affine space is about type-safety

The following operations are not allowed in the affine space:

- **add** two `quantity_point` objects (It is physically impossible to add positions of home
  and Denver airports),
- **subtract** a `quantity_point` from a `quantity` (What would it mean to subtract DEN airport
  location from the distance to it?),
- **multiply/divide** a `quantity_point` with a scalar (What is the position of `2x` DEN airport location?).
- **multiply/divide** a `quantity_point` with a quantity (What would multiplying the distance with the
  DEN airport location mean?).
- **multiply/divide** two `quantity_point` objects (What would multiplying home and DEN airport location mean?).
- **mix** `quantity_points` of different quantity kinds (It is physically impossible to subtract time
  from length),
- **mix** `quantity_points` of inconvertible quantities (What does it mean to subtract a distance
  point to DEN airport from the Mount Everest base camp altitude?),
- **mix** `quantity_points` of convertible quantities but with unrelated origins (How to subtract
  a point on our trip to CppCon measured relatively to our home location from a point measured
  relative to the center of the Solar System?).

!!! note

    The usage of `quantity_point`, and affine space types in general, improves expressiveness and
    type-safety of the code we write.
