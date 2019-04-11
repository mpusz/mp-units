# `units` - Physical Units Library for C++

## Summary

`Units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions. 

Here is a small example of possible operations:

```cpp
// simple numeric operations
static_assert(10_km / 2 == 5_km);

// unit conversions
static_assert(1_h == 3600_s);
static_assert(1_km + 1_m == 1001_m);

// dimension conversions
static_assert(1_km / 1_s == 1000_mps);
static_assert(2_kmph * 2_h == 4_km);
static_assert(2_km / 2_kmph == 1_h);

static_assert(1000 / 1_s == 1_kHz);

static_assert(10_km / 5_km == 2);
```


## Requirements

1. Safety and performance
  - strong types
  - template metaprogramming
  - `constexpr` all the things
2. The best possible user experience
  - compiler errors
  - debugging
3. No macros in the user interface
4. Easy extensibility
5. No external dependencies
6. Possibility to be standardized as a freestanding part of the C++ Standard Library 


## Basic Concepts

### `Dimensions`

`units::dimension` is a type-list like type that stores an ordered list of exponents of one
or more base dimensions:

```cpp
template<Exponent... Es>
struct dimension : upcast_base<dimension<Es...>> {};
```

`units::Dimension` is a Concept that is satisfied by a type that is empty and publicly
derived from `units::dimension` class template: 

```cpp
template<typename T>
concept Dimension =
    std::is_empty_v<T> &&
    detail::is_dimension<upcast_from<T>>; // exposition only
```

#### `Exponents`

`units::exp` provides an information about a single base dimension and its exponent in a derived
dimension:

```cpp
template<typename BaseDimension, int Value>
struct exp {
  using dimension = BaseDimension;
  static constexpr int value = Value;
};
```

where `BaseDimension` is a unique sortable compile-time value and for now is implemented as:

```cpp
template<int UniqueValue>
using dim_id = std::integral_constant<int, UniqueValue>;
```

but it is meant to be replaced with C++20 class `constexpr` values provided as non-type template
parameters (when feature will be available in a compiler) so that for example base dimension for
length will be expressed as `dimension<exp<"length", 1>>`.

`units::Exponent` concept is satisfied if provided type is an instantiation of `units::exp` class
template:

```cpp
template<typename T>
concept Exponent =
    detail::is_exp<T>;  // exposition only
```

#### `make_dimension`

Above design of dimensions is created with the ease of use for end users in mind. Compile-time
errors should provide as short as possible template instantiations strings that should be easy to
understand by every engineer. Also types visible in a debugger should be easy to understand.
That is why `units::dimension` type for derived dimensions always stores information about only
those base dimensions that are used to form that derived dimension.

However, such an approach have some challenges:

```cpp
constexpr Velocity auto v1 = 1_m / 1_s;
constexpr Velocity auto v2 = 2 / 2_s * 1_m;

static_assert(std::Same<decltype(v1), decltype(v2)>);
static_assert(v1 == v2);
``` 

Above code, no matter what is the order of the base dimensions in an expression forming our result,
must produce the same `Velocity` type so that both values can be easily compared. In order to achieve
that, `dimension` class templates should never be instantiated manually but through a `make_dimension_t`
template metaprogramming factory function:

```cpp
template<Exponent... Es>
struct make_dimension {
  using type = /* unspecified */;
};

template<Exponent... Es>
using make_dimension_t = make_dimension<Es...>::type;
``` 

So for example to create a `dimension_velocity` type we have to do:

```cpp
struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
```

In order to make `make_dimension_t` work as expected it has to provide unique ordering for
contained base dimensions. Beside providing ordering to base dimensions it also has to:
- aggregate two arguments of the same base dimension but different exponents
- eliminate two arguments of the same base dimension and with opposite equal exponents

Additionally, it would be good if the final type produced by `make_dimension_t` would be easy to
understand for the user. For example we may decide to order base dimensions with decreasing order of
their exponents. That is why second sorting of a type list may be required. For example:

```cpp
template<Exponent... Es>
struct make_dimension {
  using type = mp::type_list_sort_t<detail::dim_consolidate_t<mp::type_list_sort_t<dimension<Es...>, exp_dim_id_less>>, exp_greater_equal>;
};
```


#### `merge_dimension`

`units::merge_dimension` is similar to `make_dimension` but instead of sorting the whole list
of base dimensions from scratch it assumes that provided input `dimension` types are already
sorted as a result of `make_dimension`.

Typical use case for `merge_dimension` is to produce final `dimension` return type of multiplying
two different dimensions:

```cpp
template<Dimension D1, Dimension D2>
struct dimension_multiply;

