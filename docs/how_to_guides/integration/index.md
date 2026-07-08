# Integration

Guides for integrating **mp-units** with existing codebases, custom types, third-party
libraries, and various compiler environments.

## Available Guides

<!-- markdownlint-disable MD013 -->
- [Working with Legacy Interfaces](working_with_legacy_interfaces.md) - Integrate with existing code that uses raw numeric types
- [Using Custom Representation Types](using_custom_representation_types.md) - Define your own numeric types as quantity representations
- [Using a Linear Algebra Library](using_linear_algebra_libraries.md) - Use Eigen, GLM, or Blaze vector/matrix types as representations via the shipped plugins
- [Converting to and from External Quantity Types](converting_external_quantity_types.md) - Bridge with strong types, other units libraries, and `std::chrono`
- [Wide Compatibility](wide_compatibility.md) - Maximize portability across different compilers and environments
<!-- markdownlint-enable MD013 -->

## When to Use These Guides

Use these integration guides when you:

- Need to integrate mp-units into existing codebases
- Work with legacy APIs expecting raw numeric types
- Want to use custom numeric types (fixed-point, arbitrary precision, etc.)
- Need interoperability with other libraries
- Target multiple compilers or environments
- Require maximum portability and compatibility
