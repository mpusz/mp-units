# V2 Design Discussion

**<big>Please help determining the based design for the library!</big>**

## System Definition

A common convention in this library is to assign the same name for a type and an object of this type.
Besides defining them user never works with the types in the source code. All operations
are done on the objects. Contrarily, the types are the only one visible in the compilation
errors. Having them of the same names improves user experience and somehow blurs those separate domains.


### Prefixes

```cpp
namespace mp_units::si {

// ...
template<PrefixableUnit auto U> struct centi_ : prefixed_unit<"c", mag_power<10, -2>, U> {};
template<PrefixableUnit auto U> struct deci_  : prefixed_unit<"d", mag_power<10, -1>, U> {};
template<PrefixableUnit auto U> struct deca_  : prefixed_unit<"da", mag_power<10, 1>, U> {};
template<PrefixableUnit auto U> struct hecto_ : prefixed_unit<"h", mag_power<10, 2>, U> {};
template<PrefixableUnit auto U> struct kilo_  : prefixed_unit<"k", mag_power<10, 3>, U> {};
// ...

// ...
template<PrefixableUnit auto U> inline constexpr centi_<U> centi;
template<PrefixableUnit auto U> inline constexpr deci_<U> deci;
template<PrefixableUnit auto U> inline constexpr deca_<U> deca;
template<PrefixableUnit auto U> inline constexpr hecto_<U> hecto;
template<PrefixableUnit auto U> inline constexpr kilo_<U> kilo;
// ...

}
```

_NOTE:_ Currently in the C++ language it is impossible to create a class template and a variable
or a variable template with the same identifier. This is why prefixes are the only exception from
the general rule used in the library that the type and the instance of it have the same identifier.


### Units

```cpp
namespace mp_units::si {

// base units
inline constexpr struct second : named_unit<"s", isq::time> {} second;
inline constexpr struct metre : named_unit<"m", isq::length> {} metre;
inline constexpr struct gram : named_unit<"g", isq::mass> {} gram;
inline constexpr struct kilogram : decltype(kilo<gram>) {} kilogram;
// ...

// derived named units
inline constexpr struct radian : named_unit<"rad", metre / metre> {} radian;
inline constexpr struct steradian : named_unit<"sr", square<metre> / square<metre>> {} steradian;
inline constexpr struct hertz : named_unit<"Hz", 1 / second> {} hertz;
inline constexpr struct newton : named_unit<"N", kilogram * metre / square<second>> {} newton;
inline constexpr struct pascal : named_unit<"Pa", newton / square<metre>> {} pascal;
inline constexpr struct joule : named_unit<"J", newton * metre> {} joule;
// ....


// non-SI units accepted for use with the SI
inline constexpr struct minute : named_unit<"min", mag<60> * second> {} minute;
inline constexpr struct hour : named_unit<"h", mag<60> * minute> {} hour;
inline constexpr struct day : named_unit<"d", mag<24> * hour> {} day;
inline constexpr struct degree : named_unit<basic_symbol_text{"°", "deg"}, mag_pi / mag<180> * radian> {} degree;
inline constexpr struct are : named_unit<"a", square<deca<metre>>> {} are;
inline constexpr struct hectare : decltype(hecto<are>) {} hectare;
inline constexpr struct litre : named_unit<"l", cubic<deci<metre>>> {} litre;
inline constexpr struct tonne : named_unit<"t", mag<1000> * kilogram> {} tonne;
inline constexpr struct dalton : named_unit<"Da", mag<ratio{16'605'390'666'050, 10'000'000'000'000}> * mag_power<10, -27> * kilogram> {} dalton;
// ...

}
```

The library does not provide strong types for prefixed versions of all units because we believe
that `si::kilo<si::metre>` is readable well enough and with limiting the number of definitions
we are significantly limiting compile-times and standardization effort.

In case a user would like to have a dedicated type/identifier for some specific commonly used units,
those can easily be defined with:

```cpp
inline constexpr auto kilometre = si::kilo<si::metre>;
```

or a bit longer via:

