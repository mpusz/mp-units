<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# IEC System

**Namespace:** `mp_units::iec`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/iec.h>`

**Secondary Headers:**

- `<mp-units/systems/iec/binary_prefixes.h>`
- `<mp-units/systems/iec/quantities.h>`
- `<mp-units/systems/iec/unit_symbols.h>`
- `<mp-units/systems/iec/units.h>`
- `<mp-units/systems/iec80000.h>`

## Quantities

| Quantity | Character | Dimension | Kind of | Parent | Equation | Hierarchy |
|----------|:---------:|:---------:|:-------:|:------:|----------|:---------:|
| <span id="Hamming_distance"></span>`Hamming_distance` | Real | — | — | — | alias to `isq::Hamming_distance` | — |
| <span id="binary_digit_rate"></span>`binary_digit_rate` | Real | — | — | — | alias to `isq::binary_digit_rate` | — |
| <span id="bit_period"></span>`bit_period` | Real | — | — | — | alias to `isq::period_of_binary_digits` | — |
| <span id="bit_rate"></span>`bit_rate` | Real | — | — | — | alias to `isq::binary_digit_rate` | — |
| <span id="call_intensity"></span>`call_intensity` | Real | — | — | — | alias to `isq::call_intensity` | — |
| <span id="calling_rate"></span>`calling_rate` | Real | — | — | — | alias to `isq::call_intensity` | — |
| <span id="carrier_power"></span>`carrier_power` | Real | — | — | — | alias to `isq::carrier_power` | — |
| <span id="clock_frequency"></span>`clock_frequency` | Real | — | — | — | alias to `isq::clock_frequency` | — |
| <span id="clock_rate"></span>`clock_rate` | Real | — | — | — | alias to `isq::clock_frequency` | — |
| <span id="completed_call_intensity"></span>`completed_call_intensity` | Real | — | — | — | alias to `isq::completed_call_intensity` | — |
| <span id="decision_content"></span>`decision_content` | Real | — | — | — | alias to `isq::decision_content` | — |
| <span id="equivalent_binary_digit_rate"></span>`equivalent_binary_digit_rate` | Real | — | — | — | alias to `isq::equivalent_binary_digit_rate` | — |
| <span id="equivalent_binary_storage_capacity"></span>`equivalent_binary_storage_capacity` | Real | — | — | — | alias to `isq::equivalent_binary_storage_capacity` | — |
| <span id="equivalent_bit_rate"></span>`equivalent_bit_rate` | Real | — | — | — | alias to `isq::bit_rate` | — |
| <span id="error_probability"></span>`error_probability` | Real | — | — | — | alias to `isq::error_probability` | — |
| <span id="line_digit_rate"></span>`line_digit_rate` | Real | — | — | — | alias to `isq::modulation_rate` | — |
| <span id="loss_probability"></span>`loss_probability` | Real | — | — | — | alias to `isq::loss_probability` | — |
| <span id="mean_queue_length"></span>`mean_queue_length` | Real | — | — | — | alias to `isq::mean_queue_length` | — |
| <span id="modulation_rate"></span>`modulation_rate` | Real | — | — | — | alias to `isq::modulation_rate` | — |
| <span id="period_of_binary_digits"></span>`period_of_binary_digits` | Real | — | — | — | alias to `isq::period_of_binary_digits` | — |
| <span id="period_of_data_elements"></span>`period_of_data_elements` | Real | — | — | — | alias to `isq::period_of_data_elements` | — |
| <span id="quantizing_distortion_power"></span>`quantizing_distortion_power` | Real | — | — | — | alias to `isq::quantizing_distortion_power` | — |
| <span id="signal_energy_per_binary_digit"></span>`signal_energy_per_binary_digit` | Real | — | — | — | alias to `isq::signal_energy_per_binary_digit` | — |
| <span id="storage_capacity"></span>`storage_capacity` | Real | — | — | — | alias to `isq::storage_capacity` | — |
| <span id="storage_size"></span>`storage_size` | Real | — | — | — | alias to `isq::storage_capacity` | — |
| <span id="traffic_carried_intensity"></span>`traffic_carried_intensity` | Real | — | — | — | alias to `isq::traffic_carried_intensity` | — |
| <span id="traffic_intensity"></span>`traffic_intensity` | Real | — | — | — | alias to `isq::traffic_intensity` | — |
| <span id="traffic_load"></span>`traffic_load` | Real | — | — | — | alias to `isq::traffic_carried_intensity` | — |
| <span id="traffic_offered_intensity"></span>`traffic_offered_intensity` | Real | — | — | — | alias to `isq::traffic_offered_intensity` | — |
| <span id="transfer_rate"></span>`transfer_rate` | Real | — | — | — | alias to `isq::transfer_rate` | — |
| <span id="waiting_probability"></span>`waiting_probability` | Real | — | — | — | alias to `isq::waiting_probability` | — |

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="baud"></span>`baud` | Bd | `Bd` | `one / si::second`<br>kind: `isq::modulation_rate` |
| <span id="bit"></span>`bit` | bit | `bit` | `one`<br>kind: `isq::storage_capacity` |
| <span id="byte"></span>`byte` | B | `B` | `mag<8> * bit` |
| <span id="erlang"></span>`erlang` | E | `E` | kind: `isq::traffic_intensity` |
| <span id="octet"></span>`octet` | o | `o` | `mag<8> * bit` |
| <span id="volt_ampere_reactive_power"></span>`volt_ampere_reactive_power` | var | `var` | `si::volt * si::ampere`<br>kind: `isq::reactive_power` |

## Prefixes

| Name | Symbol | Definition |
|------|:------:|------------|
| <span id="kibi"></span>`kibi` | Ki | `mag_power<2, 10>` |
| <span id="mebi"></span>`mebi` | Mi | `mag_power<2, 20>` |
| <span id="gibi"></span>`gibi` | Gi | `mag_power<2, 30>` |
| <span id="tebi"></span>`tebi` | Ti | `mag_power<2, 40>` |
| <span id="pebi"></span>`pebi` | Pi | `mag_power<2, 50>` |
| <span id="exbi"></span>`exbi` | Ei | `mag_power<2, 60>` |
| <span id="zebi"></span>`zebi` | Zi | `mag_power<2, 70>` |
| <span id="yobi"></span>`yobi` | Yi | `mag_power<2, 80>` |
