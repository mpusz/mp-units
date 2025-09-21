# Concepts

This chapter enumerates all user-facing concepts in **mp-units**.


## `Dimension<T>` { #Dimension }

`Dimension` matches a [dimension](../../appendix/glossary.md#dimension) of either a base or
derived [quantity](../../appendix/glossary.md#quantity):

- [Base dimensions](../../appendix/glossary.md#base-dimension) are explicitly defined by
  inheriting from a `base_dimension` instantiation with a unique symbol identifier for a
  specific [system of quantities](../../appendix/glossary.md#system-of-quantities).
- [Derived dimensions](../../appendix/glossary.md#derived-dimension) are implicitly created by
  the framework from a [quantity equation](../../appendix/glossary.md#quantity-equation)
  provided in a [quantity specification](../../appendix/glossary.md#quantity_spec).

All of the above dimensions have to be marked as `final`.


### `DimensionOf<T, V>` { #DimensionOf }

`DimensionOf` is satisfied when both arguments satisfy [`Dimension`](#Dimension)
and they compare equal.


## `QuantitySpec<T>` { #QuantitySpec }

`QuantitySpec` matches all [quantity specifications](../../appendix/glossary.md#quantity_spec),
including:

- [Base quantities](../../appendix/glossary.md#base-quantity) defined by inheriting from
  `quantity_spec<base_dimension>`.
- [Derived named quantities](../../appendix/glossary.md#derived-quantity) defined by inheriting
  from `quantity_spec` instantiated with a [quantity equation](../../appendix/glossary.md#quantity-equation).
- Other named quantities forming a [hierarchy of quantities](../../appendix/glossary.md#quantity-hierarchy)
  of the same [kind](../../appendix/glossary.md#kind) defined by passing parent `quantity_spec`
  instantiation as an argument.
- [Quantity kinds](../../appendix/glossary.md#kind) describing a family of mutually comparable
  quantities.
- Intermediate [derived quantity](../../appendix/glossary.md#derived-quantity) specifications
  produced by a [quantity equation](../../appendix/glossary.md#quantity-equation) on other
  specifications.

All of the above quantity specifications have to be marked as `final`.


### `QuantitySpecOf<T, V>` { #QuantitySpecOf }

`QuantitySpecOf` is satisfied when both arguments satisfy [`QuantitySpec`](#QuantitySpec) and
`T` is implicitly convertible to `V`.


## `UnitMagnitude<T>` { #UnitMagnitude }

`UnitMagnitude` is satisfied by all types defining a unit magnitude.

!!! info

    Unit magnitude implementation is a private implementation detail of the library.


## `Unit<T>` { #Unit }

`Unit` matches all [units](../../appendix/glossary.md#unit) in the library including:

- [Base units](../../appendix/glossary.md#base-unit) defined by inheriting from `named_unit`
  with a unique symbol identifier for a specific [system of units](../../appendix/glossary.md#system-of-units).
- Named scaled units defined by inheriting from `named_unit` with a unique symbol
  identifier and the product of another unit and a magnitude.
- Prefixed units defined by inheriting from `prefixed_unit` with a prefix symbol, a magnitude,
  and a base unit.
- [Derived named units](../../appendix/glossary.md#derived-unit) defined by inheriting from
  `named_unit` with a unique symbol identifier and a
  [unit equation](../../appendix/glossary.md#unit-equation) result.
- [Derived unnamed units](../../appendix/glossary.md#derived-unit) produced by a
  [unit equation](../../appendix/glossary.md#unit-equation) on other units.

All of the above units have to be marked as `final`.

!!! note

    In the **mp-units** library, [physical constants are also implemented as units](faster_than_lightspeed_constants.md).


### `AssociatedUnit<T>` { #AssociatedUnit }

`AssociatedUnit` describes a [unit with an associated quantity](../../appendix/glossary.md#associated-unit)
and is satisfied by:

- All units derived from `named_unit` instantiated with a unique symbol identifier and a
  [`QuantitySpec`](#QuantitySpec) of a [quantity kind](../../appendix/glossary.md#kind).
- All units produced by [unit equations](../../appendix/glossary.md#unit-equation) on other
  associated units.

??? abstract "Examples"

    All units in the [SI](../../appendix/glossary.md#si) have associated quantities. For example,
    `si::second` is specified to measure `isq::time`.

    Natural units typically do not have an associated quantity. For example, if we assume `c = 1`,
    a `natural::second` unit can be used to measure both `time` and `length`. In such case, `speed`
    would have a unit of `one`.


### `PrefixableUnit<T>` { #PrefixableUnit }

`PrefixableUnit` is satisfied by all units derived from `named_unit`. Such units can be
used as arguments to `prefixed_unit`.


### `UnitOf<T, V>` { #UnitOf }

`UnitOf` is satisfied for all units `T` whose associated quantity spec is implicitly convertible
to the provided [`QuantitySpec`](#QuantitySpec).


## `Reference<T>` { #Reference }

`Reference` is satisfied by all [quantity reference](../../appendix/glossary.md#reference)
types. Such types provide the meta-information required to create a
[`Quantity`](#Quantity).
A `Reference` can either be:

- An [`AssociatedUnit`](#AssociatedUnit).
- An instantiation of `reference<QuantitySpec, Unit>`.


### `ReferenceOf<T, V>` { #ReferenceOf }

`ReferenceOf` is satisfied by references `T` whose quantity specification satisfies
[`QuantitySpecOf<V>`](#QuantitySpecOf).


### `RepresentationOf<T, V>` { #RepresentationOf }

`RepresentationOf` constrains a numeric type that stores the
[value of a quantity](../../appendix/glossary.md#quantity-value) and is satisfied:

- if `V` satisfies [`QuantitySpec`](#QuantitySpec):

    - all representation types when `V` is a [quantity kind](../../appendix/glossary.md#kind),
    - otherwise, representation types whose [quantity character](../../appendix/glossary.md#character)
      matches the specification `V`.

- if `V` is a `quantity_character`:

    - by representation types that are of a provided
      [quantity character](../../appendix/glossary.md#character).


## `Quantity<T>` { #Quantity }

`Quantity` matches every [quantity](../../appendix/glossary.md#quantity) and is satisfied by
any type that is or derives from `quantity<...>`.


### `QuantityOf<T, V>` { #QuantityOf }

`QuantityOf` is satisfied by all quantities for which [`ReferenceOf<V>`](#ReferenceOf) is `true`.


### `QuantityLike<T>` { #QuantityLike }

`QuantityLike` enables interoperability with other libraries. A type `T` satisfies it if a
`quantity_like_traits<T>` specialization provides:

- `reference` static data member that matches the [`Reference`](#Reference) concept,
- `rep` type that matches [`RepresentationOf`](#RepresentationOf) concept with the character
  provided in `reference`,
- `explicit_import` static data member convertible to `bool` that specifies that the conversion
  from `T` to a `quantity` type should happen explicitly (if `true`),
- `explicit_export` static data member convertible to `bool` that specifies that the conversion
  from a `quantity` type to `T` should happen explicitly (if `true`),
- `to_numerical_value(T)` static member function returning a raw value of the quantity,
- `from_numerical_value(rep)` static member function returning `T`.

??? abstract "Examples"

    This is how support for `std::chrono::seconds` can be provided:

    ```cpp
    template<>
    struct mp_units::quantity_like_traits<std::chrono::seconds> {
      static constexpr auto reference = si::second;
      static constexpr bool explicit_import = false;
      static constexpr bool explicit_export = false;
      using rep = std::chrono::seconds::rep;

      [[nodiscard]] static constexpr rep to_numerical_value(const std::chrono::seconds& d)
      {
        return d.count();
      }

      [[nodiscard]] static constexpr std::chrono::seconds from_numerical_value(const rep& v)
      {
        return std::chrono::seconds(v);
      }
    };

    quantity q = 42s;
    std::chrono::seconds dur = 42 * s;
    ```


## `PointOrigin<T>` { #PointOrigin }

`PointOrigin` matches all [quantity point origins](../../appendix/glossary.md#point-origin).
It is satisfied by either:

- All types derived from an `absolute_point_origin` class template.
- All types derived from a `relative_point_origin` class template.


### `PointOriginFor<T, V>` { #PointOriginFor }

`PointOriginFor` is satisfied by all [`PointOrigin`](#PointOrigin) types whose quantity type
is implicitly convertible from quantity specification `V` (i.e. `V` satisfies
[`QuantitySpecOf<T::quantity_spec>`](#QuantitySpecOf)).

??? abstract "Examples"

    `si::ice_point` can serve as a point origin for _points_ of `isq::Celsius_temperature` because this
    quantity type implicitly converts to `isq::thermodynamic_temperature`.

    However, if we define `mean_sea_level` in the following way:

    ```cpp
    inline constexpr struct mean_sea_level final : absolute_point_origin<isq::altitude> {} mean_sea_level;
    ```

    then it can't be used as a point origin for _points_ of `isq::length` or `isq::width` as none of them
    is implicitly convertible to `isq::altitude`:

    - not every _length_ is an _altitude_,
    - _width_ is not compatible with _altitude_.


## `QuantityPoint<T>` { #QuantityPoint }

`QuantityPoint` is satisfied by all types that are or derive from `quantity_point<...>`.


### `QuantityPointOf<T, V>` { #QuantityPointOf }

`QuantityPointOf` concept is satisfied by all the quantity points `T` that match the
following value `V`:

| `V`            | Condition                                                                                     |
|----------------|-----------------------------------------------------------------------------------------------|
| `QuantitySpec` | The quantity point quantity specification satisfies [`ReferenceOf<V>`](#ReferenceOf) concept. |
| `PointOrigin`  | The _point_ and `V` have the same absolute point origin.                                      |


### `QuantityPointLike<T>` { #QuantityPointLike }

`QuantityPointLike` concept provides interoperability with other libraries and is satisfied by a type `T`
for which an instantiation of `quantity_point_like_traits` type trait yields a valid type that provides:

- `reference` static data member that matches the [`Reference`](#Reference) concept.
- `point_origin` static data member that matches the [`PointOrigin`](#PointOrigin) concept.
- `rep` type that matches [`RepresentationOf`](#RepresentationOf) concept with the character provided
  in `reference`.
- `explicit_import` static data member convertible to `bool` that specifies that the conversion
  from `T` to a `quantity_point` type should happen explicitly (if `true`),
- `explicit_export` static data member convertible to `bool` that specifies that the conversion
  from a `quantity_point` type to `T` should happen explicitly (if `true`),
- `to_numerical_value(T)` static member function returning a raw value of the quantity being the offset
  of the point from the origin,
- `from_numerical_value(rep)` static member function returning `T`.


??? abstract "Examples"

    This is how support for a `std::chrono::time_point` of `std::chrono::seconds` can be provided:

    ```cpp
    template<typename C>
    struct mp_units::quantity_point_like_traits<std::chrono::time_point<C, std::chrono::seconds>> {
      static constexpr auto reference = si::second;
      static constexpr struct point_origin_ final : absolute_point_origin<isq::time> {} point_origin{};
      static constexpr bool explicit_import = false;
      static constexpr bool explicit_export = false;
      using rep = std::chrono::seconds::rep;
      using T = std::chrono::time_point<C, std::chrono::seconds>;

      [[nodiscard]] static constexpr rep to_numerical_value(const T& tp)
      {
        return tp.time_since_epoch().count();
      }

      [[nodiscard]] static constexpr T from_numerical_value(const rep& v)
      {
        return T(std::chrono::seconds(v));
      }
    };

    quantity_point qp = time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
    std::chrono::sys_seconds q = qp + 42 * s;
    ```
