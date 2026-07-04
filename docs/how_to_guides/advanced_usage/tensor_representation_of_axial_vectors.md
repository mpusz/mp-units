# Represent an Axial Vector as an Antisymmetric Tensor

This guide shows how to work with a quantity, such as _angular velocity_, that the ISQ
defines as a **vector** but that your domain represents as an **antisymmetric tensor** (a
skew-symmetric matrix). It explains why the tensor form is a _separate_ quantity rather than
the ISQ one in bigger storage, how to define it, and how to bridge to and from
`isq::angular_velocity` with an explicit dual map.

For background on the concepts used here, see:

<!-- markdownlint-disable MD013 -->
- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) -
  scalar, vector, and tensor characters
- [Why a Quantity Has a Character](../../blog/posts/why-a-quantity-has-a-character.md) -
  why character is part of a quantity's identity
- [Representation Types](../../users_guide/framework_basics/representation_types.md) -
  what a type must provide to act as a representation
<!-- markdownlint-enable MD013 -->

!!! info "The examples use Eigen"

    The snippets below use [Eigen](https://eigen.tuxfamily.org) through
    `#include <mp-units/integrations/eigen.h>`, the common choice in robotics and rigid-body
    dynamics. The technique is backend-agnostic: it works the same with Blaze or the built-in
    `cartesian_vector` / `cartesian_tensor`.


## The Problem: the Matrix Does Not Fit the Vector Slot

ISO 80000-3 (item 3-12) defines _angular velocity_ as a **vector**: a scalar rate of
rotation carrying the axis direction. So in **mp-units** it has vector character, and a
three-component representation fits:

```cpp
quantity w = Eigen::Vector3d{0.0, 0.0, 2.5} * isq::angular_velocity[rad / s];  // OK
```

Robotics and rigid-body dynamics, however, routinely represent the same physical
quantity as the antisymmetric **angular velocity tensor** `Ω`, the 3x3 skew-symmetric
matrix for which `Ω r` equals `ω x r`. That matrix is an order-2 object, and it does
**not** fit the vector slot:

```cpp
Eigen::Matrix3d Omega{{ 0.0, -2.5, 0.0},
                      { 2.5,  0.0, 0.0},
                      { 0.0,  0.0, 0.0}};
quantity bad = Omega * isq::angular_velocity[rad / s];   // error: order 2 does not fit a vector
```

The rank-ordering that lets a scalar stand in for a vector does not run the other way, so
an order-2 representation is rejected for a vector quantity. That is deliberate, and
forcing the matrix in is the wrong fix, for reasons worth understanding before the
solution.


## Why It Is a Separate Quantity, Not `isq::angular_velocity`

It is tempting to reach for either of two shortcuts. Both are wrong.

1. **Make the tensor form the same `isq::angular_velocity`, just stored differently.** A
   quantity's character is part of its _identity_, and the ISQ defines _angular velocity_
   only as a vector. The matrix is a different mathematical object, the dual of the vector,
   related to it by an explicit map, not the same quantity in a larger representation. Were
   it the same quantity, it would also have to flow through the vector's arithmetic (for
   example `angular_momentum = moment_of_inertia * angular_velocity` yields the right vector
   `L` only for the vector `ω`, never for the matrix `Ω`).
2. **Make it a child of `isq::angular_velocity`.** The ISQ does model a vector as a child
   of a scalar (`displacement` under `length`, `velocity` under `speed`),
   so a higher-order child is not forbidden in itself. But there the scalar parent is the
   _magnitude_ of the vector child: `speed` is `|velocity|`, a genuine refinement where
   the child adds a direction and the parent is recovered by the norm. The tensor and the
   vector _angular velocity_ are not in that relationship. They carry the _same_
   information, related by a bijective dual (the Hodge star), and the vector is _not_ the
   magnitude of the tensor (that norm is `√2·|ω|`). Modeling a lateral dual as a vertical
   refinement is a category error, and it inverts generality: across dimensions the tensor
   is the general object and the 3D vector the special case, so making the tensor a child
   of the vector is backwards.

The honest model is that the tensor form is its own quantity, defined in your domain, bridged
to the ISQ vector by an explicit dual. See
[Why a Quantity Has a Character](../../blog/posts/why-a-quantity-has-a-character.md) for the
full reasoning.


## Define the Tensor Quantity

