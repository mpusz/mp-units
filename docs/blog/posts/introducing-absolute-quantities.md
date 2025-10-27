---
date: 2025-06-16
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Introducing absolute quantities

An absolute quantity is a quantity that represents a whole measurement of an entity (for
example: a _mass_, a _temperature_ on an absolute scale, or the total _height_ of an object).

Unlike an affine `point` (which is defined relative to a named origin) or a `delta`
(which represents a difference), an absolute quantity behaves like a delta for arithmetic
(you can add and multiply it) while conceptually denoting a whole value without a
user-specified origin. This makes many common physical expressions (summing _masses_,
computing _energies_) more natural and less verbose.

We are considering making absolute quantities the default quantity abstraction in
**mp-units** V3.

<!-- more -->

## Affine space in a nutshell

So far **mp-units** and other quantities and units libraries have been modeling two kinds
of abstractions:

1. Points (modeled with `quantity_point` class template in **mp-units** V2)

    - can be interpolated,
    - can be subtracted,
    - can't be added,
    - can't be multiplied or divided by another quantity or scalar,
    - some of them could be specified as non-negative,
    - specified relative to some absolute or relative point origin,
    - conversion to offset units (e.g., degree Celsius) accounts for offset.

2. Deltas (modeled with `quantity` class template in **mp-units** V2)

    - can be interpolated,
    - can be subtracted,
    - can be added,
    - can be multiplied and divided by another quantity and scalar,
    - may be negative as there is always a chance that we will subtract a larger value
      from a smaller one,
    - don't use point origins in its definition,
    - conversion to offset units (e.g., degree Celsius) ignores the offset (just the conversion
      factor is used).

!!! info

    More information on this subject can be found in
    [the Affine Space chapter](../../users_guide/framework_basics/the_affine_space.md).


## Issues

A current affine space implementation works well for many essential use cases. However,
it also produces some issues.

1. We can't print quantity points as, at least for today, we do not have the means to properly
   describe the user-provided origin in the text output. Also, it is often unclear if those
   should be printed against the current relative origin or the absolute one.
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
(e.g., _temperatures_ in degrees Celsius, tared _mass_ measurements, _altitudes_ above
some reference point, etc.) and do it really well. Constrained affine space arithmetic
(e.g., preventing accidental addition of points) also improves the safety of our programs.
This is why it is a valuable abstraction and should be used even more often than now.

To improve the user experience and open the doors for new features in the future, we are
considering adding a third abstraction for absolute quantities. In terms of properties,
an absolute quantity will lie between points and deltas.

| Feature                        |         Point          |                    Absolute                    |         Delta          |
|--------------------------------|:----------------------:|:----------------------------------------------:|:----------------------:|
| **Interpolation**              |   :white_check_mark:   |               :white_check_mark:               |   :white_check_mark:   |
| **Subtraction**                |   :white_check_mark:   |               :white_check_mark:               |   :white_check_mark:   |
| **Addition**                   | :material-close-thick: |               :white_check_mark:               |   :white_check_mark:   |
| **Multiply/Divide**            | :material-close-thick: |               :white_check_mark:               |   :white_check_mark:   |
| **May be non-negative**        |   :white_check_mark:   |               :white_check_mark:               | :material-close-thick: |
| **Relative to origin**         |  Absolute & relative   | No user-provided origin (implicit global zero) | :material-close-thick: |
| **Can use offset units**       |   :white_check_mark:   |             :material-close-thick:             |   :white_check_mark:   |
| **Conversion to offset units** |      With offset       |             :material-close-thick:             |       No offset        |
| **Text output**                | :material-close-thick: |               :white_check_mark:               |   :white_check_mark:   |

As we can see above, absolute quantities have only two limitations, and both are connected
to the use of offset units. They can't use those because they must remain absolute
instead of being measured relative to some custom origin.

Absolute quantities could be considered delta quantities that represent a whole
-- the entire entity being measured. This is why we can represent a _system mass_ by
adding absolute _masses_ of all system elements, or a _system energy_ by adding absolute
_temperatures_ of all the system elements.

As those are more related to deltas than points, it is impossible to specify their origin
points. This also allows us to print them, as we do not need any special text to describe
their origin, as they are always measured against nothing/void.


## Interfaces refactoring

