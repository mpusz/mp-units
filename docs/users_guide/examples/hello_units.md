---
tags:
  - International System
  - Text Formatting
---

# `hello_units`

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/Tsesa1Pvq)"

This is a really simple example showcasing the features of the **mp-units** library.

First, we include the headers for:

- a system of quantities (ISQ)
- symbols of SI units
- symbols of international units
- text and stream output support

```cpp title="hello_units.cpp" linenums="1"
--8<-- "example/hello_units.cpp:28:39"
```

Also, to shorten the definitions, we "import" `mp_units` namespace.

```cpp title="hello_units.cpp" linenums="12"
--8<-- "example/hello_units.cpp:40:41"
```

Next we define a simple function that calculates average speed based on the provided
arguments of length and time:

```cpp title="hello_units.cpp" linenums="13"
--8<-- "example/hello_units.cpp:42:45"
```

The above function template takes any quantities implicitly convertible to `isq::length`
and `isq::time` respectively. Those quantities can use any compatible unit and a
representation type. The function returns a result of a really simple equation and ensures
that its quantity type is implicitly convertible to `isq::speed`.

!!! tip

    Besides verifying the type returned from the function, constraining a generic return
    type is really useful for users of such a function as it provides more information
    of what to expect from a function than just using `auto`.

```cpp title="hello_units.cpp" linenums="17"
--8<-- "example/hello_units.cpp:47:50"
```

The above lines explicitly opt-in to use unit symbols from two systems of units.
As this introduces a lot of short identifiers into the current scope, it is not done
implicitly while including a header file.

```cpp title="hello_units.cpp" linenums="21"
--8<-- "example/hello_units.cpp:52:58"
```

- Lines `16` & `17` create a quantity of kind `isq::length / isq::time` with the numbers
  and units provided. Such quantities can be converted or assigned to any other quantity
  with a matching kind.
- Line `18` calls our function template with quantities of kind `isq::length` and
  `isq::time` and number and units provided.
- Line `19` explicitly provides quantity types of the quantities passed to a function template.
  This time those will not be quantity kinds anymore and will have
  [more restrictive conversion rules](../framework_basics/simple_and_typed_quantities.md#quantity_cast-to-force-unsafe-conversions).
- Line `20` changes the unit of a quantity `v3` to `m / s` in a
  [value-preserving way](../framework_basics/value_conversions.md#value-preserving-conversions)
  (floating-point representations are considered to be value-preserving).
- Line `21` does a similar operation but this time it would succeed also for
  [value-truncating cases](../framework_basics/value_conversions.md#value-truncating-conversions)
  (if it was the case).
- Line `22` does a [value-truncating conversion](../framework_basics/value_conversions.md#value-truncating-conversions)
  of changing the underlying representation type from `double` to `int`.

```cpp title="hello_units.cpp" linenums="28"
--8<-- "example/hello_units.cpp:60"
```

The above presents [various ways to print a quantity](../framework_basics/text_output.md).
Both stream insertion operations and `std::format` are supported.

!!! tip

    `MP_UNITS_STD_FMT` is used for compatibility reasons. In case a specific compiler
    does not support `std::format` or a user prefers to use `{fmt}` library, this macro
    will resolve to `fmt` namespace. Otherwise, `std` namespace will be used.
