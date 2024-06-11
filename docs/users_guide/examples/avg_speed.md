---
tags:
  - CGS System
  - International System
  - Text Formatting
---

# `avg_speed`

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/TnqGa4sdn)"

Let's continue the previous example. This time, our purpose will not be to showcase as many
library features as possible, but we will scope on different interfaces one can provide
with the **mp-units**. We will also describe some advantages and disadvantages of presented
solutions.

First, we either import a module or include all the necessary header files and import all
the identifiers from the `mp_units` namespace:

```cpp title="avg_speed.cpp" linenums="1"
--8<-- "example/avg_speed.cpp:28:42"
```

Next, we define two functions calculating average speed based on quantities of fixed units
and integral and floating-point representation types, respectively, and a third function
that we introduced in the [previous example](hello_units.md):

```cpp title="avg_speed.cpp" linenums="16"
--8<-- "example/avg_speed.cpp:44:58"
```

We also added a simple utility to print our results:

```cpp title="avg_speed.cpp" linenums="31"
--8<-- "example/avg_speed.cpp:60:66"
```

Now, let's analyze how those three utility functions behave with different sets of arguments.
First, we are going to use quantities of SI units and integral representation:

```cpp title="avg_speed.cpp" linenums="38"
--8<-- "example/avg_speed.cpp:68:82"
```

The above provides the following output:

```text
SI units with 'int' as representation
Average speed of a car that makes 220 km in 2 h is 108 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
```

Please note that in the first two cases, we must convert length from `km` to `m` and
time from `h` to `s`. The converted values are used to calculate speed in `m/s` which
is then again converted to the one in `km/h`. Those conversions not only impact the
application's runtime performance but may also affect the precision of the final result.
Such truncation can be easily observed in the first case where we deal with integral
representation types (the resulting speed is `108 km/h`).

The second scenario is really similar to the previous one, but this time, function arguments
have floating-point representation types:

```cpp title="avg_speed.cpp" linenums="53"
--8<-- "example/avg_speed.cpp:84:95"
```

Conversion from floating-point to integral representation types is
[considered value-truncating](../framework_basics/value_conversions.md#value-truncating-conversions)
and that is why now, in the first case, we need an explicit call to `value_cast<int>`.

In the text output, we can observe that, again, the resulting value gets truncated during conversions
in the first cast:

```text
SI units with 'double' as representation
Average speed of a car that makes 220 km in 2 h is 108 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
Average speed of a car that makes 220 km in 2 h is 110 km/h.
```

Next, let's do the same for integral and floating-point representations, but this time
using international mile:

```cpp title="avg_speed.cpp" linenums="65"
--8<-- "example/avg_speed.cpp:97:129"
```

One important difference here is the fact that as it is not possible to make a lossless conversion
of miles to meters on a quantity using an integral representation type, so this time, we need a
`value_cast<m, int>` to force it.

If we check the text output of the above, we will see the following:

```text
International mile with 'int' as representation
Average speed of a car that makes 140 mi in 2 h is 111 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
Average speed of a car that makes 140 mi in 2 h is 112 km/h.

International mile with 'double' as representation
Average speed of a car that makes 140 mi in 2 h is 111 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
Average speed of a car that makes 140 mi in 2 h is 112.654 km/h.
```

Please note how the first and third results get truncated using integral representation types.

In the end, we repeat the scenario for CGS units:

```cpp title="avg_speed.cpp" linenums="97"
--8<-- "example/avg_speed.cpp:131:161"
```

Again, we observe `value_cast` being used in the same places and consistent truncation errors
in the text output:

```text
CGS units with 'int' as representation
Average speed of a car that makes 22000000 cm in 7200 s is 108 km/h.
Average speed of a car that makes 22000000 cm in 7200 s is 110 km/h.
Average speed of a car that makes 22000000 cm in 7200 s is 109 km/h.

CGS units with 'double' as representation
Average speed of a car that makes 2.2e+07 cm in 7200 s is 108 km/h.
Average speed of a car that makes 2.2e+07 cm in 7200 s is 110 km/h.
Average speed of a car that makes 2.2e+07 cm in 7200 s is 110 km/h.
```

The example file ends with a simple `main()` function:

```cpp title="avg_speed.cpp" linenums="128"
--8<-- "example/avg_speed.cpp:163:"
```
