# Bounds Validation Workshop

Practice implementing production-ready bounds validation with custom contract handlers
for real-world integration scenarios.

!!! tip "Prerequisites"

    Complete the [Custom Contract Handlers](../../tutorials/affine_space/custom_contract_handlers.md)
    tutorial first to understand the concepts.


## Problem Statement

You're building an aviation weather monitoring system that integrates data from external
weather stations. The system receives:

- **Barometric pressure**: 870-1085 hPa (valid atmospheric range)
- **Wind speed**: 0-150 kt (tropical cyclone maximum)  
- **Altitude**: -500 m to 9'000 m (Dead Sea to high-altitude airports)

The external weather API can return invalid data due to:

- Sensor malfunctions (readings out of physical range)
- Data corruption during transmission
- Incorrect unit conversions by third-party systems
- Timezone/location mismatches

Your system must:

1. ✅ **Validate** all incoming data against physical bounds
2. ✅ **Log** violations for monitoring and debugging
3. ✅ **Throw exceptions** for invalid data (don't terminate!)
4. ✅ **Provide type-safe fallbacks** when validation fails
5. ✅ **Track violation statistics** for reliability metrics


## Your Task

Implement a robust weather data validation system with custom error policies.

Create an error policy that:

- Logs violations with timestamp and message using `std::chrono`
- Throws `std::domain_error` exception (don't terminate!)

```cpp
// ce-embed height=1400 compiler=clang2110 flags="-std=c++23 -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <chrono>
#include <iostream>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::yard_pound::unit_symbols;

// Violation statistics (provided)
struct violation_stats {
  int pressure_violations = 0;
  int wind_violations = 0;
  int altitude_violations = 0;

  void print() const
  {
    std::cout << "\n=== Violation Statistics ===\n";
    std::cout << "Pressure: " << pressure_violations << "\n";
    std::cout << "Wind: " << wind_violations << "\n";
    std::cout << "Altitude: " << altitude_violations << "\n";
  }
};
violation_stats stats;

// TODO: Define a custom error policy named "weather_policy"
// Hint 1: Create a struct with static method on_constraint_violation(std::string_view msg)
// Hint 2: Log `msg` with timestamp obtained via `std::chrono::system_clock::now();`
// Hint 3: Throw std::domain_error(std::string(msg)) after logging

// TODO: Create type alias for safe representation
// using safe_double = constrained<double, weather_policy>;

// TODO: Define three absolute point origins with bounds as template parameters:
// 1. qnh_zero for barometric pressure (isq::pressure) with check_in_range{870 * hPa, 1085 * hPa}
// 2. calm_air for wind speed (isq::speed) with check_in_range{0 * kt, 150 * kt}
// 3. mean_sea_level for altitude (isq::altitude) with check_in_range{-500 * m, 9'000 * m}

// Type aliases with safe representation
using safe_pressure = quantity_point<hPa, qnh_zero, safe_double>;
using safe_wind = quantity_point<kt, calm_air, safe_double>;
using safe_altitude = quantity_point<m, mean_sea_level, safe_double>;

// Safe reading functions (provided - focus on units-related code)
std::optional<safe_pressure> safe_read_pressure(double value_hpa)
{
  try {
    return safe_pressure{value_hpa * hPa, qnh_zero};
  } catch (const std::domain_error&) {
    stats.pressure_violations++;
    return std::nullopt;
  }
}

std::optional<safe_wind> safe_read_wind_speed(double value_kt)
{
  try {
    return safe_wind{value_kt * kt, calm_air};
  } catch (const std::domain_error&) {
    stats.wind_violations++;
    return std::nullopt;
  }
}

std::optional<safe_altitude> safe_read_altitude(double value_m)
{
  try {
    return safe_altitude{value_m * m, mean_sea_level};
  } catch (const std::domain_error&) {
    stats.altitude_violations++;
    return std::nullopt;
  }
}

int main()
{
  // Test data from external API (some invalid)
  struct WeatherReport {
    double pressure_hpa;
    double wind_kt;
    double altitude_m;
  };

  WeatherReport reports[] = {
    {1013.25, 25.0, 150.0},   // Valid
    {1200.0, 15.0, 300.0},    // Invalid pressure (too high)
    {1015.0, 200.0, 500.0},   // Invalid wind (hurricane force!)
    {950.0, 10.0, 12000.0},   // Invalid altitude (above range)
    {850.0, 5.0, 100.0},      // Invalid pressure (too low)
    {1010.0, -5.0, 200.0},    // Invalid wind (negative)
    {1005.0, 30.0, 1500.0},   // Valid
  };

  std::cout << "Processing " << sizeof(reports) / sizeof(reports[0]) << " weather reports...\n\n";

  for (const auto& [i, r] : std::views::enumerate(reports)) {
    std::cout << "Report #" << i + 1 << ":\n";

    auto pressure = safe_read_pressure(r.pressure_hpa);
    auto wind = safe_read_wind_speed(r.wind_kt);
    auto altitude = safe_read_altitude(r.altitude_m);

    if (pressure && wind && altitude) {
      std::cout << "  ✓ Valid: P=" << (*pressure - qnh_zero)
                << ", W=" << (*wind - calm_air)
                << ", A=" << (*altitude - mean_sea_level) << "\n";
    } else {
      std::cout << "  ✗ REJECTED - using fallback values\n";

      // Use safe fallback values
      safe_pressure fallback_pressure{1013.25 * hPa, qnh_zero};
      safe_wind fallback_wind{0.0 * kt, calm_air};
      safe_altitude fallback_altitude{0.0 * m, mean_sea_level};

      std::cout << "  → Fallback: P=" << (fallback_pressure - qnh_zero)
                << ", W=" << (fallback_wind - calm_air)  
                << ", A=" << (fallback_altitude - mean_sea_level) << "\n";
    }
    std::cout << "\n";
  }

  stats.print();
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/core.h>
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/yard_pound.h>
    #include <chrono>
    #include <iostream>
    #include <optional>
    #include <ranges>
    #include <stdexcept>
    #include <string>
    #include <string_view>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;
    using namespace mp_units::yard_pound::unit_symbols;

    // Violation statistics
    struct violation_stats {
      int pressure_violations = 0;
      int wind_violations = 0;
      int altitude_violations = 0;

      void print() const
      {
        std::cout << "\n=== Violation Statistics ===\n";
        std::cout << "Pressure: " << pressure_violations << "\n";
        std::cout << "Wind: " << wind_violations << "\n";
        std::cout << "Altitude: " << altitude_violations << "\n";
      }
    };
    violation_stats stats;

    // Custom error policy
    struct weather_policy {
      static void on_constraint_violation(std::string_view msg)
      {
        const auto timestamp = std::chrono::system_clock::now();
        std::cerr << "[" << timestamp << "] BOUNDS VIOLATION: " << msg << "\n";
        throw std::domain_error(std::string(msg));
      }
    };

    // Safe representation type
    using safe_double = constrained<double, weather_policy>;

    // Define point origins with bounds as template parameters
    inline constexpr struct qnh_zero :
        absolute_point_origin<isq::pressure, check_in_range{870 * hPa, 1085 * hPa}> {} qnh_zero;

    inline constexpr struct calm_air :
        absolute_point_origin<isq::speed, check_in_range{0 * kt, 150 * kt}> {} calm_air;

    inline constexpr struct mean_sea_level :
        absolute_point_origin<isq::altitude, check_in_range{-500 * m, 9'000 * m}> {} mean_sea_level;

    // Type aliases with safe representation
    using safe_pressure = quantity_point<hPa, qnh_zero, safe_double>;
    using safe_wind = quantity_point<kt, calm_air, safe_double>;
    using safe_altitude = quantity_point<m, mean_sea_level, safe_double>;

    // Safe reading functions
    std::optional<safe_pressure> safe_read_pressure(double value_hpa)
    {
      try {
        return safe_pressure{value_hpa * hPa, qnh_zero};
      } catch (const std::domain_error&) {
        stats.pressure_violations++;
        return std::nullopt;
      }
    }

    std::optional<safe_wind> safe_read_wind_speed(double value_kt)
    {
      try {
        return safe_wind{value_kt * kt, calm_air};
      } catch (const std::domain_error&) {
        stats.wind_violations++;
        return std::nullopt;
      }
    }

    std::optional<safe_altitude> safe_read_altitude(double value_m)
    {
      try {
        return safe_altitude{value_m * m, mean_sea_level};
      } catch (const std::domain_error&) {
        stats.altitude_violations++;
        return std::nullopt;
      }
    }

    int main()
    {
      // Test data from external API (some invalid)
      struct WeatherReport {
        double pressure_hpa;
        double wind_kt;
        double altitude_m;
      };

      WeatherReport reports[] = {
        {1013.25, 25.0, 150.0},   // Valid
        {1200.0, 15.0, 300.0},    // Invalid pressure (too high)
        {1015.0, 200.0, 500.0},   // Invalid wind (hurricane force!)
        {950.0, 10.0, 12000.0},   // Invalid altitude (above range)
        {850.0, 5.0, 100.0},      // Invalid pressure (too low)
        {1010.0, -5.0, 200.0},    // Invalid wind (negative)
        {1005.0, 30.0, 1500.0},   // Valid
      };

      std::cout << "Processing " << sizeof(reports) / sizeof(reports[0]) << " weather reports...\n\n";

      for (const auto& [i, r] : std::views::enumerate(reports)) {
        std::cout << "Report #" << i + 1 << ":\n";

        auto pressure = safe_read_pressure(r.pressure_hpa);
        auto wind = safe_read_wind_speed(r.wind_kt);
        auto altitude = safe_read_altitude(r.altitude_m);

        if (pressure && wind && altitude) {
          std::cout << "  ✓ Valid: P=" << (*pressure - qnh_zero)
                    << ", W=" << (*wind - calm_air)
                    << ", A=" << (*altitude - mean_sea_level) << "\n";
        } else {
          std::cout << "  ✗ REJECTED - using fallback values\n";

          // Use safe fallback values
          safe_pressure fallback_pressure{1013.25 * hPa, qnh_zero};
          safe_wind fallback_wind{0.0 * kt, calm_air};
          safe_altitude fallback_altitude{0.0 * m, mean_sea_level};

          std::cout << "  → Fallback: P=" << (fallback_pressure - qnh_zero)
                    << ", W=" << (fallback_wind - calm_air)  
                    << ", A=" << (fallback_altitude - mean_sea_level) << "\n";
        }
        std::cout << "\n";
      }

      stats.print();
    }
    ```

??? abstract "What you learned?"

    ### Production-Ready Constraint Enforcement

    - **Custom error policies** enable production-grade validation with exceptions and logging
    - **`constrained<T, ErrorPolicy>`** guarantees enforcement regardless of contract checking mode
    - **`on_constraint_violation(std::string_view)`** receives detailed error messages
    - **Log violations** for debugging and monitoring in production systems

    ### Integrating with Application Code

    - Use **`std::optional`** or **`std::expected`** to propagate validation failures safely
    - Provide **type-safe fallback values** when external data is invalid
    - Track **violation statistics** to identify data quality issues
    - Test with **invalid data** to verify policy behavior

    ### Real-World Patterns

    - **Aviation data may be noisy** - expect regular violations from sensors/APIs
    - **Always log violations** - they indicate real hardware or integration problems
    - **Never silently ignore** constraint violations - they're early warnings
    - **Monitor violation rates** - sharp increases signal system degradation


## Takeaways

- ✅ **`constrained<T, ErrorPolicy>`** enables production-ready bounds validation
- ✅ Custom error policies provide **exceptions + logging** for real-world systems
- ✅ **Bounds are checked automatically** when constructing constrained quantity points
- ✅ Use **`std::optional`** or **`std::expected`** to handle validation failures gracefully
- ✅ Always **log violations** - they indicate real problems with data sources
- ✅ Track **violation statistics** to monitor data quality over time


## Next Steps

- **[Tutorial: Custom Contract Handlers](../../tutorials/affine_space/custom_contract_handlers.md)** -
  Review the concepts
- **[Ensure Ultimate Safety](../../how_to_guides/advanced_usage/ultimate_safety.md)** -
  Deep dive into constraint enforcement
- **[Bounded Quantity Points Tutorial](../../tutorials/affine_space/bounded_quantity_points.md)** -
  Learn about different bounds policies
- **Production deployment** - Integrate with your logging/monitoring infrastructure
