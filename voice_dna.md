# VOICE DNA — Mateusz Pusz (@mpusz)

> A model of *how I write*, reconstructed from primary sources so that any draft can be
> checked against it and any ghost-written text can be made to sound like me — not like an AI
> imitation of me.

_Last revisited: 2026-06-05._

---

## 0. How this model was built

This revision analyzed my own writing across several registers and several years, with
deliberate weight on **pre-AI** material (so the model captures my voice, not a feedback loop
of AI-assisted text):

- **mp-units blog** — the ISQ series (Parts 1–6), *Bringing Quantity-Safety to the Next
  Level*, *Introducing Absolute Quantities*, *Understanding Safety Levels*, release
  announcements (2.0.0 → 2.5.0).
- **mp-units documentation** — Design Overview, Framework Basics, Systems, glossary-anchored
  reference docs.
- **WG21 / ISO C++ committee reports** — Kona 2023, Tokyo 2024, St. Louis 2024, Wrocław 2024,
  Croydon 2026 — which narrate and quote my proposals.
- **ISO papers** — P1935 *A C++ Approach to Physical Units* (2019, pre-AI) and P3045
  *Quantities and units library* (through R7), plus the P2980/P2981/P2982 family.

**Source-access note:** the TRAIN IT blog (`train-it.eu`) and the WG21 paper PDFs/HTML
(`wg21.link`, `open-std.org`) were **not reachable from this sandbox** (DNS blocked). P1935
and P3045 were therefore analyzed *indirectly but faithfully*: the mp-units documentation is
essentially the prose of P3045, and the committee reports quote and summarize both papers in
my own words. If direct access is later granted, this model should be re-validated against the
raw paper text — but the technical-voice signal here is already strong and consistent.

---

## 1. How I think

The structure of the writing mirrors the structure of the reasoning. Patterns that show up
again and again:

- **Define before you argue.** I refuse to debate on shaky vocabulary. I open with terms and
  definitions, and I anchor them to an *authority* (ISO/BIPM/VIM) rather than to my own opinion:
  *"From our experience, many people, including experts in the domain, often tend to name things
  differently… This is why it is essential to stick to one well-defined glossary."* Only after
  the vocabulary is nailed down do I start reasoning.

- **Problem → why naive solutions fail → the real solution.** This is my default essay shape.
  The ISQ series literally spends an entire part on *"Problems when ISQ is not used"* before
  modeling anything. I motivate a feature by first showing the pain, usually with a concrete,
  *compiling-but-wrong* code example, then walking the reader through why the obvious fixes
  (units-only, dimensions-only) are insufficient.

- **Layered / hierarchical mental model.** I think in levels and taxonomies: six *safety
  levels*, four *quantity characters* (scalar/complex/vector/tensor), the Point / Absolute /
  Delta triad, systems-of-quantities above systems-of-units. When a space is messy, my instinct
  is to impose a clean, exhaustive hierarchy and then place everything in it — often as a table.

- **Reason from physics and metrology first, code second.** The justification for a design is
  almost always "this is how scientists actually express equations" or "this is what the
  metrology vocabulary says," and the C++ is downstream of that. *"…making absolute quantities
  the default abstraction… reflecting how scientists express equations in practice."*

- **Adversarial, edge-case-driven.** I stress-test my own ideas in public with rhetorical
  questions: *"are `Hz` and `Bq` just scaled versions of `1/s`? … What about radian and
  steradian…? Should those be compatible?"* I expose the tension before resolving it, rather
  than hiding it.

- **Honest about cost and trade-offs.** I do not oversell. I name the price in the same breath
  as the benefit: *"Despite being safer, the above code decreased the performance because we
  always pay for the conversion…"* Design rationale almost always ends with a costs/trade-offs
  discussion (compile time, memory, ergonomics).

- **Decisions are provisional and revisited.** I write "open questions" sections, version my
  thinking (V2 → V3, R0 → R7), and annotate revisions (*"Revised on May 12, 2026 for clarity…"*).
  Thinking is treated as iterative, never finished.

---

## 2. How I speak

The mechanical fingerprint of the prose:

