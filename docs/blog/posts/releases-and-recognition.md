---
date: 2026-12-10
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Releases and recognition are how a project grows up

A library you depend on ships a new version. You go looking for what changed and find a git
tag, `v2.0`, and a changelog that is a list of commit subjects. Did anything break? What is
worth upgrading for? Do you need to touch your own code? You cannot tell, so you either pin
the old version forever or spend an afternoon reading diffs. A release is a conversation with
your users, and most projects refuse to have it.

<!-- more -->

!!! info "Part of a series: Why Great C++ Libraries Fail"

    This post is part of a
    [series](../../../../category/why-great-c-libraries-fail/) based on my using std::cpp 2026
    talk on why technically excellent C++ libraries fail to get adopted. It covers the
    **Community** stage of the six-stage library journey: does it sustain itself? New here?
    Start with [the overview](nobody-uses-your-great-library.md).

Two things turn a project from one person's repository into something that outlives them:
releases that actually communicate, and recognition that makes people want to stay. Neither
is about code. Both are about the people on the other side of it.

## A release is a marketing event, not a git tag

The default C++ release is a tag and silence. It is a wasted opportunity, because a release
is the one moment your existing users are paying attention and new ones might arrive. So write
a real release post, and make it answer three questions:

- **Why.** What motivated this version? A performance problem, a batch of user requests, a
  standards alignment. Give it a reason to exist.
- **The highlights.** Lead with the handful of changes most users actually care about, even
  when the changelog underneath is long.
- **The breaking changes.** Tell users exactly how to migrate, with before and after code.

That last one is where C++ is unusually treacherous, because "breaking" is often invisible
until a user's build fails. A refined concept, an added template parameter, a renamed entity:
none of it looks dramatic in a diff, and all of it can stop downstream code from compiling.
So communicate breakage in two places. In the code, `[[deprecated]]` carries the migration
path straight into the compiler warning:

```cpp
[[deprecated("2.3.0: Use `mag<pi>` instead")]]
inline constexpr UnitMagnitude auto mag_pi = mag<pi_c>;
```

The version in the message tells the user how long the name has been on notice, the message
itself tells them the fix, and they see both without opening any docs. Back that with a
simple, stated rule for *when* deprecated names actually disappear, tied to your versioning.
The cleanest one is to remove them only at the next major release, so no minor upgrade can
ever break a build and a user always has a whole major cycle to migrate. In the release post,
then mark every breaking change clearly and show the before and after side by side. mp-units'
release posts do this, so a user can scan one in two minutes and know precisely which changes
touch them. Then distribute it, on LinkedIn, on r/cpp, wherever your users actually are. A
release post nobody reads helps nobody.

## Recognition is the fuel open source runs on

Nobody contributing to your project is paid. In practice the people who show up are your own
users, someone who hit a rough edge and fixed it, or people who care about the domain itself,
about getting units and quantities right in C++. A few are also there for the resume line
or the chance to learn bleeding-edge C++ on a real codebase. Whatever brings them, there are
never many, so your job is to make contributing rewarding enough that the ones who do arrive
want to stay. It costs little and it compounds.

Start with a `CONTRIBUTORS.md` that lists everyone with a link to their profile. It is fine
for this to be generated, mp-units builds the list automatically from contribution history,
so nobody is forgotten and it never goes stale. Then put the specific, personal credit where
it carries the most weight: in the release post, name the people whose work shipped in that
version, where their whole professional network will see it. That is the multiplier. You
credit someone, they share it, their network discovers the project, and some of those people
become users and contributors in turn. It is free reach, and you start it by being generous
first.

The advanced tier costs more time and creates lifelong advocates. Help a contributor write
up their work. Invite them to co-present. Write them a reference letter when they job-hunt.
"I contributed to mp-units, a C++ standardization candidate" is a line that wins interviews,
and the person who got that from you will talk about your project for years.

## Where mp-units actually is, and the honest gap

The release side is a genuine strength. The
[release posts](https://mpusz.github.io/mp-units/latest/blog/category/releases/) are prose,
not changelogs: they explain the why, walk through breaking changes with migration guidance,
and mark each one clearly. Deprecations carry their replacement in the message. And getting
a release in front of people, on r/cpp and LinkedIn, is the part I never struggle with.

Two honest gaps remain, and both come back to scale. First, the releases have grown large.
2.6.0, the last before the 3.0 rework, kept accreting features until it was a lot to absorb
in one post, and in hindsight I should have split it into smaller, more frequent releases
that are easier to read and to adopt. Second, the recognition rituals beyond the release
post, contributor spotlights, co-presenting invitations, reference letters, stay sparse. The
honest reason is not that I am too busy for them, it is that there are so few contributors
to celebrate in the first place, the same shortage the Contribution post ended on. The hardest
version of recognition, turning a regular contributor into a co-maintainer who genuinely
shares the load, is the one that would break that cycle. mp-units has had that before and
could not keep it, which is the harder half of the problem and the one the final post takes
up. Growing up is not an act you perform once and check off.

These tips come from my conference talk on why technically excellent C++ libraries fail to
get adopted, and how to fix it. You can
[watch the recording](https://www.youtube.com/watch?v=DWXlyOd_z88), which won best talk at
using std::cpp 2026, and read
[the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
I am giving a shorter version at CppCon 2026, and the full two-hour keynote at Meeting C++
2026.
