# The MIT License (MIT)
#
# Copyright (c) 2018 Mateusz Pusz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os

from conan import ConanFile
from conan.errors import ConanException
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class TestPackageConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    # backing libraries exercised by the linear algebra integration consumers
    _linear_algebra_libs = ["eigen/5.0.1", "glm/1.0.1", "blaze/3.8.2"]

    def requirements(self):
        self.requires(self.tested_reference_str)
        for ref in self._linear_algebra_libs:
            self.requires(ref)

    def build_requirements(self):
        self.tool_requires("cmake/[>=4.3.0 <5]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        opt = self.dependencies["mp-units"].options
        tc.cache_variables["MP_UNITS_BUILD_CXX_MODULES"] = opt.cxx_modules
        if opt.cxx_modules or opt.import_std:
            tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = True
        if opt.cxx_modules:
            # Consuming the installed C++ modules needs a single active configuration. Ninja
            # Multi-Config hits CMake #26312 (transitive synthetic targets for imported modules are
            # only hooked up for the first configuration in CMAKE_CONFIGURATION_TYPES), so
            # `import mp_units;` would otherwise fail to find the dependency modules. Pinning the
            # configuration types to the one being built keeps it first. See test_package/CMakeLists.txt.
            tc.cache_variables["CMAKE_CONFIGURATION_TYPES"] = str(
                self.settings.build_type
            )
        if opt.import_std:
            tc.cache_variables["CMAKE_CXX_MODULE_STD"] = True
            # Current experimental support according to `Help/dev/experimental.rst`
            tc.cache_variables["CMAKE_EXPERIMENTAL_CXX_IMPORT_STD"] = (
                "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
            )
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if not can_run(self):
            return
        bindir = self.cpp.build.bindirs[0]
        modules = bool(self.dependencies["mp-units"].options.cxx_modules)

        # `test_package` consumers always exist and must run; a missing one means the build layout
        # and test() disagree on the binary location, which would otherwise pass silently.
        required = ["test_package-headers"]
        if modules:
            required.append("test_package")
        # Linear algebra integration consumers are optional: only the backends found at build time
        # produce a binary. They are verified in header mode and, in a modules build, in module mode.
        optional = []
        for backend in ("eigen", "glm", "blaze"):
            optional.append(f"test_package_integration-{backend}-headers")
            if modules:
                optional.append(f"test_package_integration-{backend}")

        def run_if_present(name, *, must_exist):
            bin_path = os.path.join(bindir, name)
            if os.path.exists(bin_path) or os.path.exists(bin_path + ".exe"):
                self.run(bin_path, env="conanrun")
            elif must_exist:
                raise ConanException(
                    f"expected test binary '{name}' not found in '{bindir}'"
                )

        for name in required:
            run_if_present(name, must_exist=True)
        for name in optional:
            run_if_present(name, must_exist=False)
