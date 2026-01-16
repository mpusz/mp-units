<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# magnetization Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_magnetization["<b>isq::magnetization</b><br><i>(magnetic_moment / volume)</i>"]
    isq_magnetic_field_strength["<b>isq::magnetic_field_strength | isq::magnetizing_field</b>"]
    isq_magnetization --- isq_magnetic_field_strength
    isq_coercivity["<b>isq::coercivity | isq::coercive_field_strength</b>"]
    isq_magnetic_field_strength --- isq_coercivity
```