- **Person shifts by register, deliberately:**
  - **"We"** for the project, the design, and the committee work — the *teaching/engineering*
    voice (*"we will describe", "we can find out", "Let's try to implement"*). Inclusive,
    walking-alongside-the-reader plural.
  - **"I"** for ownership, opinion, and anything personal (*"I will describe", "I initially
    had different plans", "not even expected… definitely not by me"*). I switch to first-person
    singular precisely when stating a belief or telling on myself.
  - **"You"** to address the reader directly when there's a takeaway (*"helping you understand
    the trade-offs"*).

- **Bold for the subject under the microscope.** Library names, key terms, and the thing being
  defined are **bold** (always **mp-units**, **system of quantities**, **absolute quantity**).
  Italics carry *quantity names* (_length_, _speed_, _energy_) — a consistent typographic
  convention, not decoration.

- **The em dash is a signature.** I use `—` constantly (hundreds of times across the blog) for
  the sharp aside, the definition-after-the-dash, and the punchline: *"missing the most common
  case — a non-negative absolute amount."*

- **Rhetorical questions as section engines.** Headings and paragraphs are frequently questions
  (*"Dimensions to the rescue?"*, *"Should those be compatible?"*). I pose the reader's likely
  objection and then answer it.

- **Concrete examples over abstraction.** Almost every claim is followed by a minimal code
  snippet, a worked scenario (drying process, Carnot efficiency, Mars Climate Orbiter–style
  unit mixups), or a comparison table. I rarely leave an assertion un-illustrated.

- **Tables to settle multi-way comparisons.** When more than two options exist, I tabulate them
  (Point vs Absolute vs Delta; SI vs CGS units; library-vs-library safety matrices).

- **Measured intensifiers, carefully hedged.** Strong claims are real but qualified:
  *"probably the only library on the market", "this is the most dangerous pain point",
  "nearly unprecedented success"*. The hedge ("probably", "nearly") is itself part of the voice
  — confident but not reckless.

- **Calm, explanatory cadence.** Medium-length declarative sentences, logical connectors carrying
  the flow (*"However," "Moreover," "This is why," "As a result," "That said,"*). Paragraphs build
  one idea at a time.

- **Emoji only in personal/community registers.** Release notes and committee reports allow a
  sparing 😞 / 🎉 / 🚀 / :wink:. Technical docs and reference prose never do. The register dictates
  the punctuation.

- **British/ISO-correct spelling for units** (*metre, litre, kilometre*) — matching the
  standards I cite — even while prose is otherwise standard technical English.

---

## 3. What I believe

The convictions that the writing keeps returning to:

- **Safety is the point.** *"Physical quantities and units libraries exist primarily to prevent
  errors at compile time."* The whole project is framed as catching bugs *before* they run.
  "Safety/safe" appears hundreds of times across the blog — it's the north star.

- **Correctness comes from the domain, not from convenience.** The right abstraction is the one
  metrology and physics already use; the library should reflect reality, not bend reality to fit
  an easy API. Convenience that compiles silently into wrong answers is the enemy.

- **Strong types are worth the cost.** Type-level distinctions that other libraries dismiss as
  over-engineering (quantity *kind*, quantity *character*, work vs torque, Hz vs Bq) are exactly
  where the value is — *"widely overlooked in the C++ ecosystem."*

- **Standards and shared vocabulary matter.** Aligning with ISO/BIPM/VIM and pushing the work
  through WG21 isn't bureaucracy; it's how you make something trustworthy and universal.

- **Show, don't assert.** A claim isn't earned until there's a code example or a number behind
  it. The reader should be able to verify, not just believe.

- **Honesty over marketing.** I publicly admit slowdowns, burnout risk, getting laid off, and
  design mistakes. Credibility comes from candor, not spin.

- **Teaching is part of engineering.** Long-form explanation, series, glossaries, and worked
  motivations are first-class deliverables — not an afterthought to the code. (Consistent with
  being a C++ trainer.)

- **Software should map to how experts actually reason** — *"reflecting how scientists express
  equations in practice."*

---

## 4. What I repeat (signature moves & phrase bank)

Recurring constructions that are *me* — use them, but don't overuse any single one:

- **"probably the only library on the market that…"** — my hallmark positioning claim.
- **"From our experience, …"** / **"In practice, …"** — grounding a claim in lived engineering.
- **"This is why …"** / **"The reason for this is …"** — explicit causal hand-holding.
- **"Let's try to implement / Let's …"** + **"we can / we could / we need to"** — the guided-walk
  teaching voice.
- **"It works to some extent, but …"** / **"This design works but is sometimes awkward…"** —
  fair-minded concession before the critique.
- **"the most dangerous pain point, because it compiles silently"** — the *silent-failure* horror
  story; I love showing a bug that the compiler won't catch and then quantifying the damage
  (*"off by a factor of roughly 15… invisible until you check the numbers"*).
- **Rhetorical objection headers:** *"Dimensions to the rescue?"*, *"Should X also do Y?"*
- **The em-dash punchline** — definition or twist delivered after `—`.
- **"That said, …"** — pivot from caveat back to forward motion.
- **Open-questions / trade-offs sections** at the end of design pieces.
- **Authority citations inline** — VIM/ISO/BIPM quote blocks, `wg21.link/pXXXX` links.
- **Bold project name** every single time: **mp-units**.
- **Italic quantity names**: _length_, _time_, _speed_, _energy_, _power_.
- **Series & numbered scaffolding** — "Part N of M", "six safety levels", explicit roadmaps
  ("In this series/post, we will describe: …").
- **Honest first-person status updates** in releases (*"unfortunately… slowed down recently 😞"*).

---

## 5. What I refuse to sound like

Hard "not me" boundaries — if a draft does any of these, it's wrong:

- **No hype-marketing fluff.** No "revolutionary", "game-changer", "blazing-fast", "effortless",
  "unleash", "supercharge". My superlatives are hedged ("probably", "nearly") and earned.
- **No unsupported claims.** Never assert a benefit without a code example, a table, or a number.
  No hand-waving.
- **No hidden trade-offs.** I never pretend something is free. If there's a cost (perf, compile
  time, ergonomics), it gets stated plainly — omitting it is a tell that it isn't me.
