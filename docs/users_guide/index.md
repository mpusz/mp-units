# User's Guide

Consider `1 * Hz + 1 * Bq + 1 * Bd`. Hertz measures _frequency_, becquerel measures
_activity_, and baud measures _modulation rate_. All three share the dimension
$\mathsf{T}^{-1}$, so a library that checks only dimensions cannot reject the sum, yet
adding them is meaningless. **mp-units** rejects it, because it models quantities the way
ISO 80000 defines them.

The [Tutorials](../tutorials/index.md) teach how to write code with the library. This
guide explains why the code looks the way it does: which problem a feature solves and
which part of the model solves it. It is the explanation part of the documentation.

Much of that model comes from metrology. The library follows the International System of
Quantities (ISQ) and uses the vocabulary that ISO and the BIPM define for measurement.
Most C++ developers have never needed either, so the guide introduces the terms where they
first matter, and the [Glossary](../reference/glossary.md) collects them in one place.

## Who this guide is for

The guide assumes you know C++ and have written a few quantities with the library, for
example by working through the [Quick Start](../tutorials/index.md#1-quick-start-30-minutes)
tutorials. It assumes no knowledge of ISO 80000.

Read it when you need to know why the library refuses a conversion, or how to model your
own domain on top of it. Questions about design alternatives that were considered and
rejected are answered in the [FAQ](../getting_started/faq.md).

## Recommended reading path

1. Skim [Terms and Definitions](terms_and_definitions.md) and keep the
   [Glossary](../reference/glossary.md) at hand.
2. Read the Framework Basics chapters in order. Each one builds on the ones before it.
3. Read the [Systems](systems/index.md) chapters that match your application domain.
4. Turn to the Advanced Topics when you write your own representation type, need the
   complete conversion rules between quantity specifications, inspect types at compile
   time, or constrain generic code with the library's concepts.

You can also jump straight to a single chapter and follow its links back to the terms it
relies on.

## Guide Structure

### [Terms and Definitions](terms_and_definitions.md)

The metrology vocabulary used throughout the library and its documentation.

### Framework Basics

The core model of **mp-units**, in the recommended reading order:

<!-- markdownlint-disable MD013 -->
- [Interface Introduction](framework_basics/interface_introduction.md) - How library entities are defined and spelled
- [Design Overview](framework_basics/design_overview.md) - The entities of the library and how they relate
- [Systems of Quantities](framework_basics/systems_of_quantities.md) - Quantity hierarchies and kinds
- [Systems of Units](framework_basics/systems_of_units.md) - Units, prefixes, and how unit systems are organized
- [Simple and Typed Quantities](framework_basics/simple_and_typed_quantities.md) - Two ways to create a quantity
- [Value Conversions](framework_basics/value_conversions.md) - Converting the unit and the representation of a value
- [Character of a Quantity](framework_basics/character_of_a_quantity.md) - Scalar, vector, and tensor quantities
- [`safe_int<T>`](framework_basics/safe_int.md) - Overflow-safe integer arithmetic wrapper
- [Dimensionless Quantities](framework_basics/dimensionless_quantities.md) - Ratios, counts, and quantities of dimension one
- [Quantity Arithmetics](framework_basics/quantity_arithmetics.md) - Mathematical operations on quantities
- [Generic Interfaces](framework_basics/generic_interfaces.md) - Writing functions that accept many quantity types
- [Faster-than-lightspeed Constants](framework_basics/faster_than_lightspeed_constants.md) - Physical constants as units, simplified at compile time
- [The Affine Space](framework_basics/the_affine_space.md) - Points, differences, and origins
- [Text Output](framework_basics/text_output.md) - Formatting and printing quantities
<!-- markdownlint-enable MD013 -->

### [Systems](systems/index.md)

The systems of quantities and units that ship with the library (SI, CGS, yard-pound,
astronomical, natural units, and others), with guidance on choosing one for your domain.

### Advanced Topics

Topics you need when you extend the library or write generic code against it:

<!-- markdownlint-disable MD013 -->
- [Representation Types](advanced_topics/representation_types.md) - Requirements on numeric types and how to plug in your own
- [Quantity Specification Conversions](advanced_topics/quantity_spec_conversions.md) - The complete rules for converting between quantity specifications
- [Type Introspection](advanced_topics/type_introspection.md) - Inspecting quantity types at compile time
- [Concepts](advanced_topics/concepts.md) - The C++20 concepts the library defines
<!-- markdownlint-enable MD013 -->

## Other Resources

- **[Tutorials](../tutorials/index.md)** - Learning-oriented lessons for getting started
- **[How-to Guides](../how_to_guides/index.md)** - Task-oriented solutions to specific problems
- **[Workshops](../workshops/index.md)** - Hands-on practice with advanced patterns
- **[Examples](../examples/index.md)** - Complete, runnable applications
- **[Reference](../reference/index.md)** - Information-oriented technical descriptions
- **[FAQ](../getting_started/faq.md)** - Design decisions and the alternatives we rejected
