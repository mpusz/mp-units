---
tags:
- Level - Intermediate
- System - SI
- Feature - Affine Space
- Feature - Volatile Quantities
- Feature - Text Formatting
- Domain - Embedded
- Domain - Electronics
---

# Hardware Register Mapping with Quantity Points

[Try it live on Compiler Explorer](https://godbolt.org/z/vf51enrM9){ .md-button }

## Overview

This example demonstrates how to use `quantity_point` and custom origins to model hardware
measurements with both scaling and offset. It implements a realistic embedded scenario where
a 16-bit hardware register maps the _voltage_ range [$-10\ \mathrm{V}$, $10\ \mathrm{V}$]
to raw values [$-32767$, $32767$].

As explained in [The Affine Space](../users_guide/framework_basics/the_affine_space.md),
measurements should be modeled as `quantity_point` rather than plain `quantity` when they
represent absolute values on a scale (like _temperature_, _voltage level_, or _position_).

## Key Features Demonstrated

- Creating custom quantity point origins
- Defining offset units with scaling factors
- Handling hardware register mappings in embedded systems
- Converting between hardware units and SI units

## Code Walkthrough

### Defining the Voltage Range

We start by defining the actual measurement _voltage_ range:

```cpp title="hw_voltage.cpp"
--8<-- "example/hw_voltage.cpp:45:48"
```

Next, we define the hardware representation characteristics:

```cpp title="hw_voltage.cpp"
--8<-- "example/hw_voltage.cpp:50:56"
```

### Creating a Custom Origin and Offset Unit

The key part is defining a custom origin and an offset unit that handles both scaling
and translation:

```cpp title="hw_voltage.cpp"
--8<-- "example/hw_voltage.cpp:60:66"
```

Here's what's happening:

1. **`hw_voltage_origin`**: A custom point origin representing -10 V (the hardware's zero point)
2. **`hw_voltage_unit`**: An offset unit that:
    - Scales by the conversion factor (20 V / 65534 counts)
    - Uses `hw_voltage_origin` as its origin
3. **`hw_voltage_quantity_point`**: A `quantity_point` type using this offset unit

This setup automatically handles both the scaling (converting counts to volts) and the offset
(shifting the zero point from -10 V to 0 V).

### Reading Hardware Values

Now we can simulate reading from a hardware register:

```cpp title="hw_voltage.cpp"
--8<-- "example/hw_voltage.cpp:69:77"
```

#### Working with Volatile Hardware Registers

The hardware register is declared as `volatile` to prevent compiler optimizations that could
eliminate or reorder hardware reads:


```cpp
volatile voltage_hw_t hw_voltage_value;  // Declare as volatile
```

**Why the local copy?**

Direct use of `volatile` values in expressions can cause multiple hardware reads.
The pattern used here:

1. **Makes exactly one read** from the hardware register into a non-volatile local variable
2. **Checks for error conditions** (invalid hardware state)
3. **Constructs the quantity point** only if the value is valid

This is the standard embedded practice for working with hardware registers:

```cpp
voltage_hw_t local_copy = hw_voltage_value;  // Single volatile read
if (local_copy == voltage_hw_error) return std::nullopt;
return point<hw_voltage_unit>(local_copy);   // Use non-volatile copy
```

**Key Benefits:**

- **Predictable behavior**: Exactly one hardware access per call
- **Efficiency**: No redundant register reads
- **Safety**: Error checking before constructing typed quantities
- **Library compatibility**: `quantity_point` constructors work with non-volatile values

!!! note "Volatile and mp-units"

    The **mp-units** library works seamlessly with values read from `volatile` memory.
    Simply copy the volatile value to a local variable before constructing quantities
    or quantity points. This pattern is standard in embedded systems and provides the
    best balance of safety and efficiency.

### Output

The example reads three hardware values and displays them in both hardware units and SI volts:

```cpp title="hw_voltage.cpp"
--8<-- "example/hw_voltage.cpp:86:"
```

Output:

```text
     0 hwV (-10 V)
 32767 hwV (  0 V)
 65534 hwV ( 10 V)
```

Notice how:

- Hardware value `0` corresponds to `-10 V` (the minimum of the range)
- Hardware value `32767` corresponds to `0 V` (the midpoint)
- Hardware value `65534` corresponds to `10 V` (the maximum of the range)

## Why Quantity Points Matter

This example illustrates why `quantity_point` is essential for real-world measurements:

1. **Prevents Errors**: You can't accidentally add two absolute voltage measurements
   (e.g., `5 V + 3 V` doesn't make physical sense for absolute readings)
2. **Handles Offsets**: The zero of the hardware scale differs from the zero of the
   physical scale
3. **Type Safety**: The compiler ensures correct conversions between coordinate systems
4. **Embedded-Friendly**: Minimal runtime overhead while maintaining safety

## Practical Applications

This pattern is useful for:

- ADC readings with offset calibration
- Temperature sensors (especially with non-zero reference points)
- Position encoders with custom datum points
- Pressure sensors with atmospheric offset
- Any measurement device with non-standard zero reference

## Related Concepts

- [The Affine Space](../users_guide/framework_basics/the_affine_space.md) - Detailed explanation
  of quantity points
