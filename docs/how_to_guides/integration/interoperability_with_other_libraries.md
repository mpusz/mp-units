# Interoperability with Other Libraries

This guide shows you how to make **mp-units** work seamlessly with other libraries—whether
it's a simple home-grown strongly typed wrapper, a feature-rich units library, or the C++
Standard Library's `std::chrono` types. You'll learn how to specify conversion traits,
control whether conversions are implicit or explicit, and maintain safety at library
boundaries.

For background on the concepts used in these conversions, see
[Concepts](../../users_guide/framework_basics/concepts.md) in the User's Guide.

## What You Need to Provide

To enable interoperability between **mp-units** and external types, you need to provide
specializations of:

- `quantity_like_traits<T>` for external quantity-like types
- `quantity_point_like_traits<T>` for external quantity point-like types

These trait specializations tell **mp-units** how to convert between your types and the
library's types, while maintaining as much safety as possible.

## Choosing Conversion Semantics

### Implicit vs. Explicit Conversions

Before diving into the template specializations, decide whether conversions should be implicit
or explicit. There's no one-size-fits-all answer—it depends on your use case.

**Consider implicit conversions when:**

- Both abstractions mean exactly the same thing
- Interchanging them in code wouldn't change its logic
- There's no significant runtime overhead (no dynamic allocation or large buffer copies)
- The target type provides the same or better safety
- You're refactoring from one library to another and want a smoother transition

**Prefer explicit conversions when:**

- The abstractions have subtle semantic differences
- There's meaningful runtime cost to the conversion
- The target type provides less safety
- You want to make library boundaries visible in code

### Configuring Conversion Direction

Conversion traits control directionality using two flags:

- **`explicit_import`**: Set to `true` to require explicit conversion **from** the external
  type **to** mp-units types (import case)
- **`explicit_export`**: Set to `true` to require explicit conversion **from** mp-units types
  **to** the external type (export case)

If a flag is `false`, conversions in that direction are implicit.

## Integrating Quantity Types

### Example: Simple Strong Type

Let's say your company has a `Meter` strong-type wrapper:

```cpp
struct Meter {
  int value;
};
```

You want conversions **to** `mp_units::quantity` to be implicit (since `Meter` is at least as
safe), but conversions **from** `mp_units::quantity` to `Meter` to be explicit (since `quantity`
provides more safety).

### Step 1: Define the Trait Specialization

Provide a partial specialization of `quantity_like_traits<T>` with these members:

- `reference` - static data member providing the quantity reference (unit)
- `rep` - type specifying the underlying storage type
- `explicit_import` - bool flag controlling import conversions (external → mp-units)
- `explicit_export` - bool flag controlling export conversions (mp-units → external)
- `to_numerical_value(T)` - static function returning the raw value as `rep`
- `from_numerical_value(rep)` - static function constructing `T` from a raw value

```cpp
template<>
struct mp_units::quantity_like_traits<Meter> {
  static constexpr auto reference = si::metre;
  static constexpr bool explicit_import = false;  // Meter → quantity is implicit
  static constexpr bool explicit_export = true;   // quantity → Meter is explicit
  using rep = decltype(Meter::value);

  static constexpr rep to_numerical_value(Meter m) { return m.value; }
  static constexpr Meter from_numerical_value(rep v) { return Meter{v}; }
};
```

### Step 2: Verify the Concept

