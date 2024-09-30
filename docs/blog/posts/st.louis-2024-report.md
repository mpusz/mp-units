---
date: 2024-07-02
authors:
 - mpusz
categories:
 - WG21 Updates
---

# Report from the St. Louis 2024 ISO C++ Committee meeting

We made significant progress in the standardization of this library during the ISO C++ Committee
meeting in St. Louis.

<!-- more -->

## [P30942R3: `std::basic_fixed_string`](https://wg21.link/p3094r3)

First, the `fixed_string` was unanimously forwarded from the SG18 LEWG Incubator to the Library
Evolution Working Group (LEWG). The group suggested a few minor changes to the paper, which
resulted in the R3 version of the proposal.

The paper is in excellent shape, and the entire wording is ready as well. Hopefully
it will progress quickly through the remaining groups in the Committee.


## [P3045R1: Quantities and units library](https://wg21.link/p3045r1)

In the SG6 (Numerics), we had a really efficient discussion about the recently raised usability
issues with temperatures and the Minimal Viable Product (MVP) scope.

The following polls were taken:

!!! question "POLL: If WG21 adds anything to the standard to provide units or quantities, then such a solution must at least include the necessary abstractions for units, dimensions, and quantity kinds. (It does not have to provide system definitions, e.g. ISQ/SI definitions.)"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         7         |    4     |    7    |    0    |        0         |

!!! question "POLL: If WG21 adds anything to the standard to provide units or quantities, then such a solution must at least include the necessary abstractions for units, dimensions, quantity kinds, and quantities of the same kind. (It does not have to provide system definitions, e.g. ISQ/SI definitions.)"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         5         |    2     |    5    |    2    |        0         |

!!! question "POLL: If WG21 adds anything to the standard to provide units or quantities, then such a solution must at least include the necessary abstractions for units, dimensions, quantity kinds, and affine spaces. (It does not have to provide system definitions, e.g. ISQ/SI definitions.)"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |         5         |    0     |    8    |    1    |        0         |

As we can see, there are no controversies about the first poll that states that the MVP should
include at least:

- units,
- dimensions,
- quantity kinds (e.g., _frequency_ vs _activity_, _plane angle_ vs _solid angle_,
 _fuel_consumption_ vs _area_).

The next polls add either:

- quantities of the same kind (e.g., _length_ vs _width_ vs _wavelength_, _potential energy_ vs
_kinetic energy_),
- the affine space abstractions (i.e., point origins and `quantity_point`).

SG6 considered those less important, but no one was strongly against including those in the MVP.
We were asked to return with better motivation and usage examples for those features.

**If you are depending on quantities of the same kind or quantity points in your project and you
would like to see them in the `std` library, please let us know about your use cases.**

Besides SG6, we spent six hours in the SG18 LEWG Incubator discussing the details of the library
design. The proposal was very well received, and we got a few valuable comments and suggestions
that we will apply to the next version of the paper.
