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
from conan.tools.scm import Version

required_conan_version = ">=2.0.0"


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
        "cxx_modules": [True, False],
        "std_format": [True, False],
        "string_view_ret": [True, False],
        "no_crtp": [True, False],
        "contracts": ["none", "gsl-lite", "ms-gsl"],
        "freestanding": [True, False],
    }
    default_options = {
        # "cxx_modules" default set in config_options()
        # "std_format" default set in config_options()
        # "string_view_ret" default set in config_options()
        # "no_crtp" default set in config_options()
        "contracts": "gsl-lite",
        "freestanding": False,
    }
    implements = "auto_header_only"
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
                "min_cppstd": "20",
                "compiler": {
                    "gcc": "12",
                    "clang": "16",
                    "apple-clang": "15",
                    "msvc": "",
                },
            },
            "std_format": {
                "min_cppstd": "20",
                "compiler": {
                    "gcc": "13",
                    "clang": "17",
                    "apple-clang": "",
                    "msvc": "",
                },
            },
            "cxx_modules": {
                "min_cppstd": "20",
                "compiler": {"gcc": "", "clang": "17", "apple-clang": "", "msvc": ""},
            },
            "static_constexpr_vars_in_constexpr_func": {
                "min_cppstd": "23",
                "compiler": {"gcc": "13", "clang": "17", "apple-clang": "", "msvc": ""},
            },
            "explicit_this": {
                "min_cppstd": "23",
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

    def _set_default_option(self, name):
        compiler = self.settings.compiler
        feature_name = self._option_feature_map[name]
        feature = self._feature_compatibility[feature_name]
        min_version = feature["compiler"].get(str(compiler))
        setattr(
            self.options,
            name,
            bool(
                min_version
                and Version(compiler.version) >= min_version
                and valid_min_cppstd(self, feature["min_cppstd"])
            ),
        )

    def _check_feature_supported(self, name, feature_name=name):
        compiler = self.settings.compiler
        cppstd = compiler.get_safe("cppstd", default_cppstd(self))
        feature = self._feature_compatibility[feature_name]

        # check C++ version
        if not valid_min_cppstd(self, feature["min_cppstd"]):
            raise ConanInvalidConfiguration(
                f"'{name}' requires at least cppstd={feature['min_cppstd']} ({cppstd} in use)",
            )

        # check compiler version
        min_version = feature["compiler"].get(str(compiler))
        if min_version == None:
            # not tested compiler being used - use at your own risk
            return
        if min_version == "":
            raise ConanInvalidConfiguration(
                f"'{name}' is not yet supported by any known {compiler} version"
            )
        if Version(compiler.version) < min_version:
            raise ConanInvalidConfiguration(
                f"'{name}' requires at least {compiler}-{min_version} ({compiler}-{compiler.version} in use)"
            )

    @property
    def _build_all(self):
        return bool(self.conf.get("user.mp-units.build:all", default=False))

    @property
    def _skip_la(self):
        # broken until https://github.com/BobSteagall/wg21/issues/77 is fixed
        return bool(self.conf.get("user.mp-units.build:skip_la", default=True))

    @property
    def _run_clang_tidy(self):
        return bool(self.conf.get("user.mp-units.analyze:clang-tidy", default=False))

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "src/CMakeLists.txt"))
        version = re.search(
            r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content
        ).group(1)
        self.version = version.strip()

    def config_options(self):
        for key in self._option_feature_map.keys():
            self._set_default_option(key)

    def configure(self):
        if self.options.freestanding:
            self.options.rm_safe("std_format")

    def requirements(self):
        if not self.options.freestanding:
            if self.options.contracts == "gsl-lite":
                self.requires("gsl-lite/0.41.0")
            elif self.options.contracts == "ms-gsl":
                self.requires("ms-gsl/4.0.0")
            if not self.options.std_format:
                self.requires("fmt/11.0.1")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.29 <4]")
        if self._build_all:
            if not self.options.freestanding:
                self.test_requires("catch2/3.6.0")
            if not self._skip_la:
                self.test_requires("wg21-linear_algebra/0.7.3")

    def validate(self):
        self._check_feature_supported("mp-units", "minimum_support")
        for key, value in self._option_feature_map.items():
            if self.options.get_safe(key) == True:
                self._check_feature_supported(key, value)
        if self.options.freestanding and self.options.contracts != "none":
            raise ConanInvalidConfiguration(
                "'contracts' should be set to 'none' for a freestanding build"
            )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.absolute_paths = True  # only needed for CMake CI
        if self._build_all:
            tc.cache_variables["CMAKE_EXPORT_COMPILE_COMMANDS"] = True
            tc.cache_variables["CMAKE_VERIFY_INTERFACE_HEADER_SETS"] = True
            tc.cache_variables["MP_UNITS_DEV_BUILD_LA"] = not self._skip_la
            if self._run_clang_tidy:
                tc.cache_variables["MP_UNITS_DEV_CLANG_TIDY"] = True
        if self.options.cxx_modules:
            tc.cache_variables["CMAKE_CXX_SCAN_FOR_MODULES"] = True
            tc.cache_variables["MP_UNITS_BUILD_CXX_MODULES"] = True
        if self.options.freestanding:
            tc.cache_variables["MP_UNITS_API_FREESTANDING"] = True
        else:
            tc.cache_variables["MP_UNITS_API_STD_FORMAT"] = self.options.std_format
        tc.cache_variables[
            "MP_UNITS_API_STRING_VIEW_RET"
        ] = self.options.string_view_ret
        tc.cache_variables["MP_UNITS_API_NO_CRTP"] = self.options.no_crtp
        tc.cache_variables["MP_UNITS_API_CONTRACTS"] = str(
            self.options.contracts
        ).upper()
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder=None if self._build_all else "src")
        if self._build_all or self.options.cxx_modules:
            cmake.build()
        if self._build_all:
            cmake.build(target="all_verify_interface_header_sets")
            if can_run(self):
                cmake.ctest(cli_args=["--output-on-failure"])

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

        # handle contracts
        if self.options.contracts == "none":
            self.cpp_info.components["core"].defines.append("MP_UNITS_API_CONTRACTS=0")
        elif self.options.contracts == "gsl-lite":
            self.cpp_info.components["core"].requires.append("gsl-lite::gsl-lite")
            self.cpp_info.components["core"].defines.append("MP_UNITS_API_CONTRACTS=2")
        elif self.options.contracts == "ms-gsl":
            self.cpp_info.components["core"].requires.append("ms-gsl::ms-gsl")
            self.cpp_info.components["core"].defines.append("MP_UNITS_API_CONTRACTS=3")

        # handle API options
        self.cpp_info.components["core"].defines.append(
            "MP_UNITS_API_STRING_VIEW_RET="
            + str(int(self.options.string_view_ret == True))
        )
        self.cpp_info.components["core"].defines.append(
            "MP_UNITS_API_NO_CRTP=" + str(int(self.options.no_crtp == True))
        )
        self.cpp_info.components["core"].defines.append(
            "MP_UNITS_API_STD_FORMAT=" + str(int(self.options.std_format == True))
        )
        if not self.options.std_format:
            self.cpp_info.components["core"].requires.append("fmt::fmt")

        # handle hosted configuration
        if not self.options.freestanding:
            self.cpp_info.components["core"].defines.append("MP_UNITS_HOSTED=1")

        if compiler == "msvc":
            self.cpp_info.components["core"].cxxflags = ["/utf-8"]

        self.cpp_info.components["systems"].requires = ["core"]
