# Custom Contract Handlers

Learn how to ensure guaranteed bounds enforcement for production systems using custom
representation types with error policies.

**Goal**: Replace default contract termination with guaranteed exception handling or logging
          that works in all build modes  
**Time**: ~20 minutes

**Prerequisites**:  
- [Bounded Quantity Points](bounded_quantity_points.md)  
- [Temperature Applications](temperature_applications.md)  
- Basic understanding of C++ exceptions


## The Problem: Contract Checking Can Be Disabled

When using `check_in_range` or `check_non_negative` policies with plain representation types
(like `double`), the library detects invalid values:

```cpp
// Define sensor-specific origin (relative to ice_point with 0 offset)
// Typical sensor operating range: -40°C to +85°C
inline constexpr struct sensor_scale_origin :
    relative_point_origin<si::ice_point + delta<deg_C>(0),
                          check_in_range{delta<deg_C>(-40), delta<deg_C>(85)}> {} sensor_scale_origin;

quantity_point temp = sensor_scale_origin + 150 * deg_C;  // Out of range! (150°C > 85°C)
```

By default, this **terminates the program** via contract violation. While appropriate for
debugging and safety-critical systems with fail-fast requirements, production systems often
need:

- **Graceful degradation**: Log the error and use a fallback value
- **Exception-based error handling**: Integrate with existing error propagation
- **Monitoring and alerting**: Track violation frequency for diagnostics
- **Recovery strategies**: Retry sensor reads, switch to backup sensors

