<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# propagation_coefficient Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_propagation_coefficient["<b><a href="../../systems/isq/#propagation_coefficient" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::propagation_coefficient</a></b><br><i>(inverse(<a href="../../systems/isq/#length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">length</a>))</i>"]
    isq_attenuation["<b><a href="../../systems/isq/#attenuation" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::attenuation</a> | <a href="../../systems/isq/#extinction" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::extinction</a></b><br><i>(inverse(<a href="../../systems/isq/#distance" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">distance</a>))</i>"]
    isq_propagation_coefficient --- isq_attenuation
    isq_phase_coefficient["<b><a href="../../systems/isq/#phase_coefficient" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::phase_coefficient</a></b><br><i>(<a href="../../systems/isq/#phase_angle" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">phase_angle</a> / <a href="../../systems/isq/#path_length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">path_length</a>)</i>"]
    isq_propagation_coefficient --- isq_phase_coefficient
```
