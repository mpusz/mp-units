<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# specific_energy Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_specific_energy["<b><a href="../../systems/isq/#specific_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_energy</a></b><br><i>(<a href="../../systems/isq/#energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">energy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_specific_internal_energy["<b><a href="../../systems/isq/#specific_internal_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_internal_energy</a> | <a href="../../systems/isq/#specific_thermodynamic_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_thermodynamic_energy</a></b><br><i>(<a href="../../systems/isq/#internal_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">internal_energy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_specific_energy --- isq_specific_internal_energy
    isq_specific_Helmholtz_energy["<b><a href="../../systems/isq/#specific_Helmholtz_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_Helmholtz_energy</a> | <a href="../../systems/isq/#specific_Helmholtz_function" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_Helmholtz_function</a></b><br><i>(<a href="../../systems/isq/#Helmholtz_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">Helmholtz_energy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_specific_internal_energy --- isq_specific_Helmholtz_energy
    isq_specific_enthalpy["<b><a href="../../systems/isq/#specific_enthalpy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_enthalpy</a></b><br><i>(<a href="../../systems/isq/#enthalpy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">enthalpy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_specific_internal_energy --- isq_specific_enthalpy
    isq_specific_Gibbs_energy["<b><a href="../../systems/isq/#specific_Gibbs_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_Gibbs_energy</a> | <a href="../../systems/isq/#specific_Gibbs_function" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::specific_Gibbs_function</a></b><br><i>(<a href="../../systems/isq/#Gibbs_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">Gibbs_energy</a> / <a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a>)</i>"]
    isq_specific_enthalpy --- isq_specific_Gibbs_energy
```
