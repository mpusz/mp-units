# Quantity Arithmetics

## `quantity` is a numeric wrapper

If we think about it, the `quantity` class template is just a "smart" numeric wrapper. It exposes
properly constrained set of arithmetic operations on one or two operands.

!!! important "Important: `quantity` propagates the underlying interface"

    Every single arithmetic operator is exposed by the `quantity` class template only if
    the underlying representation type provides it as well and its implementation has proper
    semantics (e.g. returns a reasonable type).

For example, in the following code, `-a` will compile only if `MyInt` exposes such an operation
as well:

```cpp
quantity a = MyInt{42} * m;
quantity b = -a;
```

Assuming that:

- `q` is our quantity,
- `qq` is a quantity implicitly convertible to `q`,
- `q2` is any other quantity,
- `kind` is a [quantity of the same kind](systems_of_quantities.md#quantities-of-the-same-kind) as `q`,
- `one` is a [quantity of `dimension_one` with the unit `one`](dimensionless_quantities.md),
- `number` is a value of a type "compatible" with `q`'s representation type,

here is the list of all the supported operators:

- unary:
    - `+q`
    - `-q`
    - `++q`
    - `q++`
    - `--q`
    - `q--`
- compound assignment:
    - `q += qq`
    - `q -= qq`
    - `q %= qq`
    - `q *= number`
    - `q *= one`
    - `q /= number`
    - `q /= one`
- binary:
    - `q + kind`
    - `q - kind`
    - `q % kind`
    - `q * q2`
    - `q * number`
    - `number * q`
    - `q / q2`
    - `q / number`
    - `number / q`
- ordering and comparison:
    - `q == kind`
    - `q <=> kind`

As we can see, there are plenty of operations one can do on a value of a `quantity` type. As most
of them are obvious, in the following chapters, we will discuss only the most important or non-trivial
aspects of quantity arithmetics.


## Addition and subtraction

Quantities can easily be added or subtracted from each other:

```cpp
static_assert(1 * m + 1 * m == 2 * m);
static_assert(2 * m - 1 * m == 1 * m);
static_assert(isq::height(1 * m) + isq::height(1 * m) == isq::height(2 * m));
static_assert(isq::height(2 * m) - isq::height(1 * m) == isq::height(1 * m));
```

The above uses the same types for LHS, RHS, and the result, but in general, we can add, subtract,
or compare the values of any quantity type as long as both
[quantities are of the same kind](systems_of_quantities.md#quantities-of-the-same-kind).
The result of such an operation will be the common type of the arguments:

```cpp
static_assert(1 * km + 1.5 * m == 1001.5 * m);
static_assert(isq::height(1 * m) + isq::width(1 * m) == isq::length(2 * m));
static_assert(isq::height(2 * m) - isq::distance(0.5 * m) == 1.5 * m);
static_assert(isq::radius(1 * m) - 0.5 * m == isq::radius(0.5 * m));
```

!!! note

    Please note that for the compound assignment operators, both arguments have to either be of
    the same type or the RHS has to be implicitly convertible to the LHS, as the type of
    LHS is always the result of such an operation:

    ```cpp
    static_assert((1 * m += 1 * km) == 1001 * m);
    static_assert((isq::height(1.5 * m) -= 1 * m) == isq::height(0.5 * m));
    ```

    If we break those rules, the following code will not compile:

    ```cpp
    static_assert((1 * m -= 0.5 * m) == 0.5 * m);                       // Compile-time error(1)
    static_assert((1 * km += 1 * m) == 1001 * m);                       // Compile-time error(2)
    static_assert((isq::height(1 * m) += isq::length(1 * m)) == 2 * m); // Compile-time error(3)
    ```

    1. Floating-point to integral representation type is [considered narrowing](value_conversions.md).
    2. Conversion of quantity with integral representation type from a unit of a higher resolution to the one
       with a lower resolution is [considered narrowing](value_conversions.md).
    3. Conversion from a more generic quantity type to a more specific one is
       [considered unsafe](simple_and_typed_quantities.md#quantity_cast-to-force-unsafe-conversions).


## Multiplication and division

Multiplying or dividing a quantity by a number does not change its quantity type or unit. However,
its representation type may change. For example:

```cpp
static_assert(isq::height(3 * m) * 0.5 == isq::height(1.5 * m));
```

!!! note

    Unless we use a compound assignment operator, in which case truncating operations are again not allowed:

    ```cpp
    static_assert((isq::height(3 * m) *= 0.5) == isq::height(1.5 * m)); // Compile-time error(1)
    ```

    1. Floating-point to integral representation type is [considered narrowing](value_conversions.md).

However, suppose we multiply or divide quantities of the same or different types, or we divide a raw
number by a quantity. In that case, we most probably will end up in a quantity of yet another type:

```cpp
static_assert(120 * km / (2 * h) == 60 * km / h);
static_assert(isq::width(2 * m) * isq::length(2 * m) == isq::area(4 * m2));
static_assert(50 / isq::time(1 * s) == isq::frequency(50 * Hz));
```

!!! note

    An exception from the above rule happens when one of the arguments is
    a [dimensionless quantity](dimensionless_quantities.md). If we multiply or divide by such
    a quantity, the quantity type will not change. If such a quantity has a unit `one`,
    also the unit of a quantity will not change:

    ```cpp
    static_assert(120 * m / (2 * one) == 60 * m);
    ```

An interesting special case happens when we divide the same quantity kinds or multiply a quantity
by its inverted type. In such a case, we end up with a [dimensionless quantity](dimensionless_quantities.md).

```cpp
static_assert(isq::height(4 * m) / isq::width(2 * m) == 2 * one); // (1)!
static_assert(5 * h / (120 * min) == 0 * one);  // (2)!
static_assert(5. * h / (120 * min) == 2.5 * one);
```

1. The resulting quantity type of the LHS is `isq::height / isq::width`, which is a quantity of the
dimensionless kind.
2. The resulting quantity of the LHS is `0 * dimensionless[h / min]`. To be consistent with the division
of different quantity types, we do not convert quantity values to a common unit before the division.

!!! important "Important: Beware of integral division"

    The physical units library can't do any runtime branching logic for the division operator.
    All logic has to be done at compile-time when the actual values are not known, and the quantity types
    can't change at runtime.

    If we expect `120 * km / (2 * h)` to return `60 km / h`, we have to agree with the fact that
    `5 * km / (24 * h)` returns `0 km/h`. We can't do a range check at runtime to dynamically adjust scales
    and types based on the values of provided function arguments.

    **This is why we often prefer floating-point representation types when dealing with units.**
    Some popular physical units libraries even
    [forbid integer division at all](https://aurora-opensource.github.io/au/main/troubleshooting/#integer-division-forbidden).


## Modulo

Now that we know how addition, subtraction, multiplication, and division work, it is time to talk about
modulo. What would we expect to be returned from the following quantity equation?

```cpp
auto q = 5 * h % (120 * min);
```

Most of us would probably expect to see `1 h` or `60 min` as a result. And this is where the problems start.

C++ language defines its `/` and `%` operators with the [quotient-remainder theorem](https://eel.is/c++draft/expr.mul#4):

```text
q = a / b;
r = a % b;
q * b + r == a;
```

The important property of the modulo operation is that it only works for integral representation
types (it is undefined what modulo for floating-point types means). However, as we saw in the previous
chapter, integral types are tricky because they often truncate the value.

From the quotient-remainder theorem, the result of modulo operation is `r = a - q * b`.
Let's see what we get from such a quantity equation on integral representation types:

```cpp
const quantity a = 5 * h;
const quantity b = 120 * min;
const quantity q = a / b;
const quantity r = a - q * b;

std::cout << "reminder: " << r << "\n";
```

The above code outputs:

```text
reminder: 5 h
```

And now, a tough question needs an answer. Do we really want modulo operation on physical units
to be consistent with the quotient-remainder theorem and return `5 h` for `5 * h % (120 * min)`?

This is exactly why we decided not to follow this hugely surprising path in the **mp-units** library.
The selected approach was also consistent with the feedback from the C++ experts. For example,
this is what Richard Smith said about this issue:

!!! quote "Richard Smith"

    I think the quotient-remainder property is a less important motivation here than other factors
    -- the constraints on `%` and `/` are quite different, so they lack the inherent connection they
    have for integers. In particular, I would expect that `A / B` works for all quantities `A` and `B`,
    whereas `A % B` is only meaningful when `A` and `B` have the same dimension. It seems like
    a nice-to-have for the property to apply in the case where both `/` and `%` are defined,
    but internal consistency of `/` across all cases seems much more important to me.

    I would expect `61 min % 1 h` to be `1 min`, and `1 h % 59 min` to also be `1 min`, so my
    intuition tells me that the result type of `A % B`, where `A` and `B` have the same dimension,
    should have the smaller unit of `A` and `B` (and if the smaller one doesn't divide
    the larger one, we should either use the `gcd / std::common_type` of the units of
    `A` and `B` or perhaps just produce an error). I think any other behavior for `%` is hard to
    defend.

    On the other hand, for division it seems to me that the choice of unit should probably not affect
    the result, and so if we want that `5 mm / 120 min = 0 mm/min`, then `5 h / 120 min == 0 hc`
    (where `hc` is a dimensionless "hexaconta", or `60x`, unit). I don't like the idea of taking
    SI base units into account; that seems arbitrary and like it would do the wrong thing as often
    as it does the right thing, especially when the units have a multiplier that is very large or
    small. We could special-case the situation of a dimensionless quantity, but that could lead to
    problematic overflow pretty easily: a calculation such as `10 s * 5 GHz * 2 uW` would overflow
    an `int` if it produces a dimensionless quantity for `10 s * 5 GHz`, but it could equally
    produce `50 G * 2 uW = 100 kW` without any overflow, and presumably would if the terms were merely
    reordered.

    If people want to use integer-valued quantities, I think it's fundamental that you need
    to know what the units of the result of an operation will be, and take that into account in how you
    express computations; the simplest rule for heterogeneous operators like `*` or `/` seems to be that
    the units of the result are determined by applying the operator to the units of the operands
    -- and for homogeneous operators like `+` or `%`, it seems like the only reasonable option is
    that you get the `std::common_type` of the units of the operands.

To summarize, the modulo operation on physical units has more in common with addition and
division operators than with the quotient-remainder theorem. To avoid surprising results, the
operation uses a common unit to do the calculation and provide its result:

```cpp
static_assert(5 * h / (120 * min) == 0 * one);
static_assert(5 * h % (120 * min) == 60 * min);
static_assert(61 * min % (1 * h) == 1 * min);
static_assert(1 * h % (59 * min) == 1 * min);
```


## Comparison against zero

In our code, we often want to compare the value of a quantity against zero. For example, we do it
every time when we want to ensure that we deal with a non-zero or positive value.

We could implement such checks in the following way:

```cpp
if (q1 / q2 != 0 * m / s)
  // ...
```

The above would work (assuming we are dealing with the quantity of speed), but could be suboptimal
if the result of `q1 / q2` is not expressed in `m / s`. To eliminate the need for conversion, we
need to write:

```cpp
if (auto q = q1 / q2; q != q.zero())
  // ...
```

but that is a bit inconvenient, and inexperienced users could be unaware of this technique
and its reasons.

For the above reasons, the library provides dedicated interfaces to compare against zero that
follow the naming convention of
[named comparison functions](https://en.cppreference.com/w/cpp/utility/compare/named_comparison_functions)
in the C++ Standard Library. The _mp-units/compare.h_ header file exposes the following functions:

- `is_eq_zero`
- `is_neq_zero`
- `is_lt_zero`
- `is_gt_zero`
- `is_lteq_zero`
- `is_gteq_zero`

Thanks to them, to save typing and not pay for unneeded conversions, our check could be implemented
as follows:

```cpp
if (is_neq_zero(q1 / q2))
  // ...
```

!!! tip

    Those functions will work with any type `T` that exposes `zero()` member function returning
    something comparable to `T`. Thanks to that, we can use them not only with quantities but also
    with [quantity points](the_affine_space.md#quantity_point),
    [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration) or any other type
    that exposes such an interface.


## Other maths

This chapter scopes only on the `quantity` type's operators. However, there are many named math
functions provided in the _mp-units/math.h_ header file. Among others, we can find there
the following:

- `pow()`, `sqrt()`, and `cbrt()`,
- `exp()`,
- `abs()`,
- `epsilon()`,
- `floor()`, `ceil()`, `round()`,
- `inverse()`,
- `hypot()`,
- `sin()`, `cos()`, `tan()`,
- `asin()`, `acos()`, `atan()`.

In the library, we can also find _mp-units/random.h_ header file with all the pseudo-random number
generators.
