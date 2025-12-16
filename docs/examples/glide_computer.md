---
tags:
  - Level - Advanced
  - System - ISQ
  - System - SI
  - System - International
  - Feature - Affine Space
  - Feature - Custom Representation
  - Feature - Multi-System Units
  - Feature - Custom Quantities
  - Feature - Text Formatting
  - Domain - Aerospace
  - Domain - Aviation
---

# Glide Computer: Tactical Flight Planning

<!-- markdownlint-disable MD013 -->
**Library:**

- [`glide_computer_lib/glide_computer_lib.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/glide_computer_lib/include/glide_computer_lib.h) - Glider performance models and polar curves
- [`glide_computer_lib/glide_computer_lib.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/glide_computer_lib/glide_computer_lib.cpp) - Implementation
- [`include/geographic.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/include/geographic.h) - Geographic primitives (_position_, coordinates, _distance_)
- [`include/ranged_representation.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/include/ranged_representation.h) - Ranged value types for _latitude_/_longitude_

**Example:**

- [`glide_computer.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/glide_computer.cpp) - Flight planning simulation
<!-- markdownlint-enable MD013 -->

This advanced example implements a simplified tactical glide computer for sailplane (glider)
flight planning. It demonstrates how **mp-units** can model a complex real-world aviation
application with multiple interacting subsystems, geographic coordinates, and mixed unit
systems.

## What is a Glide Computer?

A glide computer is an avionic instrument used in soaring (glider) aviation to help pilots
make tactical decisions about:

- Whether they can reach their destination from current _altitude_
- How long a flight will take given weather conditions
- When and where to climb in thermals (rising air columns)
- Optimal _speeds_ to fly between waypoints

This example simulates these calculations for various glider types and weather conditions.

## Domain Modeling

### Custom Quantity Types

The example defines aviation-specific quantities:

```cpp title="glide_computer_lib.h" linenums="67"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:67:67"
```

This creates a distinct quantity type for _vertical speed_ (_rate of climb/sink_),
which is different from _horizontal speed_ even though both have dimensions of
_length_/_time_.

### Type-Safe Aliases

The example uses type aliases for code clarity:

```cpp title="glide_computer_lib.h" linenums="70"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:70:80"
```

These aliases:

- Make code more readable (_height_ vs generic _length_)
- Encode domain intent (these are specific aviation measurements)
- Don't sacrifice type safety (still checked at compile-time)

### Complex Data Structures

The glider is modeled with its performance characteristics:

```cpp title="glide_computer_lib.h" linenums="83"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:83:91"
```

Each point on the polar curve describes the glider's _sink rate_ at a given _airspeed_ -
fundamental aerodynamic data needed for flight planning.

## Geographic Integration

The example uses a separate `geographic` module that provides type-safe geographic primitives.

### Position and Coordinates

```cpp title="geographic.h" linenums="79"
--8<-- "example/include/geographic.h:79:84"
```

_Latitude_ and _longitude_ are modeled as `quantity_point` with:

- Distinct origins (`equator`, `prime_meridian`)
- Range-validated representations (±90° for _latitude_, ±180° for _longitude_)
- Custom user-defined literals (`_N`, `_S`, `_E`, `_W`)

### Great Circle Distance

```cpp title="geographic.h" linenums="173"
--8<-- "example/include/geographic.h:173:175"
```

The `spherical_distance` function calculates the shortest _distance_ between two points
on Earth's surface using the great-circle formula, automatically handling all unit
conversions and trigonometric operations with proper dimensional analysis.

### Waypoint Definition

With these geographic primitives, waypoints become simple and type-safe:

```cpp title="glide_computer_lib.h" linenums="103"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:103:107"
```

Each waypoint contains:

- Name (airport ICAO code)
- Geographic position with _latitude_/_longitude_ using custom literals
- _MSL altitude_ in aviation-standard feet

This shows how **mp-units** integrates with domain-specific abstractions while maintaining
type safety across module boundaries.

## Multi-System Units

The example naturally mixes unit systems as aviators actually do:

```cpp title="glide_computer.cpp" linenums="53"
--8<-- "example/glide_computer.cpp:53:84"
```

- **SI units** (`km/h`, `m/s`) for glider performance
- **Imperial/International units** (`ft`) for _altitude_ (standard in aviation)
- **_Latitude_/_longitude_** in degrees with custom literals

**mp-units** handles these conversions automatically and safely.

## Key Features Demonstrated

### 1. Quantity Points for Absolute Values

```cpp title="glide_computer_lib.h" linenums="75"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:75:76"
```

_Timestamps_ are absolute points in _time_, not _durations_ - correctly modeled using
`quantity_point` with `chrono_point_origin`.

### 2. Custom Quantity Specifications

```cpp
QUANTITY_SPEC(rate_of_climb_speed, mp_units::isq::speed, 
              mp_units::isq::height / mp_units::isq::time);
```

Creates a specialized _speed_ quantity specifically for vertical movement, distinct from
general _speed_.

### 3. Dimensionless Ratios

```cpp title="glide_computer_lib.h" linenums="93"
--8<-- "example/glide_computer_lib/include/glide_computer_lib.h:93:96"
```

The _glide ratio_ (_distance_ traveled per unit _altitude_ lost) is a _dimensionless_ quantity,
correctly computed as a ratio of two _speeds_.

### 4. Complex Calculations

The library performs sophisticated calculations including:

- _Distance_ between geographic coordinates
- _Time-to-climb_ to thermal top
- _Ground speed_ vs _airspeed_
- Safety margins and minimum _altitudes_
- Multi-leg route planning

All with full compile-time dimensional analysis.

### 5. Formatted Output

```cpp title="glide_computer.cpp" linenums="97"
--8<-- "example/glide_computer.cpp:97:98"
```

Custom formatting for all quantity types makes the output readable for pilots.

## Scenario Simulation

The main program runs multiple scenarios:

- **4 different glider types** (from trainer to high-performance)
- **3 weather conditions** (good, medium, bad thermals)
- **Multi-waypoint tasks** with varying distances

For each combination, it calculates whether the flight is possible and estimates the time required.

### Sample Output

The program outputs detailed flight plans with proper unit formatting:

```text
Safety:
=======
- Min AGL separation: 300 m

Gliders:
========
- Name: SZD-56 Diana
- Polar:
  * -0.6366 m/s @ 110.0 km/h -> 48.0 (1.2°)

Waypoints:
==========
- EPPR: 54.24772° N 18.6745° E, 4.9 m AMSL
- EPGI: 53.52442° N 18.84947° E, 35.1 m AMSL

Weather:
========
- Good
  * Cloud base:        1900 m AGL
  * Thermals strength: 4.3 m/s

Task:
=====
- Start: EPPR
- Finish: EPPR
- Length:  162.5 km
- Legs: 
  * EPPR -> EPGI (81.2 km)
  * EPGI -> EPPR (81.2 km)

Tow:
====
- Type:        aircraft
- Height:      400 m
- Performance: 1.6 m/s

Scenario: Glider = SZD-56 Diana, Weather = Good
===============================================

| Flight phase | Duration                    | Distance                  | Height                |
|--------------|-----------------------------|---------------------------|-----------------------|
| Tow          | 4.2 min (Total:   4.2 min)  | 0.0 km (Total:   0.0 km)  | 400 m ( 405 m AMSL)   |
| Glide        | 2.6 min (Total:   6.8 min)  | 4.8 km (Total:   4.8 km)  | -100 m ( 305 m AMSL)  |
| Circle       | 7.3 min (Total:  14.1 min)  | 0.0 km (Total:   4.8 km)  | 1602 m (1907 m AMSL)  |
| Glide        | 41.9 min (Total:  56.0 min) | 76.8 km (Total:  81.6 km) | -1600 m ( 307 m AMSL) |
| Circle       | 6.3 min (Total:  62.3 min)  | 0.0 km (Total:  81.6 km)  | 1383 m (1690 m AMSL)  |
| Final Glide  | 44.1 min (Total: 106.4 min) | 80.8 km (Total: 162.5 km) | -1685 m (   5 m AMSL) |
```

??? note

    The above is just a part of the actual text output.

Each flight phase shows:

- **Duration** - _time_ for this phase and cumulative total
- **Distance** - _distance_ covered and total so far  
- **Height** - _altitude_ change and resulting _MSL altitude_

The output demonstrates:

- **Tow** - initial climb to release _altitude_ (400 m AGL)
- **Glide** - descending flight between thermals
- **Circle** - climbing in thermals to regain _altitude_
- **Final Glide** - direct glide to destination without further thermals

All with automatic unit formatting showing proper aviation notation (km/h, m, m/s, AMSL).

## Practical Applications

This example is close to what a real glide computer might do (though simplified).
Real aviation software needs:

- ✅ **Type safety** - mixing _altitude_ and _distance_ is dangerous
- ✅ **Multiple unit systems** - aviation uses nautical miles, feet, meters, knots, etc.
- ✅ **Exact conversions** - no room for approximation errors
- ✅ **Performance** - zero runtime overhead from the units library
- ✅ **Maintainability** - clear, self-documenting code

**mp-units** provides all of these guarantees.

## Key Takeaways

- Complex real-world applications can be fully typed with **mp-units**
- Custom quantity specifications capture domain-specific knowledge
- Multiple unit systems integrate seamlessly
- Geographic and physical quantities work together naturally
- Type safety scales to large, multi-module codebases
- The library doesn't get in the way of domain logic

This example shows that **mp-units** is production-ready for safety-critical aviation software
where correctness is paramount.
