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
from conans.tools import load
from conans.errors import ConanInvalidConfiguration
import re


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
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "cmcstl2/2019.03.18@mpusz/stable",
        "gsl-lite/0.33.0@nonstd-lite/stable"
    )
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    generators = "cmake"

    def _configure_cmake(self):
        cmake = CMake(self)
        if tools.get_env("CONAN_RUN_TESTS", False):
            cmake.configure()
        else:
            cmake.configure(source_dir="%s/src" % self.source_folder)
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="*license*", dst="licenses", excludes="cmake/common/*", ignore_case=True, keep_path=False)
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']

    def package_id(self):
        self.info.header_only()
