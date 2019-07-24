# `units` - Physical Units Library for C++

## Summary

`Units` is a compile-time enabled Modern C++ library that provides compile-time dimensional
analysis and unit/quantity manipulation. The basic idea and design heavily bases on
`std::chrono::duration` and extends it to work properly with many dimensions. 

Here is a small example of possible operations:

```cpp
// simple numeric operations
static_assert(10km / 2 == 5km);

// unit conversions
static_assert(1h == 3600s);
static_assert(1km + 1m == 1001m);

// dimension conversions
static_assert(1km / 1s == 1000mps);
static_assert(2kmph * 2h == 4km);
static_assert(2km / 2kmph == 1h);

static_assert(1000 / 1s == 1kHz);

static_assert(10km / 5km == 2);
```


## Approach

1. Safety and performance
  - strong types
  - compile-time safety
  - `constexpr` all the things
2. The best possible user experience
  - compiler errors
  - debugging
3. No macros in the user interface
4. Easy extensibility
5. No external dependencies
6. Possibility to be standardized as a freestanding part of the C++ Standard Library 


## Overview

The library framework consists of a few concepts: quantities, units, dimensions and their exponents. From the user's
point of view the most important is a quantity.

Quantity is a concrete amount of a unit for a specified dimension with a specific representation:

```cpp
units::quantity<units::kilometer, double> d1(123);
auto d2 = 123km;    // stde::units::quantity<units::kilometer, std::int64_t>
```

There are C++ concepts provided for each such quantity type:

```cpp
template<typename T>
concept Length = Quantity<T> && std::Same<typename T::dimension, dimension_length>;
```

With that we can easily write a function template like this:

```cpp
constexpr stde::units::Velocity auto avg_speed(units::Length auto d,stde::units::Time auto t)
{
  return d / t;
}
```

## Basic Concepts

### `Dimensions`

`units::dimension` is a type-list like type that stores an ordered list of exponents of one
or more base dimensions:

```cpp
template<Exponent... Es>
struct dimension : downcast_base<dimension<Es...>> {};
```

`units::Dimension` is a Concept that is satisfied by a type that is empty and publicly
derived from `units::dimension` class template: 

```cpp
template<typename T>
concept Dimension =
    std::is_empty_v<T> &&
    detail::is_dimension<downcast_from<T>>; // exposition only
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
constexpr Velocity auto v1 = 1_m / 1s;
constexpr Velocity auto v2 = 2 / 2s * 1m;

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
  using type = downcasting_traits_t<merge_dimension_t<dimension<E1...>, dimension<E2...>>>;
};

template<Dimension D1, Dimension D2>
using dimension_multiply_t = dimension_multiply<typename D1::base_type, typename D2::base_type>::type;
```

Example implementation of `merge_dimension` may look like:

```cpp
template<Dimension D1, Dimension D2>
struct merge_dimension {
  using type = detail::dim_consolidate_t<mp::type_list_merge_sorted_t<D1, D2, exp_dim_id_less>>;
};
```


### `Units`

`units::unit` is a class template that expresses the unit of a specific physical dimension:

```cpp
template<Dimension D, Ratio R>
  requires (R::num > 0)
struct unit : downcast_base<unit<D, R>> {
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
    detail::is_unit<downcast_from<T>>;  // exposition only
```

### `Quantities`

`units::quantity` is a class template that expresses the quantity/amount of a specific dimension 
expressed in a specific unit of that dimension:

```cpp
template<Unit U, Scalar Rep>
class quantity;
```

`units::Quantity` is a Concept that is satisfied by a type that is an instantiation of `units::quantity`
class template:

```cpp
template<typename T>
concept Quantity =
    detail::is_quantity<T>;  // exposition only
```

`units::quantity` provides the interface really similar to `std::chrono::duration`. The difference is that
it uses `double` as a default representation and has a few additional member types and functions as below:

