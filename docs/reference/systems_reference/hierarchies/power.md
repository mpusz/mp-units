<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# power Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_power["<b>isq::power</b><br><i>(mass * pow<2>(length) / pow<3>(time))</i>"]
    isq_active_power["<b>isq::active_power</b><br><i>(inverse(period) * (instantaneous_power * time))</i>"]
    isq_power --- isq_active_power
    isq_carrier_power["<b>isq::carrier_power</b>"]
    isq_power --- isq_carrier_power
    isq_electromagnetism_power["<b>isq::electromagnetism_power | isq::instantaneous_power</b><br><i>(voltage * electric_current)</i>"]
    isq_power --- isq_electromagnetism_power
    isq_mechanical_power["<b>isq::mechanical_power</b><br><i>(force * velocity)</i>"]
    isq_power --- isq_mechanical_power
    isq_quantizing_distortion_power["<b>isq::quantizing_distortion_power</b>"]
    isq_power --- isq_quantizing_distortion_power
    isq_radiant_flux["<b>isq::radiant_flux | isq::radiant_power</b><br><i>(radiant_energy / time)</i>"]
    isq_power --- isq_radiant_flux
```
