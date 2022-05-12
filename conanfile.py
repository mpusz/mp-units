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
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
from conan.tools.files import copy, load, rmdir
from conan.tools.scm import Version
from conans.errors import ConanInvalidConfiguration
from conans.tools import (  # TODO replace with new tools for Conan 2.0
    check_min_cppstd,
    get_env,
)

required_conan_version = ">=1.48.0"


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
    options = {"downcast_mode": ["off", "on", "auto"], "build_docs": [True, False]}
    default_options = {"downcast_mode": "on", "build_docs": True}
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
    def _run_tests(self):
        return get_env("CONAN_RUN_TESTS", False)

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
        if self._run_tests:
            self.test_requires("catch2/2.13.9")
            self.test_requires("wg21-linear_algebra/0.7.2")
            if self.options.build_docs:
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

    # TODO Uncomment this when environment is supported in the Conan toolchain
    # def config_options(self):
    #     if not self._run_tests:
    #         # build_docs has sense only in a development or CI build
    #         del self.options.build_docs

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UNITS_DOWNCAST_MODE"] = str(self.options.downcast_mode).upper()
        # if self._run_tests:  # TODO Enable this when environment is supported in the Conan toolchain
        tc.variables["UNITS_BUILD_DOCS"] = bool(self.options.build_docs)
        tc.variables["UNITS_USE_LIBFMT"] = self._use_libfmt
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._run_tests else "src")
        cmake.build()
        if self._run_tests:
            cmake.test()

    def package_id(self):
        self.info.header_only()

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
        self.cpp_info.components["core"].includedirs = ["include"]
        if compiler == "Visual Studio":
            self.cpp_info.components["core"].cxxflags = ["/utf-8"]
        if self._use_range_v3:
            self.cpp_info.components["core"].requires.append("range-v3::range-v3")

        # rest
        self.cpp_info.components["core-io"].requires = ["core"]
        self.cpp_info.components["core-io"].includedirs = ["include"]

        self.cpp_info.components["core-fmt"].requires = ["core"]
        self.cpp_info.components["core-fmt"].includedirs = ["include"]
        if self._use_libfmt:
            self.cpp_info.components["core-fmt"].requires.append("fmt::fmt")

        self.cpp_info.components["isq"].requires = ["core"]
        self.cpp_info.components["isq"].includedirs = ["include"]

        self.cpp_info.components["isq-natural"].requires = ["isq"]
        self.cpp_info.components["isq-natural"].includedirs = ["include"]

        self.cpp_info.components["si"].requires = ["isq"]
        self.cpp_info.components["si"].includedirs = ["include"]

        self.cpp_info.components["si-cgs"].requires = ["si"]
        self.cpp_info.components["si-cgs"].includedirs = ["include"]

        self.cpp_info.components["si-fps"].requires = ["si-international"]
        self.cpp_info.components["si-fps"].includedirs = ["include"]

        self.cpp_info.components["si-hep"].requires = ["si"]
        self.cpp_info.components["si-hep"].includedirs = ["include"]

        self.cpp_info.components["si-iau"].requires = ["si"]
        self.cpp_info.components["si-iau"].includedirs = ["include"]

        self.cpp_info.components["si-imperial"].requires = ["si"]
        self.cpp_info.components["si-imperial"].includedirs = ["include"]

        self.cpp_info.components["si-international"].requires = ["si"]
        self.cpp_info.components["si-international"].includedirs = ["include"]

        self.cpp_info.components["si-typographic"].requires = ["si"]
        self.cpp_info.components["si-typographic"].includedirs = ["include"]

        self.cpp_info.components["si-uscs"].requires = ["si"]
        self.cpp_info.components["si-uscs"].includedirs = ["include"]

        self.cpp_info.components["isq-iec80000"].requires = ["si"]
        self.cpp_info.components["isq-iec80000"].includedirs = ["include"]

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
