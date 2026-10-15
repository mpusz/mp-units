# Character of a Quantity

A quantity's _character_ has two orthogonal axes: its **tensor order** (scalar, vector, or
tensor) and its **numeric field** (real or complex). The two are independent, so every
combination is possible: a _velocity_ is a real vector, a _voltage phasor_ a complex scalar,
and a complex field amplitude in electromagnetics a complex vector. This chapter covers both
axes and how to declare them.


## Scalars, vectors, and tensors

!!! quote "ISO 80000-2"

    Scalars, vectors and tensors are mathematical objects that can be used to denote certain physical
    quantities and their values. They are as such independent of the particular choice of a coordinate system,
    whereas each scalar component of a vector or a tensor and each component vector and component
    tensor depend on that choice.

Such distinction is important because each quantity character represents different properties
and allows different operations to be done on its quantities.

For example, imagine a physical units library that allows the creation of a $speed$ quantity
from both $length / time$ and $length * time$.
It wouldn't be too safe to use such a product, right?

Now we have to realize that both of the above operations (multiplication and division)
are not even mathematically defined for linear algebra types such as vectors or tensors.
On the other hand, two vectors can be passed as arguments to dot and cross-product operations.
The result of the first one is a scalar. The second one results in a vector that is
perpendicular to both vectors passed as arguments. Again, it wouldn't be safe to allow
replacing those two operations with each other or expect the same results from both cases.
This simply can't work.


## ISQ defines quantities of all characters

While defining quantities, ISO 80000 explicitly mentions when a specific quantity has a vector
or tensor character. Here are some examples:

| Quantity                 |  Character   |                    Quantity Equation                    |
|--------------------------|:------------:|:-------------------------------------------------------:|
| $duration$               |    scalar    |                    _{base quantity}_                    |
| $mass$                   |    scalar    |                    _{base quantity}_                    |
| $length$                 |    scalar    |                    _{base quantity}_                    |
| $path\; length$          |    scalar    |                    _{base quantity}_                    |
| $radius$                 |    scalar    |                    _{base quantity}_                    |
| $position\; vector$      |  **vector**  |                    _{base quantity}_                    |
| $velocity$               |  **vector**  |             $position\; vector / duration$              |
| $acceleration$           |  **vector**  |                  $velocity / duration$                  |
| $force$                  |  **vector**  |                  $mass * acceleration$                  |
| $power$                  |    scalar    |                 $force \cdot velocity$                  |
| $moment\; of\; force$    |  **vector**  |            $position\; vector \times force$             |
| $torque$                 |    scalar    |      $moment\; of\; force \cdot \{unit\; vector\}$      |
| $surface\; tension$      |    scalar    |             $\lvert force \rvert / length$              |
| $angular\; displacement$ |    scalar    |                $path\; length / radius$                 |
| $angular\; velocity$     |  **vector**  | $angular\; displacement / duration * \{unit\; vector\}$ |
| $momentum$               |  **vector**  |                    $mass * velocity$                    |
| $angular\; momentum$     |  **vector**  |           $position\; vector \times momentum$           |
| $moment\; of\; inertia$  | **_tensor_** |     $angular\; momentum \otimes angular\; velocity$     |

In the above equations:

- $a * b$ - regular multiplication where one of the arguments has to be scalar
- $a / b$ - regular division where the divisor has to be scalar
- $a \cdot b$ - dot product of two vectors
- $a \times b$ - cross product of two vectors
- $\lvert a \rvert$ - magnitude of a vector
- $\{unit\; vector\}$ - a special vector with the magnitude of $1$
- $a \otimes b$ - tensor product of two vectors or tensors

!!! note

    As of now, all of the C++ physical units libraries on the market besides **mp-units** do not
    support the operations mentioned above. They expose only multiplication and division operators,
    which do not work for linear algebra-based representation types. If a user of those libraries
    would like to create the quantities provided in the above table properly, this would result in
    a compile-time error stating that multiplication and division of two linear algebra vectors is
    impossible.


## Real and complex quantities

Order is only half of the character. The other axis is the **field**: whether a quantity's
values are real or complex. Most quantities are real. Some are inherently complex, carrying
a magnitude and a phase in a single value rather than two separate real numbers:

| Quantity           |     Character      | Notes                             |
|--------------------|:------------------:|-----------------------------------|
| $voltage$          |    real scalar     | an instantaneous value            |
| $voltage\; phasor$ | **complex** scalar | magnitude and phase of a sinusoid |
| $impedance$        | **complex** scalar | $resistance + j\,reactance$       |
| $admittance$       | **complex** scalar | $1 / impedance$                   |

Like the order, the field is fixed by what a quantity _means_, before any C++ type is chosen.
Operations such as `real()`, `imag()`, and `modulus()` are meaningful only on a complex
quantity, and the library exposes them exactly where the field says complex. The field is
then matched to the representation exactly: a complex quantity requires a complex representation
(such as `std::complex`), and a real quantity a real one, with no implicit lift between them.

The two axes are independent. Besides the familiar real scalars and real vectors, a quantity
can be a complex scalar (a _voltage phasor_), or even a complex vector or tensor (a complex
field amplitude or a complex _permittivity_ in electromagnetics).


