<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# admittance Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_admittance["<b>isq::admittance | isq::complex_admittance</b><br><i>(inverse(impedance))</i>"]
    isq_admittance_of_vacuum["<b>isq::admittance_of_vacuum</b><br><i>(inverse(impedance_of_vacuum))</i>"]
    isq_admittance --- isq_admittance_of_vacuum
    isq_apparent_admittance["<b>isq::apparent_admittance</b>"]
    isq_admittance --- isq_apparent_admittance
    isq_susceptance["<b>isq::susceptance</b>"]
    isq_admittance --- isq_susceptance
```
