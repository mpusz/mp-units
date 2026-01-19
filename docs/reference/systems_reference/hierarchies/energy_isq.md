<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# energy Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_energy["<b><a href="../../systems/isq/#energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::energy</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a> * pow<2>(<a href="../../systems/isq/#length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">length</a>) / pow<2>(<a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>))</i>"]
    isq_enthalpy["<b><a href="../../systems/isq/#enthalpy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::enthalpy</a></b>"]
    isq_energy --- isq_enthalpy
    isq_Gibbs_energy["<b><a href="../../systems/isq/#Gibbs_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Gibbs_energy</a> | <a href="../../systems/isq/#Gibbs_function" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Gibbs_function</a></b>"]
    isq_enthalpy --- isq_Gibbs_energy
    isq_internal_energy["<b><a href="../../systems/isq/#internal_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::internal_energy</a> | <a href="../../systems/isq/#thermodynamic_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::thermodynamic_energy</a></b>"]
    isq_enthalpy --- isq_internal_energy
    isq_Helmholtz_energy["<b><a href="../../systems/isq/#Helmholtz_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Helmholtz_energy</a> | <a href="../../systems/isq/#Helmholtz_function" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Helmholtz_function</a></b>"]
    isq_internal_energy --- isq_Helmholtz_energy
    isq_heat["<b><a href="../../systems/isq/#heat" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::heat</a> | <a href="../../systems/isq/#amount_of_heat" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::amount_of_heat</a></b>"]
    isq_energy --- isq_heat
    isq_latent_heat["<b><a href="../../systems/isq/#latent_heat" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::latent_heat</a></b>"]
    isq_heat --- isq_latent_heat
    isq_mechanical_energy["<b><a href="../../systems/isq/#mechanical_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::mechanical_energy</a></b>"]
    isq_energy --- isq_mechanical_energy
    isq_kinetic_energy["<b><a href="../../systems/isq/#kinetic_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::kinetic_energy</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">mass</a> * pow<2>(<a href="../../systems/isq/#speed" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">speed</a>))</i>"]
    isq_mechanical_energy --- isq_kinetic_energy
    isq_potential_energy["<b><a href="../../systems/isq/#potential_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::potential_energy</a></b>"]
    isq_mechanical_energy --- isq_potential_energy
    isq_radiant_energy["<b><a href="../../systems/isq/#radiant_energy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::radiant_energy</a></b>"]
    isq_energy --- isq_radiant_energy
```
