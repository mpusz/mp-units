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
from conans.tools import load, Version
from conans.errors import ConanInvalidConfiguration
import re
import os


def get_version():
    try:
        content = load("src/CMakeLists.txt")
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        return version.strip()
    except Exception:
        return None


class UnitsConan(ConanFile):
    name = "mp-units"
    version = get_version()
    author = "Mateusz Pusz"
    license = "https://github.com/mpusz/units/blob/master/LICENSE.md"
    url = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    exports = ["LICENSE.md"]
    exports_sources = ["src/*", "test/*", "cmake/*", "example/*","CMakeLists.txt"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "range-v3/0.9.1@ericniebler/stable",
        "Catch2/2.10.0@catchorg/stable",
        "fmt/6.0.0"
    )
    generators = "cmake"

    @property
    def _run_tests(self):
        return tools.get_env("CONAN_RUN_TESTS", False)

    def configure(self):
        if self.settings.compiler != "gcc":
            raise ConanInvalidConfiguration("Library works only with gcc")
        if Version(self.settings.compiler.version) < "9":
            raise ConanInvalidConfiguration("Library requires at least gcc-9")
        if self.settings.compiler.cppstd not in ["20", "gnu20"]:
            raise ConanInvalidConfiguration("Library requires at least C++20 support")

    def _configure_cmake(self, folder="src"):
        cmake = CMake(self)
        if self._run_tests:
            cmake.configure()
        else:
            cmake.configure(source_folder="src", build_folder="src")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self._run_tests:
            self.run(os.path.join("bin", "unit_tests_runtime"), run_environment=True)

    def package(self):
        self.copy(pattern="LICENSE.md", dst="licenses")
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']
        if self.settings.compiler == "gcc":
            self.cpp_info.cxxflags = [
                "-fconcepts",
                "-Wno-literal-suffix",
                "-Wno-non-template-friend",
                "-Wno-stringop-overflow",
                "-Wno-pedantic"
            ]

    def package_id(self):
        self.info.settings.clear()
        self.info.settings.compiler = self.settings.compiler
        self.info.settings.compiler.version = self.settings.compiler.version
