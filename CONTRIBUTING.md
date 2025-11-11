# Contributing

Thank you for your interest in contributing to **mp-units**! 👍🎉

Before contributing, we strongly recommend familiarizing yourself with the
[official documentation](https://mpusz.github.io/mp-units).

??? info "This file is a part of MkDocs documentation"

    This file is part of the documentation and uses some MkDocs‑specific formatting that may
    look unusual in a plain Markdown renderer. For the fully rendered version, visit the
    [Contributing](https://mpusz.github.io/mp-units/latest/getting_started/contributing)
    chapter.


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
- Run `.devcontainer/check_all.sh` to test across all supported compilers.
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
    conan upload -r <remote-name> --all mp-units/2.4.0@<user>/<channel>
    ```


    ### Building documentation

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


## Before committing git changes

There are a few recommended checks before committing and pushing changes.

### Naming conventions

Here are the main rules for naming things in this repo:

- types, functions, variables use `standard_case`,
- template parameters use `PascalCase`,
- C++ concepts: `PascalCase` ([subject to change](https://github.com/mpusz/mp-units/issues/93))

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
After it finishes review and stage all modified files.

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

- **Multi-compiler builds**: GCC 12-15, Clang 16-21 across different configurations
- **Code formatting**: clang-format and pre-commit checks
- **Static analysis**: clang-tidy analysis for code quality
- **Documentation**: Verification that docs build successfully
- **Package testing**: Conan package creation and consumption tests
- **Dependency security**: Automated vulnerability and license scanning
