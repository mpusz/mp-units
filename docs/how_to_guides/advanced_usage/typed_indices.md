# Type-Safe Indices and Offsets

This guide shows how to apply **mp-units** to a common programming domain problem:
safely distinguishing **indices** (positions in a sequence) from **offsets**
(displacements between positions), and providing unit-aware arithmetic for both —
including the difference between SI kilo- and IEC kibi-element counts, and safe
handling of 0-based vs 1-based indexing conventions.

For background on the concepts used here, see:

- [Dimensionless Quantities](../../users_guide/framework_basics/dimensionless_quantities.md) —
  `is_kind` and its semantics
- [The Affine Space](../../users_guide/framework_basics/the_affine_space.md) —
  `quantity_point`, origins, and their algebra
- [Workshop: Affine Spaces and Origins](../../workshops/foundation/affine_spaces.md) —
  hands-on exercises with absolute and relative origins
- [Workshop: Strongly-Typed Counts](../../workshops/advanced/strongly_typed_counts.md) —
  the same `is_kind` pattern for screen coordinates and counts


## The Core Insight: Points vs. Displacements

In physics, you cannot add two _positions_ — only subtract them to get a
_displacement_. You can add a _displacement_ to a _position_ to get a new _position_.
This is the **affine space** model, and it applies equally to indices and offsets in
software:

| Concept    | Affine space role | Examples                                       |
|------------|-------------------|------------------------------------------------|
| **Index**  | point             | `v[3]`, `line 42`, `row 0`                     |
| **Offset** | displacement      | `3 elements`, `42 bytes`, `row_stride × steps` |

| Operation         | Result | Valid?           |
|-------------------|--------|------------------|
| `index - index`   | offset | ✅                |
| `index + offset`  | index  | ✅                |
| `offset + offset` | offset | ✅                |
| `index + index`   | —      | ❌ nonsensical    |
| `int` as index    | —      | ❌ no type safety |

A 0-based index and a 1-based index are both _positions_, but measured from different
**origins**. Encoding those origins in the type eliminates an entire class of
off-by-one errors at compile time.


## Setting Up the Domain

### Offset quantity and element unit

An offset is a dimensionless quantity with its own **kind**, preventing accidental
mixing with plain numbers or other dimensionless quantities:

```cpp
#include <mp-units/framework.h>
#include <mp-units/systems/iec/binary_prefixes.h>
#include <mp-units/systems/si/prefixes.h>
#include <iostream>
#include <span>

using namespace mp_units;

namespace seq {

inline constexpr struct offset : quantity_spec<dimensionless, is_kind> {} offset;

// "element" is the base unit of the offset kind
inline constexpr struct element : named_unit<"el", one, kind_of<offset>> {} element;
inline constexpr auto el   = element;
inline constexpr auto kel  = si::kilo<element>;   // 1 kel  = 1000 el  (SI kilo)
inline constexpr auto Kiel = iec::kibi<element>;  // 1 Kiel = 1024 el  (IEC kibi)

}  // namespace seq
```

The `is_kind` specifier makes `offset` a distinct dimensionless kind: same dimension
as `one`, but the type system prevents mixing it with arbitrary dimensionless values.
The named `element` unit gives quantities meaningful output (`3 el`, `2 kel`) and
allows both SI and IEC scaled prefixes to attach cleanly.

### Indices and their origins

An index is a _position_ in the sequence. In **mp-units**' affine space model, a
position is a `quantity_point` — a value measured from a **point origin**.

We define two origins: one for 0-based and one for 1-based indexing:

<!-- TODO: In V3, `index` will be expressible directly as `point_for<offset>`.
     For now, reuse the offset quantity spec for the point's quantity kind. -->

