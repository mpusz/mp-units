# Introduction

**mp-units** is a Modern C++ library that provides compile-time dimensional analysis and unit/quantity
manipulation. The initial versions of the library were inspired by the
[`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration)
but with each release, the interfaces diverged from the original to provide a better user
experience.

!!! info

    A brief introduction to the library's interfaces and the rationale for changes in version 2.0
    of **mp-units** were provided in detail by [Mateusz Pusz](https://github.com/mpusz) in the
    ["The Power of C++ Templates With mp-units: Lessons Learned & a New Library Design" talk at the C++ on Sea 2023 conference](https://www.youtube.com/watch?v=eUdz0WvOMm0).


## Open Source

**mp-units** is Free and Open Source, with a permissive
[MIT license](https://github.com/mpusz/mp-units/blob/master/LICENSE.md). Check out the source
code and issue tracking (for questions and support, reporting bugs, suggesting feature requests
and improvements) at <https://github.com/mpusz/mp-units>.


## With the User's Experience in Mind

Most of the critical design decisions in the library are dictated by the requirement of
providing the best user experience possible. Other C++ physical units libraries are
"famous" for their enormous and hard-to-understand error messages (one line of the error log often
does not fit on one slide). The ultimate goal of **mp-units** is to improve this and make compile-time
errors and debugging as easy and user-friendly as possible.

To achieve this goal, several techniques are applied:

- [usage of C++20 concepts](../users_guide/framework_basics/concepts.md) that improve
  compile-times and the readability of error messages when compared to the traditional template
  metaprogramming with [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae),
- [usage of strong types for framework entities](../users_guide/framework_basics/interface_introduction.md#strong-types-instead-of-aliases) (instead of type aliases),
- [usage of expression templates](../users_guide/framework_basics/interface_introduction.md#expression-templates) to improve the readability of generated types,
- limiting the number of template arguments to the bare minimum.

!!! important "Important: It is all about errors"

    In many generic C++ libraries, compile-time errors do not happen often. It is hard to
    break `std::string` or `std::vector` in a way that won't compile with a huge error
    log. Physical quantities and units libraries are different.
    **Generation of compile-time errors is the main reason to use such a library.**


## Key Features

| Feature                      | Description                                                                                                                                                                                                                                                                                                                                                                                                                |
|------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Safety**                   | - [The affine space strong types] (`quantity` and `quantity_point`)<br>- [Compile-time checked conversions of quantities and units]<br>- [Unique support for many quantities of the same kind]<br>- [Type-safe equations on scalar, vector, and tensor quantities and their units]<br>- [Value-preserving conversions]                                                                                                     |
| **Performance**              | - All the compile-time logic implemented as immediate (`consteval`) functions<br>- As fast or even faster than working with fundamental types<br>- No space size overhead needed to implement high-level abstractions                                                                                                                                                                                                      |
| **Great User Experience**    | - [Optimized for readable compilation errors and great debugging experience]<br>- [Efficient and composable way to specify a unit of choice]<br>- [Value-based dimension, unit, and quantity equations]                                                                                                                                                                                                                    |
| **Feature Rich**             | - [Systems of Quantities]<br>- [Systems of Units]<br>- [Scalar, vector, and tensor quantities]<br>- [The affine space]<br>- [Different models of the universe (e.g. natural units systems)]<br>- [Strong dimensionless quantities]<br>- [Strong angular system]<br>- [Supports any unit's magnitude (huge, small, floating-point)]<br>- [Faster-than-lightspeed constants]<br>- [Highly adjustable text-output formatting] |
| **Easy to Extend**           | - [Each entity can be defined with a single line of code]<br>- [User can easily extend the systems with custom dimensions, quantities, and units]                                                                                                                                                                                                                                                                          |
| **Low Standardization Cost** | - Small number of predefined entities thanks to their composability<br>- No external dependencies (assuming full C++20 support)<br>- No macros in the user interface (besides portability and standard-compliance issues)<br>- Possibility to be standardized as a [freestanding] part of the C++ Standard Library                                                                                                         |


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
[Different models of the universe (e.g. natural units systems)]: ../users_guide/defining_systems/natural_units.md
[Strong dimensionless quantities]: ../users_guide/framework_basics/dimensionless_quantities.md
[Strong angular system]: ../users_guide/defining_systems/strong_angular_system.md
[Supports any unit's magnitude (huge, small, floating-point)]: ../users_guide/framework_basics/systems_of_units.md#scaled-units
[Faster-than-lightspeed constants]: ../users_guide/framework_basics/faster_than_lightspeed_constants.md
[Highly adjustable text-output formatting]: ../users_guide/framework_basics/text_output.md

[Each entity can be defined with a single line of code]: ../users_guide/framework_basics/interface_introduction.md#new-style-of-definitions
[User can easily extend the systems with custom dimensions, quantities, and units]: ../users_guide/use_cases/extending_the_library.md

[freestanding]: https://en.cppreference.com/w/cpp/freestanding
