<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# electric_potential Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_electric_potential["<b>isq::electric_potential</b><br><i>(electric_field_strength * length)</i>"]
    isq_electric_potential_difference["<b>isq::electric_potential_difference</b>"]
    isq_electric_potential --- isq_electric_potential_difference
    isq_voltage["<b>isq::voltage | isq::electric_tension</b>"]
    isq_electric_potential --- isq_voltage
    isq_induced_voltage["<b>isq::induced_voltage</b>"]
    isq_voltage --- isq_induced_voltage
    isq_source_voltage["<b>isq::source_voltage | isq::source_tension</b>"]
    isq_voltage --- isq_source_voltage
    isq_voltage_phasor["<b>isq::voltage_phasor | isq::electric_tension_phasor</b>"]
    isq_voltage --- isq_voltage_phasor
```
