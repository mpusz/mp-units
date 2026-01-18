<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# duration Hierarchy

**System:** ISQ

**Dimension:** dim_time

```mermaid
flowchart LR
    isq_duration["<b><a href="../../systems/isq/#duration" style="color: black; text-decoration: none;">isq::duration</a> | <a href="../../systems/isq/#time" style="color: black; text-decoration: none;">isq::time</a></b>"]
    isq_period_duration["<b><a href="../../systems/isq/#period_duration" style="color: black; text-decoration: none;">isq::period_duration</a> | <a href="../../systems/isq/#period" style="color: black; text-decoration: none;">isq::period</a></b>"]
    isq_duration --- isq_period_duration
    isq_period_of_binary_digits["<b><a href="../../systems/isq/#period_of_binary_digits" style="color: black; text-decoration: none;">isq::period_of_binary_digits</a> | <a href="../../systems/isq/#bit_period" style="color: black; text-decoration: none;">isq::bit_period</a></b><br><i>(inverse(<a href="../../systems/isq/#binary_digit_rate" style="color: black; text-decoration: none;">binary_digit_rate</a>))</i>"]
    isq_period_duration --- isq_period_of_binary_digits
    isq_period_of_data_elements["<b><a href="../../systems/isq/#period_of_data_elements" style="color: black; text-decoration: none;">isq::period_of_data_elements</a></b><br><i>(inverse(<a href="../../systems/isq/#transfer_rate" style="color: black; text-decoration: none;">transfer_rate</a>))</i>"]
    isq_period_duration --- isq_period_of_data_elements
    isq_time_constant["<b><a href="../../systems/isq/#time_constant" style="color: black; text-decoration: none;">isq::time_constant</a></b>"]
    isq_duration --- isq_time_constant
```
