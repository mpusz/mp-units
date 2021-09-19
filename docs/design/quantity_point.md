# Background

This issue shall provide a basis for the discussion of PR #232 which provides basis to implement PointOrigins
that are related to each other using a known fixed offset.

## Real-world examples of PointOrigins

The main example are the temperatures, where e.g. the origin of the Celsius temperature scale differs from the
Fahrenheit temperature scale by 160/9 °C or 32 °F.

Another standard example are timestamps or calendars, which identify a point in time by counting from an epoch,
such as Unix time whose epoch is midnight January 1st 1970, thus offset by roughly 1970 years from
the epoch of the christian calendar.

In specific fields many more examples exist, such as elevations used in cartography;
elevation above mean sea level depends on which sea you choose.
For example Germany refers its elevation to the north sea,
while Switzerland refers its elevations to the mediterran sea.
The difference is 27 cm, and apparently has caused
[some embarrassment in the past](https://www.normaalamsterdamspeil.nl/wp-content/uploads/2015/03/website_bridge.pdf).
Aviation doesn't usually care about those 27cm, but they use numerous distinct elevation/height measurements,
such as [AGL (height above ground level), AMSL (height above mean sea level, AFE (above [air-]field elevation), barometric height, etc.](https://en.wikipedia.org/wiki/Height_above_ground_level).

## Library implementation

The library's approach to this issue is by explicitly naming the origin that a QuantityPoint refers to,
through its template type argument of concept PointType (introduced in PR #288).
This follows the example of `std::chrono::time_point`, which denotes the epoch used through its `Clock` template argument.

## Design challenges of implicit and explicit PointOrigins

The goal of this library is to provide a) an easy-to-use interface that
b) makes it hard to make mistakes (and c) causing close to zero performance overhead).

These two come to clash especially with temperature scales,
where a statement of a temperature ambiguously may refer to a temperature difference
(e.g. the difference between the melting point and the boiling point of water; approximately 100 °C or 180 °F)
or to a temperature point (e.g. the boiling point of water itself; approximately 100 °C or 212 °F).

In attempt to achieve both ease of use and correctness,
the library will have to deal with explicitly stated PointOrigins such as the various elevation references in aviation,
as well as origins implicitly implied by the unit of measurement.
There are cases where the library has to decide between interpreting statements of such units as including a statement
about an origin or only as a relative measurement.
Therefore, **many of the poll questions will distinguish between the case of units with an implicitly implied origin (temperatures)
and without implicit origins**.

# Design choices poll

The formulation of the questions is intended to be as orthogonal as possible.
You are asked to select a single choice for each question.
If the current state of PR #232 implements a specific choice, it is indicated by 🔘.
The poll favourite is indicated by ⭐. The author's favourite is indicated by 🤳.

Also: Essentially all names in the proposal are up for bike-shedding, but please keep the bike-shedding separate
from both the "should a name exist for that concept?" and "what should that name mean?".

## Implicit and explicit conversions between QuantityPoints

### 1.1) Should re-expression with respect to a shifted origin be considered a *safe* conversion, and thereby implicitly allowed?

Taking the example of cartographic elevations, let the PointOrigin types `mean_mediterran_sea_level` and `mean_north_sea_level`
denote distinct cartographic null used in Switzerland and Germany respectively, whose offset is known and fixed at 27 cm.
Then, the expressions `quantity_point<mean_mediterran_sea_level, centimetre, int>(43200)`
and `quantity_point<mean_north_sea_level, centimetre, int>(43227)` describe the same elevation.

When the library knows the offset between the origins,
it is generally able to re-express quantity points with respect to one origin as quantity points with respect to another origin.
Such a re-expression is also what happens in **temperature conversions**.

As with the conversion between quantites expressed in different units, there are representation issues (truncation, overflow)
to be considered though. Generally, determining the exact conditions when such issues appear is more difficult
for shifted origin quantity points than for plain quantities, as there is both a scaling and an offset involved.

Which of those conversions should be allowed implicitly by the library?

 - [ ] **T)** All non-truncating (i.e. always for floating-point types); similarly to plain quantities, overflow is not being taken into account. *°C* to *K* would only be allowed for floating point types (the offset is non-integer), but *°C* to *mK* would be (the offset is 273150 mK). For an `int16_t` target representation, the conversion of *0 °C* would silently overflow to *11006 mK* (implementation defined) instead of *273160 mK*.
 - [ ] **O)** 🤳 Only if non-truncating and the source origin fits the range of the destination type, as a partial precaution against conversion to small-ranged target representation if the offset is large.
 - [ ] **S)** Always for floating-point types, plus those conversions on integer types that are able to exactly represent all source values over the full source range. Note that this essentially always requires a widening of the integer type. 
 - [ ] **F)** Only for floating-point types.
 - [ ] **N)** 🔘 None at all. All conversions with non-equivalent origins should be explicit.