Check that the [`QuantityLike`](../../users_guide/framework_basics/concepts.md#QuantityLike)
concept is satisfied:

```cpp
static_assert(mp_units::QuantityLike<Meter>);
```

### Step 3: Use the Conversions

```cpp
void print(Meter m) { std::cout << m.value << " m\n"; }

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  Meter height{42};

  // ✅ Implicit conversions (import)
  quantity h1 = height;
  quantity<isq::height[m], int> h2 = height;

  std::cout << h1 << "\n";
  std::cout << h2 << "\n";

  // ✅ Explicit conversions (export)
  print(Meter(h1));
  print(Meter(h2));
}
```

### Safety is Always Enforced

Even if you allow implicit conversions, **mp-units** won't allow unsafe operations. The
library automatically performs unit conversions during interoperability, but only when
no value truncation would occur:

- **Value-preserving conversions work automatically**: Converting from a smaller unit
  to a larger representation (e.g., meters to millimeters for integers) works implicitly
  because the conversion factor preserves all values
- **Truncating conversions require explicit action**: Converting from a finer unit to
  a coarser one with a non-floating-point representation (e.g., millimeters to meters
  for integers) requires `.force_in()` because information would be lost

Examples of compile-time safety checks:

```cpp
Meter height{42};

quantity<isq::height[m]> h3 = height;      // ✅ OK
quantity<isq::height[mm], int> h4 = height;  // ✅ OK

// ❌ Compile error: truncation while converting from meters to kilometers
quantity<isq::height[km], int> h5 = height;

// ❌ Compile error: conversion of double to int not value-preserving
print(Meter(h3));

// ❌ Compile error: truncation while converting from millimeters to meters
print(Meter(h4));
```

**To fix these issues:**

```cpp
// Explicitly allow truncation
quantity<isq::height[km], int> h5 = quantity{height}.force_in(km);

// Force conversion double → int
print(Meter(h3.force_in<int>()));

// Force conversion mm → m
print(Meter(h4.force_in(m)));
```

## Querying Type Information

When working with quantity-like types from other libraries, you may need to extract type
information at compile time. **mp-units** provides variable templates to query:

- `unit_for<T>` - extracts the unit used by a quantity-like type
- `reference_for<T>` - extracts the full reference (unit + quantity specification)
- `rep_for<T>` - extracts the representation type

These utilities are particularly useful in generic code and work with any type that satisfies
`QuantityLike` or `QuantityPointLike`, including `std::chrono` types. They are convenient
accessors that automatically use the underlying `quantity_like_traits` or
`quantity_point_like_traits` you've defined for custom types.

### Basic Usage

```cpp
#include <mp-units/systems/si/chrono.h>
#include <chrono>

using namespace mp_units;

// Query std::chrono types
constexpr Unit auto u1 = unit_for<std::chrono::seconds>;            // si::second
constexpr Unit auto u2 = unit_for<std::chrono::milliseconds>;       // si::milli<si::second>
constexpr Reference auto ref = reference_for<std::chrono::seconds>; // si::second (reference)
using rep = rep_for<std::chrono::nanoseconds>;                      // std::chrono::nanoseconds::rep

// Query mp-units types
constexpr Unit auto u3 = unit_for<quantity<si::metre>>;   // si::metre
using rep2 = rep_for<quantity<si::metre, int>>;      // int
```

### Generic Code Example

These utilities enable writing generic algorithms that work with both **mp-units** and
`std::chrono` types:

```cpp
template<typename Duration>
Duration compute_eta(QuantityOf<isq::distance> auto remaining_distance,
                     QuantityOf<isq::speed> auto avg_speed)
{
  // Calculate time needed using mp-units dimensional analysis
  quantity eta = remaining_distance / avg_speed;

  // Convert to the specific duration type expected by the caller
  // Requires querying unit and representation type
  return value_cast<rep_for<Duration>, unit_for<Duration>>(eta);
}

// Works with both mp-units and std::chrono:
using car_clock = std::chrono::system_clock;
auto d1 = compute_eta<car_clock::duration>(50 * km, 100 * km / h);  // std::chrono::duration
auto d2 = compute_eta<quantity<s, int>>(50 * km, 100 * km / h);     // mp-units quantity
```

## Integrating Quantity Point Types

### Example: Timestamp Type

Suppose you have a `Timestamp` strong type representing time points:

```cpp
struct Timestamp {
  int seconds;
};
```

As discussed in [The Affine Space](../../users_guide/framework_basics/the_affine_space.md),
timestamps should be modeled as quantity points rather than regular quantities.

<!-- markdownlint-disable-next-line MD024 -->
### Step 1: Define the Trait Specialization

Provide a partial specialization of `quantity_point_like_traits<T>` with these members:

- `reference` - static data member providing the quantity point reference (unit)
- `point_origin` - static data member specifying the absolute origin point
- `rep` - type specifying the underlying storage type
- `explicit_import` - bool flag controlling import conversions
- `explicit_export` - bool flag controlling export conversions
- `to_numerical_value(T)` - static function returning raw value of the quantity offset
- `from_numerical_value(rep)` - static function constructing `T` from a raw value

```cpp
template<>
struct mp_units::quantity_point_like_traits<Timestamp> {
  static constexpr auto reference = si::second;
  static constexpr auto point_origin = default_point_origin(reference);
  static constexpr bool explicit_import = false;
  static constexpr bool explicit_export = true;
  using rep = decltype(Timestamp::seconds);

  static constexpr rep to_numerical_value(Timestamp ts) { return ts.seconds; }
  static constexpr Timestamp from_numerical_value(rep v) { return Timestamp{v}; }
};
```

<!-- markdownlint-disable-next-line MD024 -->
### Step 2: Verify the Concept

Check that the [`QuantityPointLike`](../../users_guide/framework_basics/concepts.md#QuantityPointLike)
concept is satisfied:

```cpp
static_assert(mp_units::QuantityPointLike<Timestamp>);
```

<!-- markdownlint-disable-next-line MD024 -->
### Step 3: Use the Conversions

```cpp
void print(Timestamp ts) { std::cout << ts.seconds << " s\n"; }

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  Timestamp ts{42};

  // ✅ Implicit conversion (import)
  quantity_point qp = ts;

  std::cout << qp.quantity_from_zero() << "\n";

  // ✅ Explicit conversion (export)
  print(Timestamp(qp));
}
```

## Working with `std::chrono`

The C++ Standard Library provides two types for handling time in the affine space:

- [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration) - quantities of time
- [`std::chrono::time_point`](https://en.cppreference.com/w/cpp/chrono/time_point) - points in time

**mp-units** comes with built-in interoperability for these types.

### Enabling `std::chrono` Integration

Include the header to enable bidirectional conversions:

```cpp
#include <mp-units/systems/si/chrono.h>
// or
#include <mp-units/systems/si.h>
```

This header provides:

- Partial specializations of `quantity_like_traits` and `quantity_point_like_traits` for
  implicit conversions in both directions
- `chrono_point_origin<Clock>` - point origin for `std::chrono` clocks
- `to_chrono_duration()` and `to_chrono_time_point()` - dedicated conversion functions that
  produce types exactly representing **mp-units** abstractions

### Origin Requirements

Only `quantity_point` types that use `chrono_point_origin<Clock>` as their origin (directly
or indirectly) can be converted to `std::chrono` types:

```cpp
using namespace std::chrono;

inline constexpr struct ts_origin final :
  relative_point_origin<chrono_point_origin<system_clock> + 1 * non_si::hour> {} ts_origin;

inline constexpr struct my_origin final :
  absolute_point_origin<isq::time> {} my_origin;

// ✅ OK: directly uses chrono_point_origin
quantity_point qp1 = sys_seconds{1s};
auto tp1 = to_chrono_time_point(qp1);

// ✅ OK: explicitly constructed from chrono_point_origin
quantity_point qp2 = chrono_point_origin<system_clock> + 1 * s;
auto tp2 = to_chrono_time_point(qp2);

// ✅ OK: derived from chrono_point_origin
quantity_point qp3 = ts_origin + 1 * s;
auto tp3 = to_chrono_time_point(qp3);

// ❌ Compile error: origin not related to chrono_point_origin
quantity_point qp4 = my_origin + 1 * s;
auto tp4 = to_chrono_time_point(qp4);

// ❌ Compile error: zeroth_point_origin not related to chrono_point_origin
quantity_point qp5{1 * s};
auto tp5 = to_chrono_time_point(qp5);
```

### Practical Example: Flight Time Calculator

Here's a complete example showing how **mp-units** and `std::chrono` work together:

```cpp
#include <mp-units/systems/si.h>
#include <chrono>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace std::chrono;

  // Start with a chrono time_point
  sys_seconds ts_now = floor<seconds>(system_clock::now());

  // Convert to mp-units and perform calculations
  quantity_point start_time = ts_now;
  quantity speed = 925. * km / h;
  quantity distance = 8111. * km;
  quantity flight_time = distance / speed;
  quantity_point exp_end_time = start_time + flight_time;

  // Convert back to chrono for time zone handling
  sys_seconds ts_end = value_cast<sys_seconds::rep>(exp_end_time.in(s));

  auto curr_time = zoned_time(current_zone(), ts_now);
  auto mst_time = zoned_time("America/Denver", ts_end);

  std::cout << "Takeoff: " << curr_time << "\n";
  std::cout << "Landing: " << mst_time << "\n";
}
```

Output:

```text
Takeoff: 2026-01-28 19:32:33 UTC
Landing: 2026-01-28 21:18:40 MST
```

## Summary

**mp-units** interoperability features allow you to:

1. **Bridge with legacy code** using simple trait specializations
2. **Control conversion semantics** (implicit vs. explicit) based on safety requirements
3. **Maintain type safety** even when crossing library boundaries
4. **Work seamlessly with `std::chrono`** for time-related calculations

Key points to remember:

- Use `quantity_like_traits` for quantity-like types
- Use `quantity_point_like_traits` for point-like types
- Set `explicit_import` and `explicit_export` flags to control conversion direction
- Safety checks always apply, regardless of conversion semantics
- Built-in `std::chrono` support requires `chrono_point_origin`-based origins

These features make it practical to gradually introduce **mp-units** into existing codebases
or to integrate with external libraries while maintaining strong dimensional safety.

## See Also

**User's Guide:**

- [The Affine Space](../../users_guide/framework_basics/the_affine_space.md) - Understanding quantity points
- [Concepts](../../users_guide/framework_basics/concepts.md) - `QuantityLike` and `QuantityPointLike` concepts
- [Value Conversions](../../users_guide/framework_basics/value_conversions.md) - Understanding conversion rules

**Related How-to Guides:**

- [Working with Legacy Interfaces](working_with_legacy_interfaces.md) - Extracting values for non-type-safe APIs
