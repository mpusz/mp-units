# Wide Compatibility

This guide shows you how to write **mp-units** code that compiles across a wide range of
compilers and C++ standards. You'll learn about the compatibility macros provided by the
library and how to choose between terse, modern code and portable code that works everywhere.

While mp-units allows you to write clean, modern code for the latest C++ standards, the
library also provides compatibility macros to support older environments. This guide helps
you make the right trade-offs for your project.

!!! info "Configuration Dependency"

    The behavior of compatibility macros depends on configuration options set during library
    installation. See [Conan options](../../getting_started/installation_and_usage.md#conan-options)
    and [CMake options](../../getting_started/installation_and_usage.md#cmake-options)
    in the Installation and Usage guide for details on how to configure the library.

## Choosing Your Compatibility Level

The library supports various levels of C++ standard conformance and formatting library choices.
Depending on your requirements, you can choose to write:

- **Modern, terse code** targeting C++23 with modules
- **Portable code** using preprocessor macros for wide compatibility
- **Formatting library** - choose between `std::format` or
  [{fmt}](https://github.com/fmtlib/fmt) based on your needs (features, compile times,
  or compiler support)
- **Something in between** based on your minimum supported environment

!!! note "Examples Use Compatibility Macros"

    The library's [example applications](../../examples/index.md) use compatibility macros
    because they're built on all [supported compilers](../../getting_started/cpp_compiler_support.md),
    some of which don't yet support `std::format`, C++ modules, or C++ versions newer than C++20.

## Code Style Options

Here are the different ways you can write the same code, ordered from most modern to most
compatible:

=== "C++23 with Modules"

    ```cpp
    import mp_units;
    import std;

    // ...

    // C++23 explicit object parameter syntax
    inline constexpr struct horizontal_length final :
        quantity_spec<isq::length> {} horizontal_length;

    // ...

    quantity q = horizontal_length(42 * m);
    std::println("length: {}", q);
    ```

=== "C++20 with Modules"

    ```cpp
    #include <format>
    #include <iostream>
    import mp_units;

    // ...

    // C++20 CRTP syntax (explicit type parameter)
    inline constexpr struct horizontal_length final :
        quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    quantity q = horizontal_length(42 * m);
    std::cout << std::format("length: {}\n", q);
    ```

=== "C++20 with Headers"

    ```cpp
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <format>
    #include <iostream>

    // ...

    // C++20 CRTP syntax
    inline constexpr struct horizontal_length final :
        quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    quantity q = horizontal_length(42 * m);
    std::cout << std::format("length: {}\n", q);
    ```

=== "C++20 with fmtlib"

    ```cpp
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <fmt/format.h>
    #include <iostream>

    // ...

    // C++20 CRTP syntax
    inline constexpr struct horizontal_length final :
        quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    quantity q = horizontal_length(42 * m);
    std::cout << fmt::format("length: {}\n");
    ```

=== "Wide Compatibility"

    ```cpp
    #include <iostream>
    #include <mp-units/ext/format.h>
    #ifdef MP_UNITS_MODULES
    #include <mp-units/compat_macros.h>
    import mp_units;
    #else
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #endif

    // ...

    // Works with both C++23 and C++20
    QUANTITY_SPEC(horizontal_length, isq::length);

    // ...

    // Works with both std::format and {fmt}
    quantity q = horizontal_length(42 * m);
    std::cout << MP_UNITS_STD_FMT::format("length: {}\n");
    ```

!!! note "Formatting Library Choice"

    The examples above show different C++ standard versions and include/import styles.
    The formatting library choice (`std::format` vs `fmt::format`) is independent of
    the C++ version - you can use [{fmt}](https://github.com/fmtlib/fmt) with C++20,
    C++23, or any other standard. Many projects prefer [{fmt}](https://github.com/fmtlib/fmt)
    even with C++23 for its additional features, faster compile times, or consistency
    with existing code.

!!! tip "Choose Based on Your Needs"

    - **Targeting a specific compiler?** Use the terse, modern style directly
    - **Need maximum portability?** Use the compatibility macros throughout
    - **Formatting library?** Choose `std::format` or [{fmt}](https://github.com/fmtlib/fmt)
      based on features, compile times, and project needs
    - **Transitioning?** Start with compatibility macros and refactor later

## Using Compatibility Macros

### Including the Compatibility Header

When using C++ modules, compatibility macros must be explicitly included:

```cpp
#ifdef MP_UNITS_MODULES
#include <mp-units/compat_macros.h>
#endif
```

With legacy headers, the compat header is automatically included.

### `QUANTITY_SPEC(name, ...)` {#QUANTITY_SPEC}

**Problem:** Quantity specification definitions use C++23's explicit object parameters to
avoid CRTP (Curiously Recurring Template Parameter), which significantly simplifies the syntax.

**Solution:** This macro automatically uses the appropriate syntax for your C++ version:

```cpp
// With macro - works on C++20 and C++23
QUANTITY_SPEC(horizontal_length, isq::length);

// Expands to:
//   C++23: struct horizontal_length final : quantity_spec<isq::length> {} horizontal_length;
//   C++20: struct horizontal_length final : quantity_spec<horizontal_length, isq::length> {} horizontal_length;
```

!!! note "Configuration"

    The macro's behavior depends on the [`no_crtp`](../../getting_started/installation_and_usage.md#no_crtp)
    Conan option or [`MP_UNITS_API_NO_CRTP`](../../getting_started/installation_and_usage.md#MP_UNITS_API_NO_CRTP)
    CMake option.

### `MP_UNITS_STD_FMT`

**Problem:** Projects need to choose between [`std::format`](https://en.cppreference.com/w/cpp/utility/format/format)
and [{fmt}](https://github.com/fmtlib/fmt) based on compiler support, desired features,
compile times, or project standards. Many projects prefer [{fmt}](https://github.com/fmtlib/fmt)
even when `std::format` is available.

**Solution:** This macro resolves to either the `std` or `fmt` namespace based on your
configuration:

```cpp
#include <mp-units/ext/format.h>  // Includes the right formatting header

// Use the macro instead of hardcoding std or fmt
std::cout << MP_UNITS_STD_FMT::format("{}", distance) << "\n";
```

!!! note "Configuration"

    The macro's behavior depends on the [`std_format`](../../getting_started/installation_and_usage.md#std_format)
    Conan option or [`MP_UNITS_API_STD_FORMAT`](../../getting_started/installation_and_usage.md#MP_UNITS_API_STD_FORMAT)
    CMake option.

### Contract Checking Macros

**Problem:** The library uses contract checking (preconditions, postconditions, assertions)
which can be provided by either [gsl-lite](https://github.com/gsl-lite/gsl-lite) or
[ms-gsl](https://github.com/microsoft/GSL).

**Solution:** Use these macros for portable contract checks:

```cpp
#include <mp-units/ext/contracts.h>

void process_speed(quantity<si::metre / si::second, int> speed)
{
  MP_UNITS_EXPECTS(speed > 0 * m / s);  // Precondition (always checked if enabled)
  // ...
  MP_UNITS_ASSERT(result > 0);          // Assertion (always checked if enabled)
}

void debug_function()
{
  MP_UNITS_EXPECTS_DEBUG(condition);    // Only checked in debug builds
  // ...
  MP_UNITS_ASSERT_DEBUG(invariant);     // Only checked in debug builds
}
```

Available macros:

- `MP_UNITS_EXPECTS(expr)` - Precondition check
- `MP_UNITS_EXPECTS_DEBUG(expr)` - Debug-only precondition check
- `MP_UNITS_ASSERT(expr)` - Assertion check
- `MP_UNITS_ASSERT_DEBUG(expr)` - Debug-only assertion check

Their behavior is consistent with [gsl-lite contract checking](https://github.com/gsl-lite/gsl-lite?tab=readme-ov-file#contract-checking-configuration-macros).

!!! note "Configuration"

    The contract checking implementation depends on the [`contracts`](../../getting_started/installation_and_usage.md#conan-options)
    Conan option or [`MP_UNITS_API_CONTRACTS`](../../getting_started/installation_and_usage.md#MP_UNITS_API_CONTRACTS)
    CMake option.

## Complete Example

Here's a complete example using all compatibility features:

```cpp
#include <iostream>
#include <mp-units/ext/format.h>
#include <mp-units/ext/contracts.h>

#ifdef MP_UNITS_MODULES
#include <mp-units/compat_macros.h>
import mp_units;
#else
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Define custom quantity specification with wide compatibility
QUANTITY_SPEC(flight_distance, isq::length);

quantity<flight_distance[km]> calculate_distance(quantity<isq::speed[km / h]> speed,
                                                 quantity<isq::time[h]> duration)
{
  MP_UNITS_EXPECTS(speed > 0 * km / h);
  MP_UNITS_EXPECTS(duration > 0 * h);

  return flight_distance(speed * duration);
}

int main()
{
  quantity speed = 850 * km / h;
  quantity duration = 2.5 * h;
  quantity distance = calculate_distance(speed, duration);

  // Format output with portable macro
  std::cout << MP_UNITS_STD_FMT::format("Flying at {} for {} covers {}\n",
                                        speed, duration, distance);
}
```

## Summary

**mp-units** provides flexibility in how you write code:

- **Modern syntax** for latest compilers: clean and terse
- **Compatibility macros** for wide support: verbose but portable
- **Incremental adoption**: start portable, refactor to modern later

Key compatibility tools:

- `QUANTITY_SPEC()` - Works across C++20 and C++23
- `MP_UNITS_STD_FMT` - Works with `std::format` or [{fmt}](https://github.com/fmtlib/fmt)
- Contract macros - Works with gsl-lite or ms-gsl
- Module guards - Support both modules and headers

Choose the approach that best fits your project's requirements and minimum supported
environment.

## See Also

**Getting Started:**

- [Installation and Usage](../../getting_started/installation_and_usage.md) - Configuration
  options:
    - [Conan options](../../getting_started/installation_and_usage.md#conan-options) - Control
      compatibility features via Conan
    - [CMake options](../../getting_started/installation_and_usage.md#cmake-options) - Control
      compatibility features via CMake
- [C++ Compiler Support](../../getting_started/cpp_compiler_support.md) - Supported compilers
  and features

**Examples:**

- [Examples](../../examples/index.md) - All examples use compatibility macros