```cpp
template<Unit U, Scalar Rep = double>
class quantity {
public:
  using unit = U;
  using rep = Rep;
  using dimension = U::dimension;

  [[nodiscard]] static constexpr quantity one() noexcept { return quantity(quantity_values<Rep>::one()); }

  template<Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
      requires treat_as_floating_point<decltype(lhs.count() * rhs.count())> ||
               (std::ratio_multiply<typename U1::ratio, typename U2::ratio>::den == 1)
  [[nodiscard]] constexpr Quantity operator*(const quantity<U1, Rep1>& lhs,
                                             const quantity<U2, Rep2>& rhs);

  template<Scalar Rep1, typename U, typename Rep2>
  [[nodiscard]] constexpr Quantity operator/(const Rep1& v,
                                             const quantity<U, Rep2>& q)

  template<Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
    requires std::Same<typename U1::dimension, typename U2::dimension>
  [[nodiscard]] constexpr Scalar operator/(const quantity<U1, Rep1>& lhs,
                                           const quantity<U2, Rep2>& rhs);

  template<Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
    requires (!std::Same<typename U1::dimension, typename U2::dimension>) &&
             (treat_as_floating_point<decltype(lhs.count() / rhs.count())> ||
              (ratio_divide<typename U1::ratio, typename U2::ratio>::den == 1))
  [[nodiscard]] constexpr Quantity operator/(const quantity<U1, Rep1>& lhs,
                                             const quantity<U2, Rep2>& rhs);

  // ...
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

## Strong types instead of aliases, and type downcasting capability

Most of the important design decisions in the library are dictated by the requirement of providing
the best user experience as possible.

For example with template aliases usage the following code:

```cpp
const Velocity auto t = 20s;
```

could generate a following compile time error:

```text
<path>\example\example.cpp:39:22: error: deduced initializer does not satisfy placeholder constraints
   const Velocity auto t = 20s;
                           ^~~~
In file included from <path>\example\example.cpp:23:
<path>/src/include/units/si/velocity.h:41:16: note: within 'template<class T> concept const bool stde::units::Velocity<T> [with T = stde::units::quantity<units::unit<units::dimension<units::exp<units::base_dim_time, 1> >, std::ratio<1> >, long long int>]'
   concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
           ^~~~~~~~
In file included from <path>/src/include/units/bits/tools.h:25,
                 from <path>/src/include/units/dimension.h:25,
                 from <path>/src/include/units/si/base_dimensions.h:25,
                 from <path>/src/include/units/si/velocity.h:25,
                 from <path>\example\example.cpp:23:
<path>/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool std::Same<T, U> [with T = stde::units::dimension<units::exp<units::base_dim_time, 1> >; U = stde::units::dimension<units::exp<units::base_dim_length, 1>,stde::units::exp<units::base_dim_time, -1> >]'
     concept Same = std::is_same_v<T, U>;
             ^~~~
<path>/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
```

Time and velocity are not that complicated dimensions and there are much more complicated dimensions
out there, but even for those dimensions

```text
[with T = stde::units::quantity<units::unit<units::dimension<units::exp<units::base_dim_time, 1> >, std::ratio<1> >, long long int>]
```

and

```text
[with T = stde::units::dimension<units::exp<units::base_dim_time, 1> >; U = stde::units::dimension<units::exp<units::base_dim_length, 1>,stde::units::exp<units::base_dim_time, -1> >]
```

starts to be really hard to analyze or debug.

That is why it was decided to provide automated downcasting capability when possible. With that the
same code will result with such an error:

```text
<path>\example\example.cpp:40:22: error: deduced initializer does not satisfy placeholder constraints
   const Velocity t = 20s;
                      ^~~~
In file included from <path>\example\example.cpp:23:
<path>/src/include/units/si/velocity.h:48:16: note: within 'template<class T> concept const bool stde::units::Velocity<T> [with T = stde::units::quantity<units::second, long long int>]'
   concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
           ^~~~~~~~
In file included from <path>/src/include/units/bits/tools.h:25,
                 from <path>/src/include/units/dimension.h:25,
                 from <path>/src/include/units/si/base_dimensions.h:25,
                 from <path>/src/include/units/si/velocity.h:25,
                 from <path>\example\example.cpp:23:
<path>/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool std::Same<T, U> [with T = stde::units::dimension_time; U = stde::units::dimension_velocity]'
     concept Same = std::is_same_v<T, U>;
             ^~~~
<path>/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
``` 

Now

```text
[with T = stde::units::quantity<units::second, long long int>]
```

and

```text
[with T = stde::units::dimension_time; U = stde::units::dimension_velocity]
```

are not arguably much easier to understand thus provide better user experience.

Downcasting capability is provided through dedicated `downcasting_traits`, concept, a few helper aliases and by
`base_type` member type in `downcast_base` class template.

```cpp
template<typename BaseType>
struct downcast_base {
  using base_type = BaseType;
};

