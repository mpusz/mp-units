# Contributing

Thank you for your interest in contributing to **mp-units**! 👍🎉

Before contributing, we strongly recommend familiarizing yourself with the
[official documentation](https://mpusz.github.io/mp-units).

??? info "This file is a part of MkDocs documentation"

    This file is part of the documentation and uses some MkDocs‑specific formatting that may
    look unusual in a plain Markdown renderer. For the fully rendered version, visit the
    [Contributing](https://mpusz.github.io/mp-units/latest/getting_started/contributing)
    chapter.

    It is symlinked into the documentation as _docs/getting_started/contributing.md_, so links
    to other repository files have to be written as absolute `https://github.com/mpusz/mp-units`
    URLs. A repository-relative link would resolve against the documentation tree and break on
    the website.


## Where to Start

- See [good first issues](https://github.com/mpusz/mp-units/labels/good%20first%20issue)
  for beginner-friendly tasks.
- [help wanted](https://github.com/mpusz/mp-units/labels/help%20wanted) and
  [high priority](https://github.com/mpusz/mp-units/labels/high%20priority)
  labels highlight other important work.


## Preconfigured Development Environment

The easiest way to start coding is to use our containerized development environment in
[GitHub Codespaces](https://docs.github.com/en/codespaces). You can either click the
button below

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/mpusz/mp-units)

or navigate to the main page of the repository and click "Code" button -> "Codespaces"
tab -> "Create codespace on master".

You can also use the pre-configured devcontainer and Docker image manually
within your IDE.

### Setting Up Your Codespace

1. **Fork the repository** on GitHub (if you haven't already)
2. **Create a CodeSpace** from your fork
    - Navigate to your forked repository
    - Click "Code" → "Codespaces" → "Create codespace on master"
    - Choose a 4-core machine or larger for better performance

3. **Wait for initialization** (~3–5 minutes)
    - Container will download and start automatically
    - Conan will be configured for your environment
    - All VS Code extensions will be installed

### Helpful commands

```bash
# Test with a single compiler configuration
conan build . -pr gcc15 -c user.mp-units.build:all=True -b missing

# Build and test with all supported compilers
.devcontainer/check_all.sh build

# ... or build them as Conan packages instead
.devcontainer/check_all.sh create

# Generating API Reference
.devcontainer/api_reference.sh

# Preview documentation locally
mkdocs serve

# Run pre-commit checks
pre-commit run --all-files
```

For more, see the [official documentation](https://mpusz.github.io/mp-units) and
[`.devcontainer/README.md`](https://github.com/mpusz/mp-units/blob/master/.devcontainer/README.md).

### Tips

- Use the provided scripts for building, testing, and packaging. See the docs for
  [advanced Conan/CMake options](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#build-options).
- For documentation, see the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/)
  guide.
- Run `.devcontainer/check_all.sh build` to test across all supported compilers (the script
  requires one of `install`, `build`, or `create`; run it with `-h` for the options).
- Use `pre-commit` to auto-format code and check style before committing.
- See the [C++ compiler support](https://mpusz.github.io/mp-units/latest/getting_started/cpp_compiler_support)
  page for supported toolchains.


??? info "Alternative: Custom Development Environment"

    ## Custom Development Environment

    Alternatively, please refer to our official docs for
    [download, build, and install instructions](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage)
    with the below changes if you want to set up a development environment on your local machine.

    ### Building the entire repository

    To build all the **mp-units** source code (with unit tests and examples), you should:

    1. Use the _CMakeLists.txt_ from the top-level directory.
    2. Run Conan with [`user.mp-units.build:all`](#user.mp-units.build-all) = `True`.

    ```shell
    git clone https://github.com/mpusz/mp-units.git && cd mp-units
    conan build . -pr <your_conan_profile> -s compiler.cppstd=23 -c user.mp-units.build:all=True -b missing
    ```

    The above will download and install all of the dependencies needed for the development of the library,
    build all of the source code, and run unit tests.

    If you prefer to build the project via CMake rather than Conan, then you should replace
    the `conan build` with `conan install` command and then follow with a regular CMake build and testing:

    ```shell
    conan install . -pr <your_conan_profile> -s compiler.cppstd=23 -c user.mp-units.build:all=True -b missing
    cmake --preset conan-default
    cmake --build --preset conan-release
    cmake --build --preset conan-release --target all_verify_interface_header_sets
    cmake --build --preset conan-release --target test
    ```

    !!! hint

        To ensure that we always build all the targets and to save some typing of the Conan commands,
        we can set the following in the `~/.conan2/global.conf`:

        ```text
        user.mp-units.build:all=True
        ```

    ### Packaging

    To test CMake installation and Conan packaging run:

    ```shell
    conan create . --user <username> --channel <channel> -pr <your_conan_profile> -s compiler.cppstd=23 \
                   -c user.mp-units.build:all=True -b missing
    ```

    The above will create a Conan package and run tests provided in _./test_package_ directory.

    In case you would like to upload **mp-units** package to the Conan server, do the following:

    ```shell
    conan upload -r <remote-name> mp-units/<version>@<user>/<channel>
    ```


    ### Building documentation

    We are building our documentation using [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/).
    The easiest way to install all the required dependencies is with `pip` and the
    [`requirements.txt`](https://github.com/mpusz/mp-units/blob/master/requirements.txt) file
    provided in the repository:

    ```shell
    pip install -U -r requirements.txt
    ```

    Additionally, a [Cairo Graphics library](https://www.cairographics.org/) is required by
    Material for MkDocs. Please follow the
    [official MkDocs documentation to install it](https://squidfunk.github.io/mkdocs-material/plugins/requirements/image-processing/#cairo-graphics).

    After that, you can either:

    - easily [start a live server to preview the documentation as you write](https://squidfunk.github.io/mkdocs-material/creating-your-site/#previewing-as-you-write)

        ```shell
        mkdocs serve
        ```

    - [build the documentation](https://squidfunk.github.io/mkdocs-material/creating-your-site/#building-your-site)

        ```shell
        mkdocs build
        ```

    ### Generating API reference

    We need to take a few steps to set up our environment so that we are ready to generate API reference
    documents.

    First, we need to satisfy the requirements described in <https://github.com/cplusplus/draft> and
    <https://github.com/Eelis/cxxdraft-htmlgen>.
    On the Ubuntu platform, this is equivalent to the following instructions run from the user's home
    directory:

    ```bash
    sudo apt install latexmk texlive-latex-recommended texlive-latex-extra texlive-fonts-recommended lmodern
    sudo apt install haskell-stack graphviz nodejs npm ghc cabal-install
    npm install split mathjax-full mathjax-node-sre mathjax-node-cli yargs@16.2.0
    cabal update
    ```

    On some platforms, installing `mathjax-node-cli` through `npm` does not update the system's `PATH`
    environment variable resulting in `tex2html` not found errors. In such cases we need to add
    the `.bin` folder to the `PATH` environment variable manually:

    ```bash
    echo "export PATH=\"~/node_modules/.bin:\$PATH\"" >> ~/.bashrc && source ~/.bashrc
    ```

    Now, we are ready to start building our API reference using our automated script:

    ```bash
    .devcontainer/api_reference.sh
    ```

    This script will:

    - Configure CMake with the correct module paths
    - Build both PDF and HTML documentation
    - Create a symlink to the generated documentation

    If you only want to setup CMake without building the documentation, use:

    ```bash
    .devcontainer/api_reference.sh -s
    ```

    #### Custom Dependency Directory

    By default, the script downloads dependencies to `../api_reference_deps` relative to the
    project root. You can specify a custom directory using the `-d` parameter:

    ```bash
    # Use a custom directory for dependencies
    .devcontainer/api_reference.sh -d /path/to/custom/deps

    # Combine with setup-only mode
    .devcontainer/api_reference.sh -s -d ./local_deps
    ```

    The dependency directory will contain:

    - `jegp.cmake_modules`: CMake modules for documentation generation
    - `draft`: C++ standard draft sources for reference formatting
    - `cxxdraft-htmlgen`: HTML generation tools for standardese-style output

    This is useful when you want to:

    - Share dependencies across multiple project checkouts
    - Avoid re-downloading dependencies for development containers

    #### Manual documentation generation

    Alternatively, you can run the individual steps manually.

    First, we need to clone the following git repositories:

    - <https://github.com/JohelEGP/jegp.cmake_modules>
    - `standardese_sources_base` branch of <https://github.com/JohelEGP/draft>
    - `standardese_sources_base` branch of <https://github.com/JohelEGP/cxxdraft-htmlgen>

    For example:

    ```bash
    git clone https://github.com/JohelEGP/jegp.cmake_modules.git --depth=1
    git clone https://github.com/JohelEGP/draft.git --branch=standardese_sources_base --depth=1
    git clone https://github.com/JohelEGP/cxxdraft-htmlgen.git --branch=standardese_sources_base --depth=1
    ```

    Next, configure CMake with the following:

    ```bash
    cmake -S docs/api_reference/src -B build/docs/api_reference \
          -DCMAKE_MODULE_PATH="<path to gh:JohelEGP/jegp.cmake_modules>/modules" \
          -DJEGP_STANDARDESE_SOURCES_GIT_REPOSITORY="<path to gh:JohelEGP/draft>" \
          -DJEGP_CXXDRAFT_HTMLGEN_GIT_REPOSITORY="<path to gh:JohelEGP/cxxdraft-htmlgen>"
    ```

    Then build the documentation with CMake:

    ```bash
    cmake --build build/docs/api_reference
    ```

    The generated documentation will be available at:

    - PDF: `build/docs/api_reference/mp-units.pdf`
    - HTML: `build/docs/api_reference/mp-units.html/`

    In the end, we need to move the generated documentation to the `docs/api_reference/gen` subdirectory:

    ```bash
    mv build/docs/api_reference/mp-units.html docs/api_reference/gen
    ```

    or just link the entire directory:

    ```bash
    ln -sf ../../build/docs/api_reference/mp-units.html docs/api_reference/gen
    ```


## Configuration Options for Developers and Contributors

### Conan

[`user.mp-units.build:all`](#user.mp-units.build-all){ #user.mp-units.build-all }

:   Enables compilation of all the source code, including tests and examples. To support
    this, it requires some additional Conan build dependencies described in
    [Repository directory tree and dependencies](https://mpusz.github.io/mp-units/latest/getting_started/project_structure#cmake-projects-and-dependencies).
    It also runs unit tests during the Conan build (unless
    [`tools.build:skip_test`](https://docs.conan.io/2/reference/commands/config.html?highlight=tools.build:skip_test#conan-config-list)
    configuration property is set to `True`).

    [conan build all support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

<!-- markdownlint-disable-next-line MD013 -->
[`user.mp-units.analyze:clang-tidy`](#user.mp-units.analyze-clang-tidy){ #user.mp-units.analyze-clang-tidy }

:   Enables clang-tidy analysis.

    [conan clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0


### CMake

[`MP_UNITS_DEV_IWYU`](#MP_UNITS_DEV_IWYU){ #MP_UNITS_DEV_IWYU }

<!-- markdownlint-disable-next-line MD013 MD052 -->
:   [:octicons-tag-24: 2.2.0][cmake iwyu support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

    Enables include-what-you-use analysis.

    [cmake iwyu support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_CLANG_TIDY`](#MP_UNITS_DEV_CLANG_TIDY){ #MP_UNITS_DEV_CLANG_TIDY }

<!-- markdownlint-disable-next-line MD013 MD052 -->
:   [:octicons-tag-24: 2.2.0][cmake clang-tidy support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

    Enables clang-tidy analysis.

    [cmake clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_TIME_TRACE`](#MP_UNITS_DEV_TIME_TRACE){ #MP_UNITS_DEV_TIME_TRACE }

<!-- markdownlint-disable-next-line MD013 MD052 -->
:   [:octicons-tag-24: 2.5.0][cmake time-trace support] · :octicons-milestone-24: `NONE`/`ALL`/`MODULES`/`HEADERS` (Default: `NONE`)

    Enables compilation performance data collection with `-ftime-trace` for clang compilers.

    All our unit tests compile only for headers and never for modules. To allow fair
    comparison, `MODULES` and `HEADERS` do not enable the data collection for unit tests.
    This means that they affect only the core, systems, and examples.

    Please use `ALL` to profile unit tests as well.

    [cmake time-trace support]: https://github.com/mpusz/mp-units/releases/tag/v2.5.0


## AI-Generated Contributions

We welcome AI assistants and coding agents (LLMs) to help you write prose, generate tests,
explore a design, or draft a change. They are useful tools. But **mp-units** is a bleeding-edge
C++20/23/26 library, and the bar we hold code to does not drop because a machine wrote it.
Whatever tool you use, **you are the author**: you are accountable for every line, and we
review it as your work.

1. **Understand and defend every line:** Never open a "blind" pull request. If you cannot
  explain why a change is correct and why it is designed the way it is, it is not ready,
  however confident the model sounded.
2. **Build and test it yourself first:** Run the change in the provided `.devcontainer` (or
  GitHub Codespaces) and make the tests pass locally, rather than leaving that to CI or to
  the reviewer. AI-written tests are especially suspect: a test must encode the *intended*
  behavior, not lock in whatever the code currently happens to do.
3. **Keep the diff minimal:** No unrequested reformatting, renaming, or file-wide rewrites.
  Large, monolithic diffs for a small change will be rejected.
4. **Watch for confident-but-wrong code:** Models are weakest exactly where this library
  lives, on the newest language features, template metaprogramming, and compile-time
  interfaces. Check for legacy patterns, needless template machinery, and subtly incorrect
  metaprogramming.
5. **Be transparent:** If a coding agent or an LLM did a significant part of the work,
  say so in the pull request, so reviewers know where to look hardest.

Low-effort, untested, or high-volume agent-generated pull requests, submitted faster than
a human could have reviewed them, will be closed without a long back-and-forth. A contribution
is a request for our time; please make sure it is worth more of ours than it cost you.

If you drive your contribution with an AI coding agent, point it at
[`AGENTS.md`](https://github.com/mpusz/mp-units/blob/master/AGENTS.md), which gives the agent
the build, test, and design pointers it needs to follow these rules.


## Before committing git changes

There are a few recommended checks before committing and pushing changes.

### Naming conventions

Here are the main rules for naming things in this repo:

- types, functions, variables use `standard_case`,
- template parameters use `PascalCase`,
- C++ concepts: `PascalCase` ([subject to change](https://github.com/mpusz/mp-units/issues/93))

Avoid one-letter lowercase names for variables and parameters. The library defines very short
unit symbols (`m`, `s`, `l`, `h`, ...), so such a name easily collides with one of them, and
MSVC then reports a shadowing warning that no other compiler produces. Prefer descriptive
names, and `lhs`/`rhs` for the two sides of a binary operator. Short uppercase template
parameter names are fine.

### Code style

Formatting is handled for you by `pre-commit`, but a few conventions are worth knowing before
you write **mp-units** code:

- **Prefer CTAD to `auto`** for the library's own types — `quantity q = 42 * m;` rather than
  `auto q = 42 * m;`. The type name carries semantic weight, and hiding it costs the reader
  more than the typing saves. A constrained `QuantityOf<isq::length> auto` is a reasonable
  middle ground when the type is genuinely not evident from the right-hand side. Plain `auto`
  remains fine for standard library types such as iterators.
- **Prefer copy initialization** (`T var = expr;`) to direct initialization (`T var{expr};`).
  Copy initialization considers only implicit conversions, so the syntax itself records
  whether a conversion was meant to be implicit; braces quietly accept explicit ones too.
- **Comments explain _why_, not _what_.** A comment narrating what the following block does
  usually means that block wants to be a named function, or that a condition wants to be a
  named concept; extract it and drop the comment. Keep the comments no name can carry —
  compiler-bug workarounds, standard-mandated subtleties, invariants, and references to a
  specification.

### Unified code formatting

A formatting standard is enforced with the `pre-commit` script. Before committing your changes,
please do the following:

```bash
pip install -U pre-commit
pre-commit run --all-files
```

This will run:

- `clang-format` for code formatting with the `.clang-format` file provided in the repo,
- `cmake-format` to format the CMake files,
- `markdownlint-cli2` for the Markdown documentation (`MD013` caps the line length, so prose
  has to be wrapped),
- `shellcheck` for the shell scripts and `actionlint` for the GitHub Actions workflows,
- `black`, `isort`, and `flake8` for the Python scripts,
- some other checks (e.g., trailing whitespace, YAML and JSON validity, etc.).

The script will run on all the files in the repo and will apply the changes in place when
needed. After it finishes review and stage all modified files.

### Header placement

Everything the library exports lives in the public directories. A header under a `bits/`
subdirectory holds implementation details only and **must not export anything**: no
`MP_UNITS_EXPORT`, no `MP_UNITS_EXPORT_BEGIN`. If an entity has to be visible to users, or
has to be named by an exported declaration, its header belongs in a public directory such
as _framework/_ instead.

Note that this is about the *directory*, not the namespace. An entity may perfectly well stay
in `mp_units::detail` while living in a public header, because being reachable to the module's
importers and being part of the supported API are two different things.

Relatedly, never place an `#include` in the middle or at the end of a header. When
splitting a large header up, decompose it into self-contained components in a linear
dependency chain plus a thin aggregator whose includes all sit at the top, keeping the
public aggregator path stable so that existing include sites are unaffected.

### Backward compatibility

Before submission, please remember to check if the code compiles fine on the supported compilers.
The CI will check it anyway, but it is good to check at least some of the configurations before
pushing changes.
Especially older compilers can be tricky as those do not have full C++20 conformance.
The official list of supported compilers can always be found in the
[C++ compiler support (API/ABI)](https://mpusz.github.io/mp-units/latest/getting_started/cpp_compiler_support)
chapter of our documentation.


## CI/CD Integration

Each pull request triggers a comprehensive CI pipeline:

<!-- markdownlint-disable-next-line MD013 -->
- **Multi-compiler builds**: GCC 12-16, [Clang 16-21 except Clang-19](https://mpusz.github.io/mp-units/latest/getting_started/cpp_compiler_support),
  Apple Clang, and MSVC, across C++20/23/26 and the library's configuration options
- **Code formatting**: clang-format and pre-commit checks
- **Static analysis**: clang-tidy analysis for code quality
- **Documentation**: Verification that docs build successfully
- **Package testing**: Conan package creation and consumption tests
- **Dependency security**: Automated vulnerability and license scanning

!!! note "The compiler matrix is sampled, not exhaustive"

    The full combination of toolchains, C++ versions, and library options is far too large to
    build on every push, so
    [`.github/generate-job-matrix.py`](https://github.com/mpusz/mp-units/blob/master/.github/generate-job-matrix.py)
    selects a random subset for each run (while guaranteeing coverage of the configurations
    that change the API). A green pull request therefore does not prove that *every*
    configuration builds, and a later run may uncover a problem in a combination yours never
    tried. This is expected, and not a sign that something went wrong with your change.


## For Maintainers: Creating Good First Issues

!!! note "Guidance for Repository Maintainers"

    This section provides guidance for maintainers creating curated onboarding tasks.

A "good-first-issue" label alone is insufficient. Well-crafted good first issues remove
decision paralysis and provide explicit mentorship commitment. They are **invitations**, not
just labels.

When creating a good first issue, use the **[MAINTAINERS ONLY] Good First Issue Template**
in the issue picker. This structured template ensures you include:

- **What:** Specific deliverables with file paths and clear scope
- **Why:** Rationale for the work and its impact on the project
- **Mentorship:** Explicit commitment to guide the contributor through the PR
- **Skills Learned:** Concrete concepts, APIs, or techniques they'll learn
- **Estimated Time:** Realistic time investment (1-8 hours)
- **Getting Started:** Optional step-by-step guidance
- **Acceptance Criteria:** Optional completion checklist

### Example

```markdown
## Good First Issue: Add Celsius to Fahrenheit example

**What:** Create a new example in `example/temperature_conversion.cpp` that demonstrates:
- Converting between Celsius and Fahrenheit using affine space
- Using `quantity_point` for absolute temperatures
- Using `quantity` for temperature differences

**Why:** Temperature conversion is a common use case that confuses new users.
This example will clarify the distinction between absolute and relative temperatures.

**Mentorship:** @mpusz will help you through the PR process

**Skills Learned:**
- C++20 concepts and constraints
- mp-units API design patterns
- Affine space modeling
- Writing clear example code

**Estimated Time:** 2 hours
```

### Philosophy

Lower the barrier to entry with clear guidance. A well-written good-first-issue provides:

1. **Zero ambiguity** — contributor knows exactly what to do and where
2. **Context** — understands why the work matters
3. **Safety net** — explicit promise that someone will guide them
4. **Growth** — clear learning objectives make it worthwhile

This is not just labeling issues. This is **curated mentorship at scale**.
