# Contributing to `mp-units`

👍🎉 First off, thanks for taking the time to contribute! 🎉👍

## Gitpod

The easiest way to start coding is to jump straight into [Gitpod](https://www.gitpod.io). You can either click the button
below or prefix any `mp-units` URL (main branch, other branches, issues, PRs, ...) in your web browser with `gitpod.io/#`
(e.g., <https://gitpod.io/#https://github.com/mpusz/mp-units>).

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/mpusz/mp-units)

The above environment provides you with:

- all supported compilers for Linux development and the latest version of build tools like `cmake` and `conan`
- all Conan dependencies preinstalled on the machine
- all documentation generation tools ready to use
- completed prebuilds for all targets (Debug and Release builds for each compiler)
- VSCode preconfigured to benefit from all the above

## Download, Build, Install

Alternatively, please refer to our official docs for
[download, build, and install instructions](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage)
if you want to setup a development environment on your local machine.

## Before Committing git Changes

There are a few steps recommended to check before committing and pushing your changes to the git repository.

### Naming Conventions

Here are the main rules for naming things in this repo:

- types, functions, variables naming in a `standard_case`
- template parameters in a `PascalCase`
- C++20 concepts names for now in a `PascalCase` but we plan to change it (see <https://github.com/mpusz/mp-units/issues/93>
  for more details)

### Unified Code Formatting

There is a formatting standard enforced with the `pre-commit` script. Before committing your changes please do the following:

```bash
pip3 install -U pre-commit
pre-commit run --all-files
```

This will run `clang-format` for code formatting with the `.clang-format` file provided in the repo, `cmake-format` to format
the CMake files, and some other check as well.
The script will run on all the files in the repo and will apply the changes in-place when needed.
After the script is done please make sure to stage all those changes to git commit.

### Build All CMake Targets And Run Unit Tests

The simplest way to verify if all targets build correctly and all unit tests pass is to run:

```bash
conan build . -pr <your_conan_profile> -s compiler.cppstd=23 -o cxx_modules=True -c user.build:all=True -b missing
```

as described in the
[Installation and Usage](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#contributing-or-just-building-all-the-tests-and-examples)
chapter of our documentation.

### Backward Compatibility

Before submission, please remember to check if the code compiles fine on the supported compilers.
The CI will check it anyway but it is good to check at least some of the configurations before pushing changes.
Especially older compilers can be tricky as those do not support all the C++20 features well enough. The official
list of supported compilers can be always found in the
[Installation And Usage](https://mpusz.github.io/mp-units/latest/getting_started/installation_and_usage/#cpp-compiler-support)
chapter of our documentation.


## Where To Start?

If you are looking for a good issue to start with, please check the following:

- [good first issue](https://github.com/mpusz/mp-units/labels/good%20first%20issue) - issues that should be pretty simple to implement.
- [help wanted](https://github.com/mpusz/mp-units/labels/help%20wanted) - issues that typically are a bit more involved than beginner issues.
- [high priority](https://github.com/mpusz/mp-units/labels/high%20priority) - things to fix ASAP but often of higher complexity.
