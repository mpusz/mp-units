<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# USC System

**Namespace:** `mp_units::usc`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/usc.h>`

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="acre"></span>`acre` | acre | — | `mag<10> * square(survey1893::chain)` |
| <span id="barrel"></span>`barrel` | bbl | `bbl` | `mag_ratio<315, 10> * gallon` |
| <span id="bushel"></span>`bushel` | bu | `bu` | `mag_ratio<3'523'907'016'688, 100'000'000'000> * si::litre` |
| <span id="cable"></span>`cable` | cb(us) | `cb` | `mag<120> * yard_pound::fathom` |
| <span id="chain"></span>`chain` | ch | `ch` | `mag<4> * rod` |
| <span id="cup"></span>`cup` | c | `c` | `mag_ratio<1, 2> * pint` |
| <span id="degree_Fahrenheit"></span>`degree_Fahrenheit` | ℉ (\`F) | `deg_F` | `rankine`<br>origin: `zeroth_degree_Fahrenheit` |
| <span id="dry_barrel"></span>`dry_barrel` | bbl | `dry_bbl` | `mag<7056> * cubic(inch)` |
| <span id="dry_gallon"></span>`dry_gallon` | gal | `dry_gal` | `mag_ratio<1, 2> * peck` |
| <span id="dry_pint"></span>`dry_pint` | pt | `dry_pt` | `mag_ratio<1, 2> * dry_quart` |
| <span id="dry_quart"></span>`dry_quart` | qt | `dry_qt` | `mag_ratio<1, 4> * dry_gallon` |
| <span id="fluid_dram"></span>`fluid_dram` | fl dr | `fl_dr` | `mag<60> * minim` |
| <span id="fluid_ounce"></span>`fluid_ounce` | fl oz | `fl_oz` | `mag_ratio<1, 4> * gill` |
| <span id="furlong"></span>`furlong` | fur | `fur` | `mag<10> * chain` |
| <span id="gallon"></span>`gallon` | gal | `gal` | `mag<231> * cubic(inch)` |
| <span id="gill"></span>`gill` | gi | `gi` | `mag_ratio<1, 2> * cup` |
| <span id="hogshead"></span>`hogshead` | hogshead | — | `mag<63> * gallon` |
| <span id="inch_of_mercury"></span>`inch_of_mercury` | inHg | `inHg` | `mag_ratio<3'386'389, 1'000> * si::pascal` |
| <span id="link"></span>`link` | li | `li` | `mag_ratio<33, 50> * foot` |
| <span id="minim"></span>`minim` | min | `min` | `mag_ratio<1, 80> * teaspoon` |
| <span id="oil_barrel"></span>`oil_barrel` | bbl | — | `mag_ratio<4, 3> * barrel` |
| <span id="peck"></span>`peck` | pk | `pk` | `mag_ratio<1, 4> * bushel` |
| <span id="pennyweight"></span>`pennyweight` | dwt | `dwt` | `mag<24> * grain` |
| <span id="pint"></span>`pint` | pt | `pt` | `mag_ratio<1, 2> * quart` |
| <span id="pottle"></span>`pottle` | pot | `pot` | `mag_ratio<1, 2> * gallon` |
| <span id="quart"></span>`quart` | qt | `qt` | `mag_ratio<1, 2> * pottle` |
| <span id="quarter"></span>`quarter` | qr | `qr` | `mag<25> * pound` |
| <span id="rankine"></span>`rankine` | °R (\`R) | — | `mag_ratio<5, 9> * si::kelvin`<br>origin: `zeroth_rankine` |
| <span id="rod"></span>`rod` | rd | `rd` | `mag<25> * link` |
| <span id="section"></span>`section` | section | — | `mag<640> * acre` |
| <span id="short_hundredweight"></span>`short_hundredweight` | cwt | `cwt` | `mag<100> * pound` |
| <span id="short_ton"></span>`short_ton` | — | — | alias to `ton` |
| <span id="shot"></span>`shot` | jig | `jig` | `mag<3> * tablespoon` |
| <span id="tablespoon"></span>`tablespoon` | tbsp | `tbsp` | `mag_ratio<1, 2> * fluid_ounce` |
| <span id="teaspoon"></span>`teaspoon` | tsp | `tsp` | `mag_ratio<1, 3> * tablespoon` |
| <span id="ton"></span>`ton` | t | `t` | `mag<2'000> * pound` |
| <span id="troy_once"></span>`troy_once` | oz t | `oz_t` | `mag<20> * pennyweight` |
| <span id="troy_pound"></span>`troy_pound` | lb t | `lb_t` | `mag<12> * troy_once` |

## Point Origins

| Name | Type | Definition |
|------|:----:|------------|
| <span id="zeroth_degree_Fahrenheit"></span>`zeroth_degree_Fahrenheit` | relative | `::mp_units::point<si::milli<rankine>>(459'670)` |
| <span id="zeroth_rankine"></span>`zeroth_rankine` | — | alias to `si::zeroth_kelvin` |
