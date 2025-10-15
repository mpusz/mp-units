
# Tutorial 7: Typed Quantities of the Same Kind

## Introduction

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
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
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

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>
    #include <optional>
    #include <utility>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;

    inline constexpr struct payload_width final : quantity_spec<isq::width> {} payload_width;

    std::optional<std::pair<quantity<isq::distance[km]>, quantity<min>>> find_route(quantity<isq::height[m]> height,
                                                                                    quantity<payload_width[m]> width,
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
      quantity width = payload_width(3.2 * m);
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

## Takeaways

- Typed quantities clarify the semantic role of each argument, even when their units and
  dimensions match.
- This approach prevents accidental swaps and makes code more maintainable and robust.
- Use typed quantities for APIs where argument order matters and physical meaning is
  distinct.
- Extend the default quantity hierarchies when additional safety is needed.
- Not all quantities need to be typed—use strong types where confusion is possible
  (e.g., multiple _lengths_), and keep others simple when only one role exists
  (e.g., _duration_).
