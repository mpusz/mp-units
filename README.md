# `units` - Physical Units Library for C++

## Summary

`Units` is a compile-time friendly Modern C++ library that provides support for converting units.
The basic idea and design heavily bases on `std::chrono::duration` extending it to work properly
with many dimensions. 

Here is a small example of possible conversions:

```cpp
static_assert(1000 / 1_s == 1_kHz);
static_assert(1_h == 3600_s);
static_assert(1_km + 1_m == 1001_m);
static_assert(10_km / 5_km == 2);
static_assert(10_km / 2 == 5_km);
static_assert(1_km / 1_s == 1000_mps);
static_assert(2_kmph * 2_h == 4_km);
static_assert(2_km / 2_kmph == 1_h);
```


## Basic Concepts

### `Dimensions`

`units::dimension` is a type-list like type that stores an ordered list of exponents of one
or more base dimensions:

```cpp
template<Exponent... Es>
struct dimension {
  using base_type = dimension<Es...>;
};
```

`units::Dimension` is a Concept that is satisfied by a type that is empty and publicly
derived from `units::dimension` class template: 

```cpp
template<typename T>
concept bool Dimension =
    std::is_empty_v<T> &&
    detail::is_dimension<typename T::base_type> &&
    DerivedFrom<T, typename T::base_type>;
```

#### `Exponents`

`units::exp` provides an information about single base dimension and its exponent in a derived
dimension:

```cpp
template<typename BaseDimension, int Value>
struct exp {
  using dimension = BaseDimension;
  static constexpr int value = Value;
};
```

where `BaseDimension` for now is:

```cpp
template<int UniqueValue>
using dim_id = std::integral_constant<int, UniqueValue>;
```

but it is meant to be replaced with C++20 class constexpr values provided as non-type template
parameters (when feature will be available in a compiler) so that for example base dimension for
length will be expressed as `dimension<exp<"length", 1>>`.

`units::Exponent` concept is satisfied if provided type is an instantiation of `units::exp` class
template:

```cpp
template<typename T>
concept bool Exponent = detail::is_exp<T>;
```

#### `make_dimension`

Above design of dimensions is created with the ease of use for end users in mind. Compile-time
errors should provide as short as possible template instantiations strings that should be easy to
understand by every C++ programmer. Also types visible in debugger should be easy to understand.
That is why `units::dimension` type for derived dimensions always stores information about only
those base dimensions that are used to form that derived dimension.

However, such an approach have some challenges:

```cpp
constexpr Velocity auto v1 = 1_m / 1_s;
constexpr Velocity auto v2 = 2 / 2_s * 1_m;

static_assert(Same<decltype(v1), decltype(v2)>);
static_assert(v1 == v2);
``` 

Above code, no matter what is the order of the base dimensions in an expression forming our result,
must produce the same `Velocity` type so that both values can be easily compared. To achieve that
dimension class templates should never be instantiated manually but through a `make_dimension_t`
template metaprogramming factory function:

```cpp
template<Exponent... Es>
struct make_dimension {
  using type = /* unspecified */;
};

template<Exponent... Es>
using make_dimension_t = typename make_dimension<Es...>::type;
``` 

So for example to create a `dimension_velocity` type we have to do:

```cpp
struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
```

Also for example to return the result of multiplying two different dimensions we have to
create a final dimension type using:

```cpp
template<Dimension D1, Dimension D2>
struct dimension_multiply;

template<Exponent... E1, Exponent... E2>
struct dimension_multiply<dimension<E1...>, dimension<E2...>> {
  using type = upcasting_traits_t<make_dimension_t<E1..., E2...>>;
};

template<Dimension D1, Dimension D2>
using dimension_multiply_t = typename dimension_multiply<typename D1::base_type, typename D2::base_type>::type;
```

In order to make `make_dimension_t` work as expected it has to provide unique ordering for
contained base dimensions. Beside providing ordering to base dimensions it also has to:
- aggregate two arguments of the same base dimension but different exponents
- eliminate two arguments of the same base dimension and with opposite equal exponents

