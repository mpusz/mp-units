# `mp-units` - Documentation

## How to build?

1. Install the requirements (Sphinx) with:

    ```shell
    pip3 install -r docs/requirements.txt
    ```

2. Install all dependencies with Conan for a developer's build:

    ```shell
    conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -e CONAN_RUN_TESTS=True -b outdated
    ```

3. Install Python 3
4. Build the documentation with a regular CMake build


## How to contribute?

To make any contribution to **mp-units** documentation please fork this repository and open
a Pull Request.

### Style Guidelines

This guidelines are just general good practices for the formatting and structure of the whole
documentation and do not pretend to be a stopper for any helpful contribution. Any contribution
that may include relevant information for **mp-units** users will always be welcomed.

**mp-units** documentation is written in [reStructuredText](http://docutils.sourceforge.net/rst.html)
and follows [reStructuredText Markup Specification](http://docutils.sourceforge.net/docs/ref/rst/restructuredtext.html).

[Quick reStructuredText](http://docutils.sourceforge.net/docs/user/rst/quickref.html) is also
used for reference.

Any detail not covered by this guidelines will follow the aforementioned rules.

#### Section titles

Use section titles in this level of importance:

```rst
Section Title
=============

Subsection Title
----------------

Subsubsection Title
^^^^^^^^^^^^^^^^^^^
```

#### Text emphasis/highlighting

- **Bold text** to highlight important text:

  ```rst
  **mp-units** is a compile-time enabled Modern C++ library that provides compile-time dimensional
  analysis and unit/quantity manipulation.
  ```

- *Italics* to refer to file names, directory names, and paths.

  ```rst
  Create Conan configuration file (either *conanfile.txt* or *conanfile.py*) in your project's
  top-level directory...
  ```

- ``Inline literals`` to refer to the in examples that is not a part of the **mp-units** library:

  ```rst
  Let's assume that the user wants to implement an ``avg_speed`` function that will
  be calculating the average speed based on provided distance and duration quantities.
  ```

#### Literal blocks

Most of the C++ code examples should be provided as literal blocks after double `::` symbol:

```rst
For this dimension-specific concepts come handy again and with usage of C++20 generic
functions our function can look as simple as::

    constexpr Velocity auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }
```

#### code-blocks

Use code-blocks for exceptional cases like code samples in other languages or a need
to emphasize specific lines of code:

```rst
Quantities of the same dimension can be easily added or subtracted with
each other and the result will always be a quantity of the same dimension:

.. code-block::
    :emphasize-lines: 3-4

    Length auto dist1 = 2q_m;
    Length auto dist2 = 1q_m;
    Length auto res1 = dist1 + dist2;
    Length auto res2 = dist1 - dist2;
```

#### Indentation and line length

Make sure all indentation is done with spaces. Normally 2 space indentation for bulleted lists
and 4 space indentation for code blocks and RST directives contents:

Do not leave any unnecessary or trailing spaces.
