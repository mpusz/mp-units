<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# electric_potential Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_electric_potential["<b><a href="../../systems/isq/#electric_potential" style="color: black; text-decoration: none;">isq::electric_potential</a></b><br><i>(<a href="../../systems/isq/#electric_field_strength" style="color: black; text-decoration: none;">electric_field_strength</a> * <a href="../../systems/isq/#length" style="color: black; text-decoration: none;">length</a>)</i>"]
    isq_electric_potential_difference["<b><a href="../../systems/isq/#electric_potential_difference" style="color: black; text-decoration: none;">isq::electric_potential_difference</a></b>"]
    isq_electric_potential --- isq_electric_potential_difference
    isq_voltage["<b><a href="../../systems/isq/#voltage" style="color: black; text-decoration: none;">isq::voltage</a> | <a href="../../systems/isq/#electric_tension" style="color: black; text-decoration: none;">isq::electric_tension</a></b>"]
    isq_electric_potential --- isq_voltage
    isq_induced_voltage["<b><a href="../../systems/isq/#induced_voltage" style="color: black; text-decoration: none;">isq::induced_voltage</a></b>"]
    isq_voltage --- isq_induced_voltage
    isq_source_voltage["<b><a href="../../systems/isq/#source_voltage" style="color: black; text-decoration: none;">isq::source_voltage</a> | <a href="../../systems/isq/#source_tension" style="color: black; text-decoration: none;">isq::source_tension</a></b>"]
    isq_voltage --- isq_source_voltage
    isq_voltage_phasor["<b><a href="../../systems/isq/#voltage_phasor" style="color: black; text-decoration: none;">isq::voltage_phasor</a> | <a href="../../systems/isq/#electric_tension_phasor" style="color: black; text-decoration: none;">isq::electric_tension_phasor</a></b>"]
    isq_voltage --- isq_voltage_phasor
```