!!! warning "Contract Checking Can Be Disabled"

    **Critical**: Contract checking via `MP_UNITS_EXPECTS` may be **disabled at compile-time** by
    setting the [`contracts` Conan option](../../getting_started/installation_and_usage.md#contracts) to `none`
    or the [`MP_UNITS_API_CONTRACTS` CMake option](../../getting_started/installation_and_usage.md#MP_UNITS_API_CONTRACTS)
    to `NONE`. When disabled, **all safety checks vanish** - invalid values pass through unchecked.

    The practices in this tutorial help you **enforce safety guarantees** regardless of
    build configuration by implementing explicit runtime validation that cannot be
    optimized away.


## Understanding How Bounds Checking Works

When you use `check_in_range` as a bounds policy on an origin, the library follows this logic:

1. **Check if the representation type has a `constraint_violation_handler`**
    - If yes → Call `handler::on_violation()` with the error message
    - If no → Fall back to `MP_UNITS_EXPECTS` (contract checking macro)

2. **Contract checking macros** (when no handler exists):
    - Depend on the [`contracts` configuration](../../getting_started/installation_and_usage.md#contracts)
    - May be **disabled** in release builds or when set to `none`

3. **For guaranteed enforcement**, use a representation type with a handler:
    - The library provides `constrained<T, ErrorPolicy>` wrapper
    - Or specialize `constraint_violation_handler` for your custom type

!!! tip "Two Levels of Safety"

    **mp-units** provides two complementary safety mechanisms:

    - **Development safety**: Contract macros (GSL-Lite/MS-GSL) that may be disabled
    - **Production safety**: `constraint_violation_handler` that always runs

    This tutorial focuses on production safety for guaranteed enforcement.

!!! example "Example: Typical Sensor Operating Range"

    Industrial temperature sensors commonly operate in the range **-40°C to +85°C**.
    This tutorial defines `sensor_scale_origin` as a relative origin to `si::ice_point`
    with 0°C offset. This keeps sensor bounds isolated (doesn't affect other `ice_point`
    usage) while remaining convertible to Kelvin and Fahrenheit.


## Using `constrained<T, ErrorPolicy>` for Guaranteed Safety

The simplest way to get always-on bounds enforcement is using `constrained<T, ErrorPolicy>`
as your representation type:

### Step 1: Choose an Error Policy

The library provides two built-in policies in `<mp-units/constrained.h>`:

```cpp
#include <mp-units/constrained.h>

using namespace mp_units;

// Option 1: Throw exceptions (hosted environments)
using safe_double = constrained<double, throw_policy>;

// Option 2: Terminate immediately (all environments)
using safe_double = constrained<double, terminate_policy>;
```

You can also define custom policies:

```cpp
struct log_and_throw_policy {
  static void on_constraint_violation(std::string_view msg) {
    spdlog::error("Bounds violation: {}", msg);
    throw std::domain_error(std::string(msg));
  }
};

using logged_double = constrained<double, log_and_throw_policy>;
```

### Step 2: Use `constrained<T>` as Your Representation Type

```cpp
#include <mp-units/constrained.h>
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// Define representation type with exception policy
using safe_double = constrained<double, throw_policy>;

// Attach sensor operating range bounds directly to our sensor origin
// -40°C to +85°C absolute
inline constexpr struct sensor_scale_origin :
    relative_point_origin<si::ice_point + delta<deg_C>(0),
                          check_in_range{delta<deg_C>(-40), delta<deg_C>(85)}> {} sensor_scale_origin;

// Use safe_double as the representation type
using safe_temp = quantity_point<deg_C, sensor_scale_origin, safe_double>;
```

Now violations throw exceptions **in all build modes**:

```cpp
try {
  safe_temp temp{delta<deg_C>(150.0), sensor_scale_origin};  // Always throws! (150°C > 85°C)
} catch (const std::domain_error& e) {
  std::cerr << "Invalid temperature: " << e.what() << "\n";
  // Handle error gracefully
}
```


## Complete Example: Sensor Validation

Here's a production-ready example with guaranteed exception handling for temperature sensors:

```cpp
// ce-embed height=1300 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/constrained.h>
#include <mp-units/framework.h>
#include <mp-units/systems/si.h>
#include <optional>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// STEP 1: Define safe representation type
using safe_double = constrained<double, throw_policy>;

// STEP 2: Define sensor-specific origin with bounds (relative to ice_point, 0°C offset)
//         Bounds: -40°C to +85°C (typical sensor operating range)
inline constexpr struct sensor_scale_origin :
    relative_point_origin<si::ice_point + delta<deg_C>(0),
                          check_in_range{delta<deg_C>(-40), delta<deg_C>(85)}> {} sensor_scale_origin;

// STEP 3: Use safe_double as representation type
using safe_temp = quantity_point<deg_C, sensor_scale_origin, safe_double>;

// STEP 4: Safe sensor reading function
std::optional<safe_temp> read_sensor_safe(double raw_celsius)
{
  try {
    return safe_temp{delta<deg_C>(raw_celsius), sensor_scale_origin};
  } catch (const std::domain_error& e) {
    std::cerr << "Sensor reading invalid: " << e.what() << "\n";
    return std::nullopt;  // Signal failure
  }
}

// STEP 6: Application logic with fallback
int main()
{
  // Simulate sensor readings
  double readings[] = {22.5, 150.0, -10.0, 90.0, 25.0};

  for (double raw : readings) {
    auto temp = read_sensor_safe(raw);

    if (temp) {
      std::cout << "Valid: " << (*temp - sensor_scale_origin) << "\n";
    } else {
      std::cout << "Invalid reading " << raw << " °C - using fallback (20 °C)\n";
      // Use safe fallback
      auto fallback = safe_temp{delta<deg_C>(20.0), sensor_scale_origin};
      std::cout << "Fallback: " << (fallback - sensor_scale_origin) << "\n";
    }
  }
}
```

**Key points**:

- ✅ Invalid readings (150 °C, 90 °C) throw exceptions **in all build modes**
- ✅ No dependency on contract checking configuration
- ✅ Program continues with fallback values instead of crashing
- ✅ Errors are captured for debugging and monitoring
- ✅ Type safety is preserved - all valid operations still enforced


## Custom Error Policies

You can create custom error policies for domain-specific needs:

### Logging Policy

For monitoring and diagnostics with exceptions:

```cpp
#include <spdlog/spdlog.h>
#include <stdexcept>

struct logging_policy {
  static void on_constraint_violation(std::string_view msg)
  {
    spdlog::error("Bounds violation: {}", msg);
    throw std::domain_error(std::string(msg));
  }
};

using logged_double = constrained<double, logging_policy>;
```

### Terminate Policy with Logging

For safety-critical systems that must terminate but need audit trails:

```cpp
struct audit_terminate_policy {
  static void on_constraint_violation(std::string_view msg)
  {
    std::cerr << "[FATAL] Bounds violation: " << msg << std::endl;
    std::abort();
  }
};

using audited_double = constrained<double, audit_terminate_policy>;
```


## Best Practices

### When to Use Each Error Policy

| Scenario                    | Recommended Policy                         |
|-----------------------------|--------------------------------------------|
| Safety-critical systems     | `terminate_policy` (fail-fast)             |
| Web services, APIs          | `throw_policy` with logging                |
| Embedded with no exceptions | `terminate_policy` with audit logging      |
| Data processing pipelines   | `throw_policy` + `std::optional`           |
| User input validation       | `throw_policy` with user-friendly messages |
| Sensor fusion systems       | Custom recovery policy with fallback logic |

### General Guidelines

✅ **Do:**

- **Use `constrained<T, ErrorPolicy>`** for guaranteed enforcement across all build modes
- **Choose appropriate error policies** - throw for recoverable errors, terminate for safety-critical
- **Log violations** for debugging and monitoring (include in your policy)
- **Test your error policies** with deliberately invalid values
- **Document enforcement behavior** in your API
- **Keep policies simple** - they should only handle the error, not complex recovery logic

❌ **Don't:**

- **Don't rely only on contract macros** - they may be disabled in release builds
- **Don't silently ignore violations** - always throw or terminate
- **Don't put business logic in policies** - keep them focused on error handling
- **Don't catch and discard** exceptions without consideration
- **Don't assume violations are rare** - sensor data can be noisy


## Advanced: Custom `constraint_violation_handler`

If you have your own safe numeric types, you can specialize `constraint_violation_handler`
instead of using `constrained<T>`:

```cpp
// Your custom safe-double type
class my_safe_double {
  double value_;
public:
  explicit my_safe_double(double v) : value_{v} {}
  operator double() const { return value_; }
  // ... arithmetic operators ...
};

// Register violation handler
template<>
struct mp_units::constraint_violation_handler<my_safe_double> {
  static void on_violation(std::string_view msg) {
    throw std::domain_error(std::string(msg));
  }
};
```

Now `check_in_range` will use your handler for any `quantity_point` with `my_safe_double` rep.

!!! info "See Also"

    For comprehensive coverage of this topic, see
    [Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md).


## Related Topics

<!-- markdownlint-disable MD013 -->
- **[Bounded Quantity Points](bounded_quantity_points.md)** - Setting up bounds policies
- **[Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md)** - Deep dive into constraint enforcement
- **[Workshop: Bounds Validation](../../workshops/advanced/bounds_validation.md)** - Practice exercises
<!-- markdownlint-enable MD013 -->


## Summary

This tutorial covered how to ensure guaranteed bounds enforcement using `constrained<T, ErrorPolicy>`:

- **The problem**: Contract checking via `MP_UNITS_EXPECTS` can be disabled at compile-time
- **The solution**: Use `constrained<T, ErrorPolicy>` to ensure violations are always handled
- **Built-in policies**: `throw_policy` (exceptions) and `terminate_policy` (abort immediately)
- **Custom policies**: Implement `on_constraint_violation(std::string_view)` for
  domain-specific needs
- **Advanced**: Specialize `constraint_violation_handler<Rep>` for existing safe numeric types

For production systems with exception handling, `throw_policy` + `std::optional` provides
both safety and graceful error propagation.
