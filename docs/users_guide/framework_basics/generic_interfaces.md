# Generic Interfaces

Using a concrete unit in the interface often has a lot of sense. It is especially useful if we
store the data internally in the object. In such a case, we have to select a specific unit anyway.

For example, let's consider a simple storage tank:

```cpp
class StorageTank {
  quantity<horizontal_area[m2]> base_;
  quantity<isq::height[m]> height_;
  quantity<isq::mass_density[kg / m3]> density_ = air_density;
public:
  constexpr StorageTank(const quantity<horizontal_area[m2]>& base, const quantity<isq::height[m]>& height) :
      base_(base), height_(height)
  {
  }

  // ...
};
```

As the quantities provided in the function's interface are then stored in the class, there is probably
no sense in using generic interfaces here.


## The issues with unit-specific interfaces

However, in many cases, using a specific unit in the interface is counterproductive. Let's consider
the following function:

```cpp
quantity<isq::speed[km / h]> avg_speed(quantity<isq::length[km]> distance,
                                       quantity<isq::time[h]> duration)
{
  return distance / duration;
}
```

Everything seems fine for now. It also works great if we call it with:

```cpp
quantity<isq::speed[km / h]> s1 = avg_speed(220 * km, 2 * h);
```

However, if the user starts doing the following:

```cpp
quantity<isq::speed[mi / h]> s2 = avg_speed(140 * mi, 2 * h);
quantity<isq::speed[m / s]> s3 = avg_speed(20 * m, 2 * s);
```

some issues start to be clearly visible:

1. The arguments must be converted to units mandated by the function's parameters at each call.
   This involves potentially expensive multiplication/division operations at runtime.
2. After the function returns the speed in a unit of `km/h`, another potentially expensive
   multiplication/division operations have to be performed to convert the resulting quantity into
   a unit being the derived unit of the initial function's arguments.
3. Besides the obvious runtime cost, some unit conversions may result in a data truncation which
   means that the result will not be exactly equal to a direct division of the function's arguments.
4. We have to use a floating-point representation type (the `quantity` class template by default uses
   `double` as a representation type) which is considered
   [value preserving](../value_conversions/#value-preserving-conversions).
   Trying to use an integral type in this scenario will work only for `s1`, while `s2` and `s3`
   will fail to compile. Failing to compile is a good thing here as the library tries to prevent
   the user from doing a clearly wrong thing. To make the code compile, the user needs to use
   a dedicated [`value_cast`](../value_conversions/#value-truncating-conversions) like this:

    ```cpp
    quantity<isq::speed[mi / h]> s2 = avg_speed(value_cast<km>(140 * mi), 2 * h);
    quantity<isq::speed[m / s]> s3 = avg_speed(value_cast<km>(20 * m), value_cast<h>(2 * s));
    ```

    but the above will obviously provide an incorrect behavior (i.e. division by `0` in the evaluation
    of `s3`).


## A naive solution

A naive solution here would be to implement the function as an unconstrained function template:

```cpp
auto avg_speed(auto distance, auto duration)
{
  return distance / duration;
}
```

Beware that there are better solutions than this. The above code is too generic. Such a function template
accepts everything:

- quantities of other types
    - the compiler will not prevent accidental reordering of the function's arguments
    - quantities of different types can be passed as well
- plain `double` arguments
- `std::vector` and `std::lock_guard` will be accepted as well (of course, this will fail in the
  function's body later in the compilation process)


!!! note

    The usage of `auto` instead of a function parameter type is a C++20 feature. It makes such
    a code a function template where the type of such a parameter will be deduced during
    the template instantiation process from the argument type passed by the user.


## Constraining function template arguments with concepts

Much better generic code can be implemented using [basic concepts](../basic_concepts)
provided with the library:

```cpp
auto avg_speed(QuantityOf<isq::length> auto distance,
               QuantityOf<isq::time> auto duration)
{
  return isq::speed(distance / duration);
}
```

This explicitly states that the arguments passed by the user must not only satisfy
a [`Quantity`](../basic_concepts/#quantity) concept but also their quantity specification must
be implicitly convertible to `isq::length` and `isq::time` accordingly. This no longer leaves
room for error while still allowing the compiler to generate the most efficient code.

!!! tip

    Please note that now it is safe just to use integral types all the way which again
    improves the runtime performance as the multiplication/division operations are often
    faster on integral rather than floating-point types.


## Constraining function template return type

The above function template resolves all of the [issues described before](#the-issues-with-unit-specific-interfaces).
However, we can do even better here by additionally constraining the return type:

```cpp
QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto distance,
                                      QuantityOf<isq::time> auto duration)
{
  return isq::speed(distance / duration);
}
```

Doing so has two important benefits:

1. It informs the users of our interface about what to expect to be the result of a function
   invocation. It is superior to just returning `auto`, which does not provide any hint about
   the thing being returned there.
2. Such a concept constrains the type returned from the function. This means that it works as
   a unit test to verify if our function actually performs what it is supposed to do. If there is
   an error in [quantity equations](../../appendix/glossary/#quantity-equation), we will learn
   about it right away.


## Constraining a variable on the stack

If we know exactly what the function does in its internals and if we know the exact argument types
passed to such a function, we often know the exact type that will be returned from its invocation.

However, if we care about performance, we should often use the generic interfaces described in this
chapter. A side effect is that we sometimes are unsure about the return type. Even if we know it
today, it might change a week from now due to some code refactoring.

In such cases, we can again use `auto` to denote the type:

```cpp
auto s1 = avg_speed(220 * km, 2 * h);
auto s2 = avg_speed(140 * mi, 2 * h);
auto s3 = avg_speed(20 * m, 2 * s);
```

In this case, it is probably OK to do so as the `avg_speed` function name explicitly provides
the information on what to expect as a result.

In other scenarios where the returned quantity type is not so obvious, it is again helpful to
constrain the type with a concept like so:

```cpp
QuantityOf<isq::speed> auto s1 = avg_speed(220 * km, 2 * h);
QuantityOf<isq::speed> auto s2 = avg_speed(140 * mi, 2 * h);
QuantityOf<isq::speed> auto s3 = avg_speed(20 * m, 2 * s);
```

Again this explicitly provides additional information about the quantity we are dealing with in
the code, and it serves as a unit test checking if the "thing" returned from a function is actually
what we expected here.


!!! note

    The `QuantityOf` and `QuantityPointOf` concepts are probably the most useful, but there
    are a few more to play with. A list of all the concepts can be found in
    [the "Basic Concepts" chapter](../basic_concepts).
