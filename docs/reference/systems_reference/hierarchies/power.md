<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# power Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_power["<b><a href="../../systems/isq/#power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::power</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a> * pow<2>(<a href="../../systems/isq/#length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">length</a>) / pow<3>(<a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>))</i>"]
    isq_active_power["<b><a href="../../systems/isq/#active_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::active_power</a></b><br><i>(inverse(<a href="../../systems/isq/#period" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">period</a>) * (<a href="../../systems/isq/#instantaneous_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">instantaneous_power</a> * <a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>))</i>"]
    isq_power --- isq_active_power
    isq_carrier_power["<b><a href="../../systems/isq/#carrier_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::carrier_power</a></b>"]
    isq_power --- isq_carrier_power
    isq_electromagnetism_power["<b><a href="../../systems/isq/#electromagnetism_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electromagnetism_power</a> | <a href="../../systems/isq/#instantaneous_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::instantaneous_power</a></b><br><i>(<a href="../../systems/isq/#voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">voltage</a> * <a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a>)</i>"]
    isq_power --- isq_electromagnetism_power
    isq_mechanical_power["<b><a href="../../systems/isq/#mechanical_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::mechanical_power</a></b><br><i>(<a href="../../systems/isq/#force" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">force</a> * <a href="../../systems/isq/#velocity" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">velocity</a>)</i>"]
    isq_power --- isq_mechanical_power
    isq_quantizing_distortion_power["<b><a href="../../systems/isq/#quantizing_distortion_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::quantizing_distortion_power</a></b>"]
    isq_power --- isq_quantizing_distortion_power
    isq_radiant_flux["<b><a href="../../systems/isq/#radiant_flux" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::radiant_flux</a> | <a href="../../systems/isq/#radiant_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::radiant_power</a></b><br><i>(<a href="../../systems/isq/#radiant_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">radiant_energy</a> / <a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>)</i>"]
    isq_power --- isq_radiant_flux
```