As I mentioned in my [previous post](bringing-quantity-safety-to-the-next-level.md#should-we-get-rid-of-a-quantity_point),
we are seriously considering removing `quantity_point` class template and replacing it with
a `quantity_spec` point wrapper. For example, `quantity_point<isq::altitude[m]>` will become
`quantity<point<isq::altitude[m]>>`.

I initially planned `quantity<isq::mass>` to be the same as `quantity<delta<isq::mass>>`,
but it turns out that deltas probably should not be the default. It is consistent with how
we write physical expressions on paper, right? Delta symbol (∆) is always "verbose"
in physical equations, it would be nice for the C++ code to do the same. So, deltas will always
need to be explicit.

And this brings us to absolute quantities. They should actually be the default we are looking
for. This is what we write as quantities in most of the physical equations. This is why we
will not need any specifier to denote them.

For example:

```cpp
inline constexpr struct tare final : relative_point_origin<quantity_point{2 * kg}> {} tare;

quantity<point<isq::mass>>       m1(10 * kg);         // point quantity with an implicit point origin
quantity<point<isq::mass, tare>> m2 = tare + 8 * kg;  // point quantity with an explicit relative point origin
quantity<isq::mass>              m3 = 15 * kg;        // absolute quantity (e.g., non-negative)
quantity<delta<isq::mass>>       m13 = m1 - m3;       // delta quantity (e.g., may be negative)
quantity<delta<isq::mass>>       m23 = m2 - m3;       // delta quantity (e.g., may be negative)
```

With the above, the initial example may be refactored to:

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


## Arithmetic

Affine space arithmetic is well-defined. However, we are adding a new type to the library
that lands between points and deltas. This is why we must agree on the arithmetic for all
possible combinations.

### Addition

Absolute quantities are deltas against nothing so adding them to a point yields another point.

Adding a delta to them yields a delta, as a delta may represent only a part of something and
a whole, and a part is not a whole. The delta may also be negative and greater
than the absolute quantity, which may yield a negative value. This is why delta is a good
result here.

Only adding whole non-negative entities of the system yields a system being expressed as
an entire non-negative entity. This is why adding absolute quantities results in an absolute
quantity.

| Lhs \ Rhs    |         Point          | Absolute | Delta |
|--------------|:----------------------:|:--------:|:-----:|
| **Point**    | :material-close-thick: |  Point   | Point |
| **Absolute** |         Point          | Absolute | Delta |
| **Delta**    |         Point          |  Delta   | Delta |

### Subtraction

Similarly, during subtraction, regular affine space arithmetics for deltas apply.
Subtracting an absolute quantity from a point yields a point, and trying to do the opposite
does not make physical sense.

Subtracting two non-negative absolute quantities may yield a negative value if we subtract
a larger one from the smaller one, so the result should be a delta. A similar result
may be obtained by subtracting a delta from absolute quantity or absolute quantity from a delta.

| Lhs \ Rhs    |         Point          | Absolute | Delta |
|--------------|:----------------------:|:--------:|:-----:|
| **Point**    |         Delta          |  Point   | Point |
| **Absolute** | :material-close-thick: |  Delta   | Delta |
| **Delta**    | :material-close-thick: |  Delta   | Delta |

!!! info

    Based on the above assumptions, one of the lines of the below code can't compile:

    ```cpp
    quantity temp1 = 270 * K;
    quantity temp2 = point<K>(300);
    quantity temp3 = temp2 - temp1;             // Point
    // quantity temp4 = temp1 - temp2;          // Compile-time error
    quantity temp5 = temp1 - temp2.absolute();  // Delta
    ```


### Motivating example

Let's consider a room with a table and two glasses filled with fluid on top of it.
Let's also assume that we want to stack one on top of the other and treat them as a system
we observe.

![Absolute quantities](absolute-quantities.jpeg){ width="500" }

```cpp
// absolute quantities
quantity<isq::height[cm]> glass1_height = 20 * cm;
quantity<isq::height[cm]> glass2_height = 15 * cm;

// delta quantities
quantity<delta<isq::height>[cm]> fluid_level = 16 * cm;

// point quantities
inline constexpr struct floor_level final : absolute_point_origin<isq::height> floor_level;
quantity<point<isq::height[cm], floor_level>> table_top = floor_level + 1 * m;

// absolute results
quantity system_height = glass1_height + glass2_height;

// delta results
quantity empty_height_res = glass1_height - fluid_level;
quantity glass2_height_res = system_height - glass1_height;    // could result in an absolute quantity
assert(glass2_height_res == glass2_height);
quantity height_diff_res = glass2_height - glass1_height;      // but this one should definitely return delta

// point results
quantity<point<isq::height[cm], floor_level>> glass1_top = table_top + glass1_height;
quantity point1 = glass1_top - glass1_height;
assert(point1 == table_top);
// quantity point2 = glass1_height - glass1_top;               // no sense - does not compile
```


## What about time?

Everything looks promising and nice for now. But let's look closer into the quantity of time.
There is no way to measure its absolute value as we don't even know where (when?) the time
axis starts... Only time points and time deltas (durations) make sense.

The above raises a few questions:

1. Does it mean that `quantity<si::seconds>` or `quantity<isq::time[s]>` should not compile?
2. Should we require the user to always state `quantity<delta<si::seconds>>` or
   `quantity<delta<isq::time[s]>>`? This would be consistent with physical equations but more
   verbose in the source code.
3. Should the syntax `40 * s` be disallowed or should it implicitly create`quantity<delta<si::seconds>>`
   instead of `quantity<si::seconds>`?
4. A somehow similar case might be the length quantity as there is no one well-established
   zero origin that serves for all length measurements. However, asking the users always
   to provide a `delta` specifier for length would probably be an overkill.

As you can see, I do not yet have good answers to the above problems yet. Please feel welcome
to share some feedback on this.


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

We believe that adding absolute quantities will be a significant improvement in the library
that will allow us to model physical equations in a terser, more correct, easier to
write, understand, and maintain way.

We plan to deliver the features mentioned in this post as a part of **mp-units** V3.

Please share your feedback.
