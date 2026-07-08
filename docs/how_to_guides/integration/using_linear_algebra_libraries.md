# Using a Linear Algebra Library

This guide shows you how to use a linear algebra library's vector and matrix types
(e.g. [Eigen](https://eigen.tuxfamily.org), [GLM](https://github.com/g-truc/glm),
[Blaze](https://bitbucket.org/blaze-lib/blaze)) directly as **mp-units** vector and tensor
representation types. For most mainstream libraries you do not need to write any adapter code
at all. **mp-units** ships opt-in integration plugins that wire up the required customization
points for you.

For the design rationale behind representation types and the full customization-point
reference, see
[Representation Types](../../users_guide/framework_basics/representation_types.md) in the
User's Guide. To author a representation type of your own from scratch, see
[Using Custom Representation Types](using_custom_representation_types.md).

## Shipped Integration Plugins

**mp-units** ships opt-in integration plugins, each available as a header (header mode) and
as a named module (module mode). The currently available plugins adapt linear algebra libraries,
so their vector and matrix types can be used **directly** as vector/tensor representations:

<!-- markdownlint-disable MD013 -->
| Header                            | Module                        | CMake target (module mode)     | Library                                        |
|-----------------------------------|-------------------------------|--------------------------------|------------------------------------------------|
| `<mp-units/integrations/eigen.h>` | `mp_units.integrations.eigen` | `mp-units::integrations-eigen` | [Eigen](https://eigen.tuxfamily.org)           |
| `<mp-units/integrations/glm.h>`   | `mp_units.integrations.glm`   | `mp-units::integrations-glm`   | [GLM](https://github.com/g-truc/glm)           |
| `<mp-units/integrations/blaze.h>` | `mp_units.integrations.blaze` | `mp-units::integrations-blaze` | [Blaze](https://bitbucket.org/blaze-lib/blaze) |
<!-- markdownlint-enable MD013 -->

Include the plugin header next to the library's own headers and the library types become
usable as representations immediately:

```cpp
#include <Eigen/Core>
#include <mp-units/integrations/eigen.h>  // header mode; or: import mp_units.integrations.eigen;
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

quantity v = Eigen::Vector3d{30, 40, 0} * isq::velocity[km / h];
quantity speed = magnitude(v);  // 50 km/h (a vector quantity supports magnitude() directly)
```

Each header is guarded with `__has_include`, so it is a harmless no-op when its library is
not available, and it is always safe to include. The matching module is built only when C++
modules and the library are both available. See the
[linear algebra example](../../examples/linear_algebra.md) for the same scenario compiled
against all three libraries, in both header and module mode, alongside the built-in
`cartesian_vector`.

### Enabling in CMake

In **header mode** the integration headers ship with the library itself, so your existing
`mp-units::mp-units` link already makes them available. You only need to give the compiler
access to the linear algebra library's own headers. The guarded plugin header stays a no-op
until they are visible:

```cmake
find_package(mp-units REQUIRED)
find_package(Eigen3 REQUIRED)
target_link_libraries(my_app PRIVATE mp-units::mp-units Eigen3::Eigen)
```

In **module mode** each plugin is a separate, optionally-built package. Discover it with
`find_package` (which also pulls in its `mp-units` and third-party dependencies) and link
its per-library target. That target deliberately links only `mp-units::core` and the linear
algebra library, **not** the systems, so keep your usual `mp-units::mp-units` (or
`mp-units::systems`) link for the `si`/`isq` definitions your code uses:

```cmake
find_package(mp-units-integrations-eigen REQUIRED)  # also finds its mp-units and Eigen3 deps
target_link_libraries(my_app PRIVATE mp-units::mp-units mp-units::integrations-eigen)
```

The header-only `mp-units::integrations` target, the per-library targets
(`mp-units::integrations-eigen`, `-glm`, `-blaze`), and their separate packaging are
documented in [Project Structure](../../getting_started/project_structure.md).

!!! note "Expression templates"

    Eigen and Blaze evaluate lazily: their arithmetic operators return proxy expression types
    that reference their operands. The plugin headers map each such proxy to its evaluated
    concrete type (via
    [`representation_canonical_type`](../../users_guide/framework_basics/representation_types.md#representation_canonical_type))
    so a `quantity` never stores a dangling proxy.

!!! warning "V2 limitation: vector-operation result types"

    A vector quantity supports `magnitude()` directly, but the result drops the precise
    _quantity spec_ down to the unit's kind. V2 cannot yet express a dedicated
    scalar-magnitude quantity spec. As a result the magnitude keeps `vector` character
    whenever the unit is tied to a vector quantity spec (e.g. `N` is `kind_of<isq::force>`),
    and only collapses to scalar character for units built from scalar base units (e.g.
    `km/h`). For the same reason quantity-level `scalar_product()` / `vector_product()`
    (dot/cross) are **not** provided in V2. They must return a _different_ quantity kind,
    which V2 references cannot name. Compute those on the raw representation and re-attach the
    reference yourself.

## Adapting a Library That Has No Plugin

Any _weakly-regular_ vector type (copyable, with `bool`-returning equality) that exposes a
Euclidean norm can serve as a representation. The bundled plugins wire up exactly the
customization points described here, and they are the template for adapting any other library.
Five things vary between libraries:

- **Underlying type.** Eigen and GLM expose a `value_type` member, so
  [`representation_underlying_type`](../../users_guide/framework_basics/representation_types.md#representation_underlying_type)
  detects it automatically. Blaze names it `ElementType` instead, so its plugin specializes
  [`representation_underlying_type`](../../users_guide/framework_basics/representation_types.md#representation_underlying_type)
  explicitly.
- **Magnitude.** Eigen and Blaze provide `norm()`, which the
  [`magnitude()`](../../users_guide/framework_basics/representation_types.md#character-determination)
  CPO uses directly. GLM spells it `length()`, so its plugin adds a one-line
  [`magnitude()`](../../users_guide/framework_basics/representation_types.md#character-determination)
  overload (found by ADL) that forwards to it.
- **Order.** A library whose structural shape is ambiguous declares its order explicitly.
  An Eigen column vector is an `N×1` matrix, so it exposes *both* single-index `operator[]`
  and two-index `operator()`, making its order ambiguous, so
  [`tensor_order`](../../users_guide/framework_basics/representation_types.md#tensor_order)
  has no default for it and its plugin specializes it from the compile-time shape. GLM and
  Blaze, whose vectors and matrices are structurally distinct types, need no such override.
- **Field.** No override is needed:
  [`numeric_field`](../../users_guide/framework_basics/representation_types.md#numeric_field)
  reads the field off a scalar element, so a real matrix of `double` is real even though Eigen
  and Blaze also expose `real()`/`imag()` on it, and a complex matrix is complex. The one
  requirement is that a complex-element container also expose `real()`/`imag()` on its surface
  (Eigen and Blaze do); a complex container missing that API is left unclassified rather than
  guessed, so it is not a representation until it provides the API or specializes
  `numeric_field`.
- **Materializing expression templates.** Eigen and Blaze return lazy proxy types from
  their arithmetic operators. A proxy holds references to its operands, so storing one
  inside a `quantity` would leave dangling references once those operands expire. Their
  plugins specialize
  [`representation_canonical_type`](../../users_guide/framework_basics/representation_types.md#representation_canonical_type)
  to map each proxy to its evaluated concrete type (Eigen's `PlainObject`, Blaze's
  `ResultType`), so a `quantity` always stores a materialized value. GLM evaluates
  eagerly, so it needs no such specialization.

    ```cpp
    // materialize an expression-template proxy before a quantity stores it
    template<typename T>
      requires /* T is one of your library's lazy expression types */
    struct mp_units::representation_canonical_type<T> {
      using type = std::remove_cvref_t<typename T::evaluated_type>;
    };
    ```

!!! warning "Element-wise `operator==` is disqualifying"

    [Armadillo](https://arma.sourceforge.net) is the notable type that does **not** qualify: its
    `operator==` returns an element-wise mask rather than a `bool`, so it is not
    `std::equality_comparable` and cannot satisfy the representation requirements.

## See Also

<!-- markdownlint-disable MD013 -->
**User's Guide:**

- [Representation Types](../../users_guide/framework_basics/representation_types.md) - Complete design and requirements reference, including the customization points the plugins specialize
- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) - Understanding vector and tensor characters

**How-to Guides:**

- [Using Custom Representation Types](using_custom_representation_types.md) - Authoring a representation type of your own from scratch
- [Decompose a Vector Quantity into Components](../advanced_usage/decompose_vector_quantity.md) - Splitting a vector quantity into named component quantities

**Examples:**

- [Using a Linear Algebra Library as the Representation](../../examples/linear_algebra.md) - Eigen, GLM, Blaze, and the built-in `cartesian_vector` driven through the same scenario
<!-- markdownlint-enable MD013 -->
