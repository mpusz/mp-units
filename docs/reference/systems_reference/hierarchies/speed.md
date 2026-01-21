<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# speed Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_speed["<b><a href="../../systems/isq/#speed" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::speed</a></b><br><i>(<a href="../../systems/isq/#length" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">length</a> / <a href="../../systems/isq/#time" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">time</a>)</i>"]
    isq_speed_of_light_in_a_medium["<b><a href="../../systems/isq/#speed_of_light_in_a_medium" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::speed_of_light_in_a_medium</a></b>"]
    isq_speed --- isq_speed_of_light_in_a_medium
    isq_speed_of_light_in_vacuum["<b><a href="../../systems/isq/#speed_of_light_in_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::speed_of_light_in_vacuum</a> | <a href="../../systems/isq/#light_speed_in_vacuum" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::light_speed_in_vacuum</a> | <a href="../../systems/isq/#luminal_speed" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::luminal_speed</a></b>"]
    isq_speed --- isq_speed_of_light_in_vacuum
    isq_velocity["<b><a href="../../systems/isq/#velocity" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::velocity</a></b><br><i>(<a href="../../systems/isq/#displacement" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">displacement</a> / <a href="../../systems/isq/#duration" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">duration</a>)</i>"]
    isq_speed --- isq_velocity
```
