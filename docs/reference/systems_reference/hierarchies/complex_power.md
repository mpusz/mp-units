<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# complex_power Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_complex_power["<b>isq::complex_power</b><br><i>(voltage_phasor * electric_current_phasor)</i>"]
    isq_apparent_power["<b>isq::apparent_power</b>"]
    isq_complex_power --- isq_apparent_power
    isq_non_active_power["<b>isq::non_active_power</b><br><i>(pow<1, 2>(pow<2>(apparent_power)))</i>"]
    isq_apparent_power --- isq_non_active_power
```
