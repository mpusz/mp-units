---
date: 2026-06-23
authors:
 - mpusz
categories:
 - Feature
comments: true
---

# Units Meet Linear Algebra: Two Approaches, Two Problems

How do units and linear algebra fit together? We get that question often. The honest
answer is that two different questions hide inside it, and they have different solutions.
The occasion for answering now is concrete: **mp-units** ships opt-in integrations that let
mainstream linear algebra libraries ([Eigen](https://eigen.tuxfamily.org),
[GLM](https://github.com/g-truc/glm), and [Blaze](https://bitbucket.org/blaze-lib/blaze))
act directly as the representation type of a _quantity_.

<!-- more -->

## A vector *as* a quantity

The first question is: _"I have one physical vector, such as a velocity, a force, or an
acceleration. How do I attach a unit and a direction to it?"_

This is a **vector quantity**: a single quantity whose representation type happens to be a
3D vector. One unit governs the whole object, and the vector supplies the direction. With
the new integrations this is a one-liner against any supported backend:

```cpp
#include <Eigen/Core>
#include <mp-units/integrations/eigen.h>   // or .../glm.h, or .../blaze.h
// module mode: keep the <Eigen/Core> include and `import mp_units.integrations.eigen;`

quantity velocity = isq::velocity(Eigen::Vector3d{30, 40, 0} * km / h);
```

From there the framework treats the vector exactly like any other representation:

```cpp
quantity velocity_mps = velocity.in(m / s);   // unit conversion scales every component
quantity displacement = velocity * (2 * h);   // vector quantity × scalar quantity
quantity speed = magnitude(velocity);         // Euclidean magnitude → scalar quantity
```

The integration is deliberately thin. Each header is guarded with `__has_include`, so it
is a harmless no-op when the library is absent, and it only wires up the
[representation customization points](../../users_guide/framework_basics/representation_types.md):

- the vector's `value_type` is detected as the underlying numeric type,
- the library's `norm()`/`length()` is picked up by the `magnitude()` CPO,
- and, crucially for expression-template libraries, every lazy `operator*`/`operator+`
  proxy is materialized into its concrete `PlainObject` before being stored, so a
  _quantity_ never holds a dangling reference to an expired operand.

No adapter code of your own is required. The library's native vector types are used
directly. Any _weakly-regular_ vector type with a Euclidean norm qualifies as a
representation, with Armadillo the notable exception (its `operator==` returns an
element-wise mask rather than a `bool`).

You are not limited to third-party libraries. The built-in `cartesian_vector` works out of
the box with no plugin, and even plain `double` and `int` serve as one-dimensional vector
representations.

This is the case most people mean by "units and linear algebra", and the integrations
cover it today.

### Naming the components

A vector _quantity_ travels as one object, but you often need its axes by name: a drone's
_forward speed_, its _lateral drift_, its _rate of climb_. Reaching into the representation
for `v[2]` throws the types away and hands you a bare number with no unit and no idea which
axis it came from. **mp-units** lets you decompose a vector _quantity_ into named,
strongly-typed 1D-vector components instead. You give the whole a spec, declare each axis
as its own kind, and list them in coordinate order:

```cpp
inline constexpr struct flight_velocity : quantity_spec<isq::velocity> {} flight_velocity;
inline constexpr struct forward_velocity : quantity_spec<isq::velocity, is_kind> {} forward_velocity;
inline constexpr struct lateral_velocity : quantity_spec<isq::velocity, is_kind> {} lateral_velocity;
inline constexpr struct vertical_velocity : quantity_spec<isq::velocity, is_kind> {} vertical_velocity;

template<>
struct mp_units::vector_components<flight_velocity> :
    mp_units::vector_axes<forward_velocity, lateral_velocity, vertical_velocity> {};
```

The components then come out as _quantities_, by index, by axis spec, or through structured
bindings:

```cpp
quantity velocity = flight_velocity(Eigen::Vector3d{30, 10, -2} * km / h);

const auto [forward, lateral, vertical] = velocity;   // three typed component quantities
quantity climb = get<vertical_velocity>(velocity);    // or pull one axis out by its spec
```

Mixing axes (`forward + vertical`) is now a compile error, while same-axis arithmetic
still works. The
[Decompose a Vector Quantity into Components](../../how_to_guides/advanced_usage/decompose_vector_quantity.md)
guide covers how to form the hierarchy, what the representation must provide, and the
`tuple_size` / `tuple_element` protocol for compile-time code.

### A V2 limitation worth knowing

There is a rough edge in this release, and it is worth being explicit about. **mp-units**
can attach a unit to a vector representation, but the V2 type system cannot always name the
*result* of a vector operation precisely.

Take `magnitude()`. It works on a vector _quantity_ directly:

```cpp
quantity speed = magnitude(velocity);   // 50 km/h
```

but the result deliberately drops the precise _quantity spec_ down to the unit's **kind**.
The type V2 *should* produce (a dedicated scalar-magnitude _quantity spec_, for example
`vec_mag<isq::force>` with scalar character) cannot be expressed yet. The consequence is
subtle but real. When the unit derives purely from scalar base units (`km/h` →
_length/time_) the result collapses to a clean scalar character. But for a unit tied to a
vector _quantity spec_ (`N` is `kind_of<isq::force>`) the result **keeps vector
character**, so technically you could take the magnitude of a magnitude. That is a known
limitation, not a feature.

!!! info "Why no quantity-level `scalar_product()` / `vector_product()` in V2?"

    The same problem hits the scalar product (dot) and vector product (cross) **much harder**.
    The only reference V2 lets you synthesize from the operands is `Q1::reference * Q2::reference`,
    and the product of two vector _quantity specs_ is itself **vector**-character.

    For `vector_product()` (cross) that happens to be right: the result *is* a vector, so a vector
    representation paired with a vector-character reference is consistent.

    ```cpp
    template<Quantity Q1, Quantity Q2>
    QuantityOf<Q1::quantity_spec * Q2::quantity_spec> auto vector_product(const Q1& a, const Q2& b)
    {
      return vector_product(a.numerical_value_in(a.unit), b.numerical_value_in(b.unit)) *
             (Q1::reference * Q2::reference);
    }
    ```

    For `scalar_product()` (dot) the very same reference is **wrong**: the numerical result is a
    *scalar*, yet `Q1::reference * Q2::reference` is vector-character, and V2 gives you no way to
    say "same dimension, but scalar". There is simply no correct reference to attach. That
    asymmetry is why the library provides *neither* as a quantity-level operation. Both stay on the
    raw representation (for example `cartesian_vector`) until V3 can express the result
    _quantity spec_.

This is exactly the expressiveness the V3 _quantity spec_ redesign is meant to restore,
making vector operations return correctly-typed results instead of relying on
caller-supplied references.

## A vector *of* quantities

The second question is different: _"I have an aggregate of several scalar quantities,
possibly of different kinds, that I want to manipulate with matrix algebra."_ Think of a
Kalman filter state vector:

$$
\mathbf{x} = \begin{bmatrix} \text{position} \\ \text{velocity} \end{bmatrix}
$$

Here the first element is a _length_ and the second a _speed_. The covariance matrix that
goes with it is worse still. Every entry has its own composite unit ($\mathsf{m^2}$,
$\mathsf{m^2/s}$, $\mathsf{m^2/s^2}$, …). This is **not** a vector quantity. It is a
**heterogeneous container of quantities** that happens to obey the rules of linear algebra.

You might reach for `Eigen::Matrix<quantity<...>, 2, 1>`, and it works *only* in the
homogeneous case, where every element shares one type. The moment the elements differ, you
hit the wall that every general-purpose linear algebra library shares:

> **A matrix demands a single, homogeneous element type.**

That constraint is not an oversight. It is what makes a contiguous, cache-friendly, SIMD-able
buffer possible. So storing per-element units inside the matrix is a non-starter for the
mainstream libraries.

### The missing piece: typed indices

The way out is to move the type information **out of the storage and onto the indices**. If
row $i$ and column $j$ each carry a compile-time type, then element $(i,j)$ has a *derived*
type, the buffer stays homogeneous, and the algebra can still type-check.

This is exactly the approach taken by [TypedLinearAlgebra](https://github.com/FrancoisCarouge/TypedLinearAlgebra),
a library by [François Carouge](https://github.com/FrancoisCarouge) (of Kalman-filter-library
fame). It wraps a backend such as Eigen, tags it with row/column index types, and gives you
type-safe access:

```cpp
// element (0,1) has a type derived from row-0 and column-1 index types
auto value = m.at<0, 1>();
```

with the documented principle that _"type safety cannot be guaranteed at compile time
without index safety."_ Compose that index typing with **mp-units** quantities as the
element types and you get a matrix whose every cell is a strongly-typed quantity. The
Kalman filter case is handled properly, with the underlying buffer still a plain
homogeneous array.

This is young, promising work that deserves more eyes and feedback. If the "vector of quantities"
problem is yours, go try it, file issues, and help it grow. It solves a problem that the big
libraries structurally cannot.

## Which one do I want?

|         | **Vector quantity**                            | **Vector/matrix of quantities**            |
|---------|------------------------------------------------|--------------------------------------------|
| Models  | one physical vector (velocity, force, …)       | an aggregate of distinct scalar quantities |
| Unit    | **one** unit for the whole object              | **per-element** units, often heterogeneous |
| Storage | unit outside, plain vector inside              | homogeneous buffer + typed indices         |
| Example | drone velocity, EM field                       | Kalman state & covariance, Jacobians       |
| Today   | built-in `cartesian_vector` or Eigen/GLM/Blaze | **mp-units** elements + TypedLinearAlgebra |

Both are valid and both are first-class use cases. They simply answer different questions.
The mistake is trying to force one tool to do the other's job: a vector quantity is not the
place for a heterogeneous state vector, and a typed matrix is overkill for a single
velocity.

## Try it

The vector-quantity integrations ship now. The
[Using a Linear Algebra Library](../../how_to_guides/integration/using_linear_algebra_libraries.md)
guide walks through the shipped plugins and how to adapt a library that has none, and the
[linear algebra example](../../examples/linear_algebra.md) compiles the *same* scenario
against Eigen, GLM, Blaze, and the built-in `cartesian_vector`. For the
vector-of-quantities case, try
[TypedLinearAlgebra](https://github.com/FrancoisCarouge/TypedLinearAlgebra) and tell
François (and us) how it goes.

As always, feedback on both fronts is very welcome.