*In general, implicit conversions should only ever be allowed when the risk of unintended consequences is negligible.*

*Quantities in this library represent physical objects. The numerical representation is secondary.
Both `1 m` and `1000 mm` denote the same physical length, therefore an implicit conversion between the former
and the latter is generally acceptable, even if the result of the `.number()` accessor differs drastically between the two.*

*However, in code, there are additional challenges surrounding the representability of the number depending 
on the representation type.
Therefore, this library allows implicit conversions between quantities with different representations if the corresponding
implicit conversion between the representation types is enabled.
Furthermore, this library also allows implicit conversions between quantities with different units as long as the necessary
rescaling can be performed without undue truncation (that is, it is always enabled for floating point types,
but for integer types only if the scaling factor is integer). Potential overflow on the other hand is not taken into account.*

### 1.2) `quantity_[point_][kind_]cast<Unit>`, when `Unit` is associated with an implicit origin

(The author believes that there should be just a single such cast, `quantity_cast` -
see [this comment](https://github.com/mpusz/units/issues/239#issuecomment-922451489) to #239).
`quantity_cast<Unit>` will convert a quantity to the requested unit.
For a general quantity_point (maybe with user-defined PointOrigin)
it would similarly convert the unit used to represent the distance from the origin, without affecting the origin itself.
However, if `Unit` implicitly implies an origin, the library has the choice to also convert to that origin.
Depending on the choice, the resulting representation differs, even if the physically represented point always stays the same.

When should the library convert to the implicit origin? 

 - [ ] **N)** 🔘 Never. `quantity_cast<kelvin>(quantity_point<zero_deg_C_point,celsius>(10.))` becomes `quantity_point<zero_deg_C_point,kelvin>(10.)`.
 - [ ] **M)** Only when the source origin point is equivalent to the implicit origin of the source unit. `quantity_cast<kelvin>(quantity_point<zero_deg_C_point,celsius>(10.))` becomes `quantity_point<absolute_zero_point,kelvin>(283.15)`, but `quantity_cast<fahrenheit>(quantity_point<zero_deg_C_point,kelvin>(10.))` becomes `quantity_point<zero_deg_C_point,fahrenheit>(18)`.
 - [ ] **A)** 🤳 Always. `quantity_cast<kelvin>(quantity_point<zero_deg_C_point,celsius>(10.))` becomes `quantity_point<absolute_zero_point,kelvin>(283.15)`.

## Ways to construct instances of QuantityPoints

### 2.1) Explicit constructor with fully specified PointOrigin type

For an expression of the form `quantity_point<mean_sea_level,meter,int>(h)` or
`quantity_point<absolute_zero,millikelvin,double>(t)`, what arguments should be allowed, and what should it mean?

If the argument is a `quantity_point`, then it shall be allowed, and shall be equivalent to a `quantity_cast`:
It is an explicit conversion, respecting quantity semantics.

