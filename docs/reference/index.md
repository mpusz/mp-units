# Reference

Welcome to the **mp-units** Reference documentation! This section provides quick lookups,
detailed API documentation, and comprehensive reference material for all library features.

## What is the Reference?

The Reference is **information-oriented** documentation designed for looking up specific
details when you already know what you need.

This section:

- ✅ Provides quick access to specific information
- ✅ Organizes content for efficient lookup
- ✅ Lists all available systems, units, and quantities
- ✅ Documents API details comprehensively

**Prerequisites**: Basic familiarity with **mp-units** concepts. Reference documentation
assumes you understand fundamentals and need specific details.

## Available References

### [Cheat Sheet](cheat_sheet.md)

Quick reference for common operations and patterns—your go-to page for syntax and usage
reminders.

### [Systems Reference](systems_reference/index.md)

Comprehensive listings of all available physical units systems:

<!-- markdownlint-disable MD013 -->
- **[Dimensions](systems_reference/dimensions_index.md)** - All base and derived dimensions
- **[Quantities](systems_reference/quantities_index.md)** - All quantity types
- **[Units](systems_reference/units_index.md)** - All unit definitions
- **[Prefixes](systems_reference/prefixes_index.md)** - Unit prefixes (kilo, mega, etc.)
- **[Point Origins](systems_reference/point_origins_index.md)** - Reference points for affine space
- **[Quantity Hierarchies](systems_reference/hierarchies/index.md)** - Detailed quantity type relationships
<!-- markdownlint-enable MD013 -->

### [API Reference](api_reference/overview.md)

Declarations for everything the library exposes: class templates, concepts,
functions and their signatures, generated from the headers with
[MrDocs](https://www.mrdocs.com) so they cannot drift from the code.

Units, quantities, dimensions and constants are not here - those live in the
[Systems Reference](systems_reference/index.md), which documents them as
metrology rather than as C++ declarations.

### [Glossary](glossary.md)

Definitions of metrology terms and concepts used throughout the library.

### [Bibliography](bibliography.md)

References to standards, papers, and resources that inform the library's design.

## How to Use the Reference

**Quick Lookup**: Use the [Cheat Sheet](cheat_sheet.md) for common operations and syntax.

**Find a Unit**: Browse [Systems Reference](systems_reference/index.md) to locate specific
units, quantities, or dimensions.

**API Details**: Check [API Reference](api_reference/overview.md) for detailed function signatures
and class interfaces.

**Terminology**: Consult the [Glossary](glossary.md) when you encounter unfamiliar metrology
terms.

## When to Use the Reference

Use the Reference when you:

- Need to quickly look up syntax or function names
- Want to find a specific unit or quantity
- Need precise API details
- Are looking for a specific piece of information
- Want to verify exact behavior or parameters

## Other Resources

- **[Tutorials](../tutorials/index.md)** - Learning-oriented lessons for getting started
- **[User's Guide](../users_guide/index.md)** - Explanation-oriented understanding of concepts
- **[How-to Guides](../how_to_guides/index.md)** - Task-oriented solutions to specific problems
- **[Workshops](../workshops/index.md)** - Practice advanced patterns
- **[Examples](../examples/index.md)** - Complete applications
