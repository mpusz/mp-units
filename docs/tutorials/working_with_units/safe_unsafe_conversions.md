# Safe and Unsafe Conversions

Learn about truncation, forcing conversions, and system-specific units.

**Goal**: Handle unsafe conversions and understand different unit systems  
**Time**: ~15 minutes

## Safe vs Unsafe Conversions

**mp-units** allows implicit conversions when safe (no truncation), but blocks dangerous ones:

```cpp
// ce-embed height=500 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // These work (no truncation):
  quantity<si::metre, double> meters = 5.0 * km;  // ✅ 5000.0 m - safe
  quantity<si::metre, int> meters2 = 5 * km;      // ✅ 5000 m - safe

  std::cout << "Safe conversions:\n";
  std::cout << "  " << meters << "\n";
  std::cout << "  " << meters2 << "\n";

  // This won't compile (truncation risk):
  // quantity<si::kilo<si::metre>, int> km_bad = 500 * m;  // ❌ 0.5 km → 0

  // Try it! Uncomment the line above to see the compiler error message.
}
```

**Key insight**: The library prevents implicit conversions that could lose data through
truncation. Converting to a larger unit with integer representation (500 m → 0 km)
requires explicit syntax to acknowledge the data loss.

**Why?** Converting 500 m to km with `int` representation would truncate 0.5 → 0, losing data!

## Floating-Point Types Are Value-Preserving

**mp-units** follows `std::chrono::duration` logic: floating-point types are considered
value-preserving:

!!! note "Default representation type"

    When you explicitly specify a unit without a representation type (e.g., `quantity<si::metre>`),
    it defaults to `double`. But when using CTAD (e.g., `quantity distance = 5.0 * km`), the
    representation type is deduced from the expression.

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Integer → floating-point: safe (value-preserving)
  quantity<si::metre, int> int_meters = 1500 * m;
  quantity<si::kilo<si::metre>, double> km_value = int_meters;  // ✅ 1.5 km

  std::cout << "Int to double: " << int_meters << " → " << km_value << "\n";

  // Floating-point → integer: NOT safe (truncation risk)
  quantity<si::kilo<si::metre>, double> km_double = 2.7 * km;
  // quantity<si::metre, int> meters_int = km_double;        // ❌ Won't compile!
  // quantity<si::kilo<si::metre>, int> km_int = km_double;  // ❌ Also won't compile!

  std::cout << "\nDouble to int: blocked by compiler (even without unit change!)\n";
}
```

**Key insight**: Following `std::chrono::duration` logic, floating-point types are considered
value-preserving (int → double is safe), but any conversion to an integer type risks
truncation and is blocked - even without changing units!

!!! info "Customization points"

    You can customize this behavior with:

    - `treat_as_floating_point<Rep>`: Tells the library if a type should be treated as floating-point
    - `implicitly_scalable<FromUnit, FromRep, ToUnit, ToRep>`: Controls whether a specific conversion is implicit or explicit

    By default, **mp-units** uses `std::chrono::duration`-like logic for these.

## Truncating Conversions

When a conversion cannot keep the value, you have to say what should happen to the part
that does not fit:

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Option 1: keep the integer and pick a rounding policy
  quantity km_trunc = (1567 * m).in(km, truncated);      // ✅ 1 km (towards zero)
  quantity km_round = (1567 * m).in(km, rounded);        // ✅ 2 km (nearest, ties to even)
  quantity km_down = (1567 * m).in(km, rounded_down);    // ✅ 1 km (towards -infinity)
  quantity km_up = (1567 * m).in(km, rounded_up);        // ✅ 2 km (towards +infinity)

  // Option 2: switch to a floating-point representation and keep every digit
  quantity km_dbl = (1567 * m).in<double>(km);           // ✅ 1.567 km
  quantity km_cast = value_cast<double, km>(1567 * m);   // ✅ 1.567 km (using value_cast)

  std::cout << "Converting " << 1567 * m << " to km:\n";
  std::cout << "  truncated:    " << km_trunc << "\n";
  std::cout << "  rounded:      " << km_round << "\n";
  std::cout << "  rounded_down: " << km_down << "\n";
  std::cout << "  rounded_up:   " << km_up << "\n";
  std::cout << "  in<double>:   " << km_dbl << "\n";
  std::cout << "  value_cast:   " << km_cast << "\n";

  // Watch out for negatives: truncated rounds towards zero, rounded_down does not
  std::cout << "\n-1567 m truncated:    " << (-1567 * m).in(km, truncated) << "\n";
  std::cout << "-1567 m rounded_down: " << (-1567 * m).in(km, rounded_down) << "\n";
}
```

**Key insight**: The policy is not paperwork. It is the answer to "which of the two
neighboring values do I want", and the compiler makes you give it.

## Challenges

1. **Round three ways**: Create 1234 m as `int` and convert it to km with `truncated`,
   `rounded`, and `rounded_up` (you should get 1 km, 1 km, and 2 km)
2. **Safe with doubles**: Create 1234 m as `double`, convert to km (should get 1.234 km implicitly)
3. **Mixed representations**: Add 5.5 km (double) + 2000 m (int), what's the result type?

## What You Learned

✅ Implicit conversions only work when safe (no truncation)  
✅ Floating-point types are considered value-preserving  
✅ Use `.in(Unit, policy)`, `.in<Rep>()`, or `value_cast` for truncating conversions  
✅ `truncated`, `rounded`, `rounded_down`, and `rounded_up` name the rounding direction  
✅ The library follows `std::chrono::duration` logic for safety  
✅ Explicit syntax prevents accidental data loss
