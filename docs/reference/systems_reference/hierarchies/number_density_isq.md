<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# number_density Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_number_density["<b><a href="../../systems/isq/#number_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::number_density</a></b><br><i>(<a href="../../systems/isq/#number_of_entities" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">number_of_entities</a> / <a href="../../systems/isq/#volume" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">volume</a>)</i>"]
    isq_acceptor_density["<b><a href="../../systems/isq/#acceptor_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::acceptor_density</a></b>"]
    isq_number_density --- isq_acceptor_density
    isq_donor_density["<b><a href="../../systems/isq/#donor_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::donor_density</a></b>"]
    isq_number_density --- isq_donor_density
    isq_electron_density["<b><a href="../../systems/isq/#electron_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::electron_density</a></b>"]
    isq_number_density --- isq_electron_density
    isq_hole_density["<b><a href="../../systems/isq/#hole_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::hole_density</a></b>"]
    isq_number_density --- isq_hole_density
    isq_intrinsic_carrier_density["<b><a href="../../systems/isq/#intrinsic_carrier_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::intrinsic_carrier_density</a></b><br><i>(pow<1, 2>(<a href="../../systems/isq/#electron_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">electron_density</a> * <a href="../../systems/isq/#hole_density" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hole_density</a>))</i>"]
    isq_number_density --- isq_intrinsic_carrier_density
```
