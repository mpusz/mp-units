# Text Output

Besides providing dimensional analysis and unit conversions, the library also tries hard to print
any quantity in the most user-friendly way. We can print the entire quantity or its
selected parts (numerical value, unit, or dimension).

!!! note

    The library does not provide a text output for quantity points. The quantity stored inside
    is just an implementation detail of this type. It is a vector from a specific origin.
    Without the knowledge of the origin, the vector by itself is useless as we can't determine
    which point it describes.

    In the current library design, point origin does not provide any text in its definition.
    Even if we could add such information to the point's definition, we would not
    know how to output it in the text. There may be many ways to do it. For example, should we
    prepend or append the origin part to the quantity text?

    For example, the text output of `42 m` for a quantity point may mean many things. It may be
    an offset from the mountain top, sea level, or maybe the center of Mars.
    Printing `42 m AMSL` for altitudes above mean sea level is a much better solution, but the
    library does not have enough information to print it that way by itself.

    Please let us know if you have a good idea of how to solve this issue.


## Predefined symbols

The definitions of dimensions, units, prefixes, and constants require assigning text symbols
for each entity. Those symbols will be composed by the library's framework to express dimensions
and units of derived quantities.

=== "Dimensions"

    ```cpp
    inline constexpr struct dim_length final : base_dimension<"L"> {} dim_length;
    inline constexpr struct dim_mass final : base_dimension<"M"> {} dim_mass;
    inline constexpr struct dim_time final : base_dimension<"T"> {} dim_time;
    inline constexpr struct dim_electric_current final : base_dimension<"I"> {} dim_electric_current;
    inline constexpr struct dim_thermodynamic_temperature final : base_dimension<{u8"Θ", "O"}> {} dim_thermodynamic_temperature;
    inline constexpr struct dim_amount_of_substance final : base_dimension<"N"> {} dim_amount_of_substance;
    inline constexpr struct dim_luminous_intensity final : base_dimension<"J"> {} dim_luminous_intensity;
    ```

=== "Units"

    ```cpp
    inline constexpr struct second final : named_unit<"s", kind_of<isq::time>> {} second;
    inline constexpr struct metre final : named_unit<"m", kind_of<isq::length>> {} metre;
    inline constexpr struct gram final : named_unit<"g", kind_of<isq::mass>> {} gram;
    inline constexpr auto kilogram = kilo<gram>;

    inline constexpr struct newton final : named_unit<"N", kilogram * metre / square(second)> {} newton;
    inline constexpr struct joule final : named_unit<"J", newton * metre> {} joule;
    inline constexpr struct watt final : named_unit<"W", joule / second> {} watt;
    inline constexpr struct coulomb final : named_unit<"C", ampere * second> {} coulomb;
    inline constexpr struct volt final : named_unit<"V", watt / ampere> {} volt;
    inline constexpr struct farad final : named_unit<"F", coulomb / volt> {} farad;
    inline constexpr struct ohm final : named_unit<{u8"Ω", "ohm"}, volt / ampere> {} ohm;
    ```

=== "Prefixes"

    ```cpp
    template<PrefixableUnit U> struct micro_ : prefixed_unit<{u8"µ", "u"}, mag_power<10, -6>, U{}> {};
    template<PrefixableUnit U> struct milli_ : prefixed_unit<"m", mag_power<10, -3>, U{}> {};
    template<PrefixableUnit U> struct centi_ : prefixed_unit<"c", mag_power<10, -2>, U{}> {};
    template<PrefixableUnit U> struct deci_  : prefixed_unit<"d", mag_power<10, -1>, U{}> {};
    template<PrefixableUnit U> struct deca_  : prefixed_unit<"da", mag_power<10, 1>, U{}> {};
    template<PrefixableUnit U> struct hecto_ : prefixed_unit<"h", mag_power<10, 2>, U{}> {};
    template<PrefixableUnit U> struct kilo_  : prefixed_unit<"k", mag_power<10, 3>, U{}> {};
    template<PrefixableUnit U> struct mega_  : prefixed_unit<"M", mag_power<10, 6>, U{}> {};
    ```

