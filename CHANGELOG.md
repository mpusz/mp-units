# Release Notes

## mp-units

### 2.2.0 <small>WIP</small> { id="2.2.0" }

- (!) feat: C++ modules support added by [@JohelEGP](https://github.com/JohelEGP)
- (!) feat: formatting grammar improved and units formatting support added
- (!) feat: `has_unit_symbol` support removed
- (!) feat: ABI concerns resolved with introduction of u8 strings for symbols
- feat: implicit point origins support added
- feat: unit default point origin support added
- feat: `fma`, `isfinite`, `isinf`, and `isnan` math function added by [@NAThompson](https://github.com/NAThompson)
- feat: `quantity_point` support added for `quantity_cast` and `value_cast`
- feat: `value_cast<Unit, Representation>` added
- feat: `value_cast<Quantity>(q)`, `value_cast<Quantity>(qp)` and `value_cast<QuantityPoint>(qp)` added by [@burnpanck](https://github.com/burnpanck)
- feat: `interconvertible(QuantitySpec, QuantitySpec)` added
- feat: `qp.quantity_from_zero()` added
- feat: `underlying_type` type trait added
- feat: do not print space between a number and `percent` or `per_mille`
- feat: `ppm` parts per million added by [@nebkat](https://github.com/nebkat)
- feat: `atan2` 2-argument arctangent added by [@nebkat](https://github.com/nebkat)
- feat: `fmod` floating-point division remainder added by [@nebkat](https://github.com/nebkat)
- feat: `remainder` IEEE division remainder added by [@nebkat](https://github.com/nebkat)
- feat: `std::format` support added
- feat: unit text output support added
- feat: formatting error messages improved
- feat: improve types readability by eliminating extraneous `()` in references, prefixes, and `kind_of`
- (!) refactor: `zero_Fahrenheit` renamed to `zeroth_degree_Fahrenheit`
- (!) refactor: SI-related trigonometric functions moved to the `si` subnamespace
- (!) refactor: `math.h` header file broke up to smaller pieces
- (!) refactor: `fixed_string` interface refactored
- (!) refactor: `ReferenceOf` does not take a dimension anymore
- (!) refactor: 'o' replaced with '1' as a modifier for `unit_symbol_solidus::one_denominator`
- (!) refactor: `get_kind()` now returns `kind_of`
- refactor: math functions constraints refactored
- refactor: `si_quantities.h` added to improve compile-times
- refactor: `validate_ascii_string` refactored to `is_basic_literal_character_set`
- fix: `QuantityLike` conversions required `Q::rep` instead of using one provided by `quantity_like_traits`
- fix: `QuantitySpec[Unit]` replaced with `make_reference` in `value_cast`
- fix: `ice_point` is now defined with the integral offset from `absolute_zero`
- fix: performance regression in `sudo_cast` fixed
- fix: explicit object parameter support fixed
- fix: missing `version` header file added to `hacks.h`
- docs: project blog and first posts added
- docs: project documentation layout refactored
- docs: "Interoperability with Other Libraries" chapter added
- docs: "Framework Basics" chapters updated and cleaned up
- docs: `smoot` unit example added to the main page
- docs: "Code Example" chapter renamed to "Look and Feel" and reordered in TOC to be after "Quick Start"
- docs: "Quick Start" chapter reworked to be simpler and include quantity points
- docs: "Quantity points" chapter extended
- docs: "The Affine Space" chapter updated to reflect the recent design changes
- docs: "Working with Legacy interfaces" chapter added
- docs: "Text Output" chapter updated
- docs: mkdocs social plugin enabled
- docs: project logo and custom color scheme added
- docs: minimum compiler requirements updated
- docs: Cairo dependency described in the MkDocs section
- (!) build: Conan and CMake options refactored
- (!) build: `MP_UNITS_AS_SYSTEM_HEADERS` support removed
- build: gsl-lite updated to 0.41.0
- build: catch2 updated to 3.5.1
- build: fmt updated to 10.2.1
- build: gitpod environment updated
- build: `check_cxx_feature_supported` added
- build(conan): `generate()` now set `cache_variables`
- build(conan): `can_run` check added before running tests

### 2.1.0 <small>December 9, 2023</small> { id="2.1.0" }

- (!) feat: `inverse()` support added for dimensions, quantity_spec, units, and references
            (`1 / s` will now create `quantity` and not a `Unit`)
- (!) feat: `quantity_point` does not provide `zero()` anymore
- (!) feat: `quantity_spec` and its kind should not compare equal
- (!) feat: mutating interface removed from `fixed_string`
- (!) feat: `common_type` with a raw value is not needed anymore as for a long time now raw values are
  not convertible to the dimensionless quantities
- (!) feat: `symbol_text` definition simplified
- (!) feat: users are now allowed to inherit their own types from absolute point origins
- (!) feat: interoperability with other libraries redesigned
- feat: `basic_fixed_string(const CharT*, std::integral_constant<std::size_t, N>)` constructor added
- feat: `isq::activity` added and `becquerel` definition updated to benefit from it
- feat: `gray` and `sievert` now have correct associated quantity kinds
- feat: `UnitCompatibleWith` concept added and applied to `in(U)` and `force_in(U)` functions
- feat: quantities can now be multiplied and divided by units (no parenthesis needed anymore)
- feat: `Magnitude / Unit` operator added
- feat: equality for dimensions now will allow derived classes as well (but not from `derived_dimension`)
- feat: `zero_Fahrenheit` point origin added
- feat: equivalent point origins handling improved
- feat(example): unit symbols added to the currency example
- (!) refactor: `unit_symbol<fmt>(U)` signature refactored and the resulting text can now also be used at runtime
- (!) refactor: `make_xxx` factory functions replaced with two-parameter constructors
- (!) refactor: `unit_symbol` changed to `consteval`
- refactor: `in(U)` and `force_in(U)` now return `auto` to provide better diagnostics on clang
- refactor: `quantity` operators constraints refactored
- refactor: more type members added to `fixed_string` definition
- refactor: `unit_symbol_formatting` enums now use `std::int8_t` as a representation type
- fix: symbols of named dimensionless units with the ratio = 1 were not printed
- fix: iterator is now properly updated for all cases in `unit_symbol`
- fix: Fahrenheit conversion ratio was inverted
- fix: `CommonlyInvocableQuantities` was overconstrained for the current library design
- fix: `are_ingredients_convertible` now mandates explicit conversion for `To` dimensionless quantities
- fix: `quantity_point::point_for(PO)` constraints fixed
- fix(example): `latitude` and `longitude` fixed to include `0` for `N` and `E` respectively
- ci: clang-17 enabled
- ci: apple-clang-15 enabled
- ci: Added C++23 builds to the CI matrix
- docs: "Getting Started" chapters updated
- docs: "Basic Concepts" and "Interface Introduction" chapters updated
- docs: "Design Overview" chapter added and "Concepts" chapter reworked
- docs: "Output stream formatting" chapter updated
- docs: "Default formatting" chapter updated
- docs: "Derived unit symbols generation" chapter added
- docs: outdated affine space chapter updated
- docs: `CameCase` concept identifiers FAQ added
- docs: `gravitational_potential_energy` equation fixed on a graph
- docs: YouTube video link updated to the C++ on Sea 2023
- docs: ISO papers reference added to docs and README
- docs: a representation type in a dimensionless quantity FAQ fixed
- docs: titles added to some important admonitions
- docs: "Terms and Definitions" slightly updated
- docs: "canonical unit" added to glossary and its documentation in code was updated
- docs: Design overview graph updated

### 2.0.0 <small>September 24, 2023</small> { id="2.0.0" }

- `units` namespace renamed to `mp_units` (#317)
- header files in the `<mp-units/...>` rather then in `<units/...>` (#317)
- the downcasting facility is removed (#383, #211, #32)
- unified and simplified quantity creation (#274)
- determining the best way to create a quantity (#413)
- V2 `quantity_point` (#414)
- introduction of `quantity_spec` to store not only `dimension` but also additional information about quantities (#405)
- `quantity` now takes `reference` object, which aggregates `quantity_spec` and a `unit` and a `representation` type
- units, prefixes, dimensions, quantity specifications, and references are passed as NTTPs to templates and provide arithmetic operations and comparison
- expression templates consistently used in all derived types to increase the readability (#351, #166)
- derived dimensions are now factors of only base dimensions (#281)
- convertibility of derived quantities (#427)
- dimensions, quantity specifications, units, and references are now composable, significantly reducing the number of definitions and resulting types
- heavily simplified unit systems definitions (no need to define unnamed derived units, systems-specific dimensions, aliases for quantities, concepts, UDLs, ... anymore)
- improved definition of all systems
- support for ISO 80000 Part 3-6 quantities
- faster than lightspeed constants (#169)
- extensions to quantity formatting with `fmt`
- `quantity_kind` removed
- improved casting of unit with `.in(Unit)`, `.force_in(Unit)` for `quantity` and `quantity_spec`
- numerical value accessor safety improved with `.numerical_value_in(Unit)` and `.force_numerical_value_in(Unit)`
- `quantity` can no longer be constructed with a raw value (#434)
- Implicit construction of quantities from a value (#410)
- `quantity_point` can no longer be constructed with just a `quantity` and an explicit `PointOrigin` is always needed
- `ceil` and `floor` are dangerous (#432)
- quecto, ronto, ronna, quetta new SI prefixes support
- comparison against zero added (#487)
- documentation rewritten from scratch
- many smaller changes not possible to address with the previous design (#205, #210, #134)

### 0.8.0 <small>June 14, 2023</small> { id="0.8.0" }

- (!) refactor: `common_quantity`, `common_quantity_for`, `common_quantity_point`, `common_quantity_kind`, and `common_quantity_point_kind` removed
- (!) refactor: `named_derived_unit` removed as it was not used
- (!) refactor: `derived_unit` renamed to `derived_scaled_unit`
- (!) refactor: `unit` renamed to `derived_unit`
- (!) refactor: `U::is_named` removed from the unit types and replaced with `NamedUnit` concept
- (!) refactor: `PrefixFamily` support removed
- (!) refactor: `mi(naut)` renamed to `nmi`
- (!) refactor: `knot` unit helper renamed to `kn` in FPS
- (!) refactor: `knot` text symbol changed from `"knot"` to `"kn"`
- refactor: `quantity` `op+()` and `op-()` reimplemented in terms of `reference` rather then `quantity` types
- refactor(example): `glide_computer` now use dimensionless quantities with `ranged_representation` as `rep`
- feat: HEP system support added (thanks [@RalphSteinhagen](https://github.com/RalphSteinhagen))
- feat: `floor()`, `ceil()`, and `round()` support added (thanks [@hofbi](https://github.com/hofbi))
- feat: `std::format` support for compliant compilers added
- feat: conversion helpers from `mp-units` to `std::chrono` types added
- feat: math functions can now be safely used with user-defined types
- feat: conversion from `quantity_point` to `std::chrono::time_point` added
- feat: `nautical_mile_per_hour` and `knot` added to `si::international` system
- (!) fix: add `quantity_point::origin`, like `std::chrono::time_point::clock`
- fix: enable any prefixes for most of the named units (beside those that use prefixes already)
- fix: `hectare` definition fixed to be a prefixed version of `are` + other units
- fix: account for different dimensions in `quantity_point_cast`'s constraint
- fix: output stream operator now properly handles state
- fix: `fmt` algorithms were overconstrained with `forward_iterator`
- fix: CTAD for aliases fixed
- fix: `derived_ratio` calculation
- fix: `fill_t` assignment operator fixed
- fix: improve downcast mode off
- fix: `radioactivity` header compilation fixed
- fix: `si::hep::dim_momentum` duplicated definition fixed
- fix: `fps` can now coexist with `international` system
- fix: public headers fixed to be standalone
- test: standalone public headers tests added
- (!) build: CMake generator in Conan is no longer obtained from an environment variable
- (!) build: Required Conan version bumped to 1.48
- (!) build: Conan 1.48 does not set `CMAKE_BUILD_TYPE` in the `conan_toolchain.cmake` anymore
- build: AppleClang 13 support added (thanks [@fdischner](https://github.com/fdischner))
- build: most of the `conanfile.py` refactored to be Conan 2.0 ready
- build: `validate()` replaced with `configure()` to raise errors during `conan install` in Conan 1.X
- build: minimum Conan version changed to 1.40
- build: `linear-algebra` Conan repo is no needed anymore
- build: Gitpod support added
- build: clang-format-15 support added
- build: export config to local build (#322)
- build: fix export name of `mp-units-system`
- build: fmt updated to 8.0.1
- build: gsl-lite updated to 0.40.0
- build: catch2 updated to 2.13.9
- build: doxygen updated to 1.9.4
- build: linear_algebra/0.7.0 switched to wg21-linear_algebra/0.7.2
- ci: VS2022, gcc-11, clang-13, clang-14, and AppleClang 13 support added
- ci: pre-commit support added (thanks [@hofbi](https://github.com/hofbi))
- docs: Project documentation updated
- docs: `CITATION.cff` file added
- docs: `CONTRIBUTING.md` updated

### 0.7.0 <small>May 11, 2021</small> { id="0.7.0" }

- (!) refactor: `ScalableNumber` renamed to `Representation`
- (!) refactor: output stream operators moved to the `units/quantity_io.h` header file
- (!) refactor: Refactored the library file tree
- (!) refactor: `quantity::count()` renamed to `quantity::number()`
- (!) refactor: `data` system renamed to `isq::iec80000` (quantity names renamed too)
- (!) refactor: `*deduced_unit` renamed to `*derived_unit`
- (!) refactor: got rid of a `noble_derived_unit`
- refactor: quantity (kind) point updated to reflect latest changes to `quantity`
- refactor: basic concepts, `quantity` and `quantity_cast` refactored
- refactor: `abs()` definition refactored to be more explicit about the return type
- feat: quantity (point) kind support added (thanks [@johelegp](https://github.com/johelegp))
- feat: quantity references support added (thanks [@johelegp](https://github.com/johelegp))
- feat: quantity aliases support addded
- feat: interoperability with `std::chrono::duration` and other units libraries
- feat: CTAD for dimensionless quantity added
- feat: `modulation_rate` support added (thanks [@go2sh](https://github.com/go2sh))
- feat: SI prefixes for `isq::iec80000` support added (thanks [@go2sh](https://github.com/go2sh))
- feat: a possibility to disable quantity UDLs support with `UNITS_NO_LITERALS` preprocessor define
- feat: a support to define ISQ derived dimensions in terms of different number or order of components
- perf: preconditions check do not influence the runtime performance of a Release build
- perf: `quantity_cast()` generates less assembly instructions
- perf: temporary string creation removed from `quantity::op<<()`
- perf: value initialization for quantity value removed (left with a default initialization)
- perf: limited the `equivalent` trait usage
- perf: limited the C++ Standard Library headers usage
- perf: rvalue references support added for constructors and getters
- (!) fix: `exp()` has sense only for dimensionless quantities
- (!) fix: `dim_torque` now properly divides by an angle (instead of multiply) + default unit name change
- fix: quantity's operators fixed to behave like the underlying types do
- fix: `quantity_cast()` fixed to work correctly with representation types not convertible from `std::intmax_t`
- fix: ambiguous case for empty type list resolved
- fix: downcasting facility for non-default-constructible types
- fix: restore user-warnings within the library implementation
- fix: the text symbol of `foot_pound_force` and `foot_pound_force_per_second`
- fix: quantity modulo arithmetics fixed
- (!) build: Conan testing version is now hosted on [Artifactory](https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units)
- (!) build: Linear Algebra is now hosted on its [Artifactory](https://twonington.jfrog.io/artifactory/api/conan/conan-oss)
- (!) build: `BUILD_DOCS` CMake option renamed to `UNITS_BUILD_DOCS`
- build: doxygen updated to 1.8.20
- build: catch2 updated to 2.13.4
- build: fmt updated to 7.1.3
- build: ms-gsl replaced with gsl-lite/0.38.0
- build: Conan generator switched to `cmake_find_package_multi`
- build: Conan CMakeToolchain support added
- build: CMake scripts cleanup
- build: ccache support added
- ci: CI switched from Travis CI to GitHub Actions

### 0.6.0 <small>September 13, 2020</small> { id="0.6.0" }

- feat: `quantity_point` support added (thanks [@johelegp](https://github.com/johelegp))
- feat: Added angle as SI base dimension (thanks [@kwikius](https://github.com/kwikius))
- feat: `si::angular_velocity` support added (thanks [@mikeford3](https://github.com/mikeford3))
- feat: FPS system added (thanks [@mikeford3](https://github.com/mikeford3))
- feat: Added support for mathematical function `exp(quantity)`
- feat: Localization support for text output added (thanks [@rbrugo](https://github.com/rbrugo))
- feat: Added STL random number distribution wrappers (thanks [@yasamoka](https://github.com/yasamoka))
- (!) refactor: Refactored and cleaned up the library file tree
- (!) refactor: `q_*` UDL renamed to `_q_*`
- (!) refactor: UDLs with "per" in name renamed from `*p*` to `*_per_*`
- (!) refactor: `ratio` changed to the NTTP kind
- (!) refactor: `exp` and `Exp` renamed to `exponent` and `Exponent`
- (!) refactor: `Scalar` concept renamed to `ScalableNumber`
- (!) refactor: Dimensionless quantities redesigned to be of a `quantity` type
- refactor: `math.h` function signatures refactored to use a `Quantity` concept (thanks [@kwikius](https://github.com/kwikius))
- refactor: `[[nodiscard]]` added to many functions
- fix: `si::day` unit symbol fixed to `d` (thanks [@komputerwiz](https://github.com/komputerwiz))
- fix: `si::mole` unit symbol fixed to `mol` (thanks [@mikeford3](https://github.com/mikeford3))
- (!) build: gcc-9 is no longer supported (at least gcc-10 is required)
- build: Visual Studio 16.7 support added
- build: linear_algebra updated to 0.7.0/stable
- build: fmt updated to 7.0.3
- build: range-v3 updated to 0.11.0
- build: catch2 updated to 2.13.0
- build: doxygen updated to 1.8.18
- build: ms-gsl 3.1.0 dependency added
- build: Removed the dependency on a git submodule with common CMake scripts

### 0.5.0 <small>May 17, 2020</small> { id="0.5.0" }

- Major refactoring and rewrite of the library
- Units are now independent from dimensions
- Dimensions now depend on units (base or coherent units are provided in a class template)
- Quantity gets a Dimension template parameter again (as unit does not provide information about
  its dimension anymore)
- Spaceship operator support added
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
- Added absolute functions and epsilon to math.h (thanks [@mikeford3](https://github.com/mikeford3))
- Added a lot of prefixes to named units and introduced `alias_unit` (thanks [@yasamoka](https://github.com/yasamoka))
- Linking with Conan targets only when they exists ([#98](https://github.com/mpusz/units/issues/98))
- All physical dimensions and units put into `physical` namespace
- CMake improvements
- Velocity renamed to speed

Many thanks to GitHub users [@oschonrock](https://github.com/oschonrock),
[@kwikius](https://github.com/kwikius), and [@i-ky](https://github.com/i-ky) for their support
in drafting a new library design.

### 0.4.0 <small>Nov 17, 2019</small> { id="0.4.0" }

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

### 0.3.1 <small>Sep 18, 2019</small> { id="0.3.1" }

- cmcstl2 dependency changed to range-v3 0.9.1

### 0.3.0 <small>Sep 16, 2019</small> { id="0.3.0" }

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

### 0.2.0 <small>July 18, 2019</small> { id="0.2.0" }

- The design as described on C++Now 2019 talk (<https://youtu.be/wKchCktZPHU>)
- Added C++20 features supported by gcc-9.1 (`std::remove_cvref_t`, down with typename, `std::type_identity`)
- Compile-time performance optimizations (`type_list`, `common_ratio`, `ratio`, `conditional_t`)

### 0.1.0 <small>May 18, 2019</small> { id="0.1.0" }

- Initial library release
- Begin semantic versioning
- The last version to work with gcc-8
