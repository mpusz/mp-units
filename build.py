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

# The following environment variables are required
# - CONAN_USERNAME
# - CONAN_UPLOAD
# - CONAN_LOGIN_USERNAME
# - CONAN_PASSWORD

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(
        # package id
        channel="testing",
        stable_branch_pattern=r"v\d+\.\d+\.\d+.*",
        # dependencies
        build_policy=["mp-units", "outdated"],
        upload_dependencies="all",
        pip_install=["sphinx", "recommonmark", "breathe"],
        # build configurations
        archs=["x86_64"],  # limit to 64-bit only
    )
    builder.add_common_builds(pure_c=True)
    for settings, options, env_vars, build_requires, reference in builder.items:
        env_vars["mp-units:CONAN_RUN_TESTS"] = "True"
    builder.run()
