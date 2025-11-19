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

import io
import os
import re

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.scm import Version


class TestPackageConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self)

    def _cmake_program(self) -> str:
        return self.conf.get(
            "tools.cmake:cmake_program",
            default="cmake",
            check_type=str,
        )

    def _cmake_version(self) -> Version:
        """
        Run `<cmake> --version` and return a `Version`.

        Fails loudly if it cannot parse the version.
        """
        cmake = self._cmake_program()

        buf = io.StringIO()
        # We call the executable directly; no env file needed just to read --version
        self.run(
            f'"{cmake}" --version',
            stdout=buf,
            stderr=buf,
            quiet=True,
            env=None,
            scope="build",
        )
        out = buf.getvalue()

        # Typical first line: "cmake version 3.30.0-rc1"
        match = re.search(r"cmake version ([0-9][^\s]*)", out)
        if not match:
            raise ConanInvalidConfiguration(
                f"Could not parse CMake version from `{cmake} --version` output:\n{out}"
            )

        version = match.group(1)
        return Version(version)

    def _import_std_uuid_for(self, cmake_version: Version | None = None) -> str:
        # Current experimental support according to `Help/dev/experimental.rst`
        # see also: https://discourse.cmake.org/t/import-std-appears-to-be-broken-in-release-4-0-2/14062
        if cmake_version is None:
            cmake_version = self._cmake_version()
        table = [
            (Version("4.1.0"), "d0edc3af-4c50-42ea-a356-e2862fe7a444"),
            (Version("4.0.2"), "a9e1cf81-9932-4810-974b-6eccaf14e457"),
            (Version("3.30.0"), "0e5b6991-d74f-4b3d-a41c-cf096e0b2508"),
        ]
        ret = None
        for lower_bound, uuid in table:
            if lower_bound > cmake_version:
                continue
            ret = uuid
            break
        self.output.highlight(
            f"For {cmake_version}, we selected CMAKE_EXPERIMENTAL_CXX_IMPORT_STD={ret!r}"
        )
        if ret is None:
            raise ConanInvalidConfiguration(
                "import_std=True but this recipe does not know a "
                "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD value for "
                f"CMake {cmake_version}."
            )
        return ret

    def generate(self):
        tc = CMakeToolchain(self)
        opt = self.dependencies["mp-units"].options
        if opt.cxx_modules:
            tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = True
            tc.cache_variables["MP_UNITS_BUILD_CXX_MODULES"] = True
        if opt.import_std:
            tc.cache_variables["CMAKE_CXX_MODULE_STD"] = True
            tc.cache_variables["CMAKE_EXPERIMENTAL_CXX_IMPORT_STD"] = (
                self._import_std_uuid_for()
            )
        # TODO remove the below when Conan will learn to handle C++ modules
        if opt.cxx_modules:
            if opt.freestanding:
                tc.cache_variables["MP_UNITS_API_FREESTANDING"] = True
            else:
                tc.cache_variables["MP_UNITS_API_STD_FORMAT"] = opt.std_format
            tc.cache_variables["MP_UNITS_API_CONTRACTS"] = str(opt.contracts).upper()
        tc.cache_variables["MP_UNITS_API_NATURAL_UNITS"] = opt.natural_units
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            if self.dependencies["mp-units"].options.cxx_modules:
                bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package")
                self.run(bin_path, env="conanrun")
            bin_path = os.path.join(self.cpp.build.bindirs[0], "test_package-headers")
            self.run(bin_path, env="conanrun")