Additionally, it would be good if the final type produced by `make_dimension_t` would be easy to
understand by the user, so for example base dimensions could be sorted with decreasing order of
their exponents. That is why second sorting of a type list may be required, for example:

```cpp
template<Exponent... Es>
struct make_dimension {
  using type = mp::type_list_sort_t<detail::dim_consolidate_t<mp::type_list_sort_t<dimension<Es...>, exp_dim_id_less>>, exp_greater_equal>;
};
```

### `Units`

`units::unit` is a class template that expresses the unit of a specific dimension:

```cpp
template<Dimension D, Ratio R>
  requires (R::num > 0)
struct unit {
  using base_type = unit<D, R>;
  using dimension = D;
  using ratio = R;
};
```

`units::Unit` is a Concept that is satisfied by a type that is empty and publicly
derived from `units::unit` class template: 

```cpp
template<typename T>
concept bool Unit =
    std::is_empty_v<T> &&
    detail::is_unit<typename T::base_type> &&
    DerivedFrom<T, typename T::base_type>;
```

### `Quantities`

`units::quantity` is a class template that expresses the quantity/amount of a specific dimension 
expressed in a specific unit of that dimension:

```cpp
template<Dimension D, Unit U, Number Rep>
  requires Same<D, typename U::dimension>
class quantity;
```

`units::Unit` is a Concept that is satisfied by a type that is a specialization of `units::quantity`
class template:

```cpp
template<typename T>
concept bool Quantity = detail::is_quantity<T>;
```

`units::quantity` provides the interface really similar to `std::chrono::duration` with additional
member types and functions as below:

```cpp
template<Dimension D, Unit U, Number Rep>
    requires Same<D, typename U::dimension>
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
                      const quantity<D, U, Rep2>& q);

  template<Dimension D1, Unit U1, Number Rep1, Dimension D2, Unit U2, Number Rep2>
      requires treat_as_floating_point<std::common_type_t<Rep1, Rep2>> || std::ratio_divide<typename U1::ratio, typename U2::ratio>::den == 1
  quantity<dimension_divide_t<D1, D2>, upcasting_traits_t<unit<dimension_divide_t<D1, D2>, std::ratio_divide<typename U1::ratio, typename U2::ratio>>>, std::common_type_t<Rep1, Rep2>>
  constexpr operator/(const quantity<D1, U1, Rep1>& lhs,
                      const quantity<D2, U2, Rep2>& rhs);
};
```

Additional functions provide the support for operations that result in a different dimension type
than those of their arguments.

#### `quantity_cast`

To explicitly force truncating conversions `quantity_cast` function is provided which is a direct
counterpart of `std::chrono::duration_cast`.

## Strong types instead of aliases and type upcasting capability

Most of the important design decisions in the library are dictated by the requirement of providing
the best user experience as possible.

For example the following code:

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
   concept bool Velocity = Quantity<T> && Same<typename T::dimension, dimension_velocity>;
                ^~~~~~~~
In file included from C:/repos/units/src/include/units/bits/tools.h:25,
                 from C:/repos/units/src/include/units/dimension.h:25,
                 from C:/repos/units/src/include/units/si/base_dimensions.h:25,
                 from C:/repos/units/src/include/units/si/velocity.h:25,
                 from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool mp::std_concepts::Same<T, U> [with T = units::dimension<units::exp<units::base_dim_time, 1> >; U = units::dimension<units::exp<units::base_dim_length, 1>, units::exp<units::base_dim_time, -1> >]'
     concept bool Same = std::is_same_v<T, U>;
                  ^~~~
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
```

Time and velocity are not that complicated dimensions and there are much more complicated dimensions
out there, but even for those dimensions `[with T = units::quantity<units::dimension<units::exp<units::base_dim_time, 1> >, units::unit<units::dimension<units::exp<units::base_dim_time, 1> >, std::ratio<1> >, long long int>]`
and `[with T = units::dimension<units::exp<units::base_dim_time, 1> >; U = units::dimension<units::exp<units::base_dim_length, 1>, units::exp<units::base_dim_time, -1> >]`
starts to be really hard to analyze or debug.

That is why it was decided to provide automated upcasting capability when possible. With that the
same code will result with such an error:

```text
C:\repos\units\example\example.cpp:40:22: error: deduced initializer does not satisfy placeholder constraints
   const Velocity t = 20_s;
                      ^~~~
