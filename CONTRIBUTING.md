# Contributing

👍🎉 First off, thanks for taking the time to contribute! 🎉👍


## **mp-units** documentation

Before contributing, it is highly recommended to familiarize yourself with our
[official documentation](https://mpusz.github.io/mp-units).

This file is also a part of it, and this is why it has non-standard Markdown formatting (which can
be seen when reading in a regular Markdown renderer). To benefit from full mkdocs rendering, please
switch to the [Contributing](https://mpusz.github.io/mp-units/latest/getting_started/contributing)
chapter of our documentation.


## Where to start?

If you are looking for a good issue to start with, please check the following:

- [good first issue](https://github.com/mpusz/mp-units/labels/good%20first%20issue) - issues that
  should be pretty simple to implement,
- [help wanted](https://github.com/mpusz/mp-units/labels/help%20wanted) - issues that typically are
  a bit more involved than beginner issues,
- [high priority](https://github.com/mpusz/mp-units/labels/high%20priority) - things to fix ASAP
  but often of higher complexity.


## GitHub Codespaces Development Environment

The easiest way to start coding is to use our containerized development environment in
[GitHub Codespaces](https://docs.github.com/en/codespaces). You can either click the button below

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/mpusz/mp-units)

or navigate to the main page of the repository and click "Code" button -> "Codespaces" tab ->
"Create codespace on master".

You can also use the pre-configured devcontainer and Docker image manually within your IDE.


### Environment Features

The development environment is built on our [custom `trainiteu/mp-units` Docker image](https://github.com/train-it-eu/docker-images/blob/main/mp-units/Dockerfile)
and provides:

#### 🔧 **Complete Compiler Matrix**

- **GCC**: 12, 13, 14, 15
- **Clang**: 16, 17, 18, 19, 20
- **C++ versions**: C++20/23/26
- **Standard Libraries**: libstdc++11 and libc++ for thorough testing

#### 🚀 **Automated Build & Testing**

- **Multi-compiler testing script**: `.devcontainer/check_all.sh` tests recommended configurations
- **Pre-configured Conan profiles**: Ready-to-use compiler configurations
- **Incremental builds**: Persistent Conan cache across sessions
- **Fast dependencies**: Dedicated Conan Artifactory with pre-compiled packages

#### 📖 **Documentation Tools**

- **MkDocs with Material theme**: Live documentation server at port 8000
- **API reference generation**: Standardeese-like API reference built on top of LaTex
- **Python tooling**: pre-commit, mike, mkdocs-material, and plugins via pipx

#### 🛠️ **Development Tools**

- **Code quality**: clang-format, clang-tidy, include-what-you-use
- **VS Code extensions**: 20+ pre-installed extensions for C++, Python, Git


### Quick Start Commands

```bash
# Test with a single compiler configuration
conan build . -pr gcc14 -c user.mp-units.build:all=True -b missing

# Test all compiler configurations
.devcontainer/check_all.sh create

# Generating API Reference
.devcontainer/api_reference.sh

# Build documentation locally
mkdocs serve

# Run pre-commit checks
pre-commit run --all-files
```

For detailed environment documentation and troubleshooting, see _.devcontainer/README.md_.


## Building, testing, and packaging

Alternatively, please refer to our official docs for
[download, build, and install instructions](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage) with the below changes
if you want to set up a development environment on your local machine.


### Conan configuration properties

[`user.mp-units.build:all`](#user.mp-units.build-all){ #user.mp-units.build-all }

:   Enables compilation of all the source code, including tests and examples. To support this, it requires some additional Conan build dependencies described in
    [Repository directory tree and dependencies](https://mpusz.github.io/mp-units/latest/getting_started/project_structure#cmake-projects-and-dependencies).
    It also runs unit tests during the Conan build (unless
    [`tools.build:skip_test`](https://docs.conan.io/2/reference/commands/config.html?highlight=tools.build:skip_test#conan-config-list)
    configuration property is set to `True`).

    [conan build all support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`user.mp-units.build:skip_la`](#user-skip-la){ #user-skip-la }

:   If `user.mp-units.build:all` is enabled, among others, Conan installs the external
    [wg21-linear_algebra](https://conan.io/center/recipes/wg21-linear_algebra)
    dependency and enables the compilation of linear algebra-based tests and usage examples.
    Such behavior can be disabled with this option.

    [conan skip la support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`user.mp-units.analyze:clang-tidy`](#user.mp-units.analyze-clang-tidy){ #user.mp-units.analyze-clang-tidy }

:   Enables clang-tidy analysis.

    [conan clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0


### CMake options for mp-units project developers

[`MP_UNITS_DEV_IWYU`](#MP_UNITS_DEV_IWYU){ #MP_UNITS_DEV_IWYU }

:   [:octicons-tag-24: 2.2.0][cmake iwyu support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

    Enables include-what-you-use analysis.

    [cmake iwyu support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_CLANG_TIDY`](#MP_UNITS_DEV_CLANG_TIDY){ #MP_UNITS_DEV_CLANG_TIDY }

:   [:octicons-tag-24: 2.2.0][cmake clang-tidy support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

    Enables clang-tidy analysis.

    [cmake clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_TIME_TRACE`](#MP_UNITS_DEV_TIME_TRACE){ #MP_UNITS_DEV_TIME_TRACE }

:   [:octicons-tag-24: 2.5.0][cmake time-trace support] · :octicons-milestone-24: `NONE`/`ALL`/`MODULES`/`HEADERS` (Default: `NONE`)

    Enables compilation performance data collection with `-ftime-trace` for clang compilers.

    All our unit tests compile only for headers and never for modules. To allow fair
    comparison, `MODULES` and `HEADERS` do not enable the data collection for unit tests.
    This means that they affect only the core, systems, and examples.

    Please use `ALL` to profile unit tests as well.

    [cmake time-trace support]: https://github.com/mpusz/mp-units/releases/tag/v2.5.0


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
conan upload -r <remote-name> --all mp-units/2.2.0@<user>/<channel>
```


## Building documentation

We are building our documentation using [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/).
The easiest way to install all the required dependencies is with `pip`:

```shell
pip install -U mkdocs-material mkdocs-rss-plugin
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
- Use a specific location with faster disk access
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
cmake -S docs/api_reference/src -B build/docs/api_reference -DCMAKE_MODULE_PATH="/workspaces/api_reference_deps/jegp.cmake_modules/modules" -DJEGP_STANDARDESE_SOURCES_GIT_REPOSITORY="/workspaces/api_reference_deps/draft" -DJEGP_CXXDRAFT_HTMLGEN_GIT_REPOSITORY="/workspaces/api_reference_deps/cxxdraft-htmlgen"
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


## Before committing git changes

There are a few steps recommended to check before committing and pushing your changes to the git
repository.


### Naming conventions

Here are the main rules for naming things in this repo:

- types, functions, variables use `standard_case`,
- template parameters use `PascalCase`,
- C++ concept names, for now, use `PascalCase`, but we plan to change it
 (see [GitHub Issue #93](https://github.com/mpusz/mp-units/issues/93) for more details).

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
- some other checks (e.g., python script checkers, whitespaces, etc.).

The script will run on all the files in the repo and will apply the changes in place when needed.
After the script is done, please make sure to review and stage all those changes for the git commit.

### Backward compatibility

Before submission, please remember to check if the code compiles fine on the supported compilers.
The CI will check it anyway, but it is good to check at least some of the configurations before
pushing changes.
Especially older compilers can be tricky as those do not have full C++20 conformance.
The official list of supported compilers can always be found in the
[C++ compiler support (API/ABI)](https://mpusz.github.io/mp-units/latest/getting_started/cpp_compiler_support)
chapter of our documentation.

#### Multi-Compiler Testing

In GitHub Codespaces, you can use the `.devcontainer/check_all.sh` script to test your changes
across all supported compiler configurations:

```bash
# Test with all compilers (release builds only)
.devcontainer/check_all.sh create

# Include debug builds as well
.devcontainer/check_all.sh -d create

# Just build without creating packages
.devcontainer/check_all.sh build

# Install dependencies only
.devcontainer/check_all.sh install
```

This script tests a similar compiler matrix to the one used in CI, ensuring your changes
work across:

- GCC 12, 13, 14 with different C++ standards and feature combinations
- Clang 16, 17, 18, 20 with libstdc++ and libc++
- Various build configurations (modules, `import std`, `std::format`, contracts)