In analogy to quantities, construction from pure representation types could be allowed too,
causing the provided value to be casted into `.number()`.

As an intermediate option, quantities (relative ones, not quantity_point) could be interpreted as the offset from the explicit origin.

Which of those should be enabled?

- [ ] **A)** 🤳 All. `QuantityPoint`, `Quantity` and `Representation`.
- [ ] **R)** Only `QuantityPoint` and `Representation`.
- [ ] **N)** Only `QuantityPoint`. User code can only enter quantity-point-space by using other means.

### 2.2) Explicit constructor with unspecified PointOrigin using CTAD, when the argument is a `Quantity`.

This question only applies if the previous design choice (2.1) is selected to **A)** - allowing `Quantity` as constructor arguments.
In this case, the library has to select (guess!) a PointOrigin, thereby imposing physical meaning not provided explicitly by the user.
When there is no implicit origin associated with the unit of the quantity, it has two sensible options:
The library can either refuse the argument, or it could supply an `unspecified_origin` which would be considered
to be of unknown offset to all other origins encountered.
When there is an implicit origin associated with the unit, the library could choose that origin instead.

Should the library:
- [ ] **R)** Refuse guessing an origin according to the (Zen of Python: "In the face of ambiguity, refuse the temptation to guess")[https://www.python.org/dev/peps/pep-0020].
- [ ] **U)** Always choose `unspecified_origin`. Attention: `quantity_point(0*deg_C) == quantity_point(0*deg_F)` - potentially unexpected.
- [ ] **I)** Never choose `unspecified_origin`, allow only implicit origins. `quantity_point(0*deg_C) == quantity_point(32*deg_F)`. Careful with generic code: `quantity_cast<temperature_unit>(quantity_point(temperature_quantity)) != quantity_point(quantity_cast<temperature_unit>(temperature_quantity))`.
- [ ] **C)** 🔘🤳 Choose either `unspecified_origin` or the implicit one. `quantity_point(0*deg_C) == quantity_point(32*deg_F)`. Careful with generic code: `quantity_cast<temperature_unit>(quantity_point(temperature_quantity)) != quantity_point(quantity_cast<temperature_unit>(temperature_quantity))`.

Those same rules would equivalently be applied to the `Kind`-equivalents.

### 2.3) `absolute` free-standing factory function with explicit `PointOrigin`

This factory function is templated on the `PointOrigin`-type and used as follows:
`absolute<mean_sea_level>(432*m)`.

- [ ] **Y)** 🔘🤳 The library should provide such a factory function.
- [ ] **N)** Don't offer this.

### 2.4) Factory function with implicit `PointOrigin`

This is similar to 2.2) (CTAD-based guessing of the PointOrigin in the constructor), but given an explicit shouting name.
Again, the function is presented with a `Quantity`, and the library selects a suitable `PointOrigin` type.

The library should
- [ ] **R)** Not provide such a factory function.
- [ ] **U)** Provide `with_unspecified_origin`, which always chooses `unspecified_origin`: `with_unspecified_origin(0*deg_C) == with_unspecified_origin(0*deg_F)`.
- [ ] **I)** 🔘🤳 Provide `interpret_as_temperature_point`, which never chooses `unspecified_origin`, and allows only units with implicit origins. `interpret_as_temperature_point(0*deg_C) == interpret_as_temperature_point(32*deg_F)`. Careful with generic code: `quantity_cast<temperature_unit>(interpret_as_temperature_point(temperature_quantity)) != interpret_as_temperature_point(quantity_cast<temperature_unit>(temperature_quantity))`.
- [ ] **C)** Provide `with_default_origin`: Chooses either `unspecified_origin` or the implicit one. `with_default_origin(0*deg_C) == with_default_origin(32*deg_F)`. Careful with generic code: `quantity_cast<temperature_unit>(with_default_origin(temperature_quantity)) != with_default_origin(quantity_cast<temperature_unit>(temperature_quantity))`.

