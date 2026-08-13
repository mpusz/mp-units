---
date: 2026-10-01
authors:
 - mpusz
categories:
 - Best Practices
 - Why Great C++ Libraries Fail
comments: true
---

# Let them try it before they install anything

You found a promising library. To actually try it, you clone the repository, install a
toolchain it expects, wire it into your build system, and resolve its dependencies. One
hour later, before you have written a single line of your own, you are debugging someone
else's CMake. Most people do not get that far. They close the tab.

<!-- more -->

!!! info "Part of a series: Why Great C++ Libraries Fail"

    This post is part of a
    [series](../../../../category/why-great-c-libraries-fail/) based on my using std::cpp 2026
    talk on why technically excellent C++ libraries fail to get adopted. It covers the
    **Integration** stage of the six-stage library journey: can people actually use it? New
    here? Start with [the overview](nobody-uses-your-great-library.md).

The gap between "this looks interesting" and "I tried it" is where you lose people who were
ready to adopt you. Every setup step is another chance for them to give up. The single most
effective thing you can do about it is let a stranger run your code in a browser, before they
install anything at all.

## Compiler Explorer is the onramp

[Compiler Explorer](https://godbolt.org), by [Matt Godbolt](https://xania.org), is arguably
the most important C++ tool of the last decade, and most projects use a fraction of what it
offers. The headline feature is zero install: a stranger can compile and run your library
in a browser tab with nothing on their machine. Sharing is a single URL, not a repository
plus a page of build instructions. Getting your own library added is easier than people
expect: for a header-only library it is close to trivial, and the team is friendly and
responsive about it.

## One tool, the whole lifecycle

The same link pays off at almost every stage of a project, not just the first demo:

- **Prototyping.** Explore an idea in seconds, with no build system to stand up first.
- **Design discussion.** Send a link that compiles instead of a wall of code and "apply this
  patch, build with these flags."
- **Bug reports.** Require a Compiler Explorer reproduction link in your issue template, and
  the "it does not work on my machine" loop simply ends. mp-units' bug report form asks for
  exactly this.
- **Documentation.** "Try it" links and embedded, editable examples turn passive readers into
  people who change a unit and watch what the compiler says.
- **Contributor onboarding.** Someone can explore how your library works, and prototype a
  fix, before they ever clone the repository.

## Show the assembly, do not claim zero overhead

This is the use that matters most for a library that promises performance, and it is the proof
I promised back in the Evaluation post. "Zero overhead" is easy to assert and easy to doubt.
So do not assert it. Show the generated assembly.

The editor below holds two functions that do the same thing: one on raw `double`s, one on
fully type-checked mp-units quantities. Check the assembly pane for yourself.

<iframe width="100%" height="350px" src="https://godbolt.org/e#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:16,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:2,endLineNumber:14,positionColumn:2,positionLineNumber:14,selectionStartColumn:2,selectionStartLineNumber:14,startColumn:2,startLineNumber:14),source:'%23include+%3Cmp-units/systems/si.h%3E%0A%0Ausing+namespace+mp_units%3B%0Ausing+namespace+mp_units::si::unit_symbols%3B%0A%0Adouble+ttg_s(double+d_m,+double+v_mps)%0A%7B%0A++return+d_m+/+v_mps%3B%0A%7D%0A%0Aquantity%3Cs%3E+ttg(quantity%3Cm%3E+d,+quantity%3Cm/s%3E+v)%0A%7B%0A++return+d+/+v%3B%0A%7D'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:50,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:clang2110,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'1',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:16,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!((name:mp-units,ver:trunk)),options:'-std%3Dc%2B%2B23+-O3+-DNDEBUG',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+21.1.0+(Editor+%231)',t:'0')),k:50,l:'4',m:100,n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4"></iframe>

Both reduce to a single `divsd` and a `ret`, identical to the hand-written version. The type
safety and dimensional analysis are gone by the time the compiler is done, with nothing left
to pay for at runtime. "I see no extra generated code" is more convincing than any benchmark
I could publish, because the reader verifies it themselves in thirty seconds, on the compiler
they care about, without trusting me at all. That is the honest way to make a performance
claim: hand the skeptic the means to check it.

## Make it real for your library

- **Get added to Godbolt.** Open an issue or PR on the
  [compiler-explorer](https://github.com/compiler-explorer/compiler-explorer) repository.
  For header-only libraries it is a small change, and the maintainers are helpful.
- **Put a "Try it" link on every example.** mp-units leads its README with a live Compiler
  Explorer badge, so the first thing a visitor can do is run the code.
- **Generate embeds from real code, do not hand-paste them.** A pasted Compiler Explorer
  iframe, like the one above, freezes its source inside the URL, so it drifts out of date
  exactly like a deep link the moment the API moves. Generating the embed from an ordinary
  code block instead keeps it current (see below).
- **Require a reproduction link in bug reports**, and provide **release tags** so a shared
  link pins to a stable version rather than moving under your users.

## Keep the embedded code in sync

The embed above is convenient but frozen: its source lives in the URL, not in the docs, so
nothing keeps it honest as the API changes. For the mp-units tutorials and workshops I fixed
that with a small script. The example stays an ordinary fenced `cpp` block in the Markdown,
and its first line is a directive comment:

`// ce-embed height=600 compiler=clang2110 flags="-std=c++23 -O3" mp-units=trunk`

A little JavaScript
([`ce_embed.js`](https://github.com/mpusz/mp-units/blob/master/docs/javascripts/ce_embed.js))
turns every such block into a live Compiler Explorer iframe when the page loads.

The point is that the code is a normal code block, the single source of truth. It renders
as plain, readable code even with JavaScript off, it is the text the reader copies, and
because
it is real source it can be extracted and compiled in CI, so it cannot silently rot against
the API. Change the block and the embed changes with it. The
[interactive tutorials](../../tutorials/index.md) use this throughout.

## Where mp-units actually is, and one honest gap

This is a stage mp-units gets right. It is on Compiler Explorer, the README opens with a "Try
it live" badge, the tutorials embed interactive examples, and the bug report form asks for
a Compiler Explorer link. A visitor really can go from "interesting" to "running it" in one
click.

The honest gap is the zero-overhead proof itself. Today it is demonstrated, not automated.
I verify the assembly by eye on Compiler Explorer, not in CI. Automating it is harder than
it sounds, because the same source compiles to different but equally optimal instructions
across compilers and flags, so a stored golden file would be brittle. The robust approach
is differential: compile the mp-units version next to a hand-written baseline under the same
toolchain and assert they match, and it is currently an open good-first-issue
([#804](https://github.com/mpusz/mp-units/issues/804)). So for now the guarantee is shown
and checked by eye, not gated by CI. The proof is real. The automation is still on the list.

These tips come from my talk on why technically excellent C++ libraries fail to get
adopted, and how to fix it. You can
[watch the using std::cpp 2026 version](https://www.youtube.com/watch?v=DWXlyOd_z88), or
browse [the slides](https://github.com/train-it-eu/conf-slides/tree/master/2026.03%20-%20using%20std_cpp).
An expanded version is coming as a keynote at Meeting C++ 2026.
