# CODATA Fundamental Physical Constants

The [CODATA](https://physics.nist.gov/cuu/Constants/) recommended values of the
fundamental physical constants, expressed in SI units.

```cpp
#include <mp-units/systems/codata.h>

using namespace mp_units;

quantity mu = measurement_of(codata::magnetic_constant).in(si::henry / si::metre);
std::cout << mu << "\n";  // 1.25664e-06 ± 2.01062e-16 H/m
```

## Why a separate system

The SI fixes exactly seven constants, and those live in the
[SI system](si.md#si-defining-constants) where they belong. Everything CODATA publishes
beyond them is **measured**: each value carries a relative standard uncertainty and shifts
with every adjustment.

Keeping them here rather than in `si` is a deliberate cost decision.
`<mp-units/systems/si.h>` is the header most translation units reach for, and most of them
never name a CODATA constant. Making everyone pay for the full table so that a minority
need not write one extra `#include` is the wrong trade. Include this header when you want
these constants.

## Adjustments

Each CODATA adjustment gets its own namespace, because a measured constant has no single
correct value:

```cpp
quantity now = measurement_of(codata::magnetic_constant);                  // current
quantity pinned = measurement_of(codata::codata2018::magnetic_constant);   // a specific one
```

The most recent adjustment is `inline`, so the unqualified name always refers to it while
older ones stay reachable by spelling them out. Moving that `inline` to a newer adjustment
is a deliberate, documented change of the default rather than something that happens
silently.

Constants that are exact under a given adjustment carry no uncertainty at all, which makes
the exact-versus-measured distinction visible to the type system:

```cpp
static_assert(MeasuredConstant<decltype(codata::codata2022::magnetic_constant)>);
static_assert(!MeasuredConstant<decltype(codata::codata2014::magnetic_constant)>);
```

The magnetic constant μ₀ (IEC 80000-6, item 6-26.1) shows why. Before 2019 the ampere was
defined *through* μ₀, which made it exactly $4\pi \times 10^{-7}$ H/m. The 2019
redefinition tied the ampere to the elementary charge instead, so μ₀ became measured, and
its value departs from $4\pi \times 10^{-7}$ in the tenth digit.

!!! warning "Not interchangeable with the `hep` constants"

    The [HEP system](hep.md) provides `codata2014`/`codata2018`/`codata2022` namespaces of
    its own, but those constants are **not** the same entities. HEP declares its own system
    of quantities, with energy as a base quantity and no mass dimension at all, so its
    constants are unrelated types that no conversion can reach.

    Their published uncertainties can differ as well. Before 2019 the elementary charge was
    itself measured, so a mass expressed in kg and the same mass expressed in MeV/c² are
    separate CODATA table entries with different $u_r$. Deriving one from the other would
    combine correlated uncertainties in quadrature and overstate them.

## Working with the uncertainties

These constants are ordinary units, so they cost nothing until a conversion actually needs
their value, and they cancel symbolically when they appear on both sides. To obtain a
quantity that carries the uncertainty, use `measurement_of`, described in
[Working with Measurement Uncertainty](../../how_to_guides/advanced_usage/working_with_measurement_uncertainty.md).