### 2.5) UDLs for temperatures

The author believes that the library should only provide creation helpers for quantity points where it is also able
to provide a well-specified origin, i.e. temperatures.

Let us consider this poll question independent of #274 ("The best quantity creation helper"),
for the case that UDLs become the "blessed" creation helpers.

- [ ] **N)** 🔘 No UDLs for temperature points, but normal UDLs for quantities: `10_q_deg_C == 18_q_deg_F`.
- [ ] **Q)** Normal UDLs create quantities, extra UDLs create quantity_points: `10_q_deg_C == 18_q_deg_F`, but `10_qp_deg_C == 50_qp_deg_F`.
- [ ] **P)** Normal UDLs create quantity points, extra UDLs create quantities: `10_q_deg_C == 50_q_deg_F` but `10_dq_deg_C == 18_dq_deg_F` (this is roughly the approach taken by the [Pint python library](https://pint.readthedocs.io/en/stable/nonmult.html)). 
- [ ] **S)** To avoid confusion, do not provide any normal UDL, but separate UDLs for quantites and quantity_points: `10_qp_deg_C == 50_qp_deg_F` and `10_dq_deg_C == 18_dq_deg_F`.

### 2.6) Aliases/factory functions for temperatures

The author believes that the library should only provide creation helpers for quantity points where it is also able
to provide a well-specified origin, i.e. temperatures.

Note that true type-aliases require creation by CTAD with specified PointOrigin (Question 2.1 A) or R)),
but even if that were disallowed, we could still create maching factory functions.

Let us consider this poll question independent of #274 ("The best quantity creation helper"),
for the case that Aliases become the "blessed" creation helpers.

- [ ] **N)** 🔘 No Aliases for temperature points, but normal Aliases for quantities: `deg_C(10) == deg_F(18)`.
- [ ] **Q)** Normal Aliases create quantities, extra Aliases create quantity_points: `deg_C(10) == deg_F(18)`, but `p_deg_C(10) == p_deg_F(50)`.
- [ ] **P)** Normal Aliases create quantity points, extra Aliases create quantities: `deg_C(10) == deg_F(50)` but `delta_deg_C(10) == delta_deg_F(18)` (this is roughly the approach taken by the [Pint python library](https://pint.readthedocs.io/en/stable/nonmult.html)).
- [ ] **S)** To avoid confusion, do not provide any normal Aliases, but separate Aliases for quantites and quantity_points: `p_deg_C(10) == p_deg_F(50)` and `delta_deg_C(10) == delta_deg_F(18)`.

### 2.7) References for temperatures

The author believes that the library should only provide creation helpers for quantity points where it is also able
to provide a well-specified origin, i.e. temperatures.

Note that references for quantity_points cannot be combined at all (multiplied/divided with other references) -
the origin point lives in a specific dimension, and loses it's meaning in the dimension for a coherent/combined unit.

Let us consider this poll question independent of #274 ("The best quantity creation helper"),
for the case that References become the "blessed" creation helpers.

- [ ] **N)** 🔘 No References for temperature points, but normal References for quantities: `10*deg_C == 18*deg_F`.
- [ ] **Q)** Normal References create quantities, extra References create quantity_points: `10*deg_C == 18*deg_F`, but `10*p_deg_C == 50*p_deg_F`.
- [ ] **P)** Normal References create quantity points, extra References create quantities: `10*deg_C == 50*deg_F` but `10*delta_deg_C == 18*delta_deg_F` (this is roughly the approach taken by the [Pint python library](https://pint.readthedocs.io/en/stable/nonmult.html)).
- [ ] **S)** To avoid confusion, do not provide any normal References, but separate References for quantites and quantity_points: `10*p_deg_C == 50*p_deg_F` and `10*delta_deg_C == 18*delta_deg_F`.

### 2.8) Zero-point constants

The library could provide compile-time constants for quantity_points with zero offset from specific origins,
potentially with optimisations implemented to make their usage completely zero-cost at runtime.

Those tend to be a bit more verbose than previous helpers.

By exception, this poll question is multiple-choice, and more than one answer may be selected.

- [ ] **N)** 🔘 No zero-point constant support provided by the library at all.
- [ ] **G)** 🤳 Provide just a templated constant so the user can plug in his origin of choice: `origin_point<zero_deg_C> + 10*deg_C == origin_point<zero_deg_F> + 50*deg_F`.
- [ ] **T)** 🤳 Create constants for the temperatures: `zero_deg_C + 10*deg_C == zero_deg_F + 50*deg_F`.
- [ ] **P)** 🤳 Let instances of the actual `PointOrigin` type act as zero-point constants: `mean_sea_level{} + 450*m == runway_threshold_elevation_ZRH{} +18*m`.

