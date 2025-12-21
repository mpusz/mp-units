---
tags:
- Level - Advanced
- System - ISQ
- System - SI
- System - International
- Feature - Affine Space
- Feature - Multi-System Units
- Feature - Runtime Conversion Functions
- Feature - Text Formatting
- Domain - Aerospace
- Domain - Aviation
- Domain - Embedded Systems
---

# UAV: Multiple Altitude Reference Systems

<!-- markdownlint-disable MD013 -->
**Library:**

- [`include/geographic.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/include/geographic.h) - Geographic primitives (MSL altitude, position)

**Example:**

- [`unmanned_aerial_vehicle.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/unmanned_aerial_vehicle.cpp) - UAV altitude reference systems
<!-- markdownlint-enable MD013 -->

This advanced example demonstrates a critical challenge in aviation software: managing
multiple _altitude_ reference systems simultaneously. In aviation, _altitudes_ can be measured
relative to different reference points (mean sea level, ground, ellipsoid, launch point),
and confusing these can have catastrophic consequences. **mp-units** makes these distinctions
type-safe and impossible to mix up.

## The Problem: Multiple Altitude Datums

In aviation and UAV operations, _altitude_ can be expressed relative to several different reference
points:

- **MSL** (Mean Sea Level) - _standard barometric altitude_
- **AGL** (Above Ground Level) - _height above local terrain_
- **HAE** (Height Above Ellipsoid) - _GPS altitude relative to WGS84 ellipsoid_
- **HAL** (Height Above Launch) - _relative altitude from takeoff point_

Each serves a different purpose, and **mixing them is dangerous**. For example:

- ATC (Air Traffic Control) uses MSL for separation
- Obstacle clearance requires AGL
- GPS provides HAE
- UAVs often track HAL for relative navigation

*[UAV]: Unmanned Aerial Vehicle
*[MSL]: Mean Sea Level
*[AGL]: Above Ground Level
*[HAE]: Height Above Ellipsoid
*[HAL]: Height Above Launch

## Type-Safe Altitude References

### Mean Sea Level (MSL)

The example uses MSL altitude from the `geographic` module as the base reference:

```cpp title="geographic.h" linenums="46"
--8<-- "example/include/geographic.h:46:49"
```

This defines the _standard barometric altitude_ reference used in aviation, with custom
formatting that appends "AMSL" (Above Mean Sea Level) to clearly identify the reference system.

### Multiple Earth Gravity Models

_HAE altitudes_ depend on which Earth gravity model is used. The example defines an enum for
different Earth gravity models and a templated point origin:

```cpp title="unmanned_aerial_vehicle.cpp" linenums="49"
--8<-- "example/unmanned_aerial_vehicle.cpp:49:60"
```

The template parameter ensures that _HAE altitudes_ using different models are distinct
types that cannot be accidentally mixed. Each combination of _altitude_ reference and
gravity model creates a unique type, making it impossible to accidentally mix them.

### Height Above Launch

For UAVs, tracking _altitude_ relative to the launch point is critical:

```cpp title="unmanned_aerial_vehicle.cpp" linenums="123"
--8<-- "example/unmanned_aerial_vehicle.cpp:123:126"
```

This creates a completely separate _altitude_ reference system for UAV operations,
with its own absolute point origin distinct from MSL, HAE, or any other reference.

## Converting Between Reference Systems

The type system prevents accidental conversions, but allows intentional ones:

### MSL to HAE Conversion

Converting from MSL to HAE requires accounting for the geoid undulation (the difference between
the ellipsoid and mean sea level at a specific location):

```cpp title="unmanned_aerial_vehicle.cpp" linenums="109"
--8<-- "example/unmanned_aerial_vehicle.cpp:109:117"
```

This conversion:

- Requires _geographic position_ (latitude/longitude)
- Accounts for local geoid undulation
- Returns _altitude_ of the correct HAE type for the specified Earth model

