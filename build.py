from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(username = "mpusz",
                                 channel = "testing",
                                 login_username = "mpusz",
                                 upload = "https://api.bintray.com/conan/mpusz/conan-mpusz",
                                 stable_branch_pattern = r"v\d+\.\d+\.\d+.*",
                                 build_policy = "outdated",
                                 upload_dependencies="all",
                                 remotes = "https://api.bintray.com/conan/martinmoene/nonstd-lite")
    builder.add_common_builds(pure_c=False)
    for settings, options, env_vars, build_requires, reference in builder.items:
        settings["cppstd"] = "20"
    builder.run()
