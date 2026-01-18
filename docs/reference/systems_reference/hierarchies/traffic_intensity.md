<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# traffic_intensity Hierarchy

**System:** ISQ

**Dimension:** dim_traffic_intensity

```mermaid
flowchart LR
    isq_traffic_intensity["<b><a href="../../systems/isq/#traffic_intensity" style="color: black; text-decoration: none;">isq::traffic_intensity</a></b>"]
    isq_traffic_carried_intensity["<b><a href="../../systems/isq/#traffic_carried_intensity" style="color: black; text-decoration: none;">isq::traffic_carried_intensity</a> | <a href="../../systems/isq/#traffic_load" style="color: black; text-decoration: none;">isq::traffic_load</a></b>"]
    isq_traffic_intensity --- isq_traffic_carried_intensity
    isq_traffic_offered_intensity["<b><a href="../../systems/isq/#traffic_offered_intensity" style="color: black; text-decoration: none;">isq::traffic_offered_intensity</a></b>"]
    isq_traffic_intensity --- isq_traffic_offered_intensity
```
