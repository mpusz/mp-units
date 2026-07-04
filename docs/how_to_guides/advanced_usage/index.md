# Advanced Usage

Advanced techniques for using **mp-units** in sophisticated scenarios beyond standard
quantity calculations.

## Available Guides

<!-- markdownlint-disable MD013 -->
- [Pure Dimensional Analysis](pure_dimensional_analysis.md) - Use dimensions without specific units for symbolic computation, compile-time validation, and custom arithmetic types
- [Type-Safe Indices and Offsets](typed_indices.md) - Model container indices and offsets as quantities with point origins, covering 0-based vs 1-based indexing, SI vs IEC element prefixes, and stride arithmetic
- [Decompose a Vector Quantity into Components](decompose_vector_quantity.md) - Split a vector quantity into named, strongly-typed 1D-vector component quantities, covering the quantity hierarchy, `get<Idx>`/`get<QS>`, structured bindings, and the representation requirements
- [Represent an Axial Vector as an Antisymmetric Tensor](tensor_representation_of_axial_vectors.md) - Model a quantity the ISQ defines as a vector (e.g. angular velocity) as a skew-symmetric tensor in your own domain, bridged to `isq::angular_velocity` with an explicit `hat`/`vee` dual
- [Ensure Ultimate Safety](ultimate_safety.md) - Combine `constrained` reps, `constraint_violation_handler`, and `check_in_range` for guaranteed bounds enforcement
- [Working With Nondimensionalized Physics](nondimensionalization.md) - Use mp-units alongside natural units and dimensionless quantities, covering the complete scale-in/scale-out workflow and tradeoffs
<!-- markdownlint-enable MD013 -->

## When to Use These Guides

Use these advanced techniques when you:

- Need dimensional analysis without actual quantity values
- Build symbolic computation or automatic differentiation systems
- Validate dimensional consistency at compile-time
- Work with template metaprogramming involving dimensions
- Implement custom arithmetic types with dimensional correctness
- Work in nondimensionalized or natural-unit systems and want to retain strong typing
