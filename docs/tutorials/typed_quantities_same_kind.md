
# Tutorial 8: Typed Quantities of the Same Kind

When working with physical quantities, it's common to encounter values with the same unit
and dimension but different meanings. For example, in route planning for oversized vehicles,
you may need to specify the cargo's _height_, _width_, and required _turn radius_—all
measured in meters. Accidentally swapping these arguments can lead to incorrect or unsafe
results.

This tutorial introduces the concept of typed quantities to prevent such mistakes and make
APIs self-documenting and robust.

## Problem statement

Suppose you are planning a route for a truck carrying a large electric turbine fan.
The route planner must query a road database to find a path where:

- The cargo's **_height_** can pass under all bridges and overpasses.
- The cargo's **_width_** fits through all tunnels and narrow sections.
- The truck's **_turn radius_** is accommodated at all intersections and curves.

All three are _lengths_, but their roles are distinct and must not be confused.

## Your task

1. Based on the graph in the
    [System of quantities is not only about kinds](../users_guide/framework_basics/systems_of_quantities.md#system-of-quantities-is-not-only-about-kinds)
    chapter, refactor the `find_route` function and its usage to use semantic wrappers
    (typed quantities) for each argument, so that accidental swaps are caught at compile
    time. Make sure to update the return type as well.
2. You do not need to introduce a typed quantity for every physical value—use them where
   confusion is possible (e.g., _length_), and keep others simple (e.g., _duration_).
3. Check how this approach prevents mistakes and clarifies the API.
4. Try to pass `turn_radius` argument to `width` function parameter and vice-versa.
   Analyze the difference and propose a solution to prevent this issue.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>
#include <optional>
#include <utility>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

std::optional<std::pair<quantity<km>, quantity<min>>> find_route(quantity<m> height,
                                                                 quantity<m> width,
                                                                 quantity<m> turn_radius)
{
  std::cout << "Looking for a route that will satisfy the following constraints:\n"
            << "- height: " << height << "\n"
            << "- width: " << width << "\n"
            << "- turn_radius: " << turn_radius << "\n";
  // very simplified for the sake of this tutorial ;-)
  if (height < 4.8 * m && width < 3.7 * m && turn_radius < 46 * m)
    return std::pair(342 * km, 286 * min);
  else
    return std::nullopt;
}

int main()
{
  quantity height = 4.1 * m;
  quantity width = 3.2 * m;
  quantity turn_radius = 42 * m;
  auto res = find_route(height, width, turn_radius);
  if (res) {
    auto [distance, duration] = *res;
    std::cout << "Route found:\n"
              << "- distance: " << distance << "\n"
              << "- duration: " << duration << "\n";
  }
  else {
    std::cout << "Route not found :-(\n";
  }
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>
    #include <optional>
    #include <utility>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    std::optional<std::pair<quantity<isq::distance[km]>, quantity<min>>> find_route(quantity<isq::height[m]> height,
                                                                                    quantity<isq::width[m]> width,
                                                                                    quantity<isq::radius[m]> turn_radius)
    {
      std::cout << "Looking for a route that will satisfy the following constraints:\n"
                << "- height: " << height << "\n"
                << "- width: " << width << "\n"
                << "- turn_radius: " << turn_radius << "\n";
      // very simplified for the sake of this tutorial ;-)
      if (height < 4.8 * m && width < 3.7 * m && turn_radius < 46 * m)
        return std::pair(342 * km, 286 * min);
      else
        return std::nullopt;
    }

    int main()
    {
      quantity height = isq::height(4.1 * m);
      quantity width = isq::width(3.2 * m);
      quantity turn_radius = isq::radius(42 * m);
      auto res = find_route(height, width, turn_radius);
      if (res) {
        auto [distance, duration] = *res;
        std::cout << "Route found:\n"
                  << "- distance: " << distance << "\n"
                  << "- duration: " << duration << "\n";
      }
      else {
        std::cout << "Route not found :-(\n";
      }
    }
    ```


??? abstract "What you learned?"

    ### Typed quantities prevent argument confusion

    When multiple parameters share the same dimension (e.g., all are _lengths_), accidental
    swaps compile successfully but produce wrong results:

    ```cpp
    // ❌ Untyped: All are just "meters"
    std::optional<std::pair<quantity<km>, quantity<min>>> find_route(quantity<m> height,
                                                                     quantity<m> width,
                                                                     quantity<m> turn_radius);

    auto result = find_route(width, height, turn_radius);  // Compiles! Wrong order!
    ```

    Typed quantities make each parameter semantically distinct:

    ```cpp
    // ✅ Typed: Each has specific meaning
    std::optional<std::pair<quantity<isq::distance[km]>, quantity<min>>> find_route(quantity<isq::height[m]> height,
                                                                                    quantity<isq::width[m]> width,
                                                                                    quantity<isq::radius[m]> turn_radius);

    auto result = find_route(width, height, turn_radius);  // Does not compile!
    ```

    The type system now distinguishes between different kinds of lengths based on their
    semantic role.

    ### Creating typed quantities from ISQ hierarchy

    The ISQ hierarchy provides many predefined typed quantities (like `height`, `width`,
    and `radius`) that you can use directly:

    ```cpp
    quantity height = isq::height(4.1 * m);
    quantity width = isq::width(3.2 * m);
    quantity turn_radius = isq::radius(42 * m);
    ```

    Each quantity inherits from its parent in the ISQ hierarchy, maintaining dimensional
    consistency while providing semantic type safety.

    !!! note
        For domain-specific quantities not covered by ISQ, you can define custom quantity
        specifications. This is covered in [Tutorial 9](custom_quantity_specifications.md).

    ### When to use typed quantities

    **Use typed quantities when:**

    - Multiple parameters share the same dimension but have different meanings (e.g., _height_,
      _width_, and _radius_ are all _lengths_, but serve distinct roles)
    - Argument order might be confusing or error-prone, especially with many parameters of
      the same dimension
    - Domain semantics are critical for correctness, where swapping arguments could lead to
      incorrect or unsafe results

    **Keep simple when:**

    - Only one quantity of a given dimension is used

        ```cpp
        // Good: Only one duration, no confusion possible
        void plan_route(...) -> std::pair<quantity<isq::distance[km]>, quantity<min>>;
        ```

    - Semantic distinction doesn't add value

        ```cpp
        // Simple is better when meaning is obvious from context
        quantity trip_duration = 286 * min;  // No need for typed duration here
        ```


## References

- [User's Guide: Systems of Quantities](../users_guide/framework_basics/systems_of_quantities.md)
- [User's Guide: Simple and Typed Quantities](../users_guide/framework_basics/simple_and_typed_quantities.md)
- [Systems Reference: Length Hierarchy](../reference/systems_reference/hierarchies/length.md)


## Takeaways

- Typed quantities clarify the semantic role of each argument, even when their units and
  dimensions match.
- This approach prevents accidental swaps and makes code more maintainable and robust.
- Use typed quantities for APIs where argument order matters and physical meaning is
  distinct.
- Not all quantities need to be typed—use strong types where confusion is possible
  (e.g., multiple _lengths_), and keep others simple when only one role exists
  (e.g., _duration_).
