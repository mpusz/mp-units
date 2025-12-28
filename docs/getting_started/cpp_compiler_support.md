# C++ compiler support (API/ABI) { #cpp-compiler-support }

!!! info

    **mp-units** library tries to provide the best user experience possible with the C++ language.
    To achieve that, it extensively uses the latest C++ language features.

    Even though the library benefits from the latest C++ versions (if available), C++20 is enough
    to compile and use all of the library's functionality. Newer features can be hidden behind
    some [preprocessor macros](../users_guide/use_cases/wide_compatibility.md#compatibility-macros)
    providing a backward-compatible way to use them.

The table below provides the minimum compiler version required to compile the code using
a specific C++ feature:

| C++ Feature                   | C++ version | gcc  |   clang    |  apple-clang   |                    MSVC                    |
|-------------------------------|:-----------:|:----:|:----------:|:--------------:|:------------------------------------------:|
| **Minimum support**           |     20      | 12+  | 16+ && !19 | 15-16 && !17.0 | 194+ :bug:{ title="BEWARE of MSVC Bugs!" } |
| **`std::format`**             |     20      | 13+  |    17+     |     16+     |                    194+                    |
| **C++ modules**               |     20      | None |    17+     |    None     |                    None                    |
| **`import std;`**             |     23      | None |    18+     |    None     |                    None                    |
| **Explicit `this` parameter** |     23      | 14+  |    18+     |    17+      |                    194+                    |

??? note "Clang-19 unfixable bug"

    Unfortunately, Clang-19 does not build **mp-units** because of an
    [unfixable bug in the compiler](https://github.com/llvm/llvm-project/pull/118288).

??? note "Apple Clang-17.0 unfixable bug"

    Unfortunately, Apple Clang-17.0 (Xcode 16.3, 16.4, 26.0, 26.0.1, 26.1, and 26.2-beta) does not
    build **mp-units** because it has the same
    [unfixable bug as Clang-19](https://github.com/llvm/llvm-project/pull/118288).

??? note "MSVC bugs"

    MSVC still has a poor C++20 conformance. We had to make many workarounds to our codebase
    to make it compile on this compiler. Usage of such nasty preprocessor macros degrade the
    readability and maintainability of our code. This is why we've applied those patches to
    the main library code but not to unit tests and examples. Those still do not compile on
    MSVC.

    Here is a list of the most important MSVC bugs:

    - [Syntax error when using non-type template parameters in templated class member function](https://developercommunity.visualstudio.com/t/Syntax-error-when-using-non-type-templat/10729428)
    - [Type always preferred over value when using qualified identifiers](https://developercommunity.visualstudio.com/t/Type-always-prefered-over-value-when-usi/10729382)

    Please upvote them so they get a higher fixing priority at Microsoft.

!!! important

    Enabling/disabling features listed above may influence the API of the library and the ABI of
    the customers' projects.


## `std::format`

- Provides
  [powerful text formatting capabilities](../users_guide/framework_basics/text_output.md#text-formatting)
  for C++.
- An alternative [fmtlib](https://github.com/fmtlib/fmt) library can be used instead if
    - the C++ language feature is not supported,
    - the customer's project did not switch to `std::format` yet (even when the compiler
      supports it).
- To write code with wide compatibility a
  [dedicated macro may be used](../users_guide/use_cases/wide_compatibility.md#mp_units_std_fmt).
- Tested with `__cpp_lib_format` [feature test macro](https://en.cppreference.com/w/cpp/feature_test).
- Related build options:
    - Conan: [std_format](installation_and_usage.md#std_format)
    - CMake: [MP_UNITS_API_STD_FORMAT](installation_and_usage.md#MP_UNITS_API_STD_FORMAT)


## C++ modules

- Provide new way to share declarations and definitions across translation units.
- If used, the library will distribute both "old-style" headers and module interface units
    - associated with the same CMake targets.
- Even with full compiler support, a user may still decide to not pay for C++ modules
  compilation if they are not needed by the customer's project.
- Feature test macro is not used for testing here because even if the compiler does not
  support the entire C++ feature (e.g. header units), it is enough to build modules for this
  library.
- Related build options:
    - Conan: [cxx_modules](installation_and_usage.md#cxx_modules)
    - CMake: [MP_UNITS_BUILD_CXX_MODULES](installation_and_usage.md#MP_UNITS_BUILD_CXX_MODULES)

!!! note

    More requirements for C++ modules support can be found in the
    [CMake's documentation](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html).


## `import std;`

- If enabled, the library will obtain all the definitions from the `std` namespace via
  `import std;` instead of the "old-style" header includes.
- Related build options:
    - Conan: [import_std](installation_and_usage.md#import_std)
    - CMake: [CMAKE_CXX_MODULE_STD](https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_MODULE_STD.html)


## Explicit `this` parameter

- This feature removes the need for the usage of the CRTP idiom in the
  [`quantity_spec` definitions](../users_guide/framework_basics/systems_of_quantities.md#defining-quantities).
- To write code with wide compatibility
  a [dedicated macro may be used](../users_guide/use_cases/wide_compatibility.md#QUANTITY_SPEC).
- Tested with `__cpp_explicit_this_parameter` [feature test macro](https://en.cppreference.com/w/cpp/feature_test).
    - Note that some compiler versions do not implement this macro even though they do support the
      feature well enough. In such cases, compilation with explicit `this` is enforced.
- Related build options:
    - Conan: [no_crtp](installation_and_usage.md#no_crtp)
    - CMake: [MP_UNITS_API_NO_CRTP](installation_and_usage.md#MP_UNITS_API_NO_CRTP)

*[CRTP]: Curiously Recurring Template Parameter
