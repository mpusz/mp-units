# Contributing

Thank you for your interest in contributing to **mp-units**! 👍🎉

Before contributing, we strongly recommend familiarizing yourself with the
[official documentation](https://mpusz.github.io/mp-units).

??? info "This file is a part of ProperDocs documentation"

    This file is part of the documentation and uses some ProperDocs‑specific formatting that may
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

# Generate the API reference
python scripts/mrdocs/run.py

# Preview documentation locally
properdocs serve

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


## Building and Testing Locally

To work on your own machine instead of in the dev container, build the project with the
_CMakeLists.txt_ in the top-level directory. Unlike _src/CMakeLists.txt_, which is meant for
library users, it also builds the examples and all the unit tests. Its dependencies can come
from Conan, from vcpkg, or from anywhere CMake's `find_package` can find them.

### Dependencies

| Library           | Needed for                                                          |
|-------------------|---------------------------------------------------------------------|
| Catch2 3          | the runtime unit tests                                              |
| gsl-lite          | `MP_UNITS_API_CONTRACTS=GSL-LITE` (the default)                     |
| Microsoft.GSL     | `MP_UNITS_API_CONTRACTS=MS-GSL`                                     |
| {fmt}             | `MP_UNITS_API_STD_FORMAT=OFF`, or when `std::format` is unavailable |
| Eigen, GLM, Blaze | the linear algebra integrations, skipped when not found             |

`MP_UNITS_API_CONTRACTS=STD` and `NONE` need no contracts library. The versions CI tests are
the ones pinned in
[_conanfile.py_](https://github.com/mpusz/mp-units/blob/master/conanfile.py).

### CMake options

Conan turns its settings and options into CMake cache variables. With vcpkg or plain CMake,
you pass them yourself with `-D`. Each Conan option below links to its description:

| Conan                                 | CMake                                                         |
|---------------------------------------|---------------------------------------------------------------|
| `compiler.cppstd` setting             | [`CMAKE_CXX_STANDARD`][v cxx standard]                        |
| [`cxx_modules`][c modules]            | [`MP_UNITS_BUILD_CXX_MODULES=ON`][m modules], [`CMAKE_CXX_SCAN_FOR_MODULES=ON`][v scan] |
| [`import_std`][c import std]          | [`CMAKE_CXX_SCAN_FOR_MODULES=ON`][v scan], [`CMAKE_CXX_MODULE_STD=ON`][cmake experimental], [`CMAKE_EXPERIMENTAL_CXX_IMPORT_STD=<UUID>`][cmake experimental] |
| [`std_format`][c std format]          | [`MP_UNITS_API_STD_FORMAT`][m std format]                     |
| [`no_crtp`][c no crtp]                | [`MP_UNITS_API_NO_CRTP`][m no crtp]                           |
| [`contracts`][c contracts]            | [`MP_UNITS_API_CONTRACTS`][m contracts]                       |
| [`freestanding`][c freestanding]      | [`MP_UNITS_API_FREESTANDING=ON`][m freestanding], [`CMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY`][v try compile] |
| [`integration_*`][c integrations]     | [`MP_UNITS_BUILD_INTEGRATION_*`][m integrations]              |
| [`user.mp-units.build:all`](#user.mp-units.build-all) | [`CMAKE_COMPILE_WARNING_AS_ERROR=ON`][v werror], [`CMAKE_EXPORT_COMPILE_COMMANDS=ON`][v compile commands], [`CMAKE_VERIFY_INTERFACE_HEADER_SETS=ON`][v verify headers] (not with `import_std`) |

[c modules]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#cxx_modules
[c import std]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#import_std
[c std format]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#std_format
[c no crtp]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#no_crtp
[c contracts]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#contracts
[c freestanding]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#freestanding
[c integrations]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#integration_eigen-integration_glm-integration_blaze
[m modules]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_BUILD_CXX_MODULES
[m std format]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_API_STD_FORMAT
[m no crtp]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_API_NO_CRTP
[m contracts]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_API_CONTRACTS
[m freestanding]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_API_FREESTANDING
[m integrations]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#MP_UNITS_BUILD_INTEGRATION_EIGEN
[v cxx standard]: https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_STANDARD.html
[v scan]: https://cmake.org/cmake/help/latest/variable/CMAKE_CXX_SCAN_FOR_MODULES.html
[v try compile]: https://cmake.org/cmake/help/latest/variable/CMAKE_TRY_COMPILE_TARGET_TYPE.html
[v werror]: https://cmake.org/cmake/help/latest/variable/CMAKE_COMPILE_WARNING_AS_ERROR.html
[v compile commands]: https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html
[v verify headers]: https://cmake.org/cmake/help/latest/variable/CMAKE_VERIFY_INTERFACE_HEADER_SETS.html
[cmake experimental]: https://github.com/Kitware/CMake/blob/master/Help/dev/experimental.rst

`<UUID>` is the value that enables `import std` in your CMake version, listed in
CMake's [experimental features][cmake experimental]; it changes between CMake releases.

The library's own variables are described in [CMake options][all cmake options], and the
options for developer tools, such as clang-tidy, in
[Configuration Options for Developers and Contributors](#cmake).

[all cmake options]: https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#cmake-options

Set `CMAKE_CXX_STANDARD` even for a default build. Without it, the library's feature checks
run in the compiler's default language mode, where MSVC reports `std::format` as
unavailable. The checks cache their results in the build directory, so add `--fresh` to the
configure command when you change the standard of an existing one.

Conan also rejects these combinations up front, while CMake lets them through to fail
during compilation:

- `import std` together with `MP_UNITS_API_CONTRACTS` other than `NONE` or `STD`, or with
  `MP_UNITS_API_STD_FORMAT=OFF`.
- `MP_UNITS_API_STD_FORMAT=ON` with MSVC in Debug builds (error C7595).

### With Conan

See
[Obtaining dependencies](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#obtaining-dependencies)
for installing Conan and creating a profile.

To build all the **mp-units** source code (with unit tests and examples), you should:

1. Use the _CMakeLists.txt_ from the top-level directory.
2. Run Conan with [`user.mp-units.build:all`](#user.mp-units.build-all) = `True`.

```shell
git clone https://github.com/mpusz/mp-units.git && cd mp-units
conan build . -pr <your_conan_profile> -s compiler.cppstd=23 -c user.mp-units.build:all=True -b missing
```

The above will download and install all of the dependencies needed for the development of
the library, build all of the source code, and run unit tests.

If you prefer to build the project via CMake rather than Conan, then you should replace
the `conan build` with `conan install` command and then follow with a regular CMake build
and testing:

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

### With vcpkg

The repository ships a [vcpkg](https://vcpkg.io) manifest (_vcpkg.json_) and a `vcpkg`
preset in _CMakePresets.json_ that uses it. You need:

- [CMake](https://cmake.org) 3.25 or newer and [Ninja](https://ninja-build.org).
- vcpkg, with the `VCPKG_ROOT` environment variable pointing at it. Visual Studio includes
  vcpkg and sets `VCPKG_ROOT` both in the IDE and in its Developer Command Prompt.
  Elsewhere, follow [vcpkg's getting started guide][vcpkg get started] to clone it and set
  `VCPKG_ROOT`.
- On Linux, vcpkg also needs `curl`, `zip`, `unzip`, `tar`, and `pkg-config`.

[vcpkg get started]: https://learn.microsoft.com/vcpkg/get_started/get-started

From the command line (on Windows, from a Developer Command Prompt or Developer PowerShell,
so that CMake finds MSVC):

```shell
cmake --preset vcpkg
cmake --build --preset vcpkg-debug
ctest --preset vcpkg-debug
```

The first configure installs the dependencies into the build directory (_out/build/vcpkg_),
which takes a few minutes. Use `vcpkg-release` instead of `vcpkg-debug` for a release build.

vcpkg always installs Catch2, gsl-lite, and {fmt}. The other libraries from the
[table above](#dependencies) are optional _features_ of _vcpkg.json_, installed only when
you turn them on with `VCPKG_MANIFEST_FEATURES`: `ms-gsl` for Microsoft.GSL, and `eigen`,
`glm`, and `blaze` for the integrations.

In Visual Studio:

1. Choose **File → Open → Folder** and open the repository root.
2. Select **vcpkg Debug** or **vcpkg Release** in the configuration list on the toolbar.
   Visual Studio configures the project and installs the dependencies.
3. Choose **Build → Build All**.
4. Run the tests from **Test → Test Explorer**.

The preset sets `CMAKE_CXX_STANDARD=20` and `MP_UNITS_API_STD_FORMAT=OFF`, the second one
because the Ninja Multi-Config generator chooses Debug or Release only at build time. It
also sets `CMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST`, so that the build never runs
the test programs (see [Troubleshooting](#troubleshooting)). To change the configuration,
pass `-D` options after the preset, and turn on the vcpkg features that the new
configuration needs:

```shell
cmake --preset vcpkg --fresh -DCMAKE_CXX_STANDARD=23 -DVCPKG_MANIFEST_FEATURES="eigen;glm;blaze"
```

To keep such a configuration in Visual Studio, add a preset that inherits from `vcpkg` to
_CMakeUserPresets.json_ (ignored by git) next to _CMakePresets.json_:

```json title="CMakeUserPresets.json"
{
  "version": 6,
  "configurePresets": [
    {
      "name": "vcpkg-cxx23",
      "inherits": "vcpkg",
      "cacheVariables": {
        "CMAKE_CXX_STANDARD": "23",
        "VCPKG_MANIFEST_FEATURES": "eigen;glm;blaze"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "vcpkg-cxx23-debug",
      "displayName": "vcpkg C++23 Debug",
      "configurePreset": "vcpkg-cxx23",
      "configuration": "Debug"
    }
  ],
  "testPresets": [
    {
      "name": "vcpkg-cxx23-debug",
      "configurePreset": "vcpkg-cxx23",
      "configuration": "Debug"
    }
  ]
}
```

`conan install` leaves a _CMakeUserPresets.json_ it did not generate untouched, so its own
presets then stop showing up. To use both, add the presets file that Conan reports as
`CMakeToolchain generated:` to the `include` list of your file, for example
`"include": ["build/msvc-195-23/generators/CMakePresets.json"]`.

### With plain CMake

Install the libraries from the [table above](#dependencies), for example with your system's
package manager, or build each from source and install it into a common prefix. When you
build them yourself:

- Build Catch2 with the same `CMAKE_CXX_STANDARD` as mp-units. Catch2 compiles its
  `std::string_view` support only in C++17 and newer, so a Catch2 built in the compiler's
  older default mode fails to link with the tests.
- Install both Debug and Release builds, so that each configuration of mp-units links
  against a matching one. With MSVC, the two use different runtime libraries and cannot be
  mixed. Catch2 and {fmt} name their Debug libraries differently, so both fit into one
  prefix.

```shell
git clone --depth 1 --branch v3.11.0 https://github.com/catchorg/Catch2.git
cmake -S Catch2 -B Catch2/build -G "Ninja Multi-Config" -DCMAKE_INSTALL_PREFIX=<prefix> -DCMAKE_CXX_STANDARD=20 -DBUILD_TESTING=OFF
cmake --build Catch2/build --config Debug
cmake --install Catch2/build --config Debug
cmake --build Catch2/build --config Release
cmake --install Catch2/build --config Release
```

Build and install gsl-lite and {fmt} the same way.

Then point `CMAKE_PREFIX_PATH` at the prefix and set the [CMake options](#cmake-options):

```shell
cmake -S . -B build -G "Ninja Multi-Config" -DCMAKE_PREFIX_PATH=<prefix> -DCMAKE_CXX_STANDARD=20 -DMP_UNITS_API_STD_FORMAT=OFF
cmake --build build --config Debug
ctest --test-dir build -C Debug
```

### Troubleshooting

- **The vcpkg presets are missing from the list.** `VCPKG_ROOT` is not set in the
  environment that CMake or the IDE runs in.
- **On Linux, vcpkg fails to build Catch2 with "Could not find pkg-config".** Install the
  vcpkg prerequisites listed above.
- **On Windows, Catch2 reports "Error listing tests from executable".** A policy such as
  Smart App Control blocks unsigned programs built locally, so Catch2 cannot ask the test
  programs for their tests. The `vcpkg` preset does that only when the tests run, so its
  build succeeds and only `ctest` reports the error. In other builds, Catch2 does it right
  after linking and stops the build, unless you configure with
  `-DCMAKE_CATCH_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST`. The runtime tests cannot run
  while the policy blocks them.
- **Linking `unit_tests_runtime` fails on `Catch::StringMaker<std::string_view>`.**
  Catch2 was built in an older language mode than mp-units. Rebuild it with the same
  `CMAKE_CXX_STANDARD`.
- **With C++ modules and a Clang installed from Debian or Ubuntu packages, configuration
  fails with "`std::format` enabled but not supported".** The feature checks failed with
  `'version' file not found`, because `clang-scan-deps` looks for libc++ relative to the
  compiler path it is given, and _/usr/bin/clang++-&lt;N&gt;_ is only a symbolic link. Point
  `CMAKE_CXX_COMPILER` at the real compiler, _/usr/lib/llvm-&lt;N&gt;/bin/clang++_.


## Packaging

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


## Building the Documentation

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
    properdocs serve
    ```

- [build the documentation](https://squidfunk.github.io/mkdocs-material/creating-your-site/#building-your-site)

    ```shell
    properdocs build
    ```

### Generating the API reference

The API reference is generated from the headers with
[MrDocs](https://www.mrdocs.com), so it cannot drift from the code:

```shell
python scripts/mrdocs/run.py
```

That is the whole of it. The script downloads the pinned MrDocs release on
first use and caches it, so nothing has to be installed beforehand, and it
runs the same way from a Unix shell, PowerShell, cmd.exe or Git Bash. The
generated pages land in _docs/reference/api_reference/mrdocs/_ and are not
committed.

Running it by hand is optional: `properdocs serve` and `properdocs build` call it
themselves. Either way it regenerates only when something it depends on
changed, which takes about a minute for the whole library, and otherwise
costs a hash of the inputs. Pass `--force` to regenerate regardless.

The synopses are laid out with clang-format, using the project's own
_.clang-format_ so that a declaration breaks where it would in a header.
The version is pinned in _requirements.txt_ to match the clang-format
hook in _.pre-commit-config.yaml_. Keep the two in step. Without
clang-format on `PATH` the pages are still generated, with the layout
MrDocs produced.

Two environment variables are worth knowing:

- `MRDOCS` points at a MrDocs binary of your own, instead of the pinned one.
- `MRDOCS_CACHE_DIR` moves the download cache, which otherwise follows the
  platform convention (`~/.cache` on Linux, `~/Library/Caches` on macOS,
  `%LOCALAPPDATA%` on Windows).

The version is pinned in _scripts/mrdocs/run.py_, along with a checksum per
platform. Page names and section headings move between MrDocs releases, so
upgrading means bumping the version, regenerating, and reading the diff.

Source links in the generated pages point at the exact commit they were
generated from, so a release's documentation links to that release's code.
The script warns when _src/_ has uncommitted changes, because the line
numbers then will not match whatever the links point at.


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

### Documentation structure

The documentation follows [Diátaxis](https://diataxis.fr), and the User's Guide
(_docs/users_guide/_) is its explanation part. A User's Guide chapter opens with a problem
the reader has, builds up the concepts that solve it, and says why the library is designed
that way. It also teaches the domain (ISO 80000, the ISQ, the metrology vocabulary),
because most C++ developers have never met it. Define each term before its first use.

Rejected design alternatives and "why is X missing?" questions do not belong in the User's
Guide. They go in the [FAQ](https://mpusz.github.io/mp-units/latest/getting_started/faq),
and the chapter links to the FAQ entry where the question arises, as _the_affine_space.md_
does for the quantity point constructor.

### Documentation style

The prose in _docs/_ and the doc comments the API reference is built from follow a few
conventions beyond the formatting `pre-commit` enforces:

- **Quantity names are italicized; the entities that model them are not.** Prose about the
  quantities themselves writes _length_, _width_, _energy_, and _torque_. Prose about the
  library entities writes `isq::length` and `isq::energy`, in code font like any other
  identifier. An ordinary English use of the same word stays plain: "the length of the buffer"
  is not a reference to the quantity.
- **Project names are prose, not code.** mp-units, Conan, and Compiler Explorer are written
  plainly. Backticks are for things you type into a source file, so `mp_units` the namespace
  is code and mp-units the project is not.

Doc comments are read by [MrDocs](https://www.mrdocs.com), which accepts Javadoc-style
commands. A few of its behaviors are worth knowing, because the obvious habits from Doxygen
produce worse output here:

- **The first paragraph is the brief, so `@brief` is redundant.** Write the summary sentence
  and leave the command out. Note that `@brief` consumes the whole paragraph rather than
  the line, so a sentence beneath it silently joins the summary.
- **Use `@ref` to point at another symbol.** A backticked name renders as code and is not
  a link. `@ref` renders as code *and* links, so nothing is lost, and an unresolvable name
  is reported, which makes these checkable.
- **Deprecation comes from the attribute.** A `[[deprecated]]` attribute already produces
  the message, the badge, and an admonition, so `@deprecated` only adds a second copy that
  can drift out of step with the one the compiler enforces.
- **Document a parameter only where the signature does not.** Restating a name and its type
  adds nothing. Say what a parameter means when that cannot be read off the declaration, and
  say nothing when it can.
- **Code examples have to compile.** An example that was never built goes stale silently,
  and a wrong example in the reference is worse than none. See
  [Code samples in the documentation](#code-samples-in-the-documentation).

### Code samples in the documentation

Every ` ```cpp ` fence under _docs/_ and every `@code` block in a header is compiled by
_scripts/check_doc_snippets.py_. The pre-commit hook runs it on every sample whenever a
commit changes a page, a header, or an example, because a header change can break a
sample on any page. Each result is cached with the files the compile read, so only the
samples whose inputs changed are compiled again, and every clone and worktree on a
machine shares that cache. CI runs it on all of them before the documentation is
published. It needs GCC 15, which CI pins because the check parses its diagnostics: the
line it blames for a marked line, and the wording of the message. The dev container has
it. Without it, the hook only warns, and CI still checks the samples. To run it yourself:

```bash
python3 scripts/check_doc_snippets.py docs/users_guide/framework_basics/the_affine_space.md
python3 scripts/check_doc_snippets.py      # every sample
```

The check wraps each sample in a template from _scripts/snippet_templates/_, which
supplies the includes and the usual using-directives. An unmarked sample tries the `si`
template, then `isq`. The sample goes into the body of `main`, at namespace scope, or is
cut in two, with the declarations above the cut at namespace scope and the statements
below it in `main`. A sample needing more says so with a marker line, which the site build
removes from the published page:

| Marker                                   | Use it when the sample                                                                |
|------------------------------------------|---------------------------------------------------------------------------------------|
| `// snippet-template: systems`           | needs a template other than `si` and `isq`. The first lines of each template say what it is for |
| `// snippet-continues`                   | uses what the previous sample on the page declared                                    |
| `// snippet-id: defs`                    | is used by another sample, which names it with `snippet-after`                       |
| `// snippet-after: defs`                 | uses what another sample declared, above or below it on the page. `docs/path/page.md#defs` names one on another page or in a header, and `example/file.cpp` a whole example program. Add `:decls` to take only its declarations, without its statements in `main` |
| `// snippet-setup: void draw_at(int x, int y);` | calls or uses something the page leaves to the reader, like a function of their own API. The line is compiled and not shown, so it may only declare. `--list-setup` prints them after the check |
| `// snippet-std: 20`                     | is written for another C++ standard than C++23, like a CRTP definition, which the library accepts only without deducing `this` |
| `// snippet-template: none (category: why)` | can't compile. The reason starts with one of the categories below                  |

A sample can show code that must not compile. Mark it with `❌` at the start of a comment,
and nothing else counts:

- A comment at the end of a line marks that line, as in
  `// auto q = a + b;  // ❌ different kinds`. The line may be commented out, so that a
  reader who copies the sample can still compile it.
- A comment line with no code on it, such as `// ❌ different kinds`, marks the statement
  under it.

A statement that spans several lines counts as one. Use `❌` for nothing else in a sample.
Code that compiles but teaches a mistake, or fails at runtime, says so in words.

The check compiles the sample without those lines first. Then it adds each of them back
on its own, uncommented. The compiler has to report an error on that line, or a
`required from here` pointing at it. The first such error must not be about an undeclared
name, because a misspelled name would make any line fail. A compile that reports a
deprecation fails too, because a sample must not teach a deprecated spelling.

Opting out is for a sample that can't compile by its nature. The reason goes on the marker
line and starts with one of these categories, followed by a colon:

- `historical`: an earlier version's API or design, shown as such;
- `not this library`: another library, a proposal, or a planned design;
- `third-party`: needs a library the check doesn't have, like Eigen;
- `pseudo-code`: a synopsis, `...` placeholders, or a name standing for the reader's type;
- `exercise skeleton`: a workshop exercise the reader completes (its solution compiles).

`--list-opt-outs` prints them all. A sample that is only hard to make compile isn't one of
these. Fix it instead.

The templates a marker can name:

- `si` (the default) and `isq`: the core library, the ISQ and the SI, with the SI unit
  symbols, and `isq` also with the ISQ quantity names;
- `plain`: the same library, with no unit symbols;
- `scoped`: `isq` with the sample in a namespace of its own, for a sample that defines its
  own `length` or `time`;
- `utility`: the `mp_units::utility` representation types (vectors, tensors, `uncertain`,
  the random distributions);
- `codata`: the measured constants;
- `systems` and `systems_plain`: the other systems, with or without the SI unit symbols;
- `hep` and `angular`: the HEP and angular systems with their own unit symbols;
- `excerpt`, `si_excerpt`, `isq_excerpt`, `natural_excerpt`, `hep_excerpt`: for a sample
  that quotes the library's own definitions, placed where the names resolve as they do in
  the library.

When you write or change a sample:

- Show what a reader needs to compile it. A sample that uses another system's unit
  symbols shows its `using namespace mp_units::yard_pound::unit_symbols;`. A name the
  sample invents is declared in it, or in a sample it continues or comes after. Only what
  the page leaves to the reader, like a stand-in for their own API or an input value,
  goes in a hidden `snippet-setup` declaration.
- Never change what a sample teaches to make it compile. A sample that fails often shows
  that the documentation fell behind the library. Fix the documentation.
- Prefer the commented-out form for code that must not compile, with a short reason after
  the cross: `// auto q = a + b;  // ❌ no common kind`. A longer reason goes on a marked
  comment line above.
- Align the trailing comments of a block to one column.
- The hook runs the check on commit. Run it earlier to see failures while you edit.

### Unified code formatting

A formatting standard is enforced with the `pre-commit` script. Before committing your changes,
please do the following:

```bash
pip install -U pre-commit
pre-commit run --all-files
```

This will run:

- `clang-format` for code formatting with the `.clang-format` file provided in the repo,
- `gersemi` to format the CMake files,
- `markdownlint-cli2` for the Markdown documentation (`MD013` caps the line length, so prose
  has to be wrapped),
- `shellcheck` for the shell scripts and `actionlint` for the GitHub Actions workflows,
- `black`, `isort`, and `flake8` for the Python scripts,
- the [check of the documentation samples](#code-samples-in-the-documentation), which needs
  GCC 15 (`SKIP=doc-snippets pre-commit run --all-files` leaves it out),
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

### Commit messages

Commit messages follow [Conventional Commits](https://www.conventionalcommits.org). The
subject line has the form `type(scope): summary`, and the Conventional Commits extension in
the dev container helps to compose it.

The `type` says what kind of change it is:

- `feat` adds a feature,
- `fix` fixes a bug,
- `perf` makes the library faster, or cheaper to compile,
- `refactor` restructures code without changing its behavior,
- `docs` changes only the documentation,
- `test` changes only the tests,
- `build` changes the build, the packaging, or the development environment,
- `ci` changes the CI workflows,
- `style` reformats, or changes the formatting tools,
- `chore` is any other maintenance.

The `scope` is optional and names the part of the repository that changed, for example
`core`, `systems`, `utility`, `example`, `docs`, or `devcontainer`. The summary starts
with a lowercase letter, unless its first word is a name such as `MSVC`, and has no
trailing period.

A `!` after the scope, as in `feat(core)!: ...`, marks a breaking change, and the body then
explains what breaks and how to migrate. The body also says why the change was needed. If
the commit resolves an issue, put `Resolves #123` in the body, and GitHub closes the issue
when the commit reaches `master`.


## CI/CD Integration

Each pull request triggers a comprehensive CI pipeline:

<!-- markdownlint-disable-next-line MD013 -->
- **Multi-compiler builds**: GCC 12-16, [Clang 16-18, 20, 21, and 23](https://mpusz.github.io/mp-units/latest/getting_started/cpp_compiler_support),
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
