# Strongly-Typed Counts

Most values in our programs aren't physical quantities with units—they're counts,
coordinates, identifiers, or indices. These discrete values don't need unit-based scaling
(meters, kilograms, seconds), but they desperately need **type safety** to prevent mixing
semantically different values.

This workshop demonstrates how **mp-units** dimensionless quantities provide strongly-typed
numeric types for countable values, preventing bugs while maintaining zero-overhead performance.


## Problem statement

Consider a simple graphics rendering system that needs to track:

- **Screen coordinates**: X and Y pixel positions (should not be mixed)
- **Item counts**: Number of sprites to render
- **Buffer indices**: Positions in render queues

All are just integers, but mixing them causes subtle bugs:

=== "Without strong types"

    ```cpp
    void render(int x, int y, int sprite_count)
    {
      // Easy to accidentally swap x and y
      draw_at(y, x, sprite_count);  // Bug! Wrong order

      // Or use a coordinate as a count
      if (sprite_count > x) {  // Compiles but meaningless comparison
        // ...
      }
    }
    ```

=== "With type aliases (no safety)"

    ```cpp
    using PixelX = int;
    using PixelY = int;
    using SpriteCount = int;

    void render(PixelX x, PixelY y, SpriteCount count)
    {
      draw_at(y, x, count);  // Still compiles! Type aliases provide no safety
      if (count > x) {  // Still compiles!
        // ...
      }
    }
    ```

Traditional solutions require either verbose wrapper classes with manual operator definitions,
or generic strong-type libraries that lack domain-specific semantics.


## Your task

Create strongly-typed numeric types for a 2D rendering system using **mp-units** dimensionless
quantities. The system should:

1. **Define distinct quantity types** for:
    - `pixel_x` — X-axis screen coordinates
    - `pixel_y` — Y-axis screen coordinates
    - `sprite_count` — Number of sprites
    - `frame_count` — Number of animation frames
    - `buffer_index` — 1D buffer position/offset
    - `resolution_width` — Horizontal screen resolution
    - `resolution_height` — Vertical screen resolution

2. **Prevent invalid operations**:
    - Cannot add `pixel_x + pixel_y` (mixing axes)
    - Cannot compare `sprite_count < pixel_x` (mixing semantics)
    - Cannot accidentally swap coordinate parameters

3. **Allow valid operations**:
    - Can add/subtract coordinates of the same axis: `pixel_x + pixel_x`
    - Can compare values of the same type: `pixel_x < pixel_x`
    - Can derive quantities: `sprite_count / time` for render rate

4. **Implement functions**:
    - `is_within_bounds(x, y, width, height)` — check if coordinates are valid
    - `render_rate(sprite_count, duration)` — calculate sprites per second
    - `grid_index(x, y, width)` — convert 2D coords to 1D buffer index

