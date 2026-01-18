<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# electric_current Hierarchy

**System:** ISQ

**Dimension:** dim_electric_current

```mermaid
flowchart LR
    isq_electric_current["<b><a href="../../systems/isq/#electric_current" style="color: black; text-decoration: none;">isq::electric_current</a></b>"]
    isq_displacement_current["<b><a href="../../systems/isq/#displacement_current" style="color: black; text-decoration: none;">isq::displacement_current</a></b><br><i>(<a href="../../systems/isq/#displacement_current_density" style="color: black; text-decoration: none;">displacement_current_density</a> * <a href="../../systems/isq/#area" style="color: black; text-decoration: none;">area</a>)</i>"]
    isq_electric_current --- isq_displacement_current
    isq_electric_current_phasor["<b><a href="../../systems/isq/#electric_current_phasor" style="color: black; text-decoration: none;">isq::electric_current_phasor</a></b>"]
    isq_electric_current --- isq_electric_current_phasor
    isq_magnetic_potential["<b><a href="../../systems/isq/#magnetic_potential" style="color: black; text-decoration: none;">isq::magnetic_potential</a></b>"]
    isq_electric_current --- isq_magnetic_potential
    isq_magnetic_tension["<b><a href="../../systems/isq/#magnetic_tension" style="color: black; text-decoration: none;">isq::magnetic_tension</a></b><br><i>(<a href="../../systems/isq/#magnetic_field_strength" style="color: black; text-decoration: none;">magnetic_field_strength</a> * <a href="../../systems/isq/#position_vector" style="color: black; text-decoration: none;">position_vector</a>)</i>"]
    isq_electric_current --- isq_magnetic_tension
    isq_magnetomotive_force["<b><a href="../../systems/isq/#magnetomotive_force" style="color: black; text-decoration: none;">isq::magnetomotive_force</a></b><br><i>(<a href="../../systems/isq/#magnetic_field_strength" style="color: black; text-decoration: none;">magnetic_field_strength</a> * <a href="../../systems/isq/#position_vector" style="color: black; text-decoration: none;">position_vector</a>)</i>"]
    isq_electric_current --- isq_magnetomotive_force
    isq_total_current["<b><a href="../../systems/isq/#total_current" style="color: black; text-decoration: none;">isq::total_current</a></b>"]
    isq_electric_current --- isq_total_current
```
