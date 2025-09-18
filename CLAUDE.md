# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

mp-units is a Modern C++ quantities and units library that provides compile-time dimensional analysis and unit/quantity manipulation. The library uses C++20 features (concepts, classes as NTTPs) for type safety and zero-runtime cost abstractions.

## Build System and Configuration

### Conan-Based Build System
- **Primary dependency manager**: Conan 2.0+ (not vcpkg or other package managers)
- **Build tool**: CMake 3.25+ with Conan integration
- **Configuration**: Use `conanfile.py` for dependency and build management

### Important: Always Ask About Conan Profile
**Before running any Conan commands, always ask the user which Conan profile to use.** Users may have:
- `default` (most common)
- Custom profiles like `gcc`, `clang`, `gcc13`, etc.
- No profiles configured (needs setup first)

Ask: "Which Conan profile would you like to use? (e.g., 'default', or run `conan profile list` to see available profiles)"

### Common Build Commands

#### Standard Development Build
```bash
# Single compiler build with all features (recommended for development)
conan build . -pr <your_profile> -c user.mp-units.build:all=True -b missing

# With specific C++ standard (adjust profile and cppstd for your system)
conan build . -pr <your_profile> -s compiler.cppstd=23 -c user.mp-units.build:all=True -b missing

# Using default profile (if configured)
conan build . -c user.mp-units.build:all=True -b missing
```

#### Multi-Compiler Testing (Codespace Environment)
```bash
# Test all compiler configurations (comprehensive - creates packages)
.devcontainer/check_all.sh create

# Build only (without package creation)
.devcontainer/check_all.sh build

# With debug builds included
.devcontainer/check_all.sh -d build

# Install dependencies only
.devcontainer/check_all.sh install
```

#### CMake Integration (when using mp-units as dependency)
```bash
# Install mp-units via Conan first
conan install . -pr <profile> -s compiler.cppstd=20

# In your CMakeLists.txt:
# find_package(mp-units REQUIRED)
# target_link_libraries(<your_target> mp-units::mp-units)
```

### Testing
- **Test framework**: Catch2 3.10.0 (automatically included with `user.mp-units.build:all=True`)
- **Run tests**: Tests run automatically during `conan build . -c user.mp-units.build:all=True`
- **Verification**: Interface header sets are verified automatically (unless using `import std;`)

#### Manual Testing Commands
After configuring the build with Conan, you can run specific test targets:

```bash
# Verify all interface header sets (ensures headers are correctly structured)
cmake --build --preset conan-release --target all_verify_interface_header_sets

# Run all unit tests
cmake --build --preset conan-release --target test
```

These commands are useful for:
- **Interface verification**: Ensures header files are compatible and properly structured
- **Test execution**: Runs the complete test suite using the configured build preset
- **Build validation**: Confirms code quality and compatibility after making changes

### Code Quality Tools
- **Formatter**: clang-format (with `.clang-format` config)
- **Linter**: clang-tidy (enable with `-c user.mp-units.analyze:clang-tidy=True`)
- **Pre-commit hooks**: `pre-commit run --all-files` (includes clang-format, cmake-format, black, flake8)
- **Static analysis**: `conan build . -c user.mp-units.analyze:clang-tidy=True`
- **Documentation**: `mkdocs serve` (live preview) or `mkdocs build` (static generation)

## Architecture Overview

### Modular Design
The library is organized into distinct modules:

#### Core Module (`src/core/`)
- **Location**: `src/core/include/mp-units/`
- **Purpose**: Fundamental quantity and unit types, dimensional analysis
- **Key components**: Concepts, basic quantity/unit definitions, type traits
- **Module file**: `src/core/mp-units-core.cpp` (for C++ modules build)

#### Systems Module (`src/systems/`)
- **Location**: `src/systems/include/mp-units/systems/`
- **Purpose**: Physical unit systems (SI, International, ISQ, etc.)
- **Key systems**:
  - `si/` - International System of Units
  - `isq/` - International System of Quantities  
  - `international/` - International units (miles, gallons, etc.)
- **Module file**: `src/systems/mp-units-systems.cpp`