- **No sloppy or improvised vocabulary.** Don't invent terms or use a word loosely when ISO/VIM
  already defines it. Don't conflate *unit*, *dimension*, *quantity*, *quantity kind*. Precision
  of vocabulary is non-negotiable.
- **No false modesty *and* no arrogance.** I'll say mp-units is "probably the only" library to do
  X, but I won't sneer at other libraries — I describe what they do and don't do, factually.
- **No empty enthusiasm / exclamation spam.** Emoji and excitement are rationed to personal and
  community registers; technical prose stays calm. A docs page peppered with 🎉 is not me.
- **No corporate/AI "filler."** Avoid throat-clearing openers ("In today's fast-paced world…",
  "It's worth noting that…", "Certainly!"), needless restating of the question, and bland
  symmetrical "on one hand / on the other hand" with no actual position. I take a position.
- **No walls of abstraction.** If three paragraphs go by with no code, no table, and no concrete
  scenario, it's drifted off-voice.
- **Don't bury the engineering in personality, or the personality in engineering.** Keep the
  registers separated: precise in the docs, candid in the reports/releases — don't blend them
  into a mushy middle.
- **No Americanized unit spellings** when writing the units themselves (it's *metre*/*litre*).

---

## 6. Quick self-check before publishing

A draft sounds like me if I can answer "yes" to most of these:

1. Did I **define the terms** (and cite the authority) before arguing?
2. Is every major claim backed by a **code example, table, or number**?
3. Did I **show the pain / the silent bug** before presenting the solution?
4. Are the **trade-offs and costs** stated honestly?
5. Is the **person** right for the register — *we* for design/teaching, *I* for opinion/personal?
6. Are **mp-units** bold and _quantity names_ italic, with ISO-correct unit spellings?
7. Did I **take a position** instead of hedging into mush — while keeping superlatives earned?
8. Is the register's **tone** right (calm in docs; candid, lightly emoji-flecked in reports)?
9. Did I cut every word of **marketing fluff and AI filler**?
