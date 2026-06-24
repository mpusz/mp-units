<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# HEP System

**Namespace:** `mp_units::hep`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/hep.h>`

**Secondary Headers:**

- `<mp-units/systems/hep/constants.h>`
- `<mp-units/systems/hep/quantities.h>`
- `<mp-units/systems/hep/units.h>`

## Dimensions

| Name | Symbol |
|------|:------:|
| <span id="dim_amount_of_substance"></span>`dim_amount_of_substance` | N |
| <span id="dim_angle"></span>`dim_angle` | α |
| <span id="dim_electric_charge"></span>`dim_electric_charge` | Q |
| <span id="dim_energy"></span>`dim_energy` | E |
| <span id="dim_length"></span>`dim_length` | L |
| <span id="dim_luminous_intensity"></span>`dim_luminous_intensity` | I |
| <span id="dim_temperature"></span>`dim_temperature` | Θ |
| <span id="dim_time"></span>`dim_time` | T |

## Quantities

| Quantity | Character | Dimension | Traits | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:------:|:-------:|:------:|----------|:---------:|
| <span id="Q_value"></span><code>Q_<wbr>value</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="absorbed_dose"></span><code>absorbed_<wbr>dose</code> | scalar | L²T⁻² | kind | <code>[absorbed_<wbr>dose](#absorbed_dose)</code> | — | <code>pow<2>([length](#length)) / <wbr>pow<2>([duration](#duration))</code> | [view](../hierarchies/absorbed_dose_hep.md) |
| <span id="activity"></span><code>activity</code> | scalar | T⁻¹ | kind | <code>[activity](#activity)</code> | — | <code>inverse([duration](#duration))</code> | [view](../hierarchies/activity_hep.md) |
| <span id="amount_of_substance"></span><code>amount_<wbr>of_<wbr>substance</code> | scalar | N | kind<br>≥ 0 | <code>[amount_<wbr>of_<wbr>substance](#amount_of_substance)</code> | — | — | [view](../hierarchies/amount_of_substance_hep.md) |
| <span id="angle"></span><code>angle</code> | scalar | — |  | — | — | alias to [angular::angle](#angle) | — |
| <span id="area"></span><code>area</code> | scalar | L² | kind | <code>[area](#area)</code> | — | <code>pow<2>([length](#length))</code> | [view](../hierarchies/area_hep.md) |
| <span id="azimuthal_angle"></span><code>azimuthal_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](#angle)</code> | <code>[angular::angle](#angle)</code> | — | [view](../hierarchies/angle.md) |
| <span id="binding_energy"></span><code>binding_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="center_of_mass_energy"></span><code>center_<wbr>of_<wbr>mass_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="coordinate_time"></span><code>coordinate_<wbr>time</code> | scalar | T | kind<br>≥ 0 | <code>[coordinate_<wbr>time](#coordinate_time)</code> | <code>[duration](#duration)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="cross_section"></span><code>cross_<wbr>section</code> | scalar | L² |  | <code>[area](#area)</code> | <code>[area](#area)</code> | — | [view](../hierarchies/area_hep.md) |
| <span id="decay_constant"></span><code>decay_<wbr>constant</code> | scalar | T⁻¹ |  | <code>[frequency](#frequency)</code> | <code>[frequency](#frequency)</code> | — | [view](../hierarchies/frequency_hep.md) |
| <span id="decay_length"></span><code>decay_<wbr>length</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="dimensionless"></span><code>dimensionless</code> | scalar | 1 | kind | <code>[dimensionless](core.md#dimensionless)</code> | — | — | — |
| <span id="displacement"></span><code>displacement</code> | vector | L |  | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="duration"></span><code>duration</code> | scalar | T | kind<br>≥ 0 | <code>[duration](#duration)</code> | — | — | [view](../hierarchies/duration_hep.md) |
| <span id="effective_mass"></span><code>effective_<wbr>mass</code> | scalar | ET²L⁻² |  | <code>[mass](#mass)</code> | <code>[mass](#mass)</code> | — | [view](../hierarchies/mass_hep.md) |
| <span id="electric_capacitance"></span><code>electric_<wbr>capacitance</code> | scalar | Q²E⁻¹ | kind | <code>[electric_<wbr>capacitance](#electric_capacitance)</code> | — | <code>pow<2>([electric_<wbr>charge](#electric_charge)) / <wbr>[energy](#energy)</code> | [view](../hierarchies/electric_capacitance.md) |
| <span id="electric_charge"></span><code>electric_<wbr>charge</code> | scalar | Q | kind | <code>[electric_<wbr>charge](#electric_charge)</code> | — | — | [view](../hierarchies/electric_charge_hep.md) |
| <span id="electric_current"></span><code>electric_<wbr>current</code> | scalar | QT⁻¹ | kind | <code>[electric_<wbr>current](#electric_current)</code> | — | <code>[electric_<wbr>charge](#electric_charge) / <wbr>[duration](#duration)</code> | [view](../hierarchies/electric_current_hep.md) |
| <span id="electric_potential"></span><code>electric_<wbr>potential</code> | scalar | EQ⁻¹ | kind | <code>[electric_<wbr>potential](#electric_potential)</code> | — | <code>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge)</code> | [view](../hierarchies/electric_potential_hep.md) |
| <span id="electric_resistance"></span><code>electric_<wbr>resistance</code> | scalar | ETQ⁻² | kind | <code>[electric_<wbr>resistance](#electric_resistance)</code> | — | <code>[energy](#energy) * <wbr>[duration](#duration) / <wbr>pow<2>([electric_<wbr>charge](#electric_charge))</code> | [view](../hierarchies/electric_resistance.md) |
| <span id="energy"></span><code>energy</code> | scalar | E | kind<br>≥ 0 | <code>[energy](#energy)</code> | — | — | [view](../hierarchies/energy_hep.md) |
| <span id="excitation_energy"></span><code>excitation_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="force"></span><code>force</code> | scalar | EL⁻¹ | kind | <code>[force](#force)</code> | — | <code>[energy](#energy) / <wbr>[length](#length)</code> | [view](../hierarchies/force_hep.md) |
| <span id="frequency"></span><code>frequency</code> | scalar | T⁻¹ | kind | <code>[frequency](#frequency)</code> | — | <code>inverse([duration](#duration))</code> | [view](../hierarchies/frequency_hep.md) |
| <span id="half_life"></span><code>half_<wbr>life</code> | scalar | T | ≥ 0 | <code>[duration](#duration)</code> | <code>[lifetime](#lifetime)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="height"></span><code>height</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="illuminance"></span><code>illuminance</code> | scalar | α²IL⁻² | kind | <code>[illuminance](#illuminance)</code> | — | <code>[luminous_<wbr>flux](#luminous_flux) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/illuminance_hep.md) |
| <span id="impact_parameter"></span><code>impact_<wbr>parameter</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="inductance"></span><code>inductance</code> | scalar | ET²Q⁻² | kind | <code>[inductance](#inductance)</code> | — | <code>pow<2>([duration](#duration)) * <wbr>[energy](#energy) / <wbr>pow<2>([electric_<wbr>charge](#electric_charge))</code> | [view](../hierarchies/inductance_hep.md) |
| <span id="interaction_length"></span><code>interaction_<wbr>length</code> | scalar | L | kind<br>≥ 0 | <code>[interaction_<wbr>length](#interaction_length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="invariant_mass"></span><code>invariant_<wbr>mass</code> | scalar | ET²L⁻² |  | <code>[mass](#mass)</code> | <code>[mass](#mass)</code> | — | [view](../hierarchies/mass_hep.md) |
| <span id="ionization_energy"></span><code>ionization_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="kinetic_energy"></span><code>kinetic_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[total_<wbr>energy](#total_energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="length"></span><code>length</code> | scalar | L | kind<br>≥ 0 | <code>[length](#length)</code> | — | — | [view](../hierarchies/length_hep.md) |
| <span id="lifetime"></span><code>lifetime</code> | scalar | T | ≥ 0 | <code>[duration](#duration)</code> | <code>[duration](#duration)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="lorentz_factor"></span><code>lorentz_<wbr>factor</code> | scalar | — | kind<br>≥ 0 | <code>[lorentz_<wbr>factor](#lorentz_factor)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="luminous_flux"></span><code>luminous_<wbr>flux</code> | scalar | α²I | kind | <code>[luminous_<wbr>flux](#luminous_flux)</code> | — | <code>[luminous_<wbr>intensity](#luminous_intensity) * <wbr>[solid_<wbr>angle](#solid_angle)</code> | [view](../hierarchies/luminous_flux_hep.md) |
| <span id="luminous_intensity"></span><code>luminous_<wbr>intensity</code> | scalar | I | kind<br>≥ 0 | <code>[luminous_<wbr>intensity](#luminous_intensity)</code> | — | — | [view](../hierarchies/luminous_intensity_hep.md) |
| <span id="magnetic_field"></span><code>magnetic_<wbr>field</code> | scalar | ETQ⁻¹L⁻² | kind | <code>[magnetic_<wbr>field](#magnetic_field)</code> | — | <code>[duration](#duration) * <wbr>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/magnetic_field.md) |
| <span id="magnetic_flux"></span><code>magnetic_<wbr>flux</code> | scalar | ETQ⁻¹ | kind | <code>[magnetic_<wbr>flux](#magnetic_flux)</code> | — | <code>[duration](#duration) * <wbr>[energy](#energy) / <wbr>[electric_<wbr>charge](#electric_charge)</code> | [view](../hierarchies/magnetic_flux_hep.md) |
| <span id="mass"></span><code>mass</code> | scalar | ET²L⁻² | kind | <code>[mass](#mass)</code> | — | <code>[energy](#energy) * <wbr>pow<2>([duration](#duration)) / <wbr>pow<2>([length](#length))</code> | [view](../hierarchies/mass_hep.md) |
| <span id="mean_free_path"></span><code>mean_<wbr>free_<wbr>path</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="mean_lifetime"></span><code>mean_<wbr>lifetime</code> | scalar | T | ≥ 0 | <code>[duration](#duration)</code> | <code>[lifetime](#lifetime)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="missing_energy"></span><code>missing_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="momentum"></span><code>momentum</code> | scalar | ETL⁻¹ | kind | <code>[momentum](#momentum)</code> | — | <code>[energy](#energy) * <wbr>[duration](#duration) / <wbr>[length](#length)</code> | [view](../hierarchies/momentum_hep.md) |
| <span id="nuclear_interaction_length"></span><code>nuclear_<wbr>interaction_<wbr>length</code> | scalar | L | ≥ 0 | <code>[interaction_<wbr>length](#interaction_length)</code> | <code>[interaction_<wbr>length](#interaction_length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="number_density"></span><code>number_<wbr>density</code> | scalar | L⁻³ | kind | <code>[number_<wbr>density](#number_density)</code> | — | <code>inverse([volume](#volume))</code> | [view](../hierarchies/number_density.md) |
| <span id="opening_angle"></span><code>opening_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](#angle)</code> | <code>[angular::angle](#angle)</code> | — | [view](../hierarchies/angle.md) |
| <span id="path_length"></span><code>path_<wbr>length</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="phase"></span><code>phase</code> | scalar | — | kind | <code>[phase](#phase)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="polar_angle"></span><code>polar_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](#angle)</code> | <code>[angular::angle](#angle)</code> | — | [view](../hierarchies/angle.md) |
| <span id="position_vector"></span><code>position_<wbr>vector</code> | vector | L |  | <code>[length](#length)</code> | <code>[displacement](#displacement)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="power"></span><code>power</code> | scalar | ET⁻¹ | kind | <code>[power](#power)</code> | — | <code>[energy](#energy) / <wbr>[duration](#duration)</code> | [view](../hierarchies/power_hep.md) |
| <span id="pressure"></span><code>pressure</code> | scalar | EL⁻³ | kind | <code>[pressure](#pressure)</code> | — | <code>[energy](#energy) / <wbr>pow<3>([length](#length))</code> | [view](../hierarchies/pressure_hep.md) |
| <span id="proper_time"></span><code>proper_<wbr>time</code> | scalar | T | kind<br>≥ 0 | <code>[proper_<wbr>time](#proper_time)</code> | <code>[duration](#duration)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="proper_velocity"></span><code>proper_<wbr>velocity</code> | vector | LT⁻¹ | kind | <code>[proper_<wbr>velocity](#proper_velocity)</code> | — | <code>[displacement](#displacement) / <wbr>[proper_<wbr>time](#proper_time)</code> | [view](../hierarchies/proper_velocity.md) |
| <span id="radiation_length"></span><code>radiation_<wbr>length</code> | scalar | L | kind<br>≥ 0 | <code>[radiation_<wbr>length](#radiation_length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="radius"></span><code>radius</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="range"></span><code>range</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="reduced_mass"></span><code>reduced_<wbr>mass</code> | scalar | ET²L⁻² |  | <code>[mass](#mass)</code> | <code>[mass](#mass)</code> | — | [view](../hierarchies/mass_hep.md) |
| <span id="relativistic_beta"></span><code>relativistic_<wbr>beta</code> | scalar | — | kind<br>≥ 0 | <code>[relativistic_<wbr>beta](#relativistic_beta)</code> | <code>[dimensionless](core.md#dimensionless)</code> | — | [view](../hierarchies/dimensionless.md) |
| <span id="rest_mass"></span><code>rest_<wbr>mass</code> | scalar | ET²L⁻² |  | <code>[mass](#mass)</code> | <code>[mass](#mass)</code> | — | [view](../hierarchies/mass_hep.md) |
| <span id="rest_mass_energy"></span><code>rest_<wbr>mass_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[total_<wbr>energy](#total_energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="scattering_angle"></span><code>scattering_<wbr>angle</code> | scalar | α |  | <code>[angular::angle](#angle)</code> | <code>[angular::angle](#angle)</code> | — | [view](../hierarchies/angle.md) |
| <span id="separation_energy"></span><code>separation_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[binding_<wbr>energy](#binding_energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="solid_angle"></span><code>solid_<wbr>angle</code> | scalar | — |  | — | — | alias to [angular::solid_<wbr>angle](#solid_angle) | — |
| <span id="speed"></span><code>speed</code> | scalar | LT⁻¹ | kind | <code>[speed](#speed)</code> | — | <code>[path_<wbr>length](#path_length) / <wbr>[duration](#duration)</code> | [view](../hierarchies/speed_hep.md) |
| <span id="temperature"></span><code>temperature</code> | scalar | Θ | kind<br>≥ 0 | <code>[temperature](#temperature)</code> | — | — | [view](../hierarchies/temperature.md) |
| <span id="threshold_energy"></span><code>threshold_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="time_of_flight"></span><code>time_<wbr>of_<wbr>flight</code> | scalar | T | ≥ 0 | <code>[duration](#duration)</code> | <code>[duration](#duration)</code> | — | [view](../hierarchies/duration_hep.md) |
| <span id="total_energy"></span><code>total_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="transverse_energy"></span><code>transverse_<wbr>energy</code> | scalar | E | ≥ 0 | <code>[energy](#energy)</code> | <code>[energy](#energy)</code> | — | [view](../hierarchies/energy_hep.md) |
| <span id="transverse_momentum"></span><code>transverse_<wbr>momentum</code> | scalar | ETL⁻¹ |  | <code>[momentum](#momentum)</code> | <code>[momentum](#momentum)</code> | — | [view](../hierarchies/momentum_hep.md) |
| <span id="velocity"></span><code>velocity</code> | vector | LT⁻¹ | kind | <code>[velocity](#velocity)</code> | — | <code>[displacement](#displacement) / <wbr>[duration](#duration)</code> | [view](../hierarchies/velocity.md) |
| <span id="vertex_position"></span><code>vertex_<wbr>position</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="volume"></span><code>volume</code> | scalar | L³ | kind | <code>[volume](#volume)</code> | — | <code>pow<3>([length](#length))</code> | [view](../hierarchies/volume_hep.md) |
| <span id="wavelength"></span><code>wavelength</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |
| <span id="width"></span><code>width</code> | scalar | L | ≥ 0 | <code>[length](#length)</code> | <code>[length](#length)</code> | — | [view](../hierarchies/length_hep.md) |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="ampere"></span><code>ampere</code> | A | — | <code>[coulomb](#coulomb) / <wbr>[second](#second)</code> |
| <span id="angstrom"></span><code>angstrom</code> | Å (A) | — | <code>mag_power<10, -10> * <wbr>[meter](#meter)</code> |
| <span id="astronomical_unit"></span><code>astronomical_<wbr>unit</code> | au | — | <code>mag<149'597'870'700> * <wbr>[meter](#meter)</code> |
| <span id="atmosphere"></span><code>atmosphere</code> | atm | — | <code>mag<101'325> * <wbr>[pascal](#pascal)</code> |
| <span id="bar"></span><code>bar</code> | bar | — | <code>mag_power<10, 5> * <wbr>[pascal](#pascal)</code> |
| <span id="barn"></span><code>barn</code> | b | `b` | <code>mag_power<10, -28> * <wbr>square([meter](#meter))</code> |
| <span id="becquerel"></span><code>becquerel</code> | Bq | `Bq` | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [activity](#activity)</code> |
| <span id="candela"></span><code>candela</code> | cd | — | <code>kind: [luminous_<wbr>intensity](#luminous_intensity)</code> |
| <span id="coulomb"></span><code>coulomb</code> | C | — | <code>mag<6'241'509'074> * <wbr>mag_power<10, 9> * <wbr>[eplus](#eplus)</code> |
| <span id="curie"></span><code>curie</code> | Ci | `Ci` | <code>mag_ratio<37, 10> * <wbr>mag_power<10, 10> * <wbr>[becquerel](#becquerel)</code> |
| <span id="degree"></span><code>degree</code> | — | — | alias to [angular::degree](angular.md#degree) |
| <span id="electronvolt"></span><code>electronvolt</code> | eV | `eV` | <code>kind: [energy](#energy)</code> |
| <span id="eplus"></span><code>eplus</code> | e⁺ (e+) | — | <code>kind: [electric_<wbr>charge](#electric_charge)</code> |
| <span id="farad"></span><code>farad</code> | F | — | <code>[coulomb](#coulomb) / <wbr>[volt](#volt)</code> |
| <span id="fermi"></span><code>fermi</code> | — | — | <code>[si::femto](si.md#femto)<[meter](#meter)></code> |
| <span id="gauss"></span><code>gauss</code> | G | — | <code>mag_power<10, -4> * <wbr>[tesla](#tesla)</code> |
| <span id="gradian"></span><code>gradian</code> | — | — | alias to [angular::gradian](angular.md#gradian) |
| <span id="gram"></span><code>gram</code> | g | `g` | <code>mag_ratio<1, 1000> * <wbr>[joule](#joule) * <wbr>square([second](#second)) / <wbr>square([meter](#meter))</code> |
| <span id="gray"></span><code>gray</code> | Gy | — | <code>[joule](#joule) / <wbr>[si::kilo](si.md#kilo)<[gram](#gram)><br>kind: [absorbed_<wbr>dose](#absorbed_dose)</code> |
| <span id="halfpi"></span><code>halfpi</code> | — | — | <code>mag_ratio<1, 2> * <wbr>[pi](#pi)</code> |
| <span id="henry"></span><code>henry</code> | H | — | <code>[weber](#weber) / <wbr>[ampere](#ampere)</code> |
| <span id="hertz"></span><code>hertz</code> | Hz | — | <code>[one](core.md#one) / <wbr>[second](#second)<br>kind: [frequency](#frequency)</code> |
| <span id="joule"></span><code>joule</code> | J | — | <code>[electronvolt](#electronvolt) * <wbr>[coulomb](#coulomb) / <wbr>[eplus](#eplus)</code> |
| <span id="kelvin"></span><code>kelvin</code> | K | — | <code>kind: [temperature](#temperature)</code> |
| <span id="liter"></span><code>liter</code> | L | `L` | <code>cubic([si::deci](si.md#deci)<[meter](#meter)>)</code> |
| <span id="lumen"></span><code>lumen</code> | lm | — | <code>[candela](#candela) * <wbr>[steradian](#steradian)</code> |
| <span id="lux"></span><code>lux</code> | lx | — | <code>[lumen](#lumen) / <wbr>square([meter](#meter))</code> |
| <span id="meter"></span><code>meter</code> | m | `m` | <code>kind: [length](#length)</code> |
| <span id="mole"></span><code>mole</code> | mol | — | <code>kind: [amount_<wbr>of_<wbr>substance](#amount_of_substance)</code> |
| <span id="newton"></span><code>newton</code> | N | — | <code>[joule](#joule) / <wbr>[meter](#meter)</code> |
| <span id="ohm"></span><code>ohm</code> | Ω (ohm) | — | <code>[volt](#volt) / <wbr>[ampere](#ampere)</code> |
| <span id="parsec"></span><code>parsec</code> | pc | `pc` | <code>[astronomical_<wbr>unit](#astronomical_unit) / <wbr>(mag_ratio<1, 3600> * <wbr>[degree](#degree))</code> |
| <span id="pascal"></span><code>pascal</code> | Pa | — | <code>[newton](#newton) / <wbr>square([meter](#meter))</code> |
| <span id="perCent"></span><code>perCent</code> | — | — | alias to [mp_<wbr>units::percent](core.md#percent) |
| <span id="perMillion"></span><code>perMillion</code> | — | — | alias to [mp_<wbr>units::parts_<wbr>per_<wbr>million](core.md#parts_per_million) |
| <span id="perThousand"></span><code>perThousand</code> | — | — | alias to [mp_<wbr>units::per_<wbr>mille](core.md#per_mille) |
| <span id="radian"></span><code>radian</code> | — | — | alias to [angular::radian](angular.md#radian) |
| <span id="revolution"></span><code>revolution</code> | — | — | alias to [angular::revolution](angular.md#revolution) |
| <span id="second"></span><code>second</code> | s | `s` | <code>kind: [duration](#duration)</code> |
| <span id="steradian"></span><code>steradian</code> | — | — | alias to [angular::steradian](angular.md#steradian) |
| <span id="tesla"></span><code>tesla</code> | T | — | <code>[weber](#weber) / <wbr>square([meter](#meter))</code> |
| <span id="twopi"></span><code>twopi</code> | — | — | <code>mag<2> * <wbr>[pi](#pi)</code> |
| <span id="volt"></span><code>volt</code> | V | — | <code>[electronvolt](#electronvolt) / <wbr>[eplus](#eplus)</code> |
| <span id="watt"></span><code>watt</code> | W | — | <code>[joule](#joule) / <wbr>[second](#second)</code> |
| <span id="weber"></span><code>weber</code> | Wb | — | <code>[volt](#volt) * <wbr>[second](#second)</code> |

## Constants

| Name | Symbol | unit_symbol | Definition |
|------|:------:|:------------:|------------|
| <span id="avogadro_constant"></span><code>avogadro_<wbr>constant</code> | N_A | `N_A` | <code>mag_ratio<602'214'076, 100'000'000> * <wbr>mag_power<10, 23> / <wbr>[mole](#mole)</code> |
| <span id="codata2014-atomic_mass_unit"></span><code>codata2014::atomic_<wbr>mass_<wbr>unit</code> | u | — | <code>mag_ratio<9'314'940'954, 10'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2014-bohr_magneton"></span><code>codata2014::bohr_<wbr>magneton</code> | μ_B (mu_B) | — | <code>mag_ratio<9'274'009'994, 1'000'000'000> * <wbr>mag_power<10, -24> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2014-bohr_radius"></span><code>codata2014::bohr_<wbr>radius</code> | a_0 | — | <code>mag_ratio<52'917'721'067, 10'000'000'000> * <wbr>mag_power<10, -11> * <wbr>[meter](#meter)</code> |
| <span id="codata2014-boltzmann_constant"></span><code>codata2014::boltzmann_<wbr>constant</code> | k_B | — | <code>mag_ratio<86'173'303, 10'000'000> * <wbr>mag_power<10, -11> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>[kelvin](#kelvin)</code> |
| <span id="codata2014-classical_electron_radius"></span><code>codata2014::classical_<wbr>electron_<wbr>radius</code> | r_e | — | <code>mag_ratio<28'179'403'227, 10'000'000'000> * <wbr>mag_power<10, -15> * <wbr>[meter](#meter)</code> |
| <span id="codata2014-electron_compton_wavelength"></span><code>codata2014::electron_<wbr>compton_<wbr>wavelength</code> | λ_C (lambda_C) | — | <code>mag_ratio<24'263'102'367, 10'000'000'000> * <wbr>mag_power<10, -12> * <wbr>[meter](#meter)</code> |
| <span id="codata2014-electron_mass"></span><code>codata2014::electron_<wbr>mass</code> | m_e | — | <code>mag_ratio<5'109'989'461, 10'000'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2014-fine_structure_constant"></span><code>codata2014::fine_<wbr>structure_<wbr>constant</code> | α (alpha) | — | <code>mag_ratio<72'973'525'664, 10'000'000'000> * <wbr>mag_power<10, -3> * <wbr>[one](core.md#one)</code> |
| <span id="codata2014-neutron_mass"></span><code>codata2014::neutron_<wbr>mass</code> | m_n | — | <code>mag_ratio<9'395'654'133, 10'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2014-nuclear_magneton"></span><code>codata2014::nuclear_<wbr>magneton</code> | μ_N (mu_N) | — | <code>mag_ratio<5'050'783'699, 1'000'000'000> * <wbr>mag_power<10, -27> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2014-proton_mass"></span><code>codata2014::proton_<wbr>mass</code> | m_p | — | <code>mag_ratio<9'382'720'813, 10'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2018-atomic_mass_unit"></span><code>codata2018::atomic_<wbr>mass_<wbr>unit</code> | u | `u` | <code>mag_ratio<93'149'410'242, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2018-bohr_magneton"></span><code>codata2018::bohr_<wbr>magneton</code> | μ_B (mu_B) | `mu_B` | <code>mag_ratio<92'740'100'783, 10'000'000'000> * <wbr>mag_power<10, -24> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2018-bohr_radius"></span><code>codata2018::bohr_<wbr>radius</code> | a_0 | `a_0` | <code>mag_ratio<529'177'210'903, 100'000'000'000> * <wbr>mag_power<10, -11> * <wbr>[meter](#meter)</code> |
| <span id="codata2018-boltzmann_constant"></span><code>codata2018::boltzmann_<wbr>constant</code> | k_B | `k_B` | <code>mag_ratio<8'617'333'262, 1'000'000'000> * <wbr>mag_power<10, -11> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>[kelvin](#kelvin)</code> |
| <span id="codata2018-classical_electron_radius"></span><code>codata2018::classical_<wbr>electron_<wbr>radius</code> | r_e | `r_e` | <code>mag_ratio<28'179'403'262, 10'000'000'000> * <wbr>mag_power<10, -15> * <wbr>[meter](#meter)</code> |
| <span id="codata2018-electron_compton_wavelength"></span><code>codata2018::electron_<wbr>compton_<wbr>wavelength</code> | λ_C (lambda_C) | `lambda_C` | <code>mag_ratio<242'631'023'867, 100'000'000'000> * <wbr>mag_power<10, -12> * <wbr>[meter](#meter)</code> |
| <span id="codata2018-electron_mass"></span><code>codata2018::electron_<wbr>mass</code> | m_e | `m_e` | <code>mag_ratio<51'099'895, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2018-fine_structure_constant"></span><code>codata2018::fine_<wbr>structure_<wbr>constant</code> | α (alpha) | `alpha` | <code>mag_ratio<72'973'525'693, 10'000'000'000> * <wbr>mag_power<10, -3> * <wbr>[one](core.md#one)</code> |
| <span id="codata2018-neutron_mass"></span><code>codata2018::neutron_<wbr>mass</code> | m_n | `m_n` | <code>mag_ratio<93'956'542'052, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2018-nuclear_magneton"></span><code>codata2018::nuclear_<wbr>magneton</code> | μ_N (mu_N) | `mu_N` | <code>mag_ratio<50'507'837'461, 10'000'000'000> * <wbr>mag_power<10, -27> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2018-proton_mass"></span><code>codata2018::proton_<wbr>mass</code> | m_p | `m_p` | <code>mag_ratio<938'272'08816, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2022-atomic_mass_unit"></span><code>codata2022::atomic_<wbr>mass_<wbr>unit</code> | u | — | <code>mag_ratio<93'149'410'372, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2022-bohr_magneton"></span><code>codata2022::bohr_<wbr>magneton</code> | μ_B (mu_B) | — | <code>mag_ratio<92'740'100'657, 10'000'000'000> * <wbr>mag_power<10, -24> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2022-bohr_radius"></span><code>codata2022::bohr_<wbr>radius</code> | a_0 | — | <code>mag_ratio<529'177'210'544, 100'000'000'000> * <wbr>mag_power<10, -11> * <wbr>[meter](#meter)</code> |
| <span id="codata2022-boltzmann_constant"></span><code>codata2022::boltzmann_<wbr>constant</code> | — | — | alias to [codata2018::boltzmann_<wbr>constant](#codata2018-boltzmann_constant) |
| <span id="codata2022-classical_electron_radius"></span><code>codata2022::classical_<wbr>electron_<wbr>radius</code> | r_e | — | <code>mag_ratio<28'179'403'205, 10'000'000'000> * <wbr>mag_power<10, -15> * <wbr>[meter](#meter)</code> |
| <span id="codata2022-electron_compton_wavelength"></span><code>codata2022::electron_<wbr>compton_<wbr>wavelength</code> | — | — | alias to [codata2018::electron_<wbr>compton_<wbr>wavelength](#codata2018-electron_compton_wavelength) |
| <span id="codata2022-electron_mass"></span><code>codata2022::electron_<wbr>mass</code> | m_e | — | <code>mag_ratio<51'099'895'069, 100'000'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2022-fine_structure_constant"></span><code>codata2022::fine_<wbr>structure_<wbr>constant</code> | α (alpha) | — | <code>mag_ratio<72'973'525'643, 10'000'000'000> * <wbr>mag_power<10, -3> * <wbr>[one](core.md#one)</code> |
| <span id="codata2022-neutron_mass"></span><code>codata2022::neutron_<wbr>mass</code> | m_n | — | <code>mag_ratio<93'956'542'194, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="codata2022-nuclear_magneton"></span><code>codata2022::nuclear_<wbr>magneton</code> | μ_N (mu_N) | — | <code>mag_ratio<50'507'837'393, 10'000'000'000> * <wbr>mag_power<10, -27> * <wbr>[joule](#joule) / <wbr>[tesla](#tesla)</code> |
| <span id="codata2022-proton_mass"></span><code>codata2022::proton_<wbr>mass</code> | m_p | — | <code>mag_ratio<93'827'208'943, 100'000'000> * <wbr>[si::mega](si.md#mega)<[electronvolt](#electronvolt)> / <wbr>square([speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum](#speed_of_light_in_vacuum))</code> |
| <span id="elementary_charge"></span><code>elementary_<wbr>charge</code> | e | `e` | <code>[eplus](#eplus)</code> |
| <span id="gas_threshold"></span><code>gas_<wbr>threshold</code> | gas_threshold | — | <code>mag<10> * <wbr>[si::milli](si.md#milli)<[gram](#gram)> / <wbr>cubic([si::centi](si.md#centi)<[meter](#meter)>)</code> |
| <span id="permeability_of_vacuum"></span><code>permeability_<wbr>of_<wbr>vacuum</code> | μ₀ (mu_0) | `mu_0` | <code>mag<4> * <wbr>mag_power<10, -7> * <wbr>[π](core.md#π) * <wbr>[henry](#henry) / <wbr>[meter](#meter)</code> |
| <span id="pi"></span><code>pi</code> | — | — | alias to [mp_<wbr>units::pi](#pi) |
| <span id="planck_constant"></span><code>planck_<wbr>constant</code> | h | `h` | <code>mag_ratio<662'607'015, 100'000'000> * <wbr>mag_power<10, -34> * <wbr>[joule](#joule) * <wbr>[second](#second)</code> |
| <span id="speed_of_light_in_vacuum"></span><code>speed_<wbr>of_<wbr>light_<wbr>in_<wbr>vacuum</code> | c | `c` | <code>mag<299'792'458> * <wbr>[meter](#meter) / <wbr>[second](#second)</code> |
| <span id="standard_pressure"></span><code>standard_<wbr>pressure</code> | P_STP | — | <code>[atmosphere](#atmosphere)</code> |
| <span id="standard_temperature"></span><code>standard_<wbr>temperature</code> | T_STP | — | <code>mag_ratio<27'315, 100> * <wbr>[kelvin](#kelvin)</code> |
| <span id="universe_mean_density"></span><code>universe_<wbr>mean_<wbr>density</code> | ρ_universe (rho_universe) | — | <code>mag_power<10, -25> * <wbr>[gram](#gram) / <wbr>cubic([si::centi](si.md#centi)<[meter](#meter)>)</code> |

!!! note "Inline Namespaces"

    The `mp_units::hep::codata2018` namespace is inline in `mp_units::hep`, making its members directly accessible from the parent namespace.
