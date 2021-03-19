Usage
=====

.. note::

  This library targets C++23/26 and extensively uses C++20 features. This is why it requires the latest C++
  compilers. The following compilers (or newer) are supported:

  - gcc-10
  - clang-12
  - Visual Studio 16.9

Repository Structure and Dependencies
-------------------------------------

This repository contains three independent CMake-based projects:

- *./src*

  - header-only project containing whole **mp-units** library
  - when this library will become part of the C++ standard it will have no external dependencies
    but until then it depends on:

    - `{fmt} <https://github.com/fmtlib/fmt>`_ to provide text formatting of quantities.
    - `gsl-lite <https://github.com/gsl-lite/gsl-lite>`_ to verify runtime contracts with the ``gsl_Expects`` macro.
    - [only for clang-12 with libc++] `range-v3 <https://github.com/ericniebler/range-v3>`_ to provide needed C++20 concepts and utilities.

- *.*

  - project used as an entry point for library development and CI/CD
  - it wraps *./src* project together with usage examples and tests
  - additionally to the dependencies of *./src* project, it uses:

    - `Catch2 <https://github.com/catchorg/Catch2>`_ library as a unit tests framework.
    - `linear algebra <https://github.com/BobSteagall/wg21/tree/master/linear_algebra/code>`_
      library based on proposal `P1385 <https://wg21.link/P1385>`_ used in some examples
      and tests.
    - `Doxygen <http://www.doxygen.nl>`_ to extract C++ entities information from the source
      code.
    - `Sphinx <https://www.sphinx-doc.org>`_ to build the documentation.
    - `Sphinx recommonmark <https://recommonmark.readthedocs.io>`_.
    - `Breathe <https://breathe.readthedocs.io/>`_ as a bridge between the Sphinx and Doxygen
      documentation systems.

- *./test_package*

  - library installation and Conan package verification.


Obtaining Dependencies
----------------------

This library assumes that most of the dependencies will be provided by the
`Conan Package Manager <https://conan.io/>`_. In case you would like to obtain required
dependencies by other means some modifications to library's CMake files might be needed.
The rest of the dependencies responsible for documentation generation are provided by
:command:`python3-pip`.

.. seealso::

  A full list of dependencies can be found in `Repository Structure and Dependencies`_.

Conan Quick Intro
^^^^^^^^^^^^^^^^^

In case you are not familiar with Conan, to install it (or upgrade) just do:

.. code-block:: shell

    pip3 install -U conan

After that you might need to add a custom profile file for your development environment
in *~/.conan/profiles* directory. An example profile can look as follows:

.. code-block:: ini
    :emphasize-lines: 8

    [settings]
    os=Linux
    os_build=Linux
    arch=x86_64
    arch_build=x86_64
    compiler=gcc
    compiler.version=10
    compiler.cppstd=20
    compiler.libcxx=libstdc++11
    build_type=Release

    [options]
    [build_requires]

    [env]
    CC=/usr/bin/gcc-10
    CXX=/usr/bin/g++-10
    CONAN_CMAKE_GENERATOR=Ninja

.. tip::

    Please note that **mp-units** library requires C++20 to be set either in a Conan profile or forced
    via Conan command line. If you do the former, you will not need to provide ``-s compiler.cppstd=20``
    every time your run a Conan command line (as it is suggested below).


Build Options
-------------

Environment Variables
^^^^^^^^^^^^^^^^^^^^^

CONAN_RUN_TESTS
+++++++++++++++

**Values**: ``True``/``False``

**Defaulted to**: ``False``

Enables compilation of all the source code (tests and examples) and building the documentation.
To support this it requires some additional Conan build dependencies described in
`Repository Structure and Dependencies`_.
It also runs unit tests during Conan build.


Conan Options
^^^^^^^^^^^^^

downcast_mode
+++++++++++++

**Values**: ``off``/``on``/``auto``

**Defaulted to**: ``on``

Specifies how :ref:`design/downcasting:The Downcasting Facility` works:

- ``off`` - no downcasting at all
- ``on`` - downcasting always forced -> compile-time errors in case of duplicated definitions
- ``automatic`` - downcasting automatically enabled if no collisions are present

build_docs
++++++++++

**Values**: ``True``/``False``

**Defaulted to**: ``True``

If enabled, Conan installs the documentation generation dependencies (i.e. doxygen).
Additionally, enables project documentation generation when the project is being built by Conan.

CMake Options
^^^^^^^^^^^^^

UNITS_DOWNCAST_MODE
+++++++++++++++++++

**Values**: ``OFF``/``ON``/``AUTO``

**Defaulted to**: ``ON``

Equivalent to `downcast_mode`_.


UNITS_BUILD_DOCS
++++++++++++++++

**Values**: ``ON``/``OFF``

**Defaulted to**: ``ON``

Enables project documentation generation.


Installation and Reuse
----------------------

There are many different ways of installing/reusing **mp-units** in your project. Below we mention
only a few of many options possible.

Copy
^^^^

As **mp-units** is a C++ header-only library you can simply copy *src/include* directory to
your source tree and use it as regular header files.

.. important::

    In such a case you are on your own to make sure all the dependencies are installed and their header
    files can be located during the build. Please also note that some compiler-specific flags are needed
    to make the code compile without issues.


