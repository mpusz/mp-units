# Tutorial 10: Custom Base Dimensions

[Tutorial 7](tutorial_7.md) showed how to create strongly-typed dimensionless quantities
using custom units with `kind_of<dimensionless>` but no conversion factors between
unrelated types. This approach works well for small-scale systems, but as complexity grows,
custom base dimensions provide better separation and scalability.

This tutorial teaches you when to progress from custom dimensionless units to custom base
dimensions, and how to create them. We'll use inventory management to demonstrate why
separate dimensions become necessary in large systems.


## Problem statement

In [Tutorial 7]((tutorial_7.md)), we created dimensionless units for `unit`, `carton`,
and `truck`. These had no conversion factors between unrelated types, preventing mixing:

```cpp
inline constexpr struct unit final : named_unit<"unit", kind_of<dimensionless>> {} unit;
inline constexpr struct carton final : named_unit<"carton", mag<12> * unit> {} carton;
inline constexpr struct truck final : named_unit<"truck", kind_of<dimensionless>> {} truck;
```

This prevents `1 * unit + 1 * truck` from compiling, which is good! But consider a
large warehouse system tracking:

- **Product inventory**: items available for sale (unit, carton, pallet)
- **Packaging materials**: boxes, containers, wrapping
- **Spare parts**: maintenance components (piece, set, kit)
- **Quality metrics**: defect counts, inspection counts
- **Personnel**: worker counts, shift counts

Similarly, financial systems dealing with multiple currencies (USD, EUR, GBP) need to prevent
accidental mixing while supporting derived quantities like _cash flow_ (_currency_/_time_).

Even international standards like ISO/IEC 80000 extend the traditional seven ISQ base dimensions
with custom dimensions for specialized domains, such as _traffic intensity_ in telecommunications,
which cannot be expressed in terms of _length_, _mass_, _time_, _electric_ current, _temperature_,
_amount of substance_, or _luminous intensity_.

### Limitations of the Tutorial 7 approach

1. **All share `dimensionless`**: Even though specifying units as unrelated based on the
   `kind_of<dimensionless>` prevents direct mixing, all unrelated types (products, pallets,
   defects, workers) conceptually belong to the same dimension. They all report as
   "dimensionless" despite representing fundamentally different things.

2. **No clear semantic grouping**: Related units (unit/carton/pallet) and unrelated units
   (defects/workers) all look the same to the type system

3. **Generic interfaces lose meaning**: Functions accepting `QuantityOf<dimensionless> auto`
   parameters don't convey whether they expect units, trucks, or workers. With custom
   dimensions, `QuantityOf<product_count> auto` makes the intent explicit.

4. **Scalability issues**: With dozens of unrelated unit types, managing `kind_of`
   relationships becomes complex and error-prone

5. **Derived quantities ambiguous**: Is `count / time` a production rate, consumption
   rate, or defect rate? All have the same type.

### Why custom dimensions?

For large systems, dedicated dimensions provide:

- **Complete semantic separation**: Products and packaging live in different type universes
- **Clear grouping**: Related quantities share a dimension (product quantities use `dim_product_count`)
- **Unambiguous derived quantities**: Production rate and defect rate have different types
- **Better error messages**: Compiler can say "can't add product_count to packaging_count"
  instead of generic dimensionless errors
- **Scalability**: Easy to add new dimensions without affecting existing code


## Your task

Build a warehouse inventory management system that requires custom dimensions for better
semantic separation than Tutorial 7's dimensionless approach could provide.

Create a complete system of quantities for product inventory management:

1. **Define a custom base dimension** `dim_product_count` for product inventory
   (not dimensionless!)
2. **Define the base quantity** `product_count` using this dimension
3. **Create specialized product count quantities**:
    - `stock_level` (current inventory, a specific type of `product_count`)
    - `reorder_point` (threshold inventory level)
    - `products_consumed` (products sold/used)
    - `products_received` (products obtained/delivered)
4. **Define derived quantities**:
    - `consumption_rate` as `products_consumed / time`
    - `replenishment_rate` as `products_received / time`
    - `sales_rate` (specialized `consumption_rate` for sales)
5. **Create units**: `item` (base unit), with scaled units like `dozen`, `gross`, `thousand`

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// TODO: Define custom base dimension for product count

// TODO: Define base quantity 'product_count' for this dimension

// TODO: Define specialized product count quantities:
// - stock_level (derived from product_count)
// - reorder_point (derived from product_count)
// - products_consumed (derived from product_count)
// - products_received (derived from product_count)

