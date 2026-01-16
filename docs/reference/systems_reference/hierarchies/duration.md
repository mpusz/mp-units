<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# duration Hierarchy

**System:** ISQ

**Dimension:** dim_time

```mermaid
flowchart LR
    isq_duration["<b>isq::duration | isq::time</b>"]
    isq_period_duration["<b>isq::period_duration | isq::period</b>"]
    isq_duration --- isq_period_duration
    isq_period_of_binary_digits["<b>isq::period_of_binary_digits | isq::bit_period</b><br><i>(inverse(binary_digit_rate))</i>"]
    isq_period_duration --- isq_period_of_binary_digits
    isq_period_of_data_elements["<b>isq::period_of_data_elements</b><br><i>(inverse(transfer_rate))</i>"]
    isq_period_duration --- isq_period_of_data_elements
    isq_time_constant["<b>isq::time_constant</b>"]
    isq_duration --- isq_time_constant
```
