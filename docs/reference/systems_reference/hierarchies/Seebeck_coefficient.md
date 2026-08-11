<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# Seebeck_coefficient Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_Seebeck_coefficient["<b><a href="../../systems/isq/#Seebeck_coefficient" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Seebeck_coefficient</a> | <a href="../../systems/isq/#thermoelectric_power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::thermoelectric_power</a></b><br><i>(<a href="../../systems/isq/#thermoelectric_voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">thermoelectric_voltage</a> / <a href="../../systems/isq/#thermodynamic_temperature" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">thermodynamic_temperature</a>)</i>"]
    isq_Thomson_coefficient["<b><a href="../../systems/isq/#Thomson_coefficient" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Thomson_coefficient</a></b><br><i>(<a href="../../systems/isq/#heat_flow_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">heat_flow_rate</a> / (<a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a> * <a href="../../systems/isq/#thermodynamic_temperature" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">thermodynamic_temperature</a>))</i>"]
    isq_Seebeck_coefficient --- isq_Thomson_coefficient
```
