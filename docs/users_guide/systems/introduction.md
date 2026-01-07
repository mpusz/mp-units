# System Definitions

This section provides detailed documentation for the most commonly used systems of quantities
and units available in **mp-units**. While the library supports many systems through its
flexible framework, we focus here on systems that are widely adopted in scientific and
engineering applications.

## Available Systems

### Systems of Quantities

As discussed in [Systems of Quantities](../framework_basics/systems_of_quantities.md),
modeling systems of quantities is crucial for type-safe dimensional analysis. The library
provides:

- **[International System of Quantities (ISQ)](isq.md)** - The comprehensive system of
  quantities defined by ISO/IEC 80000, serving as the foundation for the SI and other systems

### Systems of Units

Building on the foundation established in [Systems of Units](../framework_basics/systems_of_units.md),
the library implements several well-established measurement systems:

- **[International System of Units (SI)](si.md)** - The modern form of the metric system
  and the most widely used system of measurement
- **[CGS System](cgs.md)** - The centimeter-gram-second system, still used in some areas
- **[Strong Angular System](strong_angular_system.md)** - An experimental extension treating
  angle as a dimensional quantity, addressing long-standing issues in angular measurements
- **[Natural Units](natural_units.md)** - A system commonly used in high-energy physics
  where fundamental physical constants are set to unity of physics and chemistry

### Additional Supported Systems

Beyond the systems documented in detail here, **mp-units** also provides support for:

- **IEC 80000** - Units for information science and technology
- **IAU** - International Astronomical Union system for astronomical applications
- **International/Imperial** - Traditional units used in the United States and other countries
- **USC** - United States customary units
- **Typographic** - Units used in typography and desktop publishing
- **HEP** - High-energy physics units (barn, electronvolt, particle masses)

These systems are available in the library but are not documented in detail in this guide.
Refer to the header files in `<mp-units/systems/>` for their definitions.


## Choosing a System

- Use **SI** for most engineering and scientific applications - it's the international standard
- Use **ISQ + SI** when you need strong quantity type safety and hierarchy (e.g., distinguishing
  _length_, _width_, hei_ght, _radius_)
- Use **Strong Angular** when working with rotational mechanics and you want explicit angle
  dimensions
- Use **Natural Units** for particle physics and cosmology calculations
- Use **CGS** when interfacing with legacy physics code or specific scientific domains where
  it's standard
- Use **International** or **USC** when required by industry standards or regulations
  (e.g., aviation, construction in certain countries)
