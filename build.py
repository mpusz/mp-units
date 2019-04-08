from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(username="mpusz", login_username="mpusz",
                                 channel="testing",
                                 stable_branch_pattern="release/*",
                                 upload="https://api.bintray.com/conan/mpusz/conan-mpusz",
                                 remotes="https://api.bintray.com/conan/martinmoene/nonstd-lite",
                                 build_policy="outdated",
                                 upload_dependencies="all")
    builder.add_common_builds(pure_c=False)
    builder.run()
