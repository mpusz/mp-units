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
    - `is_value_preserving<From, To>`: Determines if a conversion preserves values

    By default, **mp-units** uses `std::chrono::duration`-like logic for these.

## Forcing Truncating Conversions

When you need to truncate, make it explicit:

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Three ways to force truncating conversions:
  quantity km_int = (500 * m).force_in(km);           // ✅ 0 km (truncates to int)
  quantity km_dbl = (500 * m).in<double>(km);         // ✅ 0.5 km (changes to double)
  quantity km_cast = value_cast<double, km>(500 * m); // ✅ 0.5 km (using value_cast)

  std::cout << "Forced conversions:\n";
  std::cout << "  force_in(km): " << km_int << "\n";
  std::cout << "  in<double>(km): " << km_dbl << "\n";
  std::cout << "  value_cast: " << km_cast << "\n";
}
```

**Key insight**: Explicit syntax prevents accidental data loss!

## Challenges

1. **Force truncation**: Create 1234 m as `int`, force convert to km (should get 1 km)
2. **Safe with doubles**: Create 1234 m as `double`, convert to km (should get 1.234 km implicitly)
3. **Mixed representations**: Add 5.5 km (double) + 2000 m (int), what's the result type?

## What You Learned?

✅ Implicit conversions only work when safe (no truncation)  
✅ Floating-point types are considered value-preserving  
✅ Use `.force_in()`, `.in<Rep>()`, or `value_cast` to force truncation  
✅ The library follows `std::chrono::duration` logic for safety  
✅ Explicit syntax prevents accidental data loss
