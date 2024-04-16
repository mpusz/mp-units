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
from conan.tools.build import can_run, default_cppstd, valid_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, load, rmdir

required_conan_version = ">=2.0.0"


def loose_lt_semver(v1, v2):
    lv1 = [int(v) for v in v1.split(".")]
    lv2 = [int(v) for v in v2.split(".")]
    min_length = min(len(lv1), len(lv2))
    return lv1[:min_length] < lv2[:min_length]


class MPUnitsConan(ConanFile):
    name = "mp-units"
    homepage = "https://github.com/mpusz/mp-units"
    description = "The quantities and units library for C++"
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
    url = "https://github.com/mpusz/mp-units"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "cxx_modules": ["auto", True, False],
        "std_format": ["auto", True, False],
        "string_view_ret": ["auto", True, False],
        "no_crtp": ["auto", True, False],
    }
    default_options = {
        "cxx_modules": "auto",
        "std_format": "auto",
        "string_view_ret": "auto",
        "no_crtp": "auto",
    }
    tool_requires = "cmake/[>=3.29]"
    exports = "LICENSE.md"
    exports_sources = (
        "docs/*",
        "src/*",
        "test/*",
        "cmake/*",
        "example/*",
        "CMakeLists.txt",
    )
    package_type = "header-library"
    no_copy_source = True

    @property
    def _feature_compatibility(self):
        return {
            "minimum_support": {
                "std": "20",
                "compiler": {
                    "gcc": "12",
                    "clang": "16",
                    "apple-clang": "15",
                    "msvc": "",
                },
            },
            "std_format": {
                "std": "20",
                "compiler": {
                    "gcc": "13",
                    "clang": "17",
                    "apple-clang": "",
                    "msvc": "",
                },
            },
            "cxx_modules": {
                "std": "20",
                "compiler": {"gcc": "14", "clang": "17", "apple-clang": "", "msvc": ""},
            },
            "static_constexpr_vars_in_constexpr_func": {
                "std": "23",
                "compiler": {"gcc": "13", "clang": "17", "apple-clang": "", "msvc": ""},
            },
            "explicit_this": {
                "std": "23",
                "compiler": {
                    "gcc": "14",
                    "clang": "18",
                    "apple-clang": "",
                    "msvc": "",
                },
            },
        }

    @property
    def _option_feature_map(self):
        return {
            "std_format": "std_format",
            "cxx_modules": "cxx_modules",
            "string_view_ret": "static_constexpr_vars_in_constexpr_func",
            "no_crtp": "explicit_this",
        }

    def _check_feature_supported(self, name, feature_name=name):
        compiler = self.settings.compiler
        cppstd = compiler.get_safe("cppstd", default_cppstd(self))
        feature = self._feature_compatibility[feature_name]

        # check C++ version
        if not valid_min_cppstd(self, feature["std"]):
            raise ConanInvalidConfiguration(
                f"'{name}' requires at least cppstd={feature['std']} ({cppstd} in use)",
            )

        # check compiler version
        min_version = feature["compiler"].get(str(compiler))
        if min_version is None:
            # not tested compiler being used - use at your own risk
            return
        if min_version == "":
            raise ConanInvalidConfiguration(
                f"'{name}' is not yet supported by any known {compiler} compiler"
            )
        if loose_lt_semver(str(compiler.version), min_version):
            raise ConanInvalidConfiguration(
                f"'{name}' requires at least {compiler}-{min_version} ({compiler}-{compiler.version} in use)"
            )

    def _is_feature_enabled(self, name):
        compiler = self.settings.compiler
        opt = self.options.get_safe(name)
        feature_name = self._option_feature_map[name]
        feature = self._feature_compatibility[feature_name]
        min_version = feature["compiler"].get(str(compiler))
        return bool(
            opt is True
            or (
                opt == "auto"
                and min_version
                and not loose_lt_semver(str(compiler.version), min_version)
            )
        )

    @property
    def _build_all(self):
        return bool(self.conf.get("user.mp-units.build:all", default=False))

    @property
    def _build_cxx_modules(self):
        return self._is_feature_enabled("cxx_modules")

    @property
    def _use_fmtlib(self):
        return not self._is_feature_enabled("std_format")

    @property
    def _skip_la(self):
        return bool(self.conf.get("user.mp-units.build:skip_la", default=False))

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "src/CMakeLists.txt"))
        version = re.search(
            r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content
        ).group(1)
        self.version = version.strip()

    def requirements(self):
        self.requires("gsl-lite/0.41.0")
        if self._use_fmtlib:
            self.requires("fmt/10.2.1")

    def build_requirements(self):
        if self._build_all:
            self.test_requires("catch2/3.5.1")
            if not self._skip_la:
                self.test_requires("wg21-linear_algebra/0.7.3")

    def validate(self):
        self._check_feature_supported("mp-units", "minimum_support")
        for key, value in self._option_feature_map.items():
            if self.options.get_safe(key) is True:
                self._check_feature_supported(key, value)

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        if self._build_all:
            tc.cache_variables["CMAKE_VERIFY_INTERFACE_HEADER_SETS"] = True
            tc.cache_variables["MP_UNITS_DEV_BUILD_LA"] = not self._skip_la
        if self._build_cxx_modules:
            tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = True
            tc.cache_variables["MP_UNITS_BUILD_CXX_MODULES"] = str(
                self.options.cxx_modules
            ).upper()
        tc.cache_variables["MP_UNITS_API_STD_FORMAT"] = str(
            self.options.std_format
        ).upper()
        tc.cache_variables["MP_UNITS_API_STRING_VIEW_RET"] = str(
            self.options.string_view_ret
        ).upper()
        tc.cache_variables["MP_UNITS_API_NO_CRTP"] = str(self.options.no_crtp).upper()
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._build_all else "src")
        cmake.build()
        if self._build_all:
            cmake.build(target="all_verify_interface_header_sets")
            if can_run(self):
                cmake.ctest(cli_args=["--output-on-failure"])

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
        self.cpp_info.components["core"].requires = ["gsl-lite::gsl-lite"]
        if self.options.use_fmtlib:
            self.cpp_info.components["core"].requires.append("fmt::fmt")
        if compiler == "msvc":
            self.cpp_info.components["core"].cxxflags = ["/utf-8"]
        self.cpp_info.components["systems"].requires = ["core"]
