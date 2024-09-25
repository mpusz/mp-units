# Contributing

👍🎉 First off, thanks for taking the time to contribute! 🎉👍


## Where to start?

If you are looking for a good issue to start with, please check the following:

- [good first issue](https://github.com/mpusz/mp-units/labels/good%20first%20issue) - issues that
 should be pretty simple to implement,
- [help wanted](https://github.com/mpusz/mp-units/labels/help%20wanted) - issues that typically are
 a bit more involved than beginner issues,
- [high priority](https://github.com/mpusz/mp-units/labels/high%20priority) - things to fix ASAP
 but often of higher complexity.


## Gitpod

The easiest way to start coding is to jump straight into [Gitpod](https://www.gitpod.io)
environment. You can either click the button below

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/mpusz/mp-units)

or prefix any `mp-units` URL (main branch, other branches, issues, PRs, ...) in your web browser
with `gitpod.io/#` (e.g., <https://gitpod.io/#https://github.com/mpusz/mp-units>).

The above environment provides you with:

- all supported compilers for Linux development and the latest version of build tools like `cmake`
  and `conan`,
- all Conan dependencies preinstalled on the machine,
- all documentation generation tools ready to use,
- completed prebuilds for all targets (Debug and Release builds for each compiler),
- VSCode preconfigured to benefit from all the above.


## Building, testing, and packaging

Alternatively, please refer to our official docs for
[download, build, and install instructions](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage) with the below changes
if you want to set up a development environment on your local machine.


### Conan configuration properties

[`user.mp-units.build:all`](#user.mp-units.build-all){ #user.mp-units.build-all }

 Enables compilation of all the source code, including tests and examples. To support this, it requires some additional Conan build dependencies described in
 [Repository directory tree and dependencies](https://mpusz.github.io/mp-units/latest/getting_started/project_structure#cmake-projects-and-dependencies).
 It also runs unit tests during the Conan build (unless
 [`tools.build:skip_test`](https://docs.conan.io/2/reference/commands/config.html?highlight=tools.build:skip_test#conan-config-list)
 configuration property is set to `True`).

 [conan build all support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`user.mp-units.build:skip_la`](#user-skip-la){ #user-skip-la }

 If `user.mp-units.build:all` is enabled, among others, Conan installs the external
 [wg21-linear_algebra](https://conan.io/center/recipes/wg21-linear_algebra)
 dependency and enables the compilation of linear algebra-based tests and usage examples.
 Such behavior can be disabled with this option.

 [conan skip la support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`user.mp-units.analyze:clang-tidy`](#user.mp-units.analyze-clang-tidy){ #user.mp-units.analyze-clang-tidy }

 Enables clang-tidy analysis.

 [conan clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0


### CMake options for mp-units project developers

[`MP_UNITS_DEV_BUILD_LA`](#MP_UNITS_DEV_BUILD_LA){ #MP_UNITS_DEV_BUILD_LA }

:   [:octicons-tag-24: 2.2.0][cmake build la support] · :octicons-milestone-24: `ON`/`OFF` (Default: `ON`)

 Enables building code depending on the linear algebra library.

 [cmake build la support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_IWYU`](#MP_UNITS_DEV_IWYU){ #MP_UNITS_DEV_IWYU }

:   [:octicons-tag-24: 2.2.0][cmake iwyu support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

 Enables include-what-you-use analysis.

 [cmake iwyu support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0

[`MP_UNITS_DEV_CLANG_TIDY`](#MP_UNITS_DEV_CLANG_TIDY){ #MP_UNITS_DEV_CLANG_TIDY }

:   [:octicons-tag-24: 2.2.0][cmake clang-tidy support] · :octicons-milestone-24: `ON`/`OFF` (Default: `OFF`)

 Enables clang-tidy analysis.

 [cmake clang-tidy support]: https://github.com/mpusz/mp-units/releases/tag/v2.2.0


### Building the entire repository

To build all the **mp-units** source code (with unit tests and examples), you should:

1. Use the _CMakeLists.txt_ from the top-level directory.
2. Run Conan with [`user.mp-units.build:all`](#user.mp-units.build-all) = `True`.

```shell
git clone https://github.com/mpusz/mp-units.git && cd units
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
npm install split mathjax-full mathjax-node-sre mathjax-node-cli
cabal update
```

On some platforms, installing `mathjax-node-cli` through `npm` does update the system's `PATH`
environment variable resulting in `tex2html` not found errors. In such cases we need to add
the `.bin` folder to the `PATH` environment variable manually:

```bash
echo "export PATH=\"~/node_modules/.bin:\$PATH\"" >> ~/.bashrc && source ~/.bashrc
```

Next, we need to clone the following git repositories:

- <https://github.com/JohelEGP/jegp.cmake_modules>
- `standardese_sources_base` branch of <https://github.com/JohelEGP/draft>
- `standardese_sources_base` branch of <https://github.com/JohelEGP/cxxdraft-htmlgen>

For example:

```bash
git clone https://github.com/JohelEGP/jegp.cmake_modules.git --depth=1
git clone https://github.com/JohelEGP/draft.git --branch=standardese_sources_base --depth=1
git clone https://github.com/JohelEGP/cxxdraft-htmlgen.git --branch=standardese_sources_base --depth=1
```

Now, we are ready to start building our API reference. First, we need to configure CMake with the
following:

```bash
cmake -S docs/api_reference/src -B build/docs/api_reference \
      -DCMAKE_MODULE_PATH="<path to gh:JohelEGP/jegp.cmake_modules>/modules" \
      -DJEGP_STANDARDESE_SOURCES_GIT_REPOSITORY="<path to gh:JohelEGP/draft>" \
      -DJEGP_CXXDRAFT_HTMLGEN_GIT_REPOSITORY="<path to gh:JohelEGP/cxxdraft-htmlgen>"
```

Then we need to build the docs with CMake:

```bash
cmake --build build/docs/api_reference
```

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
