Installation And Usage
======================

.. note::

  This library targets C++23/26 and extensively uses C++20 features. This is why it requires the latest C++
  compilers. The following compilers (or newer) are supported:

  - gcc-10
  - clang-12
  - Visual Studio 16.9
  - Apple clang 13

Repository Structure and Dependencies
-------------------------------------

This repository contains three independent CMake-based projects:

- *./src*

  - header-only project containing whole **mp-units** library
  - when this library will become part of the C++ standard it will have no external dependencies
    but until then it depends on:

    - `gsl-lite <https://github.com/gsl-lite/gsl-lite>`_ to verify runtime contracts with the ``gsl_Expects`` macro.
    - [for compilers other than VS2022] `{fmt} <https://github.com/fmtlib/fmt>`_ to provide text formatting of quantities.
    - [only for clang < 14 with libc++] `range-v3 <https://github.com/ericniebler/range-v3>`_ to provide needed C++20 concepts and utilities.

- *.*

  - project used as an entry point for library development and CI/CD
  - it wraps *./src* project together with usage examples and tests
  - additionally to the dependencies of *./src* project, it uses:

    - `Catch2 <https://github.com/catchorg/Catch2>`_ library as a unit tests framework.
    - `linear algebra <https://github.com/BobSteagall/wg21/tree/master/include>`_
      library based on proposal `P1385 <https://wg21.link/P1385>`_ used in some examples
      and tests.

  - in case you also want to build the project's documentation you will need:

    - `Doxygen <http://www.doxygen.nl>`_ to extract C++ entities information from the source
      code.
    - `Sphinx <https://www.sphinx-doc.org>`_ to build the documentation.
    - `Sphinx ReadTheDocs Theme <https://sphinx-rtd-theme.readthedocs.io/>`_
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
    compiler.version=12
    compiler.cppstd=20
    compiler.libcxx=libstdc++11
    build_type=Release

    [env]
    CC=/usr/bin/gcc-12
    CXX=/usr/bin/g++-12

.. tip::

    Please note that **mp-units** library requires C++20 to be set either in a Conan profile or forced
    via Conan command line. If you do the former, you will not need to provide ``-s compiler.cppstd=20``
    every time your run a Conan command line (as provided in the command line instructions below).

Additionally, it is recommended to set Ninja as a CMake generator for Conan. To do so you should create
a *~/.conan/global.conf* file that will set ``tools.cmake.cmaketoolchain:generator`` to one of Ninja
generators. For example:

.. code-block:: text

    tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"

.. note::

  *~/.conan/global.conf* file may also set ``tools.cmake.cmake_layout:build_folder_vars``` which
  `makes working with several compilers or build configurations easier
  <https://docs.conan.io/en/latest/reference/conanfile/tools/cmake/cmake_layout.html#multi-setting-option-cmake-layout>`_.
  For example the below line will force Conan to generate separate CMake presets and folders for each compiler:

  .. code-block:: text

      tools.cmake.cmake_layout:build_folder_vars=["settings.compiler", "settings.compiler.version"]

  In such a case you will need to use a configuration specific preset name in the Conan instructions provided below
  rather then just ``default`` and ``release`` (i.e. ``gcc-11`` and ``gcc-11-release``)


Build Options
-------------

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

Conan Configuration Properties
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

build_all
+++++++++

**Values**: ``True``/``False``

**Defaulted to**: ``False``

Enables compilation of all the source code (tests and examples) and generating the documentation.
To support this it requires some additional Conan build dependencies described in
`Repository Structure and Dependencies`_.
It also runs unit tests during Conan build (unless ``tools.build:skip_test`` configuration property is set to ``True``)

skip_docs
+++++++++

**Values**: ``True``/``False``

**Defaulted to**: ``False``

If `build_all`_ is enabled, among others, Conan installs the documentation generation dependencies (i.e. doxygen) and
turns on the project documentation generation. Such behavior can be disabled with this option.

CMake Options
^^^^^^^^^^^^^

UNITS_AS_SYSTEM_HEADERS
+++++++++++++++++++++++

**Values**: ``ON``/``OFF``

**Defaulted to**: ``OFF``

Exports library as system headers.


UNITS_BUILD_DOCS
++++++++++++++++

**Values**: ``ON``/``OFF``

**Defaulted to**: ``ON``

Enables project documentation generation.


UNITS_DOWNCAST_MODE
+++++++++++++++++++

**Values**: ``OFF``/``ON``/``AUTO``

**Defaulted to**: ``ON``

Equivalent to `downcast_mode`_.


UNITS_IWYU
++++++++++

**Values**: ``ON``/``OFF``

**Defaulted to**: ``OFF``

Enables include-what-you-use when compiling with a clang compiler.
Additionally turns on `UNITS_AS_SYSTEM_HEADERS`_.


