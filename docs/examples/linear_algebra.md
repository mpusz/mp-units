---
tags:
- Level - Intermediate
- System - SI
- System - ISQ
- Feature - Custom Representation Types
- Feature - Vector Quantities
- Feature - Unit Conversions
- Domain - Robotics
- Domain - Physics
---

# Using a Linear Algebra Library as the Representation

## Overview

This example shows how a linear algebra library can be used **directly** as the
[representation type](../users_guide/framework_basics/representation_types.md) of a vector
_quantity_. The exact same **mp-units** code is compiled against four different vector
types: three mainstream third-party libraries
([Eigen](https://eigen.tuxfamily.org), [GLM](https://github.com/g-truc/glm), and
[Blaze](https://bitbucket.org/blaze-lib/blaze)) and the library's own built-in
[`cartesian_vector`](../users_guide/framework_basics/character_of_a_quantity.md); only the
small shim that names the vector type and constructs it differs.

The key idea is that a 3D vector — `Eigen::Vector3d`, `glm::dvec3`,
`blaze::StaticVector<double, 3>`, or `cartesian_vector<double>` — becomes the
representation, so the _quantity_ carries **both** a unit and a direction:

```cpp
quantity velocity = isq::velocity(la::make_vec3(30, 40, 0) * km / h);
```

## Enabling the Integration

For each third-party library, **mp-units** ships an opt-in plugin that wires the library's
vector type into the representation
[customization points](../users_guide/framework_basics/representation_types.md)
automatically, with no adapter code of your own. It is available both as a header and a named
module:

```cpp
#include <mp-units/integrations/eigen.h>   // or <mp-units/integrations/glm.h>, or .../blaze.h>
// in module mode instead: import mp_units.integrations.eigen;
```

Each header is guarded with `__has_include`, so it is a harmless no-op when the corresponding
library is not available. The built-in `cartesian_vector` needs no plugin at all — it ships
with the library and is a representation type out of the box. This example is compiled against
all four backends, in both header and module modes.

## Key Concepts

### Unit conversion of a whole vector

Converting the unit scales every component at once:

```cpp
print("velocity [m/s]   ", velocity.in(m / s));   // [8.33333, 11.1111, 0] m/s
```

### Vector quantity × scalar quantity

Multiplying a _velocity_ vector by a _duration_ produces a _displacement_ vector, with the
units combined automatically:

```cpp
quantity displacement = velocity * (2. * h);   // [60, 80, 0] km
```

This is where expression-template libraries (Eigen, Blaze) need care: their `operator*`
returns a lazy proxy that holds references to its operands. **mp-units** materializes such
a proxy into the concrete vector type before storing it, so no dangling references are possible.

### Magnitude as a scalar quantity

The library's `magnitude()` customization point recognizes the `norm()` (or `length()`) that
linear algebra libraries provide. A vector _quantity_ supports `magnitude()` directly, returning
the Euclidean magnitude as a scalar _quantity_ in the same unit:

```cpp
quantity speed = magnitude(velocity);   // 50 km/h
```

## What About Other Libraries?

The same recipe applies to any library whose vector type is _weakly regular_ (copyable and
**`bool`-returning** equality-comparable) and provides a Euclidean norm. Note that
[Armadillo](https://arma.sourceforge.net) is **not** supported: its `operator==` returns an
element-wise mask rather than a `bool`, so its types are not `std::equality_comparable` and
cannot satisfy the representation requirements.

## See Also

- [Representation Types](../users_guide/framework_basics/representation_types.md) - the customization
  points used here
- [Using Custom Representation Types](../how_to_guides/integration/using_custom_representation_types.md) -
  how to integrate your own type
- [Character of a Quantity](../users_guide/framework_basics/character_of_a_quantity.md) - scalars vs
  vectors vs tensors
