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
from conan.tools.cmake import CMakeToolchain, CMake, CMakeDeps
from conans.errors import ConanInvalidConfiguration
import re

required_conan_version = ">=1.33.0"

def get_version():
    try:
        content = tools.load("src/CMakeLists.txt")
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        return version.strip()
    except Exception:
        return None

class UnitsConan(ConanFile):
    name = "mp-units"
    version = get_version()
    homepage = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    topics = ("units", "dimensions", "quantities", "dimensional-analysis", "physical-quantities", "physical-units", "system-of-units", "cpp23", "cpp20", "library", "quantity-manipulation")
    license = "MIT"
    url = "https://github.com/mpusz/units"
    settings = "compiler", "build_type"
    requires = (
        "fmt/7.1.3",
        "gsl-lite/0.37.0"
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

    def _configure_cmake(self):
        cmake = CMake(self)
        if self._run_tests:
            # developer's mode (unit tests, examples, documentation, restrictive compilation warnings, ...)
            cmake.configure()
        else:
            # consumer's mode (library sources only)
            cmake.configure(source_folder="src")
        return cmake

    def validate(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            if version < "10.0":
                raise ConanInvalidConfiguration("mp-units requires at least g++-10")
        elif compiler == "Visual Studio":
            if version < "16":
                raise ConanInvalidConfiguration("mp-units requires at least Visual Studio 16.7")
        else:
            raise ConanInvalidConfiguration("mp-units is supported only by gcc and Visual Studio so far")
        if compiler.get_safe("cppstd"):
            check_min_cppstd(self, "20")

    # TODO Uncomment this when environment is supported in the Conan toolchain
    # def config_options(self):
    #     if not self._run_tests:
    #         # build_docs has sense only in a development or CI build
    #         del self.options.build_docs

    def build_requirements(self):
        if self._run_tests:
            self.build_requires("catch2/2.13.4")
            self.build_requires("linear_algebra/0.7.0@public-conan/stable")
            if self.options.build_docs:
                self.build_requires("doxygen/1.8.20")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UNITS_DOWNCAST_MODE"] = str(self.options.downcast_mode).upper()
        # if self._run_tests:  # TODO Enable this when environment is supported in the Conan toolchain
        tc.variables["UNITS_BUILD_DOCS"] = self.options.build_docs
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self._run_tests:
            cmake.test(output_on_failure=True)

    def package(self):
        self.copy(pattern="LICENSE.md", dst="licenses")
        cmake = self._configure_cmake()
        cmake.install()

    def package_id(self):
        self.info.header_only()

    def package_info(self):
        self.cpp_info.filenames["cmake_find_package"] = "mp-units"
        self.cpp_info.filenames["cmake_find_package_multi"] = "mp-units"
        self.cpp_info.names["cmake_find_package"] = "mp"
        self.cpp_info.names["cmake_find_package_multi"] = "mp"
        self.cpp_info.components["units"].name = "units"
        self.cpp_info.components["units"].requires = ["fmt::fmt", "gsl-lite::gsl-lite"]

        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            self.cpp_info.components["units"].cxxflags = [
                "-Wno-non-template-friend"
            ]
        elif compiler == "Visual Studio":
            self.cpp_info.components["units"].cxxflags = [
                "/utf-8"
            ]
