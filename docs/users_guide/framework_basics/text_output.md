# Text Output

Besides providing dimensional analysis and units conversions, the library also tries hard to print
any quantity in the most user-friendly way.

!!! note

    The library does not provide a text output for quantity points, as printing just a number and a unit
    is not enough to adequately describe a quantity point. Often an additional postfix is required.

    For example, the text output of `42 m` may mean many things and can also be confused with an output
    of a regular quantity. On the other hand, printing `42 m AMSL` for altitudes above mean sea level is
    a much better solution, but the library does not have enough information to print it that way by itself.


## Customization point

The [SI Brochure](../appendix/references.md#SIBrochure) says:

!!! quote "SI Brochure"

    The numerical value always precedes the unit and a space is always used to separate the unit from
    the number. ... The only exceptions to this rule are for the unit symbols for degree, minute and
    second for plane angle, `°`, `′` and `″`, respectively, for which no space is left between the
    numerical value and the unit symbol.

To support the above, the library exposes `space_before_unit_symbol` customization point. By default,
its value is `true` for all the units, so the space between a number and a unit will be present in the
output text. To change this behavior, we have to provide a partial specialization for a specific unit:

```cpp
template<>
inline constexpr bool space_before_unit_symbol<non_si::degree> = false;
```

!!! note

    The above works only for [the default formatting](#default-formatting). In case we provide our own
    format specification (i.e. `std::format("{:%Q %q}", q)`), the library will always obey this
    specification for all the units (no matter of what is the actual value of the
    `space_before_unit_symbol` customization point) and the separating space will always be present
    in this case.


## Output streams

!!! tip

    The output streaming support is opt-in and can be enabled by including the `<mp-units/ostream.h>`
    header file.

The easiest way to print a quantity is to provide its object to the output stream:

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::international::unit_symbols;

const QuantityOf<isq::speed> auto v1 = avg_speed(220. * km, 2 * h);
const QuantityOf<isq::speed> auto v2 = avg_speed(140. * mi, 2 * h);
std::cout << v1 << '\n';  // 110 km/h
std::cout << v2 << '\n';  // 70 mi/h
```

The text output will always print the [value of a quantity](../../appendix/glossary.md#quantity-value)
typically followed by a space and then the symbol of a [unit](../../appendix/glossary.md#unit)
associated with this quantity.

!!! important

    Remember that when we deal with a quantity of an "unknown" `auto` type, it is a good practice
    to always [convert the unit to the expected one](value_conversions.md#value-conversions)
    before passing it to the text output:

    ```cpp
    std::cout << v1.in(km / h) << '\n';       // 110 km/h
    std::cout << v1.force_in(m / s) << '\n';  // 30.5556 m/s
    ```


### Output stream formatting

Only basic formatting can be applied for output streams. It includes control over width, fill,
and alignment:

```cpp
std::cout << "|" << std::setw(10) << 123 * m << "|\n";                       // |     123 m|
std::cout << "|" << std::setw(10) << std::left << 123 * m << "|\n";          // |123 m     |
std::cout << "|" << std::setw(10) << std::setfill('*') << 123 * m << "|\n";  // |123 m*****|
```


## `std::format`

!!! tip

    The text formatting facility support is opt-in and can be enabled by including the
    `<mp-units/format.h>` header file.

The **mp-units** library provides custom formatters for `std::format` facility which allows
fine-grained control over what and how it is being printed in the text output.


### Grammar

```text
units-format-spec   ::=  [fill-and-align] [width] [units-specs]
units-specs         ::=  conversion-spec
                         units-specs conversion-spec
                         units-specs literal-char
literal-char        ::=  any character other than '{' or '}'
conversion-spec     ::=  '%' units-type
units-type          ::=  [units-rep-modifier] 'Q'
                         [units-unit-modifier] 'q'
units-rep-modifier  ::=  [sign] [#] [precision] [L] [units-rep-type]
units-rep-type      ::=  one of "aAbBdeEfFgGoxX"
units-unit-modifier ::=  [units-text-encoding, units-unit-symbol-denominator, units-unit-symbol-separator]
units-text-encoding ::=  one of "UA"
units-unit-symbol-solidus   ::=  one of "oan"
units-unit-symbol-separator ::=  one of "sd"
```


In the above grammar:

- `fill-and-align`, `width`, `sign`, `#`, `precision`, and `L` tokens, as well as the individual
  tokens of `units-rep-type` are defined in the [format.string.std](https://wg21.link/format.string.std)
  chapter of the C++ standard specification,
- tokens `Q` and `q` of `units-type` are described in the [time.format](https://wg21.link/time.format)
  chapter of the C++ standard specification,
- `units-text-encoding` tokens specify the unit text encoding:
    - `U` (default) uses the **Unicode** symbols defined by the [SI](../../appendix/glossary.md#si)
      specification (i.e. `m³`, `µs`)
    - `A` token forces non-standard **ASCII**-only output (i.e. `m^3`, `us`)
- `units-unit-symbol-solidus` tokens specify how the division of units should look like:
    - `o` (default) outputs `/` only when there is only **one** unit in the denominator, otherwise negative
      exponents are printed (i.e. `m/s`, `kg m⁻¹ s⁻¹`)
    - `a` **always** uses solidus (i.e. `m/s`, `kg/(m s)`)
    - `n` **never** prints solidus, which means that negative exponents are always used (i.e. `m s⁻¹`,
      `kg m⁻¹ s⁻¹`)
- `units-unit-symbol-separator` tokens specify how multiplied unit symbols should be separated:
    - `s` (default) uses **space** as a separator (i.e. `kg m²/s²`)
    - `d` uses half-high **dot** (`⋅`) as a separator (i.e. `kg⋅m²/s²`)


### Default formatting

To format `quantity` values the formatting facility uses `units-format-spec`. If it is left empty,
the default formatting of `{:%Q %q}` is applied. The same default formatting is also applied
to the output streams. This is why the following code lines produce the same output:

```cpp
std::cout << "Distance: " << 123 * km << "\n";
std::cout << std::format("Distance: {}\n", 123 * km);
std::cout << std::format("Distance: {:%Q %q}\n", 123 * km);
```


### Controlling width, fill, and alignment

To control width, fill, and alignment, the C++ standard grammar tokens `fill-and-align` and `width`
are being used, and they treat a quantity value and symbol as a contiguous text:

```cpp
std::println("|{:0}|", 123 * m);     // |123 m|
std::println("|{:10}|", 123 * m);    // |     123 m|
std::println("|{:<10}|", 123 * m);   // |123 m     |
std::println("|{:>10}|", 123 * m);   // |     123 m|
std::println("|{:^10}|", 123 * m);   // |  123 m   |
std::println("|{:*<10}|", 123 * m);  // |123 m*****|
std::println("|{:*>10}|", 123 * m);  // |*****123 m|
std::println("|{:*^10}|", 123 * m);  // |**123 m***|
```

!!! note

    [`std::println` is a C++23 facility](https://en.cppreference.com/w/cpp/io/print). In case you
    do not have access to C++23, you can obtain the same output with:

    ```cpp
    std::cout << std::format("<format-string>\n", <format-args>);
    ```


### Quantity value, symbol, or both?

The user can easily decide to either print a whole quantity (value and symbol) or only its parts.
Also, a custom style of quantity formatting might be applied:

```cpp
std::println("{:%Q}", 123 * km);    // 123
std::println("{:%q}", 123 * km);    // km
std::println("{:%Q%q}", 123 * km);  // 123km
```


### Quantity value formatting

`sign` token allows us to specify how the value's sign is being printed:

```cpp
std::println("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", 1 * m);   // 1 m,+1 m,1 m, 1 m
std::println("{0:%Q %q},{0:%+Q %q},{0:%-Q %q},{0:% Q %q}", -1 * m);  // -1 m,-1 m,-1 m,-1 m
```

where:

- `+` indicates that a sign should be used for both non-negative and negative numbers,
- `-` indicates that a sign should be used for negative numbers and negative zero only
  (this is the default behavior),
- `<space>` indicates that a leading space should be used for non-negative numbers other
  than negative zero, and a minus sign for negative numbers and negative zero.

`precision` token is allowed only for floating-point representation types:

```cpp
std::println("{:%.0Q %q}", 1.2345 * m);  // 1 m
std::println("{:%.1Q %q}", 1.2345 * m);  // 1.2 m
std::println("{:%.2Q %q}", 1.2345 * m);  // 1.23 m
```

`units-rep-type` specifies how a value of the representation type is being printed.
For integral types:

```cpp
std::println("{:%bQ %q}", 42 * m);    // 101010 m
std::println("{:%BQ %q}", 42 * m);    // 101010 m
std::println("{:%dQ %q}", 42 * m);    // 42 m
std::println("{:%oQ %q}", 42 * m);    // 52 m
std::println("{:%xQ %q}", 42 * m);    // 2a m
std::println("{:%XQ %q}", 42 * m);    // 2A m
```

The above can be printed in an alternate version thanks to the `#` token:

```cpp
std::println("{:%#bQ %q}", 42 * m);   // 0b101010 m
std::println("{:%#BQ %q}", 42 * m);   // 0B101010 m
std::println("{:%#oQ %q}", 42 * m);   // 052 m
std::println("{:%#xQ %q}", 42 * m);   // 0x2a m
std::println("{:%#XQ %q}", 42 * m);   // 0X2A m
```

For floating-point values, the `units-rep-type` token works as follows:

```cpp
std::println("{:%aQ %q}",   1.2345678 * m);      // 0x1.3c0ca2a5b1d5dp+0 m
std::println("{:%.3aQ %q}", 1.2345678 * m);      // 0x1.3c1p+0 m
std::println("{:%AQ %q}",   1.2345678 * m);      // 0X1.3C0CA2A5B1D5DP+0 m
std::println("{:%.3AQ %q}", 1.2345678 * m);      // 0X1.3C1P+0 m
std::println("{:%eQ %q}",   1.2345678 * m);      // 1.234568e+00 m
std::println("{:%.3eQ %q}", 1.2345678 * m);      // 1.235e+00 m
std::println("{:%EQ %q}",   1.2345678 * m);      // 1.234568E+00 m
std::println("{:%.3EQ %q}", 1.2345678 * m);      // 1.235E+00 m
std::println("{:%gQ %q}",   1.2345678 * m);      // 1.23457 m
std::println("{:%gQ %q}",   1.2345678e8 * m);    // 1.23457e+08 m
std::println("{:%.3gQ %q}", 1.2345678 * m);      // 1.23 m
std::println("{:%.3gQ %q}", 1.2345678e8 * m);    // 1.23e+08 m
std::println("{:%GQ %q}",   1.2345678 * m);      // 1.23457 m
std::println("{:%GQ %q}",   1.2345678e8 * m);    // 1.23457E+08 m
std::println("{:%.3GQ %q}", 1.2345678 * m);      // 1.23 m
std::println("{:%.3GQ %q}", 1.2345678e8 * m);    // 1.23E+08 m
```


### Unit symbol formatting

Unit symbols of some quantities are specified to use Unicode signs by the
[SI](../../appendix/glossary.md#si) (i.e. `Ω` symbol for the resistance quantity). The **mp-units**
library follows this by default. From the engineering point of view, sometimes Unicode text might
not be the best solution as terminals of many (especially embedded) devices are ASCII-only.
In such a case, the unit symbol can be forced to be printed using ASCII-only characters thanks to
`units-text-encoding` token:

```cpp
std::println("{}", 10 * si::ohm);             // 10 Ω
std::println("{:%Q %Aq}", 10 * si::ohm);      // 10 ohm
std::println("{}", 125 * us);                 // 125 µs
std::println("{:%Q %Aq}", 125 * us);          // 125 us
std::println("{}", 9.8 * (m / s2));           // 9.8 m/s²
std::println("{:%Q %Aq}", 9.8 * (m / s2));    // 9.8 m/s^2
```

Additionally, both [ISQ](../../appendix/glossary.md#isq) and [SI](../../appendix/glossary.md#si)
leave some freedom on how to print unit symbols. This is why two additional tokens were introduced.

`units-unit-symbol-solidus` specifies how the division of units should look like. By default,
`/` will be used only when the denominator contains only one unit. However, with the `a` or `n`
options, we can force the facility to print the `/` character always (even when there are more units
in the denominator), or never in which case a parenthesis will be added to enclose all denominator
units.

```cpp
std::println("{:%Q %q}", 1 * (m / s));         // 1 m/s
std::println("{:%Q %q}", 1 * (kg / m / s2));   // 1 kg m⁻¹ s⁻²
std::println("{:%Q %aq}", 1 * (m / s));        // 1 m/s
std::println("{:%Q %aq}", 1 * (kg / m / s2));  // 1 kg/(m s²)
std::println("{:%Q %nq}", 1 * (m / s));        // 1 m s⁻¹
std::println("{:%Q %nq}", 1 * (kg / m / s2));  // 1 kg m⁻¹ s⁻²
```

Also, there are a few options to separate the units being multiplied:

!!! quote "ISO 80000-1"

    When symbols for quantities are combined in a product of two or more quantities, this combination
    is indicated in one of the following ways: `ab`, `a b`, `a · b`, `a × b`

    _NOTE 1_ In some fields, e.g., vector algebra, distinction is made between `a ∙ b` and `a × b`.

As of today, the **mp-units** library provides the support for `a b` and `a · b` only. Additionally,
we decided that the extraneous space in the latter case makes the result too verbose, so we decided
to just use the `·` symbol as a separator.

!!! note

    Please let us know in case you require more formatting options here.

The `units-unit-symbol-separator` token allows us to obtain the following outputs:

```cpp
std::println("{:%Q %q}", 1 * (kg * m2 / s2));   // 1 kg m²/s²
std::println("{:%Q %dq}", 1 * (kg * m2 / s2));  // 1 kg⋅m²/s²
```
