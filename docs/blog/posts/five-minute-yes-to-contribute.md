---
date: 2026-11-26
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Make it a five-minute yes to contribute

Someone hits a small bug in your library and has a one-line fix they would happily send you.
They want to help. Then they read your `CONTRIBUTING.md`: install Clang 18, CMake 3.28, Ninja,
Conan 2.0, configure three presets, wait for a build. Twenty minutes later, before they have
changed a single character, they give up. The willing contributor is the rarest thing your
project has, and you lost them at setup.

<!-- more -->

!!! info "Part of a series: Why Great C++ Libraries Fail"

    This post is part of a
    [series](../../../../category/why-great-c-libraries-fail/) based on my using std::cpp 2026
    talk on why technically excellent C++ libraries fail to get adopted. It covers the
    **Contribution** stage of the six-stage library journey: do they want to help? New here?
    Start with [the overview](nobody-uses-your-great-library.md).

A satisfied user is your best potential contributor, but only if saying yes is cheap. Two
things decide whether it is: how you handle their first message, and how much setup stands
between them and a working change. Get both wrong and even eager people quietly walk away.

## Communication is a feature of your library

How you treat an issue defines your community more than your code does. I have watched
technically inferior projects thrive because the maintainer was kind and responsive, and
brilliant ones wither because the maintainer treated every question as an imposition. One
hostile reply is remembered far longer than a hundred helpful ones.

Start by separating **issues** from **discussions**. Issues are for actionable things: bugs,
tasks, concrete feature requests. Discussions are for questions, ideas, and show-and-tell.
When a newcomer wants to file "how do I convert between these units?" as a bug report, the
form itself tells them they are doing something wrong. mp-units disables blank issues and
points questions at [GitHub Discussions](https://github.com/mpusz/mp-units/discussions), so
asking never feels like cluttering the tracker.

Then kill the "it does not work" loop before it starts. Without structure, a vague report
turns into five rounds of "what does not work?" and "what thing?", after which the reporter
gives up and you never learn why. An issue template ends that by requiring the specifics
up front: operating system, compiler and version, C++ standard, library version, and a
Compiler Explorer link that reproduces the problem. mp-units' bug report form asks for
exactly that, so the first response can be an answer instead of an interrogation.

Finally, tone, because in an issue tracker it is half the product. Open every reply with a
thank-you, even for a duplicate, even when the report turns out to be user error, because
that person spent their time on your project. Assume good faith, and never that the user is
simply holding it wrong. When someone does hit a real mistake, the interesting question is
what let them make it: a confusing name, an unstated precondition, a gap in the docs. Treat
that as a bug in the library rather than in the user, and fix the thing that misled them.
Every issue,
question, and complaint is production feedback you would otherwise have to pay for. Empathy
over ego is not politeness for its own sake, it is how a first-time contributor decides whether
there will be a second time.

## Provide the environment, do not describe it

Now the setup barrier. Picture the contributor with a small first change, a one-line fix to
behavior, staring at a page of toolchain instructions before they can even build to test it.
Every step is a chance to give up, and documentation-only setup guides rot the moment a
version bumps. The fix is to stop describing the environment and start handing it over, as
a container.

GitHub Codespaces makes the environment a container the contributor never has to build. They
click "Open in Codespaces," and a few minutes later they are editing in a browser with every
compiler and tool already installed, no local setup at all. It works on a Chromebook, an
iPad, or a locked-down corporate laptop that could never install a C++23 toolchain. mp-units
ships this from the README, and the environment is defined once in `devcontainer.json`,
pointing at a prebuilt image (`trainiteu/mp-units:latest`) that carries the compiler matrix,
Conan, the analysis tools, and the docs toolchain.

That one definition has several front doors. A contributor who prefers a local editor opens
the identical container in VS Code with the Dev Containers extension, and anyone can just
`docker pull` the image to get the exact toolchain on the command line. Codespaces, a local
dev container, or a plain Docker image: same environment, nothing for a contributor to
assemble, nothing to fall out of date.

The same container pays off well beyond the drive-by contributor, which is the part people
miss. It is one shared environment for everyone who touches the project. A new team member
is productive on day one instead of losing it to installing toolchains, and because the image
carries the compilers the project targets, anyone can build against a representative set of
configurations locally with
[`check_all.sh`](https://github.com/mpusz/mp-units/blob/master/.devcontainer/check_all.sh)
and catch the obvious breakages before pushing, rather than waiting for the full, larger CI
matrix to find them. Everyone working in it is on the same toolchains, so "works on my machine"
stops being a sentence between contributors, and a dedicated CI job rebuilds the container
on every change so it never quietly drifts.

## Where mp-units actually is, and the honest gap

This is a stage mp-units gets right. Issues and discussions are separated, the bug form
demands a reproduction, and a contributor can go from "I would like to help" to a working,
CI-compatible environment in minutes without installing anything.

The honest gap is not the infrastructure, and it is not responsiveness either. Questions
usually get an answer the same day, and small fixes are merged within days. The gap is that
there are still very few contributors. A one-person project can be welcoming and fast and
still not have grown the handful of regulars who share the review and mentoring load, and
now and then a large in-progress change holds the tree long enough that a good pull request
has to wait for it to land. The machinery makes contributing easy. Turning that ease into
a steady stream of contributors is the same bus-factor problem the Evaluation post admitted,
wearing different clothes, and it is the thread the next post picks up.

These tips come from my conference talk on why technically excellent C++ libraries fail to
get adopted, and how to fix it. You can
[watch the recording](https://www.youtube.com/watch?v=DWXlyOd_z88), which won best talk at
using std::cpp 2026, and read
[the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
I am giving a shorter version at CppCon 2026, and the full two-hour keynote at Meeting C++
2026.
