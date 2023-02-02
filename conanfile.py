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
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, load, rmdir
from conan.tools.scm import Version
from conans.errors import ConanInvalidConfiguration

required_conan_version = ">=1.50.0"


class MPUnitsConan(ConanFile):
    name = "mp-units"
    homepage = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    topics = (
        "units",
        "dimensions",
        "quantities",
        "dimensional-analysis",
        "physical-quantities",
        "physical-units",
        "system-of-units",
        "cpp23",
        "cpp20",
        "library",
        "quantity-manipulation",
    )
    license = "MIT"
    url = "https://github.com/mpusz/units"
    settings = "os", "compiler", "build_type", "arch"
    requires = "gsl-lite/0.40.0"
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
    no_copy_source = True
    generators = "cmake_paths"

    @property
    def _build_all(self):
        return bool(self.conf["user.build:all"])

    @property
    def _skip_docs(self):
        return bool(self.conf["user.build:skip_docs"])

    @property
    def _use_libfmt(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        std_support = (
            compiler == "Visual Studio" and version >= 17 and compiler.cppstd == 23
        ) or (compiler == "msvc" and version >= 193 and compiler.cppstd == 23)
        return not std_support

    @property
    def _use_range_v3(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        return "clang" in compiler and compiler.libcxx == "libc++" and version < 14

    @property
    def _msvc_version(self):
        compiler = self.settings.compiler
        if compiler.update:
            return int(f"{compiler.version}{compiler.update}")
        else:
            return int(f"{compiler.version}0")

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "src/CMakeLists.txt"))
        version = re.search(
            r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content
        ).group(1)
        self.version = version.strip()

    def requirements(self):
        if self._use_libfmt:
            self.requires("fmt/8.1.1")

        if self._use_range_v3:
            self.requires("range-v3/0.11.0")

    def build_requirements(self):
        if self._build_all:
            self.test_requires("catch2/3.1.0")
            self.test_requires("wg21-linear_algebra/0.7.2")
            if not self._skip_docs:
                self.tool_requires("doxygen/1.9.4")

    # TODO Replace with `valdate()` for Conan 2.0 (https://github.com/conan-io/conan/issues/10723)
    def configure(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            if version < 10:
                raise ConanInvalidConfiguration("mp-units requires at least g++-10")
        elif compiler == "clang":
            if version < 12:
                raise ConanInvalidConfiguration("mp-units requires at least clang++-12")
        elif compiler == "apple-clang":
            if version < 13:
                raise ConanInvalidConfiguration(
                    "mp-units requires at least AppleClang 13"
                )
        elif compiler == "Visual Studio":
            if version < 16:
                raise ConanInvalidConfiguration(
                    "mp-units requires at least Visual Studio 16.9"
                )
        elif compiler == "msvc":
            if self._msvc_version < 1928:
                raise ConanInvalidConfiguration("mp-units requires at least MSVC 19.28")
        else:
            raise ConanInvalidConfiguration("Unsupported compiler")
        check_min_cppstd(self, 20)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UNITS_DOWNCAST_MODE"] = str(self.options.downcast_mode).upper()
        tc.variables["UNITS_BUILD_DOCS"] = self._build_all and not self._skip_docs
        tc.variables["UNITS_USE_LIBFMT"] = self._use_libfmt
        tc.generate()
        deps = CMakeDeps(self)
        if self._build_all and not self._skip_docs:
            deps.build_context_activated = ["doxygen"]
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
        if compiler == "Visual Studio":
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
