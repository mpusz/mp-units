---
date: 2026-11-12
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Don't be asocial: respect the user's build system

You found the library, and now you just want to consume it. Except its CMake packaging is
broken, or there is none at all, so using it means copying headers by hand, guessing at
include paths, and fighting compiler flags it hardcoded over your own. There is no target
to link against, and it drags in a home-grown dependency you have never heard of. To adopt
it you would have to bend your build around its way of doing things, so you do what most
people do: you reach for another library, or you write your own.

<!-- more -->

!!! info "Part of a series: Why Great C++ Libraries Fail"

    This post is part of a
    [series](../../../../category/why-great-c-libraries-fail/) based on my using std::cpp 2026
    talk on why technically excellent C++ libraries fail to get adopted. It continues the
    **Integration** stage of the six-stage library journey: can people actually use it? New
    here? Start with [the overview](nobody-uses-your-great-library.md).

I think of a library that refuses to integrate with the ecosystem around it as **asocial**,
a framing I owe to David Sankel's C++Now 2015 talk,
[Big Projects, and CMake, and Git, Oh My!](https://www.youtube.com/watch?v=3eH4hMKl7XE). The
fix is to be social: provide the integration you wished existed when you were the one trying
to adopt a dependency. The rule of thumb is blunt, every consumption path you do not support
is a reason for someone to say no.

## The asocial checklist

A library is asocial when it makes you work to consume it at all. The warning signs:

- broken or absent CMake packaging: no `find_package` support, no exported targets, so you
  wire up include paths by hand,
- not on any package manager,
- obscure or home-grown dependencies,
- hardcoded compiler flags that override the ones you chose,
- a bespoke build script you are expected to run and trust.

Any one of these is friction. All of them together is a fortress. The social version is the
opposite at every point, and most of it is ordinary build hygiene rather than heroic effort.

## Modern CMake: targets, not variables

The baseline for a C++ library is that consuming it costs the user a few lines:

```cmake
find_package(mp-units REQUIRED)

target_link_libraries(my_app PRIVATE mp-units::mp-units)
```

That is the whole integration. A proper namespaced target (`mp-units::mp-units`) carries its
include paths, its compile features, and its transitive dependencies with it, so the consumer
does not configure any of that by hand. The `PRIVATE`/`PUBLIC`/`INTERFACE` distinction means
your implementation details do not leak into their build graph.

The anti-patterns are the mirror image. Do not call `include_directories()`, which pollutes
every target globally. Do not set `CMAKE_CXX_FLAGS` or force warning and ABI flags onto the
consumer, because that silently overrides choices that are theirs to make. The complexity
belongs inside your targets, not in their `CMakeLists.txt`.

## Be on a package manager

If your library is not installable through a package manager, a large class of users simply
cannot adopt it. Corporate security teams block arbitrary vendoring, and "just copy these
headers in" is not an answer they will accept. mp-units is on
[Conan Center](https://conan.io/center/mp-units), so the requirement is one line:

```text
[requires]
mp-units/2.5.0
```

Conan resolves the right version, configures CMake, and pulls in transitive dependencies,
and the library's feature flags (formatting backend, contracts library, modules, freestanding)
become Conan options the user sets without touching your source.

## Meet users where they are

Not everyone lives in a modern package-manager world, and that is fine. Air-gapped networks
cannot reach a registry. Large monorepos vendor everything. Some teams are on CMake's built-in
fetching, or on Bazel or Meson. Each environment you ignore is another "no," so support as
many consumption paths as you reasonably can:

- a package manager (Conan, and ideally vcpkg too),
- CPM or CMake's `FetchContent`, which need no extra tooling,
- `add_subdirectory` for vendoring into a monorepo,
- an install plus `find_package` for system-wide use.

The good news is that most of these come almost for free once your CMake is clean. Proper
install rules give you `find_package`. A tidy `src/` subtree gives you `add_subdirectory`.
CPM and `FetchContent` work because the targets are well defined. mp-units documents Conan,
`find_package`, CPM and `FetchContent`, and `add_subdirectory`. The more paths you support,
the fewer reasons anyone has to walk away.

## Depend on as little as possible

Every dependency you add is an adoption barrier for someone: another thing their security
team must clear, another version to reconcile, another package an air-gapped build cannot
fetch.
So treat your dependency footprint as a feature to keep small.

mp-units has **no mandatory external dependencies**. The only two it can use, a GSL
implementation for contract checking and `{fmt}` for formatting, are optional, selected
through Conan options, and exist purely to backfill features the standard library does not
provide everywhere yet. These are stopgaps. As the standard and, just as importantly, every
compiler it supports catch up (`std::format` usable across the whole matrix, contracts in
C++26), the dependencies go away entirely. "Low adoption cost" is not a slogan on the README,
it is a design constraint.

## Wide compatibility: your users are spread across every cell of the build matrix

"Modern C++" is not one thing, even at a fixed standard version. `std::format` (C++20) and
`import std` (C++23) each showed up in real compilers years after the paper, modules land
unevenly, and the explicit object parameter arrives at different versions again. Your users
are spread across every cell of that matrix, and you have three ways to deal with it. Require
the newest features, and you exclude most of your potential users. Target only the oldest
API you support, and everyone on a modern toolchain gets a worse experience than they could
have, a particularly bad trade for a library that wants to become a part of the C++ Standard
Library. Or provide compatibility, so each user compiles in the style their compiler actually
supports.

mp-units chose the third. Compatibility macros, plus CMake and Conan options (`cxx_modules`,
`import_std`, `std_format`, `no_crtp`, `contracts`, `freestanding`), let a user pick the level
that matches their toolchain, and, as covered in the Evaluation post, every combination is
exercised in the randomized CI matrix. That is why the library can claim support all the way
from older GCC to the latest Clang, rather than only on the maintainer's machine.

## Where mp-units actually is

This is a stage mp-units gets right. It exposes clean namespaced CMake targets, documents
Conan, `find_package`, CPM and `FetchContent`, and `add_subdirectory`, carries zero
mandatory dependencies, and verifies its wide compiler support in CI rather than asserting
it. It is also installable through both major package managers, Conan Center and vcpkg,
each at the current release.

The one honest nuance is who keeps those running. I maintain the Conan packaging myself,
because Conan is the ecosystem I actually know. The vcpkg port is community-maintained, and
the people who own it keep it current, the version on vcpkg today matches the latest release.
I am grateful for that, and I will also be honest that it is a path my users depend on which
I do not control and could not quickly fix if it ever lapsed. That a third party maintains
it at all is a good sign, a library only earns that when it is worth the effort, but it is
worth watching rather than taking for granted.

These tips come from my conference talk on why technically excellent C++ libraries fail to
get adopted, and how to fix it. You can
[watch the recording](https://www.youtube.com/watch?v=DWXlyOd_z88), which won best talk at
using std::cpp 2026, and read
[the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
I am giving a shorter version at CppCon 2026, and the full two-hour keynote at Meeting C++
2026.
