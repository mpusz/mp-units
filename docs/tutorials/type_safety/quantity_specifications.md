# Quantity Specifications

Learn how to use quantity specifications for stronger type safety.

**Goal**: Understand quantity kinds and their hierarchies  
**Time**: ~20 minutes

## Beyond Just Units: Quantity Hierarchies

Not all _lengths_ are the same! **mp-units** distinguishes between different quantities of
kind _length_:

!!! tip "Understanding the hierarchy"

    To better understand how _width_, _height_, _radius_, and _distance_ relate to _length_,
    see the [`isq::length` hierarchy tree](../../reference/systems_reference/hierarchies/length.md).

```cpp
// ce-embed height=850 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // Different quantity specifications
  quantity some_length = isq::length(10 * m);
  quantity width = isq::width(5 * m);
  quantity height = isq::height(3 * m);
  quantity radius = isq::radius(2 * m);

  std::cout << "Generic length: " << some_length << "\n";
  std::cout << "Width: " << width << "\n";
  std::cout << "Height: " << height << "\n";
  std::cout << "Radius: " << radius << "\n";

  // These conversions work (specific → generic)
  some_length = width;  // ✅ width is a quantity of kind length
  some_length = height; // ✅ height is a quantity of kind length
  some_length = radius; // ✅ radius is a quantity of kind length

  // But generic → specific doesn't work implicitly
  // height = some_length;  // ❌ Can't implicitly convert generic to specific!

  // Need explicit conversion or direct initialization
  height = isq::height(some_length);               // ✅ Explicit conversion
  quantity<isq::height[m]> height2{some_length};   // ✅ Direct initialization

  std::cout << "\nAfter conversions:\n";
  std::cout << "height: " << height << "\n";
  std::cout << "height2: " << height2 << "\n";

  // These DON'T compile (specific → different specific)
  // width = height;  // ❌ Can't assign height to width!
  // width = radius;  // ❌ Can't assign radius to width!

  // But you can force it with quantity_cast if you really need to
  width = quantity_cast<isq::width>(height);  // ⚠️ Explicit cast
  std::cout << "\nwidth (cast from height): " << width << "\n";
}
```

**Key insight**: Specific quantity types (_width_, _height_, _radius_) can convert to their
parent kind (_length_), but not to each other. Converting back from generic to specific
requires explicit conversion. Use `quantity_cast` between siblings only when you're certain
the semantic conversion is valid!

## Operations Across the Hierarchy

You can add, subtract, and compare different quantities from the same hierarchy:

```cpp
// ce-embed height=850 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  quantity length = isq::length(10 * m);
  quantity width = isq::width(10 * m);
  quantity height = isq::height(5 * m);
  quantity radius = isq::radius(3 * m);
  quantity thickness = isq::thickness(0.5 * m);
  quantity diameter = isq::diameter(6 * m);

  // Addition: result is the common parent type
  quantity sum1 = width + height;
  std::cout << "width + height = " << sum1 << "\n";
  static_assert(sum1.quantity_spec == isq::length);

  // Thickness is a child of width, so width + thickness = width
  quantity sum2 = width - thickness;
  std::cout << "width + thickness = " << sum2 << "\n";
  static_assert(sum2.quantity_spec == isq::width);

  // Diameter and radius have a common parent (not root!)
  quantity sum3 = diameter + radius;
  std::cout << "diameter + radius = " << sum3 << "\n";
  static_assert(sum3.quantity_spec == isq::width);

  // Comparison: works across the hierarchy
  if (width > height)
    std::cout << "width > height: true\n";
  if (radius < diameter)
    std::cout << "radius < diameter: true\n";
}
```

**Key insight**: Arithmetic operations on different specific quantities produce the nearest
common parent in the hierarchy. This isn't always the root - it depends on the relationship
between the quantities!

## Quantity Kinds vs Specific Quantities

When you write `40 * m`, you create a quantity of kind _length_, not a specific quantity:

