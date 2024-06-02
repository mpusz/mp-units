# C++ compiler support (API/ABI) { #cpp-compiler-support }

!!! info

    **mp-units** library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses the latest C++ language features.

    Even though the library benefits from the latest C++ versions (if available), C++20 is enough
    to compile and use all of the library's functionality. Newer features can be hidden behind
    some [preprocessor macros](../users_guide/use_cases/wide_compatibility.md#compatibility-macros)
    providing a backward-compatible way to use them.

The table below provides the minimum compiler version required to compile the code using a specific
C++ feature:

| C++ Feature                                               | C++ version | gcc  | clang | apple-clang | MSVC |
|-----------------------------------------------------------|:-----------:|:----:|:-----:|:-----------:|:----:|
| **Minimum support**                                       |     20      |  12  |  16   |     15      | None |
| **`std::format`**                                         |     20      |  13  |  17   |    None     | None |
| **C++ modules**                                           |     20      | None |  17   |    None     | None |
| **Static `constexpr` variables in `constexpr` functions** |     23      |  13  |  17   |    None     | None |
| **Explicit `this` parameter**                             |     23      |  14  |  18   |    None     | None |

!!! important

    Enabling/disabling features listed above may influence the API of the library and the ABI of
    the customers' projects.


## `std::format`

- Provides [powerful text formatting capabilities](../users_guide/framework_basics/text_output.md#text-formatting)
  for C++.
- An alternative [fmtlib](https://github.com/fmtlib/fmt) library can be used instead if
    - the C++ language feature is not supported,
    - the customer's project did not switch to `std::format` yet (even when the compiler
      supports it).
- To write code with wide compatibility
  a [dedicated macro may be used](../users_guide/use_cases/wide_compatibility.md#mp_units_std_fmt).
- Tested with `__cpp_lib_format` [feature test macro](https://en.cppreference.com/w/cpp/feature_test).
- Related build options:
    - Conan: [std_format](installation_and_usage.md#std_format)
    - CMake: [MP_UNITS_API_STD_FORMAT](installation_and_usage.md#MP_UNITS_API_STD_FORMAT)


## C++ modules

- Provide new way to share declarations and definitions across translation units.
- If used, the library will distribute both "old-style" headers and module interface units
    - associated with the same CMake targets.
- Even with full compiler support, a user may still decide to not pay for C++ modules compilation
  if they are not needed by the customer's project.
- Feature test macro is not used for testing here because even if the compiler does not support
  the entire C++ feature (e.g. header units), it is enough to build modules for this library.
- Related build options:
    - Conan: [cxx_modules](installation_and_usage.md#cxx_modules)
    - CMake: [MP_UNITS_BUILD_CXX_MODULES](installation_and_usage.md#MP_UNITS_BUILD_CXX_MODULES)

!!! note

    More requirements for C++ modules support can be found in the
    [CMake's documentation](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html).


## Static `constexpr` variables in `constexpr` functions

- Allows returning `std::string_view` from the
  [`unit_symbol()`](../users_guide/framework_basics/text_output.md#unit_symbol)
  and [`dimension_symbol()`](../users_guide/framework_basics/text_output.md#dimension_symbol)
  functions
    - `std::string_view` type has a reference semantics so it has to point to a storage with
      a longer lifetime.
- If this feature is not available, the API returns `mp_units::basic_fixed_string<CharT, N>` instead.
- Tested as `__cpp_constexpr >= 202211L` [feature test macro](https://en.cppreference.com/w/cpp/feature_test).
- Related build options:
    - Conan: [string_view_ret](installation_and_usage.md#string_view_ret)
    - CMake: [MP_UNITS_API_STRING_VIEW_RET](installation_and_usage.md#MP_UNITS_API_STRING_VIEW_RET)

## Explicit `this` parameter

- This feature removes the need for the usage of the CRTP idiom in the
  [`quantity_spec` definitions](../users_guide/framework_basics/systems_of_quantities.md#defining-quantities).
- To write code with wide compatibility
  a [dedicated macro may be used](../users_guide/use_cases/wide_compatibility.md#QUANTITY_SPEC).
- Tested with `__cpp_explicit_this_parameter` [feature test macro](https://en.cppreference.com/w/cpp/feature_test).
- Related build options:
    - Conan: [no_crtp](installation_and_usage.md#no_crtp)
    - CMake: [MP_UNITS_API_NO_CRTP](installation_and_usage.md#MP_UNITS_API_NO_CRTP)

*[CRTP]: Curiously Recurring Template Parameter
