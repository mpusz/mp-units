---
draft: true
date: 2024-10-14
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# International System of Quantities (ISQ): Part 2 - Problems when ISQ is not used

This article is the next one in our series about the ISQ. After introducing the basic terms and
systems, in this article, we will talk about the benefits we get from modeling it in our library.

<!-- more -->

!!! note

    The issues described in this article do not apply to the **mp-units** library. Its interfaces,
    even if when we decide only to use [simple quantities](../../users_guide/framework_basics/simple_and_typed_quantities.md)
    that only use units, those are still backed up by quantity kinds under the framework's hood._

## Articles from this series

Previous:

- [Part 1 - Introduction](isq-part-1-introduction.md)


## Limitations of units-only solutions

Units-only is not a good design for a quantities and units library. It works to some extent, but
plenty of use cases can't be addressed, and for those that somehow work, we miss important safety improvements provided by additional abstractions in this article.

### No way to specify a quantity type in generic interfaces

A common requirement in the domain is to write unit-agnostic generic interfaces. For example,
let's try to implement a generic `avg_speed` function template that takes a quantity of any
unit and produces the result. So if we call it with _distance_ in `km` and _time_ in `h`, we will
get `km/h` as a result, but if we call it with `mi` and `h`, we expect `mi/h` to be returned.

```cpp
template<Unit auto U1, typename Rep1, Unit auto U2, typename Rep2>
auto avg_speed(quantity<U1, Rep1> distance, quantity<U2, Rep2> time)
{
  return distance / time;
}

quantity speed = avg_speed(120 * km, 2 * h);
```

This function works but does not provide any type safety to the users. The function arguments
can be easily reordered on the call site. Also, we do not get any information about the
return type of the function or any safety measures to ensure that the function logic actually
returns a quantity of _speed_.

To improve safety, with a units-only library, we have to write the function in the following way:

```cpp
template<typename Rep1, typename Rep2>
quantity<si::metre / si::second, decltype(Rep1{} / Rep2{})> avg_speed(quantity<si::metre, Rep1> distance,
                                                                      quantity<si::second, Rep2> time)
{
  return distance / time;
}

avg_speed(120 * km, 2 * h).in(km / h);
```

Despite being safer, the above code decreased the performance because we always pay for the
conversion at the function's input and output.

We could try to provide concepts like `ScaledUnitOf<si::metre>` that will try to constrain
the arguments somehow, but it leads to even more problems with the unit definitions. For example,
are `Hz` and `Bq` just scaled versions of `1/s`? What about radian and steradian or a litre and
a cubic meter?

Moreover, in a good library, the above code should not compile. The reason for this is that
even though the conversion from `km` to `m` and from `h` to `s` is considered value-preserving,
it is not true in the opposite direction. When we try to convert the result stored in an
integral type from the unit of `m/s` to `km/h`, we will inevitably lose some data.


### Disjoint units of the same quantity type do not work

Sometimes, we need to define several units describing the same quantity but which do not convert
to each other. A typical example can be a currency use case. A user may want to define EURO and
USD as units of currency, but do not provide any predefined conversion factor and handle such
a conversion at runtime with custom logic (e.g., using an additional time point function argument).
In such a case, how can we specify that EURO and USD are quantities of the same type/dimension?


## Dimensions to the rescue?

To prevent the above issues, most of the libraries on the market introduce dimension abstraction.
Thanks to that, we could solve the first issue of the previous chapter with:

```cpp
QuantityOf<dim_speed> auto avg_speed(QuantityOf<dim_length> auto distance,
                                     QuantityOf<dim_time> auto time)
{
  return distance / time;
}
```

and the second one by specifying that both EURO and USD are units of `dim_currency`. This is
a significant improvement but still has some issues.


## Limitations of dimensions

Let's first look at the above solution again. A domain expert seeing this code will immediately
say there is no such thing as a speed dimension. The ISQ specifies only 7 dimensions with
unique symbols assigned, and the dimensions of all the ISQ quantities are created as a
vector product of those. For example, a quantity of _speed_ has a dimension of $L^1T^{-1}$.
So, to be physically correct, the above code should be rewritten as:

```cpp
QuantityOf<dim_length / dim_time> auto avg_speed(QuantityOf<dim_length> auto distance,
                                                 QuantityOf<dim_time> auto time)
{
  return distance / time;
}
```

