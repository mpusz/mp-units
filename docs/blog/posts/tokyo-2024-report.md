---
date: 2024-04-15
authors:
  - mpusz
categories:
  - WG21
---

# Report from the Tokyo 2024 ISO C++ Committee meeting

The Tokyo 2024 meeting was a very important step in the standardization of this library. Several
WG21 groups reviewed proposals, and the feedback was really good.

<!-- more -->

## [P3045R0: Quantities and units library](https://wg21.link/p3045r0)

The Study Group 6 (Numerics) discussed the proposal for several hours. The initial feedback
was positive. There were some concerns related to the description and design of the affine
space abstractions in the library. Besides that, the people in the room liked what they saw.

We run a few polls in SG6 as well:

!!! question "POLL: The syntax `number * unit` is the right solution for constructing quantities. Not allowing reordering the operands is correct."

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         5         |    4     |    1    |    0    |        1         |

!!! question "POLL: Not defining any UDLs is the right solution."

  No objection to unanimous consent.

The paper was also briefly discussed in SG18 LEWG Incubator, and the initial feedback was also
positive. No polls were taken.

SG16 Unicode does not meet during ISO C++ Committee F2F meetings. Still, the text output chapter paper
was also reviewed by it during an online meeting before Tokyo. We got good feedback and
are expected to return with the updated version. No polls were taken.


## [P30942R1: `std::basic_fixed_string`](https://wg21.link/p3094r1)

In the SG18 LEWG Incubator, before we started to talk about [P3045R0](https://wg21.link/p3045r0),
we spent a few hours discussing the design of the `std::basic_fixed_string`, which is proposed for
C++26. The group gave excellent feedback, and if the R2 version addresses it properly, the
paper is expected to progress to LEWG (Library Evolution Working Group) in St. Louis.

Plenty of polls were taken:

!!! question "POLL: We should promise more committee time to pursuing std::basic_fixed_string, knowing that our time is scarce and this will leave less time for other work."

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |        11         |    0     |    0    |    0    |        0         |

!!! question "POLL: Should the constructor from a string literal be consteval?"

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         6         |    3     |    2    |    0    |        0         |

!!! question "POLL: Do we want to add .view()?"

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         3         |    5     |    3    |    0    |        0         |

!!! question "POLL: Do we want the .size member to be an integral_constant<size_t, N> (and .empty to be bool_constant<N==0>)?"

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         5         |    2     |    2    |    2    |        0         |

!!! question "POLL: Should the index operator[] return a reference to const?"

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         2         |    2     |    3    |    3    |        0         |

!!! question "POLL: Should the constructor from a string literal have a precondition that txt[N] == 0?"

  | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
  |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
  |         6         |    1     |    2    |    0    |        2         |