template<Exponent... E1, Exponent... E2>
struct dimension_multiply<dimension<E1...>, dimension<E2...>> {
  using type = upcasting_traits_t<merge_dimension_t<dimension<E1...>, dimension<E2...>>>;
};

template<Dimension D1, Dimension D2>
using dimension_multiply_t = dimension_multiply<typename D1::base_type, typename D2::base_type>::type;
```

Example implementation of `merge_dimension` may look like:

```cpp
template<Dimension D1, Dimension D2>
struct merge_dimension {
  using type = mp::type_list_sort_t<detail::dim_consolidate_t<mp::type_list_merge_sorted_t<D1, D2, exp_dim_id_less>>, exp_greater_equal>;
};
```


### `Units`

`units::unit` is a class template that expresses the unit of a specific physical dimension:

```cpp
template<Dimension D, Ratio R>
  requires (R::num > 0)
struct unit : upcast_base<unit<D, R>> {
  using dimension = D;
  using ratio = R;
};
```

`units::Unit` is a Concept that is satisfied by a type that is empty and publicly
derived from `units::unit` class template: 

```cpp
template<typename T>
concept Unit =
    std::is_empty_v<T> &&
    detail::is_unit<upcast_from<T>>;  // exposition only
```

### `Quantities`

`units::quantity` is a class template that expresses the quantity/amount of a specific dimension 
expressed in a specific unit of that dimension:

```cpp
template<Dimension D, Unit U, Number Rep>
  requires std::Same<D, typename U::dimension>
class quantity;
```

`units::Quantity` is a Concept that is satisfied by a type that is an instantiation of `units::quantity`
class template:

```cpp
template<typename T>
concept Quantity =
    detail::is_quantity<T>;  // exposition only
```

`units::quantity` provides the interface really similar to `std::chrono::duration` with additional
member types and functions as below:

```cpp
template<Dimension D, Unit U, Number Rep>
    requires std::Same<D, typename U::dimension>
class quantity {
public:
  using dimension = D;
  using unit = U;

  template<Dimension D1, Unit U1, Number Rep1, Dimension D2, Unit U2, Number Rep2>
      requires treat_as_floating_point<std::common_type_t<Rep1, Rep2>> || std::ratio_multiply<typename U1::ratio, typename U2::ratio>::den == 1
  quantity<dimension_multiply_t<D1, D2>, upcasting_traits_t<unit<dimension_multiply_t<D1, D2>, std::ratio_multiply<typename U1::ratio, typename U2::ratio>>>, std::common_type_t<Rep1, Rep2>>
  constexpr operator*(const quantity<D1, U1, Rep1>& lhs,
                      const quantity<D2, U2, Rep2>& rhs);

  template<Number Rep1, Dimension D, Unit U, Number Rep2>
  quantity<dim_invert_t<D>, upcasting_traits_t<unit<dim_invert_t<D>, std::ratio<U::ratio::den, U::ratio::num>>>, std::common_type_t<Rep1, Rep2>>
  constexpr operator/(const Rep1& v,
                      const quantity<D, U, Rep2>& q) [[expects: q != quantity<D, U, Rep2>(0)]];

