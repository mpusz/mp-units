<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# angular_repetency Hierarchy

**System:** ISQ

```mermaid
flowchart LR
    isq_angular_repetency["<b><a href="../../systems/isq/#angular_repetency" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::angular_repetency</a> | <a href="../../systems/isq/#angular_wavenumber" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::angular_wavenumber</a></b><br><i>(inverse(<a href="../../systems/isq/#wavelength" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">wavelength</a>))</i>"]
    isq_Debye_angular_wavenumber["<b><a href="../../systems/isq/#Debye_angular_wavenumber" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Debye_angular_wavenumber</a> | <a href="../../systems/isq/#Debye_angular_repetency" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Debye_angular_repetency</a></b>"]
    isq_angular_repetency --- isq_Debye_angular_wavenumber
    isq_Fermi_angular_wavenumber["<b><a href="../../systems/isq/#Fermi_angular_wavenumber" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Fermi_angular_wavenumber</a> | <a href="../../systems/isq/#Fermi_angular_repetency" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::Fermi_angular_repetency</a></b>"]
    isq_angular_repetency --- isq_Fermi_angular_wavenumber
    isq_wave_vector["<b><a href="../../systems/isq/#wave_vector" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::wave_vector</a></b>"]
    isq_angular_repetency --- isq_wave_vector
    isq_angular_reciprocal_lattice_vector["<b><a href="../../systems/isq/#angular_reciprocal_lattice_vector" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::angular_reciprocal_lattice_vector</a></b>"]
    isq_wave_vector --- isq_angular_reciprocal_lattice_vector
    isq_fundamental_reciprocal_lattice_vector["<b><a href="../../systems/isq/#fundamental_reciprocal_lattice_vector" style="color: var(--md-mermaid-label-fg-color); text-decoration: none;">isq::fundamental_reciprocal_lattice_vector</a></b>"]
    isq_angular_reciprocal_lattice_vector --- isq_fundamental_reciprocal_lattice_vector
```
