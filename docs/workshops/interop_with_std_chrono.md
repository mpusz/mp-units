# Workshop 6: Interop with `std::chrono`

Modern C++ applications often need to work with both time measurements (`std::chrono`) and
physical quantities (**mp-units**). This workshop shows how these complementary libraries
can work together effectively. We'll use an autonomous driving scenario where `std::chrono`
handles system timestamps while **mp-units** ensures type-safe calculations on quantities.

## Problem statement

An autonomous driving system records two time points using a `std::chrono` clock when the car
passes two checkpoints. At each checkpoint, the odometer reading is also captured.
The system needs to:

- Calculate trip statistics (_duration_, _distance_, _average speed_)
- Schedule a driver alert for an upcoming rest area

The challenge is to seamlessly convert between `std::chrono` time representations and
**mp-units** physical quantities.

## Your task

Given two checkpoint measurements with `std::chrono` timestamps and odometer readings:

1. Convert the `std::chrono` time difference to **mp-units** _duration_.
2. Compute the _average speed_ using **mp-units** (_distance_ / _duration_).
3. Display trip details with appropriate units (minutes for _duration_, km/h for _speed_).
4. Predict when the car will reach a rest area 5 km ahead, assuming constant _speed_.
5. Schedule a driver alert using the predicted arrival time as a `std::chrono::time_point`.

Here is the initial code for your exercise:

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <chrono>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

using car_clock = std::chrono::system_clock;

// Display trip statistics
void print_trip_stats(QuantityOf<isq::duration> auto duration,
                      QuantityOf<isq::distance> auto distance,
                      QuantityOf<isq::speed> auto avg_speed)
{
  std::cout << "Duration: " << duration << "\n";
  std::cout << "Distance: " << distance << "\n";
  std::cout << "Avg Speed: " << avg_speed << "\n";
}

// Simulate scheduling a driver alert
void schedule_driver_alert(car_clock::time_point alert_time)
{
  std::cout << "[notification] Driver alert scheduled for " << alert_time;
}