```cpp
// ce-embed height=750 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

int main()
{
  using namespace mp_units;
  using namespace mp_units::si::unit_symbols;

  // This creates a quantity of KIND length (not width, height, or any specific type)
  quantity generic = 40 * m;
  static_assert(generic.quantity_spec == kind_of<isq::length>);

  // Because it's a kind, it can be assigned to ANY specific quantity of that kind
  quantity<isq::width[m]> width = generic;       // ✅ Works!
  quantity<isq::height[m]> height = generic;     // ✅ Works!
  quantity<isq::radius[m]> radius = generic;     // ✅ Works!

  std::cout << "Width: " << width << "\n";
  std::cout << "Height: " << height << "\n";
  std::cout << "Radius: " << radius << "\n\n";

  // You can also explicitly create a kind (but typically not useful):
  quantity kind = kind_of<isq::length>(25 * m);
  static_assert(kind.quantity_spec == kind_of<isq::length>);

  // Now you know what it really does ;-)
  quantity distance = isq::distance(42 * km);
}
```

**Key insight**: A plain quantity like `40 * m` represents the kind _length_, making it
flexible for initialization. Specific quantities like `isq::width(40 * m)` add semantic
meaning and stricter type checking!

## Type-Safe Function Interfaces

Use specifications to make function signatures more precise:

```cpp
// ce-embed height=750 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
#include <mp-units/core.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// Function requires specifically a radius, not just any length!
quantity<square(si::metre)> circle_area(quantity<isq::radius[si::metre]> r)
{
  return pow<2>(r) * pi;
}

// Function requires width and height specifically
quantity<isq::area[square(si::metre)]> rectangle_area(quantity<isq::width[si::metre]> w,
                                                      quantity<isq::height[si::metre]> h)
{
  return w * h;
}

int main()
{
  using namespace mp_units::si::unit_symbols;

  quantity circle_radius = isq::radius(5 * m);
  quantity rect_width = isq::width(10 * m);
  quantity rect_height = isq::height(6 * m);

  // These work - correct specifications
  quantity area1 = circle_area(circle_radius);
  quantity area2 = rectangle_area(rect_width, rect_height);

  std::cout << "Circle area: " << area1 << "\n";
  std::cout << "Rectangle area: " << area2 << "\n";

  // These DON'T compile - wrong specifications
  // auto bad1 = circle_area(rect_width);                                      // ❌ width is not a radius!
  // auto bad2 = rectangle_area(circle_radius, rect_height);                   // ❌ radius is not a width!
  // quantity<isq::length[m]> bad3 = circle_area(circle_radius);               // ❌ m² is not a unit of length!
  // quantity<isq::length[m]> bad4 =  rectangle_area(rect_width, rect_height); // ❌ area is not a length!
}
```

**Key insight**: Using specific quantity types in function signatures creates self-documenting
APIs where the compiler enforces semantic correctness. You can't accidentally pass a _width_
where a _radius_ is expected, even though both are lengths with the same units.

## Challenges

Try these examples:

1. **Create a cylinder volume function**: Accept `radius` and `height`, return `volume`
2. **Experiment with conversions**: Try assigning different specific quantities and observe
  what works
3. **See the error**: Try passing wrong quantity types to the functions above
4. **Explore the hierarchy**: Look at _diameter_ + _radius_ to understand common parent results

## What You Learned?

✅ Quantity specifications distinguish between _length_, _width_, _height_, _radius_
  (all quantities of kind _length_)  
✅ Specific quantities convert to their parent kind, but not to each other without explicit
  conversion  
✅ Generic → specific needs explicit conversion; specific → different specific needs
  `quantity_cast`  
✅ Operations return the nearest common parent in the hierarchy, not always the root  
✅ A plain `40 * m` is a kind (_length_) and can initialize any specific _length_
  quantity  
✅ Use specific types in signatures for semantic correctness  
✅ The type system catches semantic mismatches at compile time  