```cpp
namespace seq {

inline constexpr auto index = offset;

// 0-based origin: the natural zero of the sequence.
inline constexpr struct index0_origin : absolute_point_origin<index> {} index0_origin;

// 1-based origin: sits one position *before* index0_origin so that the
// displacement arithmetic resolves correctly:
//   index1_origin + 1*el  ==  index0_origin + 0*el   (first element)
//   index1_origin + 5*el  ==  index0_origin + 4*el   (fifth element)
//
// WARNING: index1_origin itself is NOT a valid index — it represents
// a non-existent "element zero" in a 1-based scheme.
// Always use idx1() to construct 1-based index points.
inline constexpr struct index1_origin : relative_point_origin<index0_origin - 1 * el> {} index1_origin;

}  // namespace seq
```

!!! question "Why is `index1_origin` not at position 1?"

    Unlike `celsius_origin`, where `celsius_origin + 0*deg_C` is meaningfully "0°C",
    `index1_origin + 0*el` has no valid interpretation — there is no zeroth element
    in a 1-based scheme. `index1_origin` is a pure mathematical artifact: the "ghost"
    origin required so that adding `1*el` to it resolves to the same absolute position
    as `index0_origin + 0*el`.

    Constructing `quantity_point{0 * el, index1_origin}` directly is valid C++ but
    semantically meaningless. Always go through the `idx1()` factory below.

### Factory functions

Instead of exposing the origins directly, provide factory functions that enforce
preconditions and hide the `quantity_point` construction details:

```cpp
namespace seq {

template<ReferenceOf<offset> auto R>
QuantityPointOf<index0_origin> auto idx0(quantity<R, int> q)
{
  MP_UNITS_EXPECTS(q >= q.zero());   // non-negative offset
  return quantity_point{q, index0_origin};
}
// Overload for plain quantity<one, int> — wraps in offset first
QuantityPointOf<index0_origin> auto idx0(quantity<one, int> q) { return idx0(offset(q)); }

template<ReferenceOf<offset> auto R>
QuantityPointOf<index1_origin> auto idx1(quantity<R, int> q)
{
  // `1*el`, not `q.zero()`: 1-based indices start at 1.
  // When `q` is in a scaled unit (e.g. kel), this comparison triggers a unit conversion.
  MP_UNITS_EXPECTS(q >= 1 * el);
  return quantity_point{q, index1_origin};
}
QuantityPointOf<index1_origin> auto idx1(quantity<one, int> q) { return idx1(offset(q)); }

}  // namespace seq
```

!!! info

    Using `QuantityPointOf<index0_origin>` / `QuantityPointOf<index1_origin>` is more precise
    than the generic `QuantityPointOf<index>`: it constrains both the quantity kind _and_ the
    origin, so a call site that requires specifically a 0-based index gets a compile error if
    a 1-based index is passed, and vice versa.

### Extracting the container subscript

To pass an index point to a standard container (`v[i]`), subtract `index0_origin` to
obtain a 0-based offset and then extract its numeric value:

```cpp
template<QuantityPointOf<seq::index> QP>
constexpr std::size_t to_size_t(QP qp)
{
  return static_cast<std::size_t>((qp - seq::index0_origin).in(one));
}
```

This works for any index point regardless of origin — the library resolves the origin
difference automatically. `.in(one)` returns a `quantity<one>` (a dimensionless quantity
with the identity unit), which `static_cast` converts to `std::size_t` via `quantity<one>`'s
explicit conversion to its underlying type.


## Using the Index Types

### Type-safe container access

```cpp
void print_idx(std::span<const int> rng, QuantityPointOf<seq::index> auto idx)
{
  const auto i = to_size_t(idx);
  std::cout << "v[" << i << "] = " << rng[i] << "\n";
}

int main()
{
  using namespace seq;
  std::vector v = {10, 20, 30, 40, 50};

  print_idx(v, idx0(0));   // 0-based: first  element  → "v[0] = 10"
  print_idx(v, idx0(2));   // 0-based: third  element  → "v[2] = 30"
  print_idx(v, idx1(1));   // 1-based: first  element  → "v[0] = 10"
  print_idx(v, idx1(3));   // 1-based: third  element  → "v[2] = 30"
}
```

### What doesn't compile?

```cpp
print_idx(v, 0);                 // ERROR: raw int is not a quantity_point
print_idx(v, idx0(0) + idx0(1)); // ERROR: cannot add two index points
```

