# Custom Base Dimensions

This workshop demonstrates when and how to create custom base dimensions for quantities that
cannot be expressed in terms of existing ISQ dimensions. We'll build a stock portfolio
tracking system using **shares** and **currency** as examples of fundamentally distinct
quantities that need complete type separation.


## Problem statement

When tracking a stock portfolio, we work with two fundamentally different quantities:

- **_Shares_**: The number of stock shares you own (e.g., 100 shares of AAPL)
- **_Currency_**: Money in dollars (e.g., $15,000 portfolio value)

These are **completely different types** that should never be mixed:

```cpp
// These operations should not compile:
shares + currency           // ❌ Can't add shares to dollars
currency - shares           // ❌ Can't subtract shares from dollars
if (shares > currency) {}   // ❌ Can't compare shares with dollars
```

However, they **combine through multiplication** to create meaningful derived quantities:

```cpp
shares × (currency/share) = currency    // ✅ Portfolio valuation
currency / shares = (currency/share)    // ✅ Average share price
```

### Why not use dimensionless or existing ISQ dimensions?

**Option 1: Dimensionless quantities** ([Workshop: Strongly-Typed Counts](../advanced/strongly_typed_counts.md)
approach)

- ❌ Shares aren't dimensionless — they have a distinct dimension
- ❌ Using `one` as a unit doesn't make sense ("100 one" is not "100 shares")
- ❌ Mixing _shares_ and _currency_ is as wrong as mixing _length_ and _time_

**Option 2: Existing ISQ dimensions**

- ❌ Can't express "shares" or "currency" using _length_, _mass_, _time_, etc.
- ❌ These are financial/economic concepts, not physical quantities

**Option 3: Custom base dimensions** ✅

- ✅ Complete isolation — _shares_ and _currency_ live in separate dimensional universes
- ✅ Cannot be converted to each other (no exchange rate makes sense)
- ✅ Can create derived quantities like _share price_ (_currency_/_share_) naturally
- ✅ Portfolio calculations have dimensional correctness: _shares_ × _price_ = _value_


## Your task

Build a stock portfolio tracking system:

1. **Define custom base dimensions**:
    - `dim_shares` — dimension for _stock shares_
    - `dim_currency` — dimension for _money_

2. **Define quantity specifications**:
    - `shares` — number of _stock shares_
    - `currency` — amount of _money_ (dollars)
    - *(Refinements `dividend` and `capital` are already provided)*
    - *(Derived quantities `share_price` and `dividend_per_share` are already provided)*

3. **Define units**:
    - `share` — unit for counting shares
    - `USD` — unit for U.S. dollars

4. **Constrain helper function signatures**:
    - The functions are already implemented, but use unconstrained `auto` parameters
    - Add `QuantityOf<T>` constraints to parameters and return types for type safety
    - Example: `[[nodiscard]] constexpr QuantityOf<currency> auto portfolio_value(QuantityOf<shares> auto num_shares, ...)`