```cpp
// ce-embed height=850 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// TODO: Define pixel_x as a distinct kind of dimensionless
// Hint: Use quantity_spec<dimensionless, is_kind> to make it distinct from other counts

// TODO: Define pixel_y as a distinct kind of dimensionless

// TODO: Define sprite_count as a distinct kind of dimensionless

// TODO: Define frame_count as a distinct kind of dimensionless

// TODO: Define buffer_index as a distinct kind of dimensionless

// TODO: Define resolution_width as a quantity_spec derived from pixel_x
// (resolution width is a specific type of X coordinate)

// TODO: Define resolution_height as a quantity_spec derived from pixel_y

// TODO: Define the derived quantity render_rate as sprite_count / time

// TODO: Define the derived quantity frame_rate as frame_count / time

// TODO: Check if coordinates are within screen bounds
// Return type: bool
// Parameters:
//   - x: QuantityOf<pixel_x> auto
//   - y: QuantityOf<pixel_y> auto
//   - width: QuantityOf<resolution_width> auto
//   - height: QuantityOf<resolution_height> auto
[[nodiscard]] constexpr bool is_within_bounds(/* TODO: add parameters */)
{
  // TODO: implement
  // Hint: x should be in [0, width) and y should be in [0, height)
}

// TODO: Calculate rendering rate (sprites per second)
// Return type: QuantityOf<render_rate> auto
// Parameters:
//   - count: QuantityOf<sprite_count> auto
//   - duration: QuantityOf<isq::time> auto
[[nodiscard]] constexpr auto render_rate_calc(/* TODO: add parameters */)
{
  // TODO: implement
}

// TODO: Convert 2D coordinates to 1D buffer index
// Return type: QuantityOf<buffer_index> auto
// Parameters:
//   - x: QuantityOf<pixel_x> auto
//   - y: QuantityOf<pixel_y> auto
//   - width: QuantityOf<resolution_width> auto
// Formula: index = y * width + x
[[nodiscard]] constexpr auto grid_index(/* TODO: add parameters */)
{
  // TODO: implement
  // Hint: Convert y * width and x to dimensionless, then wrap result in buffer_index
}

int main()
{
  using namespace si::unit_symbols;

  // Screen configuration
  quantity screen_width = resolution_width(1920 * one);
  quantity screen_height = resolution_height(1080 * one);

  // Sprite position
  quantity pos_x = pixel_x(100 * one);
  quantity pos_y = pixel_y(50 * one);

  // Rendering stats
  quantity rendered = sprite_count(1500 * one);
  quantity target_fps = 60.0 / s;
  quantity frame_time = 1.0 / target_fps;

  // Validate position
  if (is_within_bounds(pos_x, pos_y, screen_width, screen_height))
    std::cout << "Position (" << pos_x << ", " << pos_y << ") is valid\n";

  // Calculate buffer index
  quantity index = grid_index(pos_x, pos_y, screen_width);
  std::cout << "Buffer index: " << index << "\n";

  // Calculate render rate
  quantity rate = render_rate_calc(rendered, frame_time);
  std::cout << "Render rate: " << rate.in(one / s) << "\n";

  // Try these intentional errors (uncomment to test):
  // auto bad_sum = pos_x + pos_y;  // ERROR: Cannot mix X and Y coordinates
  // auto bad_cmp = (rendered < pos_x);  // ERROR: Cannot compare count with coordinate
  // auto bad_bounds = is_within_bounds(pos_y, pos_x, screen_width, screen_height);  // ERROR: Swapped X/Y
}
```

