<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# electric_potential Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_electric_potential["<b><a href="../../systems/isq/#electric_potential" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electric_potential</a></b><br><i>(<a href="../../systems/isq/#energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">energy</a> / (<a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a> * <a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>))</i>"]
    isq_electric_potential_difference["<b><a href="../../systems/isq/#electric_potential_difference" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electric_potential_difference</a></b>"]
    isq_electric_potential --- isq_electric_potential_difference
    isq_voltage["<b><a href="../../systems/isq/#voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::voltage</a> | <a href="../../systems/isq/#electric_tension" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electric_tension</a></b>"]
    isq_electric_potential --- isq_voltage
    isq_Peltier_coefficient["<b><a href="../../systems/isq/#Peltier_coefficient" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Peltier_coefficient</a></b><br><i>(<a href="../../systems/isq/#heat_flow_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">heat_flow_rate</a> / <a href="../../systems/isq/#electric_current" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electric_current</a>)</i>"]
    isq_voltage --- isq_Peltier_coefficient
    isq_induced_voltage["<b><a href="../../systems/isq/#induced_voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::induced_voltage</a></b>"]
    isq_voltage --- isq_induced_voltage
    isq_source_voltage["<b><a href="../../systems/isq/#source_voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::source_voltage</a> | <a href="../../systems/isq/#source_tension" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::source_tension</a></b>"]
    isq_voltage --- isq_source_voltage
    isq_thermoelectric_voltage["<b><a href="../../systems/isq/#thermoelectric_voltage" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::thermoelectric_voltage</a></b>"]
    isq_voltage --- isq_thermoelectric_voltage
    isq_voltage_phasor["<b><a href="../../systems/isq/#voltage_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::voltage_phasor</a> | <a href="../../systems/isq/#electric_tension_phasor" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electric_tension_phasor</a></b>"]
    isq_voltage --- isq_voltage_phasor
```
