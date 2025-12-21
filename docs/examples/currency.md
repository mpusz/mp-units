---
tags:
- Level - Intermediate
- Feature - Affine Space
- Feature - Custom Dimensions
- Feature - Custom Quantities
- Feature - Custom Units
- Feature - Runtime Conversion Functions
- Domain - Finance
---

# Extending Beyond Physics: Currency as Custom Dimension

[Try it live on Compiler Explorer](https://godbolt.org/z/1WE66WeE9){ .md-button }

## Overview

**mp-units** is a general-purpose quantities and units library — everything that can be
counted or measured benefits from its type-safe dimensional analysis. This example
demonstrates creating a custom base dimension for _currency_ and implementing type-safe
_currency_ conversions with exchange rates.

## Key Concepts

### Custom Base Dimension

You can define entirely new base dimensions beyond the standard ISQ/SI system:

```cpp
--8<-- "example/currency.cpp:48:48"
```

This creates a new fundamental dimension orthogonal to physical dimensions like
_length_ or _mass_.

### Custom Units and Quantity Specifications

Once you have a base dimension, define quantity specification and its units:

```cpp
--8<-- "example/currency.cpp:49:65"
```

Each currency becomes its own distinct unit, preventing accidental mixing of incompatible
currencies:

```cpp
--8<-- "example/currency.cpp:67:67"
```

**This won't compile** — you cannot directly compare EUR and USD quantities, as they are
defined as independent units of _currency_ with no compile-time known conversion factor
between them. The library framework cannot perform the conversion automatically because
exchange rates are time-dependent, and it doesn't know at what _time point_ (now,
yesterday, etc.) the user wants to make the conversion.

### User-Provided Runtime Conversion Functions

Since currency exchange rates are time-dependent, the library framework cannot automatically
convert between _currencies_ of independent units. Instead, users must provide their own
runtime conversion functions that account for specific _time points_.

The example implements a time-dependent conversion function:

```cpp
--8<-- "example/currency.cpp:69:79"
```

The `exchange_to` function takes a timestamp parameter and applies the appropriate conversion
factor for that specific point in _time_:

```cpp
--8<-- "example/currency.cpp:81:86"
```

This pattern allows users to handle conversions between units of the same quantity type
when the conversion rate cannot be known at compile time and varies based on external
factors like _time_.

### Type Safety with Quantity Points

The example also demonstrates currency exchange using quantity points, which can be useful
for representing prices with a reference point:

```cpp
--8<-- "example/currency.cpp:88:94"
```

## Example Usage

```cpp
--8<-- "example/currency.cpp:98:106"
```

**Sample Output:**

```text
100 USD -> 92 EUR
```

## Why This Matters

- **General-Purpose Library**: Demonstrates **mp-units** working with non-physical
  measurable quantities
- **Type Safety for Finance**: Prevents mixing _currencies_ without explicit conversion
- **Domain Modeling**: The dimensional analysis pattern applies to any domain with
  unit-like concepts
- **Time-Dependent Conversions**: Shows how to handle conversions where the rate depends
  on external factors (_time_, _location_, _market conditions_) by providing user-defined
  runtime functions

This pattern can be adapted for other non-physical domains: cryptocurrencies,
points/rewards systems, game economies, or any system where different "units" should not
be accidentally mixed.

## Practical Considerations

_Currency_ conversions differ from typical physical unit conversions:

- Exchange rates are **time-dependent** — the framework cannot know when you want to convert
- Users must provide runtime conversion functions that account for specific _time points_
- Real-world factors like bid/ask spreads and transaction costs should be incorporated in
  the user-provided conversion function
- Different markets may have different rates at the same time
- Unlike physical constants (meters to kilometers), there's no fixed mathematical relationship
  between currencies
- This example provides a simplified model for demonstration purposes
