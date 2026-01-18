<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# admittance Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_admittance["<b><a href="../../systems/isq/#admittance" style="color: black; text-decoration: none;">isq::admittance</a> | <a href="../../systems/isq/#complex_admittance" style="color: black; text-decoration: none;">isq::complex_admittance</a></b><br><i>(inverse(<a href="../../systems/isq/#impedance" style="color: black; text-decoration: none;">impedance</a>))</i>"]
    isq_admittance_of_vacuum["<b><a href="../../systems/isq/#admittance_of_vacuum" style="color: black; text-decoration: none;">isq::admittance_of_vacuum</a></b><br><i>(inverse(<a href="../../systems/isq/#impedance_of_vacuum" style="color: black; text-decoration: none;">impedance_of_vacuum</a>))</i>"]
    isq_admittance --- isq_admittance_of_vacuum
    isq_apparent_admittance["<b><a href="../../systems/isq/#apparent_admittance" style="color: black; text-decoration: none;">isq::apparent_admittance</a></b>"]
    isq_admittance --- isq_apparent_admittance
    isq_susceptance["<b><a href="../../systems/isq/#susceptance" style="color: black; text-decoration: none;">isq::susceptance</a></b>"]
    isq_admittance --- isq_susceptance
```
