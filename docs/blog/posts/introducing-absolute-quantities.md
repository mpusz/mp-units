---
date: 2025-06-16
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Introducing absolute quantities

This post introduces a new abstraction called an absolute quantity. It complements affine
space abstractions (point and delta) and will most probably be a new default in the library
when we release V3.

<!-- more -->

## Affine space in a nutshell

So far **mp-units** and other quantities and units libraries have been modeling two kinds of
abstractions:

1. Points (modeled with `quantity_point` class template)

    - can be interpolated,
    - can be subtracted,
    - can't be added,
    - can't be multiplied or divided by another quantity or scalar,
    - some of them could be specified as non-negative,
    - specified relative to some absolute or relative point origin,
    - conversion to offset units (e.g., degree Celsius) accounts for offset.

2. Deltas (modeled with `quantity` class template)

    - can be interpolated,
    - can be subtracted,
    - can be added,
    - can be multiplied and divided by another quantity and scalar,
    - may be negative as there is always a chance that we will subtract a larger value
      from a smaller one,
    - not specified relative to any origin,
    - conversion to offset units (e.g., degree Celsius) ignores the offset (just the conversion
      factor is used).

!!! info

    More information on this subject can be found in
    [the Affine Space chapter](../../users_guide/framework_basics/the_affine_space.md).


## Issues

A current affine space implementation works well for many essential use cases. However,
it also produces some issues.

1. We can't print quantity points as, at least for today, we do not have the means to properly
   describe the user-provided origin in the text output.
2. Quantity points are hard to use in physical equations to denote not-delta values.

To make both of the above work, a user needs to convert the quantity point to quantity with
either `qp.quantity_from_zero()` or `qp.quantity_from(some_origin)` member functions.

```cpp
quantity_point m1(2 * kg);
quantity_point m2(3 * kg);
quantity_point m = m1 + m2.quantity_from_zero();
quantity d_v = 30 * km / h;
quantity E_k = m.quantity_from_zero() * pow<2>(d_v) / 2;

std::cout << "Mass: " << m.quantity_from_zero() << "\n";
std::cout << "Velocity: " << d_v << "\n";
std::cout << "Kinetic energy: " << E_k.in<double>(J) << "\n";
```

This is quite inconvenient and is a common reason for avoiding quantity point abstraction
in many equations in source code where it would be a good fit otherwise.


## Absolute quantities

Despite the above drawbacks, affine space points are necessary to model some abstractions
(e.g., temperatures in degrees Celsius, tared mass measurements, altitudes above some
reference point, etc.) and do it really well. Constrained affine space arithmetic
(e.g., preventing accidental addition of points) also improves the safety of our programs.
This is why it is a very valuable abstraction and should be used even more often than now.

To improve the user experience and open the doors for new features in the future, we are
considering adding a third abstraction for absolute quantities. In terms of properties,
an absolute quantity will lie between points and deltas.

| Feature                        |         Point          |         Absolute         |         Delta          |
|--------------------------------|:----------------------:|:------------------------:|:----------------------:|
| **Interpolation**              |   :white_check_mark:   |    :white_check_mark:    |   :white_check_mark:   |
| **Subtraction**                |   :white_check_mark:   |    :white_check_mark:    |   :white_check_mark:   |
| **Addition**                   | :material-close-thick: |    :white_check_mark:    |   :white_check_mark:   |
| **Multiply/Divide**            | :material-close-thick: |    :white_check_mark:    |   :white_check_mark:   |
| **May be non-negative**        |   :white_check_mark:   |    :white_check_mark:    | :material-close-thick: |
| **Relative to origin**         |  Absolute & relative   | Absolute (implicit only) | :material-close-thick: |
| **Can use offset units**       |   :white_check_mark:   |  :material-close-thick:  |   :white_check_mark:   |
| **Conversion to offset units** |      With offset       |  :material-close-thick:  |       No offset        |
| **Text output**                | :material-close-thick: |    :white_check_mark:    |   :white_check_mark:   |