template<typename T>
concept bool Downcastable =
    requires {
      typename T::base_type;
    } &&
    std::DerivedFrom<T, downcast_base<typename T::base_type>>;

template<Downcastable T>
using downcast_from = T::base_type;

template<Downcastable T>
using downcast_to = std::type_identity<T>;

template<Downcastable T>
struct downcasting_traits : downcast_to<T> {};

template<Downcastable T>
using downcasting_traits_t = downcasting_traits<T>::type;
```

With that the downcasting functionality is enabled by:

```cpp
struct dimension_length : make_dimension_t<exp<base_dim_length, 1>> {};
template<> struct downcasting_traits<downcast_from<dimension_length>> : downcast_to<dimension_length> {};
```

```cpp
struct kilometer : unit<dimension_length, std::kilo> {};
template<> struct downcasting_traits<downcast_from<kilometer>> : downcast_to<kilometer> {};
```


## Adding new derived dimensions

In order to extend the library with custom dimensions the user has to:
1. Create a new dimension type with the recipe of how to construct it from base dimensions and provide
   downcasting trait for it:

```cpp
struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
template<> struct downcasting_traits<downcast_from<dimension_velocity>> : downcast_to<dimension_velocity> {};
``` 

2. Define a concept that will match a new dimension:

```cpp
template<typename T>
concept Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;
```

3. Define units and provide downcasting traits for them:

 - base unit

```cpp
struct meter : unit<dimension_length, std::ratio<1>> {};
template<> struct downcasting_traits<downcast_from<meter>> : downcast_to<meter> {};
```

 - units with prefixes

```cpp
struct kilometer : kilo<meter> {};
template<> struct downcasting_traits<downcast_from<kilometer>> : downcast_to<kilometer> {};
```

 - derived units

```cpp
struct kilometer_per_hour : derived_unit<dimension_velocity, kilometer, hour> {};
template<> struct downcasting_traits<downcast_from<kilometer_per_hour>> : downcast_to<kilometer_per_hour> {};
``` 

5. Provide user-defined literals for the most important units:

```cpp
inline namespace literals {
  constexpr auto operator""_mps(unsigned long long l) { return quantity<meter_per_second, std::int64_t>(l); }
  constexpr auto operator""_mps(long double l)        { return quantity<meter_per_second, long double>(l); }
  
  constexpr auto operator""_kmph(unsigned long long l) { return quantity<kilometer_per_hour, std::int64_t>(l); }
  constexpr auto operator""_kmph(long double l)        { return quantity<kilometer_per_hour, long double>(l); }
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

2. What to do with `time` which is ambiguous (conflict wit ANSI C)?

3. What to do with `std::chrono::duration`?

4. Should we provide `seconds<int>` or stay with `quantity<second, int>`?

5. What is the best way to add support for temperatures?

    Temperature absolute values not only require `std::ratio` but also should be adjusted/shifted
    by some constant values (i.e. [°C] = [K] − 273.15). Relative temperatures does need an offset.
    Users will most probably have problems with differentiating those two. Maybe the best solution
    is to provide only `K` support in quantity and provide non-member helper conversion functions
    with verbose names to convert to `°C` and `°C`?

6. Do we need non-linear scale?

7. Should we provide cmath-like functions for quantities?

8. What should be the resulting type of `auto d = 1km + 1ft;`?

9. Should we require explicit casts (i.e. quantity_cast) between different systems of
   measurement?
    
10. Should we support integral representations?

11. Provide ostream overloads to print quantity units (use `std::format`)?

12. Should we provide support for dimensionless quantities? 

    Because dimensionless quantities have no associated units, they behave as normal scalars,
    and allow implicit conversion to and from the underlying value type or types that are
    convertible to/from that value type.

13. Should we standardize accompany tools (`downcasting_traits`, `type_list` operations, `common_ratio`, etc)? 
     
14. Do we need to support fractional exponents (i.e. `dimension<exp<"length", 2, 3>>` as 2/3)?

15. `k`, `K`, `W`, `F` UDLs conflict with gcc GNU extensions (https://gcc.gnu.org/onlinedocs/gcc-4.3.0/gcc/Fixed_002dPoint.html)
    for floating point types.

16. `J` imaginary constants are a GCC extension
