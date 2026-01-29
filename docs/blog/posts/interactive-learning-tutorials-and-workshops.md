---
date: 2026-01-29
authors:
  - mpusz
categories:
  - Features
comments: true
---

# Interactive Learning: Tutorials and Workshops

We're thrilled to announce a major expansion of **mp-units** learning resources: comprehensive
tutorials and hands-on workshops that make learning type-safe physical quantities and units
both accessible and engaging. Whether you're taking your first steps with the library or
ready to master advanced patterns, we've got you covered.

<!-- more -->

## Learn by Doing: Interactive Code Examples

All tutorials and workshops feature **embedded interactive code editors** powered by
[Compiler Explorer](https://godbolt.org), making learning truly hands-on. Each code example
is a live editor that compiles and runs automatically as you type:

- ✅ **Edit and see results instantly** — Type code and watch it compile in real-time
- ✅ **Get immediate feedback** — Compiler diagnostics appear as you code
- ✅ **See program output** — View text output directly below the editor
- ✅ **Experiment freely** — Try variations without leaving the documentation
- ✅ **Learn by doing** — Active coding beats passive reading

No installation required. No setup overhead. Just start typing and learning.

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


## Why Interactive Learning Matters?

Traditional documentation tells you *what* the library does. Interactive learning shows
you *how* to use it effectively.

### Immediate Experimentation

Instead of reading about type safety, **see it in action**. Try the embedded editor above:

- Change `distance` to `200 * m` and watch the speed update
- Try adding an invalid operation like `distance + time` and see the compiler error instantly
- Experiment with different units: `distance = 100 * km` still works perfectly

The compiler diagnostic appears immediately when you make mistakes, showing exactly why
dimensions don't match. No waiting, no separate tools—just instant feedback as you learn.

### Learn from Mistakes

Making mistakes is part of learning. With embedded interactive editors:

- Type invalid operations and see compiler errors instantly
- Read compiler diagnostics in context as you code
- Fix errors and watch the program compile successfully
- Build intuition through immediate feedback

### Active Learning

Passive reading leads to passive retention. Active coding leads to deep understanding:

- **Type the code yourself** — Don't just read, write
- **Experiment with variations** — Change values, try different units
- **Break things intentionally** — Learn boundaries by testing them
- **See immediate results** — No compile/run cycle overhead


## Getting Started

Ready to learn **mp-units**? Here's your roadmap:

### For Beginners

1. Start with [Tutorials](../../tutorials/index.md) — Complete all 14 lessons (~3.5 hours)
2. Try the [Foundation Workshops](../../workshops/index.md) — Practice refactoring patterns
3. Explore [Examples](../../examples/index.md) — Study complete applications
4. Dive into specific topics in the [User's Guide](../../users_guide/index.md) as needed

### For Experienced Developers

Already familiar with units libraries?

1. Skim [Quick Start tutorials](../../tutorials/index.md#1-quick-start-30-minutes) (30 min)
2. Focus on [Type Safety tutorials](../../tutorials/index.md#3-type-safety-65-minutes) (~1 hour)
3. Jump to [Customization](../../workshops/index.md) and [Advanced Workshops](../../workshops/index.md)
4. Reference the [Systems documentation](../../users_guide/systems/index.md) for your domain

### For Migration

Coming from another library?

1. Read the relevant [Migration Guide](../../how_to_guides/migration/index.md)
2. Complete the [Workshop: Incremental Migration](../../workshops/advanced/incremental_migration.md)
3. Reference [Interoperability guides](../../how_to_guides/integration/index.md) as needed


## Tutorials: Your First 3 Hours with mp-units

Our **14 step-by-step tutorials** (~3.5 hours total) take you from "Hello World" to
confident mastery of core library features. Each tutorial is carefully crafted to:

- **Build progressively** — Each lesson builds on the previous one
- **Teach by example** — Learn through working code, not abstract theory
- **Provide instant feedback** — Interactive examples let you experiment immediately
- **End with key insights** — Every code example concludes with "What You Learned?" takeaways

<!-- markdownlint-disable MD013 -->
### Learning Path

#### 1. Quick Start (~30 minutes)

Hit the ground running with immediate hands-on experience:

- [**Your First Quantities**](../../tutorials/quick_start/first_quantities.md) — Create and manipulate quantities with units
- [**Simple Math**](../../tutorials/quick_start/simple_math.md) — Perform calculations and see dimensional analysis in action
- [**Building a Calculator**](../../tutorials/quick_start/building_calculator.md) — Assemble a real program putting concepts together

!!! success "Perfect for..."

    Developers who want to see what **mp-units** can do before diving deeper.

#### 2. Working with Units (~50 minutes)

Master unit conversions and value extraction:

- [**Unit Conversions**](../../tutorials/working_with_units/unit_conversions.md) — Convert seamlessly between compatible units
- [**Safe and Unsafe Conversions**](../../tutorials/working_with_units/safe_unsafe_conversions.md) — Handle truncation and understand explicit conversions
- [**Extracting Numeric Values**](../../tutorials/working_with_units/extracting_numeric_values.md) — Get numerical values safely when needed

!!! success "Perfect for..."

    Anyone who needs to work with multiple unit systems or integrate with legacy APIs.

#### 3. Type Safety (~65 minutes)

Experience the power of compile-time dimensional analysis:

- [**Compile-Time Protection**](../../tutorials/type_safety/compile_time_protection.md) — Watch the compiler prevent nonsensical operations
- [**Automatic Dimensional Analysis**](../../tutorials/type_safety/dimensional_analysis.md) — Let the compiler compute derived units
- [**Quantity Specifications**](../../tutorials/type_safety/quantity_specifications.md) — Use quantity hierarchies for stronger type safety
- [**Generic Interfaces**](../../tutorials/type_safety/generic_interfaces.md) — Write flexible, reusable code with `QuantityOf`

!!! success "Perfect for..."

    Developers who want to eliminate runtime errors and write self-documenting code.

#### 4. Affine Space (~55 minutes)

Understand the crucial distinction between points and quantities:

- [**Points and Quantities**](../../tutorials/affine_space/points_and_quantities.md) — Learn when to use `quantity_point` vs `quantity`
- [**Point Origins**](../../tutorials/affine_space/point_origins.md) — Work with multiple reference frames
- [**Temperature Handling**](../../tutorials/affine_space/temperature_handling.md) — Handle absolute vs relative temperatures correctly
- [**Temperature Applications**](../../tutorials/affine_space/temperature_applications.md) — Apply affine space concepts to real problems

!!! success "Perfect for..."

    Anyone working with temperatures, timestamps, GPS coordinates, or any absolute measurements.


## Workshops: 14 Hands-On Exercises

After completing the tutorials, level up with **14 workshops** featuring challenge-driven
learning. Each workshop presents real-world coding challenges that require you to apply
concepts from the [User's Guide](../../users_guide/index.md) to solve practical problems.

### What Makes Workshops Different?

Workshops aren't just tutorials with exercises—they're **deliberate practice** for advanced
patterns:

- **Exercise-driven** — Follow `TODO:` markers to implement solutions yourself
- **Real-world scenarios** — Work with production-like code, not toy examples
- **Progressive complexity** — Start with refactoring, advance to custom systems
- **Self-directed learning** — Review the User's Guide to find solutions
- **Practical patterns** — Learn techniques you'll use in actual projects

### Workshop Progression

#### Foundation Workshops

Build essential skills through refactoring and practical patterns:

- [**Refactor to Strong Types**](../../workshops/foundation/refactor_to_strong_types.md) — Transform weak types into strongly-typed quantities
- [**Generic, Type-Safe Interfaces**](../../workshops/foundation/generic_type_safe_interfaces.md) — Make APIs flexible yet safe with `QuantityOf`
- [**Extracting Numeric Values**](../../workshops/foundation/extracting_numeric_values.md) — Handle legacy interfaces requiring raw numbers
- [**Working with Temperatures**](../../workshops/foundation/working_with_temperatures.md) — Master absolute vs relative _temperature_ handling
- [**Affine Spaces and Origins**](../../workshops/foundation/affine_spaces.md) — Work with multiple coordinate systems
- [**Interop with std::chrono**](../../workshops/foundation/interop_with_std_chrono.md) — Bridge between time libraries seamlessly

!!! success "Perfect for..."

    Integrating **mp-units** into existing codebases and working with legacy systems.

#### Extensions Workshops

Create domain-specific systems of quantities and units:

- [**Custom Dimensionless Units**](../../workshops/extensions/custom_dimensionless_units.md) — Create strongly-typed counters for business domains (cartons, pallets, trucks)
- [**Typed Quantities of Same Kind**](../../workshops/extensions/typed_quantities_same_kind.md) — Distinguish _height_, _width_, and _turn radius_ to prevent argument confusion
- [**Custom Quantity Specifications**](../../workshops/extensions/custom_quantity_specifications.md) — Define domain-specific _energy_ types for elevator system design
- [**Custom Base Dimensions**](../../workshops/extensions/custom_base_dimensions.md) — Create financial dimensions (shares, currency) for portfolio tracking
- [**Distinct Quantity Kinds**](../../workshops/extensions/distinct_quantity_kinds.md) — Prevent mixing _fluid head_ and _water head_ in hydraulic engineering

!!! success "Perfect for..."

    Building specialized systems for your domain—aviation, robotics, finance, data engineering, etc.

#### Advanced Workshops

Master sophisticated patterns:

- [**Strongly-Typed Counts**](../../workshops/advanced/strongly_typed_counts.md) — Use dimensionless quantities for graphics coordinates and buffer indices
- [**Faster-than-Lightspeed Constants**](../../workshops/advanced/faster_than_lightspeed_constants.md) — Implement physical constants as compile-time units with automatic cancellation
- [**Incremental Migration**](../../workshops/advanced/incremental_migration.md) — Gradually adopt **mp-units** in legacy projects
<!-- markdownlint-enable MD013 -->

!!! success "Perfect for..."

    Advanced users working on complex systems requiring maximum type safety.


## Community and Feedback

We'd love to hear from you:

- **Questions?** Join discussions in our [GitHub Discussions](https://github.com/mpusz/mp-units/discussions)
- **Found an issue?** Report it on [GitHub Issues](https://github.com/mpusz/mp-units/issues)
- **Success story?** Share it on social media with `#mpunits` or `#cplusplus`
- **Suggestions?** We're always looking to improve our learning resources

### Help Us Improve

As you work through tutorials and workshops:

- Let us know if something is unclear
- Suggest additional exercises or topics
- Share patterns you discover
- Report broken Compiler Explorer examples


## Try It Today

No installation required. Open any tutorial or workshop, and you'll find interactive
code editors embedded right in the documentation. Start typing, and the code compiles
automatically. You'll be writing type-safe, dimensionally correct code within minutes.

**[Start Learning →](../../tutorials/index.md){ .md-button .md-button--primary }**

---

*Special thanks to [Matt Godbolt](https://xania.org) and the rest of the
[Compiler Explorer](https://godbolt.org) team for making interactive learning possible.*
