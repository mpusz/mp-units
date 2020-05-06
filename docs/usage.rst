Usage
=====

.. note::

    As this library targets C++23 and extensively uses C++20 features as of now it compiles correctly
    only with gcc-9.1 and newer.

Repository Structure and Dependencies
-------------------------------------

This repository contains three independent CMake-based projects:

- *./src*

  - header-only project containing whole **mp-units** library
  - when C++20 support will be fully supported by C++ compilers this library will have
    no external dependencies but until then it depends on
    `range-v3 <https://github.com/ericniebler/range-v3>`_ (only for gcc versions < 10.0)
    and `{fmt} <https://github.com/fmtlib/fmt>`_ libraries.

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

  - library installation and Conan package verification

.. note::

    Please note that **mp-units** repository also depends on a git submodule in the
    *./cmake/common* subdirectory providing some common CMake utilities.


Obtaining Dependencies
----------------------

This library assumes that most of the dependencies will be provided by the
`Conan Package Manager <https://conan.io/>`_. In case you would like to obtain needed
dependencies by other means some modifications to library's CMake files will be needed.
The rest of the dependencies are provided by :command:`python3-pip`.

.. seealso::

  A full list of dependencies can be found in `Repository structure and dependencies`_.

Conan Quick Intro
^^^^^^^^^^^^^^^^^

In case you are not familiar with Conan, to install it (or upgrade) just do:

.. code-block:: shell

    pip3 install -U conan

After that you might need to add a custom profile file for your development environment
in *~/.conan/profile* directory. An example profile can look as follows:

.. code-block:: ini
    :emphasize-lines: 8

    [settings]
    os=Linux
    os_build=Linux
    arch=x86_64
    arch_build=x86_64
    compiler=gcc
    compiler.version=9
    compiler.cppstd=20
    compiler.libcxx=libstdc++11
    build_type=Release

    [options]
    [build_requires]

    [env]
    CC=/usr/bin/gcc-9
    CXX=/usr/bin/g++-9

.. tip::

    Please note that **mp-units** library requires C++20 to be set either in a Conan profile or forced
    via Conan command line. If you do the former, you will not need to provide ``-s compiler.cppstd=20``
    every time your rune a Conan command line (as it is suggested below).

Non-standard Conan remotes
^^^^^^^^^^^^^^^^^^^^^^^^^^

Add the following remotes to your local Conan instance:

.. code-block:: shell

    conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz
    conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
    conan remote add linear-algebra https://api.bintray.com/conan/twonington/public-conan

.. note::

    The last two remotes are needed only if you plan to build all of the code and documentation
    in **mp-units** repository.


Build Options
-------------

Environment Variables
^^^^^^^^^^^^^^^^^^^^^

.. envvar:: CONAN_RUN_TESTS

    **Defaulted to**: Not defined (``True``/``False`` if defined)

    Enables compilation of all the source code (tests and examples) and building the documentation.
    To support that it requires some additional Conan build dependencies described in
    `Repository structure and dependencies`_.
    It also runs unit tests during Conan build.


Building, Installation, and Reuse
---------------------------------

There are a few different ways of installing/reusing **mp-units** in your project.

Copy
^^^^

As **mp-units** is a C++ header-only library you can simply copy *src/include* directory to
your source tree and use it as regular header files.

CMake + Conan
^^^^^^^^^^^^^

To use **mp-units** as a CMake imported library the following steps may be performed:

1. Clone the repository together with its submodules:

  .. code-block:: shell

      git clone --recurse-submodules https://github.com/mpusz/units.git

  or in case it is already cloned without submodules, initialize, fetch, and checkout them with:

  .. code-block:: shell

      git submodule update --init

2. Create Conan configuration file (either *conanfile.txt* or *conanfile.py*) in your
   project's top-level directory and add **mp-units** as a dependency to your Conan configuration
   file.

  - for example to use **mp-units** testing/prerelease version ``0.5.0`` in case of *conanfile.txt*
    it is enough for it to just contain the following lines:

    .. code-block:: ini

        [requires]
        mp-units/0.5.0@mpusz/testing

3. Import Conan dependencies definitions to the beginning of your top-level *CMakeLists.txt*
   file in your project:

  .. code-block:: cmake

      include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
      conan_basic_setup(TARGETS)

4. Link your CMake target with **mp-units**:

  .. code-block:: cmake

      target_link_libraries(<your_target> PUBLIC|PRIVATE|INTERFACE CONAN_PKG::mp-units)

5. Download and install Conan dependencies before running CMake configuration step:

  .. code-block:: shell

      cd build
      conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=outdated -u

6. Configure your CMake project as usual.


Full **mp-units** Build, Unit Testing, and Documentation Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case you would like to build all the source code (with unit tests and examples) and documentation
in **mp-units** repository, you should use the *CMakeLists.txt* from the top-level directory,
obtain Python dependencies, and run Conan with :envvar:`mp-units:CONAN_RUN_TESTS` = ``True``:

.. code-block:: shell

    git clone --recurse-submodules https://github.com/mpusz/units.git && cd units
    pip3 install -r docs/requirements.txt
    mkdir build && cd build
    conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -e mp-units:CONAN_RUN_TESTS=True -b outdated
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .
    ctest

The above will download and install all of the dependencies needed for the development of the library,
build all of the source code and documentation, and run unit tests.


Packaging
---------

To test CMake installation and Conan packaging or create a Conan package run:

.. code-block:: shell

    git clone --recurse-submodules https://github.com/mpusz/units.git && cd units
    pip3 install -r docs/requirements.txt
    conan create . <username>/<channel> -pr <your_conan_profile> -s compiler.cppstd=20 -e mp-units:CONAN_RUN_TESTS=True -b outdated

The above will create a Conan package and run tests provided in *./test_package* directory.


Uploading **mp-units** Package to the Conan Server
--------------------------------------------------

.. code-block:: shell

    conan upload -r <remote-name> --all mp-units/0.5.0@<user>/<channel>
