# Value Conversions

## Value-preserving conversions

```cpp
auto q1 = 5 * km;
std::cout << q1.in(m) << '\n';
quantity<si::metre, int> q2 = q1;
```

The second line above converts the current quantity to the one expressed in metres and prints its
contents. The third line converts the quantity expressed in kilometres into the one measured
in metres.

In case a user would like to perform an opposite transformation:

```cpp
auto q1 = 5 * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1;
```

Both conversions will fail to compile.

There are two ways to make the above work. The first solution is to use a floating-point
representation type:

```cpp
auto q1 = 5. * m;
std::cout << q1.in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;
```

or

```cpp
auto q1 = 5 * m;
std::cout << value_cast<double>(q1).in(km) << '\n';
quantity<si::kilo<si::metre>> q2 = q1;  // double by default
```

!!! important

    The **mp-units** library follows [`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration)
    logic and treats floating-point types as value-preserving.


## Value-truncating conversions

The second solution is to force a truncating conversion:

```cpp
auto q1 = 5 * m;
std::cout << value_cast<km>(q1) << '\n';
quantity<si::kilo<si::metre>, int> q2 = q1.force_in(km);
```

This explicit cast makes it clear that something unsafe is going on. It is easy to spot in code
reviews or while chasing a bug in the source code.

!!! note

    `q.force_in(U)` is just a shortcut to run `value_cast<U>(q)`. There is no difference in behavior
    between those two interfaces. `q.force_in(U)` was added for consistency with `q.in(U)` and
    `q.force_numerical_value_in(U)`.

Another place where this cast is useful is when a user wants to convert a quantity with
a floating-point representation to the one using an integral one. Again this is a truncating
conversion, so an explicit cast is needed:

```cpp
quantity<si::metre, int> q3 = value_cast<int>(3.14 * m);
```

!!! info

    It is often fine to use an integral as a representation type, but in general, floating-point
    types provide better precision and are privileged in the library as they are considered
    to be value-preserving.
