from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(
        # package id
        username = "mpusz",
        channel = "testing",
        stable_branch_pattern = r"v\d+\.\d+\.\d+.*",

        # dependencies
        remotes = [
            ("https://api.bintray.com/conan/bincrafters/public-conan", True, "bincrafters"),
            ("https://api.bintray.com/conan/twonington/public-conan", True, "linear-algebra")
        ],
        build_policy = ["mp-units", "outdated"],
        upload_dependencies = "all",
        pip_install = ["sphinx", "recommonmark", "breathe"],
        docker_entry_script = "sudo apt-get -qq update && sudo apt-get -qq install -y libtinfo5",

        # build configurations
        archs = ["x86_64"], #  limit to 64-bit only

        # package upload (REMEMBER to set CONAN_PASSWORD environment variable in Travis CI and AppVeyor)
        login_username = "mpusz",
        upload = "https://api.bintray.com/conan/mpusz/conan-mpusz"
    )
    builder.add_common_builds(pure_c=False)
    for settings, options, env_vars, build_requires, reference in builder.items:
        settings["compiler.cppstd"] = "20"
        env_vars["mp-units:CONAN_RUN_TESTS"] = "True"
    builder.run()
