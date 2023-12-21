# Quick Start

This chapter provides a quick introduction to get you started with **mp-units**.
Much more details can be found in our [User's Guide](../users_guide/terms_and_definitions.md).

## Quantities

A **quantity** is a concrete amount of a unit representing a quantity type of a specified dimension with a
specific representation. It is represented in the library with a `quantity` class template.

The [SI Brochure](../appendix/references.md#SIBrochure) says:

!!! quote "SI Brochure"

    The value of the quantity is the product of the number and the unit. The space between the number
    and the unit is regarded as a multiplication sign (just as a space between units implies
    multiplication).


Following the above, the value of a quantity in the **mp-units** library is created by multiplying
a number with a predefined unit:

```cpp
#include <mp-units/systems/si/si.h>

using namespace mp_units;

quantity q = 42 * si::metre / si::second;
```

!!! info

    In case someone doesn't like the multiply syntax or there is an ambiguity between `operator*`
    provided by this and other libraries, a quantity can also be created with a two-parameter
    constructor:

    ```cpp
    #include <mp-units/systems/si/si.h>

    using namespace mp_units;

    quantity q{42, si::metre / si::second};
    ```

The above creates an instance of `quantity<derived_unit<si::metre, per<si::second>>{}, int>`.
The same can be obtained using optional unit symbols:

```cpp
#include <mp-units/systems/si/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

quantity q = 42 * m / s;
```

!!! tip

    Unit symbols introduce a lot of short identifiers into the current scope, and that is
    why they are opt-in. A user has to explicitly "import" them from a dedicated `unit_symbols`
    namespace.

Quantities of the same kind can be added, subtracted, and compared to each other:

```cpp
#include <mp-units/systems/si/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

static_assert(1 * km + 50 * m == 1050 * m);
```

Various quantities can be multiplied or divided by each other:

```cpp
static_assert(140 * km / (2 * h) == 70 * km / h);
```

!!! note

    In case you wonder why this library does not use UDLs to create quantities, please check
    our [FAQ](faq.md#why-dont-we-use-udls-to-create-quantities).


## Quantity points

The `quantity_point` class template specifies an absolute quantity with respect to an origin.
Together with quantities they model [The Affine Space](../users_guide/framework_basics/the_affine_space.md).

```cpp
#include <mp-units/ostream.h>
#include <mp-units/systems/si/si.h>
#include <mp-units/systems/usc/usc.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;
  using namespace mp_units::usc::unit_symbols;

  quantity_point temp = si::zeroth_degree_Celsius + 20. * deg_C;
  std::cout << "Temperature: "
            << temp.quantity_from(si::zeroth_degree_Celsius) << " ("
            << temp.in(deg_F).quantity_from(usc::zeroth_degree_Fahrenheit) << ")\n";
}
```

The above outputs:

```text
Temperature: 20 °C (68 °F)
```