### Relative Altitude Calculations

The UAV class demonstrates safe relative _altitude_ tracking:

```cpp title="unmanned_aerial_vehicle.cpp" linenums="147"
--8<-- "example/unmanned_aerial_vehicle.cpp:147:158"
```

The `quantity_from()` method safely computes the difference between two points in the same
reference system, returning a relative _altitude_ (_height_) quantity.

## Custom Formatting for Each System

Each _altitude_ reference system has custom formatting to make output unambiguous:

```cpp title="unmanned_aerial_vehicle.cpp" linenums="128"
--8<-- "example/unmanned_aerial_vehicle.cpp:128:143"
```

Output clearly shows which reference system is being used:

```text
hal = 1219.20 m HAL
agl = 2925.00 m
EPPR: 54.24772° N 18.6745° E, 4.88 m AMSL, -24.61 m HAE(EGM2008-1)
```

## Practical UAV Operations

The example shows realistic UAV scenarios:

```cpp title="unmanned_aerial_vehicle.cpp" linenums="163"
--8<-- "example/unmanned_aerial_vehicle.cpp:163:182"
```

Each _altitude_ is:

- **Type-safe** - Cannot be accidentally mixed
- **Self-documenting** - The type tells you which reference system
- **Correctly formatted** - Output makes the reference system explicit
- **Efficiently stored** - Zero runtime overhead for the type safety

## Real-World Safety Impact

In actual UAV operations, _altitude_ confusion can cause:

- **Crashes** - Flying into terrain thinking you're higher than you are
- **Airspace violations** - Using wrong _altitude_ reference for clearances
- **Loss of aircraft** - GPS (HAE) vs barometric (MSL) confusion
- **Regulatory violations** - Incorrect _altitude_ reporting

**mp-units** makes these errors **impossible at compile-time**:

```cpp
msl_altitude msl = mean_sea_level + 1000 * m;
hal_altitude hal = height_above_launch + 500 * m;

// auto sum = msl + hal;              // ✗ Compile error!
// auto diff = msl - hal;             // ✗ Compile error!
// if (msl < hal) { /* ... */ }       // ✗ Compile error!
```

The compiler prevents all accidental mixing of _altitude_ references.

## Key Features Demonstrated

### 1. Multiple Absolute Point Origins

Different origins create incompatible types, preventing dangerous mistakes:

- `mean_sea_level` (MSL)
- `height_above_ellipsoid<Model>` (HAE, templated by gravity model)
- `height_above_launch` (HAL)

### 2. Template Parameters for Configuration

```cpp
template<earth_gravity_model M>
struct height_above_ellipsoid_t final : absolute_point_origin<isq::altitude> {
  static constexpr earth_gravity_model egm = M;
};
```

The Earth model becomes part of the type, ensuring that HAE _altitudes_ using different models
cannot be confused.

### 3. Safe Relative Calculations

```cpp
quantity<isq::altitude[m]> hal = current_.quantity_from(launch_);
```

The `quantity_from()` method safely computes relative _altitudes_ between points in the same
reference system.

### 4. Custom Formatting Per Reference

Each _altitude_ type has specialized formatting showing its reference system,
making output unambiguous and safe to interpret.

### 5. Integration with Geographic Systems

The example integrates with geographic coordinates and Earth models. The geoid undulation
calculation is simplified (using a stub function that returns a constant value), but in
production code this would call a proper geodetic library like
[GeographicLib](https://geographiclib.sourceforge.io/).

## Key Takeaways

- Multiple _altitude_ references are a critical safety issue in aviation
- **mp-units** makes mixing different references a compile error
- Type safety extends to template parameters (gravity models)
- Zero runtime overhead for this additional safety
- Custom formatting prevents interpretation errors
- The type system documents which reference is being used
- Integration with geographic libraries is seamless

For UAV and aviation software, this level of compile-time safety is invaluable and can
literally save lives by preventing _altitude_ confusion errors.