Declare a quantity with the same defining equation as _angular velocity_ (angle over
duration, hence the same dimension) but **tensor** character. It is a sibling of
`isq::angular_velocity` by construction, not a child of it:

```cpp
inline constexpr struct angular_velocity_tensor :
    quantity_spec<isq::angular_measure / isq::duration, quantity_tensor_order::tensor> {} angular_velocity_tensor;
```

An `Eigen::Matrix3d` (or a Blaze matrix, or the built-in `cartesian_tensor`) now satisfies
it: the order-2 representation that `isq::angular_velocity` rejects finally has a home. A
tensor quantity also accepts scalar and vector representations, since a scalar and a
vector are tensors of order 0 and 1 and rank-ordering lets a lower order fill a higher
slot. The restriction runs only the other way, which is exactly why the vector
`isq::angular_velocity` turns the order-2 matrix away.


## The `hat` Map: Vector to Tensor

`hat` (written `∧` in the literature) builds the skew-symmetric matrix from the vector's
components:

```cpp
inline constexpr auto rad_per_s = si::radian / si::second;

[[nodiscard]] auto hat(const quantity<isq::angular_velocity[rad_per_s], Eigen::Vector3d>& w)
{
  const auto& c = w.numerical_value_ref_in(rad_per_s);
  Eigen::Matrix3d m{{  0.0, -c.z(),  c.y()},
                    { c.z(),   0.0, -c.x()},
                    {-c.y(),  c.x(),   0.0}};
  return m * angular_velocity_tensor[rad_per_s];
}
```

The result is a `quantity` of `angular_velocity_tensor`, so it can never be mistaken for the
vector it came from.


## The `vee` Map: Tensor to Vector

`vee` (written `∨`) is the inverse, reading the three independent components back out:

```cpp
[[nodiscard]] auto vee(const quantity<angular_velocity_tensor[rad_per_s], Eigen::Matrix3d>& w)
{
  const auto& m = w.numerical_value_ref_in(rad_per_s);
  Eigen::Vector3d v{m(2, 1), m(0, 2), m(1, 0)};
  return v * isq::angular_velocity[rad_per_s];
}
```

`vee` returns an ordinary `isq::angular_velocity`, so the value re-enters the ISQ the moment
you cross back.


## Interoperating with the ISQ

Convert explicitly at the boundary. Work in the tensor form where your algorithm wants a
matrix, and call `vee` before handing the value to code (or to ISQ equations) written for
the vector _angular velocity_:

```cpp
quantity w = Eigen::Vector3d{1.0, 2.0, 3.0} * isq::angular_velocity[rad_per_s];

quantity Omega = hat(w);        // angular_velocity_tensor, backed by a 3x3 matrix
// ... rigid-body computations that want the matrix form ...
quantity w_again = vee(Omega);  // back to isq::angular_velocity, ready for ISQ equations

static_assert(Omega.quantity_spec == angular_velocity_tensor);
static_assert(w_again.quantity_spec == isq::angular_velocity);
```

The conversion is visible in the source, which is the point: the vector and the tensor are
different objects, and the dual between them is a real operation with content (in 3D it is
the Hodge star), not a silent coercion.


## What You Keep and What You Give Up

- **You keep** full character safety on both forms. The matrix cannot be used where a vector
  _angular velocity_ is expected, and vice versa, until you convert.
- **You give up** automatic participation of the tensor form in the built-in ISQ equations.
  Those are written against the vector, so you bridge with `vee` (or `hat`) at the boundary.
  This is the honest cost of the two being genuinely different objects.

The same pattern applies to any axial-vector quantity your domain prefers to carry as an
antisymmetric tensor, such as _angular momentum_ or _moment of force_. Define the tensor
sibling, then provide its `hat` / `vee` dual to `isq::angular_momentum` or
`isq::moment_of_force`.


## See Also

<!-- markdownlint-disable MD013 -->
- [Why a Quantity Has a Character](../../blog/posts/why-a-quantity-has-a-character.md) -
  why the tensor form is a separate quantity, not the ISQ one relaxed
- [Character of a Quantity](../../users_guide/framework_basics/character_of_a_quantity.md) -
  scalar, vector, and tensor characters and their operations
- [Decompose a Vector Quantity into Components](decompose_vector_quantity.md) -
  the complementary direction, naming the axes of a vector quantity
- [Representation Types](../../users_guide/framework_basics/representation_types.md) -
  the requirements a representation must meet
<!-- markdownlint-enable MD013 -->