Subtracting a 0-based index from a 1-based index compiles and gives the correct
displacement (the library resolves the origins algebraically), so cross-convention
arithmetic is safe when it is meaningful.

### Size vs index: the same asymmetry as in physics

Sizes and counts are `quantity` values (displacements); indices are `quantity_point`
values (positions).  The half-open range `[idx0(0 * el), idx0(v.size() * el))` makes
this concrete: you can form the range endpoints, subtract them to get the size, and
advance through them with offsets — but you cannot accidentally use a size as an index.

### Unit-aware arithmetic: `kel` vs `Kiel`

Offsets carry units, so arithmetic across SI and IEC prefixes is exact:

```cpp
using namespace seq;

quantity o_si  = idx0(2 * kel)  - idx0(0 * kel);   // SI kilo:  2000 el
quantity o_iec = idx0(2 * Kiel) - idx0(0 * Kiel);  // IEC kibi: 2048 el

std::cout << "o_si: " << o_si << " = " << o_si.in(el) << '\n';     // o_si: 2 kel = 2000 el
std::cout << "o_iec: " << o_iec << " = " << o_iec.in(el) << '\n';  // o_iec: 2 Kiel = 2048 el
```

This mirrors the real storage distinction: two "kilobytes" (SI) = 2000 bytes, two
"kibibytes" (IEC) = 2048 bytes — applied here to element counts.  Mixing `kel` and
`Kiel` is detected at compile time because they are distinct scaled units.


## Strides

A **stride** is an `offset` quantity: the displacement in the flat layout between
successive elements along a given dimension. Define it as a named quantity spec so it
cannot be accidentally swapped with a plain offset or raw count:

```cpp
namespace seq {

inline constexpr struct stride : quantity_spec<offset> {} stride;

}  // namespace seq
```

### 1D strided access

Access the element `step` strides away from a starting index:

```cpp
QuantityPointOf<seq::index> auto strided_at(QuantityPointOf<seq::index> auto start,
                                            quantity<seq::stride[seq::el], int> stride,
                                            int step)
{
  return seq::idx0((start - seq::index0_origin) + step * stride);
}
```

`step` is a plain scalar (the number of stride-steps to take); multiplying
`int × quantity<stride[el]>` yields a `quantity<stride[el]>` (a subtype of `offset`),
which is then added to the starting displacement.

### 2D row-major indexing: typed row and column

For a 2D array stored row-major, row and column indices are both `offset` quantities
but should be incompatible — swapping them is a silent bug. Define named sub-kinds for
each dimension:

<!-- TODO: In V3 these will be expressible as point_for<row_offset> / point_for<col_offset> -->

```cpp
namespace seq {

inline constexpr struct row_offset : quantity_spec<offset> {} row_offset;
inline constexpr struct col_offset : quantity_spec<offset> {} col_offset;
inline constexpr auto row_index = row_offset;
inline constexpr auto col_index = col_offset;

}  // namespace seq
```

`flat_index` now constrains each parameter to its correct dimension — swapping row and
column arguments is a compile error (if typed quantities are used):

```cpp
QuantityPointOf<seq::index> auto flat_index(QuantityPointOf<seq::row_index> auto row,
                                            QuantityPointOf<seq::col_index> auto col,
                                            quantity<seq::stride[seq::el], int> row_stride)
{
  // Extract the scalar row count: multiplying two offset quantities would give
  // a compound quantity, not a plain offset — so we pull out the numeric value first.
  int row_n = (row - seq::index0_origin).numerical_value_in(seq::el);
  return seq::idx0(row_n * row_stride + (col - seq::index0_origin));
}
```

```cpp
using namespace seq;
const quantity row_stride = stride(4 * el);  // 4 columns per row

// idx0 accepts row_offset and col_offset because they are subtypes of offset
quantity_point flat = flat_index(idx0(row_offset(1 * el)), idx0(col_offset(2 * el)), row_stride);
std::cout << to_size_t(flat) << "\n";  // 1 * 4 + 2 = 6

// flat_index(idx0(col_offset(2 * el)), idx0(row_offset(1 * el)), row_stride);  // ERROR: row/col swapped
```

