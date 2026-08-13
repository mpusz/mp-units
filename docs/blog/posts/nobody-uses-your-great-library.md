---
date: 2026-09-03
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Your library is great. Nobody uses it. Here's why

I can tell you exactly how many stars mp-units has. The number is right there on the GitHub
page and on my docs site, it updates in real time, and I can plot it against every competing
units library on a single chart. What I cannot tell you, even roughly, is how many people
actually use it.

<!-- more -->

That gap is the whole problem. Stars are easy to count, and they do measure something real:
reach, discoverability, the fact that people keep finding you. What they do not measure is
adoption. Users are nearly invisible. They surface only when something breaks: a bug report,
a terse question, a one-line "does this work with my compiler?", and then silence again. For
every one of those, some unknown number of people tried the library, hit a wall, and left
without a word. You never see them. You just watch the number that is easy to measure and
quietly hope it stands in for the one that matters.

Here is the uncomfortable part. It is tempting to believe a technically excellent library
earns its users on merit. It does not. Technical excellence is not the same thing as project
excellence, and the two are genuinely different skills. I have watched dozens of brilliant
C++ libraries sit at three users and no contributors, written by people who understand their
domain far better than I understand mine. The code was never the problem.

## The fortress

The failure is consistent enough to have a shape. Call it the fortress: a library that is
hard to enter ("first, install Clang 18 and CMake 3.28"), hard to understand ("read the
source"), and hard to contribute to, with no guide, no first issues, and a maintainer who
answers questions as if they were an imposition. Every one of those is a wall, and every
wall is a person who quietly turns around. Some write their own version instead. Some move
to an ecosystem that made them feel welcome. The result is the same, and it is the real
tragedy: the technical excellence goes unused.

Getting out of the fortress starts with understanding the journey every potential user takes,
because each step is a place you can lose them. There are six.

## The six stages every library must survive

- **Discovery.** Can they even find you? Most of that decision is made before they read a
  single line of your code.
- **Evaluation.** Do they trust it works? A green build is a promise you make to a stranger.
- **Understanding.** Can they learn it? Documentation is where most technically excellent
  libraries quietly lose people: there is no way in beyond reading the headers.
- **Integration.** Can they actually use it? Getting it into their build without friction,
  from zero-install prototyping to clean packaging.
- **Contribution.** Do they want to help? A satisfied user is your best future contributor,
  if you make starting easy.
- **Community.** Does it sustain itself? Releases that explain themselves, recognition that
  means something, and a way to absorb help, including the rising flood of AI-generated PRs,
  without drowning.

This post opens a series that walks those six stages, built around eleven concrete tips:
the actual engineering decisions that move someone from one stage to the next instead of out
the door. Naming and README structure. CI signals. Documentation that is prose, not a
Doxygen dump. Compiler Explorer and one-click dev environments. Contribution that takes
minutes to start. Release blogs, a culture of appreciation, and an AI contribution policy.
None of it is exotic. All of it is the difference between a repo and a product. The posts
that follow take these on a few at a time, and each one stands on its own.

## mp-units is the specimen, not the trophy

I use mp-units as the worked example throughout, including the parts where it still falls
short of its own advice. It is a high-visibility, standards-track library with more stars
than any other C++ units library, and it still does not convert that visibility into the
adoption it could have. That is exactly what makes it an honest specimen rather than a
trophy. Every stage in this series is one I have gotten wrong, fixed late, or am still fixing
in public, and I will say so plainly wherever mp-units does not practice what I preach.

There are three ways to read what follows:

- **If you maintain a C++ library** that deserves more users than it has, treat it as a
  checklist for finding the stage that is quietly costing you.
- **If you evaluate and adopt libraries**, treat it as a map of what good ones do, and a fair
  warning about what the absence of these signals is telling you.
- **If you are a developer** asking "which project should I contribute to in order to learn
  modern C++?", these are the green flags worth looking for.

One last thing, and it is the honest reason this series lives on the mp-units blog rather
than somewhere neutral. If you already use mp-units, the single most useful thing you can
do is tell me where it sits on this map: which stage nearly made you give up, and what you
wished had been there. That kind of feedback is genuinely hard to come by, and it shapes both
the library and its path into the C++ standard.

## The posts in this series

This is the opening post. The rest of the series publishes over time, a few of the six
stages at a time, each one standing on its own. As each post goes live it will appear here
and under the
[*Why Great C++ Libraries Fail*](../../../../category/why-great-c-libraries-fail/) category.

These tips come from my talk on why technically excellent C++ libraries fail to get
adopted, and how to fix it. You can
[watch the using std::cpp 2026 version](https://www.youtube.com/watch?v=DWXlyOd_z88), or
browse [the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
An expanded version is coming as a keynote at Meeting C++ 2026.
