---
tags:
- Level - Beginner
- System - International System
- Feature - Text Formatting
- Feature - Quantity Concepts
- Feature - Unit Conversions
- Feature - Value Truncation
---

# Introduction to Quantities and Units

[Try it live on Compiler Explorer](https://godbolt.org/z/qzrb3n7bd){ .md-button }

## Overview

This is a really simple example showcasing the basic features of the **mp-units** library.
It demonstrates how to create quantities, perform calculations with them, convert between
units, and format output in various ways.

## Key Features Demonstrated

- Creating quantities with units
- Generic function templates with quantity concepts
- Quantity kinds vs. typed quantities
- Unit conversions (value-preserving and value-truncating)
- Text formatting and stream output

## Code Walkthrough

### Including Headers and Namespaces

First, we either import the `mp_units` module or include the headers for:

- an International System of Quantities (ISQ),
- an International System of Units (SI),
- units derived from the International Yard and Pound,
- text formatting and stream output support.

```cpp title="hello_units.cpp" linenums="1"
--8<-- "example/hello_units.cpp:28:43"
```

Also, to shorten the definitions, we "import" all the symbols from the `mp_units` namespace.

```cpp title="hello_units.cpp" linenums="16"
--8<-- "example/hello_units.cpp:44:45"
```

### Defining a Generic Speed Function

Next, we define a simple function that calculates the average speed based on the provided
arguments of length and time:

```cpp title="hello_units.cpp" linenums="17"
--8<-- "example/hello_units.cpp:46:49"
```

The above function template takes any quantities implicitly convertible to `isq::length`
and `isq::time`, respectively. Those quantities can use any compatible unit and a
representation type. The function returns a result of a straightforward equation and ensures
that its quantity type is implicitly convertible to `isq::speed`.

!!! tip

    Besides verifying the type returned from the function, constraining a generic return type
    is beneficial for users of such a function as it provides more information about what to
    expect from a function than just using `auto`.

### Using Unit Symbols

```cpp title="hello_units.cpp" linenums="21"
--8<-- "example/hello_units.cpp:51:54"
```

The above lines explicitly opt into using unit symbols from two systems of units.
As this introduces a lot of short identifiers into the current scope, it is not done
implicitly while including a header file.

### Creating and Converting Quantities

```cpp title="hello_units.cpp" linenums="25"
--8<-- "example/hello_units.cpp:56:62"
```

- Lines `25` & `26` create a quantity of kind `isq::length / isq::time` with the numbers
  and units provided. Such quantities can be converted or assigned to any other quantity
  with a matching kind.
- Line `27` calls our function template with quantities of kind `isq::length` and
  `isq::time` and number and units provided.
- Line `28` explicitly provides quantity types of the quantities passed to a function template.
  This time, those will not be quantity kinds anymore and will have
  [more restrictive conversion rules](../users_guide/framework_basics/simple_and_typed_quantities.md#quantity_cast-to-force-unsafe-conversions).
- Line `29` changes the unit of a quantity `v3` to `m / s` in a
  [value-preserving way](../users_guide/framework_basics/value_conversions.md#value-preserving-conversions)
  (floating-point representations are considered to be value-preserving).
- Line `30` does a similar operation, but this time, it would also succeed for
  [value-truncating cases](../users_guide/framework_basics/value_conversions.md#value-truncating-conversions)
  (if that was the case).
- Line `31` does a [value-truncating conversion](../users_guide/framework_basics/value_conversions.md#value-truncating-conversions)
  of changing the underlying representation type from `double` to `int`.

### Formatting Output

```cpp title="hello_units.cpp" linenums="32"
--8<-- "example/hello_units.cpp:64"
```

The above presents [various ways to print a quantity](../users_guide/framework_basics/text_output.md).
Both stream insertion operations and `std::format` facilities are supported.

!!! tip

    `MP_UNITS_STD_FMT` is used for compatibility reasons. If a specific compiler does not
    support `std::format` or a user prefers to use the `{fmt}` library, this macro will
    resolve to `fmt` namespace. Otherwise, the `std` namespace will be used.

    More about it can be found in the [Wide Compatibility](../users_guide/use_cases/wide_compatibility.md#mp_units_std_fmt)
    chapter.

## Related Concepts

<!-- markdownlint-disable MD013 -->
- [Quantity Concepts](../users_guide/framework_basics/concepts.md#QuantityOf) - Understanding `QuantityOf`
- [Simple and Typed Quantities](../users_guide/framework_basics/simple_and_typed_quantities.md) - Quantity kinds vs. typed quantities
- [Value Conversions](../users_guide/framework_basics/value_conversions.md) - Different conversion types
- [Text Output](../users_guide/framework_basics/text_output.md) - Formatting quantities