## Characters don't apply to dimensions and units

ISO 80000 explicitly states that dimensions are orthogonal to quantity characters:

!!! quote "ISO 80000-1:2009"

    In deriving the dimension of a quantity, no account is taken of its scalar, vector, or tensor character.

Also, it explicitly states that:

!!! quote "ISO 80000-2"

    All units are scalars.

## Defining the character of a quantity

A character is one value from each axis. The two axes are plain enumerations, and
`quantity_character` pairs them:

```cpp
enum class quantity_tensor_order : std::int8_t { scalar, vector, tensor };  // rank-ordered: a lower order fills a higher
enum class quantity_field : std::int8_t { real, complex };                  // matched exactly: never one for the other

// a character is exactly one of each
struct quantity_character {
  quantity_tensor_order order = quantity_tensor_order::scalar;
  quantity_field field = quantity_field::real;
};
```

To set the character explicitly, append a `quantity_tensor_order` value (for a vector or
tensor) or a `quantity_field` value (for a complex quantity) to the `quantity_spec` describing
the quantity type:

=== "C++23"

    ```cpp
    inline constexpr struct displacement : quantity_spec<length, quantity_tensor_order::vector> {} displacement;
    inline constexpr struct position_vector : quantity_spec<displacement> {} position_vector;
    inline constexpr struct moment_of_inertia
      : quantity_spec<angular_momentum / angular_velocity, quantity_tensor_order::tensor> {} moment_of_inertia;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct displacement : quantity_spec<displacement, length, quantity_tensor_order::vector> {} displacement;
    inline constexpr struct position_vector : quantity_spec<position_vector, displacement> {} position_vector;
    inline constexpr struct moment_of_inertia
      : quantity_spec<moment_of_inertia, angular_momentum / angular_velocity, quantity_tensor_order::tensor> {} moment_of_inertia;
    ```

=== "Portable"

    ```cpp
    QUANTITY_SPEC(displacement, length, quantity_tensor_order::vector);
    QUANTITY_SPEC(position_vector, displacement);
    QUANTITY_SPEC(moment_of_inertia, angular_momentum / angular_velocity, quantity_tensor_order::tensor);
    ```

