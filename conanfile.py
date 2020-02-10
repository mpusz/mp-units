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
from conans.tools import Version
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
    author = "Mateusz Pusz"
    license = "https://github.com/mpusz/units/blob/master/LICENSE.md"
    url = "https://github.com/mpusz/units"
    description = "Physical Units library for C++"
    exports = ["LICENSE.md"]
    exports_sources = ["src/*", "test/*", "cmake/*", "example/*","CMakeLists.txt"]
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "fmt/6.1.0"
    )
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

    def _configure_cmake(self, folder="src"):
        cmake = CMake(self)
        if self._run_tests:
            # developer's mode (unit tests, examples, restrictive compilation warnings, ...)
            cmake.configure()
        else:
            # consumer's mode (library sources only)
            cmake.configure(source_folder=folder, build_folder=folder)
        return cmake

    def configure(self):
        if self.settings.compiler != "gcc": # and self.settings.compiler != "clang":
            raise ConanInvalidConfiguration("Library works only with gcc") # and clang")
        if self.settings.compiler == "gcc" and Version(self.settings.compiler.version) < "9":
            raise ConanInvalidConfiguration("Library requires at least g++-9")
        if self.settings.compiler == "clang" and Version(self.settings.compiler.version) < "11":
            raise ConanInvalidConfiguration("Library requires at least clang++-11")
        if self.settings.compiler.cppstd not in ["20", "gnu20"]:
            raise ConanInvalidConfiguration("Library requires at least C++20 support")

    def requirements(self):
        if self.settings.compiler == "clang" or Version(self.settings.compiler.version) < "10":
            self.requires("range-v3/0.10.0@ericniebler/stable")

    def build_requirements(self):
        if self._run_tests:
            self.build_requires("Catch2/2.11.0@catchorg/stable")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self._run_tests:
            self.run("ctest -VV -C %s" % cmake.build_type, run_environment=True)

    def package(self):
        self.copy(pattern="LICENSE.md", dst="licenses")
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        if self.settings.compiler == "gcc":
            self.cpp_info.cxxflags = [
                "-Wno-literal-suffix",
                "-Wno-non-template-friend",
            ]
            if Version(self.settings.compiler.version) < "10":
                self.cpp_info.cxxflags.extend([
                    "-fconcepts"
                ])

    def package_id(self):
        self.info.header_only()
