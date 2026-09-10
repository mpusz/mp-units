---
date: 2026-01-29
authors:
  - mpusz
categories:
  - Features
comments: true
---

# Interactive Learning: Tutorials and Workshops

The **mp-units** documentation has two new sections: tutorials and hands-on workshops for
learning type-safe physical quantities and units. The tutorials start at the first quantity
you create, and the workshops go on to the more advanced patterns.

<!-- more -->

## Interactive Code Examples

All tutorials and workshops use **embedded interactive code editors** powered by
[Compiler Explorer](https://godbolt.org). Each code example is a live editor that compiles
and runs automatically as you type:

- ✅ **Edit and see results instantly**: type code and watch it compile
- ✅ **Get immediate feedback**: compiler diagnostics appear as you code
- ✅ **See program output**: text output is shown directly below the editor
- ✅ **Experiment freely**: try variations without leaving the documentation

There is nothing to install and nothing to set up.

!!! tip "Experience it yourself"

    In the tutorials, every code example appears as an embedded interactive editor.
    You can modify the code directly in the documentation page and see it compile
    and run instantly. For example, a speed calculation tutorial might show:

    ```cpp
    // ce-embed height=400 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3" mp-units=trunk
    #include <mp-units/systems/si.h>
    #include <iostream>

    int main()
    {
      using namespace mp_units;
      using namespace mp_units::si::unit_symbols;

      quantity distance = 100 * m;
      quantity time = 9.58 * s;
      quantity speed = distance / time;  // Automatically derives m/s

      std::cout << "Distance: " << distance << "\n";
      std::cout << "Time: " << time << "\n";
      std::cout << "Speed: " << speed << "\n";
    }
    ```

    Just edit the numbers or units in the embedded editor, and watch the output update
    automatically below the code.


## Why the Examples Are Editable

The examples can be edited and run without leaving the page, so you can try the library
while you read about it.

### Immediate Experimentation

Try the embedded editor above:

- Change `distance` to `200 * m` and watch the speed update
- Try adding an invalid operation like `distance + time` and see the compiler error instantly
- Experiment with different units: `distance = 100 * km` still works perfectly

The compiler diagnostic appears immediately when you make a mistake, and it shows why the
dimensions don't match. There is no separate tool to run.

### Learn from Mistakes

Making mistakes is part of learning. With embedded interactive editors:

- Type invalid operations and see compiler errors instantly
- Read compiler diagnostics in context as you code
- Fix errors and watch the program compile successfully
- Build intuition through immediate feedback

### Active Learning

A few things worth doing while you read:

- **Type the code yourself** rather than only reading it
- **Experiment with variations**: change values and try different units
- **Break things on purpose** to find out where the boundaries are
- **See the results immediately**, with no compile and run cycle of your own


## Getting Started

Where to start depends on what you already know:

### For Beginners

1. Start with [Tutorials](../../tutorials/index.md): complete all 17 lessons (~4 hours)
2. Try the [Foundation Workshops](../../workshops/index.md): practice refactoring patterns
3. Explore [Examples](../../examples/index.md): study complete applications
4. Dive into specific topics in the [User's Guide](../../users_guide/index.md) as needed

### For Experienced Developers

<!-- markdownlint-disable MD013 -->
1. Skim [Quick Start tutorials](../../tutorials/index.md#1-quick-start-30-minutes) (30 min)
2. Focus on [Type Safety tutorials](../../tutorials/index.md#3-type-safety-65-minutes) (~1 hour)
3. Jump to [Customization](../../workshops/index.md) and [Advanced Workshops](../../workshops/index.md)
4. Reference the [Systems documentation](../../users_guide/systems/index.md) for your domain
<!-- markdownlint-enable MD013 -->

### For Migration

1. Read the relevant [Migration Guide](../../how_to_guides/migration/index.md)
2. Complete the [Workshop: Incremental Migration](../../workshops/advanced/incremental_migration.md)
3. Reference [Interoperability guides](../../how_to_guides/integration/index.md) as needed


## Tutorials

The **17 step-by-step tutorials** (~4 hours in total) start at "Hello World" and cover
the core features of the library. Each tutorial is meant to:

- **Build progressively**: each lesson builds on the previous one
- **Teach by example**: through working code rather than abstract theory
- **Provide instant feedback**: interactive examples let you experiment immediately
- **End with key insights**: every code example concludes with "What You Learned?" takeaways

<!-- markdownlint-disable MD013 -->
### Learning Path

#### 1. Quick Start (~30 minutes)

Three short lessons to get started:

- [**Your First Quantities**](../../tutorials/quick_start/first_quantities.md): create and manipulate quantities with units
- [**Simple Math**](../../tutorials/quick_start/simple_math.md): perform calculations and see dimensional analysis in action
- [**Building a Calculator**](../../tutorials/quick_start/building_calculator.md): assemble a real program putting concepts together

!!! success "Who it is for"

    Developers who want to see what **mp-units** can do before going further.

#### 2. Working with Units (~50 minutes)

Converting units and extracting values:

- [**Unit Conversions**](../../tutorials/working_with_units/unit_conversions.md): convert seamlessly between compatible units
- [**Safe and Unsafe Conversions**](../../tutorials/working_with_units/safe_unsafe_conversions.md): handle truncation and understand explicit conversions
- [**Extracting Numeric Values**](../../tutorials/working_with_units/extracting_numeric_values.md): get numerical values safely when needed

!!! success "Who it is for"

    Anyone who needs to work with multiple unit systems or integrate with legacy APIs.

#### 3. Type Safety (~65 minutes)

How compile-time dimensional analysis works:

- [**Compile-Time Protection**](../../tutorials/type_safety/compile_time_protection.md): watch the compiler prevent nonsensical operations
- [**Automatic Dimensional Analysis**](../../tutorials/type_safety/dimensional_analysis.md): let the compiler compute derived units
- [**Quantity Specifications**](../../tutorials/type_safety/quantity_specifications.md): use quantity hierarchies for stronger type safety
- [**Generic Interfaces**](../../tutorials/type_safety/generic_interfaces.md): write flexible, reusable code with `QuantityOf`

!!! success "Who it is for"

    Developers who want to eliminate runtime errors and write self-documenting code.

#### 4. Affine Space (~90 minutes)

The distinction between points and quantities:

- [**Points and Quantities**](../../tutorials/affine_space/points_and_quantities.md): learn when to use `quantity_point` vs `quantity`
- [**Point Origins**](../../tutorials/affine_space/point_origins.md): work with multiple reference frames
- [**Temperature Handling**](../../tutorials/affine_space/temperature_handling.md): handle absolute vs relative temperatures correctly
- [**Temperature Applications**](../../tutorials/affine_space/temperature_applications.md): apply affine space concepts to real problems
- [**Bounded Quantity Points**](../../tutorials/affine_space/bounded_quantity_points.md): attach domain constraints to origins
- [**Custom Contract Handlers**](../../tutorials/affine_space/custom_contract_handlers.md): handle constraint violations the way a production system needs
- [**Frame Projections**](../../tutorials/affine_space/frame_projections.md): convert between independent reference frames

!!! success "Who it is for"

    Anyone working with temperatures, timestamps, GPS coordinates, or any absolute measurements.


## Workshops

After the tutorials there are **15 workshops**. Each one presents a coding challenge taken
from a real-world scenario and requires you to apply concepts from the
[User's Guide](../../users_guide/index.md) to solve it.

### How Workshops Differ from Tutorials

In a workshop:

- **Exercise-driven**: follow `TODO:` markers to implement the solutions yourself
- **Real-world scenarios**: production-like code rather than toy examples
- **Progressive complexity**: start with refactoring, advance to custom systems
- **Self-directed learning**: review the User's Guide to find the solutions
- **Practical patterns**: techniques you will use in actual projects

### Workshop Progression

#### Foundation Workshops

Build essential skills through refactoring and practical patterns:

- [**Refactor to Strong Types**](../../workshops/foundation/refactor_to_strong_types.md): transform weak types into strongly-typed quantities
- [**Generic, Type-Safe Interfaces**](../../workshops/foundation/generic_type_safe_interfaces.md): make APIs flexible yet safe with `QuantityOf`
- [**Extracting Numeric Values**](../../workshops/foundation/extracting_numeric_values.md): handle legacy interfaces requiring raw numbers
- [**Working with Temperatures**](../../workshops/foundation/working_with_temperatures.md): master absolute vs relative _temperature_ handling
- [**Affine Spaces and Origins**](../../workshops/foundation/affine_spaces.md): work with multiple coordinate systems
- [**Interop with std::chrono**](../../workshops/foundation/interop_with_std_chrono.md): bridge between time libraries seamlessly

!!! success "Who it is for"

    Integrating **mp-units** into existing codebases and working with legacy systems.

#### Extensions Workshops

Create domain-specific systems of quantities and units:

- [**Custom Dimensionless Units**](../../workshops/extensions/custom_dimensionless_units.md): create strongly-typed counters for business domains (cartons, pallets, trucks)
- [**Typed Quantities of Same Kind**](../../workshops/extensions/typed_quantities_same_kind.md): distinguish _height_, _width_, and _turn radius_ to prevent argument confusion
- [**Custom Quantity Specifications**](../../workshops/extensions/custom_quantity_specifications.md): define domain-specific _energy_ types for elevator system design
- [**Custom Base Dimensions**](../../workshops/extensions/custom_base_dimensions.md): create financial dimensions (shares, currency) for portfolio tracking
- [**Distinct Quantity Kinds**](../../workshops/extensions/distinct_quantity_kinds.md): prevent mixing _fluid head_ and _water head_ in hydraulic engineering

!!! success "Who it is for"

    Building specialized systems for your domain: aviation, robotics, finance, data engineering, etc.

#### Advanced Workshops

More advanced patterns:

- [**Bounds Validation**](../../workshops/advanced/bounds_validation.md): implement production-ready bounds validation with custom contract handlers
- [**Strongly-Typed Counts**](../../workshops/advanced/strongly_typed_counts.md): use dimensionless quantities for graphics coordinates and buffer indices
- [**Faster-than-Lightspeed Constants**](../../workshops/advanced/faster_than_lightspeed_constants.md): implement physical constants as compile-time units with automatic cancellation
- [**Incremental Migration**](../../workshops/advanced/incremental_migration.md): gradually adopt **mp-units** in legacy projects
<!-- markdownlint-enable MD013 -->

!!! success "Who it is for"

    Advanced users working on complex systems requiring maximum type safety.


## Community and Feedback

We would love to hear from you:

- **Questions?** Join discussions in our [GitHub Discussions](https://github.com/mpusz/mp-units/discussions)
- **Found an issue?** Report it on [GitHub Issues](https://github.com/mpusz/mp-units/issues)
- **Success story?** Share it on social media with `#mpunits` or `#cplusplus`
- **Suggestions?** We are always looking to improve the learning resources

### Help Us Improve

As you work through tutorials and workshops:

- Let us know if something is unclear
- Suggest additional exercises or topics
- Share patterns you discover
- Report broken Compiler Explorer examples


## Try It Today

Open any tutorial or workshop and you will find interactive code editors embedded in the
documentation. Start typing, and the code compiles automatically.

**[Start Learning →](../../tutorials/index.md){ .md-button .md-button--primary }**

---

*Special thanks to [Matt Godbolt](https://xania.org) and the rest of the
[Compiler Explorer](https://godbolt.org) team for making interactive learning possible.*
