# Supported Systems Overview

This reference page provides a complete catalog of all systems of quantities and units
available in **mp-units**. For detailed explanations of system architecture and how they work,
see [System Definitions](../users_guide/systems/introduction.md) in the User's Guide.

## All Available Systems

The following table lists all systems available in the library. The "Documented" column
indicates systems with detailed User's Guide documentation.

| System                                                             | Quantities | Units | Description & Use Cases                                                                                                                                                                                                                                                                             | Documented |
|--------------------------------------------------------------------|:----------:|:-----:|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----------:|
| **[ISQ](../users_guide/systems/isq.md)**                           |     ✓      |       | International System of Quantities (from ISO/IEC 80000 series) - foundation for SI and other systems. Use when you need the full typed quantity hierarchy.                                                                                                                                          |     ✓      |
| **[SI](../users_guide/systems/si.md)**                             |            |   ✓   | International System of Units - modern metric system, most widely used. Best for most engineering and scientific applications as the international standard.                                                                                                                                        |     ✓      |
| **[CGS](../users_guide/systems/cgs.md)**                           |            |   ✓   | Centimetre-Gram-Second system - still used in some areas of physics and chemistry. Use when interfacing with legacy physics code or specific scientific domains.                                                                                                                                    |     ✓      |
| **[Yard-Pound](../users_guide/systems/yard_pound.md)**             |            |   ✓   | Base units system from the 1959 international yard and pound agreement. Foundation for Imperial and USC systems. Use directly when you need the common base units without regional extensions.                                                                                                      |     ✓      |
| **[Imperial](../users_guide/systems/yard_pound.md)**               |            |   ✓   | British Imperial system extending yard-pound with UK-specific units (stone, imperial gallon, long ton). Use when required by UK industry standards or regulations.                                                                                                                                  |     ✓      |
| **[USC](../users_guide/systems/yard_pound.md)**                    |            |   ✓   | United States customary units extending yard-pound with US-specific units (US gallon, survey units, Fahrenheit). Use for aviation, construction, and other industries in the US.                                                                                                                    |     ✓      |
| **[`angular`](../users_guide/systems/strong_angular_system.md)**   |     ✓      |   ✓   | Standalone strong angular system with dimensional angle and units. Use for rotational mechanics when you want explicit angle dimensions without the full ISQ hierarchy.                                                                                                                             |     ✓      |
| **[`isq_angle`](../users_guide/systems/strong_angular_system.md)** |     ✓      |       | ISQ amendment incorporating strong angular quantities. Use when you need both the ISQ quantity hierarchy and strong angular dimensions (e.g., for physically-correct torque units).                                                                                                                 |     ✓      |
| **[Natural](../users_guide/systems/natural_units.md)**             |     ✓      |   ✓   | High-energy physics system where fundamental constants ($c$, $\hbar$, $k_B$) equal unity. Use for particle physics and cosmology calculations.                                                                                                                                                      |     ✓      |
| **[IAU](../users_guide/systems/iau.md)**                           |            |   ✓   | International Astronomical Union standardized constants from official resolutions: nominal solar/planetary values (Resolution B3), astronomical unit (Resolution B2), CODATA 2018 gravitational constant and masses. Use for professional astronomy/astrophysics requiring IAU-standardized values. |     ✓      |
| **Astronomy**                                                      |            |   ✓   | Common astronomy utility units not standardized by IAU: light-year, lunar distance, jansky, sidereal day, Julian/tropical year, century, millennium, gaussian gravitational constant. Use for general astronomy calculations.                                                                       |            |
| **IEC**                                                            |            |   ✓   | IEC-standardized units including electrical (var), telecommunications (erlang, baud), and information technology (bit, byte). Use for electrical engineering and IT applications.                                                                                                                   |            |
| **Typographic**                                                    |            |   ✓   | Typography and desktop publishing units: US/DTP points and picas, Didot point and cicero (European), Q and H (Japanese), TeX point. Use for font design, typesetting, and publishing applications.                                                                                                  |            |
| **HEP**                                                            |            |   ✓   | High-energy physics units: electronvolt (_energy_), barn (_cross-section_), fermi (femtometre), _particle masses_ (electron, proton, neutron), _atomic mass unit_, fundamental constants (c, h, ℏ, e). Use for particle physics experiments and calculations.                                       |            |

!!! note

    The checkmarks show what each system *defines*. Systems of units are always built upon
    systems of quantities, so including a system of units (like SI) also provides access to
    the quantities it depends on (though SI only imports the ISQ quantities needed for its
    definitions, not the entire ISQ system).

## System Headers

Each system can be included via its corresponding header:

```cpp
#include <mp-units/systems/angular.h>      // Strong Angular System (standalone)
#include <mp-units/systems/astronomy.h>    // Common astronomy utility units
#include <mp-units/systems/cgs.h>          // Centimetre-Gram-Second
#include <mp-units/systems/hep.h>          // High-energy physics units
#include <mp-units/systems/iau.h>          // IAU standardized astronomical constants
#include <mp-units/systems/iec80000.h>     // IEC information units
#include <mp-units/systems/imperial.h>     // British Imperial units
#include <mp-units/systems/isq.h>          // International System of Quantities
#include <mp-units/systems/isq_angle.h>    // Strong Angular System (ISQ amendment)
#include <mp-units/systems/natural.h>      // Natural Units
#include <mp-units/systems/si.h>           // International System of Units
#include <mp-units/systems/typographic.h>  // Typographic units
#include <mp-units/systems/usc.h>          // US Customary units
#include <mp-units/systems/yard_pound.h>   // Yard-Pound base units (1959 international agreement)
```

For detailed information about each documented system, its quantities, units, and usage
patterns, refer to the individual system pages linked in the table above.
