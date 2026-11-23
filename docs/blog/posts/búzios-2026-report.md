---
date: 2026-11-24
authors:
  - mpusz
categories:
  - WG21 Updates
comments: true
---

# Report from the Búzios 2026 ISO C++ Committee meeting

The Búzios 2026 ISO C++ Committee meeting in Brazil has just finished. C++26 failed its
DIS ballot, so most of the week went to the national body comments, and many of them were
about contracts. Quantities and units still got about a day in SG6 (Numerics). Below I
describe what happened there, and I ask for your opinion on a change to the multiply
syntax that two of my papers may force on the library.

<!-- more -->

## C++26 and contracts

The Draft International Standard (DIS) is the document the national bodies vote on before
a new C++ standard is published. This time they voted it down, and a large share of their
comments asked for contracts to be removed or changed. The plenary spent Monday and
Tuesday on what to do about it.

Contracts stay in the revised DIS. A poll to remove them didn't reach consensus (41 for,
90 against, 20 abstaining). To answer the comments, the committee added an always-enforced
label ([P4451R2](https://isocpp.org/files/papers/P4451R2.pdf)), so that a contract
assertion can be marked to be enforced in every build.
The revised DIS was approved for a new ballot (111 for, 9 against, 8 abstaining), which
takes eight weeks. If it passes, C++26 ships with contracts. I describe the dispute and
this week's outcome in more detail in my
[C++26 contracts series](https://train-it.eu/blog/series/cpp26-contracts/) on the Train IT
blog, most recently in
[C++26 contracts after the Búzios meeting](https://train-it.eu/blog/cpp26-contracts-after-buzios/).

**mp-units** already uses C++26 contracts. With the `contracts=std` Conan option (or
`MP_UNITS_API_CONTRACTS=STD` in CMake), the library checks its preconditions and
postconditions with C++26 contract assertions. This support is experimental, requires
GCC 16 with `-fcontracts`, and ships in 2.6.0. The Búzios polls don't affect it. The
always-enforced label is an addition, and the syntax that **mp-units** uses didn't change.

Another DIS change touches this library. "Pragmatic approach to standard structural types"
([P3861R1](https://wg21.link/p3861)) makes `std::string_view`, `std::span`, `std::tuple`,
`std::optional`, and `std::variant` structural types, so they can be used as template
arguments. `std::variant` was added to the paper during this meeting. The paper does it by
compiler magic. The library wording names these types as structural, a status that no
user-defined type with private members can get.

I don't like this approach. Special-casing a few standard types isn't a generic solution,
and library authors still have no easier way to make their own types structural. In
**mp-units**, `quantity` still has to keep its value in a public data member
(`numerical_value_is_an_implementation_detail_`) to be usable as a template argument. The
same applies to our users. If quantities are to be used as template arguments, a custom
representation type has to be a structural type too.

## What was voted in

Besides the DIS work, the closing plenary also took the usual polls for the C++29 working
draft. Here are the most important ones.

### Core Language (CWG)

- "`break label;` and `continue label;`" ([P3568R4](https://wg21.link/p3568)): a labeled
  loop can be left or continued from inside a nested loop without a flag variable or a
  `goto`.
- "Deduced return types for defaulted operator functions"
  ([P2952R5](https://wg21.link/p2952)): a defaulted operator can be declared with a
  placeholder return type (`auto`, or `auto&` for an assignment), as `operator<=>` already
  could.
- "Oo... adding a coherent character sequence to begin octal-literals"
  ([P0085R5](https://wg21.link/p0085)): a `0o` prefix for octal literals, so an octal
  number no longer has to be spelled with a leading `0`.
- "Addenda to the Undefined Behavior and IFNDR Annexes"
  ([P4284R2](https://wg21.link/p4284)): extends the catalog of undefined behavior and
  IFNDR adopted in Brno.

### Library (LWG)

- "Carry-less product: `std::clmul`" ([P3642R8](https://wg21.link/p3642)): carry-less
  multiplication, a building block of fast CRC and of GCM, as a standard function.
- "`any_view`" ([P3411R7](https://wg21.link/p3411)): a type-erased view.
- "`views::slice`" ([P3216R6](https://wg21.link/p3216)): a view of a sub-range given by
  two indices.
- "Hazard Pointer Synchronous Reclamation" ([P3427R9](https://wg21.link/p3427)): extends
  the hazard pointers added in C++26.
- "`get()`ing the pointer from `optional`" ([P4189R0](https://wg21.link/p4189)).

## Quantities and units progress

My papers were planned for LEWG this week, but the DIS work took the room, and they were
dropped from the agenda. SG6 was first cut from a day and a half to half a day. In the end
it met for the full day and a half (Thursday afternoon and all of Friday), and quantities
and units took most of it.

### SG6 (Numerics): absolute quantities

I presented R3 of [P4185](https://wg21.link/p4185), the paper that adds absolute
quantities as a third abstraction next to deltas and points. You can read the motivation
in [Introducing Absolute Quantities](introducing-absolute-quantities.md). This revision
came with implementation experience.

- **Vectors.** An absolute quantity is always a scalar. A vector has a zero, but not the
  ratio scale an amount needs. After the first session, I changed the rules so that a
  vector quantity such as displacement or force is implicitly a delta, and it no longer
  has to be wrapped in `delta<...>`. Writing `isq::velocity[m / s](vec)` is enough.
  Multiplying a vector by a bare unit (a vector times `N`) is ill-formed.
- **Products of deltas.** A lot of time went to what multiplying two deltas means. The
  volume of a box built from three deltas is physically an absolute, yet under the rules
  of R3 it comes out as a delta. The change of an area isn't the product of the changes of
  its sides either. Some suggested that the word "delta" is misleading for the cases where
  the values are really absolute amounts.
- **One value, two abstractions.** It was suggested that some results are both an absolute
  and a delta at the same time, and that the type system can't always pick the right one.
  A type that converts either way was discussed, but it would need to support every
  operator.
- **Generic code.** One participant asked how such types compose with generic code that
  expects plain numbers.

The feedback was mixed. Some saw the design as an improvement, others doubt it is the
right approach at all, and it isn't ready to be forwarded. I was asked for many more
examples, each showing the type that comes out of every operation.

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: P4185R3 explores an interesting direction. We encourage more work as we think the design space is not fully explored yet."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         3         |    3     |    1    |    2    |        0         |

As the next step, I plan to encode the defining equations of ISO 80000 as compile-time
tests, each with the expected abstraction of its result, and report how many come out
right.

### SG6 (Numerics): non-negative quantity specifications

[P4411R1](https://wg21.link/p4411) adds a flag to quantity specifications such as
_length_, _mass_, or _duration_ that says their absolute values can't be negative. With
absolute quantities, creating a negative absolute of such a quantity is then a contract
violation. The **mp-units** master branch (to ship in 2.6.0) already applies the flag to
points measured from a natural origin.

Most of the session went to the cases where the check behaves inconsistently.
`isq::speed(v)` is checked, but the same value created as `x * (m / s)` isn't, because a
unit expression carries no sign. Some definitions hide a sign that the type can't see,
like reactive power (`Q = U * I * sin(phi)`), which is negative for some angles even
though the voltage and current amplitudes aren't. One participant asked for a compile-time
error, instead of a run-time contract violation, when a value that may be negative is
assigned to a non-negative quantity. Another suggested a way to mark an equation as
possibly negative.

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: We encourage more work for getting non-negative quantity specs into absolute quantities as this would likely increase consensus on absolute quantities."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         4         |    1     |    1    |    1    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: We encourage more work for getting non-negative quantity specs even if absolute quantities doesn't progress, as this would likely increase consensus on the main quantities and units paper."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         0         |    5     |    2    |    1    |        0         |

Non-negativity makes most sense together with absolute quantities.

### SG6 (Numerics): conversions between integer representation types

Today the library follows `std::chrono::duration`, which doesn't compare integer types,
so `quantity<m, std::int16_t> dist = 100'000 * m;` compiles and wraps around to -31072 m.
[P4408R1](https://wg21.link/p4408) proposes that a conversion between two integer
representation types be implicit only when the destination can represent every value of
the source.

The chair asked for one principle for every arithmetic conversion, either the full
value-preserving rule (as `std::simd` does), which would also make `int` to `float`
explicit, or no check beyond what the representation type itself does. Another participant
suggested allowing no implicit conversion at all. There was also a concern that a rule
depending on the widths of the types on a given platform makes correctness non-portable,
as `long` differs between targets.

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: Implicit conversions between builtin representation types must be value-preserving (see P2509 or [simd])."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         6         |    2     |    0    |    0    |        0         |

SG6 chose the value-preserving rule. The next revision of P4408 will use the trait from
[P2509](https://wg21.link/p2509), the same rule that `std::simd` follows, so it covers
more than conversions between integer types. For example, a conversion from `int` to
`float` will no longer be implicit either. I plan to implement it in V3. This is a
breaking change: code that relies on such implicit conversions will need an explicit
`value_cast`.

### SG6 (Numerics): value conversions with a rounding policy

At the end of Friday, I showed how [value
conversions](../../users_guide/framework_basics/value_conversions.md) changed since
[P3045](https://wg21.link/p3045) left SG6. A conversion that can't be exact now takes a
rounding policy, as in `q.in(km, rounded_up)`, instead of a separate forcing function. The
chair saw it as an improvement, said it doesn't need to come back to SG6, and initially
found the name `rounded` unclear. It helped when I explained that it behaves like
`std::chrono::round`.
Another participant suggested also accepting `std::float_round_style`.

## The multiply syntax

In **mp-units** today, `42 * km / h` is two operations: `(42 * km) / h`. The library
allows multiplying and dividing a quantity by a unit, so the units of a derived quantity
can be written without parentheses. In the prototype that implements both P4185 and P4411,
the first multiplication already creates an absolute quantity and checks its sign before
the second unit is applied. The order of the units then decides whether a mistake is
caught:

```cpp
#include <mp-units/systems/si.h>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

quantity q1 = -1. * A * s;    // OK: (-1. * A) is a current, which may be negative
quantity q2 = -1. * s * A;    // contract violation: (-1. * s) is a negative duration
quantity q3 = -1. * (A * s);  // OK: one unit, no intermediate quantity

quantity v1 = -1. * m / s;    // contract violation: (-1. * m) is a negative length
quantity v2 = -1. * (m / s);  // unchecked: the kind of m / s carries no sign
```

`q1` and `q2` hold the same value of the same type, but only one of them is checked. The
fix I see is to allow the multiply syntax only for `number * unit`, so that a derived unit
always needs parentheses:

```cpp
quantity speed = 42 * (km / h);  // OK
quantity bad = 42 * km / h;      // would no longer compile
```

This came up in SG6 during the P4411 session, and two participants said they prefer the
parentheses or could live with them. Still, it changes a lot of existing code.
**mp-units** required these parentheses once before,
and we dropped the rule because it proved too verbose and confusing for users. The
parentheses would also remove a surprise we have today, where `60 * km / 2 * h` gives
`30 km·h` instead of `30 km/h`.

Would you accept writing `42 * (km / h)` everywhere to get checks that don't depend on the
order of the units, or do you see a better way? Please let me know in the comments below.

## What is still ahead

None of the three papers discussed this week was forwarded. Each of them needs another
revision and another session in SG6. Only then can it go to LEWG, together with the main
paper, [P3045](https://wg21.link/p3045), which still needs its design review there and its
wording review in LWG.

Two more papers are already in the mailing and weren't discussed yet:
[P4213](https://wg21.link/p4213) with the minimal set of SI definitions, and
[P4412](https://wg21.link/p4412) with range-validated quantity points. Eight more are
drafted and wait for a future mailing. They cover comparing quantities against zero,
component vectors of vector quantities, integer division, digit grouping and composing
formatters in `std::format`, vector, tensor, and complex operations, runtime frame
projections, and the wording for the whole library. Papers on mathematical functions, and
probably on more systems of units, still have to be written.

The library needs work too. V3, which implements the absolute quantities described above,
has to be finished and released. Then it needs production experience.

## Supporting the project

All of this needs time and resources. Doing it alone is hard, so I am looking for
sponsors. If your company depends on **mp-units** or wants quantities and units in the C++
standard, please [get in touch](https://train-it.eu/contact) to discuss a sponsorship. A
company can also book my
[Domain Modeling with the C++ Type System](https://train-it.eu/workshops/domain-modeling-with-types/)
workshop for its team, which also helps fund this work. Individuals can support the
project through [GitHub Sponsors](https://github.com/sponsors/mpusz).

*[CWG]: Core Language Working Group
*[DIS]: Draft International Standard
*[IFNDR]: ill-formed, no diagnostic required
*[LEWG]: Library Evolution Working Group
*[LWG]: Library Working Group
*[SG6]: Study Group 6 - Numerics
