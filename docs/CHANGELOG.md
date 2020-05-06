# Release notes

- **0.5.0 WIP**
  - Major refactoring and rewrite of the library
  - Units are now independent from dimensions
  - Dimensions now depend on units (base or coherent units are provided in a class template)
  - Quantity gets a Dimension template parameter again (as unit does not provide information about
    its dimension anymore)
  - Added official CGS system support
  - Added official data information system support
  - Repository file tree cleanup
  - `ratio` refactored to contain `Exp` template parameter (thanks a lot [@oschonrock](https://github.com/oschonrock)!)
  - SI fundamental constants added
  - `q_` prefix applied to all the UDLs (thanks [@kwikius](https://github.com/kwikius))
  - `unknown_unit` renamed to `unknown_coherent_unit`
  - Project documentation greatly extended and switched to Sphinx
  - A few more usage examples added
  - ASCII-only output support added (thanks [@yasamoka](https://github.com/yasamoka))
  - Representation values formatting extended (thanks [@rbrugo](https://github.com/rbrugo))
  - Output streams formatting support added
  - Linear algebra from `std::experimental::math` support added
  - Named SI units and their dimensions added (thanks [@rbrugo](https://github.com/rbrugo)
  - libfmt updated to 6.2.0
  - ...

  Many thanks to GitHub users [@oschonrock](https://github.com/oschonrock) and
  [@kwikius](https://github.com/kwikius) for their support in drafting a new library design.

- **0.4.0 Nov 17, 2019**
  - Support for derived dimensions in `exp` added
  - Added `pow()` and `sqrt()` operations on quantities
  - `units` removed from a `std::experimental` namespace
  - Downcasting facility refactored so the user does not have to write the boilerplate code anymore
  - From now on base dimensions should inherit from `base_dimension` class template
  - Added unit symbols definitions to `base_dimension` and derived units
  - Added support for `operator<<` on `quantity`
  - `fmt` support added
  - Derived unit factory helpers refactored
  - Refactored the way prefixed units are defined

- **0.3.1 Sep 18, 2019**
  - cmcstl2 dependency changed to range-v3 0.9.1

- **0.3.0 Sep 16, 2019 (CppCon 2019 design)**
  - The design as described on CppCon 2019 talk (<https://youtu.be/0YW6yxkdhlU>)
  - Applied the feedback from the Cologne evening session
    - `upcasting_traits` renamed to `downcasting_traits`
    - `Dimension` template parameter removed from quantity
  - `units` moved to a `std::experimental` namespace
  - Leading underscore prefix removed from UDLs
  - Added a few more derived dimensions
  - `meter` renamed to `metre`
  - Missing `operator*` added
  - Predefined dimensions moved to a dedicated directory
  - `dimension_` prefix removed from names of derived dimensions
  - cmcstl2 library updated to 2019.09.19
  - `base_dimension` is a value provided as `const&` to the `exp` type
  - integrated with Compiler Explorer
  - gsl-lite dependency removed
  - Fractional dimension exponents support added
  - `QuantityOf` concept introduced
  - `quantity_cast<U, Rep>()` support added

- **0.2.0 July 18, 2019 (C++Now 2019 design)**
  - The design as described on C++Now 2019 talk (<https://youtu.be/wKchCktZPHU>)
  - Added C++20 features supported by gcc-9.1 (`std::remove_cvref_t`, down with typename, `std::type_identity`)
  - Compile-time performance optimizations (`type_list`, `common_ratio`, `ratio`, `conditional_t`)

- **0.1.0 May 18, 2019**
  - Initial library release
  - Begin semantic versioning
  - The last version to work with gcc-8
