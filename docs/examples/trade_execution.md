---
tags:
- Level - Intermediate
- Feature - Affine Space
- Feature - Custom Dimensions
- Feature - Custom Quantities
- Feature - Custom Units
- Feature - Fixed-Point Arithmetic
- Domain - Finance
---

# Trade Execution: Derived Financial Quantities and Fixed-Point Arithmetic

[Try it live on Compiler Explorer](https://godbolt.org/z/9jaKocxE4){ .md-button }

## Overview

This example models the execution of a buy order filled in multiple tranches and
computes the average fill price (cost basis) at several settlement precisions. It
demonstrates two ideas that do not appear in the [currency](currency.md) example:

1. **Derived financial quantities** — `notional_value` (_price_ × _market quantity_) is a
   separate quantity specification, distinct from `currency`, enforced by the type system.
2. **Integer fixed-point arithmetic** — _prices_ are scaled to `USD_8` (units of $10⁻⁸)
   and stored as `std::int64_t` to avoid floating-point rounding during accumulation.

## Key Concepts

### Derived Quantity Specifications: `market_quantity` and `notional_value`

In finance, _notional value_ = _price_ × _market quantity_. This is not the same quantity
as _currency_: you cannot directly add a _price_ to a _notional value_, and dividing
_notional value_ by _market quantity_ should recover _currency_, not _notional value_.
The example encodes this with derived quantity specifications:

```cpp
--8<-- "example/trade_execution.cpp:44:52"
```

Both `market_quantity` and `notional_value` carry `is_kind`, making each the root of its
own kind family:

- **`market_quantity`** with `is_kind` — _market quantity_ is its own kind, distinct from
  generic `dimensionless` quantities (ratios, percentages, etc.). Without it, a _market
  quantity_ would silently convert to a plain number.
- **`notional_value`** with `is_kind` — _notional value_ is its own kind, distinct from
  _currency_. Without it, a _notional value_ would silently convert to a plain _currency_
  amount, defeating the type safety the derived quantity is meant to provide.

`is_kind` on `market_quantity` disables implicit construction from plain integers, so a
`Qty` helper alias is provided: `Qty(137)` calls the quantity spec directly as a factory.

The relationship `notional_value = currency × market_quantity` is the _defining
equation_; explicit casts still work in both directions via that equation. The
`static_assert`s verify the intended semantics at compile time:

```cpp
--8<-- "example/trade_execution.cpp:54:57"
```

### Integer Fixed-Point Units

Floating-point arithmetic accumulates rounding error across many fills. Instead, prices
are stored as integers in a fine-grained unit:

```cpp
--8<-- "example/trade_execution.cpp:59:65"
```

`USD_8` represents $10⁻⁸ (one hundred-millionth of a dollar). A _price_ of $12.95
becomes `1,295,000,000 USD_8` — an exact integer. Multiplying by a _market quantity_
count gives an exact integer _notional value_. No floating-point arithmetic is involved
until the final display.

### Type Aliases and the `Fill` Struct

```cpp
--8<-- "example/trade_execution.cpp:77:85"
```

- `Price` — a `quantity_point` (_price_ has an absolute zero; $0 is meaningful)
- `Shares` — a dimensionless _market quantity_ count
- `Notional` — the derived _notional value_ in `USD_8`
- `Fill` — groups a _price_ and _market quantity_ count together; the natural unit of
  trade data

## Code Walkthrough

### Computing Notional: `force_in` for Lossless Scaling

```cpp
--8<-- "example/trade_execution.cpp:87:91"
```

`force_in<std::int64_t>(us_dollar_8)` converts the _price_ unit from `USD` to `USD_8`
and changes the representation from `double` to `std::int64_t` in one step. The product
`price_in_usd8 * f.qty` yields a derived expression (_currency_ × _market quantity_)
that is implicitly convertible to `notional_value`; the `return` converts it to
`Notional`.

### Accumulating Fills

```cpp
--8<-- "example/trade_execution.cpp:104:120"
```

Each fill is constructed with an explicit `Price{...}` (the constructor is explicit, so
direct initialization is required) and `Qty(137)` (the `market_quantity` spec called
directly as a factory, required because `is_kind` disables implicit construction from
plain integers). The accumulator variables `total_notional` and `total_qty` are
zero-initialized via copy initialization from `{}`. The `notional_value` quantity
specification ensures that adding two _price_ quantities does not accidentally produce a
_notional value_ — the `// does not compile` comment shows what the type system prevents.

### Recovering the Average Fill Price

```cpp
--8<-- "example/trade_execution.cpp:122:129"
```

`currency(total_notional / total_qty)` performs an explicit quantity cast to the
`currency` specification — without it, the type system sees
`notional_value / market_quantity` as an unreduced derived expression and `round<USD_N>`
cannot match it. The cast is required.

Rounding is then applied at four different settlement precisions so the output shows
how much information is retained at each level.

## Example Usage

```cpp
--8<-- "example/trade_execution.cpp:100:140"
```

**Sample Output:**

```text
Fills:
  137 @ 12.95 USD  (notional: 177415000000 USD_8)
  126 @ 12.4 USD  (notional: 156240000000 USD_8)
  85 @ 12.7 USD  (notional: 107950000000 USD_8)
Total:              348 shares,  notional: 441605000000 USD_8
Cost basis (exact): 12.68979885 USD
Cost basis (USD_2): 12.69 USD
Cost basis (USD_4): 12.6898 USD
Cost basis (USD_6): 12.689799 USD
Cost basis (USD_8): 12.68979885 USD
```

The notional values are large integers in `USD_8`; the cost basis is recovered as a
human-readable `USD` value at varying precisions.

## Why This Matters

- **Derived Quantities**: The `notional_value` specification makes the
  _price_ × _market quantity_ algebra compile-time-safe — you cannot confuse a
  _notional value_ with a _price_ or a plain _currency_ amount
- **Kind Safety**: `is_kind` on both `market_quantity` and `notional_value` ensures that
  _market quantity_ and _notional value_ do not silently decay to their parent kinds
  (`dimensionless` and _currency_ respectively)
- **Lossless Accumulation**: Integer arithmetic in `USD_8` avoids floating-point drift
  across many fills; only the final display converts to floating-point
- **Explicit Rounding**: Rounding to a specific settlement precision (`USD_2`, `USD_4`,
  `USD_6`, `USD_8`) is an explicit, visible operation — not hidden inside a helper
- **Domain Modeling**: The `Fill` struct groups related quantities, and `operator<<`
  provides domain-appropriate output (`shares @ price`)

## Related Example

The [currency](currency.md) example focuses on multi-currency type safety and FX
conversion. This example focuses on single-currency arithmetic precision. Together they
cover the two main concerns in financial quantity modelling.