Most of the libraries on the market ignore this fact and try to model distinct quantities through
their dimensions, giving a false sense of safety. A dimension is not enough to describe a quantity.
This has been known for a long time now. The ["Measurement Data (Archive Report)"](https://www.bkent.net/Doc/mdarchiv.pdf)
report from 1996 says explicitly:

!!! quote "[Measurement Data (Archive Report)](https://www.bkent.net/Doc/mdarchiv.pdf)"

    Dimensional analysis does not adequately model the semantics of measurement data.

In the following chapters, we will see a few use cases that can't be solved with an approach
that only relies on units or dimensions.

### SI units of quantities of the same dimension but different kinds

The SI provides several units for distinct quantities of the same dimension but different kinds.
For example:

- hertz (Hz) is a unit of _frequency_ and becquerel (Bq) is a unit of _activity_.
  Both are defined as $s^{-1}$, and have the same dimension of $T^{-1}$.
- gray (Gy) is a unit of _absorbed dose_ and sievert (Sv) is a unit of _dose equivalent_.
  Both are defined as $m^2 s^{-2}$, and have the same dimension of $L^2T^{-2}$
- radian (rad) is a unit of _plane angle_ defined as $m/m$, and
  steradian (sr) is a unit of _solid angle_ defined as $m^2/m^2$.
  Both are quantities of dimension one, which also has its own units like one (1) and percent (%).

There are many more similar examples in the ISO 80000 series. For example, _storage capacity_
quantity can be measured in units of one, bit, octet, and byte.

The above conflicts can't be solved with dimensions, and they yield many safety issues. For example,
we can ask ourselves what should be the result of the following:

1. `quantity q = 1 * Hz + 1 * Bq;`
2. `quantity<Gy> q = 42 * Sv;`
3. `bool b = (1 * rad + 1 * bit) == 2 * sr;`

None of the above code should compile, but most of the libraries on the market happily accept it
and provide meaningless results. Some of them decide not to define one or more of the above
units at all to avoid potential safety issues. For example,
[the Au library does not define Sv to avoid mixing it up with Gy](https://github.com/aurora-opensource/au/pull/157).

### Derived quantities of the same dimension but different kinds

Even if some quantities do not have a specially assigned unit, they may still have a totally
different physical meaning even if they share the same dimension:

- _work_ vs. _moment of force_ both of the same dimension $L^2MT^{-2}$,
- _fuel consumption_ expressed in $\frac{l}{100\;km}$ vs. _area_ expressed in $m^2$ both of the same
  dimension $L^2$.

Again, we don't want to accidentally mix those.

### Various quantities of the same dimension and kinds

Even if we somehow address all the above, there are still plenty of use cases that still can't be
safely implemented with such abstractions.

Let's consider that we want to implement a freight transport application to position cargo in the
container. In such a scenario, we need to be able to discriminate between _length_, _width_, and
_height_ of the package. Also, often, we can find a "This side up" arrow on the box.

A similar but also really important use case is in aviation. The current _altitude_ is a totally
different quantity than the _distance_ to the destination. The same is true for _forward speed_
and _sink rate_. We do not want to accidentally mix those.

When we deal with _energy_, we should be able to implicitly construct it from a proper product of
any _mass_, _length_, and _time_. However, when we want to calculate _gravitational potential energy_,
we may not want it to be implicitly initialized from any expression of matching dimensions.
Such an implicit construction should be allowed only if we multiply a _mass_ with
_acceleration of free fall_ and _height_. All other conversions should have an explicit annotation
to make it clear that something potentially unsafe is being done in the code. Also, we should not
be able to assign a _potential energy_ to a quantity of _kinetic energy_. However, both of them
(possibly accumulated with each other) should be convertible to a _mechanical energy_ quantity.

Yet another example comes from the audio industry. In the audio software, we want to treat specific
counts (e.g., _beats_, _samples_) as separate quantities. We could assign dedicated base dimensions
to them. However, if we divide them by _duration_, we should obtain a quantity convertible to
_frequency_ and even be able to express the result in a unit of `Hz`. With the dedicated dimensions
approach, this wouldn't work as the dimension of frequency is just $T^{-1}$, which would not match
the results of our dimensional equations. This is why we can't assign dedicated dimensions to such
counts.

The last example that we want to mention here comes from finance. This time, we need to model _volume_
as a special quantity of _currency_. _volume_ can be obtained by multiplying _currency_ by the
dimensionless _market quantity_. Of course, both _currency_ and _volume_ should be expressed in
the same units (e.g., USD).

None of the above scenarios can be addressed with just units and dimensions. We need a better
abstraction to safely implement them.

## To be continued...

In the next part of this series, we will introduce the main ideas behind the International
System of Quantities and provide solutions to the problems described above.
