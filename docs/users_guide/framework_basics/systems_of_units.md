# Systems of Units

Modeling a [system of units](../../appendix/glossary.md#system-of-units) is probably
the most important feature and a selling point of every physical units library.
Thanks to that, the library can protect users from performing invalid operations on
quantities and provide automated conversion factors between various compatible units.

Probably all the libraries in the wild model the [SI](../../appendix/glossary.md#si)
and many of them provide support for additional units belonging to various other systems
(e.g., imperial, cgs, etc).


## Systems of Units are based on Systems of Quantities

[Systems of quantities](../../appendix/glossary.md#system-of-quantities) specify a set
of quantities and equations relating to those quantities. Those equations do not take any
unit or a numerical representation into account at all. To create a quantity,
we need to add those missing pieces of information. This is where
a [system of units](../../appendix/glossary.md#system-of-units) kicks in.

The [SI](../../appendix/glossary.md#si) is explicitly stated to be based on
the [ISQ](../../appendix/glossary.md#isq). Among others, it defines
`7` [base units](../../appendix/glossary.md#base-unit), one for each
[base quantity](../../appendix/glossary.md#base-quantity). In the **mp-units**
this is expressed by associating a quantity kind (that we discussed in detail in the
previous chapter) with a unit that is used to express it:

```cpp
inline constexpr struct metre final : named_unit<"m", kind_of<isq::length>> {} metre;
```

!!! important

    The `kind_of<isq::length>` above states explicitly that this unit has
    an associated quantity kind. In other words, `si::metre` (and scaled units based
    on it) can be used to express the amount of any quantity of kind _length_.


## Units compose

One of the most vital points of the [SI](../../appendix/glossary.md#si) system
is that its units compose. This allows providing thousands of different units for
hundreds of various quantities with a tiny set of predefined units
and prefixes.

The same is modeled in the **mp-units** library, which also allows composing
predefined units to create a nearly infinite number of different
[derived units](../../appendix/glossary.md#derived-unit). For example, one can write:

```cpp
quantity<si::metre / si::second> q;
```

to express a quantity of _speed_. The resulting quantity type is implicitly inferred
from the [unit equation](../../appendix/glossary.md#unit-equation) by repeating
the same operations on the associated quantity kinds.


## Many shades of the same unit

The [SI](../../appendix/glossary.md#si) provides the names for 22 common
[coherent units](../../appendix/glossary.md#coherent-derived-unit) of 22
[derived quantities](../../appendix/glossary.md#derived-quantity).

Each such named [derived unit](../../appendix/glossary.md#derived-unit) is a result
of a specific predefined [unit equation](../../appendix/glossary.md#unit-equation).
For example, a unit of _power_ quantity is defined in the library as:

```cpp
inline constexpr struct watt final : named_unit<"W", joule / second> {} watt;
```

However, a _power_ quantity can be expressed in other units as well. For example,
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

!!! note

    The above code example may give the impression that the order of components in a derived
    unit is determined by the multiplication order. This is not the case. As stated in
    [Simplifying the resulting symbolic expressions](interface_introduction.md#simplifying-the-resulting-symbolic-expressions),
    to be able to reason about and simplify units, the library needs to order them in an
    appropriate order. This will affect the order of components in a resulting type and
    text output.

    Please refer to [our FAQ](../../getting_started/faq.md#why-derived-units-order-is-not-preserved-from-the-multiplication)
    for more information.


## Constraining a derived unit to work only with a specific derived quantity

Some derived units are valid only for specific derived quantities. For example,
[SI](../../appendix/glossary.md#si) specifies both `hertz` and `becquerel` derived units
with the same unit equation `1 / s`. However, it also explicitly states:

!!! quote "SI Brochure"

    The hertz shall only be used for periodic phenomena and the becquerel shall only be used for
    stochastic processes in activity referred to a radionuclide.

The above means that the usage of `becquerel` as a unit of a _frequency_ quantity is an error.

The library allows constraining such units to work only with quantities of a specific kind
in the following way:

```cpp
inline constexpr struct hertz final : named_unit<"Hz", one / second, kind_of<isq::frequency>> {} hertz;
inline constexpr struct becquerel final : named_unit<"Bq", one / second, kind_of<isq::activity>> {} becquerel;
```

With the above, `hertz` can only be used with _frequencies_, while `becquerel` should only
be used with quantities of _activity_. This means that the following equation will not compile:

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

Each prefix is implemented similarly to the following:

```cpp
template<PrefixableUnit U> struct quecto_ : prefixed_unit<"q", mag_power<10, -30>, U{}> {};
template<PrefixableUnit auto U> constexpr quecto_<decltype(U)> quecto;
```

and then a [PrefixableUnit](concepts.md#PrefixableUnit) can be prefixed in the following
way:

```cpp
inline constexpr auto qm = quecto<metre>;
```

The usage of `mag_power` not only enables providing support for SI prefixes, but it can also
efficiently represent any rational magnitude. For example, IEC 80000 prefixes used in the
IT industry can be implemented as:

```cpp
template<PrefixableUnit U> struct yobi_ : prefixed_unit<"Yi", mag_power<2, 80>, U{}> {};
template<PrefixableUnit auto U> constexpr yobi_<decltype(U)> yobi;
```

### Auto-scaling of a quantity

When displaying quantities with values that span many orders of magnitude, manually selecting
the appropriate prefix can be tedious and error-prone. The **mp-units** library provides the
`invoke_with_prefixed()` utility function that automatically scales a quantity to use
the most appropriate SI prefix based on the magnitude of its value.

#### Basic Usage

The function takes a callable (like a lambda), a quantity to scale, and a prefixable unit,
then invokes the callable with the quantity converted to the most appropriate prefixed version
of that unit:

```cpp
quantity voltage = 0.001'234 * V;
si::invoke_with_prefixed([](auto q) { std::cout << q << '\n'; }, voltage, V);
// Prints: 1.234 mV
```

The prefix is selected automatically to ensure the value has at least one digit in the
integral part, making it easier to read.

#### Prefix Selection Modes

The function supports two prefix selection modes via the `prefix_range` parameter:

- **`prefix_range::engineering`** (default): Selects only engineering prefixes (powers of 1000:
  kilo, mega, milli, micro, etc.), resulting in values in the range [1.0, 1000)
- **`prefix_range::full`**: Selects from all SI prefixes including intermediate ones (deca, hecto,
  deci, centi), often resulting in values in the range [1.0, 10.0)

```cpp
quantity length = 456 * m;
si::invoke_with_prefixed([](auto q) { std::cout << q << '\n'; }, length, m, si::prefix_range::engineering);
// Prints: 456 m

si::invoke_with_prefixed([](auto q) { std::cout << q << '\n'; }, length, m, si::prefix_range::full);
// Prints: 4.56 hm
```

#### Practical Example: Capacitor Discharge

A real-world example from [capacitor_time_curve.cpp](../../examples/capacitor_time_curve.md)
demonstrates automatic prefix selection for displaying voltage values that decay exponentially:

```cpp
for (auto tt = 0 * ms; tt <= 50 * ms; ++tt) {
  const QuantityOf<isq::voltage> auto Vt = V0 * exp(-tt / (RR * CC));
  std::cout << "- at " << tt << " voltage is ";
  si::invoke_with_prefixed([](auto q) { std::cout << q; }, Vt, V);
  std::cout << "\n";
}
```

This automatically transitions between appropriate prefixes as the voltage decays:

```text
- at 0 ms voltage is 5.000 V
- at 4 ms voltage is 817.638 mV
- at 19 ms voltage is 919.446 µV
- at 35 ms voltage is 657.491 nV
- at 50 ms voltage is 739.358 pV
```

Without automatic prefix selection, the output would require manual unit conversions or display
values like `0.000000000739358 V`, which are harder to read and interpret.

#### Return Value

The function returns whatever the callable returns, allowing flexible usage:

```cpp
// Use it just for output
si::invoke_with_prefixed([](auto q) { std::cout << q << '\n'; }, voltage, V);

// Use it to get a formatted string
auto str = si::invoke_with_prefixed([](auto q) { return std::format("{}", q); }, voltage, V);
```


## Scaled units

In the [SI](../../appendix/glossary.md#si), all units are either base or derived units or prefixed
versions of those. However, those are only some of the options possible.

For example, there is a list of [off-system units](../../appendix/glossary.md#off-system-unit)
accepted for use with SI. Those are scaled versions of the SI units with ratios that can't
be explicitly expressed with predefined SI prefixes. Those include units like minute, hour,
or electronvolt:

```cpp
inline constexpr struct minute final : named_unit<"min", mag<60> * si::second> {} minute;
inline constexpr struct hour final : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct electronvolt final : named_unit<"eV", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * si::joule> {} electronvolt;
```

Also, units of other [systems of units](../../appendix/glossary.md#system-of-units) are
often defined in terms of scaled versions of the SI units. For example, the international
yard is defined as:

```cpp
inline constexpr struct yard final : named_unit<"yd", mag_ratio<9'144, 10'000> * si::metre> {} yard;
```

For some units, a magnitude might also be irrational. The best example here is a `degree`
which is defined using a floating-point magnitude having a factor of the number π (Pi):

```cpp
inline constexpr struct pi final : mag_constant<symbol_text{u8"π", "pi"}, std::numbers::pi_v<long double>> {} pi;
inline constexpr auto π = pi;
```

```cpp
inline constexpr struct degree final : named_unit<{u8"°", "deg"}, mag<π> / mag<180> * si::radian> {} degree;
```


## Unit symbols

Units are available via their full names or through their short symbols.
To use a long version, it is enough to type:

```cpp
quantity q1 = 42 * si::metre / si::second;
quantity q2 = 42 * si::kilo<si::metre> / si::hour;
```

To simplify how we spell it a short, user-friendly symbols are provided in a dedicated
subnamespace in systems definitions:

```cpp
namespace si::unit_symbols {

constexpr auto m = si::metre;
constexpr auto km = si::kilo<si::metre>;
constexpr auto s = si::second;
constexpr auto h = si::hour;

}
```

Unit symbols introduce a lot of short identifiers into the current namespace. This is why they
are opt-in. A user has to explicitly "import" them from a dedicated `unit_symbols` namespace:

=== "using-declaration"

    ```cpp
    using namespace si::unit_symbols;

    quantity q1 = 42 * m / s;
    quantity q2 = 42 * km / h;
    ```

=== "using-directive"

    ```cpp
    using si::unit_symbols::m;
    using si::unit_symbols::km;
    using si::unit_symbols::s;
    using si::unit_symbols::h;

    quantity q1 = 42 * m / s;
    quantity q2 = 42 * km / h;
    ```

We also provide alternative object identifiers using UTF-8 characters in their names for most
unit symbols. The code using UTF-8 looks nicer, but it is harder to type on the keyboard.
This is why we provide both versions of identifiers for such units.

=== "Portable"

    ```cpp
    quantity resistance = 60 * kohm;
    quantity capacitance = 100 * uF;
    ```

=== "With UTF-8 glyphs"

    ```cpp
    quantity resistance = 60 * kΩ;
    quantity capacitance = 100 * µF;
    ```


## Common units

Adding, subtracting, or comparing two quantities of different units will force the library
to find a common unit for those. This is to prevent data truncation. For the cases when
one of the units is an integral multiple of the another, the resulting quantity will use
a "smaller" one in its result. For example:

```cpp
static_assert((1 * kg + 1 * g).unit == g);
static_assert((1 * km + 1 * mm).unit == mm);
static_assert((1 * yd + 1 * mi).unit == yd);
```

However, in many cases an arithmetic operation on quantities of different units will
result in a yet another unit. This happens when none of the source units is an integral
multiple of another. In such cases, the library returns a special type that denotes that
we are dealing with a common unit of such an equation:

```cpp
quantity q1 = 1 * km + 1 * mi;     // quantity<common_unit<international::mile, si::kilo_<si::metre>>{}, int>
quantity q2 = 1. * rad + 1. * deg; // quantity<common_unit<si::degree, si::radian>{}, double>
```

!!! note

    A user should never explicitly instantiate a `common_unit` class template. The library's
    framework will do it based on the provided quantity equation.
