# Generic Interfaces

Learn how to write flexible, high-performance generic functions using `QuantityOf`.

**Goal**: Master generic quantity interfaces for better performance  
**Time**: ~15 minutes

## Why Generic Interfaces?

When writing functions that work with quantities, you have several options. Consider a simple
average function:

```cpp
// Option 1: Concrete type - forces unit and may convert
quantity<isq::length[si::metre]> avg_concrete(quantity<isq::length[si::metre]> l1,
                                               quantity<isq::length[si::metre]> l2);

// Option 2: QuantityOf - accepts anything that converts to length!
QuantityOf<isq::length> auto avg_generic(QuantityOf<isq::length> auto l1,
                                         QuantityOf<isq::length> auto l2);
```

The generic version accepts any quantity that implicitly converts to `isq::length` (like
`isq::width`, `isq::height`, `isq::radius`) in any unit and any representation type,
without forcing conversions at the boundary!

## Writing Generic Functions

Use `QuantityOf` concept to accept any quantity that implicitly converts to a specified
quantity type:

```cpp
// ce-embed height=850 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

using namespace mp_units;

// Accept any quantity implicitly convertible to length
// (width, height, radius, distance, etc.) in ANY unit (m, km, ft, mi, etc.)
QuantityOf<isq::length> auto avg_length(QuantityOf<isq::length> auto l1,
                                        QuantityOf<isq::length> auto l2)
{
  // Returning a length is correct: (length + length) / 2 → length
  return (l1 + l2) / 2;

  // ❌ Wrong: (length * length) / 2 returns area, not length
  // return (l1 * l2) / 2;
}

// Accept ANY speed quantity in ANY unit
QuantityOf<isq::time> auto time_to_destination(QuantityOf<isq::length> auto distance,
                                               QuantityOf<isq::speed> auto speed)
{
  // Returning time is correct: distance / speed → time
  return distance / speed;

  // ❌ Wrong: distance * speed → area/time, not time
  // return distance * speed;
}

int main()
{
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::yard_pound::unit_symbols;

  // Different length types and units
  quantity width = isq::width(10 * m);
  quantity height = isq::height(20 * ft);  // Note: feet!
  quantity radius = isq::radius(0.005 * km);

  // All work - these quantities implicitly convert to isq::length
  QuantityOf<isq::length> auto avg1 = avg_length(width, height);
  QuantityOf<isq::length> auto avg2 = avg_length(width, radius);
  QuantityOf<isq::length> auto avg3 = avg_length(height, radius);

  std::cout << "Average (width, height): " << avg1 << " = " << avg1.in<double>(m) << "\n";
  std::cout << "Average (width, radius): " << avg2 << " = " << avg2.in<double>(m) << "\n";
  std::cout << "Average (height, radius): " << avg3 << " = " << avg3.in<double>(m) << "\n";

  // Works with different speed units too
  quantity distance = 100 * km;
  quantity speed1 = 50 * km / h;
  quantity speed2 = 30. * mi / h;

  quantity eta1 = time_to_destination(distance, speed1);
  quantity eta2 = time_to_destination(distance, speed2);

  std::cout << "\nTime at " << speed1 << ": " << eta1 << "\n";
  std::cout << "Time at " << speed2 << ": " << eta2 << " = " << eta2.in(h) << "\n";

  // These DON'T compile - wrong dimensions
  // quantity mass = 50 * kg;
  // auto bad1 = avg_length(width, mass);                       // ❌ mass is not a length!
  // auto bad2 = time_to_destination(mass, speed1);             // ❌ mass is not a distance!
  // quantity<isq::mass[kg]> bad3 = avg_length(width, height);  // ❌ length is not mass!
}
```

**Key insight**: `QuantityOf` enables zero-overhead generic programming - the function works
directly with the caller's exact types (units and representation), avoiding conversions at
boundaries. This is both more efficient and more flexible than concrete types, while still
providing compile-time dimensional safety.

