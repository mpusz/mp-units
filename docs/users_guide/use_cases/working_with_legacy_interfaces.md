# Working with Legacy interfaces

In case we are working with a legacy/unsafe interface, we may need to extract the
[numerical value of a quantity](../../appendix/glossary.md#numerical-value) and pass it to
some third-party legacy unsafe interfaces.

In such situations we can use `.numerical_value_in(Unit)` member function:

```cpp
void legacy_check_speed_limit(int speed_in_km_per_h);
```

```cpp
legacy_check_speed_limit((180 * km / (2 * h)).numerical_value_in(km / h));
```

Such a getter will explicitly enforce the usage of a correct unit required by the underlying
interface, which reduces a significant number of safety-related issues.

The above code will not compile in case value truncation may happen. To solve the issue, we
need to either use a value-preserving representation type or force the truncating conversion
with `.force_numerical_value_in(Unit)`:

```cpp
legacy_check_speed_limit((140 * mi / (2 * h)).force_numerical_value_in(km / h));
```

The getters mentioned above always return by value as a quantity value conversion may be required
to adjust it to the target unit. In case a user needs a reference to the underlying storage
`.numerical_value_ref_in(Unit)` should be used:

```cpp
void legacy_set_speed_limit(int* speed_in_km_per_h) { *speed_in_km_per_h = 100; }
```

```cpp
quantity<km / h, int> speed_limit;
legacy_set_speed_limit(&speed_limit.numerical_value_ref_in(km / h));
```

This member function again requires a target unit to enforce safety. This overload does not
participate in overload resolution if the provided unit has a different scaling factor than the
current one.
