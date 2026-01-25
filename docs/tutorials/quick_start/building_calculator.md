# Building a Calculator

Put everything together by building a complete _speed_ and _distance_ calculator!

**Goal**: Create a practical application using quantities  
**Time**: ~10 minutes

## A Complete Program

Let's build a calculator that handles multiple trips and computes statistics.

```cpp
// ce-embed height=800 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <print>

using namespace mp_units;

// Function to calculate average speed
// Notice: specific units in parameters and return type
quantity<si::kilo<si::metre> / non_si::minute> average_speed(quantity<si::kilo<si::metre>> distance,
                                                             quantity<non_si::minute> time)
{
  return distance / time;
}

int main()
{
  using namespace mp_units::si::unit_symbols;

  // Trip 1: Morning commute
  quantity morning_distance = 45.0 * km;
  quantity morning_time = 50.0 * min;

  // Trip 2: Afternoon errands
  quantity afternoon_distance = 23.0 * km;
  quantity afternoon_time = 35.0 * min;

  // Trip 3: Evening return
  quantity evening_distance = 48.0 * km;
  quantity evening_time = 55.0 * min;

  // Calculate totals
  quantity total_distance = morning_distance + afternoon_distance + evening_distance;
  quantity total_time = morning_time + afternoon_time + evening_time;

  // Calculate average speeds using our function
  quantity morning_avg = average_speed(morning_distance, morning_time);
  quantity afternoon_avg = average_speed(afternoon_distance, afternoon_time);
  quantity evening_avg = average_speed(evening_distance, evening_time);
  quantity overall_avg = average_speed(total_distance, total_time);

  // Display results
  std::println("=== Daily Travel Summary ===\n");

  std::println("Morning:    {} in {} (avg: {})", morning_distance, morning_time, morning_avg);
  std::println("Afternoon:  {} in {} (avg: {})", afternoon_distance, afternoon_time, afternoon_avg);
  std::println("Evening:    {} in {} (avg: {})", evening_distance, evening_time, evening_avg);

  std::println("\nTotal:      {} in {}", total_distance, total_time);
  std::println("Average:    {}", overall_avg);
}
```

**What this demonstrates:**

- **Function interfaces with quantities**: Explicit units in parameters and return type
- **Type safety**: Can't accidentally swap argument order (_distance_, _time_) vs
  (_time_, _distance_)
- Multiple quantity calculations
- Combining results (totals)
- Real-world application structure

## Key Takeaways

This simple calculator shows:

1. **Function interfaces**: Quantities can specify exact units in parameters and return types
2. **Type safety**: Can't swap argument order - the compiler catches such mistakes
3. **Readable code**: `average_speed(distance, time)` is self-documenting
4. **Automatic units**: Result shows `km/min` without explicit formatting

## Challenges

Enhance the calculator:

1. **Test type safety**: Try swapping arguments: `average_speed(morning_time, morning_distance)`.
   The compiler will catch this mistake - you can't accidentally pass _time_ where _distance_
   is expected!

2. **Improve formatting**: Those long decimal numbers look messy! Add precision formatting
   with `{::N[.2f]}` to show only 2 decimal places for the average _speeds_:

    ```cpp
    std::println("Average: {::N[.2f]}", overall_avg);
    ```

3. **Add fuel consumption**:

    ```cpp
    quantity fuel_used = 8.0 * L;
    quantity efficiency = total_distance / fuel_used;
    std::println("Fuel efficiency: {}", efficiency);
    ```

!!! tip "Want to learn about unit conversions?"

    In the next tutorial ([Unit Conversions](../working_with_units/unit_conversions.md)),
    you'll learn how to pass different units (like `m` and `s`) to functions expecting
    `km` and `min`, and how the library handles conversions safely!

## What You Learned?

✅ Building complete applications with quantities  
✅ Creating functions with specific unit requirements  
✅ Combining multiple calculations  
✅ Using `std::println` for cleaner output formatting  
✅ Floating-point values for precise calculations

!!! note "About `std::println`"

    While `std::println` is the recommended approach for modern C++ software, the following
    tutorials revert to using IO streams (`std::cout`) because they compile significantly
    faster on Compiler Explorer. This keeps the interactive examples responsive and enjoyable!
