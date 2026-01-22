<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# impedance Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_impedance["<b><a href="../../systems/isq/#impedance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::impedance</a> | <a href="../../systems/isq/#complex_impedance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::complex_impedance</a></b><br><i>(<a href="../../systems/isq/#voltage_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">voltage_phasor</a> / <a href="../../systems/isq/#electric_current_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current_phasor</a>)</i>"]
    isq_apparent_impedance["<b><a href="../../systems/isq/#apparent_impedance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::apparent_impedance</a></b>"]
    isq_impedance --- isq_apparent_impedance
    isq_impedance_of_vacuum["<b><a href="../../systems/isq/#impedance_of_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::impedance_of_vacuum</a> | <a href="../../systems/isq/#wave_impedance_in_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::wave_impedance_in_vacuum</a></b>"]
    isq_impedance --- isq_impedance_of_vacuum
    isq_reactance["<b><a href="../../systems/isq/#reactance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::reactance</a></b>"]
    isq_impedance --- isq_reactance
    isq_resistance["<b><a href="../../systems/isq/#resistance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::resistance</a></b><br><i>(<a href="../../systems/isq/#voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">voltage</a> / <a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a>)</i>"]
    isq_impedance --- isq_resistance
    isq_resistance_to_alternating_current["<b><a href="../../systems/isq/#resistance_to_alternating_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::resistance_to_alternating_current</a></b>"]
    isq_impedance --- isq_resistance_to_alternating_current
```
