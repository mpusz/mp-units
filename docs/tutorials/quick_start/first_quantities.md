# Your First Quantities

Welcome to **mp-units**! In this lesson, you'll create your first quantities and see
them in action.

**Goal**: Create quantities with units and display them  
**Time**: ~10 minutes

## What is a Quantity?

A **quantity** combines a numerical value with its unit. For example:

- 120 kilometers (distance)
- 2 hours (time)
- 75 kilograms (mass)

**mp-units** ensures the value and unit always stay together, preventing unit errors.

## Try It: Create Your First Quantity

The code below creates quantities. It compiles and runs automatically as you modify it!

```cpp
// ce-embed height=500 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Create a distance
  quantity distance = 120 * km;
  std::cout << "Distance: " << distance << "\n";

  // Create a time duration
  quantity time = 2 * h;
  std::cout << "Time: " << time << "\n";

  // Create a mass
  quantity mass = 75 * kg;
  std::cout << "Mass: " << mass << "\n";
}
```

**What happened:**

- `quantity distance = 120 * km;` creates a quantity with value 120 and unit kilometers
- The library remembers both the number AND its unit
- Output shows the complete quantity: `Distance: 120 km`

## Understanding the Code

Let's break it down:

```cpp
#include <mp-units/systems/si.h>
```

This includes the [International System of Units (SI)](../../users_guide/systems/si.md).

```cpp
using namespace mp_units::si::unit_symbols;
```

This lets us write `km`, `h`, `kg` directly (kilometer, hour, kilogram symbols).

```cpp
quantity distance = 120 * km;
```

- `quantity` is a class template (the compiler deduces the specific type automatically)
- `120` is the numerical value
- `km` is the unit symbol
- The `*` operator combines them

**Key insight**: The `quantity` type combines a numerical value with its unit, creating
type-safe measurements that prevent unit errors at compile time.

## Unit Names vs Symbols

So far we've used unit **symbols** like `km`, `h`, `kg`. But you can also use full unit
**names** with prefixes:

```cpp
// ce-embed height=550 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Using symbols (imported from unit_symbols namespace)
  quantity distance1 = 5.0 * km;

  // Using full unit names with prefixes
  quantity<si::kilo<si::metre>, double> distance2 = 5.0 * si::kilo<si::metre>;

  std::cout << "With symbols: " << distance1 << "\n";
  std::cout << "With names: " << distance2 << "\n";
  std::cout << "Equal? " << (distance1 == distance2 ? "Yes" : "No") << "\n";
}
```

**When to use each:**

- **Symbols** (`km`, `m`, `h`): Implementation code - quick and readable
- **Names** (`si::kilo<si::metre>`): Header files for function declarations, where you
  don't want to import `unit_symbols`

!!! tip "For most code, use symbols"

    **Symbols** are more concise and readable but come at the cost of polluting the current
    namespace with many short identifiers brought in by the `using namespace` directives.

    Use **full names** primarily in header files when declaring function interfaces. As a
    general C++ best practice, `using namespace` directives should not appear in the
    namespace scope of header files to avoid forcing namespace pollution on users of your
    headers.

**Key insight**: Both symbols and full names create identical quantity types - they're just
different syntaxes.

## Challenges

Try modifying the code above:

1. **Change values**: Try `distance = 250 * km`
2. **Try different units**: Replace `km` with `m` (meters)
3. **Add more quantities**: Create `quantity power = 1500 * W;` (watts)
4. **Try derived units**: Create `quantity speed = 100.5 * m / s;` (meters per second)

## What You Learned?

✅ How to include **mp-units** headers  
✅ Quantities combine values with units  
✅ Use `value * unit` syntax to create quantities  
✅ Unit symbols (`km`) for implementation, full names (`si::kilo<si::metre>`) for headers  
✅ Output quantities with IO streams (`std::cout`)
