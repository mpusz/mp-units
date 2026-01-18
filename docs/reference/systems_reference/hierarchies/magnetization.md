<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# magnetization Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_magnetization["<b><a href="../../systems/isq/#magnetization" style="color: black; text-decoration: none;">isq::magnetization</a></b><br><i>(<a href="../../systems/isq/#magnetic_moment" style="color: black; text-decoration: none;">magnetic_moment</a> / <a href="../../systems/isq/#volume" style="color: black; text-decoration: none;">volume</a>)</i>"]
    isq_magnetic_field_strength["<b><a href="../../systems/isq/#magnetic_field_strength" style="color: black; text-decoration: none;">isq::magnetic_field_strength</a> | <a href="../../systems/isq/#magnetizing_field" style="color: black; text-decoration: none;">isq::magnetizing_field</a></b>"]
    isq_magnetization --- isq_magnetic_field_strength
    isq_coercivity["<b><a href="../../systems/isq/#coercivity" style="color: black; text-decoration: none;">isq::coercivity</a> | <a href="../../systems/isq/#coercive_field_strength" style="color: black; text-decoration: none;">isq::coercive_field_strength</a></b>"]
    isq_magnetic_field_strength --- isq_coercivity
```
