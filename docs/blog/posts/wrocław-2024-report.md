---
date: 2024-11-25
authors:
 - mpusz
categories:
 - WG21 Updates
comments: true
---

# Report from the Wrocław 2024 ISO C++ Committee meeting

The Wrocław 2024 meeting was another efficient step in the standardization of this library.
We've spent the entire day on the joint LEWGI and SG6 discussion and got lots of feedback.
We've also introduced `std::fixed_string` to LEWG for C++26.

<!-- more -->

## [P3045R4: Quantities and units library](https://wg21.link/p3045r4)

We have presented the following chapters of our proposal to LEWGI and SG6. We reviewed all
the usage examples, discussed composing symbols for derived dimensions and units, and looked into
formatting specifications for quantities. We also discussed the minimal scope of the proposal.

We got plenty of feedback on:

- the paper itself,
- naming of specific entities in the library,
- usage examples,
- text output and formatting.

We were also asked to extend the library to provide text output support for quantity points.

You can expect all of those changes to appear in the next release.


## [P30942R5: `std::basic_fixed_string`](https://wg21.link/p3094r5)

The paper was well received. However, Barry Revzin submitted
[P3380](https://wg21.link/p3380) paper in September. This started a discussion about the scope of
this proposal. Should we:

- limit it to just a non-mutating interface (as proposed) and maybe rename it to
  `std::string_literal`,
- add range slicing interface (as we support composition already),
- add some basic mutation interface through non-const `operator[]`,
- refactor to fully blown `std::inplace_string` if [P3380](https://wg21.link/p3380) successfully
 progresses through EWG.

*[EWG]: Evolution Working Group
*[LEWG]: Library Evolution Working Group
*[SG6]: Study Group 6 - Numerics
*[LEWGI]: Study Group 18 - Library Evolution Working Group Incubator