  template<Dimension D1, Unit U1, Number Rep1, Dimension D2, Unit U2, Number Rep2>
      requires treat_as_floating_point<std::common_type_t<Rep1, Rep2>> || std::ratio_divide<typename U1::ratio, typename U2::ratio>::den == 1
  quantity<dimension_divide_t<D1, D2>, upcasting_traits_t<unit<dimension_divide_t<D1, D2>, std::ratio_divide<typename U1::ratio, typename U2::ratio>>>, std::common_type_t<Rep1, Rep2>>
  constexpr operator/(const quantity<D1, U1, Rep1>& lhs,
                      const quantity<D2, U2, Rep2>& rhs) [[expects: rhs != quantity<D, U2, Rep2>(0)]];
};
```

Additional functions provide the support for operations that result in a different dimension type
than those of their arguments.

Another change comparing to `std::chrono::duration` is that the `duration` is using
`std::common_type_t<Rep1, Rep2>` to find a common representation for a calculation result. Such
a design was reported as problematic by numerics study group members as sometimes we want to provide
a different type in case of multiplication and different in case of division. `std::common_type` lacks
that additional information. That is why `units::quantity` uses the resulting type of a concrete operator
operation and provides it directly to `units::common_quantity_t` type trait.

#### `quantity_cast`

To explicitly force truncating conversions `quantity_cast` function is provided which is a direct
counterpart of `std::chrono::duration_cast`.

## Strong types instead of aliases, and type upcasting capability

Most of the important design decisions in the library are dictated by the requirement of providing
the best user experience as possible.

For example with template aliases usage the following code:

```cpp
const Velocity t = 20_s;
```

could generate a following compile time error:

```text
C:\repos\units\example\example.cpp:39:22: error: deduced initializer does not satisfy placeholder constraints
   const Velocity t = 20_s;
                      ^~~~
In file included from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/si/velocity.h:41:16: note: within 'template<class T> concept const bool units::Velocity<T> [with T = units::quantity<units::dimension<units::exp<units::base_dim_time, 1> >, units::unit<units::dimension<units::exp<units::base_dim_time, 1> >, std::ratio<1> >, long long int>]'
   concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
           ^~~~~~~~
In file included from C:/repos/units/src/include/units/bits/tools.h:25,
                 from C:/repos/units/src/include/units/dimension.h:25,
                 from C:/repos/units/src/include/units/si/base_dimensions.h:25,
                 from C:/repos/units/src/include/units/si/velocity.h:25,
                 from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool std::Same<T, U> [with T = units::dimension<units::exp<units::base_dim_time, 1> >; U = units::dimension<units::exp<units::base_dim_length, 1>, units::exp<units::base_dim_time, -1> >]'
     concept Same = std::is_same_v<T, U>;
             ^~~~
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
```

Time and velocity are not that complicated dimensions and there are much more complicated dimensions
out there, but even for those dimensions

```text
[with T = units::quantity<units::dimension<units::exp<units::base_dim_time, 1> >, units::unit<units::dimension<units::exp<units::base_dim_time, 1> >, std::ratio<1> >, long long int>]
```

and

```text
[with T = units::dimension<units::exp<units::base_dim_time, 1> >; U = units::dimension<units::exp<units::base_dim_length, 1>, units::exp<units::base_dim_time, -1> >]
```

starts to be really hard to analyze or debug.

That is why it was decided to provide automated upcasting capability when possible. With that the
same code will result with such an error:

```text
C:\repos\units\example\example.cpp:40:22: error: deduced initializer does not satisfy placeholder constraints
   const Velocity t = 20_s;
                      ^~~~
In file included from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/si/velocity.h:48:16: note: within 'template<class T> concept const bool units::Velocity<T> [with T = units::quantity<units::dimension_time, units::second, long long int>]'
   concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
           ^~~~~~~~
