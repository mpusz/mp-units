---
date: 2023-11-12
authors:
  - mpusz
categories:
  - WG21 Updates
---

# Report from the Kona 2023 ISO C++ Committee meeting

**Several groups in the ISO C++ Committee reviewed the [P1935: A C++ Approach to Physical Units](https://wg21.link/p1935)
proposal in Belfast 2019 and Prague 2020. All those groups expressed interest in the potential
standardization of such a library and encouraged further work. The authors also got valuable
initial feedback that highly influenced the design of the V2 version of the mp-units library.**

In the following years, we scoped on getting more feedback from the production and design. This
resulted in version 2 of the **mp-units** library that resolved many issues the users and Committee
members raised. The features and interfaces of this version are close to being the best we can get
with the current version of the C++ language standard.

<!-- more -->

We submitted three new proposals related to the standardization of the quantities and units library
for the last ISO C++ Committee meeting:

- [P2980: A motivation, scope, and plan for a physical quantities and units library](https://wg21.link/p2980R1),
- [P2981: Improving our safety with a physical quantities and units library](https://wg21.link/p2981R1),
- [P2982: `std::quantity` as a numeric type](https://wg21.link/p2982R1).

Those were reviewed and briefly discussed in several groups: Numerics (SG6), Safety & Security
(SG23), and Library Evolution Working Group (LEWG). Most of the feedback was positive, and
the Committee is interested in spending more time on such proposals.

The following poll was taken by the LEWG:

!!! question "LEWG POLL: Given that our time is limited, more time should be promised for a quantities and units library"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        10         |    13    |    4    |    0    |        0         |

    Attendance: 22 + 6

    Number of Authors: 4

    Authors’ position: 4x SF

    Outcome: Strong consensus in favor

Additionally, some concerns were raised about the large scope of the proposal. We were encouraged
to return with more details and design rationale in a unified paper. This is what we are working
on now for the next Committee meeting that will happen in mid-March 2024 in Tokyo.
