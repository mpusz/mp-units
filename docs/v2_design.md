# V2 Design Discussion

**<big>Please help determining the based design for the library!</big>**

---

## Quick Intro

Below is a short intro of what types are being currently generated in specific use cases.

### Named quantity/dimension and unit

```cpp
auto p = 5 * si::power[W];

static_assert(std::is_same_v<decltype(p), quantity<reference<isq::power_dim, si::watt>{}, int>>);
```

### Named quantity/dimension and derived (unnamed) unit

```cpp
auto s = 5 * si::speed[m / s];

static_assert(std::is_same_v<decltype(s),
              quantity<reference<isq::speed_dim, derived_unit<si::metre, per<si::second>>>{}, int>>);
```

### Derived (unnamed) quantity/dimension and derived (unnamed) unit

```cpp
auto q = 10 * si::length[m] / (2 * si::time[s]);

static_assert(std::is_same_v<decltype(q),
              quantity<reference<derived_dimension<isq::length_dim, per<isq::time_dim>>,
                       derived_unit<si::metre, per<si::second>>>{}, int>>);
```

### Base quantity as a result of dimensional transformation

```cpp
auto d = 5 * si::speed[m / s] * (5 * si::time[s]);

static_assert(std::is_same_v<decltype(d), quantity<reference<isq::length_dim, si::metre>{}, int>>);
```

### Dimensionless

```cpp
auto q = 20 * si::speed[m / s] / (10 * si::length[m]) * (5 * si::time[s]);

static_assert(std::is_same_v<decltype(q), quantity<reference<one_dim, one>{}, int>>);
```

---

### Quantity Comparison

```cpp
// Same dimension type & different unit
static_assert(1000 * si::length[m] == 1 * si::length[km]);

// Named and derived dimensions (same units)
static_assert(10 * si::length[m] / (2 * si::time[s]) == 5 * si::speed[m / s]);

// Same named dimension & different but equivalent unit
static_assert(10 * si::frequency[1 / s] == 10 * si::frequency[Hz]);

// Named and derived dimensions (different but equivalent units)
static_assert(10 / (2 * si::time[s]) == 5 * si::frequency[Hz]);

// Different named dimensions
// static_assert(5 * si::activity[Bq] == 5 * si::frequency[Hz]); // ambiguous
```

---

## Quantity Arithmetics

```cpp
// Named and derived dimensions (same units)
auto q1 = 10 * si::length[m] / (2 * si::time[s]) + 5 * si::speed[m / s];
static_assert(is_same_v<decltype(q1),
              quantity<reference<isq::speed_dim, derived_unit<si::metre, per<si::second>>>{}, int>>)

// Named and derived dimensions (different units)
auto q2 = 10 / (2 * si::time[s]) + 5 * si::frequency[Hz];
static_assert(is_same_v<decltype(q1), quantity<reference<isq::frequency_dim, si::hertz>{}, int>>)

// Different named dimensions
// auto q3 = 5 * si::activity[Bq] + 5 * si::frequency[Hz];                          // ambiguous
// auto q4 = 5 * si::activity[Bq] + 10 / (2 * si::time[s]) + 5 * si::frequency[Hz]; // ambiguous
```

---

## Dimensions

```cpp
// If something compares equal it is always equivalent and convertible
static_assert(isq::speed_dim == isq::speed_dim);
static_assert(equivalent(isq::speed_dim, isq::speed_dim));
static_assert(convertible(isq::speed_dim, isq::speed_dim));

// Derived unnamed vs named
// Should the `equivalent` be consistent with the `op=`?
static_assert(isq::length_dim / isq::time_dim != isq::speed_dim);              // ???
static_assert(equivalent(isq::length_dim / isq::time_dim, isq::speed_dim));
static_assert(convertible(isq::length_dim / isq::time_dim, isq::speed_dim));

static_assert(isq::force_dim * isq::length_dim != isq::energy_dim);            // ???
static_assert(isq::force_dim * isq::length_dim != isq::torque_dim);            // ???
static_assert(equivalent(isq::force_dim * isq::length_dim, isq::energy_dim));
static_assert(equivalent(isq::force_dim * isq::length_dim, isq::torque_dim));
static_assert(convertible(isq::force_dim * isq::length_dim, isq::energy_dim));
static_assert(convertible(isq::force_dim * isq::length_dim, isq::torque_dim));

// Different (but dimensionally equivalent) dimensions
static_assert(isq::energy_dim != isq::torque_dim);
static_assert(equivalent(isq::energy_dim, isq::torque_dim));                   // ???
static_assert(!convertible(isq::energy_dim, isq::torque_dim));

// both sides return the same base dimension
// (at least for now)
static_assert(isq::speed_dim * isq::time_dim == isq::length_dim);              // ???
static_assert(isq::length_dim / isq::speed_dim == isq::time_dim);
// NOTE: "magnetomotive force" derived quantity is measured in ampere (which is a base unit)
// Should the result of the above be just `isq::length_dim` or
// `derived_dimension<isq::length_dim>` that can me converted to isq::length_dim?
// In the latter case we would be able to make the above `false` and be able to
// make "magnetomotive force" a dimension distinguishable from "electric current"
```

---

## Dimensions: Dimensionless

Should dimensionless quantity, `one_dim`, and `one` have special comparison rules?
If so how to implement that? What should be the rules for a derived_dimension simplification?

```cpp
static_assert(equivalent(isq::power_dim / isq::power_dim, isq::efficiency_dim));
static_assert(isq::power_dim / isq::power_dim != isq::efficiency_dim);             // ???
static_assert(one_dim != isq::efficiency_dim);                                     // ???

static_assert(equivalent(isq::efficiency_dim, isq::strain_dim));                   // ???
static_assert(isq::efficiency_dim != isq::strain_dim);                             // ???

static_assert(isq::stress_dim / isq::stress_dim != isq::strain_dim);               // ???
static_assert(isq::stress_dim / isq::stress_dim != isq::efficiency_dim);           // ???
static_assert(equivalent(isq::stress_dim / isq::stress_dim, isq::strain_dim));
static_assert(equivalent(isq::stress_dim / isq::stress_dim, isq::efficiency_dim)); // ???
```

---

## Units

```cpp
// Different magnitudes
static_assert(si::metre != si::kilometre);
static_assert(!equivalent(si::metre, si::kilometre));
static_assert(convertible(si::metre, si::kilometre));

// Unnamed derived unit vs named unit
static_assert(si::newton * si::metre != si::joule);
static_assert(equivalent(si::newton * si::metre, si::joule));
static_assert(convertible(si::newton * si::metre, si::joule));
```

---

## Conversions

```cpp
// Implicit conversions allowed between quantities of `convertible` references
auto q1 = 120 * si::length[km] / (2 * si::time[h]);
quantity<si::speed[km / h]> s = q1;

// Explicit casts allow changing all or only a part of the type
auto q2 = quantity_cast<isq::speed_dim>(120 * si::length[km] / (2 * si::time[h]));
auto q3 = quantity_cast<m / s>(120 * si::length[km] / (2 * si::time[h]));
auto q4 = quantity_cast<si::speed[m / s]>(120 * si::length[km] / (2 * si::time[h]));
auto q5 = quantity_cast<double>(120 * si::length[km] / (2 * si::time[h]));
auto q6 = quantity_cast<quantity<si::speed[m / s], double>>(120 * si::length[km] / (2 * si::time[h]));
```
