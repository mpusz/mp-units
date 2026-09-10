---
date: 2027-01-07
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Meet the world on purpose

A pull request lands overnight. It is well formatted, the description reads fluently, the
diff is large, and something is off. A function nobody asked to touch was rewritten, a
subtle precondition was quietly dropped, and the rationale in the description is confident
prose that doesn't survive a second read. It took a language model ninety seconds to
produce, and it will take you an hour to review. Now picture ten of them by morning. This
is the new maintainer reality, and pretending it isn't happening isn't a plan.

<!-- more -->

!!! info "Part of a series: Why Great C++ Libraries Fail"

    This post is part of a
    [series](../../../../category/why-great-c-libraries-fail/) based on my using std::cpp 2026
    talk on why technically excellent C++ libraries fail to get adopted. It closes the
    **Community** stage of the six-stage library journey: does it sustain itself? New here?
    Start with [the overview](nobody-uses-your-great-library.md).

Two forces define this last stage of the journey. One is defensive: the flood of AI-generated
contributions that can either raise your throughput or drown your design. The other is
offensive: nobody discovers, evaluates, or joins a project that stays silent. Both come down
to the same decision, choosing on purpose how your project meets the outside world instead
of letting the world set the terms.

## An AI policy is not an anti-AI policy

Language models are genuinely useful to a maintainer and to a contributor. They draft prose,
they generate the tedious half of a test suite, they are a decent rubber duck for a design
question. The most underused case is documentation: hand the model your headers and design
notes and a blank page becomes a draft you correct in ten minutes instead of writing over
two hours. Banning them is both futile and wasteful. But left unmanaged they produce exactly
the pull request above: plausible, voluminous, and subtly wrong, optimized to look like a
good contribution rather than to be one. The whole review cost lands on you, and at scale
it can bury the maintainer who is the one thing keeping the library coherent.

