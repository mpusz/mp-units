<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# complex_power Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_complex_power["<b><a href="../../systems/isq/#complex_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::complex_power</a></b><br><i>(<a href="../../systems/isq/#voltage_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">voltage_phasor</a> * <a href="../../systems/isq/#electric_current_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current_phasor</a>)</i>"]
    isq_apparent_power["<b><a href="../../systems/isq/#apparent_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::apparent_power</a></b>"]
    isq_complex_power --- isq_apparent_power
    isq_non_active_power["<b><a href="../../systems/isq/#non_active_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::non_active_power</a></b><br><i>(pow<1, 2>(pow<2>(<a href="../../systems/isq/#apparent_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">apparent_power</a>)))</i>"]
    isq_apparent_power --- isq_non_active_power
```
