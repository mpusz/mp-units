<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# speed Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_speed["<b>isq::speed</b><br><i>(length / time)</i>"]
    isq_speed_of_light_in_a_medium["<b>isq::speed_of_light_in_a_medium</b>"]
    isq_speed --- isq_speed_of_light_in_a_medium
    isq_speed_of_light_in_vacuum["<b>isq::speed_of_light_in_vacuum | isq::light_speed_in_vacuum | isq::luminal_speed</b>"]
    isq_speed --- isq_speed_of_light_in_vacuum
    isq_velocity["<b>isq::velocity</b><br><i>(displacement / duration)</i>"]
    isq_speed --- isq_velocity
```
