---
tags:
  - Level - Advanced
  - System - SI
  - System - International
  - Feature - ISQ
  - Feature - Affine Space
  - Domain - Aerospace
  - Domain - Embedded Systems
  - Domain - Robotics
---

<!-- markdownlint-disable MD013 -->
# Kalman Filter Examples

[:material-file-code: `example/kalman_filter/`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter)

**Framework:**

- [`kalman.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman.h) - Type-safe Kalman filter abstractions

**Examples:**

- [`kalman_filter-example_1.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_1.cpp) - Golden bar weight estimation (α filter)
- [`kalman_filter-example_2.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_2.cpp) - Vehicle position tracking (α-β filter)
- [`kalman_filter-example_3.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_3.cpp) - Vehicle tracking with acceleration (α-β filter)
- [`kalman_filter-example_4.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_4.cpp) - Full α-β-γ filter
- [`kalman_filter-example_5.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_5.cpp) - Building height (Kalman filter)
- [`kalman_filter-example_6.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_6.cpp) - Gold bar temperature (Kalman filter)
- [`kalman_filter-example_7.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_7.cpp) - Liquid temperature (Kalman filter)
- [`kalman_filter-example_8.cpp`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_8.cpp) - Warming liquid (Kalman filter)
<!-- markdownlint-enable MD013 -->

These examples demonstrate implementing Kalman filtering algorithms with **mp-units**,
showcasing how physical quantities and units integrate naturally into state estimation
systems. The examples progress from simple α-β filters to full Kalman filters with
process noise, covering common scenarios in aerospace, navigation, and sensor fusion
applications.

## The Problem: State Estimation with Units

Kalman filtering estimates the true state of a system from noisy measurements.
In real-world applications:

- System states have **physical dimensions**: _position_ (_length_),
  _velocity_ (_length_/_time_), _acceleration_ (_length_/_time²_), _temperature_
- Measurements include **uncertainties** expressed in physical units (meters, m/s, degrees)
- **Process noise** and **measurement noise** have covariance matrices with appropriate units
- Kalman gains are **dimensionless** quantities resulting from variance ratios
- Different state variables have **time-derivative relationships**:
  _position_ → _velocity_ → _acceleration_

Common issues in traditional implementations:

- Mixing incompatible physical quantities (e.g., adding _position_ and _velocity_)
- Incorrect unit conversions in measurement updates
- Dimensionality mismatches in covariance extrapolation
- Loss of physical meaning in intermediate calculations

## Type-Safe Kalman Filter Framework

