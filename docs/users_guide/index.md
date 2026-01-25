# User's Guide

Welcome to the **mp-units** User's Guide! This comprehensive guide covers all library
concepts in depth, providing a thorough understanding of type-safe physical quantities
and units in C++.

## What is the User's Guide?

The User's Guide is **explanation-oriented** documentation that deepens your understanding
of the library's concepts, architecture, and design philosophy.

This guide:

- ✅ Explains concepts thoroughly with context and rationale
- ✅ Covers all library features systematically
- ✅ Provides deep technical understanding
- ✅ Can be read sequentially like a book

**Prerequisites**: Familiarity with library basics from [Tutorials](../tutorials/index.md).
The User's Guide builds on tutorial knowledge to provide comprehensive coverage of all
concepts.

## Guide Structure

### [Terms and Definitions](terms_and_definitions.md)

Essential metrology vocabulary and definitions used throughout the library and documentation.

### Framework Basics

Core concepts that form the foundation of **mp-units**:

<!-- markdownlint-disable MD013 -->
- [Interface Introduction](framework_basics/interface_introduction.md) - Library API overview
- [Design Overview](framework_basics/design_overview.md) - Architecture and design principles
- [Systems of Quantities](framework_basics/systems_of_quantities.md) - Quantity hierarchies and relationships
- [Systems of Units](framework_basics/systems_of_units.md) - Unit systems and their organization
- [Simple and Typed Quantities](framework_basics/simple_and_typed_quantities.md) - Two approaches to quantity creation
- [Value Conversions](framework_basics/value_conversions.md) - Converting between representations
- [Character of a Quantity](framework_basics/character_of_a_quantity.md) - Scalar, vector, and tensor quantities
- [Dimensionless Quantities](framework_basics/dimensionless_quantities.md) - Handling ratios and counts
- [Quantity Arithmetics](framework_basics/quantity_arithmetics.md) - Mathematical operations
- [Generic Interfaces](framework_basics/generic_interfaces.md) - Writing flexible, type-safe code
- [Faster-than-lightspeed Constants](framework_basics/faster_than_lightspeed_constants.md) - Compile-time constant handling
- [The Affine Space](framework_basics/the_affine_space.md) - Points, quantities, and origins
- [Type Introspection](framework_basics/type_introspection.md) - Inspecting quantity types at compile time
- [Concepts](framework_basics/concepts.md) - C++20 concepts used throughout the library
- [Text Output](framework_basics/text_output.md) - Formatting and printing quantities
<!-- markdownlint-enable MD013 -->

### [Systems](systems/index.md)

Detailed documentation of available unit systems (SI, CGS, Imperial, astronomical units,
natural units, etc.) with guidance on choosing the right one for your application domain.

## How to Use This Guide?

**Sequential Reading**: Start with [Terms and Definitions](terms_and_definitions.md) and
progress through the Framework Basics chapters to build a complete mental model of the
library.

**Reference Reading**: Jump directly to specific chapters when you need to understand a
particular concept in depth.

**Domain-Focused**: After covering Framework Basics, explore the [Systems](systems/index.md)
chapter most relevant to your application domain.

## When to Use the User's Guide?

Read the User's Guide when you:

- Want deep understanding of library concepts
- Need to know *why* something works a certain way
- Are designing complex systems using **mp-units**
- Want to master advanced features
- Need comprehensive coverage of a topic

## Other Resources

- **[Tutorials](../tutorials/index.md)** - Learning-oriented lessons for getting started
- **[How-to Guides](../how_to_guides/index.md)** - Task-oriented solutions to specific problems
- **[Workshops](../workshops/index.md)** - Hands-on practice with advanced patterns
- **[Examples](../examples/index.md)** - Complete, runnable applications
- **[Reference](../reference/index.md)** - Information-oriented technical descriptions
