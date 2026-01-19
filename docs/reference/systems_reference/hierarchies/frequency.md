<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# frequency Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_frequency["<b><a href="../../systems/isq/#frequency" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::frequency</a></b><br><i>(inverse(<a href="../../systems/isq/#period_duration" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">period_duration</a>))</i>"]
    isq_clock_frequency["<b><a href="../../systems/isq/#clock_frequency" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::clock_frequency</a> | <a href="../../systems/isq/#clock_rate" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::clock_rate</a></b>"]
    isq_frequency --- isq_clock_frequency
```
