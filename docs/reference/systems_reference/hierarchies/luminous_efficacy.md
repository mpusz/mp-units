<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# luminous_efficacy Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_luminous_efficacy["<b><a href="../../systems/isq/#luminous_efficacy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::luminous_efficacy</a></b><br><i>(<a href="../../systems/isq/#luminous_flux" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">luminous_flux</a> / <a href="../../systems/isq/#power" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">power</a>)</i>"]
    isq_luminous_efficacy -.- isq_luminous_efficacy_of_radiation
    isq_luminous_efficacy -.- isq_luminous_efficacy_of_source
    subgraph kind_isq_luminous_efficacy_of_radiation[" "]
        isq_luminous_efficacy_of_radiation["<b><a href="../../systems/isq/#luminous_efficacy_of_radiation" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::luminous_efficacy_of_radiation</a></b><br><i>(<a href="../../systems/isq/#luminous_flux" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">luminous_flux</a> / <a href="../../systems/isq/#radiant_flux" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">radiant_flux</a>)</i>"]
        isq_maximum_luminous_efficacy["<b><a href="../../systems/isq/#maximum_luminous_efficacy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::maximum_luminous_efficacy</a></b>"]
        isq_luminous_efficacy_of_radiation --- isq_maximum_luminous_efficacy
        isq_spectral_luminous_efficacy["<b><a href="../../systems/isq/#spectral_luminous_efficacy" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::spectral_luminous_efficacy</a></b>"]
        isq_luminous_efficacy_of_radiation --- isq_spectral_luminous_efficacy
    end
    subgraph kind_isq_luminous_efficacy_of_source[" "]
        isq_luminous_efficacy_of_source["<b><a href="../../systems/isq/#luminous_efficacy_of_source" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::luminous_efficacy_of_source</a></b>"]
    end
```

**Legend:**

- Subgraphs with a dotted line from the parent indicate a distinct quantity kind (created with `is_kind`). These subtrees are type-isolated: quantities inside cannot be added or compared to those outside their subgraph without explicit conversion.