!!! note "Mixed Units and Late Conversion"

    Notice the output when mixing different units:

    **Addition/subtraction** (averaging lengths):

    - Unit multiples: `Average (width, radius): 7.5 m` - clean result (m and km are multiples)
    - Non-multiples: `Average (width, height): 10060 [(1/1250 m), (1/381 ft)]` - GCD unit!

    When adding or subtracting quantities, the library finds the **greatest common divisor (GCD)**
    of both units. If one unit is a multiple of the other, that base unit is favored (like `m`
    for m + km). Otherwise, a new common unit (the GCD) is created, even if it's very small -
    this produces the complex `[(1/1250 m), (1/381 ft)]` unit when mixing m and ft.

    **Why GCD?** Avoids premature division (no truncation risk), though values may be
    multiplied (potential overflow risk).

    **Multiplication/division** (calculating time):

    - Same units cancel: `Time at 50 km/h: 2 h` - clean result (km cancels)
    - Different units: `Time at 30 mi/h: 3.33333 h km/mi` - composite units!

    For multiplication and division, only identical/repeated units simplify and cancel.
    When units don't match, composite units naturally result without simplification.

    **Why this matters:**

    - **Performance**: Defers conversions until you explicitly request a specific unit with
      `.in(unit)`, replacing many intermediate conversions with one final scaling
    - **Optimization**: If used in further calculations, units might cancel out before
      conversion is needed (e.g., multiplying by `mi` would eliminate `mi` from the denominator)
    - **User control**: You decide the final unit system rather than the library choosing for you

    This "late conversion" strategy is especially beneficial in complex calculations where
    intermediate results are used in subsequent operations.

## When to Use `QuantityOf`

**Why this approach?**

- **Function arguments**: `QuantityOf<QS>` accepts any quantity that implicitly converts
  to `QS`, with any representation type and unit, without forcing conversions at the boundary.
- **Function return types**: Constrains the return to quantities convertible to the specified
  type, preventing accidental quantity type mismatches.
- **Stack variables**: Accepts any quantity implicitly convertible to the specified type,
  remaining flexible across units/representation types/quantity specifications;
  `QuantityOf` is clearer and stricter than CTAD.

*[CTAD]: Class Template Argument Deduction

## Comparison of Approaches

<!-- markdownlint-disable MD033 MD060 -->
| Approach                                      | Any unit<br/>(no conversion) | Any representation<br/>(no conversion) | Dimension-Safe | Quantity-Safe | Remarks                          |
|-----------------------------------------------|------------------------------|----------------------------------------|----------------|---------------|----------------------------------|
| Raw `auto`<br/>`auto var = ...`               | ✅ No conversion              | ✅ No conversion                        | ❌ No           | ❌ No          | ❌ No checking at all             |
| CTAD<br/>`quantity var = ...`                 | ✅ No conversion              | ✅ No conversion                        | ❌ No           | ❌ No          | ⚠️ No dimension check            |
| Unit-only<br/>`quantity<m>`                   | ❌ Converts to meters         | ❌ Converts representation              | ✅ Yes          | ❌ No          | ⚠️ Not Quantity-Safe             |
| Typed quantity<br/>`quantity<isq::radius[m]>` | ❌ Converts to meters         | ❌ Converts representation              | ✅ Yes          | ✅ Yes         | ✅ Strictest                      |
| Concept<br/>`QuantityOf<isq::radius>`         | ✅ No conversion              | ✅ No conversion                        | ✅ Yes          | ✅ Yes         | ✅ Fast, safe, no truncation/loss |
<!-- markdownlint-enable MD033 MD060 -->

**Best practices**:

- **Typed quantities** (`quantity<isq::radius[m]>`) for non-template functions or storage
  in data types
- **Unit-only** (`quantity<m>`) when you don't care about Quantity-Safety (any quantity
  type from the hierarchy is fine)
- **`QuantityOf`** for generic template algorithms accepting any quantity implicitly
  convertible to a provided type
- **CTAD** (`quantity var = ...`) for stack variables where the quantity type is obvious
  from the initializer

## Challenges

Try these examples:

1. **Average speed function**: Write a function `avg_speed(length, time)` with return type
  constrained to `isq::speed`. Call it with:
    - `isq::distance(100 * km)` and `2 * h` - observe result in km/h
    - `isq::height(3000 * ft)` and `10 * min` - observe result in ft/min
    - `50 * mi` and `1 * h` - observe result in mi/h
    - Try `50 * kg` and `1 * h` - see the error!

    Notice how the return type automatically matches your input units - no conversion needed!

2. **Compare with traditional approach**: Write the same function with `quantity<isq::length[m]>`
   and `quantity<isq::time[s]>` parameters and convert the result to desired units.
   How many conversions happen at the boundaries?

## What You Learned?

✅ `QuantityOf<QS>` accepts any quantity that implicitly converts to `QS`  
✅ No conversions happen at function boundaries with `QuantityOf`  
✅ Return type constraints ensure dimensional correctness  
✅ Generic interfaces improve performance and flexibility  
✅ Use `QuantityOf` for generic algorithms, typed quantities for semantic meaning  