=== "Constants"

    ```cpp
    inline constexpr struct hyperfine_structure_transition_frequency_of_cs final : named_unit<{u8"Δν_Cs", "dv_Cs"}, mag<9'192'631'770> * hertz> {} hyperfine_structure_transition_frequency_of_cs;
    inline constexpr struct speed_of_light_in_vacuum final : named_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum;
    inline constexpr struct planck_constant final : named_unit<"h", mag_ratio<662'607'015, 100'000'000> * mag_power<10, -34> * joule * second> {} planck_constant;
    inline constexpr struct elementary_charge final : named_unit<"e", mag_ratio<1'602'176'634, 1'000'000'000> * mag_power<10, -19> * coulomb> {} elementary_charge;
    inline constexpr struct boltzmann_constant final : named_unit<"k", mag_ratio<1'380'649, 1'000'000> * mag_power<10, -23> * joule / kelvin> {} boltzmann_constant;
    inline constexpr struct avogadro_constant final : named_unit<"N_A", mag_ratio<602'214'076, 100'000'000> * mag_power<10, 23> / mole> {} avogadro_constant;
    inline constexpr struct luminous_efficacy final : named_unit<"K_cd", mag<683> * lumen / watt> {} luminous_efficacy;
    ```

!!! important

    Two symbols always have to be provided if the primary symbol contains characters outside of
    the [basic literal character set](https://en.cppreference.com/w/cpp/language/charset).
    The first must be provided as a UTF-8 literal and may contain any Unicode characters.
    The second one must provide an alternative spelling and only use characters from within of
    [basic literal character set](https://en.cppreference.com/w/cpp/language/charset).

!!! note

    Unicode provides only a minimal set of characters available as subscripts, which are often used
    to differentiate various constants and quantities of the same kind. To workaround this issue,
    **mp-units** uses the '_' character to specify that the following characters should be considered
    a subscript of the symbol.

!!! tip

    For older compilers, it might be required to specify a `symbol_text` class explicitly
    template name to initialize it with two symbols:

    ```cpp
    inline constexpr struct ohm final : named_unit<symbol_text{u8"Ω", "ohm"}, volt / ampere> {} ohm;
    ```


## Symbols for derived entities

### `text_encoding`

[ISQ](../../appendix/glossary.md#isq) and [SI](../../appendix/glossary.md#si) standards always
specify symbols using Unicode encoding. This is why it is a default and primary target for
text output. However, in some applications or environments, a standard ASCII-like text output
using only the characters from the [basic literal character set](https://en.cppreference.com/w/cpp/language/charset)
can be preferred by users.

This is why the library provides an option to change the default encoding to the ASCII one with:

```cpp
enum class text_encoding : std::int8_t {
  unicode,  // µs; m³;  L²MT⁻³
  ascii,    // us; m^3; L^2MT^-3
  default_encoding = unicode
};
```

### Symbols of derived dimensions

#### `dimension_symbol_formatting`

`dimension_symbol_formatting` is a data type describing the configuration of the symbol generation
algorithm.

```cpp
struct dimension_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
};
```

#### `dimension_symbol()`

Returns a `std::string_view` with the symbol of a dimension for the provided configuration:

```cpp
template<dimension_symbol_formatting fmt = dimension_symbol_formatting{}, typename CharT = char, Dimension D>
[[nodiscard]] consteval std::string_view dimension_symbol(D);
```

For example:

```cpp
static_assert(dimension_symbol<{.encoding = text_encoding::ascii}>(isq::power.dimension) == "L^2MT^-3");
```

!!! note

    `std::string_view` is returned only when C++23 is available. Otherwise, an instance of a
    `basic_fixed_string` is being returned.

#### `dimension_symbol_to()`

Inserts the generated dimension symbol into the output text iterator at runtime.

```cpp
template<typename CharT = char, std::output_iterator<CharT> Out, Dimension D>
constexpr Out dimension_symbol_to(Out out, D d, dimension_symbol_formatting fmt = dimension_symbol_formatting{});
```

For example:

```cpp
std::string txt;
dimension_symbol_to(std::back_inserter(txt), isq::power.dimension, {.encoding = text_encoding::ascii});
std::cout << txt << "\n";
```

The above prints:

```text
L^2MT^-3
```


### Symbols of derived units

#### `unit_symbol_formatting`

`unit_symbol_formatting` is a data type describing the configuration of the symbol generation
algorithm. It contains three orthogonal fields, each with a default value.

```cpp
enum class unit_symbol_solidus : std::int8_t {
  one_denominator,  // m/s;   kg m⁻¹ s⁻¹
  always,           // m/s;   kg/(m s)
  never,            // m s⁻¹; kg m⁻¹ s⁻¹
  default_denominator = one_denominator
};

enum class unit_symbol_separator : std::int8_t {
  space,          // kg m²/s²
  half_high_dot,  // kg⋅m²/s²  (valid only for unicode encoding)
  default_separator = space
};

struct unit_symbol_formatting {
  text_encoding encoding = text_encoding::default_encoding;
  unit_symbol_solidus solidus = unit_symbol_solidus::default_denominator;
  unit_symbol_separator separator = unit_symbol_separator::default_separator;
};
```

`unit_symbol_solidus` impacts how the division of unit symbols is being presented in the text
output. By default, the '/' will be printed if only one unit component is in the
denominator. Otherwise, the exponent syntax will be used.

`unit_symbol_separator` specifies how multiple multiplied units should be separated from each
other. By default, the space (' ') will be used as a separator.

#### `unit_symbol()`

Returns a `std::string_view` with the symbol of a unit for the provided configuration:

```cpp
template<unit_symbol_formatting fmt = unit_symbol_formatting{}, typename CharT = char, Unit U>
[[nodiscard]] consteval std::string_view unit_symbol(U);
```

For example:

```cpp
static_assert(unit_symbol<{.solidus = unit_symbol_solidus::never,
                           .separator = unit_symbol_separator::half_high_dot}>(kg * m / s2) == "kg⋅m⋅s⁻²");
```

!!! note

    `std::string_view` is returned only when C++23 is available. Otherwise, an instance of a
    `basic_fixed_string` is being returned. See more in the
    [C++ compiler support](../../getting_started/cpp_compiler_support.md#static-constexpr-variables-in-constexpr-functions)
    chapter.

#### `unit_symbol_to()`

Inserts the generated unit symbol into the output text iterator at runtime.

```cpp
template<typename CharT = char, std::output_iterator<CharT> Out, Unit U>
constexpr Out unit_symbol_to(Out out, U u, unit_symbol_formatting fmt = unit_symbol_formatting{});
```

For example:

```cpp
std::string txt;
unit_symbol_to(std::back_inserter(txt), kg * m / s2,
               {.solidus = unit_symbol_solidus::never, .separator = unit_symbol_separator::half_high_dot});
std::cout << txt << "\n";
```

The above prints:

```text
kg⋅m⋅s⁻²
```


## Symbols of scaled units

In most cases [scaled units are hidden behind named units](systems_of_units.md#scaled-units).
However, there are a few real-life where a user directly faces a scaled unit. For example:

```cpp
constexpr Unit auto l_per_100km = l / (mag<100> * km);
```

The above is a derived unit of litre divided by a scaled unit of 100 kilometers. As we can
see a scaled unit has a magnitude and a reference unit. To denote the scope of such
a unit, we enclose it in `[...]`. For example, the following:

```cpp
std::cout << 6.7 * l_per_100km << "\n";
```

prints:

```text
6.7 l/[100 km]
```


## Symbols of common units

Some [common units](systems_of_units.md#common-units) expressed with a specialization of the
`common_unit` class template need special printing rules for their symbols. As they represent
a minimum set of equivalent common units resulting from the addition or subtraction of multiple
quantities, we print all of them as a scaled version of the source unit. For example,
the following:

```cpp
std::cout << 1 * km + 1 * mi << "\n";
std::cout << 1 * nmi + 1 * mi << "\n";
std::cout << 1 * km / h + 1 * m / s << "\n";
std::cout << 1 * rad + 1 * deg << "\n";
```

prints:

```text
40771 EQUIV{[1/25146 mi], [1/15625 km]}
108167 EQUIV{[1/50292 mi], [1/57875 nmi]}
23 EQUIV{[1/5 km/h], [1/18 m/s]}
183.142 EQUIV{[1/𝜋°], [1/180 rad]}
```

Thanks to the above, it might be easier for the user to reason about the magnitude of the resulting
unit and its impact on the value stored in the quantity.


## `space_before_unit_symbol` customization point

The [SI Brochure](../../appendix/references.md#SIBrochure) says:

!!! quote "SI Brochure"

    The numerical value always precedes the unit and a space is always used to separate the unit from
    the number. ... The only exceptions to this rule are for the unit symbols for degree, minute and
    second for plane angle, `°`, `′` and `″`, respectively, for which no space is left between the
    numerical value and the unit symbol.

There are more units with such properties. For example, percent (`%`) and per mille(`‰`).

To support the above and other similar cases, the library exposes `space_before_unit_symbol`
customization point. By default, its value is `true` for all the units, so the space between a number
and a unit will be inserted in the output text. To change this behavior, we have to provide a partial
specialization for a specific unit:

```cpp
template<>
constexpr bool space_before_unit_symbol<non_si::degree> = false;
```

!!! note

    The above works only for [the default formatting](#default-formatting) or for the format
    strings that use `%?` placement field (`std::format("{}", q)` is equivalent to
    `std::format("{:%N%?%U}", q)`).

    In case a user provides custom format specification (e.g., `std::format("{:%N %U}", q)`),
    the library will always obey this specification for all the units (no matter what the actual
    value of the `space_before_unit_symbol` customization point is) and the separating space will always
    be used in this case.


## Output streams

!!! tip

    The output streaming support is opt-in and can be enabled by including the `<mp-units/ostream.h>`
    header file.

The easiest way to print a dimension, unit, or quantity is to provide its object to the output stream:

```cpp
const QuantityOf<isq::speed> auto v1 = avg_speed(220. * km, 2 * h);
const QuantityOf<isq::speed> auto v2 = avg_speed(140. * mi, 2 * h);
std::cout << v1 << '\n';            // 110 km/h
std::cout << v2 << '\n';            // 70 mi/h
std::cout << v2.unit << '\n';       // mi/h
std::cout << v2.dimension << '\n';  // LT⁻¹
```

The text output will always print the value using the default formatting for this entity.

!!! important "Important: Don't assume a unit"

    Remember that when we deal with a quantity of an "unknown" (e.g., `auto`) type, it is a good
    practice to always [convert the unit to the expected one](value_conversions.md#value-conversions)
    before passing it to the text output:

    ```cpp
    std::cout << v1.in(km / h) << '\n';       // 110 km/h
    std::cout << v1.force_in(m / s) << '\n';  // 30.5556 m/s
    ```


### Output stream formatting

Only basic formatting can be applied to output streams. It includes control over width, fill,
and alignment.

The numerical value of the quantity will be printed according to the current stream state and standard
manipulators may be used to customize that (assuming that the underlying representation type
respects them).

```cpp
std::cout << "|" << std::setw(10) << 123 * m << "|\n";                       // |     123 m|
std::cout << "|" << std::setw(10) << std::left << 123 * m << "|\n";          // |123 m     |
std::cout << "|" << std::setw(10) << std::setfill('*') << 123 * m << "|\n";  // |123 m*****|
```

!!! note

    To have more control over the formatting of the quantity that is printed with the output
    stream just use `std::cout << std::format(...)`.


## Text formatting

The library provides custom formatters for `std::format` facility, which allows fine-grained control
over what and how it is being printed in the text output.

!!! tip

    The text formatting facility support is opt-in and can be enabled by including the
    `<mp-units/format.h>` header file.

### Controlling width, fill, and alignment

Formatting grammar for all the entities provides control over width, fill, and alignment. The C++
standard grammar tokens `fill-and-align` and `width` are being used. They treat the entity as
a contiguous text to be aligned. For example, here are a few examples of the quantity numerical
value and symbol formatting:

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

It is important to note that in the second line above, the quantity text is aligned to
the right by default, which is consistent with the formatting of numeric types. Units and dimensions behave
as text and, thus, are aligned to the left by default.

!!! note

    [`std::println` is a C++23 facility](https://en.cppreference.com/w/cpp/io/print). In case we
    do not have access to C++23, we can obtain the same output with:

    ```cpp
    std::cout << std::format("<format-string>\n", <format-args>);
    ```

### Dimension formatting

```ebnf
dimension-format-spec = [fill-and-align], [width], [dimension-spec];
dimension-spec        = [text-encoding];
text-encoding         = 'U' | 'A';
```

In the above grammar:

- `fill-and-align` and `width` tokens are defined in the [format.string.std](https://wg21.link/format.string.std)
  chapter of the C++ standard specification,
- `text-encoding` token specifies the symbol text encoding:
    - `U` (default) uses the **Unicode** symbols defined by [@ISO80000] (e.g., `LT⁻²`),
    - `A` forces non-standard **ASCII**-only output (e.g., `LT^-2`).

Dimension symbols of some quantities are specified to use Unicode signs by the
[ISQ](../../appendix/glossary.md#isq) (e.g., `Θ` symbol for the _thermodynamic temperature_
dimension). The library follows this by default. From the engineering point of view, sometimes
Unicode text might not be the best solution, as terminals of many (especially embedded) devices
can output only letters from the basic literal character set. In such a case, the dimension
symbol can be forced to be printed using such characters thanks to `text-encoding` token:

```cpp
std::println("{}", isq::dim_thermodynamic_temperature);   // Θ
std::println("{:A}", isq::dim_thermodynamic_temperature); // O
std::println("{}", isq::power.dimension);                 // L²MT⁻³
std::println("{:A}", isq::power.dimension);               // L^2MT^-3
```

### Unit formatting

```ebnf
unit-format-spec      = [fill-and-align], [width], [unit-spec];
unit-spec             = [text-encoding], [unit-symbol-solidus], [unit-symbol-separator], [L]
                      | [text-encoding], [unit-symbol-separator], [unit-symbol-solidus], [L]
                      | [unit-symbol-solidus], [text-encoding], [unit-symbol-separator], [L]
                      | [unit-symbol-solidus], [unit-symbol-separator], [text-encoding], [L]
                      | [unit-symbol-separator], [text-encoding], [unit-symbol-solidus], [L]
                      | [unit-symbol-separator], [unit-symbol-solidus], [text-encoding], [L];
unit-symbol-solidus   = '1' | 'a' | 'n';
unit-symbol-separator = 's' | 'd';
```

In the above grammar:

- `fill-and-align` and `width` tokens are defined in the [format.string.std](https://wg21.link/format.string.std)
  chapter of the C++ standard specification,
- `unit-symbol-solidus` token specifies how the division of units should look like:
    - '1' (default) outputs `/` only when there is only **one** unit in the denominator, otherwise
      negative exponents are printed (e.g., `m/s`, `kg m⁻¹ s⁻¹`)
    - 'a' **always** uses solidus (e.g., `m/s`, `kg/(m s)`)
    - 'n' **never** prints solidus, which means that negative exponents are always used
      (e.g., `m s⁻¹`, `kg m⁻¹ s⁻¹`)
- `unit-symbol-separator` token specifies how multiplied unit symbols should be separated:
    - 's' (default) uses **space** as a separator (e.g., `kg m²/s²`)
    - 'd' uses half-high **dot** (`⋅`) as a separator (e.g., `kg⋅m²/s²`) (requires the Unicode encoding)
- 'L' is reserved for possible future localization use in case the C++ standard library gets access to
  the ICU-like database.

!!! note

    The above grammar intended that the elements of `unit-spec` can appear in
    any order as they have unique characters. Users shouldn't have to remember the order of those tokens
    to control the formatting of a unit symbol.

Unit symbols of some quantities are specified to use Unicode signs by the [SI](../../appendix/glossary.md#si)
(e.g., `Ω` symbol for the _resistance_ quantity). The library follows this by default. From the
engineering point of view, Unicode text might not be the best solution sometimes, as terminals
of many (especially embedded) devices can output only letters from the basic literal character set.
In such a case, the unit symbol can be forced to be printed using such characters thanks to
`text-encoding` token:

```cpp
std::println("{}", si::ohm);      // Ω
std::println("{:A}", si::ohm);    // ohm
std::println("{}", us);           // µs
std::println("{:A}", us);         // us
std::println("{}", m / s2);       // m/s²
std::println("{:A}", m / s2);     // m/s^2
```

Additionally, both ISO 80000 and [SI](../../appendix/glossary.md#si) leave some freedom on how to
print unit symbols. This is why two additional tokens were introduced.

`unit-symbol-solidus` specifies how the division of units should look like. By default,
`/` will be used only when the denominator contains only one unit. However, with the 'a' or 'n'
options, we can force the facility to print the `/` character always (even when there are more units
in the denominator), or never, in which case a parenthesis will be added to enclose all denominator
units.

```cpp
std::println("{}", m / s);          // m/s
std::println("{}", kg / m / s2);    // kg m⁻¹ s⁻²
std::println("{:a}", m / s);        // m/s
std::println("{:a}", kg / m / s2);  // kg/(m s²)
std::println("{:n}", m / s);        // m s⁻¹
std::println("{:n}", kg / m / s2);  // kg m⁻¹ s⁻²
```

Also, there are a few options to separate the units being multiplied. ISO 80000 (part 1) says:

!!! quote "ISO 80000-1"

    When symbols for quantities are combined in a product of two or more quantities, this combination
    is indicated in one of the following ways: `ab`, `a b`, `a · b`, `a × b`

    _NOTE 1_ In some fields, e.g., vector algebra, distinction is made between `a ∙ b` and `a × b`.

The library supports `a b` and `a · b` only. Additionally, we decided that the extraneous space
in the latter case makes the result too verbose, so we decided just to use the `·` symbol as
a separator.

!!! note

    Please let us know if you require more formatting options here.

The `unit-symbol-separator` token allows us to obtain the following outputs:

```cpp
std::println("{}", kg * m2 / s2);    // kg m²/s²
std::println("{:d}", kg * m2 / s2);  // kg⋅m²/s²
```

!!! note

    'd' requires the Unicode encoding to be set.


### Quantity formatting

```ebnf
quantity-format-spec        = [fill-and-align], [width], [quantity-specs], [defaults-specs];
quantity-specs              = conversion-spec;
                            | quantity-specs, conversion-spec;
                            | quantity-specs, literal-char;
literal-char                = ? any character other than '{', '}', or '%' ?;
conversion-spec             = '%', placement-type;
placement-type              = subentity-id | '?' | '%';
defaults-specs              = ':', default-spec-list;
default-spec-list           = default-spec;
                            | default-spec-list, default-spec;
default-spec                = subentity-id, '[' format-spec ']';
subentity-id                = 'N' | 'U' | 'D';
format-spec                 = ? as specified by the formatter for the argument type ?;
```

In the above grammar:

- `fill-and-align` and `width` tokens are defined in the [format.string.std](https://wg21.link/format.string.std)
  chapter of the C++ standard specification,
- `placement-type` token specifies which entity should be put and where:
    - 'N' inserts a default-formatted numerical value of the quantity,
    - 'U' inserts a default-formatted unit of the quantity,
    - 'D' inserts a default-formatted dimension of the quantity,
    - '?' inserts an optional separator between the number and a unit based on the value of
      `space_before_unit_symbol` for this unit,
    - '%' just inserts '%' character.
- `defaults-specs` token allows overwriting defaults for the underlying formatters with the custom
  format string. Each override starts with a subentity identifier ('N', 'U', or 'D') followed by
  the format string enclosed in square brackets.

#### Default formatting

To format `quantity` values, the formatting facility uses `quantity-format-spec`. If left empty,
the default formatting is applied. The same default formatting is also applied to the output streams.
This is why the following code lines produce the same output:

```cpp
std::cout << "Distance: " << 123 * km << "\n";
std::cout << std::format("Distance: {}\n", 123 * km);
std::cout << std::format("Distance: {:%N%?%U}\n", 123 * km);
```

!!! note

    For some quantities, the `{:%N %U}` format may provide a different output than the default one,
    as some units have `space_before_unit_symbol` customization point explicitly set to `false`
    (e.g., `%` and `°`).


#### Quantity numerical value, unit symbol, or both?

Thanks to the grammar provided above, the user can easily decide to either:

- print a whole quantity:

    ```cpp
    std::println("Speed: {}", 120 * km / h);
    ```

    ```text
    Speed: 120 km/h
    ```

- provide custom quantity formatting:

    ```cpp
    std::println("Speed: {:%N in %U}", 120 * km / h);
    ```

    ```text
    Speed: 120 in km/h
    ```

- provide custom formatting for components:

    ```cpp
    std::println("Speed: {::N[.2f]U[n]}", 100. * km / (3 * h));
    ```

    ```text
    Speed: 33.33 km h⁻¹
    ```

- print only specific components (numerical value, unit, or dimension):

    ```cpp
    std::println("Speed:\n- number: {0:%N}\n- unit: {0:%U}\n- dimension: {0:%D}", 120 * km / h);
    ```

    ```text
    Speed:
    - number: 120
    - unit: km/h
    - dimension: LT⁻¹
    ```

#### Formatting of the quantity numerical value

The representation type used as a numerical value of a quantity must provide its own formatter
specialization. It will be called by the quantity formatter with the format-spec provided
by the user in the `N` defaults specification.

In case we use C++ fundamental arithmetic types with our quantities the standard formatter
specified in [format.string.std](https://wg21.link/format.string.std) will be used. The rest
of this chapter assumes that it is the case and provides some usage examples.

`sign` token allows us to specify how the value's sign is being printed:

```cpp
std::println("{0},{0::N[+]},{0::N[-]},{0::N[ ]}", 1 * m);   // 1 m,+1 m,1 m, 1 m
std::println("{0},{0::N[+]},{0::N[-]},{0::N[ ]}", -1 * m);  // -1 m,-1 m,-1 m,-1 m
```

where:

- `+` indicates that a sign should be used for both non-negative and negative numbers,
- `-` indicates that a sign should be used for negative numbers and negative zero only
  (this is the default behavior),
- `<space>` indicates that a leading space should be used for non-negative numbers other
  than negative zero, and a minus sign for negative numbers and negative zero.

`precision` token is allowed only for floating-point representation types:

```cpp
std::println("{::N[.0]}", 1.2345 * m);   // 1 m
std::println("{::N[.1]}", 1.2345 * m);   // 1 m
std::println("{::N[.2]}", 1.2345 * m);   // 1.2 m
std::println("{::N[.3]}", 1.2345 * m);   // 1.23 m
std::println("{::N[.0f]}", 1.2345 * m);  // 1 m
std::println("{::N[.1f]}", 1.2345 * m);  // 1.2 m
std::println("{::N[.2f]}", 1.2345 * m);  // 1.23 m
```

`type` specifies how a value of the representation type is being printed.
For integral types:

```cpp
std::println("{::N[b]}", 42 * m);    // 101010 m
std::println("{::N[B]}", 42 * m);    // 101010 m
std::println("{::N[d]}", 42 * m);    // 42 m
std::println("{::N[o]}", 42 * m);    // 52 m
std::println("{::N[x]}", 42 * m);    // 2a m
std::println("{::N[X]}", 42 * m);    // 2A m
```

The above can be printed in an alternate version thanks to the `#` token:

```cpp
std::println("{::N[#b]}", 42 * m);   // 0b101010 m
std::println("{::N[#B]}", 42 * m);   // 0B101010 m
std::println("{::N[#o]}", 42 * m);   // 052 m
std::println("{::N[#x]}", 42 * m);   // 0x2a m
std::println("{::N[#X]}", 42 * m);   // 0X2A m
```

For floating-point values, the `type` token works as follows:

```cpp
std::println("{::N[a]}",   1.2345678 * m);      // 1.3c0ca2a5b1d5dp+0 m
std::println("{::N[.3a]}", 1.2345678 * m);      // 1.3c1p+0 m
std::println("{::N[A]}",   1.2345678 * m);      // 1.3C0CA2A5B1D5DP+0 m
std::println("{::N[.3A]}", 1.2345678 * m);      // 1.3C1P+0 m
std::println("{::N[e]}",   1.2345678 * m);      // 1.234568e+00 m
std::println("{::N[.3e]}", 1.2345678 * m);      // 1.235e+00 m
std::println("{::N[E]}",   1.2345678 * m);      // 1.234568E+00 m
std::println("{::N[.3E]}", 1.2345678 * m);      // 1.235E+00 m
std::println("{::N[g]}",   1.2345678 * m);      // 1.23457 m
std::println("{::N[g]}",   1.2345678e8 * m);    // 1.23457e+08 m
std::println("{::N[.3g]}", 1.2345678 * m);      // 1.23 m
std::println("{::N[.3g]}", 1.2345678e8 * m);    // 1.23e+08 m
std::println("{::N[G]}",   1.2345678 * m);      // 1.23457 m
std::println("{::N[G]}",   1.2345678e8 * m);    // 1.23457E+08 m
std::println("{::N[.3G]}", 1.2345678 * m);      // 1.23 m
std::println("{::N[.3G]}", 1.2345678e8 * m);    // 1.23E+08 m
```
