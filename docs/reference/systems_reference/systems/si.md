<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# SI System

**Namespace:** `mp_units::si`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/si.h>`

**Secondary Headers:**

- `<mp-units/systems/si/chrono.h>`
- `<mp-units/systems/si/constants.h>`
- `<mp-units/systems/si/core.h>`
- `<mp-units/systems/si/math.h>`
- `<mp-units/systems/si/prefix_utils.h>`
- `<mp-units/systems/si/prefixes.h>`
- `<mp-units/systems/si/unit_symbols.h>`
- `<mp-units/systems/si/unit_symbols_essential.h>`
- `<mp-units/systems/si/units.h>`

## Prefixes

| Name | Symbol | Definition |
|------|:------:|------------|
| <span id="quecto"></span>`quecto` | q | `mag_power<10, -30>` |
| <span id="ronto"></span>`ronto` | r | `mag_power<10, -27>` |
| <span id="yocto"></span>`yocto` | y | `mag_power<10, -24>` |
| <span id="zepto"></span>`zepto` | z | `mag_power<10, -21>` |
| <span id="atto"></span>`atto` | a | `mag_power<10, -18>` |
| <span id="femto"></span>`femto` | f | `mag_power<10, -15>` |
| <span id="pico"></span>`pico` | p | `mag_power<10, -12>` |
| <span id="nano"></span>`nano` | n | `mag_power<10, -9>` |
| <span id="micro"></span>`micro` | µ (u) | `mag_power<10, -6>` |
| <span id="milli"></span>`milli` | m | `mag_power<10, -3>` |
| <span id="centi"></span>`centi` | c | `mag_power<10, -2>` |
| <span id="deci"></span>`deci` | d | `mag_power<10, -1>` |
| <span id="deca"></span>`deca` | da | `mag_power<10, 1>` |
| <span id="hecto"></span>`hecto` | h | `mag_power<10, 2>` |
| <span id="kilo"></span>`kilo` | k | `mag_power<10, 3>` |
| <span id="mega"></span>`mega` | M | `mag_power<10, 6>` |
| <span id="giga"></span>`giga` | G | `mag_power<10, 9>` |
| <span id="tera"></span>`tera` | T | `mag_power<10, 12>` |
| <span id="peta"></span>`peta` | P | `mag_power<10, 15>` |
| <span id="exa"></span>`exa` | E | `mag_power<10, 18>` |
| <span id="zetta"></span>`zetta` | Z | `mag_power<10, 21>` |
| <span id="yotta"></span>`yotta` | Y | `mag_power<10, 24>` |
| <span id="ronna"></span>`ronna` | R | `mag_power<10, 27>` |
| <span id="quetta"></span>`quetta` | Q | `mag_power<10, 30>` |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="ampere"></span><code>ampere</code> | A | `A` | <code>kind: [isq::electric_<wbr>current](isq.md#electric_current)</code> |
| <span id="becquerel"></span><code>becquerel</code> | Bq | `Bq` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [isq::activity](isq.md#activity)</code> |
| <span id="candela"></span><code>candela</code> | cd | `cd` | <code>kind: [isq::luminous_<wbr>intensity](isq.md#luminous_intensity)</code> |
| <span id="coulomb"></span><code>coulomb</code> | C | `C` | <code>[ampere](#ampere) * <wbr>[second](#second)</code> |
| <span id="degree_Celsius"></span><code>degree_<wbr>Celsius</code> | ℃ (\`C) | `deg_C` | <code>[kelvin](#kelvin)<br>origin: [ice_<wbr>point](#ice_point)</code> |
| <span id="farad"></span><code>farad</code> | F | `F` | <code>[coulomb](#coulomb) / <wbr>[volt](#volt)<br>kind: [isq::capacitance](isq.md#capacitance)</code> |
| <span id="gram"></span><code>gram</code> | g | `g` | <code>kind: [isq::mass](isq.md#mass)</code> |
| <span id="gray"></span><code>gray</code> | Gy | `Gy` | <code>[joule](#joule) / <wbr>[kilogram](#kilogram)<br>kind: [isq::absorbed_<wbr>dose](isq.md#absorbed_dose)</code> |
| <span id="henry"></span><code>henry</code> | H | `H` | <code>[weber](#weber) / <wbr>[ampere](#ampere)</code> |
| <span id="hertz"></span><code>hertz</code> | Hz | `Hz` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [isq::frequency](isq.md#frequency)</code> |
| <span id="joule"></span><code>joule</code> | J | `J` | <code>[newton](#newton) * <wbr>[metre](#metre)<br>kind: [isq::energy](isq.md#energy)</code> |
| <span id="katal"></span><code>katal</code> | kat | `kat` | <code>[mole](#mole) / <wbr>[second](#second)<br>kind: [isq::catalytic_<wbr>activity](isq.md#catalytic_activity)</code> |
| <span id="kelvin"></span><code>kelvin</code> | K | `K` | <code>kind: [isq::thermodynamic_<wbr>temperature](isq.md#thermodynamic_temperature)<br>origin: [absolute_<wbr>zero](#absolute_zero)</code> |
| <span id="kilogram"></span><code>kilogram</code> | — | `kg` | <code>[kilo](#kilo)<[gram](#gram)></code> |
| <span id="lumen"></span><code>lumen</code> | lm | `lm` | <code>[candela](#candela) * <wbr>[steradian](#steradian)<br>kind: [isq::luminous_<wbr>flux](isq.md#luminous_flux)</code> |
| <span id="lux"></span><code>lux</code> | lx | `lx` | <code>[lumen](#lumen) / <wbr>square([metre](#metre))<br>kind: [isq::illuminance](isq.md#illuminance)</code> |
| <span id="metre"></span><code>metre</code> | m | `m` | <code>kind: [isq::length](isq.md#length)</code> |
| <span id="mole"></span><code>mole</code> | mol | `mol` | <code>kind: [isq::amount_<wbr>of_<wbr>substance](isq.md#amount_of_substance)</code> |
| <span id="newton"></span><code>newton</code> | N | `N` | <code>[kilogram](#kilogram) * <wbr>[metre](#metre) / <wbr>square([second](#second))<br>kind: [isq::force](isq.md#force)</code> |
| <span id="ohm"></span><code>ohm</code> | Ω (ohm) | `Ω`, `ohm` | <code>[volt](#volt) / <wbr>[ampere](#ampere)<br>kind: [isq::impedance](isq.md#impedance)</code> |
| <span id="pascal"></span><code>pascal</code> | Pa | `Pa` | <code>[newton](#newton) / <wbr>square([metre](#metre))<br>kind: [isq::pressure](isq.md#pressure)</code> |
| <span id="radian"></span><code>radian</code> | rad | `rad` | <code>[metre](#metre) / <wbr>[metre](#metre)<br>kind: [isq::angular_<wbr>measure](isq.md#angular_measure)</code> |
| <span id="second"></span><code>second</code> | s | `s` | <code>kind: [isq::duration](isq.md#duration)</code> |
| <span id="siemens"></span><code>siemens</code> | S | `S` | <code>[one](core.md#one) / <wbr>[ohm](#ohm)<br>kind: [isq::admittance](isq.md#admittance)</code> |
| <span id="sievert"></span><code>sievert</code> | Sv | `Sv` | <code>[joule](#joule) / <wbr>[kilogram](#kilogram)<br>kind: [isq::dose_<wbr>equivalent](isq.md#dose_equivalent)</code> |
| <span id="steradian"></span><code>steradian</code> | sr | `sr` | <code>square([metre](#metre)) / <wbr>square([metre](#metre))<br>kind: [isq::solid_<wbr>angular_<wbr>measure](isq.md#solid_angular_measure)</code> |
| <span id="tesla"></span><code>tesla</code> | T | `T` | <code>[weber](#weber) / <wbr>square([metre](#metre))<br>kind: [isq::magnetic_<wbr>flux_<wbr>density](isq.md#magnetic_flux_density)</code> |
| <span id="volt"></span><code>volt</code> | V | `V` | <code>[watt](#watt) / <wbr>[ampere](#ampere)<br>kind: [isq::electric_<wbr>potential](isq.md#electric_potential)</code> |
| <span id="watt"></span><code>watt</code> | W | `W` | <code>[joule](#joule) / <wbr>[second](#second)</code> |
| <span id="weber"></span><code>weber</code> | Wb | `Wb` | <code>[volt](#volt) * <wbr>[second](#second)</code> |

## Non-SI units accepted for use with the SI

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="arcminute"></span><code>arcminute</code> | ′ (') | `arcmin` | <code>mag_ratio<1, 60> * <wbr>[degree](#degree)</code> |
| <span id="arcsecond"></span><code>arcsecond</code> | ″ ('') | `arcsec` | <code>mag_ratio<1, 60> * <wbr>[arcminute](#arcminute)</code> |
| <span id="are"></span><code>are</code> | a | `a` | <code>square([deca](#deca)<[metre](#metre)>)</code> |
| <span id="astronomical_unit"></span><code>astronomical_<wbr>unit</code> | au | `au` | <code>mag<149'597'870'700> * <wbr>[metre](#metre)</code> |
| <span id="dalton"></span><code>dalton</code> | Da | `Da` | <code>mag_ratio<16'605'390'666'050, 10'000'000'000'000> * <wbr>mag_power<10, -27> * <wbr>[kilogram](#kilogram)</code> |
| <span id="day"></span><code>day</code> | d | `d` | <code>mag<24> * <wbr>[hour](#hour)</code> |
| <span id="degree"></span><code>degree</code> | ° (deg) | `deg` | <code>mag_ratio<1, 180> * <wbr>[π](core.md#π) * <wbr>[radian](#radian)</code> |
| <span id="electronvolt"></span><code>electronvolt</code> | eV | `eV` | <code>mag_ratio<1'602'176'634, 1'000'000'000> * <wbr>mag_power<10, -19> * <wbr>[joule](#joule)</code> |
| <span id="hectare"></span><code>hectare</code> | — | `ha` | <code>[hecto](#hecto)<[are](#are)></code> |
| <span id="hour"></span><code>hour</code> | h | `h` | <code>mag<60> * <wbr>[minute](#minute)</code> |
| <span id="litre"></span><code>litre</code> | L | `l`, `L` | <code>cubic([deci](#deci)<[metre](#metre)>)</code> |
| <span id="minute"></span><code>minute</code> | min | `min` | <code>mag<60> * <wbr>[second](#second)</code> |
| <span id="tonne"></span><code>tonne</code> | t | `t` | <code>mag<1000> * <wbr>[kilogram](#kilogram)</code> |

!!! note "Namespace"

    These units are defined in the `mp_units::non_si` namespace and are made available in the `mp_units::si` namespace through a using-directive.

## Prefixed unit symbols

Not every unit has a symbol for every prefix. A combination that is
not listed here does not exist, however reasonable it looks.

| Unit | Essential header | Full header adds |
|------|------------------|------------------|
| <a href="#ampere"><code>ampere</code></a> | `fA` `pA` `nA` `uA` `µA` `mA` `kA` | `qA` `rA` `yA` `zA` `aA` `cA` `dA` `daA` `hA` `MA` `GA` `TA` `PA` `EA` `ZA` `YA` `RA` `QA` |
| <a href="#becquerel"><code>becquerel</code></a> | `mBq` `kBq` `MBq` `GBq` `TBq` `PBq` | `qBq` `rBq` `yBq` `zBq` `aBq` `fBq` `pBq` `nBq` `uBq` `µBq` `cBq` `dBq` `daBq` `hBq` `EBq` `ZBq` `YBq` `RBq` `QBq` |
| <a href="#candela"><code>candela</code></a> | `mcd` | `qcd` `rcd` `ycd` `zcd` `acd` `fcd` `pcd` `ncd` `ucd` `µcd` `ccd` `dcd` `dacd` `hcd` `kcd` `Mcd` `Gcd` `Tcd` `Pcd` `Ecd` `Zcd` `Ycd` `Rcd` `Qcd` |
| <a href="#coulomb"><code>coulomb</code></a> | `pC` `nC` `uC` `µC` `mC` | `qC` `rC` `yC` `zC` `aC` `fC` `cC` `dC` `daC` `hC` `kC` `MC` `GC` `TC` `PC` `EC` `ZC` `YC` `RC` `QC` |
| <a href="#farad"><code>farad</code></a> | `fF` `pF` `nF` `uF` `µF` `mF` | `qF` `rF` `yF` `zF` `aF` `cF` `dF` `daF` `hF` `kF` `MF` `GF` `TF` `PF` `EF` `ZF` `YF` `RF` `QF` |
| <a href="#gram"><code>gram</code></a> | `fg` `pg` `ng` `ug` `µg` `mg` `Mg` | `qg` `rg` `yg` `zg` `ag` `cg` `dg` `dag` `hg` `Gg` `Tg` `Pg` `Eg` `Zg` `Yg` `Rg` `Qg` |
| <a href="#gray"><code>gray</code></a> | `uGy` `µGy` `mGy` `cGy` `kGy` | `qGy` `rGy` `yGy` `zGy` `aGy` `fGy` `pGy` `nGy` `dGy` `daGy` `hGy` `MGy` `GGy` `TGy` `PGy` `EGy` `ZGy` `YGy` `RGy` `QGy` |
| <a href="#henry"><code>henry</code></a> | `nH` `uH` `µH` `mH` | `qH` `rH` `yH` `zH` `aH` `fH` `pH` `cH` `dH` `daH` `hH` `kH` `MH` `GH` `TH` `PH` `EH` `ZH` `YH` `RH` `QH` |
| <a href="#hertz"><code>hertz</code></a> | `nHz` `uHz` `µHz` `mHz` `kHz` `MHz` `GHz` `THz` `PHz` | `qHz` `rHz` `yHz` `zHz` `aHz` `fHz` `pHz` `cHz` `dHz` `daHz` `hHz` `EHz` `ZHz` `YHz` `RHz` `QHz` |
| <a href="#joule"><code>joule</code></a> | `aJ` `fJ` `pJ` `nJ` `uJ` `µJ` `mJ` `kJ` `MJ` `GJ` `TJ` `PJ` `EJ` `ZJ` | `qJ` `rJ` `yJ` `zJ` `cJ` `dJ` `daJ` `hJ` `YJ` `RJ` `QJ` |
| <a href="#katal"><code>katal</code></a> | `nkat` `ukat` `µkat` | `qkat` `rkat` `ykat` `zkat` `akat` `fkat` `pkat` `mkat` `ckat` `dkat` `dakat` `hkat` `kkat` `Mkat` `Gkat` `Tkat` `Pkat` `Ekat` `Zkat` `Ykat` `Rkat` `Qkat` |
| <a href="#kelvin"><code>kelvin</code></a> | `pK` `nK` `uK` `µK` `mK` `MK` | `qK` `rK` `yK` `zK` `aK` `fK` `cK` `dK` `daK` `hK` `kK` `GK` `TK` `PK` `EK` `ZK` `YK` `RK` `QK` |
| <a href="#lumen"><code>lumen</code></a> | `klm` | `qlm` `rlm` `ylm` `zlm` `alm` `flm` `plm` `nlm` `ulm` `µlm` `mlm` `clm` `dlm` `dalm` `hlm` `Mlm` `Glm` `Tlm` `Plm` `Elm` `Zlm` `Ylm` `Rlm` `Qlm` |
| <a href="#lux"><code>lux</code></a> | `mlx` `klx` | `qlx` `rlx` `ylx` `zlx` `alx` `flx` `plx` `nlx` `ulx` `µlx` `clx` `dlx` `dalx` `hlx` `Mlx` `Glx` `Tlx` `Plx` `Elx` `Zlx` `Ylx` `Rlx` `Qlx` |
| <a href="#metre"><code>metre</code></a> | `fm` `pm` `nm` `um` `µm` `mm` `cm` `dm` `km` | `qm` `rm` `ym` `zm` `am` `dam` `hm` `Mm` `Gm` `Tm` `Pm` `Em` `Zm` `Ym` `Rm` `Qm` |
| <a href="#mole"><code>mole</code></a> | `fmol` `pmol` `nmol` `umol` `µmol` `mmol` `cmol` `kmol` | `qmol` `rmol` `ymol` `zmol` `amol` `dmol` `damol` `hmol` `Mmol` `Gmol` `Tmol` `Pmol` `Emol` `Zmol` `Ymol` `Rmol` `Qmol` |
| <a href="#newton"><code>newton</code></a> | `pN` `nN` `uN` `µN` `mN` `cN` `daN` `kN` `MN` | `qN` `rN` `yN` `zN` `aN` `fN` `dN` `hN` `GN` `TN` `PN` `EN` `ZN` `YN` `RN` `QN` |
| <a href="#ohm"><code>ohm</code></a> | `uohm` `µΩ` `mohm` `mΩ` `kohm` `kΩ` `Mohm` `MΩ` `Gohm` `GΩ` `Tohm` `TΩ` | `qohm` `qΩ` `rohm` `rΩ` `yohm` `yΩ` `zohm` `zΩ` `aohm` `aΩ` `fohm` `fΩ` `pohm` `pΩ` `nohm` `nΩ` `cohm` `cΩ` `dohm` `dΩ` `daohm` `daΩ` `hohm` `hΩ` `Pohm` `PΩ` `Eohm` `EΩ` `Zohm` `ZΩ` `Yohm` `YΩ` `Rohm` `RΩ` `Qohm` `QΩ` |
| <a href="#pascal"><code>pascal</code></a> | `uPa` `µPa` `mPa` `daPa` `hPa` `kPa` `MPa` `GPa` `TPa` | `qPa` `rPa` `yPa` `zPa` `aPa` `fPa` `pPa` `nPa` `cPa` `dPa` `PPa` `EPa` `ZPa` `YPa` `RPa` `QPa` |
| <a href="#radian"><code>radian</code></a> | `nrad` `urad` `µrad` `mrad` | `qrad` `rrad` `yrad` `zrad` `arad` `frad` `prad` `crad` `drad` `darad` `hrad` `krad` `Mrad` `Grad` `Trad` `Prad` `Erad` `Zrad` `Yrad` `Rrad` `Qrad` |
| <a href="#second"><code>second</code></a> | `as` `fs` `ps` `ns` `us` `µs` `ms` `ks` | `qs` `rs` `ys` `zs` `cs` `ds` `das` `hs` `Ms` `Gs` `Ts` `Ps` `Es` `Zs` `Ys` `Rs` `Qs` |
| <a href="#siemens"><code>siemens</code></a> | `pS` `nS` `uS` `µS` `mS` `dS` `MS` | `qS` `rS` `yS` `zS` `aS` `fS` `cS` `daS` `hS` `kS` `GS` `TS` `PS` `ES` `ZS` `YS` `RS` `QS` |
| <a href="#sievert"><code>sievert</code></a> | `nSv` `uSv` `µSv` `mSv` | `qSv` `rSv` `ySv` `zSv` `aSv` `fSv` `pSv` `cSv` `dSv` `daSv` `hSv` `kSv` `MSv` `GSv` `TSv` `PSv` `ESv` `ZSv` `YSv` `RSv` `QSv` |
| <a href="#steradian"><code>steradian</code></a> | `msr` | `qsr` `rsr` `ysr` `zsr` `asr` `fsr` `psr` `nsr` `usr` `µsr` `csr` `dsr` `dasr` `hsr` `ksr` `Msr` `Gsr` `Tsr` `Psr` `Esr` `Zsr` `Ysr` `Rsr` `Qsr` |
| <a href="#tesla"><code>tesla</code></a> | `fT` `pT` `nT` `uT` `µT` `mT` | `qT` `rT` `yT` `zT` `aT` `cT` `dT` `daT` `hT` `kT` `MT` `GT` `TT` `PT` `ET` `ZT` `YT` `RT` `QT` |
| <a href="#volt"><code>volt</code></a> | `nV` `uV` `µV` `mV` `kV` `MV` | `qV` `rV` `yV` `zV` `aV` `fV` `pV` `cV` `dV` `daV` `hV` `GV` `TV` `PV` `EV` `ZV` `YV` `RV` `QV` |
| <a href="#watt"><code>watt</code></a> | `pW` `nW` `uW` `µW` `mW` `kW` `MW` `GW` `TW` `PW` | `qW` `rW` `yW` `zW` `aW` `fW` `cW` `dW` `daW` `hW` `EW` `ZW` `YW` `RW` `QW` |
| <a href="#weber"><code>weber</code></a> | — | `qWb` `rWb` `yWb` `zWb` `aWb` `fWb` `pWb` `nWb` `uWb` `µWb` `mWb` `cWb` `dWb` `daWb` `hWb` `kWb` `MWb` `GWb` `TWb` `PWb` `EWb` `ZWb` `YWb` `RWb` `QWb` |

!!! note "Two headers"

    The essential symbols come from `<mp-units/systems/si/unit_symbols_essential.h>`. The rest need
    `<mp-units/systems/si/unit_symbols.h>`, which is more expensive to compile, so include it
    only when you need an unusual prefix.

## Constants

| Name | Symbol | unit_symbol | Definition |
|------|:------:|:------------:|------------|
| <span id="magnetic_constant"></span><code>magnetic_<wbr>constant</code> | μ₀ (u_<wbr>0) | — | <code>mag<4> * <wbr>mag_power<10, -7> * <wbr>[π](core.md#π) * <wbr>[henry](#henry) / <wbr>[metre](#metre)</code> |
| <span id="reduced_planck_constant"></span><code>reduced_<wbr>planck_<wbr>constant</code> | ℏ (hbar) | — | <code>[si2019::planck_<wbr>constant](#si2019-planck_constant) / <wbr>(mag<2> * <wbr>[π](core.md#π))</code> |
| <span id="si2019-avogadro_constant"></span><code>si2019::avogadro_<wbr>constant</code> | N_<wbr>A | — | <code>mag_ratio<602'214'076, 100'000'000> * <wbr>mag_power<10, 23> / <wbr>[mole](#mole)</code> |
| <span id="si2019-boltzmann_constant"></span><code>si2019::boltzmann_<wbr>constant</code> | k | — | <code>mag_ratio<1'380'649, 1'000'000> * <wbr>mag_power<10, -23> * <wbr>[joule](#joule) / <wbr>[kelvin](#kelvin)</code> |
| <span id="si2019-elementary_charge"></span><code>si2019::elementary_<wbr>charge</code> | e | — | <code>mag_ratio<1'602'176'634, 1'000'000'000> * <wbr>mag_power<10, -19> * <wbr>[coulomb](#coulomb)</code> |
| <span id="si2019-hyperfine_structure_transition_frequency_of_cs"></span><code>si2019::hyperfine_<wbr>structure_<wbr>transition_<wbr>frequency_<wbr>of_<wbr>cs</code> | Δν_<wbr>Cs (dv_<wbr>Cs) | — | <code>mag<9'192'631'770> * <wbr>[hertz](#hertz)</code> |
| <span id="si2019-luminous_efficacy"></span><code>si2019::luminous_<wbr>efficacy</code> | K_<wbr>cd | — | <code>mag<683> * <wbr>[lumen](#lumen) / <wbr>[watt](#watt)</code> |
| <span id="si2019-planck_constant"></span><code>si2019::planck_<wbr>constant</code> | h | — | <code>mag_ratio<662'607'015, 100'000'000> * <wbr>mag_power<10, -34> * <wbr>[joule](#joule) * <wbr>[second](#second)</code> |
| <span id="si2019-speed_of_light_in_vacuum"></span><code>si2019::speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum</code> | c | — | <code>mag<299'792'458> * <wbr>[metre](#metre) / <wbr>[second](#second)</code> |
| <span id="standard_gravity"></span><code>standard_<wbr>gravity</code> | g₀ (g_<wbr>0) | — | <code>mag_ratio<980'665, 100'000> * <wbr>[metre](#metre) / <wbr>square([second](#second))</code> |

!!! note "Inline Namespaces"

    The `mp_units::si::si2019` namespace is inline in `mp_units::si`, making its members directly accessible from the parent namespace.

## Point Origins

| Name | Type | Definition |
|------|:----:|------------|
| <span id="absolute_zero"></span>`absolute_zero` | absolute | <code>[isq::thermodynamic_<wbr>temperature](isq.md#thermodynamic_temperature)</code> |
| <span id="chrono_point_origin"></span>`chrono_point_origin` | absolute | <code>[isq::time](isq.md#time)</code> |
| <span id="ice_point"></span>`ice_point` | relative | <code>::mp_units::point<[milli](#milli)<[kelvin](#kelvin)>>(273'150)</code> |
| <span id="zeroth_degree_Celsius"></span>`zeroth_degree_Celsius` | relative | alias to [ice_<wbr>point](#ice_point) |
| <span id="zeroth_kelvin"></span>`zeroth_kelvin` | absolute | alias to [absolute_<wbr>zero](#absolute_zero) |