int main()
{
  // Simulated odometer and time point readings
  quantity_point odo1(12345.6 * km);
  quantity_point odo2(12378.9 * km);
  auto tp2 = car_clock::now();
  auto tp1 = tp2 - std::chrono::minutes{20}; // 20 minutes earlier

  // Display trip statistics
  // TODO: Convert the difference between chrono time points to mp-units duration
  // TODO: Compute average speed using mp-units
  // TODO: Display trip details (duration[min], distance[km], avg_speed[km/h])

  // Schedule alert
  // TODO: Predict arrival time at a rest area 5 km ahead (constant speed)
  // TODO: Schedule alert with the predicted arrival time
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <chrono>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    using car_clock = std::chrono::system_clock;

    // Display trip statistics
    void print_trip_stats(QuantityOf<isq::duration> auto duration,
                          QuantityOf<isq::distance> auto distance,
                          QuantityOf<isq::speed> auto avg_speed)
    {
      std::cout << "Duration: " << duration << "\n";
      std::cout << "Distance: " << distance << "\n";
      std::cout << "Avg Speed: " << avg_speed << "\n";
    }

    // Simulate scheduling a driver alert
    void schedule_driver_alert(car_clock::time_point alert_time)
    {
      std::cout << "[notification] Driver alert scheduled for " << alert_time;
    }

    int main()
    {
      // Simulated odometer and time point readings
      quantity_point odo1(12345.6 * km);
      quantity_point odo2(12378.9 * km);
      auto tp2 = car_clock::now();
      auto tp1 = tp2 - std::chrono::minutes{20}; // 20 minutes earlier

      // Display trip statistics
      quantity duration = quantity_point{tp2} - quantity_point{tp1};
      quantity distance = odo2 - odo1;
      quantity avg_speed = distance / duration;
      print_trip_stats(duration.in<double>(min), distance, avg_speed.in(km / h));

      // Schedule alert
      car_clock::duration dur_to_arrival = (5 * km / avg_speed).force_in<car_clock::rep>();
      schedule_driver_alert(tp2 + dur_to_arrival);
    }
    ```


??? abstract "What you learned?"

    ### Seamless interoperability between `std::chrono` and **mp-units**

    **mp-units** and `std::chrono` are complementary libraries with different strengths:

    ```cpp
    using car_clock = std::chrono::system_clock;

    auto tp1 = car_clock::now();                                // std::chrono for system time
    quantity_point qp1 = tp1;                                   // Implicit conversion to mp-units
    quantity duration = quantity_point{car_clock::now()} - qp1; // Time calculations in mp-units
    quantity avg_speed = distance / duration;                   // Dimensional analysis across quantity types!
    ```

    - **`std::chrono`** is designed for clocks, calendars, and system timestamps
    - **mp-units** is designed for dimensional analysis across different physical quantities
    - Converting between compatible types is implicit and zero-cost

    ### Implicit conversions for compatible types

    Converting between `std::chrono` and **mp-units** is seamless with CTAD:

    ```cpp
    auto tp2 = car_clock::now();
    auto tp1 = tp2 - std::chrono::minutes{20};

    // std::chrono → mp-units (always implicit with CTAD)
    quantity mp_duration = tp2 - tp1;  // Deduces exact unit and representation

    // mp-units → std::chrono (implicit when types match)
    car_clock::duration chrono_dur = mp_duration;  // OK, same resolution and representation
    ```

    **Why this works:**

    - `quantity` with CTAD deduces the exact unit and representation from `std::chrono::duration`
    - `quantity_point` with CTAD deduces from `std::chrono::time_point`
    - No conversion needed - types match perfectly
    - Conversion back to `std::chrono` is implicit when there's no precision loss

    ### Explicit conversions with `.force_in<Rep>(Unit)`

    When truncation or overflow is possible, use explicit conversion:

    ```cpp
    quantity avg_speed = distance / duration;  // mp-units calculation

    // Truncating conversion requires explicit cast:
    car_clock::duration dur = (5 * km / avg_speed).force_in<car_clock::rep>(unit_for<car_clock::duration>);
    schedule_alert(tp2 + dur);  // Use result with std::chrono
    ```

    The `.force_in<Rep>(Unit)` method:

    - Converts the quantity to the specified unit and representation type
    - Explicitly acknowledges potential precision loss (truncation, overflow)
    - Returns the representation type expected by `std::chrono` duration constructors

    ### Best practices for hybrid applications

    **Use each library for its strengths:**

    ```cpp
    // ✅ Good: Use std::chrono for timestamps and clocks
    auto checkpoint_time = car_clock::now();

    // ✅ Good: Use mp-units for calculations
    quantity duration = tp2 - tp1;
    quantity distance = odo2 - odo1;
    quantity avg_speed = distance / duration;  // Dimensional analysis!

    // ✅ Good: Convert units/representation as needed
    print_stats(duration.in<double>(min), distance, avg_speed.in(km / h));
    ```

    **Keep conversions at boundaries:**

    - Perform calculations using **mp-units** for type safety
    - Convert to/from `std::chrono` only when interfacing with APIs that require it
    - Let the compiler catch unit mismatches during development


## References

- [User's Guide: Interoperability with Other Libraries](../how_to_guides/interoperability_with_other_libraries.md)
- [API Reference](../reference/api_reference.md)


## Takeaways

- **mp-units** and `std::chrono` are complementary
    - use `chrono` for clocks and system time,
    - use `mp-units` for type-safe computations.
- Interoperability lets you leverage the strengths of both libraries in real-world systems.
- Converting between compatible `std::chrono` types and **mp-units** quantities is implicit.
- Only in case of truncating conversions we need explicit casts and conversion functions.
- Always keep internal logic type-safe. Don't reach for fundamental types unless necessary.
