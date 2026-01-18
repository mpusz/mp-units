<!-- This file is auto-generated. Do not edit manually. -->
<!-- Run: python3 scripts/systems_reference.py --force -->

# USC System

**Namespace:** `mp_units::usc`

**Module:** `mp_units.systems`

**Header:** `<mp-units/systems/usc.h>`

## Units

| Unit Name | Symbol | unit_symbol | Definition |
|-----------|:------:|:-----------:|------------|
| <span id="acre"></span><code>acre</code> | acre | `acre` | <code>mag<10> * <wbr>square([survey1893::chain](usc.md#chain))</code> |
| <span id="barrel"></span><code>barrel</code> | bbl | `bbl` | <code>mag_ratio<315, 10> * <wbr>[gallon](#gallon)</code> |
| <span id="bushel"></span><code>bushel</code> | bu | `bu` | <code>mag_ratio<3'523'907'016'688, 100'000'000'000> * <wbr>[si::litre](si.md#litre)</code> |
| <span id="cable"></span><code>cable</code> | cb(us) | `cb` | <code>mag<120> * <wbr>[yard_<wbr>pound::fathom](yard_pound.md#fathom)</code> |
| <span id="chain"></span><code>chain</code> | ch | `ch` | <code>mag<4> * <wbr>[rod](#rod)</code> |
| <span id="cup"></span><code>cup</code> | c | `c` | <code>mag_ratio<1, 2> * <wbr>[pint](#pint)</code> |
| <span id="degree_Fahrenheit"></span><code>degree_<wbr>Fahrenheit</code> | ℉ (\`F) | `deg_F` | <code>[rankine](#rankine)<br>origin: [zeroth_<wbr>degree_<wbr>Fahrenheit](#zeroth_degree_Fahrenheit)</code> |
| <span id="dry_barrel"></span><code>dry_<wbr>barrel</code> | bbl | `dry_bbl` | <code>mag<7056> * <wbr>cubic([yard_<wbr>pound::inch](yard_pound.md#inch))</code> |
| <span id="dry_gallon"></span><code>dry_<wbr>gallon</code> | gal | `dry_gal` | <code>mag_ratio<1, 2> * <wbr>[peck](#peck)</code> |
| <span id="dry_pint"></span><code>dry_<wbr>pint</code> | pt | `dry_pt` | <code>mag_ratio<1, 2> * <wbr>[dry_<wbr>quart](#dry_quart)</code> |
| <span id="dry_quart"></span><code>dry_<wbr>quart</code> | qt | `dry_qt` | <code>mag_ratio<1, 4> * <wbr>[dry_<wbr>gallon](#dry_gallon)</code> |
| <span id="fluid_dram"></span><code>fluid_<wbr>dram</code> | fl dr | `fl_dr` | <code>mag<60> * <wbr>[minim](#minim)</code> |
| <span id="fluid_ounce"></span><code>fluid_<wbr>ounce</code> | fl oz | `fl_oz` | <code>mag_ratio<1, 4> * <wbr>[gill](#gill)</code> |
| <span id="furlong"></span><code>furlong</code> | fur | `fur` | <code>mag<10> * <wbr>[chain](#chain)</code> |
| <span id="gallon"></span><code>gallon</code> | gal | `gal` | <code>mag<231> * <wbr>cubic([yard_<wbr>pound::inch](yard_pound.md#inch))</code> |
| <span id="gill"></span><code>gill</code> | gi | `gi` | <code>mag_ratio<1, 2> * <wbr>[cup](#cup)</code> |
| <span id="hogshead"></span><code>hogshead</code> | hogshead | — | <code>mag<63> * <wbr>[gallon](#gallon)</code> |
| <span id="inch_of_mercury"></span><code>inch_<wbr>of_<wbr>mercury</code> | inHg | `inHg` | <code>mag_ratio<3'386'389, 1'000> * <wbr>[si::pascal](si.md#pascal)</code> |
| <span id="link"></span><code>link</code> | li | `li` | <code>mag_ratio<33, 50> * <wbr>[yard_<wbr>pound::foot](yard_pound.md#foot)</code> |
| <span id="minim"></span><code>minim</code> | min | `min` | <code>mag_ratio<1, 80> * <wbr>[teaspoon](#teaspoon)</code> |
| <span id="oil_barrel"></span><code>oil_<wbr>barrel</code> | bbl | — | <code>mag_ratio<4, 3> * <wbr>[barrel](#barrel)</code> |
| <span id="peck"></span><code>peck</code> | pk | `pk` | <code>mag_ratio<1, 4> * <wbr>[bushel](#bushel)</code> |
| <span id="pennyweight"></span><code>pennyweight</code> | dwt | `dwt` | <code>mag<24> * <wbr>[yard_<wbr>pound::grain](yard_pound.md#grain)</code> |
| <span id="pint"></span><code>pint</code> | pt | `pt` | <code>mag_ratio<1, 2> * <wbr>[quart](#quart)</code> |
| <span id="pottle"></span><code>pottle</code> | pot | `pot` | <code>mag_ratio<1, 2> * <wbr>[gallon](#gallon)</code> |
| <span id="quart"></span><code>quart</code> | qt | `qt` | <code>mag_ratio<1, 2> * <wbr>[pottle](#pottle)</code> |
| <span id="quarter"></span><code>quarter</code> | qr | `qr` | <code>mag<25> * <wbr>[yard_<wbr>pound::pound](yard_pound.md#pound)</code> |
| <span id="rankine"></span><code>rankine</code> | °R (\`R) | — | <code>mag_ratio<5, 9> * <wbr>[si::kelvin](si.md#kelvin)<br>origin: [zeroth_<wbr>rankine](#zeroth_rankine)</code> |
| <span id="rod"></span><code>rod</code> | rd | `rd` | <code>mag<25> * <wbr>[link](#link)</code> |
| <span id="section"></span><code>section</code> | section | `section` | <code>mag<640> * <wbr>[acre](#acre)</code> |
| <span id="short_hundredweight"></span><code>short_<wbr>hundredweight</code> | cwt | `cwt` | <code>mag<100> * <wbr>[yard_<wbr>pound::pound](yard_pound.md#pound)</code> |
| <span id="short_ton"></span><code>short_<wbr>ton</code> | — | — | alias to [ton](#ton) |
| <span id="shot"></span><code>shot</code> | jig | `jig` | <code>mag<3> * <wbr>[tablespoon](#tablespoon)</code> |
| <span id="survey1893-chain"></span><code>survey1893::chain</code> | ch | — | <code>mag<4> * <wbr>[survey1893::rod](usc.md#rod)</code> |
| <span id="survey1893-furlong"></span><code>survey1893::furlong</code> | fur | — | <code>mag<10> * <wbr>[survey1893::chain](usc.md#chain)</code> |
| <span id="survey1893-league"></span><code>survey1893::league</code> | lea | — | <code>mag<3> * <wbr>[survey1893::us_<wbr>survey_<wbr>mile](usc.md#us_survey_mile)</code> |
| <span id="survey1893-link"></span><code>survey1893::link</code> | li | — | <code>mag_ratio<33, 50> * <wbr>[survey1893::us_<wbr>survey_<wbr>foot](usc.md#us_survey_foot)</code> |
| <span id="survey1893-rod"></span><code>survey1893::rod</code> | rd | — | <code>mag<25> * <wbr>[survey1893::link](usc.md#link)</code> |
| <span id="survey1893-us_survey_foot"></span><code>survey1893::us_<wbr>survey_<wbr>foot</code> | ft(us) | — | <code>mag_ratio<1'200, 3'937> * <wbr>[si::metre](si.md#metre)</code> |
| <span id="survey1893-us_survey_mile"></span><code>survey1893::us_<wbr>survey_<wbr>mile</code> | mi(us) | — | <code>mag<8> * <wbr>[survey1893::furlong](usc.md#furlong)</code> |
| <span id="tablespoon"></span><code>tablespoon</code> | tbsp | `tbsp` | <code>mag_ratio<1, 2> * <wbr>[fluid_<wbr>ounce](#fluid_ounce)</code> |
| <span id="teaspoon"></span><code>teaspoon</code> | tsp | `tsp` | <code>mag_ratio<1, 3> * <wbr>[tablespoon](#tablespoon)</code> |
| <span id="ton"></span><code>ton</code> | t | `t` | <code>mag<2'000> * <wbr>[yard_<wbr>pound::pound](yard_pound.md#pound)</code> |
| <span id="troy_once"></span><code>troy_<wbr>once</code> | oz t | `oz_t` | <code>mag<20> * <wbr>[pennyweight](#pennyweight)</code> |
| <span id="troy_pound"></span><code>troy_<wbr>pound</code> | lb t | `lb_t` | <code>mag<12> * <wbr>[troy_<wbr>once](#troy_once)</code> |

## Point Origins

| Name | Type | Definition |
|------|:----:|------------|
| <span id="zeroth_degree_Fahrenheit"></span>`zeroth_degree_Fahrenheit` | relative | <code>::mp_units::point<[si::milli](si.md#milli)<[rankine](#rankine)>>(459'670)</code> |
| <span id="zeroth_rankine"></span>`zeroth_rankine` | — | alias to [si::zeroth_<wbr>kelvin](si.md#zeroth_kelvin) |
