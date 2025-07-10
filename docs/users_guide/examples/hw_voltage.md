---
tags:
  - Affine Space
  - Embedded
  - Text Formatting
---

# `hw_voltage`

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/jjod7hvsd)"

As it was stated in [The Affine Space](../framework_basics/the_affine_space.md) chapter,
every measurement can (and probably should) be modelled as a `quantity_point`. This is
a perfect example of such a use case.

This example implements a simplified scenario of measuring voltage read from hardware through
a mapped 16-bits register. The actual voltage range of [-10 V, 10 V] is mapped to [-32767, 32767]
on hardware. Translation of the value requires not only scaling of the value but also applying
of an offset.

First we include all the dependencies:

```cpp title="hw_voltage.cpp" linenums="1"
--8<-- "example/hw_voltage.cpp:28:43"
```

Next, we specify the real measurement voltage range to be in the range of [-10, 10]:

```cpp title="hw_voltage.cpp" linenums="17"
--8<-- "example/hw_voltage.cpp:45:48"
```

and provide a storage type and special values for the hardware representation:

```cpp title="hw_voltage.cpp" linenums="21"
--8<-- "example/hw_voltage.cpp:50:56"
```

Finally, we define a quantity point origin, an offset unit that scales the value and uses this
origin to offset the zero of the sale, and a dedicated quantity point alias using those:

```cpp title="hw_voltage.cpp" linenums="28"
--8<-- "example/hw_voltage.cpp:60:66"
```

Now, when everything is ready, we can simulate mapping of our hardware register, and provide
a helper function that will read the value and construct a quantity point from the obtained copy:

```cpp title="hw_voltage.cpp" linenums="35"
--8<-- "example/hw_voltage.cpp:69:77"
```

We also provide a simple print helper for our quantity points:

```cpp title="hw_voltage.cpp" linenums="44"
--8<-- "example/hw_voltage.cpp:79:83"
```

In the main function we simulate setting of 3 values by our hardware. Each of them is read
and printed in the voltage unit used on the hardware as well as in the standard SI unit:

```cpp title="hw_voltage.cpp" linenums="49"
--8<-- "example/hw_voltage.cpp:85:"
```

The above program results with the following text output:

```text
     0 hwV (-10 V)
 32767 hwV (  0 V)
 65534 hwV ( 10 V)
```