In file included from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/si/velocity.h:48:16: note: within 'template<class T> concept const bool units::Velocity<T> [with T = units::quantity<units::dimension_time, units::second, long long int>]'
   concept bool Velocity = Quantity<T> && Same<typename T::dimension, dimension_velocity>;
                ^~~~~~~~
In file included from C:/repos/units/src/include/units/bits/tools.h:25,
                 from C:/repos/units/src/include/units/dimension.h:25,
                 from C:/repos/units/src/include/units/si/base_dimensions.h:25,
                 from C:/repos/units/src/include/units/si/velocity.h:25,
                 from C:\repos\units\example\example.cpp:23:
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: within 'template<class T, class U> concept const bool mp::std_concepts::Same<T, U> [with T = units::dimension_time; U = units::dimension_velocity]'
     concept bool Same = std::is_same_v<T, U>;
                  ^~~~
C:/repos/units/src/include/units/bits/stdconcepts.h:33:18: note: 'std::is_same_v' evaluated to false
``` 

Now `[with T = units::quantity<units::dimension_time, units::second, long long int>]` and
`[with T = units::dimension_time; U = units::dimension_velocity]` are not arguably much better
user experience.

Upcasting capability is provided through dedicated `upcasting_traits` and by `base_type` member
type in `dimension` and `unit` class templates.

```cpp
template<typename T>
struct upcasting_traits : std::type_identity<T> {};

template<typename T>
using upcasting_traits_t = typename upcasting_traits<T>::type;
```

```cpp
struct dimension_length : make_dimension_t<exp<base_dim_length, 1>> {};

template<>
struct upcasting_traits<typename dimension_length::base_type> :
    std::type_identity<dimension_length> {};
```

```cpp
struct kilometer : unit<dimension_length, std::kilo> {};

template<>
struct upcasting_traits<typename kilometer::base_type> :
    std::type_identity<kilometer> {};
```


## Adding new dimensions

The user to extend the library with his/her own dimensions has to:
1. Create a new dimension type and provide upcasting trait for it:

```cpp
struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
template<> struct upcasting_traits<typename dimension_velocity::base_type> : std::type_identity<dimension_velocity> {};
``` 

2. Define the base unit (`std::ratio<1>`) and additional ones plus provide upcasting traits for them
via:

```cpp
struct meter_per_second : unit<dimension_velocity, std::ratio<1>> {};
template<> struct upcasting_traits<typename meter_per_second::base_type> : std::type_identity<meter_per_second> {};
``` 

3. Define a concept that will match a new dimension:

```cpp
template<typename T>
concept bool Velocity = Quantity<T> && Same<typename T::dimension, dimension_velocity>;
```

4. Provide user-defined literals for most important units:

```cpp
namespace literals {
  constexpr auto operator""_mps(unsigned long long l) { return velocity<meter_per_second, std::int64_t>(l); }
  constexpr auto operator""_mps(long double l)        { return velocity<meter_per_second, long double>(l); }
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

With that it should be really easy to add support for any new non-standard base unit to the
library without the risk of collision with any dimension type defined by the library itself or
by other users extending the library with their own dimension types.


## Open questions

1. Should we ensure that dimension is always a result of make_dimension? How to do it?

2. Should we provide strong types and upcasting_traits for `quantity` type?

    In such a case all the operators have to be provided to a child class. Or maybe use CRTP? 

3. What to do with time which ia ambiguous?

4. What to do with `std::chrono::duration`?

5. What is the best way to add support for temperatures?

    Temperatures require not only require `std::ratio` but also should adjusted/shifted by some
    constant values (i.e. [°C] = [K] − 273.15).

6. Should the "base dimension" be better expressed/isolated by the design?

7. `seconds<int>` or `time<second, int>`?

8. How to use CTAD?

    CTAD for alias templates were already supported by EWG in San Diego 2018 so `length(3.5)`
    will work. However,deduction with partial argument lists was rejected so `length<mile>(3)`
    will not be supported for now.
