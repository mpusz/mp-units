<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# impedance Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_impedance["<b>isq::impedance | isq::complex_impedance</b><br><i>(voltage_phasor / electric_current_phasor)</i>"]
    isq_apparent_impedance["<b>isq::apparent_impedance</b>"]
    isq_impedance --- isq_apparent_impedance
    isq_impedance_of_vacuum["<b>isq::impedance_of_vacuum | isq::wave_impedance_in_vacuum</b>"]
    isq_impedance --- isq_impedance_of_vacuum
    isq_reactance["<b>isq::reactance</b>"]
    isq_impedance --- isq_reactance
    isq_resistance_to_alternating_current["<b>isq::resistance_to_alternating_current</b>"]
    isq_impedance --- isq_resistance_to_alternating_current
```
