---
tags:
  - International System
  - Text formatting
---

# `hello_units`

This is a really simple example showcasing the features of the **mp-units** library.

```cpp title="hello_units.cpp" linenums="1"
--8<-- "example/hello_units.cpp:28:33"
```

First, we include the headers for:

- a system of quantities (ISQ)
- symbols of SI units
- symbols of international units
- text and stream output support

```cpp title="hello_units.cpp" linenums="7"
--8<-- "example/hello_units.cpp:35:36"
```

Next, to shorten the definitions, we "import" `mp_units` namespace.

```cpp title="hello_units.cpp" linenums="8"
--8<-- "example/hello_units.cpp:37:40"
```

The above function template takes any quantities implicitly convertible to `isq::length`
and `isq::time`. Those quantities can use any compatible unit and a representation type.
The function returns a result of a really simple equation and ensures that its quantity
type is implicitly convertible to `isq::speed`.

!!! note

    Besides verifying the type returned from the function, constraining a generic return
    type is really useful for users of such a function as it provides more information
    of what to expect from a function than just using `auto`.

```cpp title="hello_units.cpp" linenums="12"
--8<-- "example/hello_units.cpp:42:45"
```

The above lines explicitly opt-in to use unit symbols from two systems of units.
As this introduces a lot of short identifiers into the current scope, it is not done
implicitly while including a header file.

```cpp title="hello_units.cpp" linenums="16"
--8<-- "example/hello_units.cpp:47:53"
```

- Lines `16` & `17` create a quantity of kind `isq::length / isq::time` with the numbers
  and units provided. Such quantities can be converted or assigned to any other quantity
  with a matching kind.
- Line `18` calls our function template with quantities of kind `isq::length` and
  `isq::time` and number and units provided.
- Line `19` explicitly specifies quantity specifications of the quantities passed
  to a function template. This time those will not be quantity kinds anymore and will
  have more restrictive conversion rules.
- Line `20` changes the unit of a quantity `v3` to `m / s` in a value-preserving way
  (floating-point representations are considered to be value-preserving).
- Line `21` does a similar operation but this time it would succeed also for non-truncating
  cases (if it was the case).
- Line `22` does a value-truncating operation of changing the underlying representation
  type from `double` to `int`.

```cpp title="hello_units.cpp" linenums="23"
--8<-- "example/hello_units.cpp:55"
```

The above presents various ways to print a quantity. Both stream insertion operations
and `std::format` are supported.

!!! note

    `MP_UNITS_STD_FMT` is used for compatibility reasons. In case a specific compiler
    does not support `std::format` or a user prefers to use `{fmt}` library, this macro
    will resolve to `fmt` namespace. Otherwise, `std` namespace will be used.
