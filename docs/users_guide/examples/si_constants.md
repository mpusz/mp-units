---
tags:
  - Physical Constants
  - Text Formatting
---

# `si_constants`

!!! example "[Try it on Compiler Explorer](https://godbolt.org/z/MevcK8vYT)"

The next example presents all the seven defining constants of the SI system. We can observe
how [Faster-than-lightspeed Constants](../framework_basics/faster_than_lightspeed_constants.md)
work in practice.

```cpp title="si_constants.cpp" linenums="1"
--8<-- "example/si_constants.cpp:28:40"
```

As always, we start with the inclusion of all the needed header files. After that, for
the simplicity of this example, we
[hack the character of quantities](../framework_basics/character_of_a_quantity.md#hacking-the-character)
to be able to express vector quantities with simple scalar types.

```cpp title="si_constants.cpp" linenums="14"
--8<-- "example/si_constants.cpp:42:"
```

The main part of the example prints all of the SI-defining constants. While analyzing the output of
this program (provided below), we can easily notice that a direct printing of the quantity provides
just a value `1` with a proper constant symbol. This is the main power of the
[Faster-than-lightspeed Constants](../framework_basics/faster_than_lightspeed_constants.md) feature.
Only after we explicitly convert the unit of a quantity to proper SI units we get an actual numeric
value of the constant.

```text
The seven defining constants of the SI and the seven corresponding units they define:
- hyperfine transition frequency of Cs: 1 Δν_Cs = 9192631770 Hz
- speed of light in vacuum:             1 c = 299792458 m/s
- Planck constant:                      1 h = 6.62607015e-34 J s
- elementary charge:                    1 e = 1.602176634e-19 C
- Boltzmann constant:                   1 k = 1.380649e-23 J/K
- Avogadro constant:                    1 N_A = 6.02214076e+23 1/mol
- luminous efficacy:                    1 K_cd = 683 lm/W
```
