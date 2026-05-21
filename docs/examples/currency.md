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

[Try it live on Compiler Explorer](https://godbolt.org/z/57nxMes4h){ .md-button }

## Overview

**mp-units** is a general-purpose quantities and units library — everything that can be
counted or measured benefits from its type-safe dimensional analysis. This example
demonstrates creating a custom base dimension for _currency_, implementing type-safe
_currency_ conversions with exchange rates, and computing a multi-currency portfolio total.

## Key Concepts

### Custom Base Dimension

You can define entirely new base dimensions beyond the standard ISQ/SI system:

```cpp
--8<-- "example/currency.cpp:48:48"
```

This creates a new fundamental dimension orthogonal to physical dimensions like
_length_ or _mass_.

### Custom Units and Quantity Specification

Once you have a base dimension, define a quantity specification and its units:

```cpp
--8<-- "example/currency.cpp:50:65"
```

Each currency becomes its own distinct unit, preventing accidental mixing of incompatible
currencies. This is enforced at compile time:

```cpp
--8<-- "example/currency.cpp:67:67"
```

**This won't compile** — EUR and USD quantities cannot be directly compared. The library
framework cannot convert between them automatically because exchange rates are
time-dependent — it does not know at what _time point_ the conversion should happen.

### User-Provided Runtime Conversion Functions

Since currency exchange rates vary over time, users must supply their own runtime
conversion functions. The example implements a time-stamped rate lookup:

```cpp
--8<-- "example/currency.cpp:69:81"
```

Note that paired rates are not exact inverses of each other: the asymmetry encodes the
bid/ask spread that a real market maker charges. The `exchange_to` functions apply the
appropriate rate and return the converted quantity:

```cpp
--8<-- "example/currency.cpp:83:94"
```

Two overloads are provided: one for plain quantities (_holdings_, amounts) and one for
`quantity_point` (_prices_ anchored to an absolute zero).

### `quantity` vs `quantity_point` for Currency

The example uses both types deliberately:

- **`quantity_point`** for _prices_ — a price of $100 is an absolute reference; $0 is
  meaningful. Two prices in different currencies **cannot be added** — the type system
  rejects both forms:

  ```cpp
  const quantity_point price_usd{100 * USD};
  const quantity_point price_eur = exchange_to<euro>(price_usd, timestamp);
  // price_usd + price_eur                          // does not compile — points can't be added
  // price_usd.quantity_from_zero()
  //   + price_eur.quantity_from_zero()        // does not compile — different units (USD ≠ EUR)
  ```

- **`quantity`** for _holdings_ — a portfolio position is a displacement from zero; amounts
  in the same currency can be freely summed, but cross-currency addition is rejected until
  an explicit FX conversion is applied:

  ```cpp
  const quantity pos_usd = 14'230 * USD;
  const quantity pos_eur =  4'902 * EUR;
  // pos_usd + pos_eur                              // does not compile — must convert first
  const quantity pos_eur_usd = round<USD>(exchange_to<us_dollar>(pos_eur, timestamp)).force_in<int>();
  const quantity total_usd = pos_usd + pos_eur_usd; // ok — same unit
  ```

### Multi-Currency Portfolio Valuation

Portfolio positions are held in their native _currencies_ as plain `quantity` values.
Adding positions across currencies does not compile, making the type system enforce the
discipline of explicit FX conversion:

```cpp
--8<-- "example/currency.cpp:112:124"
```

The rounding decision is made explicitly at the call site: `round<USD>` rounds to the
nearest whole dollar; `floor<USD>` or `ceil<USD>` could be substituted depending on the
desired settlement convention. `.force_in<int>()` changes the representation type to
`int` so the result stays consistent with the integer-valued input positions.

## Example Usage

```cpp
--8<-- "example/currency.cpp:96:132"
```

**Sample Output:**

```text
100 USD -> 92.15 EUR
Portfolio positions:
  14230 USD
  4902 EUR  =>  5318 USD
  1464 GBP  =>  1852 USD
  890000 JPY  =>  6168 USD
Portfolio total (USD): 27568 USD
```

## Why This Matters

- **General-Purpose Library**: Demonstrates **mp-units** working with non-physical
  measurable quantities
- **Type Safety for Finance**: Prevents mixing _currencies_ without explicit conversion,
  caught at compile time
- **Affine Space Design**: `quantity_point` correctly models _prices_ (absolute), while
  `quantity` correctly models _holdings_ (additive)
- **Explicit Rounding**: The bid/ask spread and rounding convention are visible at the
  call site, not hidden inside a conversion helper
- **Domain Modeling**: The dimensional analysis pattern applies to any domain with
  unit-like concepts

This pattern can be adapted for other non-physical domains: cryptocurrencies,
points/rewards systems, game economies, or any system where different "units" should not
be accidentally mixed.

## Practical Considerations

_Currency_ conversions differ from typical physical unit conversions:

- **Multiple independent units of the same quantity** — `USD`, `EUR`, `GBP`, and `JPY`
  are all units of `currency`, so they participate correctly in quantity equations (e.g.,
  `volume = currency × market_quantity` holds for any currency unit). Yet they carry no
  compile-time conversion factor between them. Most units libraries cannot model this:
  either all units of a quantity are mutually convertible via a fixed ratio (impossible
  for FX), or they are defined as separate dimensions — which breaks dimensional equations
  since `EUR × shares` and `USD × shares` would then be different, incompatible quantity
  specs. mp-units threads this needle with `kind_of<currency>`.
- Exchange rates are **time-dependent** — the framework cannot know when you want to convert
- Paired rates are **not exact inverses** — the difference encodes the bid/ask spread
- Rounding convention matters — `round`, `floor`, or `ceil` produce different results and
  the choice should be explicit at the call site
- For fixed-point financial arithmetic with sub-cent precision, see the companion
  [trade_execution](trade_execution.md) example