So the answer isn't a ban, it is a written policy, in `CONTRIBUTING.md`, where a contributor
reads it before opening a pull request. It doesn't need to be long. It needs to name the
failure modes and set the expectation that the human, not the model, is accountable for the
change. mp-units spells this out in a few rules
([contributing guidelines](https://github.com/mpusz/mp-units/blob/master/CONTRIBUTING.md)):

- **You are the author.** However much a model or an agent wrote, you must understand and
  be able to defend every line. A blind refactor nobody can explain isn't ready, however
  confident the tool sounded. A library's coherence is a human responsibility.
- **Build and test it yourself first.** Run the change in the provided container and make
  the tests pass locally, rather than leaving that to CI or to the reviewer. AI-written tests
  are especially suspect: a test must encode the *intended* behavior, not lock in whatever
  the code already happens to do.
- **Keep the diff minimal, and watch for confident-but-wrong code.** Models are weakest
  exactly where this library lives, on the newest C++20/23/26 features and heavy template
  metaprogramming, so a large rewrite full of plausible-looking changes is the dangerous
  case, not the helpful one.
- **Be transparent.** If an agent did significant work, say so, and expect that low-effort,
  untested, or high-volume generated pull requests get closed without much back-and-forth.

None of this is a purity test. It is a way to keep the review cost on the side that created
it, and to protect the one thing a model can't supply: the design integrity that makes the
library worth depending on in the first place.

There is a second half to this, and it is newer. A policy is guidance a human reads; an agent
doesn't. As more of the typing is done by tools, the same guidance has to live where the
tool will see it, in a machine-readable brief at the root of the repository. mp-units keeps
an `AGENTS.md` (with a one-line `CLAUDE.md` that points to it, and a private local overlay
per maintainer) that hands an agent the build, the test commands, and the house conventions
up front. It doesn't replace the policy or the review, but it steers a well-intentioned tool
toward a good contribution instead of a confident guess.

## Do not wait to be discovered

Now the offensive half. Even a perfect library, with clean CI, real documentation, and a
five-minute path to contribute, doesn't spread on its own. Discovery, back in the first post,
gets you found by people already searching. Growth means reaching the ones who are not looking
yet. That is evangelism, and for a library author it isn't marketing gloss, it is doing the
work in public, where your future users already are.

The channels are ordinary and they compound. Conference talks. Writing, including a blog like
this one. Answering questions on r/cpp and in forums. And for a library with ambitions like
mp-units, taking the work into standardization, where the people deciding the language are
exactly the people who will one day depend on it. Each of those is a door back to the project.
A talk that helps someone becomes a reason to try the library. A post that solves a real
problem becomes a search result that keeps finding new users for years.

For a niche library there is a multiplier on top: education. Most C++ developers don't think
about dimensional analysis on an ordinary day, so the writing that lands isn't "here is the
API" but "here is why mixing up units is dangerous, and here is how the type system makes
that mistake impossible." Teach the domain first, and the library follows it in.

But there is an honest limit to evangelism, and it is the note this whole series has been
building toward. Evangelism brings users. It doesn't, on its own, bring maintainers.

## Sustain the maintainer, not just the project

All of it, the policy, the outreach, the releases, the reviews, lands on one person, and the
failure at the end of the journey is rarely a bad release. It is a burned-out maintainer who
quietly stops. So the last part of keeping a project alive is protecting the person keeping
it alive, and two habits do most of the work. Automate the boring: let CI run the tests, let
the formatter end style arguments before they start, let a bot close stale issues, let a
script or a model draft the changelog, so your scarce hours go to the work only you can do.
And empower others: give triage rights to the contributors who keep showing up, write down
how you make decisions so someone else can make them too, and get comfortable saying "not
for this release." It helps to know what you are aiming at, production adoption, a place in
the standard, or simply a healthy community, because that is what tells you which of these
tips deserves your limited time next.

## Where mp-units actually is, and the honest gap

The AI policy is real and it is recent. `CONTRIBUTING.md` carries an explicit AI-Generated
Contributions section with the rules above, written to welcome the tool while protecting
the design. That is the tip practiced, not just preached. The honest nuance is that a policy
on paper is only as strong as the reviewer time behind it, and that is the same
solo-bandwidth limit the Contribution post already owned up to.

Evangelism is a genuine strength. This series exists because of a talk. There is a keynote,
steady standardization work, a regular blog, and a presence on r/cpp. mp-units isn't a quiet
project, and its reach is real.

And yet the gap that closes the series is the one that has shadowed every stage: the
day-to-day load has concentrated back onto one person. It was not always this way, and it
isn't for lack of good people. mp-units has had real co-maintainers,
[Johel Ernesto Guerrero Peña](https://github.com/JohelEGP) and
[Chip Hogg](https://github.com/chiphogg), who shaped the library as much as I did, not only
in code but in reviewing designs, answering issues, and helping users. Chip still weighs in
on the domain side, bringing hard-won metrology experience from his own
[Au library](https://aurora-opensource.github.io/au). What I never managed was to make the
maintainer's load light enough to sustain that depth of involvement over the years, so it
drifted back to a single pair of hands. That is the harder truth about this last stage.
Evangelism has done its job on the demand side and barely moved the supply side, and even
when the supply side shows up, keeping it is a second problem underneath the first. Turning
visibility into a community that can outlive its founder, and survive people stepping back,
is the real unfinished work. It is the hardest item in the entire talk precisely because you
can't automate it or gate it in CI, and it is the part I have least figured out.

That is a fitting place to end. The journey runs from Discovery to Community, and across all
six stages the pattern holds: excellent code is only the price of entry. Every barrier that
actually decides whether a great library gets used, the name, the green build, the docs, the
zero-install demo, the clean integration, the five-minute yes, the release notes, the
recognition, the policy, the outreach, is about people, not code. Get the code right, then
do the unglamorous human work on top of it. That is how a great library stops being a
well-kept secret.

These tips come from my conference talk on why technically excellent C++ libraries fail to
get adopted, and how to fix it. You can
[watch the recording](https://www.youtube.com/watch?v=DWXlyOd_z88), which won best talk at
using std::cpp 2026, and read
[the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
I am giving an updated version of it at CppCon 2026, and the full, extended two-hour keynote
at Meeting C++ 2026.
