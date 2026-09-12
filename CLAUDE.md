# CLAUDE.md

Claude Code loads this file automatically. The shared, vendor-neutral guidance for this
repository lives in `AGENTS.md` (also used by other agent tools) and is imported below. Personal,
machine-specific notes go in `CLAUDE.local.md` (gitignored), which Claude also auto-loads.

@AGENTS.md

## Keep the learning-resources post in sync

The blog post `docs/blog/posts/interactive-learning-tutorials-and-workshops.md` states how many
tutorials and workshops exist, their per-section lesson lists, and the estimated times. Those numbers
drifted once already (the post said 14 lessons and ~3.5 h while the docs listed 17 and ~3.9 h, and 14
workshops against 15). **Whenever a tutorial or workshop is added, removed or renamed, or a section
time estimate changes in `docs/tutorials/index.md` or `docs/workshops/`, update the counts, the lists
and the times in that post in the same change.** The post is the announcement readers land on from
search, so a stale count there reads as an abandoned feature.

## What makes a good first issue here

Most of the open work in this repository is unsuitable for a newcomer, and proposing it anyway
wastes their time and ours. **A good first issue must not require any of the following:**

- **ISQ or quantity-spec modeling judgment.** Deciding a quantity's parent, character, or kind is
  the hardest design work in the library, not an on-ramp.
- **A paywalled standard.** ISO 80000 parts are not something a contributor can be assumed to have,
  and they are not written with an implementer in mind.
- **Domain expertise the issue cannot supply.** Examples under `example/` model real domain
  calculations, so a good one needs knowledge the task itself cannot hand over. Those arrive from
  contributors who already have the domain (see the audio examples in #644); they are contributions
  to accept, not tasks to assign.

What is left, and where the well-formed ones have come from: **text output**, **tooling and CI**, and
**documentation where the answer already exists** somewhere (an answered discussion, a thread, an
existing page). See #831, #832 and #833 for the shape.

Check one more thing before labelling: **an issue blocked on an unmade decision is not a first
issue.** Several here turn on a design call only the maintainer can make (#652, the scaled-unit
output format) or on a committee one (#557, #541). Make the decision first and the issue becomes
fine; label it before, and the newcomer walks into an argument. Use `gated` for those.
