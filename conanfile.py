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

from conans import ConanFile, tools
from conans.tools import Version, check_min_cppstd
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conans.errors import ConanInvalidConfiguration
import os, re

required_conan_version = ">=1.40.0"

class UnitsConan(ConanFile):
    name = "mp-units"
    homepage = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    topics = ("units", "dimensions", "quantities", "dimensional-analysis", "physical-quantities", "physical-units", "system-of-units", "cpp23", "cpp20", "library", "quantity-manipulation")
    license = "MIT"
    url = "https://github.com/mpusz/units"
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "gsl-lite/0.38.1"
    )
    options = {
        "downcast_mode": ["off", "on", "auto"],
        "build_docs": [True, False]
    }
    default_options = {
        "downcast_mode": "on",
        "build_docs": True
    }
    exports = ["LICENSE.md"]
    exports_sources = ["docs/*", "src/*", "test/*", "cmake/*", "example/*","CMakeLists.txt"]
    # scm = {
    #     "type": "git",
    #     "url": "auto",
    #     "revision": "auto",
    #     "submodule": "recursive"
    # }
    generators = "cmake_paths"

    @property
    def _run_tests(self):
        return tools.get_env("CONAN_RUN_TESTS", False)

    @property
    def _use_libfmt(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        std_support = \
            (compiler == "Visual Studio" and version >= "17" and compiler.cppstd == 23) or \
            (compiler == "msvc" and (version == "19.3" or version >= "19.30") and compiler.cppstd == 23)
        return not std_support

    @property
    def _use_range_v3(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        return ("clang" in compiler and compiler.libcxx == "libc++" and version < "14.0")

    def set_version(self):
        content = tools.load(os.path.join(self.recipe_folder, "src/CMakeLists.txt"))
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        self.version = version.strip()

    def requirements(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if self._use_libfmt:
            self.requires("fmt/8.0.1")

        if self._use_range_v3:
            self.requires("range-v3/0.11.0")

    def build_requirements(self):
        if self._run_tests:
            self.build_requires("catch2/2.13.7", force_host_context=True)              # TODO replace with test_requires in Conan 2.0
            self.build_requires("wg21-linear_algebra/0.7.2", force_host_context=True)  # TODO replace with test_requires in Conan 2.0
            if self.options.build_docs:
                self.build_requires("doxygen/1.9.2")

    def validate(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            if version < "10.0":
                raise ConanInvalidConfiguration("mp-units requires at least g++-10")
        elif compiler == "clang":
            if version < "12":
                raise ConanInvalidConfiguration("mp-units requires at least clang++-12")
        elif compiler == "apple-clang":
            if version < "13":
                raise ConanInvalidConfiguration("mp-units requires at least AppleClang 13")
        elif compiler == "Visual Studio":
            if version < "16":
                raise ConanInvalidConfiguration("mp-units requires at least Visual Studio 16.9")
        elif compiler == "msvc":
            if version < "1928":
                raise ConanInvalidConfiguration("mp-units requires at least MSVC 19.28")
        else:
            raise ConanInvalidConfiguration("Unsupported compiler")
        check_min_cppstd(self, "20")

    # TODO Uncomment this when environment is supported in the Conan toolchain
    # def config_options(self):
    #     if not self._run_tests:
    #         # build_docs has sense only in a development or CI build
    #         del self.options.build_docs

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UNITS_DOWNCAST_MODE"] = str(self.options.downcast_mode).upper()
        # if self._run_tests:  # TODO Enable this when environment is supported in the Conan toolchain
        tc.variables["UNITS_BUILD_DOCS"] = self.options.build_docs == "True"
        tc.variables["UNITS_USE_LIBFMT"] = self._use_libfmt
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._run_tests else "src")
        cmake.build()
        if self._run_tests:
            cmake.test(output_on_failure=True)

    def package(self):
        self.copy(pattern="LICENSE.md", dst="licenses")
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        self.info.header_only()

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
        self.cpp_info.components["si-fps"].requires = ["si"]
        self.cpp_info.components["si-hep"].requires = ["si"]
        self.cpp_info.components["si-iau"].requires = ["si"]
        self.cpp_info.components["si-imperial"].requires = ["si"]
        self.cpp_info.components["si-international"].requires = ["si"]
        self.cpp_info.components["si-typographic"].requires = ["si"]
        self.cpp_info.components["si-uscs"].requires = ["si"]
        self.cpp_info.components["isq-iec80000"].requires = ["si"]
        self.cpp_info.components["systems"].requires = ["isq", "isq-natural", "si", "si-cgs", "si-fps", "si-hep", "si-iau", "si-imperial", "si-international", "si-typographic", "si-uscs", "isq-iec80000"]
