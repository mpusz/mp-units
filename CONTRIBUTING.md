# Contributing to `mp-units`

👍🎉 First off, thanks for taking the time to contribute! 🎉👍

## Download, Build, Install

Please refer to our official docs for [download, build, and install instructions](https://mpusz.github.io/units/usage.html).

## Where to start?

If you are looking for a good issue to start with, please check the following:
- [good first issue](https://github.com/mpusz/units/labels/good%20first%20issue) - issues that should be pretty simple to implement.
- [help wanted](https://github.com/mpusz/units/labels/help%20wanted) - issues that typically are a bit more involved than beginner issues.
- [high priority](https://github.com/mpusz/units/labels/high%20priority) - things to fix ASAP but often of higher complexity.

## Code Formatting

We intend to use `clang-format` when it finally learns what a C++ concept is :wink:.

Until then, please code alike what is there already:
- 2 spaces for indentation
- no indentation for namespaces
- types, functions, variables naming in a `standard_case`
- template parameters in a `PascalCase`

## Backward Compatibility

Before submission, please remember to check if the code compiles fine on the supported compilers.
The CI will check it anyway but it is good to check at least some of the configurations before pushing changes.
Especially older compilers can be tricky as those do not support all the C++20 features well enough. The official
list of supported compilers can be always found on our [Usage page](https://mpusz.github.io/units/usage.html).
