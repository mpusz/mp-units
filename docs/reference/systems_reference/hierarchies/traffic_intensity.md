<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# traffic_intensity Hierarchy

**System:** ISQ

**Dimension:** dim_traffic_intensity

```mermaid
flowchart LR
    isq_traffic_intensity["<b>isq::traffic_intensity</b>"]
    isq_traffic_carried_intensity["<b>isq::traffic_carried_intensity | isq::traffic_load</b>"]
    isq_traffic_intensity --- isq_traffic_carried_intensity
    isq_traffic_offered_intensity["<b>isq::traffic_offered_intensity</b>"]
    isq_traffic_intensity --- isq_traffic_offered_intensity
```
