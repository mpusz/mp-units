<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# transfer_rate Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_transfer_rate["<b><a href="../../systems/isq/#transfer_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::transfer_rate</a></b><br><i>(<a href="../../systems/isq/#storage_capacity" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">storage_capacity</a> / isq::<a href="../../systems/isq/#duration" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">duration</a>)</i>"]
    isq_binary_digit_rate["<b><a href="../../systems/isq/#binary_digit_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::binary_digit_rate</a> | <a href="../../systems/isq/#bit_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::bit_rate</a></b>"]
    isq_transfer_rate --- isq_binary_digit_rate
    isq_equivalent_binary_digit_rate["<b><a href="../../systems/isq/#equivalent_binary_digit_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::equivalent_binary_digit_rate</a></b>"]
    isq_binary_digit_rate --- isq_equivalent_binary_digit_rate
```
