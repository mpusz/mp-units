---
tags:
  - International System
  - Text Formatting
---

# `hello_units`

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/MYn5qjPzh)"

This is a really simple example showcasing the features of the **mp-units** library.

First, we either import the `mp_units` module or include the headers for:

- an International System of Quantities (ISQ),
- an International System of units (SI),
- units derived from the International Yard and Pound,
- text formatting and stream output support.

```cpp title="hello_units.cpp" linenums="1"
--8<-- "example/hello_units.cpp:28:45"
```

Also, to shorten the definitions, we "import" all the symbols from the `mp_units` namespace.

```cpp title="hello_units.cpp" linenums="18"
--8<-- "example/hello_units.cpp:46:47"
```

Next, we define a simple function that calculates the average speed based on the provided
arguments of length and time:

```cpp title="hello_units.cpp" linenums="19"
--8<-- "example/hello_units.cpp:48:51"
```

The above function template takes any quantities implicitly convertible to `isq::length`
and `isq::time`, respectively. Those quantities can use any compatible unit and a
representation type. The function returns a result of a straightforward equation and ensures
that its quantity type is implicitly convertible to `isq::speed`.

!!! tip

    Besides verifying the type returned from the function, constraining a generic return
    type is beneficial for users of such a function as it provides more information
    of what to expect from a function than just using `auto`.

```cpp title="hello_units.cpp" linenums="23"
--8<-- "example/hello_units.cpp:53:56"
```

The above lines explicitly opt into using unit symbols from two systems of units.
As this introduces a lot of short identifiers into the current scope, it is not done
implicitly while including a header file.

```cpp title="hello_units.cpp" linenums="27"
--8<-- "example/hello_units.cpp:58:64"
```

- Lines `27` & `28` create a quantity of kind `isq::length / isq::time` with the numbers
  and units provided. Such quantities can be converted or assigned to any other quantity
  with a matching kind.
- Line `29` calls our function template with quantities of kind `isq::length` and
  `isq::time` and number and units provided.
- Line `30` explicitly provides quantity types of the quantities passed to a function template.
  This time, those will not be quantity kinds anymore and will have
  [more restrictive conversion rules](../framework_basics/simple_and_typed_quantities.md#quantity_cast-to-force-unsafe-conversions).
- Line `31` changes the unit of a quantity `v3` to `m / s` in a
  [value-preserving way](../framework_basics/value_conversions.md#value-preserving-conversions)
  (floating-point representations are considered to be value-preserving).
- Line `32` does a similar operation, but this time, it would also succeed for
  [value-truncating cases](../framework_basics/value_conversions.md#value-truncating-conversions)
  (if that was the case).
- Line `33` does a [value-truncating conversion](../framework_basics/value_conversions.md#value-truncating-conversions)
  of changing the underlying representation type from `double` to `int`.

```cpp title="hello_units.cpp" linenums="34"
--8<-- "example/hello_units.cpp:66"
```

The above presents [various ways to print a quantity](../framework_basics/text_output.md).
Both stream insertion operations and `std::format` facilities are supported.

!!! tip

    `MP_UNITS_STD_FMT` is used for compatibility reasons. If a specific compiler
    does not support `std::format` or a user prefers to use the `{fmt}` library, this macro
    will resolve to `fmt` namespace. Otherwise, the `std` namespace will be used.

    More about it can be found in the [Wide Compatibility](../use_cases/wide_compatibility.md#mp_units_std_fmt)
    chapter.