The [`kalman.h`](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman.h#L1)
header provides generic, type-safe abstractions for Kalman filtering:

### System State Representation

```cpp
template<mp_units::QuantityPoint... QPs>
  requires(sizeof...(QPs) > 0) && (sizeof...(QPs) <= 3) &&
          detail::are_time_derivatives<QPs::dimension...>
class system_state {
  std::tuple<QPs...> variables_;
  // ...
};
```

The `system_state` class template:

- Stores 1-3 related **quantity points** (absolute values on a measurement scale)
- Enforces **time-derivative relationships** via concept constraints
- Supports: _position_-only, _position-velocity_, or _position-velocity-acceleration_ states
- Uses `quantity_point` to distinguish absolute values (e.g., _altitude_ above sea level)
  from relative quantities

### State Estimation with Uncertainty

```cpp
template<mp_units::QuantityPoint QP, mp_units::QuantityPoint... Rest>
class system_state_estimate {
  state_type state_;
  variance_type variance_;  // Type computed as pow<2>(standard_deviation_type)

  [[nodiscard]] constexpr standard_deviation_type standard_deviation() const
  { return sqrt(variance_); }
};
```

The `system_state_estimate` combines:

- System state (_position_, _velocity_, etc.)
- **Variance** with correct units (e.g., m² for _position_ uncertainty)
- Automatic conversion between variance and standard deviation

### Core Kalman Operations

All operations preserve dimensional correctness:

=== "Kalman Gain"

    ```cpp
    template<Quantity Q1, Quantity Q2>
      requires requires { get_common_reference(Q1::reference, Q2::reference); }
    [[nodiscard]] constexpr quantity<dimensionless[one]> kalman_gain(Q1 variance_in_estimate,
                                                                     Q2 variance_in_measurement)
    {
      return variance_in_estimate / (variance_in_estimate + variance_in_measurement);
    }
    ```

    The Kalman gain is **dimensionless** (variance ratio), enabling:

    - Type-safe blending between prediction and measurement
    - Automatic unit cancellation in variance division
    - Range validation (always between 0 and 1)

=== "State Update"

    ```cpp
    template<typename QP, QuantityPoint QM, QuantityOf<dimensionless> K>
      requires(implicitly_convertible(QM::quantity_spec, QP::quantity_spec))
    [[nodiscard]] constexpr system_state<QP> state_update(const system_state<QP>& predicted,
                                                          QM measured, K gain)
    {
      return system_state<QP>{get<0>(predicted) + gain * (measured - get<0>(predicted))};
    }
    ```

    State update computes: **state_new = state_pred + K × (measurement − state_pred)**

    - Dimensionless gain `K` scales the innovation (measurement residual)
    - Quantity point arithmetic ensures dimensional correctness
    - Innovation `(measured - predicted)` is a relative quantity

=== "State Extrapolation"

    ```cpp
    template<typename QP1, typename QP2, QuantityOf<isq::time> T>
    [[nodiscard]] constexpr system_state<QP1, QP2> state_extrapolation(const system_state<QP1, QP2>& estimated,
                                                                       T interval)
    {
      auto to_quantity = [](const auto& qp) { return qp.quantity_ref_from(qp.point_origin); };
      const auto qp1 = fma(to_quantity(get<1>(estimated)), interval, get<0>(estimated));
      const auto qp2 = get<1>(estimated);
      return system_state<QP1, QP2>{qp1, qp2};
    }
    ```

    Projects state forward in time using physics:

    - **position_new = position + velocity × Δt**
    - Handles quantity_point to quantity conversions automatically
    - For 3-state systems, includes **½ × acceleration × Δt²** term

=== "Covariance Update"

    ```cpp
    template<Quantity Q, QuantityOf<dimensionless> K>
    [[nodiscard]] constexpr Q covariance_update(Q uncertainty, K gain)
    {
      return (1 * one - gain) * uncertainty;
    }
    ```

    Updates uncertainty after measurement: **P_new = (1 − K) × P_pred**

=== "Covariance Extrapolation"

    ```cpp
    template<Quantity Q1, Quantity Q2>
      requires requires { get_common_reference(Q1::reference, Q2::reference); }
    [[nodiscard]] constexpr Quantity auto covariance_extrapolation(Q1 uncertainty, Q2 process_noise_variance)
    {
      return uncertainty + process_noise_variance;
    }
    ```

    Projects uncertainty forward: **P_pred = P + Q**

    - Requires compatible quantity types (same physical dimension)
    - Process noise `Q` accounts for model imperfections

## Example Progression

The 8 examples demonstrate increasing complexity:

### Examples 1-4: α-β(-γ) Filters

Simple fixed-gain filters with manually tuned gains:

| Example                                                                                                                                 | Description                                     | State Variables                        | Fixed Gains               | Measurement   |
|-----------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------|----------------------------------------|---------------------------|---------------|
| [**1**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_1.cpp) | Golden bar _weight_ estimation                  | _mass_                                 | α = 1/n (dynamic)         | 1D _mass_     |
| [**2**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_2.cpp) | Vehicle _position_ tracking (constant velocity) | _position_, velocity                   | α = 0.2, β = 0.1          | 1D _position_ |
| [**3**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_3.cpp) | Vehicle tracking with _acceleration_            | _position_, velocity                   | α = 0.2, β = 0.1          | 1D _position_ |
| [**4**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_4.cpp) | Full α-β-γ filter (constant _acceleration_)     | _position_, _velocity_, _acceleration_ | α = 0.5, β = 0.4, γ = 0.1 | 1D _position_ |

**Key Feature**: Examples 2-4 demonstrate multi-state tracking where:

- Only **_position_ is measured** (e.g., GPS coordinates)
- **_Velocity_ and _acceleration_ are estimated** from position innovations
- Gains distributed across state variables: α for _position_, β for vel_ocity, γ for _acceleration_

=== "Example 1: Simple α Filter"

    ```cpp
    using qp = quantity_point<isq::mass[g]>;
    using state = kalman::system_state<
      qp
    >;

    const state initial_guess{qp{1 * kg}};
    state next = initial_guess;

    for (int index = 1; const auto& measurement : measurements) {
      const quantity gain = 1. / index;  // Decreasing gain over time
      const state current = state_update(next, measurement, gain);
      next = current;
    }
    ```

    Estimates golden bar _mass_ from 10 noisy measurements. The gain **1/n** gives equal
    weight to all measurements, computing a running average. No state extrapolation
    (static system).

=== "Example 2: α-β Filter"

    ```cpp
    using qp = quantity_point<isq::displacement[m]>;
    using state = kalman::system_state<
      qp,
      quantity_point<isq::velocity[m/s]>
    >;

    const quantity interval = isq::duration(5 * s);
    const state initial{qp{30 * km}, quantity_point{40 * m/s}};
    std::array gain = {0.2 * one, 0.1 * one};  // α and β

    state next = state_extrapolation(initial, interval);
    for (const auto& measurement : measurements) {
      const state current = state_update(next, measurement, gain, interval);
      next = state_extrapolation(current, interval);
    }
    ```

    Tracks vehicle with constant _velocity_ (40 m/s). The α-β filter:

    - **α = 0.2**: Updates _position_ estimate from measurement innovation
    - **β = 0.1**: Updates _velocity_ estimate from same innovation
    - Measurement interval: 5 seconds between GPS readings

=== "Example 4: α-β-γ Filter"

    ```cpp
    using qp = quantity_point<isq::displacement[m]>;
    using state = kalman::system_state<
      qp,
      quantity_point<isq::velocity[m/s]>,
      quantity_point<isq::acceleration[m/s2]>
    >;

    const quantity interval = isq::duration(5 * s);
    const state initial{qp{30 * km}, quantity_point{50 * m/s}, quantity_point{0 * m/s2}};
    std::array gain = {0.5 * one, 0.4 * one, 0.1 * one};  // α, β, γ

    state next = state_extrapolation(initial, interval);
    for (const auto& measurement : measurements) {
      const state current = state_update(next, measurement, gain, interval);
      next = state_extrapolation(current, interval);
    }
    ```

    Full 3-state filter for _accelerating_ vehicle. The γ gain estimates _acceleration_
    from the same _position_ measurements that update _position_ (α) and _velocity_ (β).

### Examples 5-8: Full Kalman Filters

Adaptive-gain filters that compute Kalman gain from measurement and process uncertainty:

| Example                                                                                                                                 | Description                       | Measurement               | Process Noise | Initial Uncertainty |
|-----------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------|---------------------------|---------------|---------------------|
| [**5**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_5.cpp) | Building _height_ (static)        | _height_ (σ = 5 m)        | None          | 15 m                |
| [**6**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_6.cpp) | Liquid _temperature_ (stable)     | _temperature_ (σ = 0.1°C) | 0.0001 °C²    | 100°C               |
| [**7**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_7.cpp) | Liquid _temperature_ (changing)   | _temperature_ (σ = 0.1°C) | 0.0001 °C²    | 100°C               |
| [**8**](https://github.com/mpusz/units/blob/c54d18e4892d8b4c0173054750aca5507fbf8e2e/example/kalman_filter/kalman_filter-example_8.cpp) | Liquid _temperature_ (high-noise) | _temperature_ (σ = 0.1°C) | 0.15 °C²      | 100°C               |

**Key Feature**: The Kalman gain is **computed dynamically** from uncertainty estimates:

$$K_k = \frac{P_{k|k-1}}{P_{k|k-1} + R}$$

Where:

- $P_{k|k-1}$: Predicted estimate covariance
- $R$: Measurement noise covariance
- Both have units of **variance** (e.g., m², °C²)
- $K_k$: Dimensionless Kalman gain (0 to 1)

=== "Example 5: Static System"

    ```cpp
    using qp = quantity_point<isq::height[m]>;
    using estimate = kalman::system_state_estimate<qp>;

    const estimate initial{state{qp{60. * m}}, 15. * m};              // 60m ± 15m
    const quantity measurement_error = isq::height(5. * m);
    const quantity measurement_variance = pow<2>(measurement_error);  // 25 m²

    auto predict = [](const estimate& current) {
      return current;  // No state change (static system)
    };

    estimate next = predict(initial);
    for (const auto& measurement : measurements) {
      const quantity gain = kalman::kalman_gain(next.variance(), measurement_variance);
      const estimate current = state_estimate_update(next, measurement, gain);
      next = predict(current);
    }
    ```

    Estimates building _height_ (60 m true value) from 10 noisy measurements:

    - Initial uncertainty: **15 m standard deviation** (225 m² variance)
    - Measurement noise: **5 m standard deviation** (25 m² variance)
    - No process noise (building height doesn't change)
    - Uncertainty **decreases** with each measurement

=== "Example 8: Dynamic System"

    ```cpp
    using qp = quantity_point<isq::Celsius_temperature[deg_C]>;
    using estimate = kalman::system_state_estimate<qp>;

    const quantity process_noise_variance = delta<pow<2>(deg_C)>(0.15);
    const estimate initial{state{qp{delta<deg_C>(10.)}}, delta<deg_C>(100.)}; // 10°C ± 100°C
    const quantity measurement_error = delta<deg_C>(0.1);
    const quantity measurement_variance = pow<2>(measurement_error);

    auto predict = [=](const estimate& current) {
      return estimate{
        current.state(),
        covariance_extrapolation(current.variance(), process_noise_variance)
      };
    };

    estimate next = predict(initial);
    for (const auto& measurement : measurements) {
      const quantity gain = kalman::kalman_gain(next.variance(), measurement_variance);
      const estimate current = state_estimate_update(next, measurement, gain);
      next = predict(current);
    }
    ```

    Tracks warming liquid _temperature_ (true value increasing):

    - Initial uncertainty: **100°C standard deviation** (very uncertain)
    - Measurement noise: **0.1°C** (precise sensor)
    - Process noise: **0.15 °C²** variance (_temperature_ rising unpredictably)
    - Uncertainty **converges** to balance process and measurement noise

## Key Features Demonstrated

### 1. Quantity Point Semantics

```cpp
quantity_point<isq::displacement[m]> position;  // Absolute position
quantity<isq::displacement[m]> distance;        // Relative distance

auto innovation = measured_position - predicted_position;  // quantity
auto new_position = old_position + innovation;             // quantity_point
```

**Quantity points** represent absolute values (points on a scale), while **quantities**
are relative (differences or deltas). The type system prevents mixing them incorrectly.

### 2. Dimensional Analysis in Filtering

```cpp
// State extrapolation: position_new = position + velocity × time
auto qp1 = fma(to_quantity(velocity_qp), time_interval, position_qp);
//         ↑                 ↑             ↑              ↑
//         |                 |             |              quantity_point<m>
//         |                 quantity<m/s> quantity<s>
//         |                 └─────── quantity<m> ────────┘
//         quantity_point<m>

// Acceleration contribution: ½ × acceleration × time²
auto term = acceleration_qp * pow<2>(time_interval) / 2;
//          ↑                  ↑
//          quantity<m/s²>     quantity<s²>
//          └──────── quantity<m> ──────────┘
```

Every operation preserves physical dimensions, caught at **compile time**.

### 3. Variance and Standard Deviation

```cpp
class system_state_estimate {
  using standard_deviation_type = QP::quantity_type;  // e.g., quantity<isq::height[m]>
  using variance_type = quantity<pow<2>(standard_deviation_type::reference), ...>;
  //                                    ↑
  //                              e.g., quantity<pow<2>(isq::height[m])> = quantity<m²>

  variance_type variance_;

  [[nodiscard]] constexpr standard_deviation_type standard_deviation() const
  { return sqrt(variance_); }  // √(m²) = m
};
```

The type system enforces the relationship between variance (σ²) and standard deviation (σ),
preventing errors like adding standard deviations instead of variances in covariance matrices.

### 4. Generic State Sizes

The framework handles 1, 2, or 3 state variables:

```cpp
// 1-state (position only)
system_state<quantity_point<isq::mass[g]>>

// 2-state (position + velocity)
system_state<quantity_point<isq::displacement[m]>,
             quantity_point<isq::velocity[m/s]>>

// 3-state (position + velocity + acceleration)
system_state<quantity_point<isq::displacement[m]>,
             quantity_point<isq::velocity[m/s]>,
             quantity_point<isq::acceleration[m/s2]>>
```

The concept constraint `are_time_derivatives<QPs::dimension...>` ensures state variables
have the correct dimensional relationships (each is the time derivative of the previous).

### 5. Measurement Unit Flexibility

```cpp
// Distance measurements in different units
const std::array measurements = {
  qp{30'171 * m}, qp{30'353 * m}, qp{30'756 * m}  // meters
};

// Or kilometers
const qp initial_position{30 * km};  // Automatically converts to base unit

// Temperature measurements
const std::array temps = {
  qp{delta<deg_C>(50.486)},  // Celsius
  qp{delta<deg_C>(50.963)}
};
```

Measurements can use any compatible unit. The framework handles conversions automatically
while preserving dimensional correctness.

### 6. Custom Formatting for System States

```cpp
std::cout << MP_UNITS_STD_FMT::format(
  "{:2} | {:8} | {:23:0[:N[.2f]]1[:N[.2f]]} | {:23:0[:N[.2f]]1[:N[.2f]]}\n",
  iteration, measured, current_state, next_state
);
// Output:
//  N | Measured |          Curr. Estimate |           Next Estimate
//  1 |  30221 m |    30244.20 m 49.42 m/s |    30491.30 m 49.42 m/s
//                ↑                          ↑
//            position, velocity (current)   position, velocity (next)
```

The custom `formatter` for `system_state`:

- Formats each component with individual specifications: `0[...]` for first state, `1[...]`
  for second, etc.
- Aligns multi-component states in tabular output
- Preserves unit symbols in formatted output

## Real-World Applications

These examples apply to:

- **Aerospace**: Aircraft/spacecraft _position_ and _velocity_ tracking from GPS/INS
- **Robotics**: Mobile robot localization and navigation with noisy sensors
- **Embedded Systems**: Sensor fusion in resource-constrained environments (IMU, odometry)
- **Process Control**: _Temperature_, _pressure_, _flow rate_ estimation in industrial systems
- **Weather Forecasting**: Atmospheric state estimation from satellite/ground observations

The type-safe approach prevents common bugs:

- ❌ Mixing meters with meters/second in state updates
- ❌ Adding variance and standard deviation incorrectly
- ❌ Incorrect time derivative relationships (e.g., _position_ → _acceleration_ without _velocity_)
- ❌ Dimensionally inconsistent Kalman gain computations
- ✅ All caught at **compile time** with clear error messages

## Learn More

- [Kalman Filter Tutorial](https://www.kalmanfilter.net/) - Source of example scenarios