Copy + CMake
^^^^^^^^^^^^

In case you copy the whole **mp-units** repository to your project's file tree you can reuse CMake targets
defined by the library. To do so you should use *CMakeLists.txt* file from the *./src* directory:

.. code-block:: cmake

    add_subdirectory(<path_to_units_folder>/src)
    # ...
    target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> mp-units::mp-units)

.. important::

    You are still on your own to make sure all the dependencies are installed and their header and CMake
    configuration files can be located during the build.


Conan + CMake (release)
^^^^^^^^^^^^^^^^^^^^^^^

.. tip::

    If you are new to Conan package manager it is highly recommended to read `Obtaining Dependencies`_
    and refer to `Getting Started <https://docs.conan.io/en/latest/getting_started.html>`_ and
    `Using packages <https://docs.conan.io/en/latest/using_packages.html>`_ chapters
    of the official Conan documentation for more information.

**mp-units** releases are hosted on `Conan-Center <https://conan.io/center/>`_. To obtain official
library release the following steps may be performed:

1. Create Conan configuration file (either *conanfile.txt* or *conanfile.py*) in your
   project's top-level directory and add **mp-units** as a dependency of your project.
   For example the simplest file may look as follows:

  .. code-block:: ini
      :caption: conanfile.txt

      [requires]
      mp-units/0.6.0

      [generators]
      CMakeToolchain
      CMakeDeps

2. Import **mp-units** and its dependencies definitions to your project's build procedure
   with ``find_package``:

  .. code-block:: cmake

      find_package(mp-units CONFIG REQUIRED)

3. Link your CMake targets with **mp-units**:

  .. code-block:: cmake

      target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> mp-units::mp-units)
      target_compile_features(<your_target> <PUBLIC|PRIVATE|INTERFACE> cxx_std_20)

  .. important::

    Unfortunately, packages distributed via Conan-Center cannot force the minimum version
    of the C++ language used for your build process. This is why it is important to specify
    it in `Conan profile file <Conan Quick Intro>`_ and with ``target_compile_features`` command
    for each CMake target directly linking with ``mp-units::mp-units`` in your project.

4. Download, build, and install Conan dependencies before running CMake configuration step:

  .. code-block:: shell

      mkdir build && cd build
      conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
      cmake --build .


Conan + CMake (Live At Head)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This chapter describes the procedure to Live At Head which means to use the latest version
of **mp-units** all the time.

.. note::

  Please note that even though the Conan packages that you will be using are generated **ONLY**
  for builds that are considered stable (passed our CI tests) some minor regressions may happen
  (our CI and C++20 build environment is not perfect yet). Also, please expect that the library
  interface might, and probably will, change from time to time. Even though we do our best, such
  changes might not be reflected in the project's documentation right away.

The procedure is similar to the one described in `Conan + CMake (release)`_ with the following
differences:

1. Before starting the previous procedure add **mp-units** remote to your Conan configuration:

  .. code-block:: shell

      conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz

2. In your Conan configuration file provide package identifier of the ``mpusz/testing`` stream:

  .. code-block:: ini
      :caption: conanfile.txt

      [requires]
      mp-units/0.7.0@mpusz/testing

      [generators]
      CMakeToolchain
      CMakeDeps

  .. tip::

    The identifiers of the latest packages can always be found in
    `the project's README file <https://github.com/mpusz/units/blob/master/README.md>`_ or on
    `the project's Bintray <https://bintray.com/mpusz/conan-mpusz/mp-units%3Ampusz>`_.

3. Force Conan to check for updated recipes ``-u`` and to build outdated packages ``-b outdated``:

  .. code-block:: shell

      mkdir build && cd build
      conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=outdated -u
      cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
      cmake --build .


Contributing (or just building all the tests, examples, and documentation)
--------------------------------------------------------------------------

In case you would like to build all the source code (with unit tests and examples) and documentation
in **mp-units** repository, you should:

1. Add remotes of additional Conan dependencies.
2. Use the *CMakeLists.txt* from the top-level directory.
3. Obtain Python dependencies.
4. Run Conan with `CONAN_RUN_TESTS`_ = ``True``.

.. code-block:: shell

    conan remote add linear-algebra https://api.bintray.com/conan/twonington/public-conan
    git clone https://github.com/mpusz/units.git && cd units
    pip3 install -r docs/requirements.txt
    mkdir build && cd build
    conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -e mp-units:CONAN_RUN_TESTS=True -b outdated -u
    conan build ..

The above will download and install all of the dependencies needed for the development of the library,
build all of the source code and documentation, and run unit tests.

If you prefer to build the project via CMake rather then Conan, then you should replace the last ``conan build ..``
step with the CMake build:

.. code-block:: shell

    # ...
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
    cmake --build .
    ctest


Packaging
---------

To test CMake installation and Conan packaging or create a Conan package run:

.. code-block:: shell

    conan create . <username>/<channel> -pr <your_conan_profile> -s compiler.cppstd=20 -e mp-units:CONAN_RUN_TESTS=True -b outdated -u

The above will create a Conan package and run tests provided in *./test_package* directory.


Uploading **mp-units** Package to the Conan Server
--------------------------------------------------

.. code-block:: shell

    conan upload -r <remote-name> --all mp-units/0.7.0@<user>/<channel>