??? tip "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <iostream>

    using namespace mp_units;

    // Define distinct dimensionless quantity types using is_kind
    inline constexpr struct pixel_x final : quantity_spec<dimensionless, is_kind> {} pixel_x;
    inline constexpr struct pixel_y final : quantity_spec<dimensionless, is_kind> {} pixel_y;
    inline constexpr struct sprite_count final : quantity_spec<dimensionless, is_kind> {} sprite_count;
    inline constexpr struct frame_count final : quantity_spec<dimensionless, is_kind> {} frame_count;
    inline constexpr struct buffer_index final : quantity_spec<dimensionless, is_kind> {} buffer_index;

    // Define specialized quantities (children of the distinct kinds)
    inline constexpr struct resolution_width final : quantity_spec<pixel_x> {} resolution_width;
    inline constexpr struct resolution_height final : quantity_spec<pixel_y> {} resolution_height;

    // Define derived quantity for render rate
    inline constexpr struct render_rate final : quantity_spec<sprite_count / isq::time> {} render_rate;

    // Define derived quantity for frame rate
    inline constexpr struct frame_rate final : quantity_spec<frame_count / isq::time> {} frame_rate;

    [[nodiscard]] constexpr bool is_within_bounds(QuantityOf<pixel_x> auto x,
                                                  QuantityOf<pixel_y> auto y,
                                                  QuantityOf<resolution_width> auto width,
                                                  QuantityOf<resolution_height> auto height)
    {
      return x >= 0 * one && x < width && y >= 0 * one && y < height;
    }

    [[nodiscard]] constexpr QuantityOf<render_rate> auto render_rate_calc(QuantityOf<sprite_count> auto count,
                                                                          QuantityOf<isq::time> auto duration)
    {
      return count / duration;
    }

    [[nodiscard]] constexpr QuantityOf<buffer_index> auto grid_index(QuantityOf<pixel_x> auto x,
                                                                     QuantityOf<pixel_y> auto y,
                                                                     QuantityOf<resolution_width> auto width)
    {
      // Convert to dimensionless for the index calculation
      // y * width has type pixel_y * pixel_x, which we convert to dimensionless
      // x is pixel_x, so we convert it to dimensionless too
      return buffer_index(dimensionless(y * width) + dimensionless(x));
    }

    int main()
    {
      using namespace si::unit_symbols;

      // Screen configuration
      quantity screen_width = resolution_width(1920 * one);
      quantity screen_height = resolution_height(1080 * one);

      // Sprite position
      quantity pos_x = pixel_x(100 * one);
      quantity pos_y = pixel_y(50 * one);

      // Rendering stats
      quantity rendered = sprite_count(1500 * one);
      quantity target_fps = 60.0 / s;
      quantity frame_time = 1.0 / target_fps;

      // Validate position
      if (is_within_bounds(pos_x, pos_y, screen_width, screen_height))
        std::cout << "Position (" << pos_x << ", " << pos_y << ") is valid\n";

      // Calculate buffer index
      quantity index = grid_index(pos_x, pos_y, screen_width);
      std::cout << "Buffer index: " << index << "\n";

      // Calculate render rate
      quantity rate = render_rate_calc(rendered, frame_time);
      std::cout << "Render rate: " << rate << "\n";
    }
    ```


??? abstract "What you learned?"

    ### Type safety without boilerplate

    By defining `pixel_x`, `pixel_y`, `sprite_count`, `frame_count`, and `buffer_index` as
    distinct kinds of dimensionless quantities using `is_kind`, you get:

    - **Automatic type safety**: Cannot mix coordinates, counts, frames, and indices
    - **All arithmetic operators**: Addition, subtraction, comparison, etc., work
      automatically within the kind
    - **Zero overhead**: Compiles to the same machine code as raw integers
    - **Clear semantics**: Function signatures document what each parameter represents

    ### The power of `is_kind`

    The `is_kind` specifier creates distinct quantity **subkinds** within the quantity
    hierarchy:

    ```cpp
    inline constexpr struct pixel_x final : quantity_spec<dimensionless, is_kind> {} pixel_x;
    inline constexpr struct pixel_y final : quantity_spec<dimensionless, is_kind> {} pixel_y;
    ```

    This means:

    - ✅ `pixel_x` and `pixel_y` are both dimensionless (dimension is `dimension_one`)
    - ✅ Both can use the unit `one` and its scaled versions
    - ✅ Both benefit from unit `one` superpowers (implicit construction from raw values)
    - ❌ But `pixel_x` and `pixel_y` **cannot be mixed** in arithmetic or comparisons
    - ❌ They are **distinct kinds** that require explicit conversion

    ### Hierarchies within kinds

    You can create hierarchies within your custom kinds:

    ```cpp
    inline constexpr struct pixel_x final : quantity_spec<dimensionless, is_kind> {} pixel_x;
    inline constexpr struct resolution_width final : quantity_spec<pixel_x> {} resolution_width;
    ```

    - `resolution_width` **is a** `pixel_x` (implicitly convertible)
    - `pixel_x` **is not** a `resolution_width` (would require explicit conversion)
    - Neither can be mixed with `pixel_y`, `sprite_count`, `frame_count`, or `buffer_index`

    This allows building rich type hierarchies while maintaining type safety.

    ### Derived quantities

    You can create derived quantities from your custom types:

    ```cpp
    inline constexpr struct render_rate final : quantity_spec<sprite_count / isq::time> {} render_rate;
    inline constexpr struct frame_rate final : quantity_spec<frame_count / isq::time> {} frame_rate;
    ```

    This automatically works with dimensional analysis:

    ```cpp
    // Frame rate: frames per time, and inverse gives time per frame
    quantity target_fps = 60.0 / s;
    quantity frame_time = 1.0 / target_fps;  // Dimensional analysis!

    // Render rate: sprites per time
    quantity rendered = sprite_count(1500 * one);
    // sprite_count / time implicitly converts to render_rate
    quantity<render_rate[one / s]> rate = rendered / frame_time;
    ```

    ### When to use this approach

    Use dimensionless quantities with `is_kind` for:

    - **Counts and indices**: item counts, array indices, iteration counts
    - **Coordinates**: screen pixels, grid positions (when axes should not mix)
    - **Identifiers**: user IDs, session IDs (when used in arithmetic)
    - **Discrete quantities**: button presses, error counts, retry attempts

    **Don't use custom dimensions** when:

    - You want to use the unit `one` automatically
    - Values are fundamentally countable/dimensionless
    - You need natural numeric semantics

    **Use custom dimensions** (see [Workshop: Custom Base Dimensions](../extensions/custom_base_dimensions.md))
    when:

    - Values represent truly distinct physical concepts
    - You need complete isolation from ISQ
    - Custom base dimensions better reflect your domain model


    ## Alternative: Using explicit unit kinds

    If you want even stricter type safety where each quantity type has its own unit, you can
    define units with `kind_of`:

    ```cpp
    inline constexpr struct pixel_x_unit final : named_unit<"px_x", one, kind_of<pixel_x>> {} pixel_x_unit;
    inline constexpr auto px_x = pixel_x_unit;
    inline constexpr struct pixel_y_unit final : named_unit<"px_y", one, kind_of<pixel_y>> {} pixel_y_unit;
    inline constexpr auto px_y = pixel_y_unit;
    inline constexpr struct sprite final : named_unit<"sprite", one, kind_of<sprite_count>> {} sprite;
    inline constexpr struct frame final : named_unit<"frame", one, kind_of<frame_count>> {} frame;

    quantity x = 100 * px_x;                 // pixel_x[px_x]
    quantity y = 50 * px_y;                  // pixel_y[px_y]
    quantity sprites = 1500 * sprite;        // sprite_count[sprite]
    quantity frames = 60 * frame;            // frame_count[frame]
    quantity fps = 60.0 * (frame / s);       // frame_count/time[frame/s]
    ```

    However, this prevents using the unit `one` and its superpowers (implicit construction from
    numeric literals, implicit conversion to numeric types). The approach shown in the main
    workshop (using `one` with `is_kind` quantity specs) is more flexible and ergonomic for most
    use cases.


## References

- [User's Guide: Dimensionless Quantities](../../users_guide/framework_basics/dimensionless_quantities.md)
- [User's Guide: Using dimensionless quantities as strongly-typed numeric types](../../users_guide/framework_basics/dimensionless_quantities.md#using-dimensionless-quantities-as-strongly-typed-numeric-types)
- [Workshop: Custom Dimensionless Units](../extensions/custom_dimensionless_units.md)
- [Workshop: Custom Base Dimensions](../extensions/custom_base_dimensions.md)
- [Workshop: Distinct Quantity Kinds](../extensions/distinct_quantity_kinds.md)


## Takeaways

- **mp-units** provides strongly-typed numeric types through dimensionless quantities,
  solving a decades-old C++ problem for countable values
- Use `is_kind` to create distinct quantity types that cannot be accidentally mixed
- All arithmetic operators work automatically—no boilerplate needed
- Unit `one` provides superpowers: implicit construction from raw values, explicit
  conversion to raw values, and comparison with raw values
- Build hierarchies within your custom kinds for fine-grained type safety
- This approach is ideal for counts, coordinates, identifiers, and discrete quantities
- Zero overhead—compiles to the same machine code as raw integers
- For truly distinct physical concepts, use custom dimensions instead
  (see [Workshop: Custom Base Dimensions](../extensions/custom_base_dimensions.md))
