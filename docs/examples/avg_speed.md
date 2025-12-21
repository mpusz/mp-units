---
tags:
- Level - Beginner
- System - CGS
- System - International System
- System - ISQ
- Feature - Value Truncation
- Feature - Representation Types
---

# Average Speed: Representation Types and Conversions

[Try it live on Compiler Explorer](https://godbolt.org/z/bPT9av1oM){ .md-button }

## Overview

This example builds on [hello_units](hello_units.md) by exploring different API design
choices when working with quantities. It demonstrates how fixed-unit functions compare to
generic quantity functions, and shows the impact of unit conversions and representation
types on precision and performance.

## Key Features Demonstrated

- Fixed-unit vs. generic quantity function interfaces
- Impact of representation types (`int` vs. `double`)
- Value-preserving and value-truncating conversions
- Working with multiple unit systems (SI, CGS, International)
- Precision loss during unit conversions

## Code Walkthrough

### Including Headers and Namespaces

First, we either import a module or include all the necessary header files and import all
the identifiers from the `mp_units` namespace:

```cpp title="avg_speed.cpp" linenums="1"
--8<-- "example/avg_speed.cpp:28:45"
```

### Function Definitions

Next, we define two functions calculating average speed based on quantities of fixed units
and integral and floating-point representation types, respectively, and a third function
that we introduced in the [previous example](hello_units.md):

```cpp title="avg_speed.cpp" linenums="19"
--8<-- "example/avg_speed.cpp:47:61"
```

We also added a simple utility to print our results:

```cpp title="avg_speed.cpp" linenums="34"
--8<-- "example/avg_speed.cpp:63:69"
```

### Comparing Function Behavior

Now, let's analyze how those three utility functions behave with different sets of arguments.

#### SI Units with Integral Representation

First, we are going to use quantities of SI units and integral representation:

```cpp title="avg_speed.cpp" linenums="41"
--8<-- "example/avg_speed.cpp:71:85"
```

The above provides the following output:

```text
SI units with 'int' as representation
Average speed of a car that makes 220 km in 2 h is 108 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
```

Please note that in the first two cases, we must convert length from `km` to `m` and
time from `h` to `s`. The converted values are used to calculate speed in `m/s` which
is then again converted to the one in `km/h`. Those conversions not only impact the
application's runtime performance but may also affect the precision of the final result.
Such truncation can be easily observed in the first case where we deal with integral
representation types (the resulting speed is $108\ \mathrm{km/h}$).

The second scenario is really similar to the previous one, but this time, function arguments
have floating-point representation types:

```cpp title="avg_speed.cpp" linenums="56"
--8<-- "example/avg_speed.cpp:87:98"
```

Conversion from floating-point to integral representation types is
[considered value-truncating](../users_guide/framework_basics/value_conversions.md#value-truncating-conversions)
and that is why now, in the first case, we need an explicit call to `value_cast<int>`.

In the text output, we can observe that, again, the resulting value gets truncated during conversions
in the first cast:

```text
SI units with 'double' as representation
Average speed of a car that makes 220 km in 2 h is 108 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
```

#### International Mile Units

Next, let's do the same for integral and floating-point representations, but this time
using international mile:

```cpp title="avg_speed.cpp" linenums="68"
--8<-- "example/avg_speed.cpp:100:131"
```

One important difference here is the fact that as it is not possible to make a lossless
conversion of miles to meters on a quantity using an integral representation type, so this
time, we need a `value_cast<m, int>` to force it.

If we check the text output of the above, we will see the following:

```text
International mile with 'int' as representation
Average speed of a car that makes 140 mi in 2 h is 111 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
Average speed of a car that makes 140 mi in 2 h is 112 km/h.

International mile with 'double' as representation
Average speed of a car that makes 140 mi in 2 h is 111 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
```

Please note how the first and third results get truncated using integral representation types.

#### CGS Units

In the end, we repeat the scenario for CGS units:

```cpp title="avg_speed.cpp" linenums="100"
--8<-- "example/avg_speed.cpp:133:164"
```

Again, we observe `value_cast` being used in the same places and consistent truncation errors
in the text output:

```text
CGS units with 'int' as representation
Average speed of a car that makes 22000000 cm in 7200 s is 108 km/h.
Average speed of a car that makes 22000000 cm in 7200 s is 110 km/h.
Average speed of a car that makes 22000000 cm in 7200 s is 109 km/h.

CGS units with 'double' as representation
Average speed of a car that makes 2.2e+07 cm in 7200 s is 108 km/h.
Average speed of a car that makes 2.2e+07 cm in 7200 s is 110 km/h.
Average speed of a car that makes 2.2e+07 cm in 7200 s is 110 km/h.
```

The example file ends with a simple `main()` function:

```cpp title="avg_speed.cpp" linenums="132"
--8<-- "example/avg_speed.cpp:166:"
```

## Key Takeaways

### API Design Trade-offs

1. **Fixed-Unit Functions** (`fixed_int_si_avg_speed`, `fixed_double_si_avg_speed`):

    - ✅ Simple and explicit about units
    - ❌ Require runtime unit conversions at call site
    - ❌ May truncate the quantity value
    - ❌ Less flexible for users

2. **Generic Quantity Functions** (`avg_speed`):

    - ✅ Accept any compatible units with no runtime overhead
    - ✅ More convenient for users
    - ✅ Better composability
    - ⚠️ Require C++ templates (e.g., often provided in header files)

### Precision Considerations

- **Integral representations**: Value-truncating conversions can lose precision
- **Floating-point representations**: Better for intermediate conversions
- **Multiple conversions**: Each conversion step can accumulate errors
- **Generic functions**: Minimize conversions by working in the user's units

## Related Concepts

- [Value Conversions](../users_guide/framework_basics/value_conversions.md) - Understanding
  value-preserving vs. value-truncating conversions
- [Generic Interfaces](../users_guide/framework_basics/generic_interfaces.md) - Designing
  flexible APIs
