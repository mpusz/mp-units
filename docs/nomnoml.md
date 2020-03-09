# nomnoml

Graphs in the documentation are created with <http://www.nomnoml.com>.

## Concepts

```text
[<abstract>Dimension|
[base_dimension<Symbol, Unit>]<-[exp<Dimension, Num, Den>]
[derived_dimension<Child, Unit, Exponent...>]<-[exp<Dimension, Num, Den>]
[exp<Dimension, Num, Den>]<-[derived_dimension<Child, Unit, Exponent...>]
]

[<abstract>Quantity|
[quantity<Dimension, Unit, Rep>]
]

[<abstract>Unit]<-[Dimension]
[Dimension]<-[Quantity]
[Unit]<-[Quantity]
```

## Units

```text
#direction: right

[scaled_unit<Ratio, Unit>]<:-[unit<Child>]
[scaled_unit<Ratio, Unit>]<:-[named_unit<Child, Symbol, PrefixType>]
[scaled_unit<Ratio, Unit>]<:-[named_scaled_unit<Child, Symbol, PrefixType, Ratio, Unit>]
[scaled_unit<Ratio, Unit>]<:-[prefixed_unit<Child, Prefix, Unit>]
[scaled_unit<Ratio, Unit>]<:-[deduced_unit<Child, Dimension, Unit, Unit...>]
```

## Downcasting 1

```text
[detail::derived_dimension_base<exp<si::dim_length, 2>>]<:-[dim_area]
```

## Downcasting 2

```text
[downcast_base<detail::derived_dimension_base<exp<si::dim_length, 2>>>]<:-[detail::derived_dimension_base<exp<si::dim_length, 2>>]
[detail::derived_dimension_base<exp<si::dim_length, 2>>]<:-[downcast_child<dim_area, detail::derived_dimension_base<exp<si::dim_length, 2>>>]
[downcast_child<dim_area, detail::derived_dimension_base<exp<si::dim_length, 2>>>]<:-[dim_area]```