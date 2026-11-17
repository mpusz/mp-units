<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# absorbed_dose Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_absorbed_dose["<b><a href="../../systems/isq/#absorbed_dose" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::absorbed_dose</a></b><br><i>(<a href="../../systems/isq/#energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">energy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_kerma["<b><a href="../../systems/isq/#kerma" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::kerma</a></b>"]
    isq_absorbed_dose --- isq_kerma
    isq_specific_energy_imparted["<b><a href="../../systems/isq/#specific_energy_imparted" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_energy_imparted</a></b>"]
    isq_absorbed_dose --- isq_specific_energy_imparted
```
