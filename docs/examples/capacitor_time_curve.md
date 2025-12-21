---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- Feature - Mathematical Operations
- Feature - Exponential Functions
- Feature - Auto Prefix Selection
- Feature - Quantity Concepts
- Domain - Electronics
- Domain - Engineering
---

# Capacitor Discharge with Exponential Functions

[Try it live on Compiler Explorer](https://godbolt.org/z/fMK1dah7n){ .md-button }

## Overview

This example demonstrates using mathematical functions with dimensional analysis by modeling
an RC circuit's capacitor discharge. It showcases:

- How exponential functions work with dimensionless quantities
- Automatic SI prefix selection for readable output across wide value ranges
- Dimensional analysis ensuring correct physics

## Circuit Model

The example simulates an RC discharge circuit where a charged capacitor discharges through
a resistor, following the exponential decay equation:

$$V(t) = V_0 \cdot e^{-\frac{t}{RC}}$$

<div class="grid" markdown>

<div style="text-align: center; margin: 1em 0;">
  <svg width="270" height="180" viewBox="0 0 270 180" xmlns="http://www.w3.org/2000/svg" style="max-width: 100%; height: auto;">
    <!-- Voltage source V₀ (left side) -->
    <circle cx="30" cy="90" r="25" fill="none" stroke="#666" stroke-width="2"/>
    <text x="30" y="95" font-size="16" text-anchor="middle" fill="#666">V₀</text>

    <!-- Top wire to resistor -->
    <line x1="30" y1="65" x2="30" y2="30" stroke="#666" stroke-width="2"/>
    <line x1="30" y1="30" x2="120" y2="30" stroke="#666" stroke-width="2"/>

    <!-- Resistor (zigzag pattern, centered on horizontal wire at y=30) -->
    <polyline points="120,30 125,20 135,40 145,20 155,40 165,20 175,40 180,30"
            fill="none" stroke="#666" stroke-width="2" stroke-linejoin="miter"/>
    <text x="150" y="15" font-size="14" text-anchor="middle" fill="#666">R</text>

    <!-- Wire from resistor to capacitor -->
    <line x1="180" y1="30" x2="210" y2="30" stroke="#666" stroke-width="2"/>
    <line x1="210" y1="30" x2="210" y2="70" stroke="#666" stroke-width="2"/>

    <!-- Capacitor (parallel plates, centered on vertical wire at x=210) -->
    <line x1="200" y1="70" x2="220" y2="70" stroke="#666" stroke-width="3"/>
    <line x1="200" y1="80" x2="220" y2="80" stroke="#666" stroke-width="3"/>
    <text x="235" y="77" font-size="14" text-anchor="middle" fill="#666">C</text>

    <!-- Bottom wire -->
    <line x1="210" y1="80" x2="210" y2="150" stroke="#666" stroke-width="2"/>
    <line x1="210" y1="150" x2="30" y2="150" stroke="#666" stroke-width="2"/>
    <line x1="30" y1="150" x2="30" y2="115" stroke="#666" stroke-width="2"/>
  </svg>
</div>

- **Parameters:**
    - **Capacitance**: 0.47 μF
    - **Initial voltage**: 5.0 V
    - **Resistance**: 4.7 kΩ
    - **Time constant**: τ = RC = 2.209 ms

<div style="text-align: center; margin: 2em 0;">
  <svg width="500" height="300" viewBox="0 0 500 300" xmlns="http://www.w3.org/2000/svg" style="max-width: 100%; height: auto;">
    <!-- Graph background -->
    <rect x="60" y="20" width="420" height="240" fill="#f8f8f8" stroke="#ddd" stroke-width="1"/>

    <!-- Grid lines (horizontal) - evenly spaced every 48px (1V per 48px) -->
    <line x1="60" y1="68" x2="480" y2="68" stroke="#ddd" stroke-width="1"/>
    <line x1="60" y1="116" x2="480" y2="116" stroke="#ddd" stroke-width="1"/>
    <line x1="60" y1="164" x2="480" y2="164" stroke="#ddd" stroke-width="1"/>
    <line x1="60" y1="212" x2="480" y2="212" stroke="#ddd" stroke-width="1"/>

    <!-- Grid lines (vertical) - time constants -->
    <line x1="144" y1="20" x2="144" y2="260" stroke="#ddd" stroke-width="1" stroke-dasharray="4,4"/>
    <line x1="228" y1="20" x2="228" y2="260" stroke="#ddd" stroke-width="1" stroke-dasharray="4,4"/>
    <line x1="312" y1="20" x2="312" y2="260" stroke="#ddd" stroke-width="1" stroke-dasharray="4,4"/>
    <line x1="396" y1="20" x2="396" y2="260" stroke="#ddd" stroke-width="1" stroke-dasharray="4,4"/>

    <!-- Axes -->
    <line x1="60" y1="20" x2="60" y2="260" stroke="#333" stroke-width="2"/>
    <line x1="60" y1="260" x2="480" y2="260" stroke="#333" stroke-width="2"/>

    <!-- Exponential decay curve: V(t) = 5.0 * exp(-t/τ) -->
    <!-- For 50 points from t=0 to t=5τ in steps of 0.1τ -->
    <!-- x = 60 + (t/5τ)*420, y = 260 - V(t)*48 -->
    <path d="M 60,20
             L 68.4,44 L 76.8,65 L 85.2,83 L 93.6,100 L 102,114 L 110.4,127 L 118.8,139 L 127.2,149 L 135.6,159
             L 144,168 L 152.4,176 L 160.8,183 L 169.2,190 L 177.6,196 L 186,202 L 194.4,207 L 202.8,212 L 211.2,216 L 219.6,220
             L 228,224 L 236.4,227 L 244.8,230 L 253.2,233 L 261.6,236 L 270,238 L 278.4,240 L 286.8,242 L 295.2,244 L 303.6,246
             L 312,248 L 320.4,249 L 328.8,251 L 337.2,252 L 345.6,253 L 354,254 L 362.4,255 L 370.8,256 L 379.2,257 L 387.6,258
             L 396,258 L 404.4,259 L 412.8,259 L 421.2,259 L 429.6,260 L 438,260 L 446.4,260 L 454.8,260 L 463.2,260 L 471.6,260 L 480,260"
          fill="none" stroke="#2563eb" stroke-width="2.5"/>

    <!-- Y-axis labels (voltage) - linear scale from 0 to 5V -->
    <text x="50" y="25" text-anchor="end" font-size="12" fill="#333">5 V</text>
    <text x="50" y="73" text-anchor="end" font-size="12" fill="#333">4 V</text>
    <text x="50" y="121" text-anchor="end" font-size="12" fill="#333">3 V</text>
    <text x="50" y="169" text-anchor="end" font-size="12" fill="#333">2 V</text>
    <text x="50" y="217" text-anchor="end" font-size="12" fill="#333">1 V</text>
    <text x="50" y="264" text-anchor="end" font-size="12" fill="#333">0 V</text>

    <!-- X-axis labels (time in τ) -->
    <text x="60" y="278" text-anchor="middle" font-size="12" fill="#333">0</text>
    <text x="144" y="278" text-anchor="middle" font-size="12" fill="#333">τ</text>
    <text x="228" y="278" text-anchor="middle" font-size="12" fill="#333">2τ</text>
    <text x="312" y="278" text-anchor="middle" font-size="12" fill="#333">3τ</text>
    <text x="396" y="278" text-anchor="middle" font-size="12" fill="#333">4τ</text>
    <text x="480" y="278" text-anchor="middle" font-size="12" fill="#333">5τ</text>

    <!-- Axis labels -->
    <text x="270" y="295" text-anchor="middle" font-size="14" fill="#333" font-weight="bold">Time</text>
    <text x="15" y="140" text-anchor="middle" font-size="14" fill="#333" font-weight="bold" transform="rotate(-90 15 140)">Voltage</text>

    <!-- τ marker annotation (at 37% of 5V = 1.84V) -->
    <circle cx="144" cy="168" r="3" fill="#dc2626"/>
    <text x="154" y="164" font-size="11" fill="#dc2626">≈1.84 V @ τ</text>
  </svg>
</div>

- **Expected behavior:**
    - At t = 0: V = 5.0 V (initial)
    - At t = τ: V ≈ 1.84 V (37% of initial)
    - At t = 5τ: V ≈ 0.034 V (< 1%)
</div>

## Key Concepts

### Dimensional Analysis in Exponential Functions

The library provides `exp()` that works with _dimensionless_ quantities:

```cpp title="capacitor_time_curve.cpp"hl_lines="18"
--8<-- "example/capacitor_time_curve.cpp:38:61"
```

**How it works:**

1. **RC time constant**: `RR * CC` results in a _time_ quantity (Ω·F = s)
2. **Dimensionless ratio**: `tt / (RR * CC)` divides _time_ by _time_ → _dimensionless_
3. **Exponential function**: `exp()` accepts the _dimensionless_ quantity directly
4. **Dimension restoration**: Multiplying by `V0` gives the result being a _voltage_ quantity

The library automatically verifies dimensional correctness at compile time, preventing
common physics errors.

### Automatic SI Prefix Selection

Rather than manually selecting output units, the example uses `invoke_with_prefixed()`:

```cpp title="capacitor_time_curve.cpp"
--8<-- "example/capacitor_time_curve.cpp:55:57"
```

This function automatically:

- Calculates the appropriate SI prefix (V, mV, μV, nV, pV, etc.)
- Ensures values stay in readable range (typically [1.0, 1000) for engineering mode)
- Handles the output formatting in a single call

The lambda receives the quantity scaled to the optimal prefix, ready for output.

## Sample Output

```text
Capacitor time curve example for the following parameters:
Capacitance: 0.470 µF
Initial Voltage: 5.000 V
Resistance: 4.700 kΩ
Time curve:
- at 0 ms voltage is 5.000 V
- at 1 ms voltage is 3.180 V
- at 2 ms voltage is 2.022 V
- at 3 ms voltage is 1.286 V
- at 4 ms voltage is 817.638 mV    ← Transition to millivolts
- at 5 ms voltage is 519.946 mV
...
- at 15 ms voltage is 5.623 mV
- at 18 ms voltage is 1.446 mV
- at 19 ms voltage is 919.446 µV   ← Transition to microvolts
- at 20 ms voltage is 584.688 µV
...
- at 30 ms voltage is 6.323 µV
- at 34 ms voltage is 1.034 µV
- at 35 ms voltage is 657.491 nV   ← Transition to nanovolts
...
- at 45 ms voltage is 7.110 nV
- at 49 ms voltage is 1.163 nV
- at 50 ms voltage is 739.358 pV   ← Transition to picovolts
```

The output demonstrates seamless transitions between SI prefixes (V → mV → μV → nV → pV)
as the voltage decays over 9 orders of magnitude, keeping values in a readable range
throughout.

## Why This Matters

### Compile-Time Dimensional Safety

The library ensures dimensional correctness at compile time:

```cpp
// ✓ Correct: time/time is dimensionless
QuantityOf<dimensionless> auto dimensionless_arg = -tt / (RR * CC);

// ✗ Won't compile: can't take exp of a dimensional quantity
// auto wrong = exp(-tt);  // Error: exp requires dimensionless!
```

This prevents common physics/engineering errors where equations are dimensionally invalid.

### Practical Applications

This exponential decay pattern applies across many domains:

- **Electrical**: RC/RL circuits, signal decay, filter response
- **Thermal**: Newton's law of cooling, heat dissipation
- **Mechanical**: Damped oscillations, friction
- **Chemistry**: First-order reactions, radioactive decay
- **Biology**: Drug metabolism, population decay

### Output Readability

Instead of choosing fixed units (forcing users to read `0.000005 V` or `5000000 nV`),
automatic prefix selection maintains readability across 9+ orders of magnitude.

## Related Concepts

- [Dimensionless Quantities](../users_guide/framework_basics/dimensionless_quantities.md)
- [Text Output](../users_guide/framework_basics/text_output.md)