As we can see above, absolute quantities have only two limitations, and both are connected
to the offset units' usage. They can't use those because they must remain absolute
instead of being measured relative to some custom origin.

It is also vital to notice that there will be no way to provide a custom origin for
absolute quantities, even if it is defined in terms of `absolute_point_origin`. Those
quantities are meant to model abstractions with well-established and unambiguous zero
origins. If we allow passing absolute point origins, we could define two quantities
of the same type measured according to different not-related origins, which would be
too confusing.

The above also allows us to print them, as we do not need any special text to describe
their origin.


## Interfaces refactoring

As I mentioned in my [previous post](bringing-quantity-safety-to-the-next-level.md#should-we-get-rid-of-a-quantity_point),
we are seriously considering removing `quantity_point` class template and replacing it with
a `quantity_spec` point wrapper. For example, `quantity_point<isq::altitude[m]>` will become
`quantity<point<isq::altitude[m]>>`.

I initially planned `quantity<isq::mass>` to be the same as `quantity<delta<isq::mass>>`,
but it turns out that deltas probably should not be the default. It is consistent with how we
write physical expressions on paper, right? Delta symbol (∆) is always "verbose"
in our equations, it would be nice for the C++ code to do the same. So, deltas will always
need to be explicit.

And this brings us to absolute quantities. They should actually be the default we are looking
for. This is what we write as quantities in most of the physical equations. This is why we
will not need any specifier to denote them.

For example:

```cpp
inline constexpr struct tare final : relative_point_origin<quantity_point{2 * kg}> {} tare;

quantity<point<isq::mass>> m1(10 * kg);          // point quantity with an implicit point origin
quantity<point<isq::mass>> m2 = tare + 8 * kg;   // point quantity with an explicit relative point origin
quantity<isq::mass>        m3 = 15 * kg;         // absolute quantity (e.g., non-negative)
quantity<delta<isq::mass>> m13 = m1 - m3;        // delta quantity (e.g., may be negative)
quantity<delta<isq::mass>> m23 = m2 - m3;        // delta quantity (e.g., may be negative)
```

With the above, the previous examples may be refactored to:

```cpp
quantity m1 = 2 * kg;
quantity m2 = 3 * kg;
quantity m = m1 + m2;
quantity d_v = delta<km / h>(30);
quantity E_k = m * pow<2>(d_v) / 2;

std::cout << "Mass: " << m << "\n";
std::cout << "Velocity: " << d_v << "\n";
std::cout << "Kinetic energy: " << E_k.in<double>(J) << "\n";
```


## New opportunities

The new syntax simplifies API as one `quantity` class template will now serve all quantity
variations (possibly even more in the future). It also allows us to model quantities that
were impossible to express before without some workarounds.

For example, we can now correctly calculate Carnot engine efficiency with any of the following:

```cpp
quantity temp_cold = 300. * K;
quantity temp_hot = 500. * K;
quantity carnot_eff_1 = 1. - temp_cold / temp_hot;
quantity carnot_eff_2 = (temp_hot - temp_cold) / temp_hot;
```

In the above code, we can easily create absolute or delta values of temperatures and do
arithmetics on them. Previously, we had to create deltas from both points artificially
with:

```cpp
quantity temp_cold = point<K>(300.);
quantity temp_hot = point<K>(500.);
quantity carnot_eff_1 = 1. - temp_cold.quantity_from_zero() / temp_hot.quantity_from_zero();
quantity carnot_eff_2 = (temp_hot - temp_cold) / temp_hot.quantity_from_zero();
```

It worked but was far from being physically pure and pretty.


## Summary

We believe that adding absolute quantities will be a major improvement in the library that
will allow us to more correctly model physical equations making them terser and easier to
understand at the same time.

We plan to deliver the features mentioned in this post as a part of **mp-units** V3.

Please share your feedback.