```cpp
// ce-embed height=900 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

namespace finance {

// TODO: Define custom base dimensions for shares (symbol "N") and currency (symbol "$")

// TODO: Define quantity specifications for shares and currency flows
// Refinements for different currency flows
inline constexpr struct dividend final : quantity_spec<currency> {} dividend;
inline constexpr struct capital final : quantity_spec<currency> {} capital;

// Derived quantities
inline constexpr struct share_price final : quantity_spec<currency / shares> {} share_price;
inline constexpr struct dividend_per_share final : quantity_spec<dividend / shares> {} dividend_per_share;

// TODO Define unit share for shares and USD for currency

// TODO: For all functions below constrain function parameters
//       and the return type using QuantityOf<> concept
[[nodiscard]] constexpr auto portfolio_value(auto num_shares
                                             auto price_per_share)
{
  return num_shares * price_per_share;
}

[[nodiscard]] constexpr auto shares_affordable(auto budget,
                                               auto price_per_share)
{
  // Calculate how many whole shares can be purchased
  return floor<share>(budget / price_per_share);
}

[[nodiscard]] constexpr auto dividend_income(auto num_shares,
                                             auto div_per_share)
{
  return num_shares * div_per_share;
}

}  // namespace finance

int main()
{
  using namespace finance;

  // Portfolio: Multiple purchases of stock
  quantity purchase1_shares = 50 * share;
  quantity purchase1_price = 120.0 * USD / share;
  quantity purchase1_cost = capital(purchase1_shares * purchase1_price);

  quantity purchase2_shares = 30 * share;
  quantity purchase2_price = 135.0 * USD / share;
  quantity purchase2_cost = capital(purchase2_shares * purchase2_price);

  quantity purchase3_shares = 20 * share;
  quantity purchase3_price = 142.0 * USD / share;
  quantity purchase3_cost = capital(purchase3_shares * purchase3_price);

  // Calculate totals
  quantity total_shares = purchase1_shares + purchase2_shares + purchase3_shares;
  quantity total_spent = purchase1_cost + purchase2_cost + purchase3_cost;
  quantity average_cost_basis = total_spent / total_shares;

  std::cout << "Portfolio Summary:\n";
  std::cout << "  Total shares: " << total_shares << "\n";
  std::cout << "  Total invested: " << total_spent << "\n";
  std::cout << "  Average cost basis: " << average_cost_basis << "\n\n";

  // Current market conditions
  quantity current_price = 150.0 * USD / share;
  quantity current_value = portfolio_value(total_shares, current_price);
  quantity profit = current_value - total_spent;

  std::cout << "Current Valuation:\n";
  std::cout << "  Current price: " << current_price << "\n";
  std::cout << "  Portfolio value: " << current_value << "\n";
  std::cout << "  Unrealized profit: " << profit << "\n\n";

  // Dividend income (quarterly)
  quantity div_per_share = dividend(2.50 * USD) / share;
  quantity quarterly_dividend = dividend_income(total_shares, div_per_share);
  std::cout << "Dividend Income:\n";
  std::cout << "  Quarterly dividend: " << quarterly_dividend << "\n\n";

  // Check if we can afford more shares
  quantity available_cash = 5000.0 * USD;
  quantity max_shares = shares_affordable(available_cash, current_price);
  quantity purchase_cost = max_shares * current_price;
  std::cout << "Purchase Analysis:\n";
  std::cout << "  Available cash: " << available_cash << "\n";
  std::cout << "  Current price: " << current_price << "\n";
  std::cout << "  Max affordable shares: " << max_shares << "\n";
  std::cout << "  Total cost: " << purchase_cost << "\n";

  // These should NOT compile (uncomment to verify):
  // auto bad1 = total_shares + total_spent;     // ERROR: Can't add shares + currency
  // auto bad2 = current_value - total_shares;   // ERROR: Can't subtract shares from currency
  // if (total_shares > current_value) {}        // ERROR: Can't compare shares with currency
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/core.h>
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;

    namespace finance {

    // Define custom base dimensions
    inline constexpr struct dim_shares final : base_dimension<"N"> {} dim_shares;
    inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;

    // Define quantity specifications
    inline constexpr struct shares final : quantity_spec<dim_shares> {} shares;
    inline constexpr struct currency final : quantity_spec<dim_currency> {} currency;

    // Refinements for different currency flows
    inline constexpr struct dividend final : quantity_spec<currency> {} dividend;
    inline constexpr struct capital final : quantity_spec<currency> {} capital;

    // Derived quantities
    inline constexpr struct share_price final : quantity_spec<currency / shares> {} share_price;
    inline constexpr struct dividend_per_share final : quantity_spec<dividend / shares> {} dividend_per_share;

    // Define units
    inline constexpr struct share final : named_unit<"share", kind_of<shares>> {} share;
    inline constexpr struct USD final : named_unit<"USD", kind_of<currency>> {} USD;

    [[nodiscard]] constexpr QuantityOf<currency> auto portfolio_value(QuantityOf<shares> auto num_shares,
                                                                      QuantityOf<share_price> auto price_per_share)
    {
      return num_shares * price_per_share;
    }

    [[nodiscard]] constexpr QuantityOf<shares> auto shares_affordable(QuantityOf<currency> auto budget,
                                                                      QuantityOf<share_price> auto price_per_share)
    {
      // Calculate how many whole shares can be purchased
      return floor<share>(budget / price_per_share);
    }

    [[nodiscard]] constexpr QuantityOf<dividend> auto dividend_income(QuantityOf<shares> auto num_shares,
                                                                      QuantityOf<dividend_per_share> auto div_per_share)
    {
      return num_shares * div_per_share;
    }

    }  // namespace finance

    int main()
    {
      using namespace finance;

      // Portfolio: Multiple purchases of stock
      quantity purchase1_shares = 50 * share;
      quantity purchase1_price = 120.0 * USD / share;
      quantity purchase1_cost = capital(purchase1_shares * purchase1_price);

      quantity purchase2_shares = 30 * share;
      quantity purchase2_price = 135.0 * USD / share;
      quantity purchase2_cost = capital(purchase2_shares * purchase2_price);

      quantity purchase3_shares = 20 * share;
      quantity purchase3_price = 142.0 * USD / share;
      quantity purchase3_cost = capital(purchase3_shares * purchase3_price);

      // Calculate totals
      quantity total_shares = purchase1_shares + purchase2_shares + purchase3_shares;
      quantity total_spent = purchase1_cost + purchase2_cost + purchase3_cost;
      quantity average_cost_basis = total_spent / total_shares;

      std::cout << "Portfolio Summary:\n";
      std::cout << "  Total shares: " << total_shares << "\n";
      std::cout << "  Total invested: " << total_spent << "\n";
      std::cout << "  Average cost basis: " << average_cost_basis << "\n\n";

      // Current market conditions
      quantity current_price = 150.0 * USD / share;
      quantity current_value = portfolio_value(total_shares, current_price);
      quantity profit = current_value - total_spent;

      std::cout << "Current Valuation:\n";
      std::cout << "  Current price: " << current_price << "\n";
      std::cout << "  Portfolio value: " << current_value << "\n";
      std::cout << "  Unrealized profit: " << profit << "\n\n";

      // Dividend income (quarterly)
      quantity div_per_share = dividend(2.50 * USD) / share;
      quantity quarterly_dividend = dividend_income(total_shares, div_per_share);
      std::cout << "Dividend Income:\n";
      std::cout << "  Quarterly dividend: " << quarterly_dividend << "\n\n";

      // Check if we can afford more shares
      quantity available_cash = 5000.0 * USD;
      quantity max_shares = shares_affordable(available_cash, current_price);
      quantity purchase_cost = max_shares * current_price;
      std::cout << "Purchase Analysis:\n";
      std::cout << "  Available cash: " << available_cash << "\n";
      std::cout << "  Current price: " << current_price << "\n";
      std::cout << "  Max affordable shares: " << max_shares << "\n";
      std::cout << "  Total cost: " << purchase_cost << "\n";
    }
    ```


