# Supported Systems Overview

This reference page provides a complete overview of all systems of quantities and units
available in **mp-units**. For detailed explanations of what systems are and how they work,
see [System Definitions](../users_guide/systems/introduction.md) in the User's Guide.

## Documented Systems

### Systems of Quantities

- **[International System of Quantities (ISQ)](../users_guide/systems/isq.md)** - The
  comprehensive system of quantities defined by ISO/IEC 80000, serving as the foundation
  for the SI and other systems
- **[Natural Units](../users_guide/systems/natural_units.md)** - A system of quantities and
  units commonly used in high-energy physics where fundamental physical constants are set to unity

### Systems of Units

- **[International System of Units (SI)](../users_guide/systems/si.md)** - The modern form
  of the metric system and the most widely used system of measurement
- **[Centimetre-Gram-Second (CGS) System](../users_guide/systems/cgs.md)** - The
  centimeter-gram-second system, still used in some areas of physics and chemistry
- **[Strong Angular System](../users_guide/systems/strong_angular_system.md)** - An
  experimental extension treating angle as a dimensional quantity, addressing long-standing
  issues in angular measurements
- **[Natural Units](../users_guide/systems/natural_units.md)** - A system of quantities and
  units commonly used in high-energy physics where fundamental physical constants are set to unity

## Additional Supported Systems

Beyond the systems documented in detail, **mp-units** also provides support for the following
systems. These are available in the library through headers in `<mp-units/systems/>` but are
not covered in the detailed User's Guide documentation:

<!-- markdownlint-disable-next-line MD024 -->
### Systems of Units

- **IEC 80000** - Units for information science and technology (bits, bytes, etc.)
- **IAU** - International Astronomical Union system for astronomical applications
- **International/Imperial** - Traditional units used in the United Kingdom and other
  Commonwealth countries
- **USC** - United States customary units
- **Typographic** - Units used in typography and desktop publishing (points, picas, etc.)
- **HEP** - High-energy physics units (barn, electronvolt, particle masses)

## Quick Reference Guide

### When to Use Which System

| System                | Best For                                     | Key Features                                                                 |
|-----------------------|----------------------------------------------|------------------------------------------------------------------------------|
| **SI**                | Most engineering and scientific applications | International standard, coherent, widely adopted                             |
| **ISQ + SI**          | Applications requiring strong type safety    | Hierarchical quantity types (e.g., _length_ vs _width_ vs _height_)          |
| **Strong Angular**    | Rotational mechanics, angular analysis       | Explicit angle dimensions, prevents mixing radians with dimensionless values |
| **Natural Units**     | Particle physics, cosmology                  | Fundamental constants ($c$, $\hbar$, $k_B$) set to unity                     |
| **CGS**               | Legacy physics code, specific domains        | Smaller base units, still standard in some scientific fields                 |
| **International/USC** | Industry compliance (aviation, construction) | Required by regulations in certain countries                                 |
| **HEP**               | High-energy physics experiments              | Natural units for particle physics (eV, barn)                                |
| **Typographic**       | Desktop publishing, font design              | Industry-standard units (pt, pica)                                           |

## System Headers

Each system can be included via its corresponding header:

```cpp
#include <mp-units/systems/isq.h>      // International System of Quantities
#include <mp-units/systems/si.h>       // International System of Units
#include <mp-units/systems/cgs.h>      // Centimetre-Gram-Second
#include <mp-units/systems/angular.h>  // Strong Angular System
#include <mp-units/systems/natural.h>  // Natural Units
#include <mp-units/systems/iec80000.h> // IEC information units
#include <mp-units/systems/iau.h>      // Astronomical units
#include <mp-units/systems/imperial.h> // Imperial/International units
#include <mp-units/systems/usc.h>      // US Customary units
#include <mp-units/systems/typographic.h> // Typographic units
#include <mp-units/systems/hep.h>      // High-energy physics units
```

For detailed information about each documented system, its quantities, units, and usage
patterns, refer to the individual system pages linked above.