```cpp
inline constexpr struct kilometre : decltype(si::kilo<si::metre>) {} kilometre;
```


### Unit symbols

Every named unit has associated symbols defined for its primary as well as prefixed units.
To prevent "trashing" the main namespace with lots of short identifiers, those symbols
are always defined in the `unit_symbols` subnamespace. For the SI system they are also
defined in a separate header file `<units/si/unit_symbols.h>` which helps in reducing
compilation times.

```cpp
namespace mp_units::si::unit_symbols {

// ...
inline constexpr auto cm = centi<metre>;
inline constexpr auto dm = deci<metre>;
inline constexpr auto m = metre;
inline constexpr auto dam = deca<metre>;
inline constexpr auto hm = hecto<metre>;
inline constexpr auto km = kilo<metre>;
// ...

// commonly used squared and cubic units
inline constexpr auto m2 = square<metre>;
inline constexpr auto m3 = cubic<metre>;
inline constexpr auto s2 = square<second>;
 
}
```

The above means that by default unit symbols are not available and a user has to explicitly
opt-in with using-directive to use them.

```cpp
#include <mp_units/isq/space_and_time.h>
#include <mp_units/si/unit_symbols.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

constexpr auto v1 = 110 * isq::speed[km / h];
```

### Dimensions

A user is required to create strong types for dimensions of all base quantities in the system:

```cpp
namespace mp_units::isq {

inline constexpr struct dim_length : base_dimension<"L"> {} dim_length;
inline constexpr struct dim_mass : base_dimension<"M"> {} dim_mass;
inline constexpr struct dim_time : base_dimension<"T"> {} dim_time;
inline constexpr struct dim_electric_current : base_dimension<"I"> {} dim_electric_current;
inline constexpr struct dim_thermodynamic_temperature : base_dimension<basic_symbol_text{"Θ", "O"}> {} dim_thermodynamic_temperature;
inline constexpr struct dim_amount_of_substance : base_dimension<"N"> {} dim_amount_of_substance;
inline constexpr struct dim_luminous_intensity : base_dimension<"J"> {} dim_luminous_intensity;

}
```

Derived dimensions are never explicitly created by the user. They are formed while doing
dimensional arithmetics on quantity specification objects. Derived dimensions are always
expressed in terms of base dimensions raised to proper powers.

```cpp
static_assert(isq::power.dimension == isq::dim_mass * pow<2>(isq::dim_length) / pow<3>(isq::dim_time));
static_assert(std::is_same_v<decltype(isq::power.dimension), derived_dimension<power<isq::dim_length, 2>, isq::dim_mass, per<power<isq::dim_time, 3>>>>);
```

### Quantity Specification

