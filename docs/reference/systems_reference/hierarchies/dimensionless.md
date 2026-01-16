<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# dimensionless Hierarchy

**Systems:** Core, ISQ, Natural

**Dimension:** dimensionless

```mermaid
flowchart LR
    dimensionless["<b>dimensionless</b>"]
    isq_Hamming_distance["<b>isq::Hamming_distance</b>"]
    dimensionless --- isq_Hamming_distance
    isq_Poisson_number["<b>isq::Poisson_number</b><br><i>(width / length)</i>"]
    dimensionless --- isq_Poisson_number
    isq_angular_measure["<b>isq::angular_measure</b><br><i>(arc_length / radius)</i>"]
    dimensionless --- isq_angular_measure
    isq_loss_angle["<b>isq::loss_angle</b>"]
    isq_angular_measure --- isq_loss_angle
    isq_phase_angle["<b>isq::phase_angle</b>"]
    isq_angular_measure --- isq_phase_angle
    isq_phase_difference["<b>isq::phase_difference</b>"]
    isq_phase_angle --- isq_phase_difference
    isq_rotational_displacement["<b>isq::rotational_displacement | isq::angular_displacement</b><br><i>(path_length / radius)</i>"]
    isq_angular_measure --- isq_rotational_displacement
    isq_coupling_factor["<b>isq::coupling_factor</b><br><i>(mutual_inductance / pow<1, 2>(pow<2>(self_inductance)))</i>"]
    dimensionless --- isq_coupling_factor
    isq_decision_content["<b>isq::decision_content</b>"]
    dimensionless --- isq_decision_content
    isq_drag_coefficient["<b>isq::drag_coefficient | isq::drag_factor</b><br><i>(drag_force / (mass_density * pow<2>(speed) * area))</i>"]
    dimensionless --- isq_drag_coefficient
    isq_electric_susceptibility["<b>isq::electric_susceptibility</b><br><i>(electric_polarization / electric_constant / electric_field_strength)</i>"]
    dimensionless --- isq_electric_susceptibility
    isq_error_probability["<b>isq::error_probability</b>"]
    dimensionless --- isq_error_probability
    isq_ionizing_radiation_quality_factor["<b>isq::ionizing_radiation_quality_factor</b>"]
    dimensionless --- isq_ionizing_radiation_quality_factor
    isq_isentropic_exponent["<b>isq::isentropic_exponent | isq::isentropic_expansion_factor</b><br><i>(volume / pressure * (pressure / volume))</i>"]
    dimensionless --- isq_isentropic_exponent
    isq_kinetic_friction_factor["<b>isq::kinetic_friction_factor | isq::dynamic_friction_factor</b><br><i>(kinetic_friction_force / force)</i>"]
    dimensionless --- isq_kinetic_friction_factor
    isq_leakage_factor["<b>isq::leakage_factor</b><br><i>(pow<2>(coupling_factor))</i>"]
    dimensionless --- isq_leakage_factor
    isq_logarithmic_decrement["<b>isq::logarithmic_decrement</b><br><i>(damping_coefficient * period_duration)</i>"]
    dimensionless --- isq_logarithmic_decrement
    isq_loss_factor["<b>isq::loss_factor</b><br><i>(inverse(quality_factor))</i>"]
    dimensionless --- isq_loss_factor
    isq_loss_probability["<b>isq::loss_probability</b>"]
    dimensionless --- isq_loss_probability
    isq_magnetic_susceptibility["<b>isq::magnetic_susceptibility</b><br><i>(magnetization / magnetic_field_strength)</i>"]
    dimensionless --- isq_magnetic_susceptibility
    isq_mass_fraction_of_water["<b>isq::mass_fraction_of_water</b><br><i>(mass_ratio_of_water_to_dry_matter / mass_ratio_of_water_to_dry_matter)</i>"]
    dimensionless --- isq_mass_fraction_of_water
    isq_mass_fraction_of_dry_matter["<b>isq::mass_fraction_of_dry_matter</b>"]
    isq_mass_fraction_of_water --- isq_mass_fraction_of_dry_matter
    isq_mass_ratio_of_water_to_dry_matter["<b>isq::mass_ratio_of_water_to_dry_matter</b><br><i>(mass / mass)</i>"]
    dimensionless --- isq_mass_ratio_of_water_to_dry_matter
    isq_mass_ratio_of_water_vapour_to_dry_gas["<b>isq::mass_ratio_of_water_vapour_to_dry_gas</b><br><i>(mass / mass)</i>"]
    dimensionless --- isq_mass_ratio_of_water_vapour_to_dry_gas
    isq_maximum_efficiency["<b>isq::maximum_efficiency</b><br><i>(thermodynamic_temperature / thermodynamic_temperature)</i>"]
    dimensionless --- isq_maximum_efficiency
    isq_mean_queue_length["<b>isq::mean_queue_length</b>"]
    dimensionless --- isq_mean_queue_length
    isq_mechanical_efficiency["<b>isq::mechanical_efficiency</b><br><i>(mechanical_power / mechanical_power)</i>"]
    dimensionless --- isq_mechanical_efficiency
    isq_number_of_turns_in_a_winding["<b>isq::number_of_turns_in_a_winding</b>"]
    dimensionless --- isq_number_of_turns_in_a_winding
    isq_power_factor["<b>isq::power_factor</b><br><i>(active_power / apparent_power)</i>"]
    dimensionless --- isq_power_factor
    isq_quality_factor["<b>isq::quality_factor</b><br><i>(reactance / resistance)</i>"]
    dimensionless --- isq_quality_factor
    isq_ratio_of_specific_heat_capacities["<b>isq::ratio_of_specific_heat_capacities</b><br><i>(specific_heat_capacity_at_constant_pressure / specific_heat_capacity_at_constant_volume)</i>"]
    dimensionless --- isq_ratio_of_specific_heat_capacities
    isq_refractive_index["<b>isq::refractive_index</b><br><i>(speed_of_light_in_vacuum / speed_of_light_in_a_medium)</i>"]
    dimensionless --- isq_refractive_index
    isq_relative_humidity["<b>isq::relative_humidity</b><br><i>(pressure / pressure)</i>"]
    dimensionless --- isq_relative_humidity
    isq_relative_linear_strain["<b>isq::relative_linear_strain</b><br><i>(length / length)</i>"]
    dimensionless --- isq_relative_linear_strain
    isq_relative_mass_concentration_of_vapour["<b>isq::relative_mass_concentration_of_vapour</b><br><i>(mass_concentration_of_water_vapour / mass_concentration_of_water_vapour)</i>"]
    dimensionless --- isq_relative_mass_concentration_of_vapour
    isq_relative_mass_density["<b>isq::relative_mass_density | isq::relative_density</b><br><i>(mass_density / mass_density)</i>"]
    dimensionless --- isq_relative_mass_density
    isq_relative_mass_ratio_of_vapour["<b>isq::relative_mass_ratio_of_vapour</b><br><i>(mass_ratio_of_water_vapour_to_dry_gas / mass_ratio_of_water_vapour_to_dry_gas)</i>"]
    dimensionless --- isq_relative_mass_ratio_of_vapour
    isq_relative_permeability["<b>isq::relative_permeability</b><br><i>(permeability / magnetic_constant)</i>"]
    dimensionless --- isq_relative_permeability
    isq_relative_permittivity["<b>isq::relative_permittivity</b><br><i>(permittivity / electric_constant)</i>"]
    dimensionless --- isq_relative_permittivity
    isq_relative_volume_strain["<b>isq::relative_volume_strain</b><br><i>(volume / volume)</i>"]
    dimensionless --- isq_relative_volume_strain
    isq_rolling_resistance_factor["<b>isq::rolling_resistance_factor</b><br><i>(force / force)</i>"]
    dimensionless --- isq_rolling_resistance_factor
    isq_rotation["<b>isq::rotation</b>"]
    dimensionless --- isq_rotation
    isq_shear_strain["<b>isq::shear_strain</b><br><i>(displacement / thickness)</i>"]
    dimensionless --- isq_shear_strain
    isq_solid_angular_measure["<b>isq::solid_angular_measure</b><br><i>(area / pow<2>(radius))</i>"]
    dimensionless --- isq_solid_angular_measure
    isq_static_friction_coefficient["<b>isq::static_friction_coefficient | isq::coefficient_of_static_friction | isq::static_friction_factor</b><br><i>(static_friction_force / force)</i>"]
    dimensionless --- isq_static_friction_coefficient
    isq_storage_capacity["<b>isq::storage_capacity | isq::storage_size</b>"]
    dimensionless --- isq_storage_capacity
    isq_equivalent_binary_storage_capacity["<b>isq::equivalent_binary_storage_capacity</b>"]
    isq_storage_capacity --- isq_equivalent_binary_storage_capacity
    isq_strain["<b>isq::strain</b>"]
    dimensionless --- isq_strain
    isq_thermodynamic_efficiency["<b>isq::thermodynamic_efficiency</b><br><i>(work / heat)</i>"]
    dimensionless --- isq_thermodynamic_efficiency
    isq_waiting_probability["<b>isq::waiting_probability</b>"]
    dimensionless --- isq_waiting_probability
    natural_angular_measure["<b>natural::angular_measure</b>"]
    dimensionless --- natural_angular_measure
    natural_speed["<b>natural::speed</b>"]
    dimensionless --- natural_speed
    natural_velocity["<b>natural::velocity</b>"]
    natural_speed --- natural_velocity
```
