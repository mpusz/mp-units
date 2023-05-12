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
import re

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, load, rmdir
from conan.tools.scm import Version

required_conan_version = ">=2.0.0"


class MPUnitsConan(ConanFile):
    name = "mp-units"
    homepage = "https://github.com/mpusz/units"
    description = "Physical Quantities and Units library for C++"
    topics = (
        "units",
        "dimensions",
        "quantities",
        "dimensional-analysis",
        "physical-quantities",
        "physical-units",
        "system-of-units",
        "system-of-quantities",
        "isq",
        "si",
        "library",
        "quantity-manipulation",
    )
    license = "MIT"
    url = "https://github.com/mpusz/units"
    settings = "os", "arch", "compiler", "build_type"
    options = {"downcast_mode": ["off", "on", "auto"]}
    default_options = {"downcast_mode": "on"}
    exports = ["LICENSE.md"]
    exports_sources = [
        "docs/*",
        "src/*",
        "test/*",
        "cmake/*",
        "example/*",
        "CMakeLists.txt",
    ]
    package_type = "header-library"
    no_copy_source = True

    @property
    def _min_cppstd(self):
        return "20"

    @property
    def _minimum_compilers_version(self):
        return {"gcc": "10.3", "clang": "12", "apple-clang": "13", "msvc": "192"}

    @property
    def _build_all(self):
        return bool(self.conf.get("user.build:all", default=False))

    @property
    def _skip_la(self):
        return bool(self.conf.get("user.build:skip_la", default=False))

    @property
    def _skip_docs(self):
        return bool(self.conf.get("user.build:skip_docs", default=False))

    @property
    def _use_libfmt(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        std_support = compiler == "msvc" and version >= 193 and compiler.cppstd == 23
        return not std_support

    @property
    def _use_range_v3(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        return "clang" in compiler and compiler.libcxx == "libc++" and version < 14

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "src/CMakeLists.txt"))
        version = re.search(
            r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content
        ).group(1)
        self.version = version.strip()

    def requirements(self):
        self.requires("gsl-lite/0.40.0")
        if self._use_libfmt:
            self.requires("fmt/8.1.1")
        if self._use_range_v3:
            self.requires("range-v3/0.11.0")

    def build_requirements(self):
        if self._build_all:
            self.test_requires("catch2/3.1.0")
            if not self._skip_la:
                self.test_requires("wg21-linear_algebra/0.7.3")
            if not self._skip_docs:
                self.tool_requires("doxygen/1.9.4")

    def validate(self):
        check_min_cppstd(self, self._min_cppstd)

        def loose_lt_semver(v1, v2):
            lv1 = [int(v) for v in v1.split(".")]
            lv2 = [int(v) for v in v2.split(".")]
            min_length = min(len(lv1), len(lv2))
            return lv1[:min_length] < lv2[:min_length]

        compiler = self.settings.compiler
        min_version = self._minimum_compilers_version.get(str(compiler))
        if min_version and loose_lt_semver(str(compiler.version), min_version):
            raise ConanInvalidConfiguration(
                f"{self.ref} requires at least {compiler} {min_version} ({compiler.version} in use)"
            )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UNITS_DOWNCAST_MODE"] = str(self.options.downcast_mode).upper()
        tc.variables["UNITS_BUILD_LA"] = self._build_all and not self._skip_la
        tc.variables["UNITS_BUILD_DOCS"] = self._build_all and not self._skip_docs
        tc.variables["UNITS_USE_LIBFMT"] = self._use_libfmt
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._build_all else "src")
        cmake.build()
        if self._build_all:
            cmake.test()

    def package_id(self):
        self.info.clear()

    def package(self):
        copy(
            self,
            "LICENSE.md",
            self.source_folder,
            os.path.join(self.package_folder, "licenses"),
        )
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        compiler = self.settings.compiler

        # core
        self.cpp_info.components["core"].requires = ["gsl-lite::gsl-lite"]
        if compiler == "msvc":
            self.cpp_info.components["core"].cxxflags = ["/utf-8"]
        if self._use_range_v3:
            self.cpp_info.components["core"].requires.append("range-v3::range-v3")

        # rest
        self.cpp_info.components["core-io"].requires = ["core"]
        self.cpp_info.components["core-fmt"].requires = ["core"]
        if self._use_libfmt:
            self.cpp_info.components["core-fmt"].requires.append("fmt::fmt")
        self.cpp_info.components["isq"].requires = ["core"]
        self.cpp_info.components["isq-natural"].requires = ["isq"]
        self.cpp_info.components["si"].requires = ["isq"]
        self.cpp_info.components["si-cgs"].requires = ["si"]
        self.cpp_info.components["si-fps"].requires = ["si-international"]
        self.cpp_info.components["si-hep"].requires = ["si"]
        self.cpp_info.components["si-iau"].requires = ["si"]
        self.cpp_info.components["si-imperial"].requires = ["si"]
        self.cpp_info.components["si-international"].requires = ["si"]
        self.cpp_info.components["si-typographic"].requires = ["si"]
        self.cpp_info.components["si-uscs"].requires = ["si"]
        self.cpp_info.components["isq-iec80000"].requires = ["si"]
        self.cpp_info.components["systems"].requires = [
            "isq",
            "isq-natural",
            "si",
            "si-cgs",
            "si-fps",
            "si-hep",
            "si-iau",
            "si-imperial",
            "si-international",
            "si-typographic",
            "si-uscs",
            "isq-iec80000",
        ]
