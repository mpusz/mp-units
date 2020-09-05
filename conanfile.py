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

from conans import ConanFile, CMake, tools
from conans.tools import Version, check_min_cppstd
from conans.errors import ConanInvalidConfiguration
import re


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
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "fmt/7.0.3",
        "ms-gsl/3.1.0"
    )
    exports = ["LICENSE.md"]
    exports_sources = ["docs/*", "src/*", "test/*", "cmake/*", "example/*","CMakeLists.txt"]
    # scm = {
    #     "type": "git",
    #     "url": "auto",
    #     "revision": "auto",
    #     "submodule": "recursive"
    # }
    generators = "cmake"

    @property
    def _run_tests(self):
        return tools.get_env("CONAN_RUN_TESTS", False)

    def _validate_compiler_settings(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            if version < "9.3":
                raise ConanInvalidConfiguration("mp-units requires at least g++-9.3")
        elif compiler == "Visual Studio":
            if version < "16":
                raise ConanInvalidConfiguration("mp-units requires at least MSVC 16")
        else:
            raise ConanInvalidConfiguration("mp-units is supported only by gcc and Visual Studio so far")
        check_min_cppstd(self, "20")

    def _configure_cmake(self, folder="src"):
        cmake = CMake(self)
        if self._run_tests:
            # developer's mode (unit tests, examples, documentation, restrictive compilation warnings, ...)
            cmake.configure()
        else:
            # consumer's mode (library sources only)
            cmake.configure(source_folder=folder)
        return cmake

    def configure(self):
        self._validate_compiler_settings()

    def requirements(self):
        if ((self.settings.compiler == "gcc" and Version(self.settings.compiler.version) < "10") or
            self.settings.compiler == "clang"):
            self.requires("range-v3/0.11.0")

    def build_requirements(self):
        if self._run_tests:
            self.build_requires("catch2/2.13.0")
            self.build_requires("doxygen/1.8.18")
            self.build_requires("linear_algebra/0.7.0@public-conan/stable")

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
        self.info.settings.clear()
        self.info.settings.compiler = self.settings.compiler
        self.info.settings.compiler.version = self.settings.compiler.version

    def package_info(self):
        compiler = self.settings.compiler
        version = Version(self.settings.compiler.version)
        if compiler == "gcc":
            self.cpp_info.cxxflags = [
                "-Wno-literal-suffix",
                "-Wno-non-template-friend",
            ]
            if version < "10":
                self.cpp_info.cxxflags.extend([
                    "-fconcepts"
                ])
        elif compiler == "Visual Studio":
            self.cpp_info.cxxflags = [
                "/utf-8",
                "/wd4455"
            ]