!!! question "Why `.numerical_value_in()` for `row_n`?"

    `(row - origin)` is `quantity<row_offset[el]>` — a count of rows.
    `row_stride` is `quantity<stride[el]>` — a number of elements per row.
    Their product would be a compound `row_offset × stride` quantity, not a plain `offset`.
    Extracting `row_n` as a plain scalar with `.numerical_value_in(el)` breaks the compound:
    `row_n × row_stride` then gives `quantity<stride[el]>` (a subtype of `offset`) that the
    library can add to the column displacement.


## Going Further

### Byte-level offsets

When working with raw memory, define `byte` as a primitive unit within the `seq::offset`
kind, then express a typed element as a multiple of bytes:

```cpp
namespace seq {

inline constexpr struct byte : named_unit<"B", one, kind_of<offset>> {} byte;
inline constexpr auto B = byte;

// 1 float element = sizeof(float) bytes
inline constexpr struct float_element : named_unit<"float", mag<sizeof(float)> * byte> {} float_element;
inline constexpr auto fl = float_element;

}  // namespace seq
```

With this in place, the library automatically converts between element counts and byte
counts, and prints meaningful output (`128 B` vs `32 float`), propagating the
`sizeof` factor at compile time:

```cpp
quantity offset_el = idx0(32 * seq::fl) - idx0(0 * seq::fl);
std::cout << offset_el.in(seq::B) << "\n";  // 128 B
std::cout << offset_el.in(seq::fl) << "\n"; // 32 float
```

Defining `byte_unit` as the base and `float_element` as a multiple avoids any
collision with `iec::byte` (which belongs to a different quantity kind) and makes
the direction of the conversion self-evident: a `float` element is `sizeof(float)`
bytes, so `1 fl = 4 B`.

### Modular (wrapping) indices

Circular indices — wrap-around buffer positions, angles in $[-\pi, \pi]$, etc. —
require a custom representation type that performs modular arithmetic on the stored
value. The index-as-affine-space model described here is fully compatible with such
a representation: the origin and offset infrastructure remains unchanged.

### Indices into different containers

If indices into different containers must be mutually incompatible (to prevent
cross-container access bugs), define each as a plain named subtype of `seq::offset`
— without `is_kind`:

```cpp
inline constexpr struct vertex_index : quantity_spec<seq::offset> {} vertex_index;
inline constexpr struct texcoord_index : quantity_spec<seq::offset> {} texcoord_index;
```

Omitting `is_kind` is intentional and important: because they still derive from
`seq::offset`, they satisfy `QuantityOf<seq::offset>` and `QuantityPointOf<seq::index>`,
so `idx0()`, `to_size_t()`, and subtraction to get a `seq::offset` displacement all
continue to work.  The type safety comes from them being distinct named quantity specs:
they require explicit conversion to each other, so accidentally passing a `texcoord_index`
point where a `vertex_index` point is expected is a compile error.

Note that `vertex_index + texcoord_index` is **valid** — both are subtypes of the same
`seq::offset` kind, so the library permits their arithmetic and comparison.
The type safety is at the function-parameter level: a function taking
`QuantityPointOf<vertex_index> auto` will reject a `texcoord_index` point at the call
site.


## See Also

- [The Affine Space](../../users_guide/framework_basics/the_affine_space.md) —
  detailed explanation of `quantity_point`, origins, and their algebra
- [Dimensionless Quantities](../../users_guide/framework_basics/dimensionless_quantities.md) —
  `is_kind`, the unit `one`, and their superpowers
- [Workshop: Affine Spaces and Origins](../../workshops/foundation/affine_spaces.md) —
  hands-on exercises with absolute and relative origins
- [Workshop: Strongly-Typed Counts](../../workshops/advanced/strongly_typed_counts.md) —
  the `is_kind` pattern for coordinates and counts
