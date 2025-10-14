# Introduction

**mp-units** is a modern C++ library for compile‑time dimensional analysis and
unit/quantity manipulation. The earliest versions were inspired by
[`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration),
but with each release the interfaces intentionally diverged to provide a better
user experience.

!!! info

  A brief introduction to the library's interfaces and the rationale for
  changes in version 2.0 of **mp-units** were provided in detail by
  [Mateusz Pusz](https://github.com/mpusz) in the
  ["The Power of C++ Templates With mp-units: Lessons Learned & a New
  Library Design" talk at the C++ on Sea 2023
  conference](https://www.youtube.com/watch?v=eUdz0WvOMm0).


## Open Source

**mp-units** is Free and Open Source under the permissive
[MIT license](https://github.com/mpusz/mp-units/blob/master/LICENSE.md). Browse the source,
ask questions, report bugs, or suggest improvements at
<https://github.com/mpusz/mp-units>.


## With the User's Experience in Mind

Most key design decisions aim to deliver the best possible user experience. Many other C++
physical‑units libraries are "famous" for enormous, hard‑to‑digest error messages.
**mp-units** strives to invert that reputation: making compile‑time errors concise, readable,
and actionable.

To achieve this goal, several techniques are applied:

- [Use of C++20 concepts](../users_guide/framework_basics/concepts.md) to improve
  compile-time performance and error readability vs traditional SFINAE‑based metaprogramming.
- [Strong types for framework entities](../users_guide/framework_basics/interface_introduction.md#strong-types-instead-of-aliases)
  instead of type aliases.
- [Symbolic expressions](../users_guide/framework_basics/interface_introduction.md#symbolic-expressions)
  for readable generated types.
- Minimizing the number of template parameters.

!!! important "Important: It is all about errors"

    In many generic C++ libraries, compile-time errors do not happen often. It is hard to
    break `std::string` or `std::vector` in a way that won't compile with a huge error
    log. Physical quantities and units libraries are different.
    **Generation of compile-time errors is the main reason to use such a library.**


## Key Features

### Safety

- [The affine space strong types] (`quantity`, `quantity_point`)
- [Compile-time checked conversions of quantities and units]
- [Unique support for many quantities of the same kind]
- [Type-safe equations on scalar, vector, and tensor quantities and their units]
- [Value-preserving conversions]

### Performance

- Compile-time logic via immediate (`consteval`) functions
- Performance on par with (sometimes better than) fundamental types
- Zero space overhead for high-level abstractions

### Great User Experience

- [Optimized for readable compilation errors and great debugging experience]
- [Efficient and composable way to specify a unit of choice]
- [Value-based dimension, unit, and quantity equations]

### Feature Rich

- [Systems of Quantities]
- [Systems of Units]
- [Scalar, vector, and tensor quantities]
- [The affine space]
- [Different models of the universe (e.g. natural units systems)]
- [Strong dimensionless quantities]
- [Strong angular system]
- [Supports any unit's magnitude (huge, small, floating-point)]
- [Faster-than-lightspeed constants]
- [Highly adjustable text-output formatting]

### Easy to Extend

- [Each entity can be defined with a single line of code]
- [User can easily extend the systems with custom dimensions, quantities, and units]
  
  
### Low Standardization Cost

- Few predefined entities due to high composability
- No external dependencies (with full C++20 support)
- Macro-free user interface (aside from portability / standard-compliance helpers)
- Plausible candidate for [freestanding] standardization
  
  
[The affine space strong types]: ../users_guide/framework_basics/the_affine_space.md
[Compile-time checked conversions of quantities and units]: ../users_guide/framework_basics/systems_of_quantities.md#converting-between-quantities
[Unique support for many quantities of the same kind]: ../users_guide/framework_basics/systems_of_quantities.md#quantities-of-the-same-kind
[Type-safe equations on scalar, vector, and tensor quantities and their units]: ../users_guide/framework_basics/quantity_arithmetics.md
[Value-preserving conversions]: ../users_guide/framework_basics/value_conversions.md#value-preserving-conversions

[Optimized for readable compilation errors and great debugging experience]: ../users_guide/framework_basics/simple_and_typed_quantities.md#easy-to-understand-compilation-error-messages
[Efficient and composable way to specify a unit of choice]: ../users_guide/framework_basics/systems_of_units.md#units-compose
[Value-based dimension, unit, and quantity equations]: ../users_guide/framework_basics/interface_introduction.md#value-based-equations

[Systems of Quantities]: ../users_guide/framework_basics/systems_of_quantities.md
[Systems of Units]: ../users_guide/framework_basics/systems_of_units.md
[Scalar, vector, and tensor quantities]: ../users_guide/framework_basics/character_of_a_quantity.md
[The affine space]: ../users_guide/framework_basics/the_affine_space.md
[Different models of the universe (e.g. natural units systems)]: ../users_guide/systems/natural_units.md
[Strong dimensionless quantities]: ../users_guide/framework_basics/dimensionless_quantities.md
[Strong angular system]: ../users_guide/systems/strong_angular_system.md
[Supports any unit's magnitude (huge, small, floating-point)]: ../users_guide/framework_basics/systems_of_units.md#scaled-units
[Faster-than-lightspeed constants]: ../users_guide/framework_basics/faster_than_lightspeed_constants.md
[Highly adjustable text-output formatting]: ../users_guide/framework_basics/text_output.md

[Each entity can be defined with a single line of code]: ../users_guide/framework_basics/interface_introduction.md#new-style-of-definitions
[User can easily extend the systems with custom dimensions, quantities, and units]: ../users_guide/use_cases/extending_the_library.md

[freestanding]: https://en.cppreference.com/w/cpp/freestanding
