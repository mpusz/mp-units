# Introduction

**mp-units** is a Modern C++ library that provides compile-time dimensional analysis and unit/quantity
manipulation. The initial versions of the library were inspired by the
[`std::chrono::duration`](https://en.cppreference.com/w/cpp/chrono/duration)
but with each release, the interfaces diverged from the original to provide a better user
experience.


## Open Source

**mp-units** is Free and Open Source, with a permissive
[MIT license](https://github.com/mpusz/mp-units/blob/master/LICENSE.md). Check out the source
code and issue tracking (for questions and support, reporting bugs, suggesting feature requests
and improvements) at <https://github.com/mpusz/mp-units>.


## With the User's Experience in Mind

Most of the critical design decisions in the library are dictated by the requirement of
providing the best user experience possible. Other C++ physical units libraries are
"famous" for their enormous and hard-to-understand error messages (one line of the error log often
do not fit on one slide). The ultimate goal of **mp-units** is to improve this and make compile-time
errors and debugging as easy and user-friendly as possible.

To achieve this goal, several techniques are applied:

- usage of C++20 concepts,
- using strong types for framework entities (instead of type aliases),
- usage of expression templates to improve the readability of generated types,
- limiting the number of template arguments to the bare minimum.

!!! note

    In many generic C++ libraries compile-time errors do not happen often. It is hard to
    break `std::string` or `std::vector` in a way it won't compile with a huge error
    log. Physical Units libraries are different. **Generation of compile-time errors
    is the main reason to use such a library.**


## Key Features

| Feature                      | Description                                                                                                                                                                                                                                                                                                                                                                      |
|------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Safety**                   | - The affine space strong types (`quantity` and `quantity_point`)<br>- Compile-time checked conversions of quantities and units<br>- Unique support for many quantities of the same kind<br>- Type-safe equations on scalar, vector, and tensor quantities and their units<br>- Value-preserving conversions                                                                     |
| **Performance**              | - All the compile-time logic implemented as immediate (`consteval`) functions<br>- As fast or even faster than working with fundamental types<br>- No space size overhead needed to implement high-level abstractions                                                                                                                                                            |
| **Great User Experience**    | - Optimized for readable compilation errors and great debugging experience<br>- Efficient and composable way to specify a unit of choice<br>- Value-based dimension, unit, and quantity equations                                                                                                                                                                                |
| **Feature Rich**             | - Systems of Quantities<br>- Systems of Units<br>- Scalar, vector, and tensor quantities<br>- The affine space<br>- [Natural units systems](https://en.wikipedia.org/wiki/Natural_units) support<br>- Strong angular system<br>- Supports any unit's magnitude (huge, small, floating-point)<br>- Faster-than-lightspeed constants<br>- Highly adjustable text-output formatting |
| **Easy to Extend**           | - Each entity can be defined with a single line of code<br>- User can easily extend the systems with custom dimensions, quantities, and units                                                                                                                                                                                                                                    |
| **Low Standardization Cost** | - Small number of predefined entities needed thanks to composability<br>- No external dependencies (assuming full C++20 support)<br>- No macros in the user interface (besides portability and standard-compliance issues)<br>- Possibility to be standardized as a [freestanding](https://en.cppreference.com/w/cpp/freestanding) part of the C++ Standard Library              |
