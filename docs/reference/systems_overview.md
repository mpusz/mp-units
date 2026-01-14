# Supported Systems Overview

This reference page provides a complete catalog of all systems of quantities and units
available in **mp-units**. For detailed explanations of system architecture and how they work,
see [System Definitions](../users_guide/systems/introduction.md) in the User's Guide.

## All Available Systems

The following table lists all systems available in the library. The "Documented" column
indicates systems with detailed User's Guide documentation.

| System                                                             | Quantities | Units | Description & Use Cases                                                                                                                                                                        | Documented |
|--------------------------------------------------------------------|:----------:|:-----:|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----------:|
| **[ISQ](../users_guide/systems/isq.md)**                           |     ✓      |       | International System of Quantities (ISO/IEC 80000) - foundation for SI and other systems. Use when you need the full typed quantity hierarchy.                                                 |     ✓      |
| **[SI](../users_guide/systems/si.md)**                             |            |   ✓   | International System of Units - modern metric system, most widely used. Best for most engineering and scientific applications as the international standard.                                   |     ✓      |
| **[CGS](../users_guide/systems/cgs.md)**                           |            |   ✓   | Centimetre-Gram-Second system - still used in some areas of physics and chemistry. Use when interfacing with legacy physics code or specific scientific domains.                               |     ✓      |
| **[`angular`](../users_guide/systems/strong_angular_system.md)**   |     ✓      |   ✓   | Standalone strong angular system with dimensional angle and units. Use for rotational mechanics when you want explicit angle dimensions without the full ISQ hierarchy.                        |     ✓      |
| **[`isq_angle`](../users_guide/systems/strong_angular_system.md)** |     ✓      |       | ISQ amendment incorporating strong angular quantities. Use when you need both the ISQ quantity hierarchy and strong angular dimensions (e.g., for physically-correct torque units).            |     ✓      |
| **[Natural](../users_guide/systems/natural_units.md)**             |     ✓      |   ✓   | High-energy physics system where fundamental constants ($c$, $\hbar$, $k_B$) equal unity. Use for particle physics and cosmology calculations.                                                 |     ✓      |
| **IEC 80000**                                                      |            |   ✓   | Units for information science and technology (bits, bytes, etc.). Use for data storage and information theory applications.                                                                    |            |
| **IAU**                                                            |            |   ✓   | International Astronomical Union system for astronomical applications. Use for astronomy and astrophysics calculations.                                                                        |            |
| **Yard-Pound**                                                     |            |   ✓   | Base units system from the 1959 international yard and pound agreement. Foundation for Imperial and USC systems. Use directly when you need the common base units without regional extensions. |            |
| **Imperial**                                                       |            |   ✓   | British Imperial system extending yard-pound with UK-specific units (stone, imperial gallon, long ton). Use when required by UK industry standards or regulations.                             |            |
| **USC**                                                            |            |   ✓   | United States customary units extending yard-pound with US-specific units (US gallon, survey units, Fahrenheit). Use for aviation, construction, and other industries in the US.               |            |
| **Typographic**                                                    |            |   ✓   | Units used in typography and desktop publishing (points, picas, etc.). Use for font design and publishing applications.                                                                        |            |
| **HEP**                                                            |            |   ✓   | High-energy physics units (barn, electronvolt, particle masses). Use for high-energy physics experiments.                                                                                      |            |

!!! note

    The checkmarks show what each system *defines*. Systems of units are always built upon
    systems of quantities, so including a system of units (like SI) also provides access to
    the quantities it depends on (though SI only imports the ISQ quantities needed for its
    definitions, not the entire ISQ system).

## System Headers

Each system can be included via its corresponding header:

```cpp
#include <mp-units/systems/isq.h>       // International System of Quantities
#include <mp-units/systems/si.h>        // International System of Units
#include <mp-units/systems/cgs.h>       // Centimetre-Gram-Second
#include <mp-units/systems/angular.h>   // Strong Angular System (standalone)
#include <mp-units/systems/isq_angle.h> // Strong Angular System (ISQ amendment)
#include <mp-units/systems/natural.h>   // Natural Units
#include <mp-units/systems/iec80000.h>  // IEC information units
#include <mp-units/systems/iau.h>       // Astronomical units
#include <mp-units/systems/yard_pound.h> // Yard-Pound base units (1959 international agreement)
#include <mp-units/systems/imperial.h>  // British Imperial units
#include <mp-units/systems/usc.h>       // US Customary units
#include <mp-units/systems/typographic.h> // Typographic units
#include <mp-units/systems/hep.h>       // High-energy physics units
```

For detailed information about each documented system, its quantities, units, and usage
patterns, refer to the individual system pages linked in the table above.
