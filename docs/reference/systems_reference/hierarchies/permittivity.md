<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# permittivity Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_permittivity["<b><a href="../../systems/isq/#permittivity" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::permittivity</a></b><br><i>(<a href="../../systems/isq/#electric_flux_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_flux_density</a> / <a href="../../systems/isq/#electric_field_strength" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_field_strength</a>)</i>"]
    isq_electric_constant["<b><a href="../../systems/isq/#electric_constant" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electric_constant</a> | <a href="../../systems/isq/#permittivity_of_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::permittivity_of_vacuum</a></b><br><i>(inverse(<a href="../../systems/isq/#magnetic_constant" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">magnetic_constant</a> * pow<2>(<a href="../../systems/isq/#speed_of_light_in_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">speed_of_light_in_vacuum</a>)))</i>"]
    isq_permittivity --- isq_electric_constant
```
