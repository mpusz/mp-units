<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# transfer_rate Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_transfer_rate["<b>isq::transfer_rate</b><br><i>(storage_capacity / isq::duration)</i>"]
    isq_binary_digit_rate["<b>isq::binary_digit_rate | isq::bit_rate</b>"]
    isq_transfer_rate --- isq_binary_digit_rate
    isq_equivalent_binary_digit_rate["<b>isq::equivalent_binary_digit_rate</b>"]
    isq_binary_digit_rate --- isq_equivalent_binary_digit_rate
```