UNITS_USE_LIBFMT
++++++++++++++++

**Values**: ``ON``/``OFF``

**Defaulted to**: ``ON``

Enables usage of `{fmt} <https://github.com/fmtlib/fmt>`_ library instead of the C++20 Standard Library feature.


CMake with Presets Support
--------------------------

It is recommended to use at least CMake 3.23 to build this project as this version introduced a support
for CMake Presets schema version 4 used now by Conan to generate presets files. All build instructions
below assume that you have such a support. If not, your CMake invocations have to be replaced to something
like:

.. code-block:: shell

    mkdir build && cd build
    cmake .. -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=<path_to_generators_dir>/conan_toolchain.cmake
    cmake --build . --config Release

.. note::

  In case you can't use CMake 3.23 but you have access to CMake 3.20 or later, you can append
  ``-c tools.cmake.cmaketoolchain.presets:max_schema_version=2`` to the ``conan install`` command
  which will force Conan to use an older version of CMake Presets schema.


Installation and Reuse
----------------------

There are many different ways of installing/reusing **mp-units** in your project. Below we mention
only a few of many options possible.

Copy
^^^^

As **mp-units** is a C++ header-only library you can simply copy all needed ``src/*/include`` subdirectories
to your source tree.

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
      mp-units/0.7.0

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

      mkdir my_project/build && cd my_project/build
      conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
      cmake .. -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
      cmake --build . --config Release


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

      conan remote add conan-mpusz https://mpusz.jfrog.io/artifactory/api/conan/conan-oss

2. In your Conan configuration file provide package identifier of the ``mpusz/testing`` stream:

  .. code-block:: ini
      :caption: conanfile.txt

      [requires]
      mp-units/0.8.0@mpusz/testing

      [layout]
      cmake_layout

      [generators]
      CMakeToolchain
      CMakeDeps

  .. tip::

    The identifiers of the latest packages can always be found in
    `the project's README file <https://github.com/mpusz/units/blob/master/README.md>`_ or on
    `the project's Artifactory <https://mpusz.jfrog.io/ui/packages/conan:%2F%2Fmp-units>`_.

3. Force Conan to check for updated recipes ``-u`` and to build outdated packages ``-b outdated``:

  .. code-block:: shell

      conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -b=outdated -u
      cmake --preset default
      cmake --build --preset release


Install
^^^^^^^

In case you don't want to use Conan in your project and just want to install the **mp-units**
library on your file system and use it via ``find_package(mp-units)`` from another repository
to find it, it is enough to perform the following steps:

.. code-block:: shell

    conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
    mv CMakeUserPresets.json src
    cd src
    cmake --preset default -DCMAKE_INSTALL_PREFIX=<your_installation_path>
    cmake --build --preset release --target install


Contributing (or just building all the tests and examples)
----------------------------------------------------------

In case you would like to build all the source code (with unit tests and examples) in **mp-units** repository,
you should:

1. Use the *CMakeLists.txt* from the top-level directory.
2. Run Conan with `build_all`_ = ``True``
   (use ``-o build_docs=False`` if you want to skip the documentation generation).

.. code-block:: shell

    git clone https://github.com/mpusz/units.git && cd units
    conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -c user.build:all=True -c user.build:skip_docs=True -b outdated -u
    conan build .

The above will download and install all of the dependencies needed for the development of the library,
build all of the source code and run unit tests.

If you prefer to build the project via CMake rather then Conan, then you should replace the last ``conan build .``
step with the explicit CMake build:

.. code-block:: shell

    cmake --preset default
    cmake --build --preset release
    cmake --build --preset release --target test


Building documentation
----------------------

In case you would like to build the project's documentation, you should:

1. Use the *CMakeLists.txt* from the top-level directory.
2. Obtain Python dependencies.
3. Run Conan with `build_all`_ = ``True``.

.. code-block:: shell

    git clone https://github.com/mpusz/units.git && cd units
    pip3 install -r docs/requirements.txt
    conan install . -pr <your_conan_profile> -s compiler.cppstd=20 -c user.build:all=True -b missing
    cmake --preset default
    cmake --build --preset release --target documentation

The above will download and install all of the dependencies needed and build the documentation.


Packaging
---------

To test CMake installation and Conan packaging or create a Conan package run:

.. code-block:: shell

    conan create . <username>/<channel> -pr <your_conan_profile> -s compiler.cppstd=20 -c user.build:all=True -c user.build:skip_docs=True -b outdated -u

The above will create a Conan package and run tests provided in *./test_package* directory.


Uploading **mp-units** Package to the Conan Server
--------------------------------------------------

.. code-block:: shell

    conan upload -r <remote-name> --all mp-units/0.8.0@<user>/<channel>
