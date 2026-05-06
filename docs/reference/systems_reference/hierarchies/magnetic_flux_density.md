<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# magnetic_flux_density Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_magnetic_flux_density["<b><a href="../../systems/isq/#magnetic_flux_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::magnetic_flux_density</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a> / (<a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a> * pow<2>(<a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>)))</i>"]
    isq_magnetic_polarization["<b><a href="../../systems/isq/#magnetic_polarization" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::magnetic_polarization</a></b><br><i>(<a href="../../systems/isq/#magnetic_constant" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">magnetic_constant</a> * <a href="../../systems/isq/#magnetization" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">magnetization</a>)</i>"]
    isq_magnetic_flux_density --- isq_magnetic_polarization
```