In file included from C:/repos/units/src/include/units/bits/tools.h:25,
                 from C:/repos/units/src/include/units/dimension.h:25,
                 from C:/repos/units/src/include/units/si/base_dimensions.h:25,
                 from C:/repos/units/src/include/units/si/velocity.h:25,
                 from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool std::Same<T, U> [with T = units::dimension_time; U = units::dimension_velocity]'
     concept Same = std::is_same_v<T, U>;
             ^~~~
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
``` 

Now

```text
[with T = units::quantity<units::dimension_time, units::second, long long int>]
```

and

```text
[with T = units::dimension_time; U = units::dimension_velocity]
```

are not arguably much easier to understand thus provide better user experience.

Upcasting capability is provided through dedicated `upcasting_traits`, a few helper aliases and by
`base_type` member type in `upcast_base` class template.

```cpp
template<Upcastable T>
using upcast_from = T::base_type;

template<typename T>
using upcast_to = std::type_identity<T>;

template<typename T>
struct upcasting_traits : upcast_to<T> {};

template<typename T>
using upcasting_traits_t = upcasting_traits<T>::type;
```

With that the upcasting functionality is enabled by:

```cpp
struct dimension_length : make_dimension_t<exp<base_dim_length, 1>> {};
template<> struct upcasting_traits<upcast_from<dimension_length>> : upcast_to<dimension_length> {};
```

```cpp
struct kilometer : unit<dimension_length, std::kilo> {};
template<> struct upcasting_traits<upcast_from<kilometer>> : upcast_to<kilometer> {};
```


## Adding new derived dimensions

In order to extend the library with custom dimensions the user has to:
1. Create a new dimension type with the recipe of how to construct it from base dimensions and provide
   upcasting trait for it:

```cpp
struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
template<> struct upcasting_traits<upcast_from<dimension_velocity>> : upcast_to<dimension_velocity> {};
``` 

2. Provide `quantity` class template partial specialization for new dimension and provide its base type:

```cpp
template<Unit U = struct meter_per_second, Number Rep = double>
using velocity = quantity<dimension_velocity, U, Rep>;
``` 

3. Define a concept that will match a new dimension:

```cpp
template<typename T>
concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
```

4. Define units and provide upcasting traits for them:

 - base unit

```cpp
struct meter : unit<dimension_length, std::ratio<1>> {};
template<> struct upcasting_traits<upcast_from<meter>> : upcast_to<meter> {};
```

 - units with prefixes

```cpp
struct kilometer : kilo<meter> {};
template<> struct upcasting_traits<upcast_from<kilometer>> : upcast_to<kilometer> {};
```

 - derived units

```cpp
struct kilometer_per_hour : derived_unit<dimension_velocity, kilometer, hour> {};
template<> struct upcasting_traits<upcast_from<kilometer_per_hour>> : upcast_to<kilometer_per_hour> {};
``` 

5. Provide user-defined literals for the most important units:

```cpp
inline namespace literals {
  constexpr auto operator""_mps(unsigned long long l) { return velocity<meter_per_second, std::int64_t>(l); }
  constexpr auto operator""_mps(long double l)        { return velocity<meter_per_second, long double>(l); }
  
  constexpr auto operator""_kmph(unsigned long long l) { return velocity<kilometer_per_hour, std::int64_t>(l); }
  constexpr auto operator""_kmph(long double l) { return velocity<kilometer_per_hour, long double>(l); }
}
```


## Adding new base dimensions

For now base dimensions are defined in terms of `std::integral_constant<int, ...>` and the provided
values must be unique. For example:

```cpp
struct base_dim_length : dim_id<0> {};
struct base_dim_mass : dim_id<1> {};
struct base_dim_time : dim_id<2> {};
struct base_dim_electric_current : dim_id<3> {};
struct base_dim_temperature : dim_id<4> {};
struct base_dim_amount_of_substance : dim_id<5> {};
struct base_dim_luminous_intensity : dim_id<6> {};
```

However, as soon as C++20 class type values will be supported as non-type template parameters
base dimensions will be just a text values. For example:

```cpp
inline constexpr base_dim base_dim_length = "length";
```

With that it should be really easy to add support for any new non-standard base units to the
library without the risk of collision with any dimension type defined by the library itself or
by other users extending the library with their own dimension types.

Additionally, it should make the error logs even shorter thus easier to understand.


## Open questions

1. Should we ensure that dimension is always a result of `make_dimension`? How to do it?

2. Should we provide strong types and upcasting_traits for `quantity` type?

    In such a case all the operators have to be provided to a child class. Or maybe use CRTP? 

3. What to do with `time` which is ambiguous (conflict wit ANSI C)?

4. What to do with `std::chrono::duration`? Is it possible to make it derive from
  `quantity<dimension_time, U, Rep>` which will most probably an ABI break? Alternatively,
  should we provide specialization of `quantity<dimension_time, U, Rep>` to work with/covnert
  from/to `std::duration`?

5. Should we provide `seconds<int>` or stay with `time<second, int>`? What about CTAD problem
   for `units::length<units::mile> d3(3);`?

6. What is the best way to add support for temperatures?

    Temperatures not only require `std::ratio` but also should be adjusted/shifted by some
    constant values (i.e. [°C] = [K] − 273.15).

7. Should we use `units::multiply` or stay with `std::ratio` for multiplication?

8. Should we consider making `units::multiply` and `units::offset` a non-class template parameters
    as they provide different ratio values rather than types?

    In example instead:
 
    ```cpp
    struct celsius    : unit<dimension_temperature, convert<offset<-27315, 100>>> {};
    ```
    
    we could think about something like:
 
    ```cpp
    struct celsius    : unit<dimension_temperature, kelvin() - 27315/100>>> {};
    ```

9. Do we need non-linear scale?

10. Should we provide cmath-like functions for quantities?

11. What should be the resulting type of `auto d = 1_km + 1_ft;`?

12. Should we require explicit casts (i.e. quantity_cast) between different systems of
    measurement?
    
13. Should we provide Boost-like support for a `quantity_cast` to a reference that allows
    direct access to the underlying value of a quantity variable?

14. What should be the default representation (integral or `double`)?

15. Provide ostream overloads to print quantity units (use `std::format`)?

16. Should we provide support for dimensionless quantities? 

    Because dimensionless quantities have no associated units, they behave as normal scalars,
    and allow implicit conversion to and from the underlying value type or types that are
    convertible to/from that value type.

17. Should we leave `quantity` and specific dimensions as
    ```cpp
    template<Dimension D, Unit U, Number Rep>
        requires std::Same<D, typename U::dimension>
    class quantity;
    
    template<Unit U = meter_per_second, Number Rep = double>
    using velocity = quantity<dimension_velocity, U, Rep>;
    
    units::velocity<units::kilometer_per_hour> kmph = avg_speed(d, t);
    ```
    or maybe we should leave the dimension only in unit
    ```cpp
    template<Unit U, Number Rep>
    class quantity;
    
    units::quantity<units::kilometer_per_hour> kmph = avg_speed(d, t);
    ```
    which will simplify the design and shorten compile time errors but possibly will add
    more ambiguity to some cases. For example when using CTAD:
    ```cpp
    units::velocity kmph = avg_speed(d, t);
    ```
    vs
    ```cpp
    units::quantity kmph = avg_speed(d, t);
    ```
    It would be also incopatible with concepts named i.e. `Velocity`.

18. Should we standardize accompany tools (`type_list` operations, `static_sign`, `static_abs`,
    `static_gcd`, `common_ratio`)? 
     
19. Do we need to support fractional exponents (i.e. `dimension<exp<"length", 2, 3>>` as 2/3)? 

20. implicit conversion of quantity<Unit,Y> to quantity<Unit,Z> is allowed if Y and Z are implicitly convertible.
    assignment between quantity<Unit,Y> and quantity<Unit,Z> is allowed if Y and Z are implicitly convertible.

21. explicit conversion between quantity<Unit1,Y> and quantity<Unit2,Z> is allowed if Unit1 and Unit2 have the same dimensions and if Y and Z are implicitly convertible.
    implicit conversion between quantity<Unit1,Y> and quantity<Unit2,Z> is allowed if Unit1 reduces to exactly the same combination of base units as Unit2 and if Y and Z are convertible.

