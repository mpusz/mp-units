# Wide Compatibility

The **mp-units** allows us to implement nice and terse code targeting a specific C++ version and
configuration. Such code is easy to write and understand but might not be portable to some older
environments.

However, sometimes, we want to develop code that can be compiled on a wide range of various
compilers and configurations. This is why the library also exposes and uses special preprocessor
macros that can be used to ensure the wide compatibility of our code.

!!! note

    Those macros are used in our short [example applications](../../examples/tags_index.md) as
    those are meant to be built on all of
    [the supported compilers](../../getting_started/cpp_compiler_support.md). Some still do not
    support `std::format`, C++ modules, or C++ versions newer than C++20.


## Various compatibility options

Depending on your compiler's conformance, you can choose to use any of the below styles to write
your code using **mp-units**:

=== "C++23"

    ```cpp
    #include <format>
    #include <iostream>
    import mp_units;

    // ...

    inline constexpr struct horizontal_length final : quantity_spec<isq::length> {} horizontal_length;

    // ...

    std::cout << std::format(...) << "\n";
    ```

=== "C++20"

    ```cpp
    #include <format>
    #include <iostream>
    import mp_units;

    // ...

    inline constexpr struct horizontal_length final : quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    std::cout << std::format(...) << "\n";
    ```

=== "C++20 with header files"

    ```cpp
    #include <mp-units/systems/international.h>
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <format>
    #include <iostream>

    // ...

    inline constexpr struct horizontal_length final : quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    std::cout << std::format(...) << "\n";
    ```

=== "C++20 with header files + libfmt"

    ```cpp
    #include <mp-units/systems/international.h>
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #include <fmt/format.h>
    #include <iostream>

    // ...

    inline constexpr struct horizontal_length final : quantity_spec<horizontal_length, isq::length> {} horizontal_length;

    // ...

    std::cout << fmt::format(...) << "\n";
    ```

=== "Wide Compatibility"

    ```cpp
    #include <iostream>
    #include <mp-units/ext/format.h>
    #ifdef MP_UNITS_MODULES
    #include <mp-units/compat_macros.h>
    import mp_units;
    #else
    #include <mp-units/systems/international.h>
    #include <mp-units/systems/isq.h>
    #include <mp-units/systems/si.h>
    #endif

    // ...

    QUANTITY_SPEC(horizontal_length, isq::length);

    // ...

    std::cout << MP_UNITS_STD_FMT::format(...) << "\n";
    ```

!!! tip

    Depending on your preferences, you can either write:

    - terse code directly targeting your specific compiler's abilities,
    - verbose code using preprocessor branches and macros that provide the widest compatibility
      across various compilers.


## Compatibility macros

This chapter describes only the most essential tools the **mp-units** users need.
All the compatibility macros can be found in the _mp-units/compat_macros.h_ header file.

!!! tip

    The _mp-units/compat_macros.h_ header file is implicitly included when we use "legacy" headers
    in our translation units. However, it has to be explicitly included when we use C++20 modules,
    as those do not propagate preprocessor macros.

### `QUANTITY_SPEC(name, ...)` { #QUANTITY_SPEC }

Quantity specification definitions benefit from an
[explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
added in C++23 to remove the need for CRTP idiom, which significantly simplifies the code.

This macro benefits from the new C++23 feature if available. Otherwise, it uses the CRTP idiom under
the hood.

*[CRTP]: Curiously Recurring Template Parameter

### `MP_UNITS_STD_FMT`

Some of the supported compilers do not support [std::format](https://en.cppreference.com/w/cpp/utility/format/format)
and related tools. Also, despite using a conformant compiler, some projects still choose to
use [fmtlib](https://github.com/fmtlib/fmt) as their primary formatting facility (e.g., to benefit
from additional features provided with the library).

This macro resolves to either the `std` or `fmt` namespace, depending on the value of
[MP_UNITS_API_STD_FORMAT](../../getting_started/installation_and_usage.md#MP_UNITS_API_STD_FORMAT)
CMake option.

To include the header files of the underlying text formatting framework, the following include
should be used:

```cpp
#include <mp-units/ext/format.h>
```

### Contracts

The mp-units library internally does contract checking by default. It can be disabled with a Conan
or CMake option. However, when enabled, it can use either [gsl-lite](https://github.com/gsl-lite/gsl-lite)
or [ms-gsl](https://github.com/microsoft/GSL). To write a code that is independent from the
underlying framework, the following preprocessor macros are exposed:

- `MP_UNITS_EXPECTS(expr)`
- `MP_UNITS_EXPECTS_DEBUG(expr)`
- `MP_UNITS_ASSERT(expr)`
- `MP_UNITS_ASSERT_DEBUG(expr)`

Their meaning is consistent with respective [gsl-lite](https://github.com/gsl-lite/gsl-lite?tab=readme-ov-file#contract-checking-configuration-macros).

Also, to include the header files of the underlying framework, the following include should be used:

```cpp
#include <mp-units/ext/contracts.h>
```
