---
date: 2026-03-28
authors:
 - mpusz
categories:
 - WG21 Updates
comments: true
---

# Report from the Croydon 2026 ISO C++ Committee meeting

It has been 1.5 years since the last major update on the ISO C++ standardization
progress here. It is not that I got lazy :wink:, but there was really not much
to share.

This time, things were different. We achieved a nearly unprecedented
success — one probably not even expected by most, definitely not by me! 🎉

Keep reading to learn more...

<!-- more -->

## What Happened in the Meantime?

The last year of the Committee was heavily focused on finalizing C++26 and
introducing safety to the language. Huge and very important features like contracts
and profiles consumed a lot of time, energy, and resources. To make the process
work, we needed quorum in the rooms on the core standardization pipeline, which
is why sometimes incubator study groups didn't meet at all to avoid pulling people
away from the rooms critical to making progress.

As a result, we either didn't discuss quantities and units at some meetings at all,
or we were just focusing on the initial review and feedback that would allow me
to improve the paper so it would be in the best shape when we start talking about C++29.


## Initial Review Complete: Advancing to LEWG

We have been reviewing the [P3045: Quantities and units library](https://wg21.link/p3045)
in LEWG Incubator (SG18) and Numerics (SG6) study groups for more than three
years now. The proposal has been improving through this time and reached Revision 7.

In Croydon, we met again, discussed the last remaining chapters, and decided
that the paper is ready to advance to LEWG, which is responsible for the shape
of the future of the C++ Standard Library.

*[LEWG]: Library Evolution Working Group


## C++26 Is Done 🎉

C++26 was officially shipped by WG21 on Saturday, March 28, 2026! 🚀 This is a
very impactful release that will have a significant influence on the C++
community and how we build our products worldwide.

Some of the headline features include:

- **Contracts** ([P2900](https://wg21.link/p2900)) — preconditions, postconditions, and
  assertions as first-class language features
- **Reflection** ([P2996](https://wg21.link/p2996)) — compile-time introspection and
  manipulation of program entities
- **Expansion statements** ([P1306](https://wg21.link/p1306)) — `template for` to iterate
  over compile-time ranges like tuples and reflection results
- **`std::execution`** ([P2300](https://wg21.link/p2300)) — a framework for asynchronous
  and parallel execution (senders/receivers)
- **`std::simd`** ([P1928](https://wg21.link/p1928)) — portable data-parallel types
- **`std::inplace_vector`** ([P0843](https://wg21.link/p0843)) — a dynamically-resizable
  vector with fixed capacity and no heap allocation
- **`#embed`** ([P1967](https://wg21.link/p1967)) — binary resource embedding in source code
- **`constexpr` exceptions** ([P3068](https://wg21.link/p3068)) — throwing and catching
  exceptions at compile time
- **`std::hive`** ([P0447](https://wg21.link/p0447)) — a bucket-based container that
  reuses memory from erased elements
- **`std::linalg`** ([P1673](https://wg21.link/p1673)) — a free function linear algebra
  interface based on the BLAS
- **Hazard pointers and RCU** ([P2530](https://wg21.link/p2530),
  [P2545](https://wg21.link/p2545)) — concurrent data structure building blocks
- **Standard library hardening** ([P3471](https://wg21.link/p3471)) — bounds-checking
  and other safety checks for standard containers


## Day Zero: Evening Session on Quantities and Units

On Monday, the very first day of the WG21 meeting, right after plenary,
I was informed that LEWG will start discussing C++29 features and will have time
to talk about the Quantities and Units proposal. As metrology is an inherently complex
problem, we decided to dedicate an Evening Session to the subject the evening
before it gets to LEWG for the very first review. The intent was to allow people
to understand the entire scope of the problem space, realize that this domain
is more complex than most of us think, and see potential solutions to address
real production issues.

In the following three days, I produced 200 slides from scratch 🤣 and I was ready.
We had 2.5 hours and we used every minute. The session was well attended and people
learned a lot. I also got great feedback afterward. My personal feeling is
that it was probably one of the best presentations I have ever given on this subject.

Most of you could not attend but I want you to be able to benefit from the material
I created as well. Below I attach the slides from this session. Please download
them, review, and I strongly encourage you to click on the QR codes in the top-right
corner to play with code examples and also to solve workshops I have prepared.
I hope that this will allow you to better understand the domain, proposed design,
and maybe open your minds on how you can apply this library in your production
right now. We really need all the production feedback we can get before it is
shipped in C++29. After that, we will not be able to change anything. The interface
provided will need to stay stable for ages.

[Download the slides from the Evening Session](https://github.com/train-it-eu/conf-slides/blob/master/2026.03%20-%20Croydon/P3045%20-%20Quantities%20and%20Units%20Library.pdf){
.md-button }


## First LEWG Review: Unanimous Support

The next morning, we had the very first LEWG discussion about
[P3045: Quantities and Units Library](https://wg21.link/p3045). I had been dreading this
day for months, as the initial discussion is typically along the lines of
"Do we want it at all?" and "What should be the scope?". However, surprisingly,
I was quite calm. I knew I had done a great job the previous evening.

We discussed the components, the consequences of removing them, and possible alternatives.
Then we took a long series of polls to make the temperature of the room crystal clear
(pun intended :wink:):

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We acknowledge the complexity inherent to the domain of quantities and units and think this is a problem worth solving thoroughly in the standard library, following the direction presented in P3045R7"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        22         |    13    |    1    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We support the direction presented as a technical solution (instantiating quantities with units, creating the hierarchy of kinds) etc."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        16         |    22    |    0    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We see the value in covering use cases in fine granularity (so different quantities, all part of the same kind) different use cases such as width and height, or difference between kinetic and potential energy."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        22         |    12    |    3    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We see the value of standardizing these parts of the framework suggested by “P3045R7 Quantities and units library”: Core library: quantity, symbolic expressions, dimensions, units, references and concepts; Quantity kinds: support quantities of the same dimension; Quantities of the same kind: width, height, etc."

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        27         |    9     |    0    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We see the value of standardizing these parts of the framework suggested by “P3045 R7 Quantities and units library”: Affine space: quantity_point, point origin, and concepts for them"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        24         |    8     |    1    |    0    |        0         |

<!-- markdownlint-disable-next-line MD013 -->
!!! question "We see the value of standardizing these parts of the framework suggested by “P3045 R7 Quantities and units library“: Text output: for quantity, units, and dimensions"

    | Strongly in Favor | In favor | Neutral | Against | Strongly Against |
    |:-----------------:|:--------:|:-------:|:-------:|:----------------:|
    |        15         |    14    |    5    |    0    |        0         |

Not a single person opposed to the proposed scope of the library.
This is a huge milestone and I will definitely celebrate 🎉

However, we should not get too excited. It is an important, but only the first step in LEWG
on the way to accepting the design and forwarding it to LWG. We still need to finalize and
refine the design in the Committee. We also still have a lot of work ahead with **mp-units**
design and implementation. The V3 release is still on the horizon and I need to find time
to finish its implementation, rewrite lots of documentation, and deliver the release.

*[LWG]: Library Working Group


## Your Support Matters

With the above in mind, if you care about the project and would like to see quantities and
units landing in C++29 on time, please consider supporting the project.

I do not feel comfortable writing this, but I am an independent C++ trainer and I am not
getting paid for this work. I have spent thousands of hours on **mp-units**, the papers,
and the standardization effort. I do it in my free time while also needing to advance my
training career to make a living.

By this I mean various ways of support:

- ⭐ **GitHub Sponsors** — <https://github.com/sponsors/mpusz>
- 💻 **Book a training** — C++ hands-on workshops by the author of mp-units (<https://train-it.eu>)
- 🛠️ **Contribute** — report issues, submit PRs, help with documentation or test cases
- 🗣️ **Spread the word** — try the library, give feedback, share with colleagues

Thank You! 🙏

!!! important "Please share your production experience"

    **mp-units** is a candidate for ISO standardization in C++29. To support this effort,
    we need your help in demonstrating its real-world value to the ISO C++ Committee.

    **We are seeking:**

    - **Field trial partners** – Organizations interested in evaluating the library in production environments
    - **Success stories** – Real-world testimonials from production deployments, research projects, or educational use
    - **Feedback** – Practical insights from developers, academics, and organizations using the library

    Your experience—whether from production systems, research applications, or teaching—provides
    invaluable evidence for standardization and helps other potential users evaluate the library.

    [![Share Your Experience](https://img.shields.io/badge/Share_Your-Usage_Experience-blue?style=for-the-badge&labelColor=black&label=🌟%20Share%20Your)](https://github.com/mpusz/mp-units/issues/new?template=usage_experience.yml)