??? abstract "What you learned?"

    ### Complete dimensional isolation

    Custom base dimensions create completely separate dimensional "universes":

    ```cpp
    inline constexpr struct dim_shares final : base_dimension<"N"> {} dim_shares;
    inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
    ```

    - ✅ `shares` and `currency` cannot be added, subtracted, or compared
    - ✅ No conversion path exists between them — not even explicit conversion
    - ✅ Each dimension is truly independent, just like _length_ and _time_ in physics

    ### Derived quantities from custom dimensions

    Just like physical dimensions, custom dimensions can create derived quantities:

    ```cpp
    // Named derived quantity spec (like ISQ speed, force, etc.)
    inline constexpr struct share_price final : quantity_spec<currency / shares> {} share_price;
    inline constexpr struct dividend_per_share final : quantity_spec<dividend / shares> {} dividend_per_share;

    quantity price = 150.0 * USD / share;           // Share price: currency/shares
    quantity num_shares = 100 * share;              // Number of shares
    quantity value = num_shares * price;            // Portfolio value: currency
    ```

    **Why named instead of `auto share_price = currency / shares`?**

    Named quantity specs enable refinement hierarchies for specialized types.
    For example, an order book system could refine share prices:

    ```cpp
    // Named derived quantity allows refinement
    inline constexpr struct share_price final : quantity_spec<currency / shares> {} share_price;

    // Refinements for specific price types in order book
    inline constexpr struct bid_price final : quantity_spec<share_price> {} bid_price;
    inline constexpr struct ask_price final : quantity_spec<share_price> {} ask_price;
    inline constexpr struct limit_price final : quantity_spec<share_price> {} limit_price;

    // Type-safe order book operations
    quantity best_bid = bid_price(149.95 * USD / share);
    quantity best_ask = ask_price(150.05 * USD / share);
    quantity spread = best_ask - best_bid;  // Spread is generic share_price
    ```

    ### Quantity kinds vs explicit quantity specs

    **When multiplying values with units:**

    ```cpp
    quantity amount = 100.0 * USD;  // Creates a quantity *kind* - matches any currency refinement
    ```

    A quantity **kind** is flexible - it can match `currency`, `dividend`, or `capital`.
    This is useful in generic contexts (e.g., function parameters, type aliases).

    **When you need to be specific:**

    ```cpp
    // Explicit type specification (kind is implicitly converted)
    quantity<capital[USD]> invested = 100.0 * USD;     // Kind → capital
    quantity<dividend[USD]> income = 50.0 * USD;       // Kind → dividend

    // Quantity spec conversion (explicitly converts kind)
    quantity invested = capital(100.0 * USD);          // Kind → capital via conversion
    quantity income = dividend(50.0 * USD);            // Kind → dividend via conversion

    // Direct construction (not a kind conversion)
    quantity invested = 100.0 * capital[USD];          // Directly creates capital
    quantity income = 50.0 * dividend[USD];            // Directly creates dividend
    ```


## References

- [User's Guide: Systems of Quantities](../../users_guide/framework_basics/systems_of_quantities.md)
- [User's Guide: Systems of Units](../../users_guide/framework_basics/systems_of_units.md)
- [Workshop: Custom Units for Dimensionless Counters](custom_dimensionless_units.md)
- [Workshop: Distinct Quantity Kinds](distinct_quantity_kinds.md)
- [Workshop: Strongly-Typed Counts](../advanced/strongly_typed_counts.md)
- [Examples: Currency](../../examples/currency.md)


## Takeaways

- **Custom base dimensions** create completely isolated type systems for incommensurable quantities
    - Use them when quantities represent fundamentally different concepts (shares ≠ currency)
- **Different from dimensionless**: Custom dimensions cannot use `one` or convert to dimensionless
- **Natural dimensional algebra**: Custom dimensions support derived quantities (`currency/share`,
  `share × price = value`)
- **Complete type safety**: Prevents mixing incompatible types at compile time
  (shares + currency ❌)
- **When to choose custom dimensions**:
    - No meaningful direct conversion exists (shares ↔ currency needs price)
    - Unit `one` doesn't apply to the quantity
    - Quantities combine through multiplication/division, not addition
- **Real-world domains**: Finance (shares, bonds, currencies), telecommunications
  (call attempts, erlangs), manufacturing (different part types), inventory
  (product categories)
