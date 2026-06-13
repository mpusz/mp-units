---
date: 2026-06-13
authors:
  - mpusz
categories:
  - WG21 Updates
comments: true
---

# Report from the Brno 2026 ISO C++ Committee meeting

The Brno 2026 ISO C++ Committee meeting has just finished. It was the first
meeting of the C++29 cycle, and a surprising amount already landed in the working
draft. Below I share the highlights voted in during the closing plenary, followed
by an update on the quantities and units standardization effort.

<!-- more -->

## What Was Voted In

The meeting closed with the usual Saturday plenary, where the Core Language
Working Group (CWG) and the Library Working Group (LWG) forwarded a long list of
motions into the C++29 working draft. Most were defect reports and smaller
refinements, but a few stand out. Here are my personal highlights.

### Core Language (CWG)

- "Undefined Behavior and IFNDR Annexes" ([P3596R3](https://wg21.link/p3596)): the
  big one. It adds two annexes that catalog, in a single place, every case of
  undefined behavior and every case of ill-formed, no diagnostic required (IFNDR) in
  the language. That information was previously scattered across the whole document,
  so nobody could see the full surface of ways a program can silently go wrong. The
  annexes are a non-normative reference that links back to the normative wording, and
  every entry carries a stable name (for example, `lifetime.outside.pointer.delete`)
  so the main text and the annex cross-link, and so other efforts gain a shared
  vocabulary for talking about a specific hazard. This is foundational safety work:
  you cannot systematically reduce, diagnose, or build tooling against undefined
  behavior you have not first enumerated, and the mitigation catalog in
  [P3100](https://wg21.link/p3100) already builds on these names.
- "Contracts for C++: Virtual functions" ([P3097R3](https://wg21.link/p3097)):
  extends the brand-new C++26 contracts facility to virtual functions, closing one
  of the most controversial gaps in the feature.
- "Designated-initializers for Base Classes" ([P2287R6](https://wg21.link/p2287)):
  designated initialization now works for aggregates with base classes, so you
  can name inherited members instead of relying on positional initialization.
- "Defaulting Postfix Increment and Decrement Operations"
  ([P3668R4](https://wg21.link/p3668)): you can `= default` postfix `++`/`--`
  once the prefix version is available, removing a classic chunk of boilerplate. The
  matching library wording landed via [P3785R1](https://wg21.link/p3785).

### Library (LWG)

- "Fix the default floating-point representation in `std::format`"
  ([P3505R4](https://wg21.link/p3505)): corrects the default formatting of
  floating-point values so the output matches expectations - this directly improves
  how most quantities will be printed (`double` is used by default).
- "Deprecating signed character types in iostreams"
  ([P3154R3](https://wg21.link/p3154)): deprecates streaming `signed char` and
  `unsigned char` as characters. Today such a value streams as a character glyph
  rather than its number, so to print quantities backed by 1-byte integer types
  correctly we currently promote the value to `int` before streaming it. Once this
  deprecation takes effect, we will be able to drop that workaround.
- "Thread attributes" ([P2019R9](https://wg21.link/p2019)): lets you set a name and
  stack size when creating a thread.
- "Better Lookups for `map`, `unordered_map`, and `flat_map`"
  ([P3091R6](https://wg21.link/p3091)): convenient lookups that return an
  optional-like value instead of forcing you to juggle iterators and `end()`.
- "The unexpected in `std::expected`" ([P3798R1](https://wg21.link/p3798)): adds a
  `has_error()` member function to `std::expected` to complement the existing
  `has_value()` functionality.

This is only a small, opinionated selection. Plenty of other defect reports and
fixes were adopted as well.

## Quantities and Units Progress

This was a productive meeting for the quantities and units effort, and I left Brno
encouraged. The headline is that [P3045](https://wg21.link/p3045), the core library
proposal, cleared SG6 (Numerics) and was forwarded to LEWG, so the design now moves
into the main library evolution track. Alongside it,
[P4185](https://wg21.link/p4185) (the mathematical model, including the new absolute
quantities) earned good feedback and a clear encouragement to do more work,
[P3094](https://wg21.link/p3094) (`std::basic_fixed_string`), a small building block
the library leans on, had its design reviewed in LEWG, and SG20 (Education) walked
through the quantities and units teaching material.

C++29 is still the target. There is real work left before the library is ready to
land in the standard, but every session pushed it forward. Below I share what
happened in each room.

### SG6 (Numerics): Representation Types and Forwarding to LEWG

On Tuesday I presented [P3045R9](https://wg21.link/p3045) to SG6 (Numerics). We
reviewed a new chapter added in this revision,
[Representation Types](http://wg21.link/p3045#representation-types).
It describes the requirements for the type in which a quantity actually stores its
number. This is analogous to the representation type in `std::chrono`, but the
problem is richer here. A quantity may be backed not only by a scalar, but also by
a complex number, a vector, or a tensor. Each of those may be simple, or more
advanced and model uncertainties, safe integrals, bounded numeric types, and so on.

R9 introduced a hierarchy of concepts that constrain those representation types,
and I flagged upfront that the addition could be controversial. It turned out the
concepts were less controversial than I expected.

The discussion was detailed and very useful. The main threads were:

- **Constrain vs. enumerate.** Should the library constrain representation types
  with concepts, so that any conforming numeric type works, or only allow a fixed,
  known set of representations (as `std::simd` does)? The room favored the
  constraining approach, consistent with the proposal.
- **Granularity of the concepts.** Most of the concepts (`Addable`, `ScalableWith`,
  and friends) are exposition-only. There was a healthy debate about how
  fine-grained they should be, whether something like division belongs in them at
  all, and a reminder that LEWG tends to prefer coarser "what you can do" concepts
  over fine-grained ones. Since these are exposition-only, vendors can implement
  them as they wish (for example, as variable templates). It was also stated that
  it is better to derive concepts from the algorithms they need to serve than the
  other way around, which is exactly the case in this proposal.
- **Naming the vector magnitude.** A lot of energy went into what to call the
  length/magnitude operation (`abs`, `modulus`, `magnitude`, `norm`, `l2norm`) and
  the tension between physics conventions, the names used in linear algebra
  libraries, and the names already in the standard. In the end we decided to stay
  with the physics convention proposed in the paper.
- **Integer scaling and overflow.** I walked through how scaling works, including
  the use of a wider intermediate type (e.g., 128-bit) for integer representations
  to avoid intermediate overflow. This raised a broader question: should a
  quantities library take on integer-overflow avoidance at all, or leave it to a
  dedicated integer type? The room agreed that this is a safety-first library and
  that we should go the extra mile to prevent overflows, as proposed in the paper.

Three polls were taken:

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: SG6 agrees with constraining representation types rather than allowing a specific set of representation types."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         7         |    8     |    0    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: The constraints as shown in P3045R9 Section 12 match SG6 expectations for the numeric types that a user might want to use."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         2         |    5     |    2    |    2    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: Forward P3045R9, including Section 12 (not precluding further work in separate papers), to LEWG."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         4         |    4     |    1    |    2    |        1         |

The third poll forwarded the paper to LEWG 🎉, which means we will discuss the
library design details in the evolution group in Brazil.

### SG6 (Numerics): Absolute Quantities

[P4185](https://wg21.link/p4185) is the home of the design I am personally most
excited, and most nervous, about. I have been sketching the **mp-units** V3 design
for quite a while now, and this paper describes its highlights. We discussed it in
SG6 on both Tuesday and Thursday.

mp-units V2 has two abstractions: `quantity` (a delta/displacement) and
`quantity_point` (a point on a scale). The new paper argues we need at least one
more, to model the (potentially **non-negative**) **absolute** quantities that
represent a total amount measured from the natural zero. You can read more in the
[Introducing Absolute Quantities](introducing-absolute-quantities.md) blog article.

The motivating example is a physical equation such as the ideal gas law evaluated
at, say, 28 °C. Under [P3045R8](https://wg21.link/p3045r8) alone this is cumbersome
because we lack the proper abstraction. We can try to use points, but points can't
take part in arithmetic with other quantities, so we have to reach for
`quantity_from_zero` (or subtract `absolute_zero`) just to get a value we can
compute with. An absolute quantity is always anchored at true physical zero (a delta
from zero), which makes it a natural match for these use cases. It is **not** an
affine space, so it cannot use offset units like degree Celsius or Fahrenheit.

The arithmetic follows physical intuition. Subtracting two absolute _masses_ yields a
(possibly negative) _delta-mass_. Subtracting two absolute _lengths_ yields a
_delta-length_, and to pass that to an interface expecting an absolute _length_ you
call `.absolute()` (or `abs(q)`) to satisfy the required abstraction.

This was, as expected, the most debated part of my week:

- **Teachability and scope.** One SG6 member pushed back that three kinds of
  quantities, each with its own rules and tables, is a lot of mental complexity for
  an already complex field, and was unconvinced it is needed beyond temperature.
- **Strong support too.** Others took the opposite view, roughly "you've nailed
  this; get it right, take your time, and ship it." It was also noted that absolute
  quantities fill a real logical hole: most scalar quantities truly _are_ an amount
  of something.
- **Worked examples.** Some SG6 experts raised averaging a set of temperatures and
  computing a center of mass, which led to a good discussion of how those
  operations live in the delta domain (formally subtracting the domain's zero), and
  whether the defaults make them harder than necessary.
- **Range validation.** The paper also contains ranged quantity points. We
  discussed two kinds: the ones that just verify the quantity value is within
  bounds (and how we handle failure cases), and the ones that take a special action
  (e.g., wrap around) on range overflow. The latter are crucial to implementing
  many geography- and navigation-related quantities such as _longitude_, _bearing_,
  _heading_, and _elevation_. We also discussed how to handle measurement noise and how
  to report corrupted readouts (e.g., via `NaN`). There was appetite to split this
  into its own paper so it can advance independently. More on this is in the
  [Range-Validated](range-validated-quantity-points.md) blog article.

Two polls were taken on Thursday:

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: Encourage implementation (aka more work) on Section 6 “Absolute quantities” in P4185R1. SG6 believes this approach is likely to increase consensus of P3045R8 “Quantities and units library”."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         6         |    1     |    2    |    0    |        1         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "POLL: Encourage a split of Section 8 “Range validated quantity points” in P4185R1 into its own paper so that we can forward it to LEWG. SG6 believes this approach is likely to increase consensus of P3045R8 “Quantities and units library”."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         3         |    5     |    1    |    1    |        0         |

This is exactly the feedback I was hoping for: clear encouragement to keep building
absolute quantities, and a concrete plan to factor out range validation. I aim to
have absolute quantities implemented in **mp-units** and shipped as V3 before the
next meeting in Brazil.

### SG20 (Education): Teaching Quantities and Units

Also on Thursday, I presented [P3045](https://wg21.link/p3045) to SG20 (Education),
walking the group through the whole story: a quick domain introduction based on
ISO/IEC 80000, the API overview, the affine space abstractions, defining custom
units, the ISQ quantity hierarchy, the safety features, and, most importantly for
this room, the teaching material and per-audience guidance in the paper.

The discussion was lively and the feedback was excellent. A few themes stood out:

- **The compiler is the teacher.** A recurring observation was that students are
  guided by the compiler. A wrong mental model simply does not compile, which is
  exactly the moment they come to the tutor and learn _why_. You teach the syntax
  and the basics, and the type system enforces the semantics.
- **Safety motivation resonates.** The classic cautionary tales, the Mars Climate
  Orbiter and the Ariane 5 integer-overflow loss, were called out as great
  motivating examples for students.
- **Beyond physics.** I was asked whether the library is limited to physical units
  or also covers domains with conversions such as finance. It works for anything
  that can be counted, including currencies, where conversion rates are a runtime
  property tied to a point in time. I showed the
  [currency example](../../examples/currency.md) from our documentation.
- **Name collisions across domains.** A concern was raised about unit-name
  proliferation (e.g., a "cup" in the cooking domain). Proper namespacing handles
  this cleanly, and it is not WG21's job to standardize every unit in existence.
  Real usage can inform which units are worth adding.
- **The target-audience table** (application developers, unit authors, domain
  modelers, and deep integrators) was singled out as particularly useful.

The chair noted that the paper has some of the best teaching sections he has seen,
which was wonderful to hear. No polls were taken.

### LEWG (Library Evolution): `std::basic_fixed_string`

Additionally, on Wednesday, LEWG reviewed [P3094](https://wg21.link/p3094)
(`std::basic_fixed_string`). This one rarely makes the headlines, but the library
leans on it directly. I use a fixed-size string to encode the symbols of dimensions
and units (the `"m"` of metre, the `"kg"` of kilogram, and so on) as compile-time
constants that travel through the type system. Without a string type usable as a
non-type template parameter, those symbols cannot live in types at all.

The first thing I stressed is what the type is and is not. `std::basic_fixed_string`
is a fixed-_size_ string, not a fixed-_capacity_ one. It is used broadly, not only
in compile-time programming. As proposed today, it is a non-mutating type with no
`string`-like interface. Rather than duplicate that whole API, it exposes
a `view()` function that returns a `std::string_view`. `fixed_string` also implicitly
converts to `std::string_view` which might be useful in some scenarios.

Section 5.3 records why we move forward rather than wait for a hypothetical
`inplace_string` alternative: an in-place (fixed-_capacity_) string cannot be
implemented as a structural type without core language changes, so it is a
separate problem for a separate paper.

The debate centered on two questions:

- **Should it be mutable?** I initially proposed a non-mutating type. Several members
  pushed back. The analogy on the table was that `fixed_string` should be to `std::string`
  what `std::array` is to `std::vector`. It should have a non-`const` `operator[]`
  returning a `char&`.
- **Should it drop the trailing nul?** One person suggested that we could optionally not
  store the zero terminator, so the type could also map onto contiguous, non-terminated
  buffers. This would add one more template parameter and complicate the design
  (e.g. the semantics of `operator+`). In the end we decided to not go this route.

Two polls were taken:

!!! question "POLL: Change `fixed_string` so that it is mutable."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         5         |    9     |    6    |    4    |        1         |

!!! question "POLL: This paper needs to explore the design space of zero-termination."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         0         |    2     |    7    |    7    |        5         |

The two poll outcomes point at a small, concrete change rather than a redesign. A single
non-`const` `operator[]` returning a `char&` and non-const `begin()/end()` member
functions should satisfy the first poll, and the type stays nul-terminated. That is
the minimal form I will pursue in the next revision.

One thread is still open. A group of authors proposes `cstring_view` (null-terminated
`string_view`) for C++29 in [P3655](http://wg21.link/p3655). If it gets accepted, it
might be a better view to convert to (`fixed_string` is always null terminated).
I have to explore this option as well.

More to come. Stay tuned!

*[CWG]: Core Language Working Group
*[LWG]: Library Working Group
*[LEWG]: Library Evolution Working Group
*[SG6]: Study Group 6 - Numerics
*[SG20]: Study Group 20 - Education