## Custom PointOrigins

The library will not provide AGL, AMSL etc., but a glide computer will want to implement that.

### 3.1) Should the API to declare fixed known offsets between origin points explicitly call out the reference origin?

Without that, it would not be possible to create OriginPoints A,B,C and D on the same dimension,
where A and B have a fixed known offset,
C and D have a fixed known offset, but no relation between either A - C, A - D or B - D is known.
Operations between QuantityPoints referring to unrelated origins are essentially all disallowed,
so a similar effect could be achieved using Kinds instead.

A good example could be elevations/heights measured using different elevation models, such as
differing reference geoids or even height-above-ground-level (not a geoid).
These reference points have no fixed offset from each other (they depend on geographic position),
but on the other hand many elevation models refer to the same reference geoid but have different reference points (heights). 

Elevations based on different models are not generally interoperable, so one could use different Kind's to enforce that in code.
However, that would imply also non-interoperable quantities and the following code would not work:
`height_amsl = ground_elevation_at(current_position) + heigh_agl.relative()`.

- [ ] **Y)** 🔘🤳 The API to declare fixed offsets between PointOrigins should explicitly call out the reference origin.
- [ ] **N)** The API should be as simple to use as possible - despite the risk of confusion / impacted readability when code declares an offset failing to indicate with respect to what.

### 3.2) API to declare PointOrigins explicitly with a fixed offset to other PointOrigins
- 
- [ ] **T)** Declare the reference origin and the offset as template arguments to the base PointOrigin; e.g. `struct zero_deg_C_point : point_origin<dim_thermodynamic_temperature, absolute_zero_point, ratio(27315,100), kelvin> {};`.
- [ ] **B)** 🔘🤳 Declare the reference origin and the offset in the body of the PointOrigin declaration.
- [ ] **O)** Use another method.

## Custom units with implicit origins (like temperatures)

### 4.1) Should we provide a public API to declare implicit PointOrigins?

- [ ] **N)** 🔘🤳 Implicit origins are a constant source of confusion - we should not make it easy to implement such horrific things and therefore do not specify a public API.
- [ ] **Y)** If implicit origins are implemented in the library, there will be some form of an API. We might as-well make it public.

### 4.2) API to declare implicit PointOrigins

Public or otherwise, implicit origins will be implemented

- [ ] **D)** Declare the reference origin and the offset right in the unit declaration; e.g. `struct degree_celsius : alias_unit<kelvin, basic_symbol_text{"\u00b0C", "C"}, no_prefix, implicit_origin<kelvin,ratio(27315,100)>> {};`. Disadvantage: Might require a complicated tag/policy to implement that without imposing a dummy `no_implicit_origin` on the vast majority of units.
- [ ] **S)** 🔘 Use a traits template that needs to be specialised for opt-in units. Disadvantage: The traits template will not live in the same namespace as the unit.
- [ ] **A)** Use an ADL-based opt-in mechanism. Advantage: Can be in the same namespace. Disadvantage: Brittle implementation?