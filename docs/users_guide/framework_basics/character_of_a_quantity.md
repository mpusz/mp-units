# Character of a Quantity

!!! warning

    Features described in this chapter are experimental and are subject to change or removal. Please
    share your feedback if something doesn't seem right or could be improved.


## Scalars, vectors, and tensors

!!! quote "ISO 80000-2"

    Scalars, vectors and tensors are mathematical objects that can be used to denote certain physical
    quantities and their values. They are as such independent of the particular choice of a coordinate system,
    whereas each scalar component of a vector or a tensor and each component vector and component
    tensor depend on that choice.

While defining quantities ISO 80000 explicitly mentions when a specific quantity has a vector or tensor
character. Such distinction is important because each quantity character represent different properties
and allows different operations to be done on their quantities.

For example, imagine a physical units library that allows to create a `speed` quantity from both
`length / time` and `length * time`. It wouldn't be too safe to use such a product, right?

Now we have to realize that both of the above operations (multiplication and division) are even not
mathematically defined for linear algebra types such as vector or tensors. On the other hand, two vectors
can be passed as arguments to dot (`⋅`) and cross (`×`) product operations. The result of the first one is
a scalar. The second one results with a vector that is perpendicular to both vectors passed as arguments.
Again, it wouldn't be safe to allow replacing those two operations with each other or expect the same
results from both cases. This simply can't work.


## Examples from the ISQ

To provide some examples for further discussion let's pick a few quantities defined in the ISO 80000:

| Quantity               |  Character   |                 Quantity Equation                 |
|------------------------|:------------:|:-------------------------------------------------:|
| `duration`             |    scalar    |                 _{base quantity}_                 |
| `mass`                 |    scalar    |                 _{base quantity}_                 |
| `length`               |    scalar    |                 _{base quantity}_                 |
| `path_length`          |    scalar    |                 _{base quantity}_                 |
| `radius`               |    scalar    |                 _{base quantity}_                 |
| `position_vector`      |  **vector**  |                 _{base quantity}_                 |
| `velocity`             |  **vector**  |           `position_vector / duration`            |
| `acceleration`         |  **vector**  |               `velocity / duration`               |
| `force`                |  **vector**  |               `mass * acceleration`               |
| `power`                |    scalar    |                `force ⋅ velocity`                 |
| `moment_of_force`      |  **vector**  |             `position_vector × force`             |
| `torque`               |    scalar    |         `moment_of_force ⋅ {unit-vector}`         |
| `surface_tension`      |    scalar    |                `|force| / length`                 |
| `angular_displacement` |    scalar    |              `path_length / radius`               |
| `angular_velocity`     |  **vector**  | `angular_displacement / duration * {unit-vector}` |
| `momentum`             |  **vector**  |                 `mass * velocity`                 |
| `angular_momentum`     |  **vector**  |           `position_vector × momentum`            |
| `moment_of_inertia`    | **_tensor_** |       `angular_momentum ⊗ angular_velocity`       |

!!! note

    As of now, all of the C++ physical units libraries on the market besides **mp-units** do not
    support above-mentioned operations. They expose only multiplication and division operators which
    do not work for proper linear-algebra-based representation types. In case one would like to
    construct the quantities provided in the above table with those libraries, this would result with
    a compile-time error stating that multiplication and division of two linear-algebra vectors is not
    possible.


## Characters apply to quantities but not dimensions or units

ISO 80000 explicitly states that dimensions are orthogonal to quantity characters:

!!! quote "ISO 80000-1:2009"

    In deriving the dimension of a quantity, no account is taken of its scalar, vector, or tensor character.

Also, it explicitly states that:

!!! quote "ISO 80000-2"

    All units are scalars.

## Defining vector and tensor quantities

To specify that a specific quantity has a vector or tensor character a value of `quantity_character`
enumeration can be appended to the `quantity_spec` describing such a quantity type:

```cpp
inline constexpr struct position_vector : quantity_spec<length, quantity_character::vector> {} position_vector;
inline constexpr struct displacement : quantity_spec<length, quantity_character::vector> {} displacement;
```

From now on all the quantities derived from `position_vector` or `displacement` will have a correct
character consistent with the operations performed in the [quantity equation](../../../appendix/glossary/#quantity-equation)
on their arguments.


## Representation types for vector and tensor quantities

!!! note

    The current version of the C++ Standard Library does not provide any types that could be used as
    a representation type for vector and tensor quantities. This is why users are on their own here
    :worried:.

    To provide examples and implement unit tests our library uses the types proposed in the [P1385](https://wg21.link/p1385)
    and available as [a Conan package in the Conan Center](https://conan.io/center/wg21-linear_algebra).

In order to enable the usage of a user-defined type as a representation type for vector or tensor
quantities you need to provide a partial specialization of `is_vector` or `is_tensor` customization
points.

For example, here is how it can be done for the [P1385](https://wg21.link/p1385) types:

```cpp
#include <matrix>

using la_vector = STD_LA::fixed_size_column_vector<double, 3>;

template<>
inline constexpr bool mp_units::is_vector<la_vector> = true;
```


## Hacking the character

Sometimes you want to use a vector quantity but you do not care about its direction. For example,
the standard gravity acceleration constant is always pointing down and you might not care about this
in a particular case. In such a case you may want to "hack" the library to allow scalar types
to be used as a representation type for scalar quantities.

For example, you can do something like this:

```cpp
template<class T>
  requires mp_units::is_scalar<T>
inline constexpr bool mp_units::is_vector<T> = true;
```

which says that every type that can be used a scalar representation is also allowed for vector
quantities.

Doing the above is actually not such a big "hack" as the ISO 80000 explicitly allows it:


!!! quote "ISO 80000-2"

    A vector is a tensor of the first order and a scalar is a tensor of order zero.

However, for type-safety reasons, we do not want to allow of such a behavior by default.


## Different shades of vector and tensor quantities



    Instead of treating each coordinate of a vector as a physical quantity value (i.e. a number multiplied by
    a unit), the vector could be written as a numerical vector multiplied by a unit. 
    The same considerations apply to tensors of second and higher orders.
