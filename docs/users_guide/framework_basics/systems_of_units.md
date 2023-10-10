# Systems of Units

Modeling a [system of units](../../appendix/glossary.md#system-of-units) is probably
the most important feature and a selling point of every physical units library.
Thanks to that, the library can protect users from performing invalid operations on
quantities and provide automated conversion factors between various compatible units.

Probably all the libraries in the wild model the [SI](../../appendix/glossary.md#si)
and many of them provide support for additional units belonging to various other systems
(e.g. imperial).


## Systems of Units are based on Systems of Quantities

[Systems of quantities](../../appendix/glossary.md#system-of-quantities) specify a set
of quantities and equations relating to those quantities. Those equations do not take any
unit or a numerical representation into account at all. In order to create a quantity,
we need to add those missing pieces of information. This is where
a [system of units](../../appendix/glossary.md#system-of-units) kicks in.

The [SI](../../appendix/glossary.md#si) is explicitly stated to be based on
the [ISQ](../../appendix/glossary.md#isq). Among others, it defines
`7` [base units](../../appendix/glossary.md#base-unit), one for each
[base quantity](../../appendix/glossary.md#base-quantity). In the **mp-units**
this is expressed by associating a quantity kind (that we discussed in detail in the
previous chapter) with a unit that is used to express it:

```cpp
inline constexpr struct metre : named_unit<"m", kind_of<isq::length>> {} metre;
```

!!! important

    The `kind_of<isq::length>` above states explicitly that this unit has
    an associated quantity kind. In other words, `si::metre` (and scaled units based
    on it) can be used to express the amount of any quantity of kind length.


## Units compose

One of the strongest points of the [SI](../../appendix/glossary.md#si) system
is that its units compose. This allows providing thousands of different units for
hundreds of various quantities with a really small set of predefined units
and prefixes.

The same is modeled in the **mp-units** library, which also allows composing
predefined units to create a nearly infinite number of different
[derived units](../../appendix/glossary.md#derived-unit). For example, one can write:

```cpp
quantity<si::metre / si::second> q;
```

to express a quantity of speed. The resulting quantity type is implicitly inferred
from the [unit equation](../../appendix/glossary.md#unit-equation) by repeating
exactly the same operations on the associated quantity kinds.


## Many shades of the same unit

The [SI](../../appendix/glossary.md#si) provides the names for 22 common
[coherent units](../../appendix/glossary.md#coherent-derived-unit) of 22
[derived quantities](../../appendix/glossary.md#derived-quantity).

Each such named [derived unit](../../appendix/glossary.md#derived-unit) is a result
of a specific predefined [unit equation](../../appendix/glossary.md#unit-equation).
For example, a unit of power quantity is defined in the library as:

```cpp
inline constexpr struct watt : named_unit<"W", joule / second> {} watt;
```

However, a power quantity can be expressed in other units as well. For example,
the following:

```cpp
auto q1 = 42 * W;
std::cout << q1 << "\n";
std::cout << q1.in(J / s) << "\n";
std::cout << q1.in(N * m / s) << "\n";
std::cout << q1.in(kg * m2 / s3) << "\n";
```

prints:

```text
42 W
42 J/s
42 N m/s
42 kg m²/s³
```

All of the above quantities are equivalent and mean exactly the same.


## Constraining a derived unit to work only with a specific derived quantity

Some derived units are valid only for specific derived quantities. For example,
SI specifies both `hertz` and `becquerel` derived units with the same unit equation `1 / s`.
However, it also explicitly states:

!!! quote "SI Brochure"

    The hertz shall only be used for periodic phenomena and the becquerel shall only be used for
    stochastic processes in activity referred to a radionuclide.

The library allows constraining such units in the following way:

```cpp
inline constexpr struct hertz : named_unit<"Hz", one / second, kind_of<isq::frequency>> {} hertz;
inline constexpr struct becquerel : named_unit<"Bq", one / second, kind_of<isq::activity>> {} becquerel;
```

With the above, `hertz` can only be used for frequencies while becquerel should only be used for
quantities of activity. This means that the following equation will not compile:

```cpp
auto q = 1 * Hz + 1 * Bq;   // Fails to compile
```

This is exactly what we wanted to achieve to improve the type-safety of the library.


## Prefixed units

Besides named units, the [SI](../../appendix/glossary.md#si) specifies also 24 prefixes
(all being a power of `10`) that can be prepended to all named units to obtain various scaled
versions of them.

Implementation of `std::ratio` provided by all major compilers is able to express only
16 of them. This is why, in the **mp-units**, we had to find an alternative way to represent
unit magnitude in a more flexible way.

Each prefix is implemented as:

```cpp
template<PrefixableUnit auto U> struct quecto_ : prefixed_unit<"q", mag_power<10, -30>, U> {};
template<PrefixableUnit auto U> inline constexpr quecto_<U> quecto;
```

and then a [PrefixableUnit](basic_concepts.md#PrefixableUnit) can be prefixed in the following
way:

```cpp
inline constexpr auto qm = quecto<metre>;
```

The usage of `mag_power` not only enables providing support for SI prefixes but it can also
efficiently represent any rational magnitude. For example, IEC 80000 prefixes used in the
IT industry can be implemented as:

```cpp
template<PrefixableUnit auto U> struct yobi_ : prefixed_unit<"Yi", mag_power<2, 80>, U> {};
template<PrefixableUnit auto U> inline constexpr yobi_<U> yobi;
```

## Scaled units

In the [SI](../../appendix/glossary.md#si), all units are either base or derived units or prefixed
versions of those. However, those are not the only options possible.

For example, there is a list of [off-system units](../../appendix/glossary.md#off-system-unit)
accepted for use with SI. All of those are scaled versions of the SI units with ratios that can't
be explicitly expressed with predefined SI prefixes. Those include units like minute, hour, or
electronvolt:

```cpp
inline constexpr struct minute : named_unit<"min", mag<60> * si::second> {} minute;
inline constexpr struct hour : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct electronvolt : named_unit<"eV", mag<ratio{1'602'176'634, 1'000'000'000}> * mag_power<10, -19> * si::joule> {} electronvolt;
```

Also, units of other [systems of units](../../appendix/glossary.md#system-of-units) are often defined
in terms of scaled versions of the SI units. For example, the international yard is defined as:

```cpp
inline constexpr struct yard : named_unit<"yd", mag<ratio{9'144, 10'000}> * si::metre> {} yard;
```

For some units, a magnitude might also be irrational. The best example here is a `degree` which
is defined using a floating-point magnitude having a factor of the number π (Pi):

```cpp
inline constexpr struct mag_pi : magnitude<std::numbers::pi_v<long double>> {} mag_pi;
```

```cpp
inline constexpr struct degree : named_unit<basic_symbol_text{"°", "deg"}, mag_pi / mag<180> * si::radian> {} degree;
```
