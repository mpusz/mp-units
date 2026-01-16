<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# pressure Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_pressure["<b>isq::pressure</b><br><i>(force / area)</i>"]
    isq_gauge_pressure["<b>isq::gauge_pressure</b>"]
    isq_pressure --- isq_gauge_pressure
    isq_normal_stress["<b>isq::normal_stress</b>"]
    isq_pressure --- isq_normal_stress
    isq_shear_stress["<b>isq::shear_stress</b>"]
    isq_pressure --- isq_shear_stress
    isq_stress["<b>isq::stress</b>"]
    isq_pressure --- isq_stress
```
