# Working with Legacy Interfaces

This guide shows you how to safely interface **mp-units** with legacy code that doesn't use
type-safe units. You'll learn the different functions for extracting numerical values, when
to use each one, and how to maintain as much type safety as possible at the boundary between
type-safe and legacy code.

For background on numerical values and value conversions, see
[Value Conversions](../../users_guide/framework_basics/value_conversions.md) in the User's Guide.

## The Challenge

When working with legacy or unsafe interfaces, you need to extract the
[numerical value of a quantity](../../reference/glossary.md#numerical-value) and pass it to
third-party APIs that don't understand type-safe units:

```cpp
// Legacy function expects an integer representing speed in km/h
void legacy_check_speed_limit(int speed_in_km_per_h);

// How do we safely pass our quantity to this function?
quantity speed = 180 * km / (2 * h);  // 90 km/h
```

Simply accessing the raw value without specifying the unit would be dangerous—you might
accidentally pass a value in the wrong unit. **mp-units** provides several member functions
to safely bridge this gap while enforcing correctness.

## Safe Value Extraction

### The `.numerical_value_in(Unit)` Function

The primary way to extract a numerical value is using `.numerical_value_in(Unit)`. This
function requires you to explicitly specify the expected unit:

```cpp
void legacy_check_speed_limit(double speed_in_km_per_h);

quantity speed = 180 * km / (2 * h);  // 90 km/h
legacy_check_speed_limit(speed.numerical_value_in(km / h));  // ✅ Passes 90
```

**Key benefits:**

- Explicitly enforces the correct unit required by the interface
- Automatically converts to the specified unit if needed
- Won't compile in case of incompatible quantity kinds or value truncation
- Significantly reduces safety-related issues
- Makes code self-documenting about what unit the legacy function expects

**Example of compile-time protection:**

```cpp
// ❌ Compile error: dimensionally incompatible
legacy_check_speed_limit(speed.numerical_value_in(m));

// ❌ Compile error: wrong dimensions
quantity distance = 100 * m;
legacy_check_speed_limit(distance.numerical_value_in(km / h));
```

### Handling Value Truncation

The `.numerical_value_in()` function prevents value truncation to ensure safety. If the
quantity's representation type is not value preserving and the unit conversion may
truncate the value, you'll get a compile error:

```cpp
// ❌ Compile error: value truncation
quantity speed = 30 * m / s;
legacy_check_speed_limit(speed.numerical_value_in(km / h));
```

!!! warning "Assignment/Construction Truncation Not Detected"

    The library cannot detect or prevent truncation that occurs when assigning or passing
    the returned value to an incompatible type. For example:

    ```cpp
    void legacy_function(int speed_in_km_per_h);  // Expects int

    quantity<km / h, double> speed = 112.654 * km / h;
    // ⚠️ Compiles but truncates: .numerical_value_in() returns double,
    // which gets implicitly converted to int when passed to the function
    legacy_function(speed.numerical_value_in(km / h));  // Passes 112
    ```

    The library only protects against truncation within the quantity's own representation
    type conversions, not what happens to the value after it's extracted.

You have two options to resolve this:

**Option 1: Use a value-preserving representation type**

```cpp
// ✅ Automatic conversion (floating-point types are value preserving)
quantity speed = 33. * m / s;   // 118.8 km/h
legacy_check_speed_limit(speed.numerical_value_in(km / h));
```

**Option 2: Explicitly force truncation**

When you're certain that truncation is acceptable, use `.force_numerical_value_in(Unit)`:

```cpp
quantity speed = 33 * m / s;   // 118.8 km/h
legacy_check_speed_limit(speed.force_numerical_value_in(km / h));  // ✅ Truncates to 118
```

!!! warning "Use `.force_numerical_value_in()` Sparingly"

    Only use this function when you've verified that truncation is acceptable for your
    use case. Document why truncation is safe to help future maintainers understand
    your reasoning.


## Direct Storage Access

### The `.numerical_value_ref_in(Unit)` Function

The functions above always return by value because unit conversion and value scaling
may be required. When you need direct access to the underlying storage—for example,
when a legacy function requires a pointer—use `.numerical_value_ref_in(Unit)`:

```cpp
void legacy_set_speed_limit(int* speed_in_km_per_h)
{
  *speed_in_km_per_h = 100;
}

quantity<km / h, int> speed_limit;
legacy_set_speed_limit(&speed_limit.numerical_value_ref_in(km / h));  // ✅ Direct access
```

**Important constraints:**

- Still requires specifying the target unit for safety
- Only works when no scaling is needed
- The provided unit must have the same scaling factor as the quantity's current unit
- Returns a reference to the internal storage

**When it won't compile:**

```cpp
quantity<m / s, int> speed;
// ❌ Compile error: requires conversion from m/s to km/h
int& value = speed.numerical_value_ref_in(km / h);
```

## Practical Examples

### Example 1: Legacy API with Multiple Parameters

```cpp
// Legacy function expecting separate value and unit code
void legacy_log_measurement(double value, int unit_code);

constexpr int UNIT_METERS = 1;
constexpr int UNIT_KILOMETERS = 2;

quantity distance = 1500. * m;

// Clear and explicit about units
legacy_log_measurement(distance.numerical_value_in(m), UNIT_METERS);
legacy_log_measurement(distance.numerical_value_in(km), UNIT_KILOMETERS);
```

### Example 2: Filling Legacy Data Structures

```cpp
struct LegacyMeasurement {
  double value_in_meters;
  double value_in_feet;
};

quantity distance = 10. * m;

LegacyMeasurement legacy_data{
  .value_in_meters = distance.numerical_value_in(m),
  .value_in_feet = distance.numerical_value_in(ft)
};
```

### Example 3: Working with Output Parameters

```cpp
void legacy_get_dimensions(int* width_mm, int* height_mm);

quantity<mm, int> width, height;
legacy_get_dimensions(
  &width.numerical_value_ref_in(mm),
  &height.numerical_value_ref_in(mm)
);
```

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