With the above, all the quantities derived from these
will have a correct character determined according to the kind of operations included in the
[quantity equation](../../reference/glossary.md#quantity-equation) defining a
[derived quantity](../../reference/glossary.md#derived-quantity).

For example, `velocity` in the below definition will be defined as a vector quantity.
No explicit character override is needed:

=== "C++23"

    ```cpp
    inline constexpr struct velocity : quantity_spec<speed, displacement / duration> {} velocity;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct velocity : quantity_spec<velocity, speed, displacement / duration> {} velocity;
    ```

=== "Portable"

    ```cpp
    QUANTITY_SPEC(velocity, speed, displacement / duration);
    ```

A complex quantity is declared the same way, with a `quantity_field` value. For example, a
_voltage phasor_ shares everything with _voltage_ but is complex rather than real:

=== "C++23"

    ```cpp
    inline constexpr struct voltage_phasor : quantity_spec<voltage, quantity_field::complex> {} voltage_phasor;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct voltage_phasor : quantity_spec<voltage_phasor, voltage, quantity_field::complex> {} voltage_phasor;
    ```

=== "Portable"

    ```cpp
    QUANTITY_SPEC(voltage_phasor, voltage, quantity_field::complex);
    ```


## A quantity kind has no character

Every *concrete* quantity spec has one character, as declared above. A
[quantity kind](systems_of_quantities.md) has none, and cannot be given one, because a kind
covers its whole quantity tree. The _length_ kind holds scalars like _radius_, _width_, _height_,
but also vectors like _displacement_ and _position vector_. Similarly _speed_ kind holds both
the scalar _speed_ and the vector _velocity_. No single character can describe such kinds:

```cpp
static_assert(get_kind(isq::velocity) == get_kind(isq::speed));   // one kind, two characters
```

It goes further than the tree, too. A [simple quantity](simple_and_typed_quantities.md) is
of a kind as well, since the spec its unit yields is a `kind_of<…>`, so there is no concrete
quantity type anywhere and the representation you hand it is the only thing that decides:

```cpp
const quantity a = 30.0 * km / h;                               // a scalar quantity
const quantity b = cartesian_vector{30.0, 40.0, 0.0} * km / h;  // a vector quantity
const quantity c = cartesian_tensor<double>{} * km / h;         // a tensor quantity
```

All three are quantities of the same kind. Nothing in the quantity hierarchy could have
predicted that `c` holds a tensor, so a kind constrains neither axis and the representation
carries the answer.

## Asking what a quantity is

Because of the above, a character is not something to read off a quantity and compare. It
is something to *ask about*, and the question always weighs the spec together with the
representation:

```cpp
using enum quantity_tensor_order;

static_assert(QuantityOf<quantity<isq::speed[m / s], double>, scalar>);
static_assert(!QuantityOf<quantity<isq::speed[m / s], double>, vector>);   // a speed is never a vector

static_assert(QuantityOf<quantity<isq::velocity[m / s], double>, vector>);  // a 1D velocity
static_assert(QuantityOf<quantity<isq::velocity[m / s], cartesian_vector<double>>, vector>);
static_assert(QuantityOf<quantity<isq::stress[Pa], cartesian_tensor<double>>, tensor>);

static_assert(QuantityOf<quantity<km / h, cartesian_vector<double>>, vector>);   // a kind: the rep decides
static_assert(QuantityOf<quantity<km / h, cartesian_tensor<double>>, tensor>);

static_assert(QuantityOf<quantity<isq::speed[m / s], double>, quantity_field::real>);
static_assert(QuantityOf<quantity<V, std::complex<double>>, quantity_field::complex>);
```

The same `QuantityOf` you already use to pin a _kind_ answers this, with a character in
place of a specification. The two readings are guarded by disjoint argument types: a
specification still means the quantity-tree question and never silently becomes a character
one. A _quantity point_ asks [`QuantityPointOf`](concepts.md#QuantityPointOf) the same way,
since a _point_ is not a `Quantity`.

Neither half of that question is sufficient alone. A spec is not, because a kind admits
every character. A representation is not either, because `double` is a valid scalar,
1D-vector, *and* degenerate-tensor representation, so it cannot tell you what the quantity
is.

The rank ordering of clause 18 of ISO 80000-2:2019 lives in the representation half of
that question, not in the specification half. A scalar representation fills a vector or a
tensor slot, which is what makes a 1D _velocity_ stored in a `double` a vector quantity
with a single component. The specification half stays exact, so for a concrete spec the
three orders are mutually exclusive, as the asserts above show for a _speed_. The field
axis is disjoint on both sides: a quantity is real or complex, never both.

These are also what the library itself uses to decide which operations exist.
`magnitude()` is available on a vector or a tensor quantity whose representation can
produce a norm, and never on a definite scalar:

```cpp
const quantity v = isq::velocity(cartesian_vector{3.0, 4.0, 0.0} * m / s);
const quantity speed = v.magnitude();               // 5 m/s

const quantity s = 30.0 * isq::speed[m / s];
// auto nonsense = s.magnitude();                   // error: a speed is a definite scalar
```

## Representation types for vector and tensor quantities

The `quantity` class template's second parameter is constrained with
[`RepresentationOf`](../advanced_topics/concepts.md#RepresentationOf), which verifies that
the representation type satisfies the requirements for the quantity's character. See
[Representation Types](../advanced_topics/representation_types.md) for the full details on
requirements and how to use custom types.

!!! note

    The current version of the C++ Standard Library does not provide any types that could be used as
    a representation type for vector and tensor quantities. **mp-units** ships two built-in types:
    `cartesian_vector<T, N>` for `N`-dimensional Cartesian vectors and
    `cartesian_tensor<T, N>` for second-order `N`×`N` Cartesian tensors, where the
    compile-time dimension `N` is `2` or `3` (default `3`). Any third-party linear
    algebra library types can also be used via the customization points described in
    [Representation Types](../advanced_topics/representation_types.md).

    Both types live in the `mp_units::utility` namespace. The snippets below assume a
    `using namespace mp_units::utility;` (or an `mp_units::utility::` qualification).

For example, using the built-in `cartesian_vector`:

```cpp
#include <mp-units/utility/cartesian_vector.h>

Quantity auto q = cartesian_vector{1., 2., 3.} * isq::velocity[m / s];
```

!!! note

    The following does not work:

    ```cpp
    Quantity auto q1 = cartesian_vector{1., 2., 3.} * m / s;
    Quantity auto q2 = isq::velocity(cartesian_vector{1., 2., 3.} * m / s);
    quantity<isq::velocity[m/s]> q3{cartesian_vector{1., 2., 3.} * m / s};
    ```

    In all the cases above, the SI unit `m / s` has an associated scalar quantity of `isq::length / isq::duration`.
    `cartesian_vector` is not a correct representation type for a scalar quantity so the construction fails.

A _tensor quantity_ works the same way through the built-in `cartesian_tensor`, an
`N`×`N` second-order Cartesian tensor:

```cpp
#include <mp-units/utility/cartesian_tensor.h>

Quantity auto stress = cartesian_tensor{1., 0., 0., 0., 1., 0., 0., 0., 1.} * isq::stress[Pa];
```

`cartesian_tensor` satisfies the tensor character and provides the ISO 80000-2 second-order
operations (`tensor_product`, `inner_product`, `scalar_product`). It is kept out of the vector
character on purpose, so it cannot be used where a vector representation is expected.

!!! quote "ISO 80000-2"

    A vector is a tensor of the first order and a scalar is a tensor of order zero.

By that same ordering, a lower-order representation fills a higher-order slot, so the
simpler types are still accepted for a tensor quantity: a fundamental type models a scalar
tensor measure (for example a _von Mises stress_), and a `cartesian_vector` is also a
valid tensor representation. See [Representation
Types](../advanced_topics/representation_types.md) for the full rules.
