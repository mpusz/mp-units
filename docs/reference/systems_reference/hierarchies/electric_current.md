<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# electric_current Hierarchy

**System:** ISQ

**Dimension:** dim_electric_current

```mermaid
flowchart LR
    isq_electric_current["<b>isq::electric_current</b>"]
    isq_displacement_current["<b>isq::displacement_current</b><br><i>(displacement_current_density * area)</i>"]
    isq_electric_current --- isq_displacement_current
    isq_electric_current_phasor["<b>isq::electric_current_phasor</b>"]
    isq_electric_current --- isq_electric_current_phasor
    isq_magnetic_potential["<b>isq::magnetic_potential</b>"]
    isq_electric_current --- isq_magnetic_potential
    isq_magnetic_tension["<b>isq::magnetic_tension</b><br><i>(magnetic_field_strength * position_vector)</i>"]
    isq_electric_current --- isq_magnetic_tension
    isq_magnetomotive_force["<b>isq::magnetomotive_force</b><br><i>(magnetic_field_strength * position_vector)</i>"]
    isq_electric_current --- isq_magnetomotive_force
    isq_total_current["<b>isq::total_current</b>"]
    isq_electric_current --- isq_total_current
```
