<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# dimensionless Hierarchy

**Systems:** Core, ISQ, Natural

**Dimension:** dimensionless

```mermaid
flowchart LR
    dimensionless["<b><a href="../../systems/core/#dimensionless" style="color: black; text-decoration: none;">dimensionless</a></b>"]
    isq_Hamming_distance["<b><a href="../../systems/isq/#Hamming_distance" style="color: black; text-decoration: none;">isq::Hamming_distance</a></b>"]
    dimensionless --- isq_Hamming_distance
    isq_Poisson_number["<b><a href="../../systems/isq/#Poisson_number" style="color: black; text-decoration: none;">isq::Poisson_number</a></b><br><i>(<a href="../../systems/isq/#width" style="color: black; text-decoration: none;">width</a> / <a href="../../systems/isq/#length" style="color: black; text-decoration: none;">length</a>)</i>"]
    dimensionless --- isq_Poisson_number
    isq_angular_measure["<b><a href="../../systems/isq/#angular_measure" style="color: black; text-decoration: none;">isq::angular_measure</a></b><br><i>(<a href="../../systems/isq/#arc_length" style="color: black; text-decoration: none;">arc_length</a> / <a href="../../systems/isq/#radius" style="color: black; text-decoration: none;">radius</a>)</i>"]
    dimensionless --- isq_angular_measure
    isq_loss_angle["<b><a href="../../systems/isq/#loss_angle" style="color: black; text-decoration: none;">isq::loss_angle</a></b>"]
    isq_angular_measure --- isq_loss_angle
    isq_phase_angle["<b><a href="../../systems/isq/#phase_angle" style="color: black; text-decoration: none;">isq::phase_angle</a></b>"]
    isq_angular_measure --- isq_phase_angle
    isq_phase_difference["<b><a href="../../systems/isq/#phase_difference" style="color: black; text-decoration: none;">isq::phase_difference</a></b>"]
    isq_phase_angle --- isq_phase_difference
    isq_rotational_displacement["<b><a href="../../systems/isq/#rotational_displacement" style="color: black; text-decoration: none;">isq::rotational_displacement</a> | <a href="../../systems/isq/#angular_displacement" style="color: black; text-decoration: none;">isq::angular_displacement</a></b><br><i>(<a href="../../systems/isq/#path_length" style="color: black; text-decoration: none;">path_length</a> / <a href="../../systems/isq/#radius" style="color: black; text-decoration: none;">radius</a>)</i>"]
    isq_angular_measure --- isq_rotational_displacement
    isq_coupling_factor["<b><a href="../../systems/isq/#coupling_factor" style="color: black; text-decoration: none;">isq::coupling_factor</a></b><br><i>(<a href="../../systems/isq/#mutual_inductance" style="color: black; text-decoration: none;">mutual_inductance</a> / pow<1, 2>(pow<2>(<a href="../../systems/isq/#self_inductance" style="color: black; text-decoration: none;">self_inductance</a>)))</i>"]
    dimensionless --- isq_coupling_factor
    isq_decision_content["<b><a href="../../systems/isq/#decision_content" style="color: black; text-decoration: none;">isq::decision_content</a></b>"]
    dimensionless --- isq_decision_content
    isq_drag_coefficient["<b><a href="../../systems/isq/#drag_coefficient" style="color: black; text-decoration: none;">isq::drag_coefficient</a> | <a href="../../systems/isq/#drag_factor" style="color: black; text-decoration: none;">isq::drag_factor</a></b><br><i>(<a href="../../systems/isq/#drag_force" style="color: black; text-decoration: none;">drag_force</a> / (<a href="../../systems/isq/#mass_density" style="color: black; text-decoration: none;">mass_density</a> * pow<2>(<a href="../../systems/isq/#speed" style="color: black; text-decoration: none;">speed</a>) * <a href="../../systems/isq/#area" style="color: black; text-decoration: none;">area</a>))</i>"]
    dimensionless --- isq_drag_coefficient
    isq_electric_susceptibility["<b><a href="../../systems/isq/#electric_susceptibility" style="color: black; text-decoration: none;">isq::electric_susceptibility</a></b><br><i>(<a href="../../systems/isq/#electric_polarization" style="color: black; text-decoration: none;">electric_polarization</a> / <a href="../../systems/isq/#electric_constant" style="color: black; text-decoration: none;">electric_constant</a> / <a href="../../systems/isq/#electric_field_strength" style="color: black; text-decoration: none;">electric_field_strength</a>)</i>"]
    dimensionless --- isq_electric_susceptibility
    isq_error_probability["<b><a href="../../systems/isq/#error_probability" style="color: black; text-decoration: none;">isq::error_probability</a></b>"]
    dimensionless --- isq_error_probability
    isq_ionizing_radiation_quality_factor["<b><a href="../../systems/isq/#ionizing_radiation_quality_factor" style="color: black; text-decoration: none;">isq::ionizing_radiation_quality_factor</a></b>"]
    dimensionless --- isq_ionizing_radiation_quality_factor
    isq_isentropic_exponent["<b><a href="../../systems/isq/#isentropic_exponent" style="color: black; text-decoration: none;">isq::isentropic_exponent</a> | <a href="../../systems/isq/#isentropic_expansion_factor" style="color: black; text-decoration: none;">isq::isentropic_expansion_factor</a></b><br><i>(<a href="../../systems/isq/#volume" style="color: black; text-decoration: none;">volume</a> / <a href="../../systems/isq/#pressure" style="color: black; text-decoration: none;">pressure</a> * (<a href="../../systems/isq/#pressure" style="color: black; text-decoration: none;">pressure</a> / <a href="../../systems/isq/#volume" style="color: black; text-decoration: none;">volume</a>))</i>"]
    dimensionless --- isq_isentropic_exponent
    isq_kinetic_friction_factor["<b><a href="../../systems/isq/#kinetic_friction_factor" style="color: black; text-decoration: none;">isq::kinetic_friction_factor</a> | <a href="../../systems/isq/#dynamic_friction_factor" style="color: black; text-decoration: none;">isq::dynamic_friction_factor</a></b><br><i>(<a href="../../systems/isq/#kinetic_friction_force" style="color: black; text-decoration: none;">kinetic_friction_force</a> / <a href="../../systems/isq/#force" style="color: black; text-decoration: none;">force</a>)</i>"]
    dimensionless --- isq_kinetic_friction_factor
    isq_leakage_factor["<b><a href="../../systems/isq/#leakage_factor" style="color: black; text-decoration: none;">isq::leakage_factor</a></b><br><i>(pow<2>(<a href="../../systems/isq/#coupling_factor" style="color: black; text-decoration: none;">coupling_factor</a>))</i>"]
    dimensionless --- isq_leakage_factor
    isq_logarithmic_decrement["<b><a href="../../systems/isq/#logarithmic_decrement" style="color: black; text-decoration: none;">isq::logarithmic_decrement</a></b><br><i>(<a href="../../systems/isq/#damping_coefficient" style="color: black; text-decoration: none;">damping_coefficient</a> * <a href="../../systems/isq/#period_duration" style="color: black; text-decoration: none;">period_duration</a>)</i>"]
    dimensionless --- isq_logarithmic_decrement
    isq_loss_factor["<b><a href="../../systems/isq/#loss_factor" style="color: black; text-decoration: none;">isq::loss_factor</a></b><br><i>(inverse(<a href="../../systems/isq/#quality_factor" style="color: black; text-decoration: none;">quality_factor</a>))</i>"]
    dimensionless --- isq_loss_factor
    isq_loss_probability["<b><a href="../../systems/isq/#loss_probability" style="color: black; text-decoration: none;">isq::loss_probability</a></b>"]
    dimensionless --- isq_loss_probability
    isq_magnetic_susceptibility["<b><a href="../../systems/isq/#magnetic_susceptibility" style="color: black; text-decoration: none;">isq::magnetic_susceptibility</a></b><br><i>(<a href="../../systems/isq/#magnetization" style="color: black; text-decoration: none;">magnetization</a> / <a href="../../systems/isq/#magnetic_field_strength" style="color: black; text-decoration: none;">magnetic_field_strength</a>)</i>"]
    dimensionless --- isq_magnetic_susceptibility
    isq_mass_fraction_of_water["<b><a href="../../systems/isq/#mass_fraction_of_water" style="color: black; text-decoration: none;">isq::mass_fraction_of_water</a></b><br><i>(<a href="../../systems/isq/#mass_ratio_of_water_to_dry_matter" style="color: black; text-decoration: none;">mass_ratio_of_water_to_dry_matter</a> / <a href="../../systems/isq/#mass_ratio_of_water_to_dry_matter" style="color: black; text-decoration: none;">mass_ratio_of_water_to_dry_matter</a>)</i>"]
    dimensionless --- isq_mass_fraction_of_water
    isq_mass_fraction_of_dry_matter["<b><a href="../../systems/isq/#mass_fraction_of_dry_matter" style="color: black; text-decoration: none;">isq::mass_fraction_of_dry_matter</a></b>"]
    isq_mass_fraction_of_water --- isq_mass_fraction_of_dry_matter
    isq_mass_ratio_of_water_to_dry_matter["<b><a href="../../systems/isq/#mass_ratio_of_water_to_dry_matter" style="color: black; text-decoration: none;">isq::mass_ratio_of_water_to_dry_matter</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: black; text-decoration: none;">mass</a> / <a href="../../systems/isq/#mass" style="color: black; text-decoration: none;">mass</a>)</i>"]
    dimensionless --- isq_mass_ratio_of_water_to_dry_matter
    isq_mass_ratio_of_water_vapour_to_dry_gas["<b><a href="../../systems/isq/#mass_ratio_of_water_vapour_to_dry_gas" style="color: black; text-decoration: none;">isq::mass_ratio_of_water_vapour_to_dry_gas</a></b><br><i>(<a href="../../systems/isq/#mass" style="color: black; text-decoration: none;">mass</a> / <a href="../../systems/isq/#mass" style="color: black; text-decoration: none;">mass</a>)</i>"]
    dimensionless --- isq_mass_ratio_of_water_vapour_to_dry_gas
    isq_maximum_efficiency["<b><a href="../../systems/isq/#maximum_efficiency" style="color: black; text-decoration: none;">isq::maximum_efficiency</a></b><br><i>(<a href="../../systems/isq/#thermodynamic_temperature" style="color: black; text-decoration: none;">thermodynamic_temperature</a> / <a href="../../systems/isq/#thermodynamic_temperature" style="color: black; text-decoration: none;">thermodynamic_temperature</a>)</i>"]
    dimensionless --- isq_maximum_efficiency
    isq_mean_queue_length["<b><a href="../../systems/isq/#mean_queue_length" style="color: black; text-decoration: none;">isq::mean_queue_length</a></b>"]
    dimensionless --- isq_mean_queue_length
    isq_mechanical_efficiency["<b><a href="../../systems/isq/#mechanical_efficiency" style="color: black; text-decoration: none;">isq::mechanical_efficiency</a></b><br><i>(<a href="../../systems/isq/#mechanical_power" style="color: black; text-decoration: none;">mechanical_power</a> / <a href="../../systems/isq/#mechanical_power" style="color: black; text-decoration: none;">mechanical_power</a>)</i>"]
    dimensionless --- isq_mechanical_efficiency
    isq_number_of_turns_in_a_winding["<b><a href="../../systems/isq/#number_of_turns_in_a_winding" style="color: black; text-decoration: none;">isq::number_of_turns_in_a_winding</a></b>"]
    dimensionless --- isq_number_of_turns_in_a_winding
    isq_power_factor["<b><a href="../../systems/isq/#power_factor" style="color: black; text-decoration: none;">isq::power_factor</a></b><br><i>(<a href="../../systems/isq/#active_power" style="color: black; text-decoration: none;">active_power</a> / <a href="../../systems/isq/#apparent_power" style="color: black; text-decoration: none;">apparent_power</a>)</i>"]
    dimensionless --- isq_power_factor
    isq_quality_factor["<b><a href="../../systems/isq/#quality_factor" style="color: black; text-decoration: none;">isq::quality_factor</a></b><br><i>(<a href="../../systems/isq/#reactance" style="color: black; text-decoration: none;">reactance</a> / <a href="../../systems/isq/#resistance" style="color: black; text-decoration: none;">resistance</a>)</i>"]
    dimensionless --- isq_quality_factor
    isq_ratio_of_specific_heat_capacities["<b><a href="../../systems/isq/#ratio_of_specific_heat_capacities" style="color: black; text-decoration: none;">isq::ratio_of_specific_heat_capacities</a></b><br><i>(<a href="../../systems/isq/#specific_heat_capacity_at_constant_pressure" style="color: black; text-decoration: none;">specific_heat_capacity_at_constant_pressure</a> / <a href="../../systems/isq/#specific_heat_capacity_at_constant_volume" style="color: black; text-decoration: none;">specific_heat_capacity_at_constant_volume</a>)</i>"]
    dimensionless --- isq_ratio_of_specific_heat_capacities
    isq_refractive_index["<b><a href="../../systems/isq/#refractive_index" style="color: black; text-decoration: none;">isq::refractive_index</a></b><br><i>(<a href="../../systems/isq/#speed_of_light_in_vacuum" style="color: black; text-decoration: none;">speed_of_light_in_vacuum</a> / <a href="../../systems/isq/#speed_of_light_in_a_medium" style="color: black; text-decoration: none;">speed_of_light_in_a_medium</a>)</i>"]
    dimensionless --- isq_refractive_index
    isq_relative_humidity["<b><a href="../../systems/isq/#relative_humidity" style="color: black; text-decoration: none;">isq::relative_humidity</a></b><br><i>(<a href="../../systems/isq/#pressure" style="color: black; text-decoration: none;">pressure</a> / <a href="../../systems/isq/#pressure" style="color: black; text-decoration: none;">pressure</a>)</i>"]
    dimensionless --- isq_relative_humidity
    isq_relative_linear_strain["<b><a href="../../systems/isq/#relative_linear_strain" style="color: black; text-decoration: none;">isq::relative_linear_strain</a></b><br><i>(<a href="../../systems/isq/#length" style="color: black; text-decoration: none;">length</a> / <a href="../../systems/isq/#length" style="color: black; text-decoration: none;">length</a>)</i>"]
    dimensionless --- isq_relative_linear_strain
    isq_relative_mass_concentration_of_vapour["<b><a href="../../systems/isq/#relative_mass_concentration_of_vapour" style="color: black; text-decoration: none;">isq::relative_mass_concentration_of_vapour</a></b><br><i>(<a href="../../systems/isq/#mass_concentration_of_water_vapour" style="color: black; text-decoration: none;">mass_concentration_of_water_vapour</a> / <a href="../../systems/isq/#mass_concentration_of_water_vapour" style="color: black; text-decoration: none;">mass_concentration_of_water_vapour</a>)</i>"]
    dimensionless --- isq_relative_mass_concentration_of_vapour
    isq_relative_mass_density["<b><a href="../../systems/isq/#relative_mass_density" style="color: black; text-decoration: none;">isq::relative_mass_density</a> | <a href="../../systems/isq/#relative_density" style="color: black; text-decoration: none;">isq::relative_density</a></b><br><i>(<a href="../../systems/isq/#mass_density" style="color: black; text-decoration: none;">mass_density</a> / <a href="../../systems/isq/#mass_density" style="color: black; text-decoration: none;">mass_density</a>)</i>"]
    dimensionless --- isq_relative_mass_density
    isq_relative_mass_ratio_of_vapour["<b><a href="../../systems/isq/#relative_mass_ratio_of_vapour" style="color: black; text-decoration: none;">isq::relative_mass_ratio_of_vapour</a></b><br><i>(<a href="../../systems/isq/#mass_ratio_of_water_vapour_to_dry_gas" style="color: black; text-decoration: none;">mass_ratio_of_water_vapour_to_dry_gas</a> / <a href="../../systems/isq/#mass_ratio_of_water_vapour_to_dry_gas" style="color: black; text-decoration: none;">mass_ratio_of_water_vapour_to_dry_gas</a>)</i>"]
    dimensionless --- isq_relative_mass_ratio_of_vapour
    isq_relative_permeability["<b><a href="../../systems/isq/#relative_permeability" style="color: black; text-decoration: none;">isq::relative_permeability</a></b><br><i>(<a href="../../systems/isq/#permeability" style="color: black; text-decoration: none;">permeability</a> / <a href="../../systems/isq/#magnetic_constant" style="color: black; text-decoration: none;">magnetic_constant</a>)</i>"]
    dimensionless --- isq_relative_permeability
    isq_relative_permittivity["<b><a href="../../systems/isq/#relative_permittivity" style="color: black; text-decoration: none;">isq::relative_permittivity</a></b><br><i>(<a href="../../systems/isq/#permittivity" style="color: black; text-decoration: none;">permittivity</a> / <a href="../../systems/isq/#electric_constant" style="color: black; text-decoration: none;">electric_constant</a>)</i>"]
    dimensionless --- isq_relative_permittivity
    isq_relative_volume_strain["<b><a href="../../systems/isq/#relative_volume_strain" style="color: black; text-decoration: none;">isq::relative_volume_strain</a></b><br><i>(<a href="../../systems/isq/#volume" style="color: black; text-decoration: none;">volume</a> / <a href="../../systems/isq/#volume" style="color: black; text-decoration: none;">volume</a>)</i>"]
    dimensionless --- isq_relative_volume_strain
    isq_rolling_resistance_factor["<b><a href="../../systems/isq/#rolling_resistance_factor" style="color: black; text-decoration: none;">isq::rolling_resistance_factor</a></b><br><i>(<a href="../../systems/isq/#force" style="color: black; text-decoration: none;">force</a> / <a href="../../systems/isq/#force" style="color: black; text-decoration: none;">force</a>)</i>"]
    dimensionless --- isq_rolling_resistance_factor
    isq_rotation["<b><a href="../../systems/isq/#rotation" style="color: black; text-decoration: none;">isq::rotation</a></b>"]
    dimensionless --- isq_rotation
    isq_shear_strain["<b><a href="../../systems/isq/#shear_strain" style="color: black; text-decoration: none;">isq::shear_strain</a></b><br><i>(<a href="../../systems/isq/#displacement" style="color: black; text-decoration: none;">displacement</a> / <a href="../../systems/isq/#thickness" style="color: black; text-decoration: none;">thickness</a>)</i>"]
    dimensionless --- isq_shear_strain
    isq_solid_angular_measure["<b><a href="../../systems/isq/#solid_angular_measure" style="color: black; text-decoration: none;">isq::solid_angular_measure</a></b><br><i>(<a href="../../systems/isq/#area" style="color: black; text-decoration: none;">area</a> / pow<2>(<a href="../../systems/isq/#radius" style="color: black; text-decoration: none;">radius</a>))</i>"]
    dimensionless --- isq_solid_angular_measure
    isq_static_friction_coefficient["<b><a href="../../systems/isq/#static_friction_coefficient" style="color: black; text-decoration: none;">isq::static_friction_coefficient</a> | <a href="../../systems/isq/#coefficient_of_static_friction" style="color: black; text-decoration: none;">isq::coefficient_of_static_friction</a> | <a href="../../systems/isq/#static_friction_factor" style="color: black; text-decoration: none;">isq::static_friction_factor</a></b><br><i>(<a href="../../systems/isq/#static_friction_force" style="color: black; text-decoration: none;">static_friction_force</a> / <a href="../../systems/isq/#force" style="color: black; text-decoration: none;">force</a>)</i>"]
    dimensionless --- isq_static_friction_coefficient
    isq_storage_capacity["<b><a href="../../systems/isq/#storage_capacity" style="color: black; text-decoration: none;">isq::storage_capacity</a> | <a href="../../systems/isq/#storage_size" style="color: black; text-decoration: none;">isq::storage_size</a></b>"]
    dimensionless --- isq_storage_capacity
    isq_equivalent_binary_storage_capacity["<b><a href="../../systems/isq/#equivalent_binary_storage_capacity" style="color: black; text-decoration: none;">isq::equivalent_binary_storage_capacity</a></b>"]
    isq_storage_capacity --- isq_equivalent_binary_storage_capacity
    isq_strain["<b><a href="../../systems/isq/#strain" style="color: black; text-decoration: none;">isq::strain</a></b>"]
    dimensionless --- isq_strain
    isq_thermodynamic_efficiency["<b><a href="../../systems/isq/#thermodynamic_efficiency" style="color: black; text-decoration: none;">isq::thermodynamic_efficiency</a></b><br><i>(<a href="../../systems/isq/#work" style="color: black; text-decoration: none;">work</a> / <a href="../../systems/isq/#heat" style="color: black; text-decoration: none;">heat</a>)</i>"]
    dimensionless --- isq_thermodynamic_efficiency
    isq_waiting_probability["<b><a href="../../systems/isq/#waiting_probability" style="color: black; text-decoration: none;">isq::waiting_probability</a></b>"]
    dimensionless --- isq_waiting_probability
    natural_angular_measure["<b><a href="../../systems/natural/#angular_measure" style="color: black; text-decoration: none;">natural::angular_measure</a></b>"]
    dimensionless --- natural_angular_measure
    natural_speed["<b><a href="../../systems/natural/#speed" style="color: black; text-decoration: none;">natural::speed</a></b>"]
    dimensionless --- natural_speed
    natural_velocity["<b><a href="../../systems/natural/#velocity" style="color: black; text-decoration: none;">natural::velocity</a></b>"]
    natural_speed --- natural_velocity
```
