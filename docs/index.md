# Welcome to **mp-units**!

**mp-units** is a compile-time enabled feature-rich Modern C++ header-only library that provides
compile-time dimensional analysis and unit/quantity manipulation. Its key strengths include safety,
performance, and developer experience.

The library source code is hosted on [GitHub](https://github.com/mpusz/mp-units) with a permissive
[MIT license](https://github.com/mpusz/units/blob/master/LICENSE.md).

!!! info "Help needed!"

    The **mp-units** library might be the subject of ISO standardization for C++29. More on this can
    be found in the ISO C++ proposal [P1935](https://wg21.link/p1935). We are actively looking for
    parties interested in field-trialing the library.

??? info "Supported compilers"

    This library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses C++20 features and the
    [explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
    from C++23.

    Even though the library benefits from C++23 (if available), C++20 is enough to compile and
    use all of the library's functionality. C++23 features are hidden behind a preprocessor macro
    providing a backward-compatible way to use it.

    Sadly, as of today, only a few compilers provide full C++20 support. The library compiles fine
    on the following compilers (or newer):

    - gcc-12.2

    In the upcoming weeks, we will be actively working to extend the support to other compilers as well.