The unit, dimension, and a representation type is not enough to represent all properties of the quantity
(see [#405](https://github.com/mpusz/units/issues/405)). We would like the mp-units to be able to model
most or even all quantities defined by the ISO 80000. This is why a new abstraction called `quantity_spec`
was introduced.

`quantity_spec` specifies the following quantity properties:
- dimension that can be obtained via `dimension` static class member
- quantity kind
- characteristics (scalar, vector, tensor)
- quantity factor (i.e. to model radius being 1/2 of a diameter)

_NOTE:_ The name of this class template is still a subject for bikeshedding. Please let us know if you have
a better name for it.

```cpp
namespace mp_units::isq {

// base quantities
inline constexpr struct length : quantity_spec<dim_length> {} length;  // scalar quantity by default
inline constexpr struct mass : quantity_spec<dim_mass> {} mass;
inline constexpr struct time : quantity_spec<dim_time> {} time;
inline constexpr struct electric_current : quantity_spec<dim_electric_current> {} electric_current;
inline constexpr struct thermodynamic_temperature : quantity_spec<dim_thermodynamic_temperature> {} thermodynamic_temperature;
inline constexpr struct amount_of_substance : quantity_spec<dim_amount_of_substance> {} amount_of_substance;
inline constexpr struct luminous_intensity : quantity_spec<dim_luminous_intensity> {} luminous_intensity;

// quantity kinds
inline constexpr struct width : quantity_spec<length> {} width;
inline constexpr struct height : quantity_spec<length> {} height;
inline constexpr struct thickness : quantity_spec<width> {} thickness;
inline constexpr struct diameter : quantity_spec<width> {} diameter;
inline constexpr struct radius : quantity_spec<mag<ratio{1, 2}> * diameter> {} radius;   // TBD (multipliers on quantity level)
inline constexpr struct position_vector : quantity_spec<length, quantity_character::vector> {} position_vector;  // vector quantity
// ...

// aliased names (different name for the same quantity)
inline constexpr auto breadth = width;
inline constexpr auto depth = height;
inline constexpr auto altitude = height;
inline constexpr auto duration = time;
// ...

// derived quantities
inline constexpr struct curvature : quantity_spec<1 / radius_of_curvature> {} curvature;
inline constexpr struct area : quantity_spec<pow<2>(length)> {} area;
inline constexpr struct volume : quantity_spec<pow<3>(length)> {} volume;
inline constexpr struct velocity : quantity_spec<position_vector / duration> {} velocity; // vector character derived from ingredients
inline constexpr struct speed : quantity_spec<distance / duration> {} speed;
inline constexpr struct force : quantity_spec<mass * acceleration, quantity_character::vector> {} force;
inline constexpr struct power : quantity_spec<force * velocity, quantity_character::scalar> {} power;

// ...
inline constexpr struct moment_of_inertia : quantity_spec<angular_momentum / angular_velocity, quantity_character::tensor> {} moment_of_inertia; // tensor quantity

inline constexpr struct potential_energy : quantity_spec<mass * acceleration * height> {} potential_energy;
// Is the below really a good idea (see https://github.com/mpusz/units/issues/405#issuecomment-1346970914)?
inline constexpr struct kinetic_energy : quantity_spec<mag<1, 2> * mass * pow<2>(speed)> {} kinetic_energy;

// Still not clear how to model the below
inline constexpr struct mechanical_energy : quantity_spec<potential_energy + kinetic_energy> {} mechanical_energy;

// Still not clear how to model the below which is defined as
// γ = α + iβ where α denotes attenuation and β the phase coefficient of a plane wave
inline constexpr struct propagation_coefficient : ...; 

// ...

}
```

### Quantity Reference

User should not instantiate a `reference` type explicitly. It is created indirectly by assigning
a unit to a quantity specification via overloaded indexing operator.

_NOTE:_ Reference + number form a quantity.

```cpp
static_assert(is_same_v<decltype(isq::power[W]), reference<isq::power, si::watt>>);
static_assert(is_same_v<decltype(5 * isq::power[W]), quantity<reference<isq::power, si::watt>{}, int>>);
```

Sometimes it might be useful to store a named object of a commonly used reference type:

```cpp
constexpr auto kmph = isq::speed[km / h];
constexpr auto speed = 90 * kmph;
static_assert(is_same_v<decltype(speed), quantity<reference<isq::speed, derived_unit<si::kilo_<si::metre>, per<si::hour>>{}>{}, int>>);
```

References store `quantity_spec`, `dimension`, and `unit`. They are also equality comparable.

```cpp
static_assert(isq::power[W].quantity_spec == isq::power);
static_assert(isq::power[W].dimension == (isq::force * isq::speed).dimension);
static_assert(isq::power[W].unit == si::watt);
static_assert(isq::power[W].unit == kg * m2 / s3);
static_assert(isq::power[W] == isq::power[N * m / s]);
```

### Comparison of the V2 framework with the initial design

Introduction of above abstractions, when compared to the initial version of the library,
provided better flexibility, enabled new features, and removed (or at least heavily limited)
the number of definitions needed to define a system.

In the new design we do not have to define:
- strong types for derived dimensions
- unnamed derived versions of all units
- most of the quantity creation helpers (no UDLs and aliases)
  - unit symbols are provided in separate namespaces only for named base and derived units
    and their prefixes


## Quantity Creation

In the new design, quantity takes a `reference` object and a representation type to store a number.
`reference` represents all properties of a quantity. Representation type has to obey quantity
characteristics defined in `quantity_spec`.

Such design unifies two ways to construct a quantity:
- through initialization of its quantity type

  ```cpp
  quantity<isq::speed[km / h]> speed{60.};
  ```

- via a multiplication of a number and a quantity reference

  ```cpp
  auto speed = 60. * isq::speed[km / h];
  ```

### Named quantity and named unit

```cpp
using namespace mp_units;
auto p = quantity<isq::power[si::watt], int>{5};
```

or

```cpp
using namespace mp_units;
auto p = 5 * isq::power[si::watt];
```

or

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;
auto p = 5 * isq::power[W];
```

```cpp
static_assert(std::is_same_v<decltype(p), quantity<reference<isq::power, si::watt>{}, int>>);
static_assert(std::is_same_v<decltype(p.dimension), derived_dimension<power<isq::dim_length, 2>, isq::dim_mass, per<power<isq::dim_time, 3>>>>);
static_assert(p.quantity_spec != isq::force * isq::speed);
static_assert(interconvertible(p.quantity_spec, isq::force * isq::speed));
static_assert(p.dimension == isq::force.dimension * isq::speed.dimension);
```

### Named quantity and derived (unnamed) unit

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;
auto s = quantity<isq::speed[m / s], int>{5};
```

or

```cpp
using namespace mp_units;
auto s = 5 * isq::speed[si::metre / si::second];
```

or

```cpp
using namespace mp_units;
using namespace mp_units::si::unit_symbols;
auto s = 5 * isq::speed[m / s];
```

```cpp
static_assert(std::is_same_v<decltype(s),
              quantity<reference<isq::speed, derived_unit<si::metre, per<si::second>>{}>{}, int>>);
```


## Concepts

### `quantity_of<T>`

Checks whether the quantity is exactly of a specific property where property can be:
- `quantity_spec`
- dimension
- `reference`

```cpp
static_assert(quantity_of<decltype(10 * isq::speed[m/s] / (2 * isq::length[m]) * (4 * isq::time[s])), dimension_one>);
static_assert(!quantity_of<decltype(10 * isq::length[m]), isq::height>);   // kind of
static_assert(!quantity_of<decltype(10 * isq::width[m]), isq::height>);    // different kinds
static_assert(quantity_of<decltype(10 * isq::speed[m/s]), isq::speed>);
static_assert(!quantity_of<decltype(20 * isq::length[m] / (2 * isq::time[s])), isq::speed>);  // derived unnamed quantity
static_assert(quantity_of<decltype(10 * isq::speed[m/s]), isq::speed[m /s]>);
static_assert(!quantity_of<decltype(10 * isq::speed[m/s]), isq::speed[km / h]>);  // different unit
```


### `weak_quantity_of<T>`

Checks whether the quantity is of an interconvertible `quantity_spec` or of the same dimension
and unit (in case of `reference`). It will fail for non-equivalent but inconvertible unit.

```cpp
static_assert(weak_quantity_of<decltype(10 * isq::speed[m/s] / (2 * isq::length[m]) * (4 * isq::time[s])), dimension_one>);
static_assert(weak_quantity_of<decltype(10 * isq::length[m]), isq::height>);   // kind of
static_assert(!weak_quantity_of<decltype(10 * isq::width[m]), isq::height>);    // different kinds
static_assert(weak_quantity_of<decltype(10 * isq::speed[m/s]), isq::speed>);
static_assert(weak_quantity_of<decltype(20 * isq::length[m] / (2 * isq::time[s])), isq::speed>);  // derived unnamed quantity
static_assert(weak_quantity_of<decltype(10 * isq::speed[m/s]), isq::speed[m /s]>);
static_assert(!weak_quantity_of<decltype(10 * isq::speed[m/s]), isq::speed[km / h]>);  // different unit
```


## Quantity Comparison

```cpp
// Same dimension type & different unit
static_assert(1000 * isq::length[m] == 1 * isq::length[km]);

// Named and derived dimensions (same units)
static_assert(10 * isq::length[m] / (2 * isq::time[s]) == 5 * isq::speed[m / s]);

// Same named dimension & different but equivalent unit
static_assert(10 * isq::frequency[1 / s] == 10 * isq::frequency[Hz]);

// Named and derived dimensions (different but equivalent units)
static_assert(10 / (2 * isq::time[s]) == 5 * isq::frequency[Hz]);

// Different named dimensions
// static_assert(5 * isq::activity[Bq] == 5 * isq::frequency[Hz]); // ambiguous
```


## Quantity Dimensional Analysis

Nearly all the types described above (quantities, references, quantity specifications, dimensions,
and units) support multiplication and division operators to allow dimensional arithmetics.
As a result in most cases we get derived entities being expression templates.

Below is a short intro of what types are being currently generated in specific use cases.

### Derived (unnamed) quantity and derived (unnamed) unit

```cpp
weak_quantity_of<isq::speed> auto q = 10 * isq::length[m] / (2 * isq::time[s]);
quantity<isq::speed[m / s], int> q1 = q;  // implicitly convertible
quantity_of<isq::speed> auto q2 = quantity_cast<isq::speed>(q);

static_assert(std::is_same_v<decltype(q),
              quantity<reference<derived_quantity_spec<isq::length, per<isq::time>>{},
                       derived_unit<si::metre, per<si::second>>{}>{}, int>>);
static_assert(isq::length / isq::time != isq::speed);
static_assert(interconvertible(isq::length / isq::time, isq::speed));
static_assert((isq::length / isq::time).dimension == isq::speed.dimension);
```

### Quantity with a derived spec and a base dimension as a result of dimensional transformation

```cpp
weak_quantity_of<isq::length> auto d = 5 * isq::speed[m / s] * (5 * isq::time[s]);
quantity_of<isq::length> auto d1 = quantity_cast<isq::length>(d);

static_assert(std::is_same_v<decltype(d), quantity<reference<derived_quantity_spec<isq::speed, isq::time>{}, si::metre>{}, int>>);
static_assert(std::is_same_v<decltype(d.dimension), isq::dim_length>);
static_assert(d.dimension == isq::length.dimension);
static_assert(d.quantity_spec != isq::length);
static_assert(interconvertible(d.quantity_spec, isq::length));
```

### Dimensionless

```cpp
auto q1 = 20 * isq::speed[m / s] / (2 * isq::speed[m / s]);
auto q2 = 20 * isq::speed[m / s] / (10 * isq::length[m]) * (5 * isq::time[s]);

static_assert(std::is_same_v<decltype(q1), quantity<reference<dimensionless, one>{}, int>>);
static_assert(std::is_same_v<decltype(q2), quantity<reference<derived_quantity_spec<isq::speed, isq::time, per<isq::length>>{}, one>{}, int>>);
static_assert(interconvertible(q2.quantity_spec, dimensionless));
```


## Quantity Arithmetics

```cpp
// Named and derived dimensions (same units)
auto q1 = 10 * isq::length[m] / (2 * isq::time[s]) + 5 * isq::speed[m / s];
static_assert(is_same_v<decltype(q1),
              quantity<reference<isq::speed, derived_unit<si::metre, per<si::second>>{}>{}, int>>);

// Named and derived dimensions (different units)
auto q2 = 10 / (2 * isq::time[s]) + 5 * isq::frequency[Hz];
static_assert(is_same_v<decltype(q2), quantity<reference<isq::frequency, si::hertz>{}, int>>);

// Different named dimensions
// auto q3 = 5 * isq::activity[Bq] + 5 * isq::frequency[Hz];                           // ambiguous
// auto q4 = 5 * isq::activity[Bq] + 10 / (2 * isq::time[s]) + 5 * isq::frequency[Hz]; // ambiguous
```


## Conversions

```cpp
// Implicit conversions allowed between quantities of `interconvertible` references
auto q1 = 120 * isq::length[km] / (2 * isq::time[h]);
quantity<isq::speed[km / h]> q2 = q1;

// Unit of a quantity can be changed if no truncation will happen
auto q3 = q1[m / s];

// Explicit casts allow changing all or only a part of the type (even if truncating)
auto q4 = quantity_cast<isq::speed>(q1);
auto q5 = quantity_cast<m / s>(q1);
auto q6 = quantity_cast<isq::speed[m / s]>(q1);
auto q7 = quantity_cast<int>(q1);
auto q8 = quantity_cast<quantity<isq::speed[m / s], int>>(q1);
```


## Physical Constants

V2 framework implements [#169](https://github.com/mpusz/units/issues/169). To achieve that
a constant is modeled as a special case of named unit and a named reference:

```cpp
// constants units
inline constexpr struct speed_of_light_in_vacuum_unit :
  constant_unit<"c", mag<299'792'458> * metre / second> {} speed_of_light_in_vacuum_unit;
inline constexpr struct standard_gravity_unit :
  constant_unit<"g", mag<ratio{980'665, 100'000}> * metre / square<second>> {} standard_gravity_unit;

// constants references
inline constexpr auto speed_of_light_in_vacuum = isq::speed[speed_of_light_in_vacuum_unit];
inline constexpr auto standard_gravity = isq::acceleration[standard_gravity_unit];
```

With that we can define:

```cpp
auto speed = 0.5 * speed_of_light_in_vacuum;
auto weight = 85 * isq::mass[kg] * (1 * si::standard_gravity);
```

Having a constant factor in a type rather than in a quantity value improves runtime performance
and increases precision.

If possible, such constants will cancel at compile time during dimensional manipulation of
quantities which will improve runtime performance and precision even further.

```cpp
inline constexpr auto g = 1 * si::standard_gravity;
inline constexpr auto air_density = 1.225 * isq::mass_density[kg / m3];

class Box {
  quantity<isq::area[m2]> base_;
  quantity<isq::height[m]> height_;
  quantity<isq::mass_density[kg / m3]> density_ = air_density;
public:
  // ...

  [[nodiscard]] constexpr quantity_of<isq::weight> auto filled_weight() const
  {
    const weak_quantity_of<isq::volume> auto volume = base_ * height_;
    const weak_quantity_of<isq::mass> auto mass = density_ * volume;
    return quantity_cast<isq::weight>(mass * g);
  }

  [[nodiscard]] constexpr quantity<isq::length[m]> fill_level(const quantity<isq::mass[kg]>& measured_mass) const
  {
    return height_ * measured_mass * g / filled_weight();
  }
};
```

In the above example calling `fill_level()` will not multiply and divide by `g` at runtime.


## Quantity Characteristics

ISO 80000 defines not only scalar quantities. Some of them are defined to have vector or tensor
characteristics. To model that the library defines:

```cpp
enum class quantity_character { scalar, vector, tensor };
```

The above assumes the following hierarchy:
- every quantity can be expressed with a scalar representation type
- vector representation type can be provided only to quantities with vector or tensor characteristics
- tensor representation type can be used only for tensor quantities

The support for the above is still WIP. However, we assume that there will be variable template
customization points for representation types that a user will have to specialize for a custom types.

The default values for type traits will be as follows:

```cpp
template<typename T>
inline constexpr bool is_scalar = false;

template<typename T>
  requires std::is_floating_point_v<T> ||
             (std::is_integral_v<T> && one_of_types<T,
                                                    signed char, short, int, long, long long,
                                                    unsigned char, unsigned short, unsigned,
                                                    unsigned long, unsigned long long>())
inline constexpr bool is_scalar = true;

template<typename T>
inline constexpr bool is_vector = is_scalar<T>;

template<typename T>
inline constexpr bool is_tensor = is_vector<T>;
```

Quantities will preserve their characteristics:
- quantity kinds will have the same characteristics as the parent quantity (unless overridden)
- derived quantities will have the most restrictive characteristics from its ingredients (unless overridden)

It is still under analysis if the vector multiplied by the vector quantity should result
in a tensor quantity or a vector one by default.