// TODO: Define derived quantities:
// - consumption_rate as products_consumed / time
// - replenishment_rate as products_received / time
// - sales_rate (specialized consumption_rate for sales)

// TODO: Define units:
// - item (base unit) - singular discrete product
// - dozen = 12 items
// - gross = 144 items (12 dozen)
// - thousand = 1000 items

namespace unit_symbols {

// TODO: Create convenient unit symbols (doz, gr, k_items, etc.)

}

// Check if reorder is needed
[[nodiscard]] constexpr bool needs_reorder(QuantityOf<stock_level> auto current_stock,
                                           QuantityOf<reorder_point> auto reorder_threshold)
{
  return current_stock <= reorder_threshold;
}

// Calculate days of inventory remaining
[[nodiscard]] constexpr QuantityOf<isq::time> auto days_of_inventory(QuantityOf<stock_level> auto stock,
                                                                     QuantityOf<sales_rate> auto rate)
{
  return isq::time(stock / rate);
}

int main()
{
  using namespace unit_symbols;

  // Warehouse inventory scenario
  quantity current_stock = stock_level(500 * item);
  quantity daily_sales = sales_rate(75 * item / si::day);
  quantity reorder_threshold = reorder_point(150 * item);
  quantity lead_time = 5 * si::day;  // delivery takes 5 days

  auto days_remaining = days_of_inventory(current_stock, daily_sales);
  auto stock_at_delivery = current_stock - daily_sales * lead_time;

  std::cout << "Inventory analysis:\n";
  std::cout << "- Current stock: " << current_stock << "\n";
  std::cout << "- Daily sales: " << daily_sales << "\n";
  std::cout << "- Reorder point: " << reorder_threshold << "\n";
  std::cout << "- Days remaining: " << days_remaining.in(si::day) << "\n";
  std::cout << "- Stock at delivery (if ordered now): " << stock_at_delivery << "\n";
  std::cout << "- Reorder needed: " << (needs_reorder(current_stock, reorder_threshold) ? "YES" : "NO") << "\n";

  // Economic order quantity scenario
  quantity target_stock = stock_level(1 * k_items);
  quantity replenishment_needed = target_stock - current_stock;

  std::cout << "\nReplenishment planning:\n";
  std::cout << "- Target stock level: " << target_stock << "\n";
  std::cout << "- Replenishment needed: " << replenishment_needed.in<double>(doz) << "\n";
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;

    // Define custom base dimension for product count
    inline constexpr struct dim_product_count final : base_dimension<"P"> {} dim_product_count;

    // Define base quantity
    inline constexpr struct product_count final : quantity_spec<dim_product_count> {} product_count;

    // Define specialized product count quantities
    inline constexpr struct stock_level final : quantity_spec<product_count> {} stock_level;
    inline constexpr struct reorder_point final : quantity_spec<product_count> {} reorder_point;
    inline constexpr struct products_consumed final : quantity_spec<product_count> {} products_consumed;
    inline constexpr struct products_received final : quantity_spec<product_count> {} products_received;

    // Define derived quantities
    inline constexpr struct consumption_rate final : quantity_spec<products_consumed / isq::time> {} consumption_rate;
    inline constexpr struct replenishment_rate final : quantity_spec<products_received / isq::time> {} replenishment_rate;
    inline constexpr struct sales_rate final : quantity_spec<consumption_rate> {} sales_rate;

    // Define units
    inline constexpr struct item final : named_unit<"item", kind_of<product_count>> {} item;
    inline constexpr struct dozen final : named_unit<"doz", mag<12> * item> {} dozen;
    inline constexpr struct gross final : named_unit<"gr", mag<12> * dozen> {} gross;
    inline constexpr struct thousand final : named_unit<"k", mag<1000> * item> {} thousand;

    namespace unit_symbols {

    inline constexpr auto doz = dozen;
    inline constexpr auto gr = gross;
    inline constexpr auto k_items = thousand;

    }

    // Check if reorder is needed
    [[nodiscard]] constexpr bool needs_reorder(QuantityOf<stock_level> auto current_stock,
                                               QuantityOf<reorder_point> auto reorder_threshold)
    {
      return current_stock <= reorder_threshold;
    }

    // Calculate days of inventory remaining
    [[nodiscard]] constexpr QuantityOf<isq::time> auto days_of_inventory(QuantityOf<stock_level> auto stock,
                                                                         QuantityOf<sales_rate> auto rate)
    {
      return isq::time(stock / rate);
    }

    int main()
    {
      using namespace unit_symbols;

      // Warehouse inventory scenario
      quantity current_stock = stock_level(500 * item);
      quantity daily_sales = sales_rate(75 * item / si::day);
      quantity reorder_threshold = reorder_point(150 * item);
      quantity lead_time = 5 * si::day;  // delivery takes 5 days

      auto days_remaining = days_of_inventory(current_stock, daily_sales);
      auto stock_at_delivery = current_stock - daily_sales * lead_time;

      std::cout << "Inventory analysis:\n";
      std::cout << "- Current stock: " << current_stock << "\n";
      std::cout << "- Daily sales: " << daily_sales << "\n";
      std::cout << "- Reorder point: " << reorder_threshold << "\n";
      std::cout << "- Days remaining: " << days_remaining.in(si::day) << "\n";
      std::cout << "- Stock at delivery (if ordered now): " << stock_at_delivery << "\n";
      std::cout << "- Reorder needed: " << (needs_reorder(current_stock, reorder_threshold) ? "YES" : "NO") << "\n";

      // Economic order quantity scenario
      quantity target_stock = stock_level(1 * k_items);
      quantity replenishment_needed = target_stock - current_stock;

      std::cout << "\nReplenishment planning:\n";
      std::cout << "- Target stock level: " << target_stock << "\n";
      std::cout << "- Replenishment needed: " << replenishment_needed.in<double>(doz) << "\n";
    }
    ```

    !!! info "Tutorial 7 approach vs Custom Dimensions"

        === "Tutorial 7 approach (dimensionless with `kind_of`)"

            ```cpp
            inline constexpr struct product final : named_unit<"product", kind_of<dimensionless>> {} product;
            inline constexpr struct carton final : named_unit<"carton", mag<12> * product> {} carton;
            inline constexpr struct truck final : named_unit<"truck", kind_of<dimensionless>> {} truck;
            ```

            - Works well for small systems with few unrelated types
            - All share `dimensionless` dimension
            - Derived quantities like `product/time` and `truck/time` have same type

        === "Custom dimension approach (this tutorial)"

            ```cpp
            inline constexpr struct dim_product_count final : base_dimension<"P"> {} dim_product_count;
            inline constexpr struct product_count final : quantity_spec<dim_product_count> {} product_count;
            inline constexpr struct item final : named_unit<"item", kind_of<product_count>> {} item;
            ```

            - Better for large systems with many unrelated countable types
            - Each category gets its own dimension (products, packaging, defects, etc.)
            - Derived quantities have distinct types (`product_rate` vs `defect_rate`)
            - Cannot be converted to `dimensionless` or use unit `one`


## References

- [User's Guide: Systems of Quantities](../users_guide/framework_basics/systems_of_quantities.md)
- [User's Guide: Systems of Units](../users_guide/framework_basics/systems_of_units.md)
- [Tutorial 7: Custom Units for Dimensionless Counters](tutorial_7.md)
- [Examples: Currency](../examples/currency.md)


## Takeaways

- **When to use custom dimensions**: Use when you have fundamentally unrelated countable
  things that should never be mixed (_product count_ vs _pallet count_ vs _defect count_)
- **Dimensionless isn't always right**: While convenient for single counters, `dimensionless`
  allows mixing all countable quantities, losing type safety in complex systems
- **Progression of approaches**:
    - `dimensionless`: Simple, single-counter scenarios (loop iterations)
    - Custom units ([Tutorial 7](tutorial_7.md)): Unrelated dimensionless types (product units
      vs trucks)
    - Custom dimensions (this tutorial): Fundamentally different domains requiring separate
      dimensions (_currencies_, _product counts_ vs _defect counts_)
- **Scalability**: In large systems with many countable types, separate dimensions prevent
  a whole class of bugs that would slip through with `dimensionless`
- **Complete type safety**: Custom dimensions work just like ISQ dimensions, preventing
  accidental mixing at compile time
- **Derived quantities**: Combine custom dimensions with ISQ _time_ to create domain-specific
  rates (_consumption rate_, _replenishment rate_)
- **Practical applications**: Essential for domains like:
    - Finance: multiple currencies (USD, EUR, GBP) with derived quantities like _cash flow_
    - Telecommunications (ISO/IEC 80000): _traffic intensity_ not expressible in traditional
      ISQ base dimensions
    - Manufacturing: _part counts_, _defect counts_, _cycle counts_
    - Logistics: _product inventory_, _shipping container counts_, _transport vehicle counts_
    - Retail: _product counts_, _customer counts_, _transaction counts_
- **Design principle**: Create separate dimensions when there's no meaningful conversion
  factor and mixing would always be an error
