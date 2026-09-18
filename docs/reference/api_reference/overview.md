# API Reference

C++ declarations for everything the library exposes: class templates, concepts,
functions and their signatures, generated from the headers.

## What is here, and what is not

This section answers **"what does this C++ entity look like?"** — the template
parameters of [`quantity`][mp_units::quantity], what
[`quantity_point`][mp_units::quantity_point] inherits, which overloads of `in()`
exist.

It is not where you look up a unit or a quantity. Units, quantities, dimensions,
prefixes, constants and point origins, with their symbols and hierarchies, live in
the [Systems Reference](../systems_reference/index.md). The two overlap on purpose:
a unit appears here as the C++ object you name in code, and there as the
metrological definition behind it.

## Where to start

- **[`mp_units`](mrdocs/mp_units/index.md)** — the framework: quantities, quantity
  points, units, dimensions, references and the concepts that constrain them.
- **[`mp_units::utility`](mrdocs/mp_units/utility/index.md)** — representation types
  that are not part of the standardization effort: the vector and tensor types,
  `uncertain`, the random generators.
- **The systems** — [`si`](mrdocs/mp_units/si/index.md),
  [`isq`](mrdocs/mp_units/isq/index.md), [`iec`](mrdocs/mp_units/iec/index.md),
  [`hep`](mrdocs/mp_units/hep/index.md),
  [`angular`](mrdocs/mp_units/angular/index.md) and the rest, each with its own
  `unit_symbols` namespace holding the short identifiers you write in code.

## Reading a page

A symbol page opens with its **Synopsis**: the declaration as written, with every
type in it linked to its own page.

Where the library promises nothing, the synopsis says so rather than showing the
detail. That is most often a base class existing only to carry operators.

Anything under `mp_units::detail` is private and is not documented here. Do not
depend on it.

## Other references

- **[Systems Reference](../systems_reference/index.md)** — units, quantities,
  dimensions and constants
- **[Cheat Sheet](../cheat_sheet.md)** — common operations at a glance
- **[Glossary](../glossary.md)** — metrology terms used throughout
