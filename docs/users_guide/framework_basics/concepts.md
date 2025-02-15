# Concepts

This chapter enumerates all the user-facing concepts in the **mp-units** library.


## `Dimension<T>` { #Dimension }

`Dimension` concept matches a [dimension](../../appendix/glossary.md#dimension) of either a base
or derived [quantity](../../appendix/glossary.md#quantity):

- [Base dimensions](../../appendix/glossary.md#base-dimension) are explicitly defined by the user
  by inheriting from the instantiation of a `base_dimension` class template. It should be instantiated with
  a unique symbol identifier describing this dimension in a specific
  [system of quantities](../../appendix/glossary.md#system-of-quantities).
- [Derived dimensions](../../appendix/glossary.md#derived-dimension) are implicitly created
  by the library's framework based on the [quantity equation](../../appendix/glossary.md#quantity-equation)
  provided in the [quantity specification](../../appendix/glossary.md#quantity_spec).

All of the above dimensions have to be marked as `final`.


### `DimensionOf<T, V>` { #DimensionOf }

`DimensionOf` concept is satisfied when both arguments satisfy a [`Dimension`](#Dimension) concept and
when they compare equal.


## `QuantitySpec<T>` { #QuantitySpec }

`QuantitySpec` concept matches all the [quantity specifications](../../appendix/glossary.md#quantity_spec)
including:

- [Base quantities](../../appendix/glossary.md#base-quantity) defined by a user by inheriting from
  the `quantity_spec` class template instantiated with a [base dimension](../../appendix/glossary.md#base-dimension)
  argument.
- [Derived named quantities](../../appendix/glossary.md#derived-quantity) defined by a user by
  inheriting from the `quantity_spec` class template instantiated with a result of a
  [quantity equation](../../appendix/glossary.md#quantity-equation) passed as an argument.
- Other named quantities forming a [hierarchy of quantities](../../appendix/glossary.md#quantity-hierarchy)
  of the same [kind](../../appendix/glossary.md#kind) defined by a user by inheriting from the
  `quantity_spec` class template instantiated with another "parent" quantity specification passed as an
  argument.
- [Quantity kinds](../../appendix/glossary.md#kind) describing a family of mutually comparable quantities.
- Intermediate [derived quantity](../../appendix/glossary.md#derived-quantity) specifications being
  a result of a [quantity equations](../../appendix/glossary.md#quantity-equation) on other specifications.

All of the above quantity specifications have to be marked as `final`.


### `QuantitySpecOf<T, V>` { #QuantitySpecOf }

`QuantitySpecOf` concept is satisfied when both arguments satisfy a [`QuantitySpec`](#QuantitySpec) concept
and when `T` is implicitly convertible to `V`.


## `UnitMagnitude<T>` { #UnitMagnitude }

`UnitMagnitude` concept is satisfied by all types defining a unit magnitude.

!!! info

    Unit magnitude implementation is a private implementation detail of the library.


## `Unit<T>` { #Unit }

`Unit` concept matches all the [units](../../appendix/glossary.md#unit) in the library including:

- [Base units](../../appendix/glossary.md#base-unit) defined by a user by inheriting from the `named_unit`
  class template instantiated with a unique symbol identifier describing this unit in a specific
  [system of units](../../appendix/glossary.md#system-of-units).
- Named scaled units defined by a user by inheriting from the `named_unit` class template instantiated
  with a unique symbol identifier and a product of multiplying another unit with some magnitude.
- Prefixed units defined by a user by inheriting from the `prefixed_unit` class template instantiated
  with a prefix symbol, a magnitude, and a unit to be prefixed.
- [Derived named units](../../appendix/glossary.md#derived-unit) defined by a user by inheriting from the
  `named_unit` class template instantiated with a unique symbol identifier and a result of
  [unit equation](../../appendix/glossary.md#unit-equation) passed as an argument.
- [Derived unnamed units](../../appendix/glossary.md#derived-unit) being a result of a
  [unit equations](../../appendix/glossary.md#unit-equation) on other units.

All of the above units have to be marked as `final`.

!!! note

    In the **mp-units** library, [physical constants are also implemented as units](faster_than_lightspeed_constants.md).


### `AssociatedUnit<T>` { #AssociatedUnit }

`AssociatedUnit` concept describes a [unit with an associated quantity](../../appendix/glossary.md#associated-unit)
and is satisfied by:

- All units derived from a `named_unit` class template instantiated with a unique symbol identifier
  and a [`QuantitySpec`](#QuantitySpec) of a [quantity kind](../../appendix/glossary.md#kind).
- All units being a result of [unit equations](../../appendix/glossary.md#unit-equation) on other
  associated units.

??? abstract "Examples"

    All units in the [SI](../../appendix/glossary.md#si) have associated quantities. For example,
    `si::second` is specified to measure `isq::time`.

    Natural units typically do not have an associated quantity. For example, if we assume `c = 1`,
    a `natural::second` unit can be used to measure both `time` and `length`. In such case, `speed`
    would have a unit of `one`.


### `PrefixableUnit<T>` { #PrefixableUnit }

`PrefixableUnit` concept is satisfied by all units derived from a `named_unit` class template.
Such units can be passed as an argument to a `prefixed_unit` class template.


### `UnitOf<T, V>` { #UnitOf }

`UnitOf` concept is satisfied for all units `T` for which an associated quantity spec is implicitly
convertible to the provided [`QuantitySpec`](#QuantitySpec) value.


## `Reference<T>` { #Reference }

`Reference` concept is satisfied by all [quantity reference](../../appendix/glossary.md#reference)
types. Such types provide all the meta-information required to create a [`Quantity`](#Quantity).
A `Reference` can either be:

- An [`AssociatedUnit`](#AssociatedUnit).
- The instantiation of a `reference` class template with a [`QuantitySpec`](#QuantitySpec) passed as
  the first template argument and a [`Unit`](#Unit) passed as the second one.


### `ReferenceOf<T, V>` { #ReferenceOf }

`ReferenceOf` concept is satisfied by references `T` which have a quantity specification that satisfies
[`QuantitySpecOf<V>`](#QuantitySpecOf) concept.


### `RepresentationOf<T, V>` { #RepresentationOf }

`RepresentationOf` concept constraints a type of a number that stores the
[value of a quantity](../../appendix/glossary.md#quantity-value) and is satisfied:

- if the type of `V` satisfies [`QuantitySpec`](#QuantitySpec):

    - by all representation types when `V` describes
      a [quantity kind](../../appendix/glossary.md#kind),
    - otherwise, by representation types that are of
      a [quantity character](../../appendix/glossary.md#character) associated with a provided
      quantity specification `V`.

- if `V` is of `quantity_character` type:

    - by representation types that are of a provided
      [quantity character](../../appendix/glossary.md#character).


## `Quantity<T>` { #Quantity }

`Quantity` concept matches every [quantity](../../appendix/glossary.md#quantity) in the library and is
satisfied by all types being or deriving from an instantiation of a `quantity` class template.


### `QuantityOf<T, V>` { #QuantityOf }

`QuantityOf` concept is satisfied by all the quantities for which a [`ReferenceOf<V>`](#ReferenceOf)
is `true`.


### `QuantityLike<T>` { #QuantityLike }

`QuantityLike` concept provides interoperability with other libraries and is satisfied by a type `T`
for which an instantiation of `quantity_like_traits` type trait yields a valid type that provides:

- `reference` static data member that matches the [`Reference`](#Reference) concept,
- `rep` type that matches [`RepresentationOf`](#RepresentationOf) concept with the character provided
  in `reference`,
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

`PointOrigin` concept matches all [quantity point origins](../../appendix/glossary.md#point-origin) in
the library. It is satisfied by either:

- All types derived from an `absolute_point_origin` class template.
- All types derived from a `relative_point_origin` class template.


### `PointOriginFor<T, V>` { #PointOriginFor }

`PointOriginFor` concept is satisfied by all [`PointOrigin`](#PointOrigin) types that have quantity type
implicitly convertible from quantity specification `V`, which means that `V` must satisfy
[`QuantitySpecOf<T::quantity_spec>`](#QuantitySpecOf).

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

`QuantityPoint` concept is satisfied by all types being either a specialization or derived from `quantity_point`
class template.


### `QuantityPointOf<T, V>` { #QuantityPointOf }

`QuantityPointOf` concept is satisfied by all the quantity points `T` that match the following value `V`:

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
