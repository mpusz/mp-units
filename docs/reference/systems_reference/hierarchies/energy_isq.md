<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# energy Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_energy["<b>isq::energy</b><br><i>(mass * pow<2>(length) / pow<2>(time))</i>"]
    isq_enthalpy["<b>isq::enthalpy</b>"]
    isq_energy --- isq_enthalpy
    isq_Gibbs_energy["<b>isq::Gibbs_energy | isq::Gibbs_function</b>"]
    isq_enthalpy --- isq_Gibbs_energy
    isq_internal_energy["<b>isq::internal_energy | isq::thermodynamic_energy</b>"]
    isq_enthalpy --- isq_internal_energy
    isq_Helmholtz_energy["<b>isq::Helmholtz_energy | isq::Helmholtz_function</b>"]
    isq_internal_energy --- isq_Helmholtz_energy
    isq_heat["<b>isq::heat | isq::amount_of_heat</b>"]
    isq_energy --- isq_heat
    isq_latent_heat["<b>isq::latent_heat</b>"]
    isq_heat --- isq_latent_heat
    isq_mechanical_energy["<b>isq::mechanical_energy</b>"]
    isq_energy --- isq_mechanical_energy
    isq_kinetic_energy["<b>isq::kinetic_energy</b><br><i>(mass * pow<2>(speed))</i>"]
    isq_mechanical_energy --- isq_kinetic_energy
    isq_potential_energy["<b>isq::potential_energy</b>"]
    isq_mechanical_energy --- isq_potential_energy
    isq_radiant_energy["<b>isq::radiant_energy</b>"]
    isq_energy --- isq_radiant_energy
```
