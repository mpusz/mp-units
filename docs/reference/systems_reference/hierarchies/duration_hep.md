<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# duration Hierarchy

**System:** HEP

**Dimension:** dim_time

```mermaid
flowchart LR
    hep_duration["<b><a href="../../systems/hep/#duration" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::duration</a></b>"]
    hep_duration -.- hep_coordinate_time
    hep_lifetime["<b><a href="../../systems/hep/#lifetime" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::lifetime</a></b>"]
    hep_duration --- hep_lifetime
    hep_half_life["<b><a href="../../systems/hep/#half_life" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::half_life</a></b>"]
    hep_lifetime --- hep_half_life
    hep_mean_lifetime["<b><a href="../../systems/hep/#mean_lifetime" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::mean_lifetime</a></b>"]
    hep_lifetime --- hep_mean_lifetime
    hep_duration -.- hep_proper_time
    hep_time_of_flight["<b><a href="../../systems/hep/#time_of_flight" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::time_of_flight</a></b>"]
    hep_duration --- hep_time_of_flight
    subgraph kind_hep_coordinate_time[" "]
        hep_coordinate_time["<b><a href="../../systems/hep/#coordinate_time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::coordinate_time</a></b>"]
    end
    subgraph kind_hep_proper_time[" "]
        hep_proper_time["<b><a href="../../systems/hep/#proper_time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">hep::proper_time</a></b>"]
    end
```

**Legend:**

- Subgraphs with a dotted line from the parent indicate a distinct quantity kind (created with `subkind`). These subtrees are type-isolated: quantities inside cannot be added or compared to those outside their subgraph without explicit conversion.
