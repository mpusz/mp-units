# System Definitions

This section provides detailed documentation for the most commonly used systems of quantities
and units available in **mp-units**. While the library supports many systems through its
flexible framework, we focus here on systems that are widely adopted in scientific and
engineering applications.

!!! tip "Quick Reference"
    For a complete list of all available systems in **mp-units**, see the
    [Supported Systems Overview](../../reference/systems_overview.md) in the Reference section.

## About Systems in mp-units

As discussed in the Framework Basics, understanding both
[Systems of Quantities](../framework_basics/systems_of_quantities.md) and
[Systems of Units](../framework_basics/systems_of_units.md) is crucial for effective use
of the library. Systems provide:

- **Type Safety**: Prevent mixing incompatible quantities and units
- **Dimensional Analysis**: Automatic verification of equation correctness
- **Domain Modeling**: Express physical relationships in type-checked code
- **Standardization**: Use well-established scientific and engineering conventions

## Documented Systems

This User's Guide provides in-depth documentation for the following systems:

### Systems of Quantities

- **[International System of Quantities (ISQ)](isq.md)** - The comprehensive system of
  quantities defined by ISO/IEC 80000, serving as the foundation for the SI and other systems
- **[Natural Units](natural_units.md)** - A system of quantities and units commonly used in
  high-energy physics where fundamental physical constants are set to unity

### Systems of Units

- **[International System of Units (SI)](si.md)** - The modern form of the metric system
  and the most widely used system of measurement
- **[Centimetre-Gram-Second (CGS) System](cgs.md)** - The centimeter-gram-second system,
  still used in some areas of physics and chemistry
- **[Strong Angular System](strong_angular_system.md)** - An experimental extension treating
  angle as a dimensional quantity, addressing long-standing issues in angular measurements
- **[Natural Units](natural_units.md)** - A system commonly used in high-energy physics
  where fundamental physical constants are set to unity

## Choosing the Right System

The choice of system depends on your application domain and requirements:

- Use **SI** for most engineering and scientific applications - it's the international standard
- Use **ISQ + SI** when you need strong quantity type safety and hierarchy (e.g., distinguishing
  _length_, _width_, _height_, _radius_)
- Use **Strong Angular** when working with rotational mechanics and you want explicit angle
  dimensions
- Use **Natural Units** for particle physics and cosmology calculations
- Use **CGS** when interfacing with legacy physics code or specific scientific domains where
  it's standard
- Use **International** or **USC** when required by industry standards or regulations
  (e.g., aviation, construction in certain countries)