#### Project Wrapper (`src/mp-units.cpp`)
- Combines core and systems modules
- Main entry point for C++ modules builds
- Dependencies: `mp-units::core` + `mp-units::systems`

### Key Configuration Options

#### API Configuration (set via CMake cache variables)
- `MP_UNITS_API_STD_FORMAT`: Enable std::format support (auto-detected)
- `MP_UNITS_API_NO_CRTP`: Use explicit `this` instead of CRTP (requires C++23)
- `MP_UNITS_API_CONTRACTS`: Contract checking (`NONE`, `GSL-LITE`, `MS-GSL`)
- `MP_UNITS_API_FREESTANDING`: Freestanding build (no hosted features)
- `MP_UNITS_API_NATURAL_UNITS`: Natural units support (default: ON)

#### Build Features
- `MP_UNITS_BUILD_CXX_MODULES`: Build as C++20 modules (requires CMake 3.29+)
- `CMAKE_CXX_MODULE_STD`: Enable `import std;` (experimental, requires CMake 3.30+)

## Development Guidelines

### Compiler Support Matrix
- **Minimum support**: GCC 12, Clang 16, Apple Clang 15, MSVC 194 (C++20)
- **std::format support**: GCC 13+, Clang 17+, Apple Clang 16+, MSVC 194
- **C++ modules**: Clang 17+, GCC (limited), MSVC (limited)
- **Explicit this (no-CRTP)**: GCC 14+, Clang 18+ (C++23)
- **Testing matrix**: GCC 12-15, Clang 16-20 (Clang 19 excluded due to compiler bug)

### Naming Conventions and Code Style
- **Types, functions, variables**: `standard_case` (e.g., `quantity`, `unit_symbol`)
- **Template parameters**: `PascalCase` (e.g., `QuantitySpec`, `UnitOf`)
- **Namespaces**: `standard_case` (e.g., `mp_units::si::unit_symbols`)
- **Constants**: `standard_case` (e.g., `speed_of_light`, `elementary_charge`)
- **Use pre-commit hooks**: Automatically formats code with clang-format

### Testing Different Configurations
The library supports multiple build configurations. Test important changes across:
- Different contract modes (`NONE`, `GSL-LITE`, `MS-GSL`)  
- With/without std::format support
- Freestanding vs hosted builds
- Traditional headers vs C++ modules
- Multiple compiler versions (use `.devcontainer/check_all.sh`)

### Code Organization Patterns
- **Header-only by default**: Most functionality in header files under `include/`
- **Module interface units**: `.cpp` files provide C++ module interfaces
- **System-specific headers**: Each unit system has its own subdirectory
- **Example code**: `example/` directory contains usage demonstrations

### Dependencies and Contracts
- **Contract libraries**: 
  - `gsl-lite` (default) - Lightweight GSL implementation
  - `ms-gsl` - Microsoft's GSL implementation  
  - `none` - No contract checking (required for freestanding)
- **Formatting**: 
  - `std::format` (preferred when available)
  - `fmt` library (fallback when std::format unavailable)
- **Test framework**: Catch2 3.10.0 (only when `user.mp-units.build:all=True`)
- **Freestanding mode**: No hosted dependencies, contracts must be `none`

### Documentation
- **User documentation**: Available at https://mpusz.github.io/mp-units
- **API reference**: Generate with `.devcontainer/api_reference.sh`
- **Examples**: See `example/` directory for usage patterns
- **Contributing**: See CONTRIBUTING.md for development setup

### Contribution Workflow
- **Issue selection**: Look for `good first issue`, `help wanted`, or `high priority` labels
- **Pre-commit hooks**: Always run `pre-commit run --all-files` before committing
- **Multi-compiler testing**: Use `.devcontainer/check_all.sh create` to test all configurations
- **Documentation**: Update docs with `mkdocs serve` for live preview
- **Backward compatibility**: Ensure changes work across all supported compiler configurations

### GitHub Codespaces (Recommended)
- **Quick start**: Use "Open in GitHub Codespaces" badge from the repository
- **Pre-configured environment**: Includes all compilers, tools, and dependencies
- **Full toolchain**: GCC 12-15, Clang 16-20, documentation tools, quality checks
- **Ready to use**: No local setup required, everything works out of the box