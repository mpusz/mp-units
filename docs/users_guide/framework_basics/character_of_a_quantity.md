# Character of a Quantity

!!! warning

    This chapter's features are experimental and subject to change or removal. Please share your feedback
    if something seems wrong or could be improved.


## Scalars, vectors, and tensors

!!! quote "ISO 80000-2"

    Scalars, vectors and tensors are mathematical objects that can be used to denote certain physical
    quantities and their values. They are as such independent of the particular choice of a coordinate system,
    whereas each scalar component of a vector or a tensor and each component vector and component
    tensor depend on that choice.

Such distinction is important because each quantity character represents different properties
and allows different operations to be done on its quantities.

For example, imagine a physical units library that allows the creation of a $speed$ quantity from both
$length / time$ and $length * time$. It wouldn't be too safe to use such a product, right?

Now we have to realize that both of the above operations (multiplication and division) are not even
mathematically defined for linear algebra types such as vectors or tensors. On the other hand, two vectors
can be passed as arguments to dot and cross-product operations. The result of the first one is
a scalar. The second one results in a vector that is perpendicular to both vectors passed as arguments.
Again, it wouldn't be safe to allow replacing those two operations with each other or expect the same
results from both cases. This simply can't work.


## ISQ defines quantities of all characters

While defining quantities ISO 80000 explicitly mentions when a specific quantity has a vector or tensor
character. Here are some examples:

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


## Characters don't apply to dimensions and units

ISO 80000 explicitly states that dimensions are orthogonal to quantity characters:

!!! quote "ISO 80000-1:2009"

    In deriving the dimension of a quantity, no account is taken of its scalar, vector, or tensor character.

Also, it explicitly states that:

!!! quote "ISO 80000-2"

    All units are scalars.

## Defining vector and tensor quantities

To specify that a specific quantity has a vector or tensor character a value of `quantity_character`
enumeration can be appended to the `quantity_spec` describing such a quantity type:

=== "C++23"

    ```cpp
    inline constexpr struct displacement final : quantity_spec<length, quantity_character::vector> {} displacement;
    inline constexpr struct position_vector final : quantity_spec<displacement> {} position_vector;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct displacement final : quantity_spec<displacement, length, quantity_character::vector> {} displacement;
    inline constexpr struct position_vector final : quantity_spec<position_vector, displacement> {} position_vector;
    ```

=== "Portable"

    ```cpp
    QUANTITY_SPEC(displacement, length, quantity_character::vector);
    QUANTITY_SPEC(position_vector, displacement);
    ```

With the above, all the quantities derived from `position_vector` or `displacement` will have a correct
character determined according to the kind of operations included in the
[quantity equation](../../appendix/glossary.md#quantity-equation) defining a
[derived quantity](../../appendix/glossary.md#derived-quantity).

For example, `velocity` in the below definition will be defined as a vector quantity (no explicit
character override is needed):

=== "C++23"

    ```cpp
    inline constexpr struct velocity final : quantity_spec<speed, displacement / duration> {} velocity;
    ```

=== "C++20"

    ```cpp
    inline constexpr struct velocity final : quantity_spec<velocity, speed, displacement / duration> {} velocity;
    ```

=== "Portable"

    ```cpp
    QUANTITY_SPEC(velocity, speed, displacement / duration);
    ```


## Representation types for vector and tensor quantities

As we remember, the `quantity` class template is defined as follows:

```cpp
template<Reference auto R,
         RepresentationOf<get_quantity_spec(R).character> Rep = double>
class quantity;
```

The second template parameter is constrained with a [`RepresentationOf`](concepts.md#RepresentationOf)
concept that checks if the provided representation type satisfies the requirements for the character
associated with this quantity type.

!!! note

    The current version of the C++ Standard Library does not provide any types that could be used as
    a representation type for vector and tensor quantities. This is why users are on their own here
    :worried:.

    To provide examples and implement unit tests, our library uses the types proposed in
    the [P1385](https://wg21.link/p1385) and available as
    [a Conan package in the Conan Center](https://conan.io/center/wg21-linear_algebra).
    However, thanks to the provided customization points, any linear algebra library types can be used
    as a vector or tensor quantity representation type.

To enable the usage of a user-defined type as a representation type for vector or tensor quantities,
we need to provide a partial specialization of `is_vector` or `is_tensor` customization points.

For example, here is how it can be done for the [P1385](https://wg21.link/p1385) types:

```cpp
#include <matrix>

using la_vector = STD_LA::fixed_size_column_vector<double, 3>;

template<>
constexpr bool mp_units::is_vector<la_vector> = true;
```

With the above, we can use `la_vector` as a representation type for our quantity:

```cpp
Quantity auto q = la_vector{1, 2, 3} * isq::velocity[m / s];
```

In case there is an ambiguity of `operator*` between **mp-units** and a linear algebra library, we can
either:

- use two-parameter constructor

    ```cpp
    Quantity auto q = quantity{la_vector{1, 2, 3}, isq::velocity[m / s]};
    ```

- provide a dedicated overload of `operator*` that will resolve the ambiguity and wrap the above

    ```cpp
    template<Reference R>
    Quantity auto operator*(la_vector rep, R)
    {
      return quantity{rep, R{}};
    }
    ```

!!! note

    The following does not work:

    ```cpp
    Quantity auto q1 = la_vector{1, 2, 3} * m / s;
    Quantity auto q2 = isq::velocity(la_vector{1, 2, 3} * m / s);
    quantity<isq::velocity[m/s]> q3{la_vector{1, 2, 3} * m / s};
    ```

    In all the cases above, the SI unit `m / s` has an associated scalar quantity of `isq::length / isq::time`.
    `la_vector` is not a correct representation type for a scalar quantity so the construction fails.


## Hacking the character

Sometimes we want to use a vector quantity, but we don't care about its direction. For example,
the standard gravity acceleration constant always points down, so we might not care about this
in a particular scenario. In such a case, we may want to "hack" the library to allow scalar types
to be used as a representation type for scalar quantities.

For example, we can do the following:

```cpp
template<class T>
  requires mp_units::is_scalar<T>
constexpr bool mp_units::is_vector<T> = true;
```

which says that every type that can be used as a scalar representation is also allowed for vector
quantities.

Doing the above is actually not such a big "hack" as the ISO 80000 explicitly allows it:


!!! quote "ISO 80000-2"

    A vector is a tensor of the first order and a scalar is a tensor of order zero.

Despite it being allowed by ISO 80000, for type-safety reasons, we do not allow such a behavior
by default, and a user has to opt into such scenarios explicitly.
