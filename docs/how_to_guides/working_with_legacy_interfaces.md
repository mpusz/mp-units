# Working with Legacy Interfaces

When working with legacy or unsafe interfaces, you may need to extract the
[numerical value of a quantity](../reference/glossary.md#numerical-value) and pass it to
third-party APIs that don't use type-safe units.

**mp-units** provides several member functions to safely bridge this gap while maintaining
as much type safety as possible.

## Extracting Values with Unit Enforcement

The `.numerical_value_in(Unit)` member function extracts a numerical value while explicitly
specifying the expected unit:

```cpp
void legacy_check_speed_limit(int speed_in_km_per_h);
```

```cpp
quantity speed = 180 * km / (2 * h);  // 90 km/h
legacy_check_speed_limit(speed.numerical_value_in(km / h));  // Passes 90
```

This approach explicitly enforces the correct unit required by the underlying interface,
significantly reducing safety-related issues. The code won't compile if you accidentally
attempt to extract the value in the wrong unit.

## Handling Value Truncation

The `.numerical_value_in()` function won't compile if value truncation would occur (e.g.,
converting a floating-point value to an integer). You have two options to resolve this:

1. **Use a value-preserving representation type** (e.g., `double` instead of `int`)
2. **Explicitly allow truncation** with `.force_numerical_value_in(Unit)`:

```cpp
quantity speed = 140 * mi / (2 * h);  // 70 mph = 112.654 km/h
legacy_check_speed_limit(speed.force_numerical_value_in(km / h));  // Truncates to 112
```

Use `.force_numerical_value_in()` only when you're certain that truncation is acceptable
for your use case.

## Accessing the Underlying Storage

The getters above always return by value because unit conversion may be required. When you
need direct access to the underlying storage (without conversion), use
`.numerical_value_ref_in(Unit)`:

```cpp
void legacy_set_speed_limit(int* speed_in_km_per_h) { *speed_in_km_per_h = 100; }
```

```cpp
quantity<km / h, int> speed_limit;
legacy_set_speed_limit(&speed_limit.numerical_value_ref_in(km / h));
```

This member function requires specifying the target unit to enforce safety. It only
participates in overload resolution if the provided unit has the same scaling factor as
the quantity's current unit (i.e., no conversion is needed).

## Summary

**mp-units** provides three functions for interfacing with legacy code:

| Function                          | Purpose                                   | Conversion | Truncation | Returns      |
|-----------------------------------|-------------------------------------------|------------|------------|--------------|
| `.numerical_value_in(Unit)`       | Safe value extraction                     | Yes        | Prevented  | By value     |
| `.force_numerical_value_in(Unit)` | Value extraction with explicit truncation | Yes        | Allowed    | By value     |
| `.numerical_value_ref_in(Unit)`   | Direct storage access                     | No         | N/A        | By reference |

These functions maintain type safety at the boundary between type-safe and legacy code,
helping prevent unit-related errors while still allowing interoperability with existing
APIs.
